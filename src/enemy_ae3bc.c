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
extern vu16 gFrameCount;
extern vu8 gBgMosaic;
extern vu16 gBg3Cnt;
extern u32 gHBlankDmaSrc[];
extern s16 gUnk_03001F00;
extern s32 gUnk_03001F2C;
extern u8 gUnk_03001F30;
extern s16 gViewRect[];
extern u32 gUnk_03002160;
extern struct PlayerState gUnk_03002170[];
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
extern u32 gUnk_0873CB1C[];
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
extern s16 gUnk_0874B614[];
extern u8 gUnk_0874B63E[];
extern u32 gUnk_0874B6BC[];
extern u32 gUnk_0874B6D4[];
extern u32 gUnk_0874B77C[];
extern u8 gUnk_0874B824[];
extern u8 gUnk_0874B82A[];
extern u8 gUnk_0874B82E[];
extern u32 gUnk_0874B831[];
extern u32 gUnk_0874B835[];
extern u32 gUnk_0874B838[];
extern u32 gUnk_0874B83B[];
extern u32 gUnk_0874B840[];
extern u32 gUnk_0874B86C[];
extern u32 gUnk_0874B898[];
extern u8 gUnk_0874B8C4[];
extern s16 gUnk_0874B8C8[];
extern s16 gUnk_0874B8F8[];
extern u8 gUnk_0874B928[];
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
extern void sub_08063908(struct ActorDef *d);
extern void sub_0806391c(u32 i, struct ActorDef *d);
extern void sub_0806395c();
extern void sub_08063974(u32 i, u16 v);
extern void sub_080639a4(u32 v);
extern void sub_080639b4(u32 v);
extern void sub_080639c8(u32 i, u32 v);
extern void sub_080639f0(struct ActorAux *v);
extern void sub_08063a00(u32 v);
extern void sub_08063a14(u32 i, u32 v);
extern s32 sub_08063b38(void);
extern s32 sub_08063cbc(u32 i);
extern s32 sub_08063cd0(void);
extern s32 sub_08063d18(u32 i);
extern s32 sub_08063d2c(void);
extern void sub_08063d4c(u32 i);
extern void sub_08063d7c(void);
extern s32 sub_08063dac(u32 i);
extern void sub_08063e14(void);
extern s32 sub_08063eb0(struct Rect *r, u32 i);
extern void sub_08063f24(s32 i);
extern void sub_08063fe0(void);
extern void sub_08063ff4(void);
extern void sub_0806406c(void);
extern s32 sub_080640a8(struct AnimCmd *p);
extern void sub_080640c8(void);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_0806415c(s32 n);
extern s32 sub_08064188(s32 n);
extern void sub_0806421c(s16 t, s16 mag);
extern u16 sub_080642fc(u32 i, s32 prec);
extern u16 sub_08064314(s32 prec);
extern u8 sub_0806433c(u32 i);
extern u8 sub_08064358(void);
extern u8 sub_0806437c(u32 i);
extern u8 sub_08064398(void);
extern void sub_080645a4(s32 step, s32 limit);
extern void sub_08064680(s32 step, s32 limit, u16 dir);
extern s32 sub_08064758(u16 lo, u16 hi);
extern s32 sub_080647fc(u16 lo, u16 hi);
extern void sub_080648a0(u32 i);
extern s32 sub_0806493c(void);
extern void sub_08064970(void);
extern void sub_08064a60(void);
extern s32 sub_08064b28(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064ba8(struct ActorSpawn *p, u8 keepPrio);
extern void sub_08064bcc(void);
extern s32 sub_08064c1c(u32 type, int xArg, int yArg, int prioArg);
extern s32 sub_08064cdc(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern s32 sub_08064d6c(u32 type, s16 xArg, s16 yArg, u8 keepPrio);
extern s32 sub_08064fc4(u8 cls, u32 sub, u8 p3, u8 p4, int x, int y, u16 prio);
extern s32 sub_0806505c(u8 p3, u8 p4, u32 x, u32 y, u16 prio);
extern u8 sub_08065160(void);
extern void sub_080651b4(void);
extern void sub_0806523c(void);
extern void sub_080653ec(void);
extern void sub_08065438(void);
extern void sub_08065640(void);
extern void sub_080656b4(void);
extern void sub_0806572c(void);
extern void sub_080657a4(void);
extern void sub_080657cc(u32 v);
extern void sub_080657f8(u32 a, u32 b);
extern void sub_08065dbc(u32 slot, u32 sub);
extern void sub_08065dd0(u32 slot, u32 i);
extern void sub_08065dfc(u32 slot);
extern u8 sub_08065f2c(u32 i);
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
extern void sub_0806685c(void *src, u32 size, u8 force);
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
extern u32 sub_08068cf8(s32 a);
extern u32 sub_08068e04(void);
extern u32 sub_08068f68(void);
extern u32 sub_080692fc(void);
extern u32 sub_0806951c(void);
extern u32 sub_0806956c(void);
extern u32 sub_080695bc(void);
extern u32 sub_080696a0(void);
extern u32 sub_08069888(void);
extern u32 sub_08069b44(void);
extern u32 sub_08069b84(void);
extern u32 sub_08069bbc(void);
extern u32 sub_0806a25c(void);
extern void sub_0806a344(void);
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

void sub_080ae3bc(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *u2;
    u8 *b42;

    c = &gCurTask;
    t = *c;
    t->unk00 = (u32)sub_0806572c;
    t->unk0C = (u32)TaskDrawWorld;
    t->unk38 = gUnk_087544B4;
    b42 = &t->unk42;
    z = 0;
    *b42 = 11;
    sub_08066088(0);
    sub_080b05e8();
    u = *c;
    u->unk04 = (u32)sub_080ae470;
    sub_080ae4c4();
    sub_08066580();
    v = *c;
    v->unk4C = 192 << 16;
    v->unk50 = 152 << 15;
    v->unk6C = z;
    do
    {
        sub_080ae548();
        sub_080ae548();
        sub_080ae628();
        sub_080ae628();
        sub_080ae79c();
        sub_080ae628();
        sub_080ae628();
        sub_080ae7ec();
        sub_080ae548();
        sub_080ae548();
        sub_080ae79c();
        sub_080ae628();
        sub_080ae628();
        sub_080ae8e0();
        u2 = gCurTask;
        u2->unk6C++;
    } while ((s16)u2->unk6C <= 1);
    sub_080aeef8();
    sub_080aef30();
}

void sub_080ae470(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    sub_080aef5c();
    c = &gCurTask;
    CallTableEntry((*c)->unk15, 4, gUnk_0874AD34);
    t = *c;
    t->unk75 = 0;
    u = *c;
    if ((s16)u->unk78 != 0)
    {
        sub_08068f68();
        sub_08069b44();
        sub_080af1d4();
        if (gUnk_02007D00[3] > 0)
            gUnk_02007D00[3]--;
    }
}

void sub_080ae4c4(void)
{
    u16 *q;
    struct Task **c;
    struct Task *t;
    struct Task *u;
    struct Task *t2;
    s32 *p;
    s16 w;

    q = (u16 *)gUnk_082FEFF4;
    RequestCopy(4, ((u32 *)q)[3], 0x06012000, q[1] << 5);
    RequestCopy(2, ((u32 *)q)[2], 0x030015B0, q[0] << 5);
    sub_08063a00((u32)gUnk_0874B450);
    c = &gCurTask;
    t = *c;
    t->unk8C->unk2C = 0;
    t->unk28 = 0;
    t->unk43 = 1;
    u = *c;
    u->unk2C = 0;
    p = gUnk_02007D00;
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
    p[4] = 0;
    p[6] = 0;
    p[7] = 0;
    p[8] = 176 << 15;
    p[9] = 0;
    w = sub_08065f5c();
    t2 = *c;
    t2->unk24 = w;
}

void sub_080ae548(void)
{
    struct Task **c;
    s32 z;
    s32 v1;
    s32 v2;
    s32 v3;
    s32 v4;
    s32 v5;
    s32 v6;
    struct Task *t;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;

    sub_0806395c(0);
    c = &gCurTask;
    t = *c;
    z = 0;
    t->unk2C = z;
    sub_080aefd4((u32)gUnk_0874AD74);
    (*c)->unk15 = z;
    u1 = *c;
    v1 = 0xFFFF0000;
    u1->unk58 = v1;
    TaskYieldTrampoline(10);
    u2 = *c;
    v2 = 0xFFFF8000;
    u2->unk58 = v2;
    TaskYieldTrampoline(10);
    u3 = *c;
    v3 = 0xFFFFE000;
    u3->unk58 = v3;
    TaskYieldTrampoline(10);
    u4 = *c;
    v4 = 128 << 6;
    u4->unk58 = v4;
    TaskYieldTrampoline(10);
    u5 = *c;
    v5 = 128 << 8;
    u5->unk58 = v5;
    TaskYieldTrampoline(10);
    u6 = *c;
    v6 = 128 << 9;
    u6->unk58 = v6;
    TaskYieldTrampoline(10);
    (*c)->unk58 = v6;
    TaskYieldTrampoline(10);
    (*c)->unk58 = v5;
    TaskYieldTrampoline(10);
    (*c)->unk58 = v4;
    TaskYieldTrampoline(10);
    (*c)->unk58 = v3;
    TaskYieldTrampoline(10);
    (*c)->unk58 = v2;
    TaskYieldTrampoline(10);
    (*c)->unk58 = v1;
    TaskYieldTrampoline(10);
    TaskStop();
}

void sub_080ae628(void)
{
    struct Task **c;
    s32 z;
    s32 va;
    s32 vb;
    s32 vc;
    s32 vd;
    s32 ve;
    struct Task *t;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;
    struct Task *u8;
    struct Task *u9;
    struct Task *u10;
    struct Task *u11;
    struct Task *u12;
    struct Task *u13;
    struct Task *u14;
    struct Task *u15;
    struct Task *u16;
    struct Task *u17;
    struct Task *u18;
    struct Task *u19;
    struct Task *u20;

    sub_0806395c(1);
    sub_080aefd4((u32)gUnk_0874ADA8);
    c = &gCurTask;
    t = *c;
    z = 0;
    t->unk15 = 1;
    u1 = *c;
    va = 0xFFFE0000;
    u1->unk54 = va;
    u1->unk58 = va;
    TaskYieldTrampoline(6);
    u2 = *c;
    vb = 0xFFFF0000;
    u2->unk54 = vb;
    u2->unk58 = va;
    TaskYieldTrampoline(6);
    u3 = *c;
    u3->unk54 = z;
    u3->unk58 = 0xFFFEC000;
    TaskYieldTrampoline(6);
    u4 = *c;
    vc = 128 << 9;
    u4->unk54 = vc;
    u4->unk58 = vb;
    TaskYieldTrampoline(6);
    u5 = *c;
    vd = 128 << 10;
    u5->unk54 = vd;
    u5->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(6);
    u6 = *c;
    u6->unk54 = vd;
    u6->unk58 = 128 << 8;
    TaskYieldTrampoline(6);
    u7 = *c;
    u7->unk54 = vc;
    u7->unk58 = vc;
    TaskYieldTrampoline(6);
    u8 = *c;
    u8->unk54 = z;
    ve = 160 << 9;
    u8->unk58 = ve;
    TaskYieldTrampoline(6);
    u9 = *c;
    u9->unk54 = vb;
    u9->unk58 = vd;
    TaskYieldTrampoline(6);
    u10 = *c;
    u10->unk54 = va;
    u10->unk58 = vd;
    TaskYieldTrampoline(6);
    u11 = *c;
    u11->unk54 = va;
    u11->unk58 = vd;
    TaskYieldTrampoline(6);
    u12 = *c;
    u12->unk54 = vb;
    u12->unk58 = vd;
    TaskYieldTrampoline(6);
    u13 = *c;
    u13->unk54 = z;
    u13->unk58 = ve;
    TaskYieldTrampoline(6);
    u14 = *c;
    u14->unk54 = vc;
    u14->unk58 = vc;
    TaskYieldTrampoline(6);
    u15 = *c;
    u15->unk54 = vd;
    u15->unk58 = 128 << 8;
    TaskYieldTrampoline(6);
    u16 = *c;
    u16->unk54 = vd;
    u16->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(6);
    u17 = *c;
    u17->unk54 = vc;
    u17->unk58 = vb;
    TaskYieldTrampoline(6);
    u18 = *c;
    u18->unk54 = z;
    u18->unk58 = 0xFFFEC000;
    TaskYieldTrampoline(6);
    u19 = *c;
    u19->unk54 = vb;
    u19->unk58 = va;
    TaskYieldTrampoline(6);
    u20 = *c;
    u20->unk54 = va;
    u20->unk58 = va;
    TaskYieldTrampoline(6);
    TaskStop();
}

void sub_080ae79c(void)
{
    sub_0806395c(2);
    sub_080aefd4((u32)gUnk_0874AEAC);
    gCurTask->unk15 = 3;
    TaskYieldTrampoline(40);
    sub_080af20c(0);
    sub_080af20c(1);
    sub_080af20c(2);
    sub_080af20c(3);
    gUnk_02007D00[3] = 140;
    TaskYieldTrampoline(80);
}

void sub_080ae7ec(void)
{
    struct Task **c;
    s32 v1;
    s32 v2;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;

    sub_0806395c(3);
    sub_080aefd4((u32)gUnk_0874ADA8);
    c = &gCurTask;
    (*c)->unk15 = 3;
    TaskStop();
    TaskYieldTrampoline(8);
    u1 = *c;
    v1 = 0xFFFA0000;
    u1->unk58 = v1;
    TaskYieldTrampoline(10);
    sub_080ae870();
    u2 = *c;
    v2 = 192 << 11;
    u2->unk58 = v2;
    TaskYieldTrampoline(16);
    sub_080ae870();
    u3 = *c;
    u3->unk58 = v1;
    TaskYieldTrampoline(12);
    sub_080ae870();
    u4 = *c;
    u4->unk58 = v2;
    TaskYieldTrampoline(16);
    sub_080ae870();
    u5 = *c;
    u5->unk58 = v1;
    TaskYieldTrampoline(10);
    sub_080ae870();
    TaskStop();
}

void sub_080ae870(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;

    TaskStop();
    TaskYieldTrampoline(8);
    PlaySfx(0x22D);
    sub_080af20c(11);
    c = &gCurTask;
    u1 = *c;
    u1->unk54 = 128 << 12;
    TaskYieldTrampoline(2);
    u2 = *c;
    u2->unk54 = 128 << 11;
    TaskYieldTrampoline(2);
    u3 = *c;
    u3->unk54 = 0xFFFC0000;
    TaskYieldTrampoline(2);
    u4 = *c;
    u4->unk54 = 0xFFF80000;
    TaskYieldTrampoline(2);
    TaskStop();
    TaskYieldTrampoline(8);
}

void sub_080ae8e0(void)
{
    sub_0806395c(4);
    sub_080aefd4((u32)gUnk_0874AF90);
    gCurTask->unk15 = 3;
    switch ((u8)sub_080aeeb0())
    {
    case 0:
        gCurTask->unk54 = 0xFFFD0000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 192 << 10;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 128 << 10;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 128 << 9;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 128 << 8;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 128 << 6;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0;
        TaskYieldTrampoline(4);
        PlaySfx(144 << 2);
        gCurTask->unk54 = 0xFFFA8000;
        TaskYieldTrampoline(32);
        gCurTask->unk54 = 0xFFFE0000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFF0000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFF8000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0;
        TaskYieldTrampoline(32);
        sub_080aec00();
        break;
    case 1:
        gCurTask->unk54 = 128 << 10;
        gCurTask->unk58 = 128 << 10;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 128 << 9;
        gCurTask->unk58 = 128 << 9;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 128 << 8;
        gCurTask->unk58 = 128 << 8;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 128 << 6;
        gCurTask->unk58 = 128 << 6;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0;
        gCurTask->unk58 = 0;
        TaskYieldTrampoline(16);
        PlaySfx(144 << 2);
        gCurTask->unk54 = 0xFFFF8000;
        gCurTask->unk58 = 0xFFFC0000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFF0000;
        gCurTask->unk58 = 0xFFFD0000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFE0000;
        gCurTask->unk58 = 0xFFFE0000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFD0000;
        gCurTask->unk58 = 0xFFFF0000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFC0000;
        gCurTask->unk58 = 0xFFFF8000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFA0000;
        gCurTask->unk58 = 0xFFFFE000;
        TaskYieldTrampoline(12);
        gCurTask->unk54 = 0xFFFD0000;
        gCurTask->unk58 = 128 << 9;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFE0000;
        gCurTask->unk58 = 128 << 10;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFF0000;
        gCurTask->unk58 = 128 << 11;
        TaskYieldTrampoline(8);
        sub_080aec00();
        break;
    case 2:
        gCurTask->unk54 = 128 << 10;
        gCurTask->unk58 = 0xFFFE0000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 128 << 9;
        gCurTask->unk58 = 0xFFFF0000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 128 << 8;
        gCurTask->unk58 = 0xFFFF8000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 128 << 6;
        gCurTask->unk58 = 0xFFFFE000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0;
        gCurTask->unk58 = 0;
        TaskYieldTrampoline(16);
        PlaySfx(144 << 2);
        gCurTask->unk54 = 0xFFFF8000;
        gCurTask->unk58 = 128 << 11;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFF0000;
        gCurTask->unk58 = 192 << 10;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFE0000;
        gCurTask->unk58 = 128 << 10;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFD0000;
        gCurTask->unk58 = 128 << 9;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFC0000;
        gCurTask->unk58 = 128 << 8;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFA0000;
        gCurTask->unk58 = 128 << 6;
        TaskYieldTrampoline(12);
        gCurTask->unk54 = 0xFFFD0000;
        gCurTask->unk58 = 0xFFFF0000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFE0000;
        gCurTask->unk58 = 0xFFFE0000;
        TaskYieldTrampoline(8);
        gCurTask->unk54 = 0xFFFF0000;
        gCurTask->unk58 = 0xFFFC0000;
        TaskYieldTrampoline(8);
        sub_080aed5c();
        break;
    }
    TaskStop();
}

void sub_080aec00(void)
{
    sub_080aefd4((u32)gUnk_0874B0A0);
    gCurTask->unk15 = 3;
    gCurTask->unk54 = 128 << 10;
    gCurTask->unk58 = 128 << 10;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 128 << 10;
    gCurTask->unk58 = 128 << 9;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 128 << 10;
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 128 << 10;
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 128 << 10;
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 128 << 10;
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 128 << 10;
    gCurTask->unk58 = 128 << 9;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 128 << 10;
    gCurTask->unk58 = 128 << 10;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 128 << 10;
    gCurTask->unk58 = 128 << 10;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 128 << 10;
    gCurTask->unk58 = 128 << 9;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 128 << 11;
    gCurTask->unk58 = 0;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 128 << 10;
    gCurTask->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 128 << 9;
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 128 << 8;
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 0xFFFF0000;
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 0xFFFE0000;
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 0xFFFF0000;
    gCurTask->unk58 = 128 << 8;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 0xFFFF0000;
    gCurTask->unk58 = 128 << 9;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 0;
    gCurTask->unk58 = 128 << 9;
    TaskYieldTrampoline(6);
    TaskStop();
    TaskYieldTrampoline(32);
}

void sub_080aed5c(void)
{
    struct Task **c;
    s32 z;
    s32 va;
    s32 vb;
    s32 vc;
    s32 vd;
    s32 ve;
    struct Task *t;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *q4;
    struct Task *q5;
    struct Task *q6;
    struct Task *q7;
    struct Task *q8;
    struct Task *q9;
    struct Task *q10;
    struct Task *q11;
    struct Task *q12;
    struct Task *q13;
    struct Task *q14;
    struct Task *q15;
    struct Task *q16;
    struct Task *q17;
    struct Task *q18;
    struct Task *q19;

    sub_080aefd4((u32)gUnk_0874B0A0);
    c = &gCurTask;
    t = *c;
    z = 0;
    t->unk15 = 3;
    q1 = *c;
    va = 128 << 10;
    q1->unk54 = va;
    vd = 0xFFFE0000;
    q1->unk58 = vd;
    TaskYieldTrampoline(8);
    q2 = *c;
    q2->unk54 = va;
    vc = 0xFFFF0000;
    q2->unk58 = vc;
    TaskYieldTrampoline(8);
    q3 = *c;
    q3->unk54 = va;
    vb = 128 << 9;
    q3->unk58 = vb;
    TaskYieldTrampoline(8);
    q4 = *c;
    q4->unk54 = va;
    q4->unk58 = va;
    TaskYieldTrampoline(8);
    q5 = *c;
    q5->unk54 = va;
    q5->unk58 = va;
    TaskYieldTrampoline(8);
    q6 = *c;
    q6->unk54 = va;
    q6->unk58 = vb;
    TaskYieldTrampoline(8);
    q7 = *c;
    q7->unk54 = va;
    q7->unk58 = vc;
    TaskYieldTrampoline(8);
    q8 = *c;
    q8->unk54 = va;
    q8->unk58 = vd;
    TaskYieldTrampoline(8);
    q9 = *c;
    q9->unk54 = va;
    q9->unk58 = vd;
    TaskYieldTrampoline(8);
    q10 = *c;
    q10->unk54 = va;
    q10->unk58 = vc;
    TaskYieldTrampoline(8);
    q11 = *c;
    q11->unk54 = 128 << 11;
    q11->unk58 = z;
    TaskYieldTrampoline(6);
    q12 = *c;
    q12->unk54 = va;
    ve = 128 << 8;
    q12->unk58 = ve;
    TaskYieldTrampoline(6);
    q13 = *c;
    q13->unk54 = vb;
    q13->unk58 = vb;
    TaskYieldTrampoline(6);
    q14 = *c;
    q14->unk54 = ve;
    q14->unk58 = va;
    TaskYieldTrampoline(6);
    q15 = *c;
    q15->unk54 = vc;
    q15->unk58 = va;
    TaskYieldTrampoline(6);
    q16 = *c;
    q16->unk54 = vd;
    q16->unk58 = vb;
    TaskYieldTrampoline(6);
    q17 = *c;
    q17->unk54 = vc;
    q17->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(6);
    q18 = *c;
    q18->unk54 = vc;
    q18->unk58 = vc;
    TaskYieldTrampoline(6);
    q19 = *c;
    q19->unk54 = z;
    q19->unk58 = vc;
    TaskYieldTrampoline(6);
    TaskStop();
    TaskYieldTrampoline(32);
}

s32 sub_080aeeb0(void)
{
    struct Task *tt;
    struct Task *tt2;
    u32 o;
    u32 o2;
    s32 i;
    s32 i2;

    i = sub_08063b38();
    o = i * 144;
    tt = (struct Task *)((u8 *)gTasks + o);
    if (tt->unk4A <= 63)
        return 1;
    i2 = sub_08063b38();
    o2 = i2 * 144;
    tt2 = (struct Task *)((u8 *)gTasks + o2);
    if (tt2->unk4A > 128)
        return 2;
    return 0;
}

void sub_080aeef8(void)
{
    struct Task **c;
    struct Task *u;

    sub_0806395c(5);
    sub_080aefd4((u32)gUnk_0874B0C4);
    c = &gCurTask;
    (*c)->unk15 = 3;
    u = *c;
    u->unk54 = 128 << 9;
    u->unk60 = 0xFFFFE000;
    TaskYieldTrampoline(60);
}

void sub_080aef30(void)
{
    gCurTask->unk04 = (u32)sub_080aef50;
    TaskStop();
    TaskSleepForever();
}

void sub_080aef50(void)
{
    sub_080aef5c();
}

void sub_080aef5c(void)
{
    s32 *p;

    p = gUnk_02007D00;
    if (p[9] == 0)
    {
        p[8] += 204 << 6;
        gUnk_03001F00 = p[8] >> 16;
    }
    if (p[5] > 23)
        p[5] = 0;
    BlendColors((u16 *)gUnk_082FE0E4, (u16 *)gUnk_082FE104, gUnk_0874AD44[p[5]], 16, (u16 *)(((gCurTask->unk40 >> 12) << 5) + (u32)gObjPalette));
    p[5]++;
}

void sub_080aefd4(u32 a)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk8C->unk2C = (struct AnimCmd *)a;
    t->unk8C->unk09 = 0;
    u = gCurTask;
    u->unk28 = 0;
    sub_080af020();
}

void sub_080aeff8(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk8C->unk2C != 0)
    {
        t->unk28--;
        if (t->unk28 <= 0)
            sub_080af020();
    }
}

void sub_080af020(void)
{
    struct Task *t;
    struct Actor *a;
    struct Actor *a2;
    struct Task *t3;
    struct Actor *a3;
    struct Task *t4;
    struct Actor *a4;
    struct Task *t5;
    struct Actor *a5;
    struct Task *t6;
    struct Actor *a6;
    struct Task *u;
    s32 i;
    s32 v;
    s32 r;

top:
    t = gCurTask;
    a = t->unk8C;
    i = a->unk09;
    v = ((s32 *)a->unk2C)[i];
    switch (v)
    {
    case -1:
        break;
    case -2:
        a->unk09 = 0;
        goto top;
    case -3:
        a->unk09 = i + 1;
        a2 = gCurTask->unk8C;
        ((void (*)(void))((s32 *)a2->unk2C)[a2->unk09++])();
        goto top;
    case -4:
        a->unk09 = i + 1;
        t3 = gCurTask;
        a3 = t3->unk8C;
        t3->unk3C = ((s32 *)a3->unk2C)[a3->unk09++];
        t4 = gCurTask;
        a4 = t4->unk8C;
        r = ((s32 (*)(void))((s32 *)a4->unk2C)[a4->unk09++])();
        u = gCurTask;
        u->unk28 = r;
        break;
    default:
        t5 = gCurTask;
        a5 = t5->unk8C;
        t5->unk3C = ((s32 *)a5->unk2C)[a5->unk09++];
        t6 = gCurTask;
        a6 = t6->unk8C;
        t6->unk28 = ((s32 *)a6->unk2C)[a6->unk09++];
        break;
    }
}

s32 sub_080af100(void)
{
    return ((s16)(u16)gCurTask->unk78 >> 3) + 1;
}

void sub_080af114(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    sub_080aeff8();
    c = &gCurTask;
    t = *c;
    t->unk2C++;
    if (t->unk2C > 39)
    {
        sub_080aefd4((u32)gUnk_0874ADA8);
        u = *c;
        u->unk15 = 1;
    }
}

void sub_080af144(void)
{
    gUnk_02007D00[1] = RandomRange(4) + 1;
    gCurTask->unk15 = 2;
    if (gUnk_02007D00[3] == 0)
    {
        gUnk_02007D00[3] = 140;
        sub_080af20c(4);
    }
}

void sub_080af178(void)
{
    gUnk_02007D00[1]--;
}

void sub_080af188(void)
{
    struct Task *t;

    sub_080aeff8();
    if (gUnk_02007D00[1] <= 0)
    {
        sub_080aefd4((u32)gUnk_0874AD74);
        t = gCurTask;
        t->unk2C = 0;
        t->unk15 = 0;
    }
}

void sub_080af1b8(void)
{
    sub_080aefd4(0x0874B07C);
}

void sub_080af1c8(void)
{
    sub_080aeff8();
}

void sub_080af1d4(void)
{
    s32 *p;
    s32 v;

    p = gUnk_02007D00;
    v = p[2];
    if (v != 0)
    {
        gCurTask->unk3C = gUnk_0874B1A8[v];
        p[2]++;
        if (p[2] > 24)
            p[2] = 0;
    }
}

void sub_080af20c(u8 a)
{
    struct ActorSpawn sp;

    if (gUnk_02007D00[0] == 0)
    {
        sp.unk00 = 32;
        sp.unk04 = 135;
        sp.unk08 = a;
        sp.unk09 = gCurTask->unk74;
        sp.unk10 = 0xA110;
        sp.unk0C = gCurTask->unk4C >> 16;
        sp.unk0E = gCurTask->unk50 >> 16;
        sp.unk0A = 0;
        gCurTask->unk46 = sub_08064ba8(&sp, 1);
    }
}

void sub_080af26c(void)
{
    gUnk_02007D00[2] = 1;
}

void sub_080af278(void)
{
    StopBgm();
    TaskSetEntry(sub_080af294, gCurTaskIdx);
}

void sub_080af294(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    TaskStop();
    c = &gCurTask;
    t = *c;
    t->unk8C->unk2C = 0;
    t->unk15 = 3;
    gUnk_02007D00[0] = 1;
    sub_080639b4(0);
    (*c)->unk8C->unk38 = 0x23E;
    sub_0800a554();
    sub_0806b05c();
    sub_0806b098();
    u = *c;
    sub_08066f50(u->unk48, u->unk4A);
    sub_080aeef8();
    (*c)->unk04 = (u32)sub_080aef50;
    sub_080b09ac();
}

void sub_080af308(void)
{
}

void sub_080af30c(void)
{
    struct Task **c;
    s32 z;
    u8 *b42;
    struct Task *t;
    struct Task *u;
    struct Task *v;

    c = &gCurTask;
    t = *c;
    t->unk00 = (u32)sub_0806572c;
    t->unk0C = (u32)sub_0806523c;
    b42 = &t->unk42;
    z = 0;
    *b42 = 9;
    u = *c;
    u->unk38 = gUnk_08754504;
    u->unk43 = 1;
    v = *c;
    v->unk2C = z;
    CallTableEntry(v->unk73, 12, gUnk_0874B1DC);
}

void sub_080af358(void)
{
    struct Task **c;
    struct Task *u;
    s32 r;

    if (gUnk_02007D00[0] != 0)
        sub_08063fe0();
    c = &gCurTask;
    r = sub_08064188((*c)->unk28);
    u = *c;
    u->unk28 = r;
    sub_08068e04();
    sub_08069b44();
}

void sub_080af38c(void)
{
    struct Task **c;
    s32 va;
    s32 r;
    s32 r2;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *t;
    struct Task *t2;
    struct Task *t4;
    struct Task *t6;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->unk04 = (u32)sub_080af358;
    sub_080639b4((u32)gUnk_0874B488);
    r = sub_080640a8((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    va = 128 << 8;
    u1->unk54 = va;
    u1->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(14);
    u2 = *c;
    u2->unk54 = va;
    u2->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(14);
    u3 = *c;
    u3->unk54 = va;
    u3->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(14);
    u4 = *c;
    u4->unk54 = va;
    u4->unk58 = 0xFFFFE000;
    TaskYieldTrampoline(14);
    TaskStop();
    TaskYieldTrampoline(32);
    r2 = sub_080640a8((struct AnimCmd *)gUnk_0874B220);
    u5 = *c;
    u5->unk28 = r2;
    PlaySfx(139 << 2);
    u6 = *c;
    u6->unk54 = 0xFFFA0000;
    sub_080af7d4();
top:
    t = *c;
    w = sub_08064d6c(213, t->unk4C >> 16, t->unk50 >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = sub_08064d6c(211, t2->unk4C >> 16, t2->unk50 >> 16, 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(4);
    t4 = *c;
    w3 = sub_08064d6c(212, t4->unk4C >> 16, t4->unk50 >> 16, 1);
    (*c)->unk46 = w3;
    TaskYieldTrampoline(2);
    t6 = *c;
    w4 = sub_08064d6c(210, t6->unk4C >> 16, t6->unk50 >> 16, 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(2);
    goto top;
}

void sub_080af4a4(void)
{
    struct Task **c;
    s32 va;
    s32 r;
    s32 r2;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *t;
    struct Task *t2;
    struct Task *t4;
    struct Task *t6;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->unk04 = (u32)sub_080af358;
    sub_080639b4((u32)gUnk_0874B488);
    r = sub_080640a8((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    va = 128 << 6;
    u1->unk54 = va;
    u1->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(14);
    u2 = *c;
    u2->unk54 = va;
    u2->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(14);
    u3 = *c;
    u3->unk54 = va;
    u3->unk58 = 0xFFFFE000;
    TaskYieldTrampoline(14);
    u4 = *c;
    u4->unk54 = va;
    u4->unk58 = 0xFFFFF800;
    TaskYieldTrampoline(14);
    TaskStop();
    TaskYieldTrampoline(32);
    r2 = sub_080640a8((struct AnimCmd *)gUnk_0874B220);
    u5 = *c;
    u5->unk28 = r2;
    PlaySfx(139 << 2);
    u6 = *c;
    u6->unk54 = 0xFFFA0000;
    sub_080af7d4();
top:
    t = *c;
    w = sub_08064d6c(213, t->unk4C >> 16, t->unk50 >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = sub_08064d6c(211, t2->unk4C >> 16, t2->unk50 >> 16, 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(4);
    t4 = *c;
    w3 = sub_08064d6c(212, t4->unk4C >> 16, t4->unk50 >> 16, 1);
    (*c)->unk46 = w3;
    TaskYieldTrampoline(2);
    t6 = *c;
    w4 = sub_08064d6c(210, t6->unk4C >> 16, t6->unk50 >> 16, 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(2);
    goto top;
}

void sub_080af5bc(void)
{
    struct Task **c;
    s32 va;
    s32 r;
    s32 r2;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *t;
    struct Task *t2;
    struct Task *t4;
    struct Task *t6;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->unk04 = (u32)sub_080af358;
    sub_080639b4((u32)gUnk_0874B488);
    r = sub_080640a8((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    va = 128 << 6;
    u1->unk54 = va;
    u1->unk58 = 128 << 9;
    TaskYieldTrampoline(14);
    u2 = *c;
    u2->unk54 = va;
    u2->unk58 = 128 << 8;
    TaskYieldTrampoline(14);
    u3 = *c;
    u3->unk54 = va;
    u3->unk58 = va;
    TaskYieldTrampoline(14);
    u4 = *c;
    u4->unk54 = va;
    u4->unk58 = 128 << 4;
    TaskYieldTrampoline(14);
    TaskStop();
    TaskYieldTrampoline(32);
    r2 = sub_080640a8((struct AnimCmd *)gUnk_0874B220);
    u5 = *c;
    u5->unk28 = r2;
    PlaySfx(139 << 2);
    u6 = *c;
    u6->unk54 = 0xFFFA0000;
    sub_080af7d4();
top:
    t = *c;
    w = sub_08064d6c(213, t->unk4C >> 16, t->unk50 >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = sub_08064d6c(211, t2->unk4C >> 16, t2->unk50 >> 16, 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(4);
    t4 = *c;
    w3 = sub_08064d6c(212, t4->unk4C >> 16, t4->unk50 >> 16, 1);
    (*c)->unk46 = w3;
    TaskYieldTrampoline(2);
    t6 = *c;
    w4 = sub_08064d6c(210, t6->unk4C >> 16, t6->unk50 >> 16, 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(2);
    goto top;
}

void sub_080af6c8(void)
{
    struct Task **c;
    s32 va;
    s32 r;
    s32 r2;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *t;
    struct Task *t2;
    struct Task *t4;
    struct Task *t6;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->unk04 = (u32)sub_080af358;
    sub_080639b4((u32)gUnk_0874B488);
    r = sub_080640a8((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    va = 128 << 8;
    u1->unk54 = va;
    u1->unk58 = 128 << 10;
    TaskYieldTrampoline(14);
    u2 = *c;
    u2->unk54 = va;
    u2->unk58 = 128 << 9;
    TaskYieldTrampoline(14);
    u3 = *c;
    u3->unk54 = va;
    u3->unk58 = va;
    TaskYieldTrampoline(14);
    u4 = *c;
    u4->unk54 = va;
    u4->unk58 = 128 << 6;
    TaskYieldTrampoline(14);
    TaskStop();
    TaskYieldTrampoline(32);
    r2 = sub_080640a8((struct AnimCmd *)gUnk_0874B220);
    u5 = *c;
    u5->unk28 = r2;
    PlaySfx(139 << 2);
    u6 = *c;
    u6->unk54 = 0xFFFA0000;
    sub_080af7d4();
top:
    t = *c;
    w = sub_08064d6c(213, t->unk4C >> 16, t->unk50 >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = sub_08064d6c(211, t2->unk4C >> 16, t2->unk50 >> 16, 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(4);
    t4 = *c;
    w3 = sub_08064d6c(212, t4->unk4C >> 16, t4->unk50 >> 16, 1);
    (*c)->unk46 = w3;
    TaskYieldTrampoline(2);
    t6 = *c;
    w4 = sub_08064d6c(210, t6->unk4C >> 16, t6->unk50 >> 16, 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(2);
    goto top;
}

void sub_080af7d4(void)
{
    struct Task *t;
    struct Task *e;
    struct Task *e2;
    struct Task *t3;
    s32 i;
    s32 j;
    u32 o;
    u32 o2;
    s32 w;

    t = gCurTask;
    i = t->unk44;
    o = i * 144;
    e = (struct Task *)((u8 *)gTasks + o);
    if ((s16)e->unk78 < e->unk24 >> 1)
    {
        j = sub_08063b38();
        o2 = j * 144;
        e2 = (struct Task *)((u8 *)gTasks + o2);
        w = e2->unk4A;
        t3 = gCurTask;
        if (w > t3->unk4A - gViewRect[2])
            t3->unk58 = 128 << 10;
        else
            t3->unk58 = 0xFFFE0000;
    }
}

void sub_080af844(void)
{
    struct Task *t;
    struct Task *u;

    switch (gUnk_02007D00[4])
    {
    case 0:
        PlaySfx(0x22A);
        sub_080af20c(5);
        TaskYieldTrampoline(16);
        t = gCurTask;
        t->unk4C = ((struct Task *)(t->unk44 * 144 + (u32)gTasks))->unk4C;
        t->unk50 = ((struct Task *)(t->unk44 * 144 + (u32)gTasks))->unk50;
        PlaySfx(0x22A);
        sub_080af20c(5);
        TaskYieldTrampoline(16);
        u = gCurTask;
        u->unk4C = ((struct Task *)(u->unk44 * 144 + (u32)gTasks))->unk4C;
        u->unk50 = ((struct Task *)(u->unk44 * 144 + (u32)gTasks))->unk50;
        PlaySfx(0x22A);
        sub_080af20c(5);
        gUnk_02007D00[4] = 1;
        break;
    case 1:
        PlaySfx(0x22B);
        sub_080af20c(6);
        sub_080af20c(7);
        sub_080af20c(8);
        gUnk_02007D00[4] = 2;
        break;
    case 2:
        PlaySfx(0x22B);
        sub_080af20c(9);
        sub_080af20c(10);
        gUnk_02007D00[4] = 0;
        break;
    }
    TaskExitTrampoline();
}

void sub_080af938(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *t;
    struct Task *t2;
    struct Task *t3;
    struct Task *t4;
    s32 r;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->unk04 = (u32)sub_080af358;
    sub_080639b4((u32)gUnk_0874B488);
    r = sub_080640a8((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    u1->unk54 = 0xFFFC0000;
top:
    t = *c;
    w = sub_08064d6c(213, t->unk4C >> 16, t->unk50 >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = sub_08064d6c(210, t2->unk4C >> 16, (s16)((t2->unk50 >> 16) + 2), 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(8);
    t3 = *c;
    w3 = sub_08064d6c(213, t3->unk4C >> 16, t3->unk50 >> 16, 1);
    t4 = *c;
    t4->unk46 = w3;
    w4 = sub_08064d6c(210, t4->unk4C >> 16, (s16)((t4->unk50 >> 16) - 2), 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(8);
    goto top;
}

void sub_080af9e4(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *t;
    struct Task *t2;
    struct Task *t3;
    struct Task *t4;
    s32 r;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->unk04 = (u32)sub_080af358;
    sub_080639b4((u32)gUnk_0874B488);
    r = sub_080640a8((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    u1->unk54 = 0xFFFDCCCD;
    u1->unk58 = 0xFFFE4CCD;
top:
    t = *c;
    w = sub_08064d6c(213, t->unk4C >> 16, t->unk50 >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = sub_08064d6c(210, (s16)((t2->unk4C >> 16) + 1), (s16)((t2->unk50 >> 16) - 1), 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(8);
    t3 = *c;
    w3 = sub_08064d6c(213, t3->unk4C >> 16, t3->unk50 >> 16, 1);
    t4 = *c;
    t4->unk46 = w3;
    w4 = sub_08064d6c(210, (s16)((t4->unk4C >> 16) - 1), (s16)((t4->unk50 >> 16) + 1), 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(8);
    goto top;
}

void sub_080afaa4(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *t;
    struct Task *t2;
    struct Task *t3;
    struct Task *t4;
    s32 r;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->unk04 = (u32)sub_080af358;
    sub_080639b4((u32)gUnk_0874B488);
    r = sub_080640a8((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    u1->unk54 = 0xFFFD0000;
top:
    t = *c;
    w = sub_08064d6c(213, t->unk4C >> 16, t->unk50 >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = sub_08064d6c(210, t2->unk4C >> 16, (s16)((t2->unk50 >> 16) + 2), 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(8);
    t3 = *c;
    w3 = sub_08064d6c(213, t3->unk4C >> 16, t3->unk50 >> 16, 1);
    t4 = *c;
    t4->unk46 = w3;
    w4 = sub_08064d6c(210, t4->unk4C >> 16, (s16)((t4->unk50 >> 16) - 2), 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(8);
    goto top;
}

void sub_080afb50(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *t;
    struct Task *t2;
    struct Task *t3;
    struct Task *t4;
    s32 r;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->unk04 = (u32)sub_080af358;
    sub_080639b4((u32)gUnk_0874B488);
    r = sub_080640a8((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    u1->unk54 = 0xFFFDCCCD;
    u1->unk58 = 0x0001B333;
top:
    t = *c;
    w = sub_08064d6c(213, t->unk4C >> 16, t->unk50 >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = sub_08064d6c(210, (s16)((t2->unk4C >> 16) - 1), (s16)((t2->unk50 >> 16) - 1), 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(8);
    t3 = *c;
    w3 = sub_08064d6c(213, t3->unk4C >> 16, t3->unk50 >> 16, 1);
    t4 = *c;
    t4->unk46 = w3;
    w4 = sub_08064d6c(210, (s16)((t4->unk4C >> 16) + 1), (s16)((t4->unk50 >> 16) + 1), 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(8);
    goto top;
}

void sub_080afc10(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *u2;
    struct Task *t;
    struct Task *t2;
    struct Task *t3;
    struct Task *t4;
    s32 r;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->unk04 = (u32)sub_080af358;
    sub_080639b4((u32)gUnk_0874B488);
    r = sub_080640a8((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    u1->unk54 = 0xFFFE0000;
    u1->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(16);
    u2 = *c;
    u2->unk54 = 0xFFFD0000;
    u2->unk58 = 0;
top:
    t = *c;
    w = sub_08064d6c(213, t->unk4C >> 16, t->unk50 >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = sub_08064d6c(210, t2->unk4C >> 16, (s16)((t2->unk50 >> 16) + 2), 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(8);
    t3 = *c;
    w3 = sub_08064d6c(213, t3->unk4C >> 16, t3->unk50 >> 16, 1);
    t4 = *c;
    t4->unk46 = w3;
    w4 = sub_08064d6c(210, t4->unk4C >> 16, (s16)((t4->unk50 >> 16) - 2), 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(8);
    goto top;
}

void sub_080afcd4(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *u2;
    struct Task *t;
    struct Task *t2;
    struct Task *t3;
    struct Task *t4;
    s32 r;
    s16 w;
    s16 w2;
    s16 w3;
    s16 w4;

    c = &gCurTask;
    (*c)->unk04 = (u32)sub_080af358;
    sub_080639b4((u32)gUnk_0874B488);
    r = sub_080640a8((struct AnimCmd *)gUnk_0874B20C);
    u1 = *c;
    u1->unk28 = r;
    u1->unk54 = 0xFFFE0000;
    u1->unk58 = 128 << 10;
    TaskYieldTrampoline(16);
    u2 = *c;
    u2->unk54 = 0xFFFD0000;
    u2->unk58 = 0;
top:
    t = *c;
    w = sub_08064d6c(213, t->unk4C >> 16, t->unk50 >> 16, 1);
    t2 = *c;
    t2->unk46 = w;
    w2 = sub_08064d6c(210, t2->unk4C >> 16, (s16)((t2->unk50 >> 16) + 2), 1);
    (*c)->unk46 = w2;
    TaskYieldTrampoline(8);
    t3 = *c;
    w3 = sub_08064d6c(213, t3->unk4C >> 16, t3->unk50 >> 16, 1);
    t4 = *c;
    t4->unk46 = w3;
    w4 = sub_08064d6c(210, t4->unk4C >> 16, (s16)((t4->unk50 >> 16) - 2), 1);
    (*c)->unk46 = w4;
    TaskYieldTrampoline(8);
    goto top;
}

void sub_080afd9c(void)
{
    struct Task **c;
    struct Task *u1;
    struct Task *t;
    s16 w;
    s32 r;

    c = &gCurTask;
    (*c)->unk04 = (u32)sub_080af358;
    sub_080639b4((u32)gUnk_0874B4A4);
    r = sub_080640a8((struct AnimCmd *)gUnk_0874B234);
    u1 = *c;
    u1->unk28 = r;
    u1->unk54 = 0xFFFA0000;
top:
    t = *c;
    w = sub_08064d6c(213, t->unk4C >> 16, t->unk50 >> 16, 1);
    (*c)->unk46 = w;
    TaskYieldTrampoline(16);
    goto top;
}

void sub_080afdf0(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 w;
    struct Task *x;

    t = gCurTask;
    t->unk00 = (u32)sub_0806572c;
    t->unk0C = (u32)TaskDrawWorld;
    t->unk42 = 12;
    gCurTask->unk38 = gUnk_08754504;
    gCurTask->unk43 = 1;
    gUnk_030023B4 = Div(abs(gTasks[gCurTask->unk44].unk54) << 4,
                        abs(gTasks[gCurTask->unk44].unk54) + abs(gTasks[gCurTask->unk44].unk58));
    gUnk_030023D4 = Div(abs(gTasks[gCurTask->unk44].unk58) << 4,
                        abs(gTasks[gCurTask->unk44].unk54) + abs(gTasks[gCurTask->unk44].unk58));
    u = gCurTask;
    u->unk54 = ((gUnk_030023B4 << 17) >> 4) + gTasks[u->unk44].unk54;
    /* a second task pointer: the unk58 store after the if goes through it */
    x = gCurTask;
    v = (gUnk_030023D4 << 17) >> 4;
    w = gTasks[x->unk44].unk58;
    /* the arms re-read the cell: `v + cell` keeps the ROM's operand order */
    if (w >= 0)
        w = gTasks[x->unk44].unk58 - v;
    else
        w = v + gTasks[x->unk44].unk58;
    x->unk58 = w;
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C = 19;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 20;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 21;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 22;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskExitTrampoline();
}

void sub_080aff40(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 w;
    s32 y;
    struct Task *x;

    t = gCurTask;
    t->unk00 = (u32)sub_0806572c;
    t->unk0C = (u32)TaskDrawWorld;
    t->unk42 = 12;
    gCurTask->unk38 = gUnk_08754504;
    gCurTask->unk43 = 1;
    gUnk_030023B4 = Div(abs(gTasks[gCurTask->unk44].unk54) << 4,
                        abs(gTasks[gCurTask->unk44].unk54) + abs(gTasks[gCurTask->unk44].unk58));
    gUnk_030023D4 = Div(abs(gTasks[gCurTask->unk44].unk58) << 4,
                        abs(gTasks[gCurTask->unk44].unk54) + abs(gTasks[gCurTask->unk44].unk58));
    /* u for unk54, a second pointer x per arm for the single unk58 store */
    u = gCurTask;
    w = gTasks[u->unk44].unk58;
    if (w > 0)
    {
        u->unk54 = ((gUnk_030023B4 << 17) >> 4) + gTasks[u->unk44].unk54 + 0xFFFEE000;
        x = gCurTask;
        v = (gUnk_030023D4 << 17) >> 4;
        w = gTasks[x->unk44].unk58;
        if (w >= 0)
            y = w - v + 0xFFFFC000;
        else
            y = v + w + 0xFFFFC000;
    }
    else if (w < 0)
    {
        u->unk54 = ((gUnk_030023B4 << 17) >> 4) + gTasks[u->unk44].unk54 + (128 << 7);
        x = gCurTask;
        v = (gUnk_030023D4 << 17) >> 4;
        w = gTasks[x->unk44].unk58;
        if (w >= 0)
            y = w - v + 0xFFFF4000;
        else
            y = v + w + 0xFFFF4000;
    }
    else
    {
        u->unk54 = ((gUnk_030023B4 << 17) >> 4) + gTasks[u->unk44].unk54 + 0xFFFF0000;
        x = gCurTask;
        v = (gUnk_030023D4 << 17) >> 4;
        w = gTasks[x->unk44].unk58;
        if (w >= 0)
            y = w - v + 0xFFFF8000;
        else
            y = v + w + 0xFFFF8000;
    }
    x->unk58 = y;
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C = 19;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 20;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 21;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 22;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskExitTrampoline();
}

void sub_080b0144(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 w;
    s32 y;
    struct Task *x;

    t = gCurTask;
    t->unk00 = (u32)sub_0806572c;
    t->unk0C = (u32)TaskDrawWorld;
    t->unk42 = 12;
    gCurTask->unk38 = gUnk_08754504;
    gCurTask->unk43 = 1;
    gUnk_030023B4 = Div(abs(gTasks[gCurTask->unk44].unk54) << 4,
                        abs(gTasks[gCurTask->unk44].unk54) + abs(gTasks[gCurTask->unk44].unk58));
    gUnk_030023D4 = Div(abs(gTasks[gCurTask->unk44].unk58) << 4,
                        abs(gTasks[gCurTask->unk44].unk54) + abs(gTasks[gCurTask->unk44].unk58));
    /* u for unk54, a second pointer x per arm for the single unk58 store */
    u = gCurTask;
    w = gTasks[u->unk44].unk58;
    if (w > 0)
    {
        u->unk54 = ((gUnk_030023B4 << 17) >> 4) + gTasks[u->unk44].unk54 + (128 << 7);
        x = gCurTask;
        v = (gUnk_030023D4 << 17) >> 4;
        w = gTasks[x->unk44].unk58;
        if (w >= 0)
            y = w - v + (192 << 8);
        else
            y = v + w + (192 << 8);
    }
    else if (w < 0)
    {
        u->unk54 = ((gUnk_030023B4 << 17) >> 4) + gTasks[u->unk44].unk54 + 0xFFFEE000;
        x = gCurTask;
        v = (gUnk_030023D4 << 17) >> 4;
        w = gTasks[x->unk44].unk58;
        if (w >= 0)
            y = w - v + (128 << 7);
        else
            y = v + w + (128 << 7);
    }
    else
    {
        u->unk54 = ((gUnk_030023B4 << 17) >> 4) + gTasks[u->unk44].unk54 + 0xFFFF0000;
        x = gCurTask;
        v = (gUnk_030023D4 << 17) >> 4;
        w = gTasks[x->unk44].unk58;
        if (w >= 0)
            y = w - v + (128 << 8);
        else
            y = v + w + (128 << 8);
    }
    x->unk58 = y;
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C = 19;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 20;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 21;
        TaskYieldTrampoline(2);
        gCurTask->unk3C = 22;
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskExitTrampoline();
}

void sub_080b0338(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 w;
    struct Task *x;

    t = gCurTask;
    t->unk00 = (u32)sub_0806572c;
    t->unk0C = (u32)TaskDrawWorld;
    t->unk42 = 13;
    gCurTask->unk38 = gUnk_08754504;
    gCurTask->unk43 = 1;
    gUnk_030023B4 = Div(abs(gTasks[gCurTask->unk44].unk54) << 4,
                        abs(gTasks[gCurTask->unk44].unk54) + abs(gTasks[gCurTask->unk44].unk58));
    gUnk_030023D4 = Div(abs(gTasks[gCurTask->unk44].unk58) << 4,
                        abs(gTasks[gCurTask->unk44].unk54) + abs(gTasks[gCurTask->unk44].unk58));
    u = gCurTask;
    u->unk54 = ((gUnk_030023B4 << 16) >> 4) + gTasks[u->unk44].unk54;
    /* a second task pointer: the unk58 store after the if goes through it */
    x = gCurTask;
    v = (gUnk_030023D4 << 16) >> 4;
    w = gTasks[x->unk44].unk58;
    /* the arms re-read the cell: `v + cell` keeps the ROM's operand order */
    if (w >= 0)
        w = gTasks[x->unk44].unk58 - v;
    else
        w = v + gTasks[x->unk44].unk58;
    x->unk58 = w;
    switch (gTasks[gCurTask->unk44].unk73)
    {
    case 0:
    case 1:
    case 2:
    case 3:
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C = 17;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 16;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 15;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 14;
            TaskYieldTrampoline(2);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 1);
        break;
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
    case 10:
        if (gTasks[gCurTask->unk44].unk3C == 3)
        {
            gCurTask->unk3C = 13;
            TaskYieldTrampoline(4);
            gCurTask->unk3C = 12;
            TaskYieldTrampoline(4);
            gCurTask->unk3C = 11;
            TaskYieldTrampoline(4);
            gCurTask->unk3C = 10;
            TaskYieldTrampoline(4);
        }
        else
        {
            gCurTask->unk3C = 11;
            TaskYieldTrampoline(4);
            gCurTask->unk3C = 10;
            TaskYieldTrampoline(4);
            gCurTask->unk3C = 13;
            TaskYieldTrampoline(4);
            gCurTask->unk3C = 12;
            TaskYieldTrampoline(4);
        }
        break;
    case 11:
        gCurTask->unk3C = 18;
        TaskYieldTrampoline(16);
        break;
    }
    TaskExitTrampoline();
}

void sub_080b0570(void)
{
    struct Task *t1;
    struct Task *t;
    struct Task *tb;
    s16 *a;
    struct Task *tt;
    struct Task *tt2;
    u32 o;
    u32 o2;
    s32 x;
    s32 y;

    t1 = gCurTask;
    if (t1->unk54 != 0 || t1->unk58 != 0 || t1->unk5C != 0 || t1->unk60 != 0)
        TaskIntegrateMotion();
    t = gCurTask;
    x = t->unk4C;
    tb = gTasks;
    a = &t->unk44;
    o = *a * 144;
    tt = (struct Task *)((u8 *)tb + o);
    t->unk48 = (x >> 16) + (u16)tt->unk48;
    y = t->unk50;
    o2 = *a * 144;
    tt2 = (struct Task *)((u8 *)tb + o2);
    t->unk4A = (y >> 16) + (u16)tt2->unk4A;
}

void sub_080b05e8(void)
{
    struct Task **c;
    s32 *p;
    s32 z;
    s32 vA;
    s32 vB;
    s32 vC;
    s32 vD;
    s32 vE;
    s32 vF;
    s32 vG;
    struct Task *u;
    s32 r;
    s32 r2;
    s32 r3;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    struct Task *u6;
    struct Task *u7;
    struct Task *u8;
    struct Task *u9;
    struct Task *u10;
    struct Task *u11;
    struct Task *u12;
    struct Task *u13;
    struct Task *u14;
    struct Task *u15;
    struct Task *u16;
    struct Task *u17;
    struct Task *u18;
    struct Task *u19;
    struct Task *u20;
    struct Task *u21;

    PlayBgm(32);
    sub_08003184();
    SetBgmVolume(0);
    p = gUnk_02007D00;
    z = 0;
    p[9] = z;
    c = &gCurTask;
    (*c)->unk43 = 1;
    u = *c;
    u->unk3C = 0xFFFF;
    u->unk8C->unk2C = (struct AnimCmd *)z;
    p[6] = z;
    p[7] = 3;
    sub_08063698(80, 32);
    (*c)->unk04 = (u32)sub_080b07d8;
    TaskYieldTrampoline(65);
    r = sub_080640a8((struct AnimCmd *)gUnk_0874B240);
    u2 = *c;
    u2->unk28 = r;
    u2->unk4C = 128 << 17;
    u2->unk50 = 0xFFF80000;
    TaskYieldTrampoline(28);
    u3 = *c;
    vA = 0xFFFF0000;
    u3->unk54 = vA;
    vB = 128 << 9;
    u3->unk58 = vB;
    TaskYieldTrampoline(72);
    u4 = *c;
    vC = 128 << 8;
    u4->unk58 = vC;
    TaskYieldTrampoline(10);
    u5 = *c;
    u5->unk58 = 128 << 6;
    TaskYieldTrampoline(10);
    u6 = *c;
    u6->unk58 = z;
    TaskYieldTrampoline(16);
    sub_08064d34(220, 1);
    PlaySfx(0x241);
    r2 = sub_080640a8((struct AnimCmd *)gUnk_0874B254);
    u7 = *c;
    u7->unk28 = r2;
    vD = 128 << 11;
    u7->unk54 = vD;
    u7->unk58 = vA;
    TaskYieldTrampoline(8);
    u8 = *c;
    vE = 128 << 10;
    u8->unk54 = vE;
    TaskYieldTrampoline(8);
    u9 = *c;
    vF = 0xFFFE0000;
    u9->unk58 = vF;
    TaskYieldTrampoline(8);
    u10 = *c;
    u10->unk54 = vB;
    u10->unk58 = vA;
    TaskYieldTrampoline(6);
    u11 = *c;
    u11->unk54 = vC;
    u11->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(4);
    u12 = *c;
    u12->unk54 = vF;
    u12->unk58 = vC;
    TaskYieldTrampoline(6);
    r3 = sub_080640a8((struct AnimCmd *)gUnk_0874B2C8);
    u13 = *c;
    u13->unk28 = r3;
    u13->unk54 = 0xFFFC0000;
    vG = 192 << 8;
    u13->unk58 = vG;
    TaskYieldTrampoline(6);
    u14 = *c;
    u14->unk58 = 192 << 9;
    TaskYieldTrampoline(11);
    u15 = *c;
    u15->unk54 = vA;
    TaskYieldTrampoline(6);
    u16 = *c;
    u16->unk54 = vB;
    TaskYieldTrampoline(6);
    u17 = *c;
    u17->unk54 = vE;
    TaskYieldTrampoline(6);
    u18 = *c;
    u18->unk54 = vD;
    u18->unk58 = vG;
    TaskYieldTrampoline(6);
    u19 = *c;
    u19->unk54 = vE;
    u19->unk58 = 0xFFFFC000;
    TaskYieldTrampoline(6);
    u20 = *c;
    u20->unk54 = vC;
    u20->unk58 = 0xFFFF4000;
    TaskYieldTrampoline(6);
    u21 = *c;
    u21->unk54 = 128 << 7;
    TaskYieldTrampoline(6);
    TaskStop();
    TaskYieldTrampoline(18);
}

void sub_080b07d8(void)
{
    struct Task **c;
    struct Task *u;
    s32 *p;
    s32 r;

    c = &gCurTask;
    r = sub_0806415c((*c)->unk28);
    u = *c;
    u->unk28 = r;
    p = gUnk_02007D00;
    if (p[5] > 23)
        p[5] = 0;
    BlendColors((u16 *)gUnk_082FE0E4, (u16 *)gUnk_082FE104, gUnk_0874AD44[p[5]], 16, (u16 *)((((*c)->unk40 >> 12) << 5) + (u32)gObjPalette));
    p[5]++;
}

void sub_080b0840(void)
{
    struct Task **c;
    struct Task *u;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;

    c = &gCurTask;
    u = *c;
    u->unk04 = (u32)sub_080b08a0;
    u->unk28 = -4;
    TaskYieldTrampoline(143);
    u2 = *c;
    u2->unk28 = -3;
    TaskYieldTrampoline(32);
    u3 = *c;
    u3->unk28 = -2;
    TaskYieldTrampoline(32);
    u4 = *c;
    u4->unk28 = -1;
    TaskYieldTrampoline(32);
    u5 = *c;
    u5->unk28 = 0;
    TaskYieldTrampoline(16);
    TaskExitTrampoline();
}

void sub_080b08a0(void)
{
    s32 *p;

    *(u16 *)&gUnk_03001F00 += gCurTask->unk28;
    p = gUnk_02007D00;
    if (p[9] < 0)
        SetBgmVolume(0);
    if (p[9] <= 255)
    {
        SetBgmVolume((u16)p[9]);
        p[9]++;
    }
}

void sub_080b08e4(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)TaskDrawWorld;
    t->unk42 = 10;
    u = gCurTask;
    u->unk38 = gUnk_087544B4;
    u->unk40 |= 0x800;
    u->unk4C = (gTasks[u->unk44].unk48 - 16) << 16;
    u->unk50 = gTasks[u->unk44].unk4A << 16;
    u->unk3C = 14;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 15;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 16;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 17;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 18;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 19;
    TaskYieldTrampoline(2);
    TaskExitTrampoline();
}

void sub_080b09ac(void)
{
    TaskStop();
    while (gViewRect[2] > 8)
        TaskYieldTrampoline(1);
    gCurTask->unk04 = 0;
    gCurTask->unk0C = (u32)sub_080b0b50;
    sub_080b0b04();
    gCurTask->unk43 = 1;
    TaskSetFrame(0);
    gCurTask->unk18 = 128 << 13;
    gCurTask->unk28 = 0xFFFFF300;
    gCurTask->unk4C = 138 << 16;
    gCurTask->unk50 = 0xFFF00000;
    gCurTask->unk54 = 0xFFFF0000;
    gCurTask->unk58 = 128 << 8;
    TaskYieldTrampoline(28);
    gCurTask->unk54 = 0xFFFF4000;
    TaskYieldTrampoline(28);
    gCurTask->unk54 = 0xFFFF8000;
    TaskYieldTrampoline(28);
    gCurTask->unk54 = 0xFFFFC000;
    TaskYieldTrampoline(14);
    gCurTask->unk54 = 0xFFFFF000;
    gCurTask->unk58 = 128 << 7;
    TaskYieldTrampoline(14);
    gCurTask->unk54 = 0xFFFFF800;
    TaskYieldTrampoline(14);
    gCurTask->unk54 = 128 << 5;
    TaskYieldTrampoline(14);
    gCurTask->unk54 = 128 << 6;
    TaskYieldTrampoline(14);
    gCurTask->unk58 = 128 << 6;
    TaskYieldTrampoline(62);
    gCurTask->unk3C = 1;
    gCurTask->unk40 = (gCurTask->unk40 & 0xFFF) | 0xF000;
    gCurTask->unk18 = 0;
    gCurTask->unk28 = 0;
    TaskYieldTrampoline(48);
    gCurTask->unk58 = 128 << 5;
    TaskYieldTrampoline(84);
    gCurTask->unk54 = 128 << 4;
    gCurTask->unk58 = 128 << 3;
    TaskSleepForever();
}

void sub_080b0b04(void)
{
    u16 *q;
    struct Task *t;

    q = (u16 *)gUnk_082FFDF0;
    RequestCopy(4, ((u32 *)q)[3], 0x06010000, q[1] << 5);
    RequestCopy(2, ((u32 *)q)[2], (u32)gUnk_03001570, 32);
    t = gCurTask;
    t->unk40 = 0x8010;
    t->unk38 = gUnk_08754560;
}

void sub_080b0b50(void)
{
    struct Task *t;
    u32 *tbl;
    s8 sign;
    s32 r;

    if (gCurTask->unk18 > 0)
    {
        gCurTask->unk18 += gCurTask->unk28;
        if (gCurTask->unk18 < 0)
            gCurTask->unk18 = 0;
        if (gCurTask->unk18 > (252 << 14))
            gCurTask->unk18 = 252 << 14;
    }
    if (gCurTask->unk38 == 0)
        return;
    if (gCurTask->unk3C == -1)
        return;
    if (sub_08065160() == 0)
        return;
    if (TaskIsOnScreen() != 0)
    {
        t = gCurTask;
        tbl = t->unk38;
        if (t->unk18 > 0)
        {
            sign = (t->unk3E & 0x8000) ? -1 : 1;
            t->unk3E &= 0x7FFF;
            r = DrawAffineSprite(tbl[t->unk3C], (u16)gUnk_0873FF98[t->unk18 >> 16] * sign, gUnk_0873FF98[t->unk18 >> 16], 0);
            QueueSprite(gCurTask->unk42, r, gCurTask->unk3E, gCurTask->unk40,
                         gCurTask->unk48 - gSpriteCameraX,
                         (s16)(gCurTask->unk4A - gSpriteCameraY));
            if (sign < 0)
                gCurTask->unk3E |= 0x8000;
        }
        else
        {
            QueueSprite(t->unk42, tbl[t->unk3C], t->unk3E, t->unk40, t->unk48 - gSpriteCameraX,
                         (s16)(t->unk4A - gSpriteCameraY));
        }
    }
}

void sub_080b0cc8(void)
{
    struct Task *t;
    s32 d; /* an int: the ROM sign-extends the copied byte before the strb */

    t = gCurTask;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk38 = gUnk_08753BA4;
    t->unk42 = 9;
    d = ((struct Task *)(gCurTask->unk44 * 144 + (u32)gTasks))->unk43;
    gCurTask->unk43 = d;
    gCurTask->unk7A = 0;
    TaskSetFrame(0);
    TaskYieldTrampoline(2);
    TaskSetFrame(-1);
    TaskYieldTrampoline(2);
    TaskSetFrame(1);
    TaskYieldTrampoline(2);
    TaskSetFrame(-1);
    TaskYieldTrampoline(2);
    TaskSetFrame(0);
    TaskYieldTrampoline(2);
    TaskSetFrame(-1);
    TaskYieldTrampoline(2);
    TaskSetFrame(1);
    TaskYieldTrampoline(2);
    TaskSetFrame(-1);
    TaskYieldTrampoline(2);
    gCurTask->unk6C = 0;
    do
    {
        TaskSetFrame(2);
        TaskYieldTrampoline(2);
        TaskSetFrame(-1);
        TaskYieldTrampoline(2);
        TaskSetFrame(3);
        TaskYieldTrampoline(2);
        TaskSetFrame(-1);
        TaskYieldTrampoline(2);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 2);
    sub_080b15c0();
    CallTableEntry(gCurTask->unk14, 8, gUnk_0874B540);
}

void sub_080b0de4(void)
{
    gCurTask->unk38 = gUnk_08753AAC;
    sub_08063908((struct ActorDef *)gUnk_0874B96C);
    gCurTask->unk40 += 0x1000;
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    gCurTask->unk04 = (u32)sub_080b1564;
    sub_08063e14();
    TaskSetMotionXFacing(192 << 10, 0x5A5A5A5A);
    gCurTask->unk60 = 168 << 5;
    gCurTask->unk68 = 192 << 10;
    while (1)
    {
        if (gCurTask->unk7A != 0)
        {
            TaskSetFrame(5);
            TaskYieldTrampoline(4);
            TaskSetFrame(6);
            TaskYieldTrampoline(4);
        }
        else
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(1);
        }
    }
}

void sub_080b0e80(void)
{
    struct Task **c;
    struct Task *u;

    c = &gCurTask;
    (*c)->unk38 = gUnk_08753AC8;
    sub_08063908((struct ActorDef *)gUnk_0874B998);
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    (*c)->unk04 = (u32)sub_080b1564;
    sub_08063e14();
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    u = *c;
    u->unk60 = 168 << 5;
    u->unk68 = 192 << 10;
top:
    TaskSetFrame(5);
    TaskYieldTrampoline(4);
    TaskSetFrame(6);
    TaskYieldTrampoline(4);
    TaskSetFrame(7);
    TaskYieldTrampoline(4);
    TaskSetFrame(6);
    TaskYieldTrampoline(4);
    goto top;
}

void sub_080b0f04(void)
{
    gCurTask->unk38 = gUnk_08753B68;
    sub_08063908((struct ActorDef *)gUnk_0874B9C4);
    gCurTask->unk40 += 0x1000;
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    gCurTask->unk04 = (u32)sub_080b1564;
    sub_08063e14();
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    gCurTask->unk60 = 168 << 5;
    gCurTask->unk68 = 192 << 10;
    while (1)
    {
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        TaskSetFrame(7);
        TaskYieldTrampoline(6);
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
    }
}

void sub_080b0f98(void)
{
    gCurTask->unk38 = gUnk_08753B48;
    sub_08063908((struct ActorDef *)gUnk_0874B9F0);
    gCurTask->unk40 += 0x1000;
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    gCurTask->unk04 = (u32)sub_080b1564;
    sub_08063e14();
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    gCurTask->unk60 = 168 << 5;
    gCurTask->unk68 = 192 << 10;
    while (1)
    {
        TaskSetFrame(5);
        TaskYieldTrampoline(6);
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
        TaskSetFrame(7);
        TaskYieldTrampoline(6);
        TaskSetFrame(6);
        TaskYieldTrampoline(4);
    }
}

void sub_080b102c(void)
{
    gCurTask->unk38 = gUnk_08753A8C;
    sub_08063908((struct ActorDef *)gUnk_0874BA1C);
    gCurTask->unk40 += 0x1000;
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    gCurTask->unk04 = (u32)sub_080b1578;
    sub_0806421c(sub_08064314(3), 128 << 2);
    gCurTask->unk54 = gUnk_030023B4;
    gCurTask->unk58 = gUnk_030023D4;
    while (1)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(10);
        TaskSetFrame(5);
        TaskYieldTrampoline(10);
        TaskSetFrame(6);
        TaskYieldTrampoline(10);
        TaskSetFrame(7);
        TaskYieldTrampoline(10);
    }
}

void sub_080b10c8(void)
{
    gCurTask->unk38 = gUnk_08753B04;
    sub_08063908((struct ActorDef *)gUnk_0874BA48);
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    gCurTask->unk04 = (u32)sub_080b123c;
    gCurTask->unk28 = sub_080640dc((struct AnimCmd *)gUnk_0874B560);
    sub_08063e14();
    TaskSetMotionXFacing(128 << 8, 0x5A5A5A5A);
    gCurTask->unk58 = 128 << 8;
    TaskYieldTrampoline(6);
    gCurTask->unk58 = 128 << 9;
    TaskYieldTrampoline(6);
    gCurTask->unk58 = 128 << 10;
    while (gCurTask->unk7A == 0)
        TaskYieldTrampoline(1);
    gCurTask->unk7A = 0;
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(6);
    gCurTask->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(6);
    gCurTask->unk58 = 128 << 8;
    TaskYieldTrampoline(6);
    gCurTask->unk58 = 128 << 10;
    while (gCurTask->unk7A == 0)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskYieldTrampoline(10);
    gCurTask->unk28 = sub_080640dc((struct AnimCmd *)gUnk_0874B568);
    TaskYieldTrampoline(44);
    gCurTask->unk28 = sub_080640dc(0);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(4);
    TaskYieldTrampoline(3);
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    TaskSetFrame(4);
    TaskYieldTrampoline(3);
    PlaySfx(189);
    sub_080639b4((u32)gUnk_0874BB98);
    sub_0806d730();
    sub_08063fe0();
}

void sub_080b123c(void)
{
    struct Task **c;
    struct Task *u;
    s32 r;

    c = &gCurTask;
    r = sub_08064188((*c)->unk28);
    u = *c;
    u->unk28 = r;
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

void sub_080b1264(void)
{
    struct ActorSpawn sp;

    gCurTask->unk38 = gUnk_08753AE8;
    sub_08063908((struct ActorDef *)gUnk_0874BA74);
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    gCurTask->unk04 = (u32)sub_080b1398;
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    TaskStop();
    TaskYieldTrampoline(16);
    gCurTask->unk28 = sub_080640dc((struct AnimCmd *)gUnk_0874B574);
    TaskYieldTrampoline(16);
    sub_08063e14();
    TaskSetMotionXFacing(128 << 9, 0x5A5A5A5A);
    while (1)
    {
        PlaySfx(0x222);
        sp.unk00 = 15;
        sp.unk04 = 117;
        sp.unk08 = gCurTask->unk73;
        sp.unk09 = gCurTask->unk74;
        sp.unk0A = 1;
        gCurTask->unk46 = sub_08064b28(&sp, 0);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk58 = 0xFFFF8000;
            TaskYieldTrampoline(6);
            gCurTask->unk58 = 0xFFFF0000;
            TaskYieldTrampoline(6);
            gCurTask->unk58 = 0xFFFF8000;
            TaskYieldTrampoline(6);
            gCurTask->unk58 = 128 << 8;
            TaskYieldTrampoline(6);
            gCurTask->unk58 = 128 << 9;
            TaskYieldTrampoline(6);
            gCurTask->unk58 = 128 << 8;
            TaskYieldTrampoline(6);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 1);
    }
}

void sub_080b1398(void)
{
    struct Task **c;
    struct Task *u;
    s32 r;

    c = &gCurTask;
    r = sub_08064188((*c)->unk28);
    u = *c;
    u->unk28 = r;
    sub_08068e04();
    sub_08069b44();
}

void sub_080b13bc(void)
{
    struct Task *t;
    s32 d; /* an int: the ROM sign-extends the copied byte before the strb */

    t = gCurTask;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 10;
    gCurTask->unk38 = gUnk_08753B88;
    d = ((struct Task *)(gCurTask->unk44 * 144 + (u32)gTasks))->unk43;
    gCurTask->unk43 = d;
    gCurTask->unk7A = 0;
    sub_08063908((struct ActorDef *)gUnk_0874BACC);
    gCurTask->unk04 = (u32)sub_080b1564;
    gCurTask->unk58 = 128 << 11;
    while (gCurTask->unk7A == 0)
    {
        TaskSetFrame(0);
        TaskYieldTrampoline(4);
        TaskSetFrame(1);
        TaskYieldTrampoline(4);
    }
    gCurTask->unk04 = 0;
    TaskStop();
    TaskSetFrame(2);
    TaskYieldTrampoline(4);
    TaskSetFrame(3);
    TaskYieldTrampoline(2);
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    sub_08063fe0();
}

void sub_080b14ac(void)
{
    struct Task **c;
    struct Task *u;
    s32 r;

    c = &gCurTask;
    (*c)->unk38 = gUnk_08753B20;
    sub_08063908((struct ActorDef *)gUnk_0874BAA0);
    TaskSetFrame(4);
    TaskYieldTrampoline(60);
    (*c)->unk04 = (u32)sub_080b14fc;
    r = sub_080640dc((struct AnimCmd *)gUnk_0874B590);
    u = *c;
    u->unk28 = r;
    u->unk2C = -32;
    TaskSleepForever();
}

void sub_080b14fc(void)
{
    struct Task **c;
    struct Task *u;
    s32 r;
    s32 w;

    c = &gCurTask;
    r = sub_08064188((*c)->unk28);
    u = *c;
    u->unk28 = r;
    w = u->unk2C + 1;
    u->unk2C = w;
    if (w > 128 << 1)
    {
        sub_080639a4((u32)gUnk_0874BFD4);
        sub_0806a25c();
    }
    else if ((w & 15) == 0)
        sub_080645a4(154 << 7, 0x18100);
    if (gCurTask->unk2C > 0)
    {
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_080b1564(void)
{
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

void sub_080b1578(void)
{
    sub_08068e04();
    sub_08069b44();
}

s32 sub_080b1588(void)
{
    TaskStopY();
    return 0;
}

s32 sub_080b1594(void)
{
    struct Task *t;

    TaskStopY();
    t = gCurTask;
    t->unk60 = 168 << 5;
    t->unk68 = 192 << 10;
    return 0;
}

s32 sub_080b15b4(void)
{
    sub_08063ff4();
    return 0;
}

void sub_080b15c0(void)
{
    s32 ofs;

    if (((struct Task *)(gCurTask->unk44 * 144 + (u32)gTasks))->unk78 >= gUnk_02007D00[9] >> 1)
        ofs = 8;
    else
        ofs = 0;
    gUnk_030023D4 = 255 - gUnk_0874B5A4[ofs + (gUnk_02007D00[3] >> 16)];
    gUnk_030023D4 -= gUnk_0874B5A4[ofs + (gUnk_02007D00[3] & 0xFFFF)];
    gUnk_030023D4 = RandomRange(gUnk_030023D4);
    gUnk_030023B4 = 7;
    while (1)
    {
        if (gUnk_030023B4 < 0)
            while (1);
        if (gUnk_030023B4 != gUnk_02007D00[3] >> 16 && gUnk_030023B4 != (gUnk_02007D00[3] & 0xFFFF))
            gUnk_030023D4 -= gUnk_0874B5A4[ofs + gUnk_030023B4];
        if (gUnk_030023D4 <= 0)
            break;
        gUnk_030023B4--;
    }
    switch (gUnk_030023B4)
    {
    case 0:
        sub_0806395c(0);
        break;
    case 1:
        sub_0806395c(1);
        break;
    case 2:
        sub_0806395c(2);
        break;
    case 3:
        sub_0806395c(3);
        break;
    case 4:
        sub_0806395c(4);
        break;
    case 5:
        sub_0806395c(5);
        break;
    case 6:
        sub_0806395c(6);
        break;
    case 7:
        sub_0806395c(7);
        break;
    }
    gUnk_02007D00[3] = ((s16 *)gUnk_02007D00)[7] + (gUnk_030023B4 << 16);
}

void sub_080b1710(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *u3;
    u8 *b42;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->unk00 = z;
    t->unk0C = (u32)TaskDrawWorld;
    t->unk38 = gUnk_08753E8C;
    b42 = &t->unk42;
    *b42 = 11;
    u = *c;
    u->unk08 = (u32)sub_080b1910;
    u->unk43 = 255;
    u2 = *c;
    u2->unk4C = z;
    u2->unk50 = z;
    u2->unk28 = 0x10078;
    sub_0806395c(0);
    u3 = *c;
    CallTableEntry(u3->unk14, 6, gUnk_0874B5E4);
}

void sub_080b1770(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *u3;
    u8 *b42;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->unk00 = z;
    t->unk0C = (u32)sub_080b1890;
    t->unk38 = gUnk_08753E8C;
    b42 = &t->unk42;
    *b42 = 8;
    u = *c;
    u->unk08 = (u32)sub_080b1910;
    u->unk43 = 1;
    u2 = *c;
    u2->unk4C = z;
    u2->unk50 = z;
    u2->unk28 = 0x10078;
    sub_0806395c(0);
    u3 = *c;
    CallTableEntry(u3->unk14, 6, gUnk_0874B5E4);
}

void sub_080b17d0(void)
{
    struct Task *t;
    struct Task *t4;
    struct Task *tt;
    struct Task *tt2;
    s16 *a;
    u32 o;
    u16 *e;
    u16 *e2;
    u32 o2;

    TaskIntegrateMotion();
    t = gCurTask;
    a = &t->unk44;
    o = *a * 144;
    tt = (struct Task *)((u8 *)gTasks + o);
    e = (u16 *)((u8 *)tt + 72);
    t4 = t;
    t->unk48 = (t4->unk4C >> 16) + *e - 8;
    o2 = *a * 144;
    tt2 = (struct Task *)((u8 *)gTasks + o2);
    e2 = (u16 *)((u8 *)tt2 + 74);
    t->unk4A = (t4->unk50 >> 16) + *e2 - 24;
}

void sub_080b1830(void)
{
    struct Task *t;
    struct Task *t4;
    struct Task *tt;
    struct Task *tt2;
    s16 *a;
    u32 o;
    u16 *e;
    u16 *e2;
    u32 o2;

    TaskIntegrateMotion();
    t = gCurTask;
    a = &t->unk44;
    o = *a * 144;
    tt = (struct Task *)((u8 *)gTasks + o);
    e = (u16 *)((u8 *)tt + 72);
    t4 = t;
    t->unk48 = (t4->unk4C >> 16) + *e - 8;
    o2 = *a * 144;
    tt2 = (struct Task *)((u8 *)gTasks + o2);
    e2 = (u16 *)((u8 *)tt2 + 74);
    t->unk4A = (t4->unk50 >> 16) + *e2 + 24;
}

void sub_080b1890(void)
{
    struct Task *t;
    u32 *tbl;
    u8 j;

    TaskDrawWorld();
    t = gCurTask;
    tbl = t->unk38;
    j = gUnk_0874B63E[t->unk3C];
    if (gUnk_0874B614[j] != -1)
        QueueSprite(11, tbl[gUnk_0874B614[j]], t->unk3E, t->unk40, t->unk48 - gSpriteCameraX,
                     (s16)(t->unk4A - gSpriteCameraY));
}

void sub_080b1910(void)
{
    struct Unk0200D120 *td;
    struct Task **c;
    struct Task *t;
    struct Task *u;
    s32 i;

    td = gUnk_0200D120;
    c = &gCurTask;
    t = *c;
    i = t->unk44 - 32;
    if (td[i].unk05 == 2)
    {
        if (t->unk43 == -1)
            sub_080b17d0();
        else
            sub_080b1830();
        TaskSetEntry(sub_080b2294, gCurTaskIdx);
    }
    else
    {
        CallTableEntry(t->unk15, 6, gUnk_0874B5FC);
        u = *c;
        if (u->unk43 == -1)
            sub_080b17d0();
        else
            sub_080b1830();
        sub_080b199c();
    }
}

void sub_080b199c(void)
{
    u8 *t63;
    u8 n;
    u32 *t4;
    u32 o;
    u8 j;

    t63 = (u8 *)gUnk_0874B63E;
    j = t63[gCurTask->unk3C];
    n = ((u8 *)gUnk_0874B6BC)[j];
    j <<= 1;
    if (n != 0)
    {
        t4 = gUnk_0874B6D4;
        do
        {
            o = j << 2;
            sub_080639b4(*(u32 *)(o + (u32)t4));
            sub_08068e04();
            sub_080308e8(*(u32 *)(o + (u32)gUnk_0874B77C));
            n = n - 1;
            j = j + 1;
        } while (n != 0);
    }
}

void sub_080b1a00(void)
{
    CallTableEntry(gCurTask->unk14, 6, gUnk_0874B5E4);
}

void sub_080b1a1c(void)
{
    gCurTask->unk15 = 0;
    if (gCurTask->unk28 >> 16 > 2)
        gCurTask->unk28 = (gCurTask->unk28 & 0xFFFF) + (128 << 10);
    gUnk_030023D4 = 0;
    if (gCurTask->unk43 == 1)
        gUnk_030023D4 = 1;
    gCurTask->unk2C = gUnk_0874B824[gUnk_030023D4 * 2 + gUnk_02007D00[3]];
    if (gCurTask->unk43 == -1)
    {
        while (1)
        {
            gCurTask->unk3C = 3;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
            gCurTask->unk3C = 2;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
            gCurTask->unk3C = 1;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
        }
    }
    else
    {
        while (1)
        {
            gCurTask->unk3C = 0;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
            gCurTask->unk3C = 1;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
            gCurTask->unk3C = 2;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
            gCurTask->unk3C = 3;
            TaskYieldTrampoline(gUnk_0874B82A[((s16 *)&gCurTask->unk28)[1]]);
        }
    }
}

void sub_080b1b2c(void)
{
    struct Task *t;
    s32 v;

    t = gCurTask;
    v = t->unk28;
    if (*(u16 *)&t->unk28 != 0)
    {
        t->unk28 = v - 1;
        if ((t->unk28 & 15) == 0)
        {
            gUnk_030023D4 = ((struct Task *)(t->unk44 * 144 + (u32)gTasks))->unk54;
            if (gUnk_030023D4 > 0)
            {
                if (t->unk28 >> 16 <= 1)
                    t->unk28 += 0x10000;
            }
            else if (gUnk_030023D4 < 0)
            {
                if (t->unk28 >> 16 != 0)
                    t->unk28 -= 0x10000;
            }
            else if (((struct Task *)(t->unk44 * 144 + (u32)gTasks))->unk58 != 0)
            {
                if (t->unk28 >> 16 <= 1)
                    t->unk28 += 0x10000;
            }
        }
    }
    else
    {
        if (v & 0xFFFF0000)
            t->unk28 = v - 0x10000;
        gCurTask->unk28 += 120;
    }
    gCurTask->unk2C--;
    if (gCurTask->unk2C == 0)
    {
        gCurTask->unk2C = gUnk_0874B82E[gUnk_02007D00[3]];
        sub_0806395c(1);
        TaskSetEntry(sub_080b1a00, gCurTaskIdx);
    }
}

void sub_080b1c04(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;
    struct Task *u1;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *q4;
    struct Task *q4b;
    s32 zz;
    u8 *tj;
    u8 *tj2;
    s32 k;
    s32 x;
    s32 x2;

    t = gCurTask;
    t->unk15 = 1;
    u = gCurTask;
    k = u->unk43;
    if (k == -1)
    {
        c = &gCurTask;
        zz = 0;
        tj = (u8 *)gUnk_0874B82A;
        for (;;)
        {
            u1 = *c;
            x = *(s16 *)((u8 *)u1 + 42);
            if (x == 3)
            {
                u1->unk3C = 4;
                TaskYieldTrampoline(1);
                u2 = *c;
                u2->unk3C = 2;
                TaskYieldTrampoline(1);
                u3 = *c;
                u3->unk3C = 5;
                TaskYieldTrampoline(1);
                u4 = *c;
                u4->unk3C = zz;
                TaskYieldTrampoline(1);
            }
            else
            {
                u1->unk3C = 3;
                TaskYieldTrampoline(*(u8 *)(x + (u32)tj));
                u2 = *c;
                u2->unk3C = 2;
                TaskYieldTrampoline(*(u8 *)(*(s16 *)((u8 *)u2 + 42) + (u32)tj));
                u3 = *c;
                u3->unk3C = 1;
                TaskYieldTrampoline(*(u8 *)(*(s16 *)((u8 *)u3 + 42) + (u32)tj));
                u4 = *c;
                u4->unk3C = zz;
                TaskYieldTrampoline(*(u8 *)(*(s16 *)((u8 *)u4 + 42) + (u32)tj));
            }
        }
    }
    else
    {
        for (;;)
        {
            q1 = gCurTask;
            x2 = *(s16 *)((u8 *)q1 + 42);
            if (x2 == 3)
            {
                q1->unk3C = 4;
                TaskYieldTrampoline(1);
                q2 = gCurTask;
                q2->unk3C = 1;
                TaskYieldTrampoline(1);
                q3 = gCurTask;
                q3->unk3C = 5;
                TaskYieldTrampoline(1);
                q4b = gCurTask;
                q4b->unk3C = x2;
                TaskYieldTrampoline(1);
            }
            else
            {
                q1->unk3C = 0;
                TaskYieldTrampoline(*(u8 *)(x2 + (u32)gUnk_0874B82A));
                q2 = gCurTask;
                q2->unk3C = 1;
                TaskYieldTrampoline(*(u8 *)(*(s16 *)((u8 *)q2 + 42) + (u32)gUnk_0874B82A));
                q3 = gCurTask;
                q3->unk3C = 2;
                TaskYieldTrampoline(*(u8 *)(*(s16 *)((u8 *)q3 + 42) + (u32)gUnk_0874B82A));
                q4 = gCurTask;
                q4->unk3C = 3;
                TaskYieldTrampoline(*(u8 *)(*(s16 *)((u8 *)q4 + 42) + (u32)gUnk_0874B82A));
            }
        }
    }
}

void sub_080b1d2c(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;
    u8 *tb;
    s32 v;
    s32 v2;
    u32 lo;

    c = &gCurTask;
    t = *c;
    t->unk2C--;
    if (t->unk2C == 0)
    {
        tb = (u8 *)gUnk_0874B831;
        sub_0806395c(tb[RandomRange(4)]);
        TaskSetEntry(sub_080b1a00, gCurTaskIdx);
    }
    u = *c;
    v = u->unk28;
    lo = *(u16 *)&u->unk28;
    if (lo != 0)
    {
        v2 = v - 1;
        u->unk28 = v2;
        if ((v2 & 15) == 0 && v2 >> 16 <= 2)
            u->unk28 = v + 0xFFFF;
    }
    else
        u->unk28 = v + 120;
}

void sub_080b1d98(void)
{
    struct Task **c;
    struct Task **c2;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    u8 *t35;
    s32 r;
    s32 w;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->unk15 = 2;
    r = RandomRange(5);
    u = *c;
    u->unk34 = r;
    u->unk30 = z;
    t35 = (u8 *)gUnk_0874B835;
    TaskYieldTrampoline(t35[gUnk_02007D00[3]]);
    u2 = *c;
    u2->unk30 = 1;
    w = u2->unk34;
    if (w == 4)
        u2->unk30 = -u2->unk30;
    else if (w != 0 && RandomRange(2) != 0)
    {
        u3 = *c;
        u3->unk30 = -u3->unk30;
    }
    sub_080b2058(0);
    c2 = &gCurTask;
    u4 = *c2;
    u4->unk30 = -u4->unk30;
    sub_080b2058(1);
    u5 = *c2;
    u5->unk30 = 0;
    TaskYieldTrampoline(2);
    sub_0806395c(0);
    TaskSleepForever();
}

void sub_080b1e20(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk14 != 2)
    {
        TaskSetEntry(sub_080b1a00, gCurTaskIdx);
        return;
    }
    if (t->unk30 == 0)
    {
        if (t->unk43 == -1)
            gUnk_030023D4 = t->unk34 * 6 + 36;
        else
            gUnk_030023D4 = t->unk34 * 6 + 96;
    }
    else
    {
        if (t->unk43 == -1)
            gUnk_030023D4 = t->unk34 * 6 + 6;
        else
            gUnk_030023D4 = t->unk34 * 6 + 66;
    }
    if (gFrameCount & 1)
        gCurTask->unk3C = gUnk_030023D4 + (((gFrameCount >> 1) & 1) + 4);
    else
        gCurTask->unk3C = gUnk_030023D4 + (gFrameCount & 3);
}

void sub_080b1ef8(void)
{
    struct Task **c;
    struct Task **c2;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *u3;
    struct Task *u4;
    struct Task *u5;
    u8 *t35;
    s32 r;
    s32 w;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->unk15 = 3;
    r = RandomRange(5);
    u = *c;
    u->unk34 = r;
    u->unk30 = z;
    t35 = (u8 *)gUnk_0874B838;
    TaskYieldTrampoline(t35[gUnk_02007D00[3]]);
    u2 = *c;
    u2->unk30 = 2;
    w = u2->unk34;
    if (w == 4)
        u2->unk30 = -u2->unk30;
    else if (w != 0 && RandomRange(2) != 0)
    {
        u3 = *c;
        u3->unk30 = -u3->unk30;
    }
    sub_080b2058(0);
    c2 = &gCurTask;
    u4 = *c2;
    u4->unk30 = -u4->unk30;
    sub_080b2058(1);
    u5 = *c2;
    u5->unk30 = 0;
    TaskYieldTrampoline(1);
    sub_0806395c(0);
    TaskSleepForever();
}

void sub_080b1f80(void)
{
    struct Task *t;

    t = gCurTask;
    if (t->unk14 != 3)
    {
        TaskSetEntry(sub_080b1a00, gCurTaskIdx);
        return;
    }
    if (t->unk30 == 0)
    {
        if (t->unk43 == -1)
            gUnk_030023D4 = t->unk34 * 6 + 36;
        else
            gUnk_030023D4 = t->unk34 * 6 + 96;
    }
    else
    {
        if (t->unk43 == -1)
            gUnk_030023D4 = t->unk34 * 6 + 6;
        else
            gUnk_030023D4 = t->unk34 * 6 + 66;
    }
    if (gFrameCount & 1)
        gCurTask->unk3C = gUnk_030023D4 + (((gFrameCount >> 1) & 1) + 4);
    else
        gCurTask->unk3C = gUnk_030023D4 + (gFrameCount & 3);
}

void sub_080b2058(u8 a)
{
    struct Task **c;
    struct Task *u;
    struct Task *u2;
    u8 *tb;
    u8 *tb2;
    s32 a2;
    s32 w;
    s32 w2;
    s32 w0;
    s32 w3;
    s32 w4;

    w0 = gCurTask->unk30;
    c = &gCurTask;
    if (w0 > 0)
    {
        a2 = a << 1;
        goto xbody1;
xinc1:
        u->unk34 = u->unk34 + 1;
xbody1:
        tb = (u8 *)gUnk_0874B83B;
        w = (*c)->unk30;
        if (w < 0)
            w = -w;
        w3 = a2 + w - 1;
        TaskYieldTrampoline(tb[w3]);
        u = *c;
        if (u->unk34 <= 3)
            goto xinc1;
    }
    else
    {
        a2 = a << 1;
        goto xbody2;
xinc2:
        u2->unk34 = u2->unk34 - 1;
xbody2:
        tb2 = (u8 *)gUnk_0874B83B;
        w2 = (*c)->unk30;
        if (w2 < 0)
            w2 = -w2;
        w4 = a2 + w2 - 1;
        TaskYieldTrampoline(tb2[w4]);
        u2 = *c;
        if (u2->unk34 > 0)
            goto xinc2;
    }
}

void sub_080b20d4(void)
{
    struct Task **c;
    u32 *t40;
    struct Task *t;
    struct Task *t2;
    struct Task *u;
    struct Task *u2;
    s32 z;
    s32 v;
    u32 o;
    s32 k;

    t = gCurTask;
    z = 0;
    t->unk15 = 5;
    t2 = gCurTask;
    t2->unk2C = 11;
    t2->unk34 = z;
    c = &gCurTask;
    t40 = gUnk_0874B840;
loop:
    u = *c;
    v = u->unk2C - 1;
    u->unk2C = v;
    o = v * 4;
    u->unk54 = *(u32 *)(o + (u32)t40);
    k = u->unk43;
    u->unk58 = *(u32 *)(o + (u32)gUnk_0874B86C) * k;
    TaskYieldTrampoline(*(u32 *)(o + (u32)gUnk_0874B898));
    if ((*c)->unk2C > 0)
        goto loop;
    TaskStop();
    u2 = gCurTask;
    u2->unk4C = 0;
    u2->unk50 = 0;
    sub_0806395c(0);
    TaskSleepForever();
}

void sub_080b214c(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t2;
    vu16 *pm;

    c = &gCurTask;
    if (gCurTask->unk14 != 5)
        TaskSetEntry(sub_080b1a00, gCurTaskIdx);
    pm = (vu16 *)&gFrameCount;
    if (*pm & 1)
    {
        t = *c;
        t->unk3C = ((*pm >> 1) & 1) + 4;
    }
    else
    {
        t2 = *c;
        t2->unk3C = *pm & 3;
    }
}

void sub_080b21a0(void)
{
    struct Task *t;

    gCurTask->unk15 = 4;
    t = gCurTask;
    t->unk2C = 24;
    t->unk30 = gUnk_0874B8C4[gUnk_02007D00[3]];
    do
    {
        gCurTask->unk2C--;
        if (gCurTask->unk43 == -1)
            gCurTask->unk34 = gUnk_0874B8C8[gCurTask->unk2C];
        else
            gCurTask->unk34 = gUnk_0874B8F8[gCurTask->unk2C];
        TaskYieldTrampoline(gUnk_0874B928[gCurTask->unk2C]);
    } while (gCurTask->unk2C > 0);
    sub_0806395c(0);
    TaskSleepForever();
}

void sub_080b2228(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t3;
    vu16 *pm;

    c = &gCurTask;
    t = *c;
    t->unk30--;
    if (t->unk30 < 0)
        sub_0806395c(0);
    t3 = *c;
    if (t3->unk14 != 4)
    {
        TaskSetEntry(sub_080b1a00, gCurTaskIdx);
        return;
    }
    pm = (vu16 *)&gFrameCount;
    if (*pm & 1)
        t3->unk3C = t3->unk34 + (((*pm >> 1) & 1) + 4);
    else
        t3->unk3C = t3->unk34 + (*pm & 3);
}

void sub_080b2294(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk04 = (u32)sub_080b22b8;
    t->unk08 = 0;
    TaskStop();
    TaskSleepForever();
}

void sub_080b22b8(void)
{
    struct Task *tb;
    struct Task *t;

    tb = gTasks;
    t = gCurTask;
    if (tb[t->unk44].unk3C == -1)
        t->unk0C = 0;
    else
        t->unk0C = (u32)TaskDrawWorld;
}

void sub_080b22f8(void)
{
    struct Task **c;
    struct Task **c4;
    struct Task **c5;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *q4;
    struct Task *p1;
    struct Task *p2;
    struct Task *p3;
    u8 *b42;
    s16 *a2;
    s32 r;
    s32 w;

    c = &gCurTask;
    t = *c;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)sub_0806523c;
    t->unk38 = gUnk_087540AC;
    b42 = &t->unk42;
    z = 0;
    *b42 = 11;
    u = *c;
    u->unk04 = (u32)sub_080b2538;
    u->unk43 = 255;
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    r = RandomRange(32);
    u2 = *c;
    w = r + 1;
    a2 = (s16 *)((u8 *)u2 + 110);
    *a2 = w;
    u2->unk6C = z;
    if (z < *a2)
    {
        c4 = c;
        do
        {
            TaskSetFrame(4);
            TaskYieldTrampoline(2);
            q1 = *c4;
            q1->unk3C++;
            TaskYieldTrampoline(2);
            q2 = *c4;
            q2->unk3C++;
            TaskYieldTrampoline(2);
            q3 = *c4;
            q3->unk3C++;
            TaskYieldTrampoline(2);
            q4 = *c4;
            q4->unk6C++;
        } while ((s16)q4->unk6C < *(s16 *)((u8 *)q4 + 110));
    }
    TaskSetFrame(8);
    TaskYieldTrampoline(2);
    TaskSetFrame(13);
    TaskYieldTrampoline(2);
    TaskSetFrame(10);
    TaskYieldTrampoline(2);
    TaskSetFrame(7);
    TaskYieldTrampoline(2);
    c5 = &gCurTask;
    for (;;)
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(2);
        p1 = *c5;
        p1->unk3C++;
        TaskYieldTrampoline(2);
        p2 = *c5;
        p2->unk3C++;
        TaskYieldTrampoline(2);
        p3 = *c5;
        p3->unk3C++;
        TaskYieldTrampoline(2);
    }
}

void sub_080b2418(void)
{
    struct Task **c;
    struct Task **c4;
    struct Task **c5;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *q4;
    struct Task *p1;
    struct Task *p2;
    struct Task *p3;
    u8 *b42;
    s16 *a2;
    s32 r;
    s32 w;

    c = &gCurTask;
    t = *c;
    t->unk00 = (u32)TaskMove;
    t->unk0C = (u32)sub_0806523c;
    t->unk38 = gUnk_087540EC;
    b42 = &t->unk42;
    z = 0;
    *b42 = 11;
    u = *c;
    u->unk04 = (u32)sub_080b2538;
    u->unk43 = 255;
    TaskSetMotionXFacing(128 << 10, 0x5A5A5A5A);
    r = RandomRange(32);
    u2 = *c;
    w = r + 1;
    a2 = (s16 *)((u8 *)u2 + 110);
    *a2 = w;
    u2->unk6C = z;
    if (z < *a2)
    {
        c4 = c;
        do
        {
            TaskSetFrame(12);
            TaskYieldTrampoline(2);
            q1 = *c4;
            q1->unk3C++;
            TaskYieldTrampoline(2);
            q2 = *c4;
            q2->unk3C++;
            TaskYieldTrampoline(2);
            q3 = *c4;
            q3->unk3C++;
            TaskYieldTrampoline(2);
            q4 = *c4;
            q4->unk6C++;
        } while ((s16)q4->unk6C < *(s16 *)((u8 *)q4 + 110));
    }
    TaskSetFrame(8);
    TaskYieldTrampoline(2);
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    TaskSetFrame(10);
    TaskYieldTrampoline(2);
    TaskSetFrame(15);
    TaskYieldTrampoline(2);
    c5 = &gCurTask;
    for (;;)
    {
        TaskSetFrame(12);
        TaskYieldTrampoline(2);
        p1 = *c5;
        p1->unk3C++;
        TaskYieldTrampoline(2);
        p2 = *c5;
        p2->unk3C++;
        TaskYieldTrampoline(2);
        p3 = *c5;
        p3->unk3C++;
        TaskYieldTrampoline(2);
    }
}

void sub_080b2538(void)
{
    sub_08068e04();
    sub_08069b44();
    sub_080308e8(gUnk_0874C108);
}

void sub_080b2550(void)
{
    sub_080639a4((u32)gUnk_0874C210);
    TaskSetEntry(sub_0806a344, gCurTaskIdx);
}

void sub_080b2574(void)
{
    struct Task *t;

    sub_080261d4(4);
    t = gCurTask;
    t->unk2C = 32;
    sub_0806619c(-1, (u32)sub_080b25e8, 0, 0, 0);
}

void sub_080b25a4(void)
{
    TaskSetFrame(4);
    TaskYieldTrampoline(1);
    TaskSetFrame(5);
    TaskYieldTrampoline(1);
    TaskSetFrame(6);
    TaskYieldTrampoline(1);
    TaskSetFrame(5);
    TaskYieldTrampoline(2);
    TaskSetFrame(4);
    TaskYieldTrampoline(2);
}

void sub_080b25e8(void)
{
    struct Task *t2;
    struct Task *u;
    s32 v;

    switch (gCurTask->unk2C)
    {
    case 32:
        TaskSetFrame(9);
        break;
    case 20:
        TaskSetFrame(7);
        break;
    case 16:
        TaskSetFrame(4);
        break;
    case 0:
        sub_0806621c();
        break;
    }
    t2 = gCurTask;
    v = t2->unk30;
    t2->unk30 = v - 1;
    switch (v)
    {
    case 100:
    case 150:
        sub_080b27b0();
        break;
    case 50:
        sub_080b27b0();
        break;
    }
    u = gCurTask;
    u->unk2C--;
}

void sub_080b2664(void)
{
    struct Task **c2;
    struct Task *ta;
    struct Task *tb;
    struct Task *tc;
    struct Task *td;
    struct Task *te;
    struct Task *tf;
    struct Task *tg;
    struct Task *th;
    s32 w;

    w = gCurTask->unk34;
    c2 = &gCurTask;
    switch (w)
    {
    case 0:
        if (RandomRange(4) == 0)
        {
            ta = gCurTask;
            ta->unk34 = 1;
            sub_0806395c(2);
        }
        else
        {
            tb = gCurTask;
            tb->unk34 = 2;
            sub_0806395c(3);
        }
        TaskSetEntry(sub_080b2ac8, gCurTaskIdx);
        break;
    case 1:
        tc = *c2;
        tc->unk34 = 2;
        sub_0806395c(3);
        TaskSetEntry(sub_080b2ac8, gCurTaskIdx);
        break;
    case 2:
        if (RandomRange(4) == 0)
        {
            td = gCurTask;
            td->unk34 = 3;
            sub_0806395c(3);
        }
        else
        {
            te = gCurTask;
            te->unk34 = 0;
            sub_0806395c(1);
        }
        TaskSetEntry(sub_080b2ac8, gCurTaskIdx);
        break;
    case 3:
        tf = *c2;
        tf->unk34 = 0;
        sub_0806395c(1);
        TaskSetEntry(sub_080b2ac8, gCurTaskIdx);
        break;
    case 4:
        if (RandomRange(2) == 0)
        {
            tg = gCurTask;
            tg->unk34 = 1;
            sub_0806395c(2);
        }
        else
        {
            th = gCurTask;
            th->unk34 = 2;
            sub_0806395c(3);
        }
        TaskSetEntry(sub_080b2ac8, gCurTaskIdx);
        break;
    }
}

void sub_080b2768(void)
{
    struct ActorSpawn sp;
    s32 z;

    PlaySfx(0x223);
    sp.unk00 = 23;
    sp.unk04 = 126;
    sp.unk08 = 0;
    sp.unk09 = 0;
    z = 0;
    sp.unk0C = 24;
    sp.unk0E = 40;
    sp.unk10 = sub_08066630(8);
    sp.unk0A = z;
    sub_08064b5c(&sp, 1);
}

void sub_080b27b0(void)
{
    struct ActorSpawn sp;
    s32 z;

    sp.unk00 = 22;
    sp.unk04 = 125;
    sp.unk08 = 0;
    sp.unk09 = 0;
    sp.unk0C = (u8)sub_080b2804();
    z = 0;
    sp.unk0E = 224;
    sp.unk10 = sub_08066630(1);
    sp.unk0A = z;
    sub_08064c1c(170, sp.unk0C, sp.unk0E, 0);
    sub_08064ba8(&sp, 1);
}

s32 sub_080b2804(void)
{
    u32 v;

    switch (RandomRange(6))
    {
    case 0:
        sub_08063d7c();
        v = gUnk_030023B4;
        if (v < 32)
            return 16;
        if (v - 32 <= 31)
            return 48;
        if (v - 64 <= 31)
            return 80;
        if (v - 96 <= 31)
            return 112;
        if (v - 128 <= 126)
            return 144;
        break;
    case 1:
        return 16;
    case 2:
        return 48;
    case 3:
        return 80;
    case 4:
        return 112;
    case 5:
        return 144;
    }
}

void sub_080b2884(void)
{
    TaskSetFrameFlip(10);
}

void sub_080b2890(void)
{
    struct Task **c;
    struct Task *t0;
    struct Task *t;
    u16 *a;
    struct Task *u;
    struct Task *u2;
    struct Task *t2;
    struct Task *t3;
    u16 *a74;
    struct Task *t5;
    u32 *tbv;
    s32 a42v;
    s32 a42b;
    s32 a42c;
    u32 *tbv2;
    u32 *tbv3;
    u16 *aw;
    u16 *aw2;
    s32 w9;
    s32 w9b;
    u16 *a74b;

    c = &gCurTask;
    t0 = *c;
    if (t0->unk38 == 0)
        return;
    if (t0->unk3C == -1)
        return;
    if ((u8)sub_08066a6c() && TaskIsOnScreen())
    {
        t = *c;
        a42v = t->unk42;
        tbv = (u32 *)gUnk_087542C0;
        t5 = t;
        QueueSprite(a42v, tbv[t5->unk3C], t5->unk3E, t5->unk40,
                     t->unk48 - gSpriteCameraX,
                     (s16)((u16)t->unk4A - (u16)gSpriteCameraY));
    }
    u = gCurTask;
    a74 = (u16 *)((u8 *)u + 74);
    *a74 += 26;
    if ((u8)sub_08066a6c() && TaskIsOnScreen())
    {
        t2 = gCurTask;
        aw = (u16 *)(74 + (u32)t2);
        a = aw;
        w9 = *aw - 26;
        *a = w9;
        a42b = t2->unk42;
        tbv2 = (u32 *)gUnk_08754358;
        QueueSprite(a42b, tbv2[t2->unk3C], t2->unk3E, t2->unk40,
                     t2->unk48 - gSpriteCameraX,
                     (s16)((u16)*a - (u16)gSpriteCameraY));
    }
    u2 = gCurTask;
    a74b = (u16 *)((u8 *)u2 + 74);
    *a74b += 42;
    if ((u8)sub_08066a6c() && TaskIsOnScreen())
    {
        t3 = gCurTask;
        aw2 = (u16 *)(74 + (u32)t3);
        a = aw2;
        w9b = *aw2 - 42;
        *a = w9b;
        a42c = t3->unk42;
        tbv3 = (u32 *)gUnk_087543A0;
        QueueSprite(a42c, tbv3[t3->unk3C], t3->unk3E, t3->unk40,
                     t3->unk48 - gSpriteCameraX,
                     (s16)((u16)*a - (u16)gSpriteCameraY));
    }
}

void sub_080b2a00(void)
{
    struct Task **c;
    struct AnimCmd *k4;
    struct Task *t;
    struct Task *u;
    struct Task *u2;
    struct Task *u3;

    sub_08066088(0);
    c = &gCurTask;
    t = *c;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_080b2890;
    t->unk42 = 11;
    u = *c;
    u->unk38 = gUnk_087542C0;
    u->unk18 = 1;
    u->unk43 = 255;
    u2 = *c;
    u2->unk4C = 206 << 16;
    u2->unk50 = 128 << 17;
    k4 = (struct AnimCmd *)gUnk_0874C110;
    sub_080666f8(k4);
    sub_080664e0(k4);
    u3 = *c;
    CallTableEntry(u3->unk73, 1, gUnk_0874C12C);
}

void sub_080b2a74(void)
{
    struct Task **c;
    struct Task *u;

    c = &gCurTask;
    (*c)->unk04 = (u32)sub_080b2aa4;
    sub_0806395c(0);
    u = *c;
    CallTableEntry(u->unk14, 4, gUnk_0874C130);
}

void sub_080b2aa4(void)
{
    CallTableEntry(gCurTask->unk15, 4, gUnk_0874C140);
    sub_08068e04();
    sub_08069b44();
}

void sub_080b2ac8(void)
{
    CallTableEntry(gCurTask->unk14, 4, gUnk_0874C130);
}

void sub_080b2ae4(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t2;
    struct Task *u;
    s32 z;

    c = &gCurTask;
    t = *c;
    z = 0;
    t->unk15 = z;
    t2 = *c;
    t2->unk28 = z;
    TaskSetFrame(17);
    TaskYieldTrampoline(8);
    sub_080b25a4();
    TaskSetFrame(17);
    TaskYieldTrampoline(63);
    sub_080b25a4();
    u = *c;
    u->unk28 = 1;
    TaskSleepForever();
}

void sub_080b2b28(void)
{
    if (gCurTask->unk28 != 0)
        sub_080b2664();
}

void sub_080b2b40(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t2;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *q4;
    struct Task *q5;
    struct Task *u;
    s32 z;

    t = gCurTask;
    z = 0;
    t->unk15 = 1;
    t2 = gCurTask;
    t2->unk6C = z;
    c = &gCurTask;
    do
    {
        TaskSetFrame(14);
        TaskYieldTrampoline(1);
        q1 = *c;
        q1->unk3C++;
        TaskYieldTrampoline(1);
        q2 = *c;
        q2->unk3C++;
        TaskYieldTrampoline(8);
        sub_080b2768();
        TaskYieldTrampoline(8);
        q3 = *c;
        q3->unk3C--;
        TaskYieldTrampoline(2);
        q4 = *c;
        q4->unk3C--;
        TaskYieldTrampoline(2);
        q5 = *c;
        q5->unk6C++;
    } while ((s16)q5->unk6C <= 1);
    TaskSetFrame(14);
    TaskYieldTrampoline(8);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    u = gCurTask;
    u->unk3C--;
    TaskYieldTrampoline(2);
    sub_0806395c(0);
    TaskSleepForever();
}

void sub_080b2bf0(void)
{
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_080b2ac8, gCurTaskIdx);
}

void sub_080b2c18(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t2;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *q4;
    struct Task *q5;
    struct Task *u;
    s32 z;

    t = gCurTask;
    z = 0;
    t->unk15 = 2;
    t2 = gCurTask;
    t2->unk6C = z;
    c = &gCurTask;
    do
    {
        TaskSetFrame(14);
        TaskYieldTrampoline(1);
        q1 = *c;
        q1->unk3C++;
        TaskYieldTrampoline(1);
        q2 = *c;
        q2->unk3C++;
        TaskYieldTrampoline(8);
        sub_080b2768();
        TaskYieldTrampoline(8);
        q3 = *c;
        q3->unk3C--;
        TaskYieldTrampoline(2);
        q4 = *c;
        q4->unk3C--;
        TaskYieldTrampoline(2);
        q5 = *c;
        q5->unk6C++;
    } while ((s16)q5->unk6C <= 3);
    TaskSetFrame(14);
    TaskYieldTrampoline(8);
    TaskSetFrame(6);
    TaskYieldTrampoline(2);
    u = gCurTask;
    u->unk3C--;
    TaskYieldTrampoline(2);
    sub_0806395c(0);
    TaskSleepForever();
}

void sub_080b2cc8(void)
{
    if (gCurTask->unk14 != 2)
        TaskSetEntry(sub_080b2ac8, gCurTaskIdx);
}

void sub_080b2cf0(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t2;
    struct Task *q1;
    struct Task *q2;
    struct Task *q3;
    struct Task *q4;
    struct Task *q5;
    s32 z;

    t = gCurTask;
    z = 0;
    t->unk15 = 3;
    t2 = gCurTask;
    t2->unk30 = 150;
    t2->unk6C = z;
    c = &gCurTask;
    do
    {
        TaskSetFrame(4);
        TaskYieldTrampoline(4);
        q1 = *c;
        q1->unk3C++;
        TaskYieldTrampoline(4);
        q2 = *c;
        q2->unk3C++;
        TaskYieldTrampoline(4);
        q3 = *c;
        q3->unk3C--;
        TaskYieldTrampoline(4);
        q4 = *c;
        q4->unk3C--;
        TaskYieldTrampoline(4);
        TaskSetFrame(17);
        TaskYieldTrampoline(30);
        q5 = *c;
        q5->unk6C++;
    } while ((s16)q5->unk6C <= 2);
    sub_0806395c(0);
    TaskSleepForever();
}

void sub_080b2d80(void)
{
    struct Task *t2;
    s32 v;

    t2 = gCurTask;
    v = t2->unk30;
    t2->unk30 = v - 1;
    switch (v)
    {
    case 100:
    case 150:
        sub_080b27b0();
        break;
    case 50:
        sub_080b27b0();
        break;
    }
    if (gCurTask->unk14 != 3)
        TaskSetEntry(sub_080b2ac8, gCurTaskIdx);
}

void sub_080b2dd4(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;
    struct Task *u2;

    c = &gCurTask;
    t = *c;
    t->unk04 = (u32)sub_080b2e20;
    t->unk00 = (u32)sub_080656b4;
    t->unk42 = 11;
    u = *c;
    u->unk38 = gUnk_087542C0;
    u->unk43 = 255;
    sub_0806395c(0);
    u2 = *c;
    CallTableEntry(u2->unk14, 1, gUnk_0874C150);
}

void sub_080b2e20(void)
{
    CallTableEntry(gCurTask->unk15, 1, gUnk_0874C154);
}

void sub_080b2e3c(void)
{
    CallTableEntry(gCurTask->unk14, 1, gUnk_0874C150);
}

void sub_080b2e58(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *t2;
    struct Task *u1;
    struct Task *q1;
    struct Task *u2;
    struct Task *q2;

    c = &gCurTask;
    t = *c;
    t->unk15 = 0;
    t2 = *c;
    sub_08066fc0(1, (s16)((u16)t2->unk48 - 32), (s16)((u16)t2->unk4A + 16));
    TaskSetFrame(10);
    TaskYieldTrampoline(4);
    TaskSetFrame(11);
    TaskYieldTrampoline(4);
top:
    u1 = *c;
    u1->unk6C = 0;
    do
    {
        TaskSetFrame(12);
        TaskYieldTrampoline(4);
        TaskSetFrame(13);
        TaskYieldTrampoline(4);
        q1 = *c;
        q1->unk6C++;
    } while ((s16)q1->unk6C <= 7);
    TaskSetFrame(12);
    TaskYieldTrampoline(30);
    u2 = *c;
    u2->unk6C = 0;
    do
    {
        TaskSetFrame(12);
        TaskYieldTrampoline(50);
        TaskSetFrame(13);
        TaskYieldTrampoline(2);
        q2 = *c;
        q2->unk6C++;
    } while ((s16)q2->unk6C <= 3);
    TaskSetFrame(12);
    TaskYieldTrampoline(180);
    TaskSetFrame(13);
    TaskYieldTrampoline(60);
    TaskSetFrame(12);
    TaskYieldTrampoline(60);
    goto top;
}

void sub_080b2f34(void)
{
}

s32 sub_080b2f38(void)
{
    struct Task *t;
    s32 z;

    t = gCurTask;
    z = t->unk14;
    if (z == 0)
    {
        t->unk28 = z;
        t->unk2C = t->unk4A;
        sub_0806395c(1);
        TaskSetEntry(sub_080b3110, gCurTaskIdx);
        return 1;
    }
    return 0;
}

void sub_080b2f78(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gCurTask;
    t = *c;
    t->unk7A = 0;
    TaskSetMotionXFacing(0x1CD00, 0x5A5A5A5A);
    u = *c;
    u->unk58 = 0xFFFFC000;
    u->unk60 = 224 << 3;
}

void sub_080b2fb0(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gCurTask;
    t = *c;
    t->unk7A = 0;
    TaskSetMotionXFacing(0x1CD00, 0x5A5A5A5A);
    u = *c;
    u->unk58 = 0xFFFFC000;
    u->unk60 = 224 << 3;
}
