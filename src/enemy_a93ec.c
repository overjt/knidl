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
extern s16 gUnk_02007D48[];
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
extern u32 gUnk_02016494[];
extern u32 gUnk_020164A0[];
extern u32 gUnk_02016860[];
extern u32 gUnk_02016870[];
extern u32 gUnk_02020000[];
extern u32 gUnk_0300003C;
extern vu16 gUnk_03000B10;
extern u32 gUnk_03000B74;
extern u16 gUnk_03000F98[];
extern u32 gUnk_03000FA4;
extern u32 gUnk_0300101C[];
extern u32 gUnk_03001184[];
extern u8 gUnk_03001470[];
extern u32 gUnk_03001570[];
extern u32 gUnk_030015B0[];
extern u16 gUnk_03001EA4;
extern vu8 gUnk_03001EB0;
extern vu16 gUnk_03001EB4;
extern u32 gUnk_03001EF0[];
extern s16 gUnk_03001F00;
extern s32 gUnk_03001F2C;
extern u8 gUnk_03001F30;
extern s16 gUnk_03002158[];
extern u32 gUnk_03002160;
extern struct PlayerState gUnk_03002170[];
extern u8 gUnk_03002340;
extern s32 gUnk_03002344;
extern s16 gUnk_03002348;
extern u8 gUnk_03002350;
extern u16 gUnk_03002360;
extern s8 gUnk_0300238C;
extern s16 gUnk_03002398;
extern u16 gUnk_030023AC;
extern u8 gUnk_030023B0;
extern s32 gUnk_030023B4;
extern u32 gUnk_030023C8[];
extern s32 gUnk_030023D4;
extern u16 gUnk_030023D8;
extern s16 gUnk_030023E4;
extern u32 gUnk_030023E8[];
extern u32 gUnk_030023EC[];
extern u32 gUnk_03002448;
extern s16 gUnk_0300244C;
extern u32 gUnk_03002464[];
extern u32 gUnk_03002468[];
extern u32 gUnk_030027A8[];
extern vs16 gUnk_03004CA0[];
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
extern u32 gUnk_08756198[];

/* External functions */
extern void TaskDispatchTrampoline(void);
extern void TaskYieldTrampoline(u32 frames);
extern void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
extern s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 sub_08001cc8(s32 a, s16 b, s16 c, s32 d);
extern void sub_08002028(void);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u32 sub_08002ee8(u32 range);
extern void sub_08003014(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
extern s32 sub_08003110(s32 songId);
extern void sub_08003184(void);
extern s32 sub_080031b8(s32 id);
extern void sub_080034d0(void);
extern void sub_080034f0(s32 player, s32 songId);
extern void sub_0800374c(s32 speed);
extern void sub_08003770(u16 volume);
extern void sub_080055b0(u8 val, s32 idx);
extern void sub_08005654(s32 id);
extern void sub_080059a0(void);
extern void sub_080059d8(void);
extern void sub_080059fc(void);
extern u32 sub_08005acc(void);
extern void sub_08005d9c(void);
extern void sub_08006138(void);
extern void sub_08006148(void *a, u32 i);
extern void sub_080061a8(s32 a, s32 b, s32 c);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_08006244(void);
extern void sub_0800625c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void sub_080062c4(void);
extern void sub_08006304(void);
extern void sub_08006338(s32 a);
extern void sub_0800634c(s32 a);
extern void sub_08006364(s32 a);
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
void sub_080b603c();
void sub_080b60e8();
extern void CpuSet(const void *src, void *dst, u32 control);

s32 sub_080a93ec(void)
{
    if ((u8)sub_0806433c(gUnk_02007D00[5]) == 2)
    {
        if ((sub_08063cbc(gUnk_02007D00[5]) < 0
             ? -sub_08063cbc(gUnk_02007D00[5])
             : sub_08063cbc(gUnk_02007D00[5])) <= 15)
            return 1;
    }
    return 0;
}

void sub_080a9434(struct Task *pt)
{
    struct Task *t;

    sub_080062c4();
    t = gUnk_03002490;
    t->unk2C = 0;
    t->unk30 = 32;
    gUnk_02007D00[0] = 131;
    t->unk18 = pt->unk48;
    t->unk20 = pt->unk4A;
    gUnk_02007D00[4] = ((u16)sub_080642fc(gUnk_02007D00[5], 3) + 256) & 496;
    if (pt->unk7A != 0)
    {
        gUnk_02007D00[3] = 1;
        gUnk_03002490->unk20 |= 15;
        gUnk_02007D00[4] = (gUnk_02007D00[4] + gUnk_087490A8[sub_08002ee8(4)]) & 496;
    }
    else
        gUnk_02007D00[3] = 0;
    gUnk_03002490->unk28 = 4;
}

void sub_080a94d4(void)
{
    if (sub_08063cbc(gUnk_02007D00[5]) < 0)
        gUnk_03001F2C = 0;
    else
        gUnk_03001F2C = 1;
    if (sub_08063d18(gUnk_02007D00[5]) < 0)
        gUnk_03002448 = 0;
    else
        gUnk_03002448 = 1;
    gUnk_03002490->unk18 = gUnk_03002448;
    if (sub_08002ee8(4) != 0)
        gUnk_03002490->unk18 ^= 1;
    if (gUnk_03002490->unk18 == 0)
    {
        if (gUnk_03001F2C == 0)
            gUnk_02007D00[4] = 128 << 1;
        else
            gUnk_02007D00[4] = 0;
        gUnk_03002490->unk20 = gUnk_03001F2C;
    }
    else
    {
        if (gUnk_03002448 == 0)
            gUnk_02007D00[4] = 192 << 1;
        else
            gUnk_02007D00[4] = 128;
        gUnk_03002490->unk20 = gUnk_03002448;
    }
    sub_0806421c((s16)gUnk_02007D00[4], 320);
    gUnk_03002490->unk54 = gUnk_030023B4;
    gUnk_03002490->unk58 = gUnk_030023D4;
}

s32 sub_080a95dc(void)
{
    struct Task *t;

    if (gUnk_03002490->unk18 == 0)
    {
        if (sub_08063cbc(gUnk_02007D00[5]) < 0)
            gUnk_03001F2C = 0;
        else
            gUnk_03001F2C = 1;
        t = gUnk_03002490;
        if (t->unk20 != gUnk_03001F2C)
        {
            t->unk18 = 1;
            if (sub_08063d18(gUnk_02007D00[5]) < 0)
                gUnk_03002490->unk20 = 0;
            else
                gUnk_03002490->unk20 = 1;
            return 1;
        }
    }
    else
    {
        if (sub_08063d18(gUnk_02007D00[5]) < 0)
            gUnk_03001F2C = 0;
        else
            gUnk_03001F2C = 1;
        t = gUnk_03002490;
        if (t->unk20 != gUnk_03001F2C)
        {
            t->unk18 = 0;
            if (sub_08063cbc(gUnk_02007D00[5]) < 0)
                gUnk_03002490->unk20 = 0;
            else
                gUnk_03002490->unk20 = 1;
            return 1;
        }
    }
    return 0;
}

void sub_080a96a4(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk34 & 0x8000) {
        if (gUnk_02007D00[6] == 0) {
            gUnk_02007D00[0] = 2;
            t->unk24 = 4;
            t->unk28 = 3;
        } else {
            t->unk34 = 0;
        }
    }
}

void sub_080a96dc(void)
{
    s32 *q;
    s32 *p;
    s32 *p2;
    s32 a;
    s32 b;
    s32 g;
    s32 w;
    s32 m;
    u32 v;

    q = &gUnk_03001F2C;
    v = (u16)sub_080642fc(gUnk_02007D00[5], 3);
    *q = v;
    p = &gUnk_03002448;
    w = v - 16;
    g = gUnk_02007D00[4];
    w = g - w;
    m = 511;
    a = w & m;
    *p = a;
    p2 = &gUnk_03002344;
    v += 16;
    b = (g - v) & m;
    *p2 = b;
    if (a > b)
        gUnk_02007D00[4] = (g - 16) & m;
    else
        gUnk_02007D00[4] = (g + 16) & m;
}

void sub_080a9738(void)
{
    s32 v;

    v = ((u16)sub_08064314(3) + 32) & 0x1FF;
    gUnk_03002490->unk3C = (v >> 6) + 4;
}

s32 sub_080a9760(void)
{
    sub_0806619c(13, (u32)sub_080a9794, (u32)gUnk_082FD438, 48, 0);
    gUnk_02007D00[1] = gUnk_02007D00[0];
    gUnk_02007D00[0] = 2;
    return 0;
}

void sub_080a9794(void)
{
    if (gUnk_03001EA4 & 1)
        gUnk_03002490->unk3C = (gUnk_03002490->unk3C + 1) & 7;
    if (gUnk_02006190[3] == 0) {
        sub_0806621c();
        gUnk_02007D00[0] = gUnk_02007D00[1];
    }
}

s32 sub_080a97d8(void)
{
    sub_08002028();
    sub_080062c4();
    sub_080639a4((u32)gUnk_08749B48);
    gUnk_03002490->unk34 = 1;
    sub_080657a4();
    sub_08006148(sub_0806a344, gCurTaskIdx);
    return 1;
}

void sub_080a9814(void)
{
    gUnk_03002490->unk34 = 2;
    sub_08066fc0(0, gUnk_03002490->unk48, gUnk_03002490->unk4A);
}

void sub_080a983c(void)
{
    s32 w;
    s32 n;

    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk0C = (u32)sub_080651b4;
    gUnk_03002490->unk42 = 12;
    gUnk_03002490->unk38 = gUnk_08754418;
    gUnk_03002490->unk04 = (u32)sub_080a9910;
    sub_08063e14();
    gUnk_03002490->unk28 = gUnk_02007D00[0] & 15;
    gUnk_03002490->unk2C = 0;
    gUnk_03002490->unk30 = gUnk_087491A8[gUnk_03002490->unk28];
    gUnk_03002490->unk3C = gUnk_087491A0[0];
    while (gUnk_02007D00[0] != 4) {
        if (gUnk_03002490->unk28 != (gUnk_02007D00[0] & 15)) {
            gUnk_03002490->unk28 = gUnk_02007D00[0] & 15;
            gUnk_03002490->unk30 = gUnk_087491A8[gUnk_03002490->unk28];
        }
        w = gUnk_03002490->unk30 - 1;
        gUnk_03002490->unk30 = w;
        if (w == 0) {
            n = gUnk_03002490->unk2C + 1;
            gUnk_03002490->unk2C = n;
            if (n > 3)
                gUnk_03002490->unk2C = w;
            gUnk_03002490->unk3C = gUnk_087491A0[gUnk_03002490->unk2C];
            gUnk_03002490->unk30 = gUnk_087491A8[gUnk_03002490->unk28];
        }
        TaskYieldTrampoline(1);
    }
    gUnk_03002490->unk3C = 0xFFFF;
    sub_08006138();
}

void sub_080a9910(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gUnk_03004CA0;
    t = gUnk_03002490;
    i = t->unk44;
    if ((s16)arr[i] != -1) {
        struct Task *o = &gUnk_03002790[i];

        t->unk48 = o->unk48;
        t->unk4A = o->unk4A;
    }
    if (gUnk_02007D00[0] & 128) {
        sub_08063e14();
        sub_08006304();
        gUnk_02007D00[0] &= ~128;
    }
    if (gUnk_02007D00[0] == 0)
        sub_08006148(sub_080a99a0, gCurTaskIdx);
}

void sub_080a99a0(void)
{
    s16 *p;
    s32 d;
    s32 d2;
    s32 w;
    s32 n;

    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk0C = (u32)sub_080651b4;
    gUnk_03002490->unk42 = 12;
    gUnk_03002490->unk38 = gUnk_08754418;
    gUnk_03002490->unk04 = (u32)sub_080a9ba0;
    gUnk_03002490->unk43 = 1;
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C = 8;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C = 8;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C = 8;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 6);
    gUnk_03002490->unk28 = (d = gUnk_02007D00[0]);
    gUnk_03002490->unk2C = 0;
    gUnk_03002490->unk3C = gUnk_087491BC[0];
    p = gUnk_087491D4[d];
    gUnk_03002490->unk30 = p[0];
    for (;;) {
        if (gUnk_03002490->unk28 != gUnk_02007D00[0]) {
            d2 = gUnk_02007D00[0];
            gUnk_03002490->unk28 = d2;
            p = gUnk_087491D4[d2];
            gUnk_03002490->unk30 = p[gUnk_03002490->unk2C];
        }
        w = gUnk_03002490->unk30 - 1;
        gUnk_03002490->unk30 = w;
        if (w == 0) {
            n = gUnk_03002490->unk2C + 1;
            gUnk_03002490->unk2C = n;
            if (n > 3)
                gUnk_03002490->unk2C = w;
            gUnk_03002490->unk3C = gUnk_087491BC[gUnk_03002490->unk2C];
            gUnk_03002490->unk30 = p[gUnk_03002490->unk2C];
        }
        TaskYieldTrampoline(1);
    }
}

void sub_080a9ba0(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gUnk_03004CA0;
    t = gUnk_03002490;
    i = t->unk44;
    if ((s16)arr[i] != -1) {
        struct Task *o = &gUnk_03002790[i];

        if (o->unk76 == 6 && o->unk34 != 2) {
            t->unk48 = o->unk48;
            t->unk4A = o->unk4A;
        } else {
            sub_08005654(gCurTaskIdx);
        }
    } else {
        sub_08005654(gCurTaskIdx);
    }
}

void sub_080a9c28(void)
{

    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_080651b4;
    gUnk_03002490->unk42 = 12;
    gUnk_03002490->unk38 = gUnk_08754448;
    gUnk_03002490->unk04 = (u32)sub_080a9e14;
    gUnk_03002490->unk43 = 1;
    gUnk_03002490->unk3C = sub_08002ee8(12);
    gUnk_03002490->unk48 += sub_08002ee8(16) + (u16)(0xFFF8 + gUnk_087491E4[gUnk_03002490->unk3C]);
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    gUnk_03002490->unk46 = sub_08064cdc(198, gUnk_087491FC[gUnk_03002490->unk3C], 16, 1);
    gUnk_03002790[gUnk_03002490->unk46].unk44 = gUnk_03002490->unk44;
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

void sub_080a9cf0(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_080651b4;
    gUnk_03002490->unk42 = 12;
    gUnk_03002490->unk38 = gUnk_08754448;
    gUnk_03002490->unk04 = (u32)sub_080a9e14;
    gUnk_03002490->unk43 = 1;
    gUnk_03002490->unk3C = sub_08002ee8(12);
    gUnk_03002490->unk48 += gUnk_087491E4[gUnk_03002490->unk3C];
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    gUnk_03002490->unk46 = sub_08064cdc(199, gUnk_087491FC[gUnk_03002490->unk3C], 12, 1);
    gUnk_03002790[gUnk_03002490->unk46].unk44 = gUnk_03002490->unk44;
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

void sub_080a9da4(void)
{
    s32 r;

    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_080651b4;
    gUnk_03002490->unk42 = 10;
    gUnk_03002490->unk38 = gUnk_08754448;
    gUnk_03002490->unk04 = (u32)sub_080a9e14;
    gUnk_03002490->unk43 = 1;
    r = sub_08002ee8(3);
    gUnk_03002490->unk4C = (gUnk_03002490->unk48 + gUnk_08749214[r]) << 16;
    gUnk_03002490->unk3C = 12;
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

void sub_080a9e14(void)
{
    vs16 *arr;
    s16 i;

    arr = gUnk_03004CA0;
    i = gUnk_03002490->unk44;
    if ((s16)arr[i] != -1) {
        struct Task *o = &gUnk_03002790[i];

        if (o->unk76 == 6 && o->unk34 != 1)
            sub_08068cf8((s32)gUnk_08749774);
        else
            sub_08005654(gCurTaskIdx);
    } else {
        sub_08005654(gCurTaskIdx);
    }
}

s32 sub_080a9e88(s32 a)
{
    if (a == gUnk_03002490->unk46)
        return 0;
    return 1;
}

void sub_080a9ea4(void)
{
    vs16 *arr;
    s16 i;

    arr = gUnk_03004CA0;
    i = gUnk_03002490->unk46;
    if ((s16)arr[i] != -1)
        sub_08063f24(i);
}

void sub_080a9ed8(void)
{
    if ((u8)sub_08066a6c() != 0 && sub_08005acc() != 0)
        sub_080a9ef4();
}

void sub_080a9ef4(void)
{
    struct Task *t;
    struct TaskGfx *g;
    u32 *gt;
    u16 *nx;
    u16 *src;
    u32 dst;
    u32 x;
    s32 pal;

    t = gUnk_03002490;
    gt = t->unk38;
    if (gt == 0)
        return;
    if (t->unk3C == -1)
        return;
    x = t->unk40;
    pal = x >> 12;
    dst = ((x & 0x7FF) << 5) + 0x0600FE00;
    g = (struct TaskGfx *)gt[t->unk3C];
    src = g->unk08;
    while (src[0] != 0xFFFF) {
        nx = src + 1;
        sub_080017e4(4, (u32)nx, dst, src[0]);
        src = (u16 *)((u32)nx + src[0]);
        dst += 128 << 3;
    }
    gUnk_03001F2C = gUnk_03001EA4 & 63;
    if (gUnk_03001F2C > 31)
        gUnk_03001F2C = 64 - gUnk_03001F2C;
    gUnk_03001F2C <<= 3;
    switch (g->unk04[0]) {
    case 32:
        sub_08003014(gUnk_08334480, gUnk_08334480 + 16, (u16)gUnk_03001F2C, 16, (u16 *)((pal << 5) + (u32)gUnk_03001470));
        break;
    case 96:
        sub_08003014(gUnk_08334480, gUnk_08334480 + 16, (u16)gUnk_03001F2C, 16, (u16 *)(((pal + 2) << 5) + (u32)gUnk_03001470));
        sub_080017e4(2, (((gUnk_03001EA4 >> 1) & 1) << 5) + (u32)gUnk_083344C0, ((pal + 1) << 5) + (u32)gUnk_03001470, 32);
        sub_080017e4(2, (u32)g->unk04 + 2, (pal << 5) + (u32)gUnk_03001470, 32);
        break;
    }
    sub_08001a94(gUnk_03002490->unk42, g->unk00, gUnk_03002490->unk3E,
                 (128 << 4) | gUnk_03002490->unk40,
                 gUnk_03002490->unk48 - gUnk_03002158[0],
                 (s16)(gUnk_03002490->unk4A - gUnk_03002158[2]));
    gUnk_03001F2C = (s8)gUnk_087493A4[gUnk_03002490->unk3C];
    if (gUnk_03001F2C == -1)
        return;
    dst = 0x06014800;
    g = (struct TaskGfx *)gUnk_087546D0[gUnk_03001F2C];
    src = g->unk08;
    while (src[0] != 0xFFFF) {
        nx = src + 1;
        sub_080017e4(4, (u32)nx, dst, src[0]);
        src = (u16 *)((u32)nx + src[0]);
        dst += 128 << 3;
    }
    pal = 12;
    switch (g->unk04[0]) {
    case 64:
        pal = 11;
        sub_080017e4(2, (((gUnk_03001EA4 >> 1) & 1) << 5) + (u32)gUnk_083344C0, ((pal + 1) << 5) + (u32)gUnk_03001470, 32);
    case 32:
        sub_080017e4(2, (u32)g->unk04 + 2, (pal << 5) + (u32)gUnk_03001470, 32);
        break;
    }
    sub_08001a94(gUnk_03002490->unk42 - 1, g->unk00, gUnk_03002490->unk3E,
                 (pal << 12) | 0xA50,
                 gUnk_03002490->unk48 - gUnk_03002158[0],
                 (s16)(gUnk_03002490->unk4A - gUnk_03002158[2]));
}

void sub_080aa16c(void)
{
    if ((u8)sub_08066a6c() != 0 && sub_08005acc() != 0)
        sub_080aa188();
}

void sub_080aa188(void)
{
    struct Task *t;
    struct TaskGfx *g;
    u32 *gt;
    u16 *nx;
    u16 *src;
    u32 dst;
    u32 x;

    t = gUnk_03002490;
    gt = t->unk38;
    if (gt == 0)
        return;
    if (t->unk3C == -1)
        return;
    x = t->unk40;
    dst = ((0x7FF & x) << 5) + 0x0600FE00;
    g = (struct TaskGfx *)gt[t->unk3C];
    src = g->unk08;
    while (src[0] != 0xFFFF) {
        nx = src + 1;
        sub_080017e4(4, (u32)nx, dst, src[0]);
        src = (u16 *)((u32)nx + src[0]);
        dst += 128 << 3;
    }
    sub_080017e4(2, (((gUnk_03001EA4 >> 1) & 1) << 5) + (u32)gUnk_083344C0, ((x >> 12) << 5) + (u32)gUnk_03001470, 32);
    sub_08001a94(gUnk_03002490->unk42, g->unk00, gUnk_03002490->unk3E,
                 (128 << 4) | gUnk_03002490->unk40,
                 gUnk_03002490->unk48 - gUnk_03002158[0],
                 (s16)(gUnk_03002490->unk4A - gUnk_03002158[2]));
}

void sub_080aa288(void)
{
    struct Task *o;

    sub_08003110(34);
    o = &gUnk_03002790[sub_08063b38()];
    if (gUnk_020069F0 == 2) {
        if (o->unk7A == 0) {
            do
                TaskYieldTrampoline(1);
            while (o->unk7A == 0);
        }
    }
    sub_08066088(0);
    sub_08066144();
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_080a9ed8;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_0875456C;
    gUnk_03002490->unk8C->unk38 = 0x23E;
    gUnk_02007D00[2] = 0;
    gUnk_02007D00[3] = 1;
    gUnk_02007D00[4] = 0;
    sub_08064d34(208, 0);
    sub_08002e98(gUnk_03002490->unk73, 1, gUnk_087493F4);
}
