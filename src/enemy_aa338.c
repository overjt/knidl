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

void sub_080aa338(void)
{
    gUnk_03002490->unk04 = (u32)sub_080aa3a8;
    gUnk_03002490->unk28 = -1;
    gUnk_03002490->unk2C = 0;
    gUnk_03002490->unk30 = 0;
    gUnk_03002490->unk34 = 0;
    gUnk_03002490->unk1C = (s16)sub_08065f5c();
    gUnk_02007D00[0] = 0;
    gUnk_02007D00[1] = 0;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 12, gUnk_087493F8);
}

void sub_080aa38c(void)
{
    sub_08002e98(gUnk_03002490->unk14, 12, gUnk_087493F8);
}

void sub_080aa3a8(void)
{
    s32 n;

    sub_08002e98(gUnk_03002490->unk15, 12, gUnk_08749428);
    if ((u16)gUnk_03002490->unk3C <= 35)
    {
        gUnk_03001F2C = gUnk_08749380[gUnk_03002490->unk3C];
        sub_080639b4(gUnk_08749358[gUnk_03001F2C]);
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
                sub_080639b4((u32)gUnk_08749870);
                sub_080639f0((struct ActorAux *)gUnk_08749AF8);
            }
            else
            {
                sub_080639b4((u32)gUnk_0874988C);
                sub_080639f0((struct ActorAux *)gUnk_08749B00);
            }
            sub_08068e04();
        }
        else
        {
            gUnk_03002490->unk7C = n;
            gUnk_03002490->unk75 = n;
            gUnk_03002490->unk7E = 255;
            gUnk_03002490->unk7F = -1;
        }
    }
    sub_08069b44();
}

void sub_080aa47c(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080640c8();
    sub_080ab4a0();
    sub_080062c4();
    sub_080ab670(1);
    gUnk_03002490->unk18 = sub_080640dc((struct AnimCmd *)gUnk_08749270);
    sub_08066544();
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(15);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(15);
        gUnk_03002490->unk58 = -0x4000;
        TaskYieldTrampoline(15);
        gUnk_03002490->unk58 = 128 << 7;
        TaskYieldTrampoline(15);
        gUnk_03002490->unk58 = 128 << 8;
        TaskYieldTrampoline(15);
        gUnk_03002490->unk58 = 128 << 7;
        TaskYieldTrampoline(15);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    sub_080062c4();
    sub_0806395c(1);
    sub_08006138();
}

void sub_080aa52c(void)
{
    gUnk_03002490->unk18 = sub_08064188(gUnk_03002490->unk18);
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_080aa38c, gCurTaskIdx);
}

void sub_080aa560(void)
{
    register s32 v asm("r0");
    struct Task *t;
    s32 r1v;
    s32 r2v;

    gUnk_03002490->unk15 = 1;
    r1v = sub_080640dc((struct AnimCmd *)gUnk_08749270);
    t = gUnk_03002490;
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
    r2v = sub_08002ee8(3);
    t = gUnk_03002490;
    t->unk2C = (t->unk2C + r2v + 1) & 3;
    v = gUnk_08749220[t->unk2C];
setv:
    t->unk20 = v;
    if (gUnk_03002490->unk24 != 4)
    {
        for (;;)
        {
            gUnk_03002490->unk58 = -0x8000;
            TaskYieldTrampoline(10);
            gUnk_03002490->unk58 = -0x10000;
            TaskYieldTrampoline(10);
            gUnk_03002490->unk58 = -0x8000;
            TaskYieldTrampoline(10);
            gUnk_03002490->unk58 = 128 << 8;
            TaskYieldTrampoline(10);
            gUnk_03002490->unk58 = 128 << 9;
            TaskYieldTrampoline(10);
            gUnk_03002490->unk58 = 128 << 8;
            TaskYieldTrampoline(10);
        }
    }
    sub_08006138();
}

void sub_080aa62c(void)
{
    s32 w;

    gUnk_03002490->unk18 = sub_08064188(gUnk_03002490->unk18);
    w = gUnk_03002490->unk20 - 1;
    gUnk_03002490->unk20 = w;
    if (w == 0)
    {
        sub_080062c4();
        sub_0806395c(gUnk_08749230[gUnk_03002490->unk24][0]);
        sub_08006148(sub_080aa38c, gCurTaskIdx);
    }
}

void sub_080aa67c(void)
{
    gUnk_03002490->unk15 = 2;
    sub_080ab4a0();
    sub_080062c4();
    sub_080ab670(1);
    sub_0806395c(1);
    sub_08006138();
}

void sub_080aa6a8(void)
{
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_080aa38c, gCurTaskIdx);
}

void sub_080aa6d0(void)
{
    s32 n;

    gUnk_03002490->unk15 = 3;
    sub_080062c4();
    sub_080ab728(1);
    n = gUnk_03002490->unk34 + 1;
    gUnk_03002490->unk34 = n;
    if ((n & 3) != 0)
    {
        TaskYieldTrampoline(sub_08002ee8(31) + 30);
        sub_0806395c(2);
    }
    else
        sub_0806395c(10);
    sub_08006138();
}

void sub_080aa71c(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_080aa38c, gCurTaskIdx);
}

void sub_080aa744(void)
{
    struct Task **c;
    struct Task *u;
    s32 *pb;
    s32 w;
    register s32 q asm("r0");

    gUnk_03002490->unk15 = 4;
    sub_0806493c();
    if (gUnk_030023B4 > 128)
        gUnk_03002490->unk43 = 255;
    else
        gUnk_03002490->unk43 = 1;
    sub_080061c0(-0x30000, 0x5A5A5A5A);
    sub_08006338(36);
    gUnk_03002490->unk24 = 2;
    c = &gUnk_03002490;
    pb = &gUnk_030023B4;
top:
    u = *c;
    w = u->unk24 - 1;
    u->unk24 = w;
    if (w == 0)
    {
        u->unk3C++;
        if ((s16)u->unk3C > 39)
            u->unk3C = 36;
        (*c)->unk24 = 2;
    }
    sub_08064970();
    if (((*c)->unk43 == 1 && *pb <= 31)
        || ((*c)->unk43 == -1 && *pb > 208))
        goto out;
    TaskYieldTrampoline(1);
    goto top;
out:
    if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 111)
    {
        sub_080061c0(-0x20000, 0x5A5A5A5A);
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_080ab7cc();
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 1);
        gUnk_03002490->unk18 = sub_080640dc((struct AnimCmd *)gUnk_08749284);
        sub_080061c0(176 << 11, 0x5A5A5A5A);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk18 = sub_08064188(gUnk_03002490->unk18);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 23);
        sub_080061c0(128 << 10, 0x5A5A5A5A);
        sub_08006338(56);
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        q = gUnk_03002490->unk43;
        gUnk_03002490->unk43 = -q;
        sub_080061c0(-0x10000, 0x5A5A5A5A);
        sub_080ab7cc();
        sub_080061c0(-0x8000, 0x5A5A5A5A);
    }
    else
    {
        sub_080062c4();
        q = gUnk_03002490->unk43;
        gUnk_03002490->unk43 = -q;
        sub_08006338(56);
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        q = gUnk_03002490->unk43;
        gUnk_03002490->unk43 = -q;
        sub_080ab7cc();
    }
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_080ab810();
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 5);
    sub_080062c4();
    if (gUnk_08749224[gUnk_03002490->unk28] == 2)
        sub_0806395c(7);
    else
        sub_0806395c(6);
    sub_08006138();
}

void sub_080aa970(void)
{
    if (gUnk_03002490->unk14 != 4)
        sub_08006148(sub_080aa38c, gCurTaskIdx);
}

void sub_080aa998(void)
{
    struct ActorSpawn sp;
    s32 d;

    gUnk_03002490->unk15 = 7;
    gUnk_02007D00[7] = 0;
    sub_080640c8();
    sub_08006338(52);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    sub_08064c1c(203, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                 gUnk_03002490->unk8C->unk20 | (128 << 4));
    sub_08064c1c(204, gUnk_03002490->unk48, gUnk_03002490->unk4A, 0xD310);
    sub_080031b8(0x235);
    gUnk_03002490->unk18 = sub_080640dc((struct AnimCmd *)gUnk_087492C0);
    if (gUnk_02007D00[7] == 0)
    {
        do
            TaskYieldTrampoline(1);
        while (gUnk_02007D00[7] == 0);
    }
    gUnk_03002490->unk18 = sub_080640dc((struct AnimCmd *)gUnk_087492D4);
    sub_080031b8(140 << 2);
    gUnk_03002490->unk6C = 0;
    do
    {
        sp.unk00 = 33;
        sp.unk04 = 136;
        sp.unk09 = 0;
        sp.unk0C = 48;
        sp.unk0E = 16;
        sp.unk10 = gUnk_03002490->unk8C->unk20;
        sp.unk0A = 1;
        d = (s16)gUnk_03002490->unk78;
        if (d <= Div(gUnk_03002490->unk1C, 3) && (gUnk_03002490->unk6C & 1))
        {
            sp.unk08 = 1;
            sub_08064b5c(&sp, 1);
            sp.unk08 = 2;
            sub_08064b5c(&sp, 1);
        }
        else
        {
            sp.unk08 = 0;
            sub_08064b5c(&sp, 1);
        }
        TaskYieldTrampoline(8);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 5);
    TaskYieldTrampoline(31);
    gUnk_02007D00[7] = 2;
    sub_080640c8();
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_080ab810();
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    sub_0806395c(3);
    sub_08006138();
}

void sub_080aab4c(void)
{
    gUnk_03002490->unk18 = sub_08064188(gUnk_03002490->unk18);
    if (gUnk_03002490->unk14 != 7)
        sub_08006148(sub_080aa38c, gCurTaskIdx);
}

void sub_080aab80(void)
{
    struct ActorSpawn sp;
    register struct Task **c asm("r4");
    struct Task *u;
    register struct Task *ua asm("r1");
    s32 *d;
    s32 w;
    s32 v;
    s32 z;
    s32 *q;
    struct Task **c7;
    register struct Task *ua3 asm("r3");
    register s32 v2 asm("r2");
    register s32 *pw asm("r5");
    register struct Task **cL asm("r5");
    register s32 t1 asm("r1");
    struct Task *uA;
    struct Task *uB;
    register u8 *pch asm("r0");
    register s32 a1 asm("r1");
    register s32 a2 asm("r2");
    s32 w4;
    s32 w18;
    s32 w2;
    s32 w3;

    c = &gUnk_03002490;
    (*c)->unk15 = 6;
    sub_080640c8();
    sub_0800622c(0, 168 << 5, 192 << 10);
    sub_08006338(36);
    (*c)->unk24 = 2;
top:
    ua = *c;
    w = ua->unk24 - 1;
    ua->unk24 = w;
    if (w == 0)
    {
        w4 = (u16)ua->unk3C + 1;
        ua->unk3C = w4;
        if ((s16)w4 > 39)
            ua->unk3C = 36;
        (*c)->unk24 = 2;
    }
    sub_08064970();
    if (gUnk_030023D4 > 64)
        goto out;
    if ((u8)sub_08064358() != 1)
        goto out;
    w2 = sub_08063d2c() < 0 ? -sub_08063d2c() : sub_08063d2c();
    if (w2 > 7)
    {
        TaskYieldTrampoline(1);
        goto top;
    }
out:
    sub_080062c4();
    w18 = sub_080640dc((struct AnimCmd *)gUnk_087492AC);
    c7 = &gUnk_03002490;
    (*c7)->unk18 = w18;
    q = &gUnk_03001F2C;
    w3 = Div((*c7)->unk1C, 3);
    v2 = 0;
    ua3 = *c7;
    if ((s16)ua3->unk78 > w3)
        v2 = 1;
    *q = v2;
    pw = (s32 *)gUnk_02007D00;
    pw[6] = gUnk_0874921C[v2];
    pw[7] = gUnk_0874921E[v2];
    asm volatile("" : "+r"(v2));
    z = 0;
    pw[5] = z;
    pch = (u8 *)ua3 + 72;
    a1 = *(s16 *)pch;
    pch += 2;
    a2 = *(s16 *)pch;
    asm volatile("" : "+r"(ua3));
    pch += 66;
    sub_08064c1c(201, a1, a2, (*(struct Actor **)pch)->unk20 | (128 << 4));
    sub_08064c1c(202, (*c7)->unk48, (*c7)->unk4A, 0xD310);
    (*c7)->unk20 = z;
    (*c7)->unk6C = z;
    if (z < pw[7])
    {
        d = (s32 *)pw;
        cL = c7;
        do
        {
            TaskYieldTrampoline(d[6]);
            w = sub_08002ee8(4);
            u = *cL;
            t1 = u->unk20 + 1;
            asm("" : "+r"(t1));
            v = t1 + w;
            u->unk20 = v;
            if (v > 4)
            {
                v -= 5;
                u->unk20 = v;
            }
            sp.unk00 = 33;
            sp.unk04 = 136;
            sp.unk08 = 3;
            sp.unk0C = 0;
            sp.unk0E = 8;
            uA = *cL;
            sp.unk10 = uA->unk8C->unk20;
            sp.unk0A = 1;
            sp.unk09 = uA->unk20;
            sub_08064b5c(&sp, 1);
            uB = *cL;
            uB->unk6C++;
        } while ((s16)uB->unk6C < d[7]);
    }
    TaskYieldTrampoline(4);
    sub_080640c8();
    gUnk_02007D00[5] = 1;
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_080ab810();
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    sub_0806395c(3);
    sub_08006138();
}

void sub_080aad64(void)
{
    gUnk_03002490->unk18 = sub_08064188(gUnk_03002490->unk18);
    if (gUnk_03002490->unk14 != 6)
        sub_08006148(sub_080aa38c, gCurTaskIdx);
}

void sub_080aad98(void)
{
    struct ActorSpawn sp;

    gUnk_03002490->unk15 = 5;
    gUnk_03002490->unk18 = sub_080640dc((struct AnimCmd *)gUnk_08749270);
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk60 = -0x2000;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk58 = -0x20000;
    gUnk_03002490->unk60 = 128 << 6;
    TaskYieldTrampoline(16);
    sub_08006244();
    gUnk_03002490->unk18 = sub_080640dc((struct AnimCmd *)gUnk_08749298);
    sub_08064c1c(207, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                 gUnk_03002490->unk8C->unk20 | (128 << 4));
    sub_08064c1c(205, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                 gUnk_03002490->unk8C->unk20 | (128 << 4));
    sub_08064c1c(206, gUnk_03002490->unk48, gUnk_03002490->unk4A, 0xD310);
    sp.unk00 = 33;
    sp.unk04 = 136;
    sp.unk08 = 4;
    sp.unk10 = gUnk_03002490->unk8C->unk20;
    sp.unk0A = 1;
    if (gUnk_03002490->unk30 & 1)
    {
        TaskYieldTrampoline(20);
        sp.unk09 = 0;
        sub_08064b28(&sp, 1);
        sp.unk09 = 1;
        sub_08064b28(&sp, 1);
        TaskYieldTrampoline(16);
        sp.unk09 = 2;
        sub_08064b28(&sp, 1);
        sp.unk09 = 3;
        sub_08064b28(&sp, 1);
        TaskYieldTrampoline(16);
        sp.unk09 = 4;
        sub_08064b28(&sp, 1);
    }
    else
    {
        TaskYieldTrampoline(20);
        sp.unk09 = 4;
        sub_08064b28(&sp, 1);
        TaskYieldTrampoline(16);
        sp.unk09 = 2;
        sub_08064b28(&sp, 1);
        sp.unk09 = 3;
        sub_08064b28(&sp, 1);
        TaskYieldTrampoline(16);
        sp.unk09 = 0;
        sub_08064b28(&sp, 1);
        sp.unk09 = 1;
        sub_08064b28(&sp, 1);
    }
    gUnk_03002490->unk30++;
    sub_0806395c(3);
    sub_08006138();
}

void sub_080aaf38(void)
{
    gUnk_03002490->unk18 = sub_08064188(gUnk_03002490->unk18);
    if (gUnk_03002490->unk14 != 5)
        sub_08006148(sub_080aa38c, gCurTaskIdx);
}

void sub_080aaf6c(void)
{
    s32 d;

    gUnk_03002490->unk15 = 8;
    gUnk_03002490->unk24 = 1;
    gUnk_03002490->unk18 = sub_080640dc((struct AnimCmd *)gUnk_087492E8);
    d = (s16)gUnk_03002490->unk78;
    if (d > Div(gUnk_03002490->unk1C, 3))
    {
        TaskYieldTrampoline(60);
        gUnk_03002490->unk58 = 160 << 8;
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_080ab570();
            TaskYieldTrampoline(8);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 9);
        gUnk_03002490->unk58 = 0;
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_080ab570();
            TaskYieldTrampoline(8);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 3);
        gUnk_03002490->unk58 = -0xA000;
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_080ab570();
            TaskYieldTrampoline(8);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 9);
    }
    else
    {
        gUnk_03002490->unk60 = -0x1900;
        if (gUnk_03002490->unk4A - gUnk_03002158[2] >= -40)
        {
            do
                TaskYieldTrampoline(1);
            while (gUnk_03002490->unk4A - gUnk_03002158[2] >= -40);
        }
        sub_08006244();
        sub_080640c8();
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(30);
        gUnk_03002490->unk18 = sub_080640dc((struct AnimCmd *)gUnk_0874930C);
        gUnk_02007D00[1] = 1;
        gUnk_03002490->unk58 = 128 << 12;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk58 = 128 << 11;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk58 = 128 << 10;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk58 = 128 << 9;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk58 = 128 << 8;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk58 = 0;
        TaskYieldTrampoline(16);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk58 = -0x20000;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk58 = -0x40000;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk58 = -0x80000;
        TaskYieldTrampoline(6);
        gUnk_02007D00[1] = 0;
    }
    sub_080062c4();
    sub_0806395c(3);
    sub_08006138();
}

void sub_080ab158(void)
{
    s32 w;

    w = gUnk_03002490->unk24 - 1;
    gUnk_03002490->unk24 = w;
    if (w == 0)
    {
        sub_080031b8(0x22E);
        gUnk_03002490->unk24 = 6;
    }
    gUnk_03002490->unk18 = sub_08064188(gUnk_03002490->unk18);
    if (gUnk_03002490->unk14 != 8)
        sub_08006148(sub_080aa38c, gCurTaskIdx);
}

void sub_080ab1a8(void)
{
    register s32 q asm("r0");

    gUnk_03002490->unk15 = 9;
    TaskYieldTrampoline(24);
    sub_080031b8(0x231);
    sub_08063e14();
    gUnk_03002490->unk18 = sub_080640dc((struct AnimCmd *)gUnk_08749284);
    sub_080061c0(-0x40000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 144 << 9;
    TaskYieldTrampoline(24);
    sub_080061c0(-0x10000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 128 << 9;
    TaskYieldTrampoline(8);
    sub_080640c8();
    q = gUnk_03002490->unk43;
    gUnk_03002490->unk43 = -q;
    sub_080061c0(128 << 8, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 128 << 9;
    sub_08006338(56);
    TaskYieldTrampoline(8);
    sub_080061c0(0, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 128 << 9;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    sub_080061c0(-0x10000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 128 << 8;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk18 = sub_080640dc((struct AnimCmd *)gUnk_08749284);
    sub_080061c0(-0x30000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 128 << 8;
    TaskYieldTrampoline(4);
    sub_080061c0(-0x40000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0;
    TaskYieldTrampoline(24);
    sub_080061c0(-0x30000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = -0x10000;
    TaskYieldTrampoline(8);
    sub_080061c0(-0x30000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(8);
    sub_080640c8();
    q = gUnk_03002490->unk43;
    gUnk_03002490->unk43 = -q;
    sub_080061c0(128 << 10, 0x5A5A5A5A);
    gUnk_03002490->unk58 = -0x8000;
    sub_08006338(56);
    TaskYieldTrampoline(8);
    sub_080061c0(128 << 9, 0x5A5A5A5A);
    gUnk_03002490->unk58 = -0x10000;
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(8);
    q = gUnk_03002490->unk43;
    gUnk_03002490->unk43 = -q;
    sub_080061c0(-0x8000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = -0x10000;
    sub_080ab7cc();
    sub_080061c0(0, 0x5A5A5A5A);
    gUnk_03002490->unk58 = -0x8000;
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_080ab810();
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    sub_080062c4();
    sub_0806395c(3);
    sub_08006138();
}

void sub_080ab394(void)
{
    gUnk_03002490->unk18 = sub_08064188(gUnk_03002490->unk18);
    if (gUnk_03002490->unk14 != 9)
        sub_08006148(sub_080aa38c, gCurTaskIdx);
}

void sub_080ab3c8(void)
{
    gUnk_03002490->unk15 = 10;
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_080ab4d0(5);
        sub_080ab670(1);
        sub_080ab728(1);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    sub_0806395c(2);
    sub_08006138();
}

void sub_080ab418(void)
{
    if (gUnk_03002490->unk14 != 10)
        sub_08006148(sub_080aa38c, gCurTaskIdx);
}

void sub_080ab440(void)
{
    gUnk_03002490->unk15 = 11;
    sub_080ab5c0();
    gUnk_02007D00[0] = 0;
    sub_0806395c(3);
    sub_08006138();
}

void sub_080ab46c(void)
{
    gUnk_03002490->unk18 = sub_08064188(gUnk_03002490->unk18);
    if (gUnk_03002490->unk14 != 11)
        sub_08006148(sub_080aa38c, gCurTaskIdx);
}

void sub_080ab4a0(void)
{
    s32 n;

    n = gUnk_03002490->unk28 + 1;
    gUnk_03002490->unk28 = n;
    if (n == 10)
        gUnk_03002490->unk28 = 0;
    sub_080ab4d0(gUnk_08749224[gUnk_03002490->unk28]);
}

void sub_080ab4d0(s32 a)
{
    gUnk_03002490->unk48 = gUnk_03002158[0] + 128;
    if (gUnk_08749244[a] != 0)
        gUnk_03002490->unk48 += sub_08002ee8(gUnk_08749244[a] + 1) - (gUnk_08749244[a] >> 1);
    gUnk_03002490->unk4A = gUnk_08749252[a] + gUnk_03002158[2];
    if (gUnk_08749260[a] != 0)
        gUnk_03002490->unk4A += sub_08002ee8(gUnk_08749260[a] + 1) - (gUnk_08749260[a] >> 1);
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
}

void sub_080ab570(void)
{
    s32 v;

    if ((u8)sub_08064398() == 4)
    {
        v = gUnk_03002490->unk54 + (128 << 7);
        gUnk_03002490->unk54 = v;
        if (v > 192 << 9)
            gUnk_03002490->unk54 = 192 << 9;
    }
    else
    {
        v = gUnk_03002490->unk54 - 0x4000;
        gUnk_03002490->unk54 = v;
        if (v < -0x18000)
            gUnk_03002490->unk54 = -0x18000;
    }
}

void sub_080ab5c0(void)
{
    sub_080062c4();
    if (gUnk_02007D00[1] == 0)
        gUnk_03002490->unk18 = sub_080640dc((struct AnimCmd *)gUnk_08749330);
    else
    {
        gUnk_03002490->unk18 = sub_080640dc((struct AnimCmd *)gUnk_08749344);
        gUnk_02007D00[1] = 0;
    }
    sub_080061c0(-0x40000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    sub_080061c0(-0x20000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    sub_080061c0(-0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    sub_080061c0(-0x8000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    sub_080061c0(-0x4000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    gUnk_03002490->unk54 = 0;
}

void sub_080ab670(s32 a)
{
    gUnk_03002490->unk0C = (u32)sub_08065438;
    if (a != 0)
        sub_080031b8(141 << 2);
    sub_08006338(67);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk0C = (u32)sub_080a9ed8;
    sub_08006338(50);
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(10);
}

void sub_080ab728(s32 a)
{
    sub_080062c4();
    gUnk_03002490->unk0C = (u32)sub_08065438;
    if (a != 0)
        sub_080031b8(0x233);
    sub_08006338(60);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0xFFFF;
}

void sub_080ab7cc(void)
{
    sub_08006338(44);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
}

void sub_080ab810(void)
{
    sub_08006338(36);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
}

s32 sub_080ab854(void)
{
    gUnk_02007D00[0] |= 1;
    gUnk_03002490->unk43 = sub_08063dac(gUnk_03002490->unk7F);
    sub_0806caa0(1, 0, 0);
    sub_0806395c(11);
    sub_08006148(sub_080aa38c, gCurTaskIdx);
    return 1;
}

s32 sub_080ab8a8(void)
{
    sub_080062c4();
    gUnk_02007FB8[0] = 0;
    gUnk_03002490->unk43 = sub_08063dac(gUnk_03002490->unk7F);
    sub_080639a4((u32)gUnk_08749B60);
    gUnk_02007D00[0] |= 2;
    if (gUnk_030023D8 == 20)
    {
        sub_0800a698();
        sub_080b7cb4(gUnk_030023E8[0]);
    }
    if (gUnk_02007D00[1] != 0)
    {
        gUnk_03002490->unk38 = gUnk_08754568;
        sub_08063e14();
        sub_08006338(0);
    }
    sub_08006148(sub_0806a344, gCurTaskIdx);
    return 1;
}

void sub_080ab93c(void)
{
    register u16 x asm("r0");

    gUnk_02007FB8[0] = 0;
    gUnk_03002490->unk0C = (u32)sub_080a9ed8;
    gUnk_03002490->unk04 = (u32)sub_080abcb4;
    gUnk_03002490->unk38 = gUnk_0875456C;
    gUnk_03002490->unk42 = 11;
    sub_080ab5c0();
    sub_080640c8();
    sub_080ab728(0);
    sub_080ab4d0(6);
    sub_080062c4();
    sub_080ab670(0);
    gUnk_03002490->unk0C = (u32)sub_08065438;
    sub_080031b8(143 << 2);
    sub_080261d4(7);
    gUnk_03002490->unk3E &= 0x7FFF;
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 76;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 9);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 78;
        TaskYieldTrampoline(2);
        x = gUnk_03002490->unk3C;
        gUnk_03002490->unk3C = x | 0xFFFF;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 79;
        TaskYieldTrampoline(2);
        x = gUnk_03002490->unk3C;
        gUnk_03002490->unk3C = x | 0xFFFF;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 9);
    sub_08064c1c(209, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                 gUnk_03002490->unk8C->unk20);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 80;
        TaskYieldTrampoline(2);
        x = gUnk_03002490->unk3C;
        gUnk_03002490->unk3C = x | 0xFFFF;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 81;
        TaskYieldTrampoline(2);
        x = gUnk_03002490->unk3C;
        gUnk_03002490->unk3C = x | 0xFFFF;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 9);
    sub_080261d4(6);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 82;
        TaskYieldTrampoline(2);
        x = gUnk_03002490->unk3C;
        gUnk_03002490->unk3C = x | 0xFFFF;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 85;
        TaskYieldTrampoline(2);
        x = gUnk_03002490->unk3C;
        gUnk_03002490->unk3C = x | 0xFFFF;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 14);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 83;
        TaskYieldTrampoline(2);
        x = gUnk_03002490->unk3C;
        gUnk_03002490->unk3C = x | 0xFFFF;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 84;
        TaskYieldTrampoline(2);
        x = gUnk_03002490->unk3C;
        gUnk_03002490->unk3C = x | 0xFFFF;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 7);
    sub_080261d4(5);
    gUnk_03002490->unk3C = 86;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 87;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = -1;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 88;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = -1;
    sub_080261d4(2);
    TaskYieldTrampoline(180);
    if (gUnk_030023D8 == 20)
        goto far;
    {
        sub_080261d4(7);
        sub_080abcd0(gUnk_03002158[0] + 80, gUnk_03002158[2] + 136, 30);
        sub_080abcd0(gUnk_03002158[0] + 224, gUnk_03002158[2] + 136, 15);
        sub_080abcd0(gUnk_03002158[0] + 208, gUnk_03002158[2] + 136, 0);
        sub_080abcd0(gUnk_03002158[0] + 48, gUnk_03002158[2] + 136, 30);
        sub_080abcd0(gUnk_03002158[0] + 192, gUnk_03002158[2] + 136, 10);
        sub_080abcd0(gUnk_03002158[0] + 112, gUnk_03002158[2] + 136, 10);
        sub_080abcd0(gUnk_03002158[0] + 32, gUnk_03002158[2] + 136, 10);
        sub_080abcd0(gUnk_03002158[0] + 216, gUnk_03002158[2] + 136, 45);
        sub_080abcd0(gUnk_03002158[0] + 80, gUnk_03002158[2] + 136, 0);
        sub_080abcd0(gUnk_03002158[0] + 200, gUnk_03002158[2] + 136, 30);
        sub_080abcd0(gUnk_03002158[0] + 224, gUnk_03002158[2] + 136, 0);
        sub_080261d4(0);
    }
    goto fin;
far:
    TaskYieldTrampoline(180);
fin:
    sub_08025b5c();
    TaskDispatchTrampoline();
}

void sub_080abcb4(void)
{
    gUnk_03002490->unk18 = sub_08064188(gUnk_03002490->unk18);
}

void sub_080abcd0(s32 x, s32 y, s32 d)
{
    sub_080031b8(189);
    sub_08064d6c(148, (s16)x, (s16)y, 0);
    if (d != 0)
        TaskYieldTrampoline(d);
}

void sub_080abd04(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_080651b4;
    gUnk_03002490->unk42 = 10;
    gUnk_03002490->unk38 = gUnk_08754738;
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 16;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 9);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 13;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 14;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 9);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C = 14;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 13;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 9);
    gUnk_03002490->unk3C = 15;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 16;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

void sub_080abe38(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_080651b4;
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk38 = gUnk_08754738;
    gUnk_03002490->unk04 = (u32)sub_080abe7c;
    sub_08064a60();
    sub_08006338(9);
    sub_08006138();
}

void sub_080abe7c(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gUnk_03004CA0;
    t = gUnk_03002490;
    i = t->unk44;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gUnk_03002790[i];

        if (o->unk76 == 8 && gUnk_02007D00[0] == 0 && gUnk_02007D00[5] == 0)
        {
            t->unk4C = o->unk48 << 16;
            t->unk50 = o->unk4A << 16;
        }
        else
            sub_08005654(gCurTaskIdx);
    }
    else
        sub_08005654(gCurTaskIdx);
}

void sub_080abf10(void)
{
    s32 w;

    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk0C = (u32)sub_080aa16c;
    gUnk_03002490->unk42 = 12;
    gUnk_03002490->unk38 = gUnk_08754708;
    gUnk_03002490->unk04 = (u32)sub_080abf94;
    sub_08064a60();
    gUnk_03002490->unk28 = 2;
    sub_08006338(0);
    while (gUnk_02007D00[5] == 0)
    {
        w = gUnk_03002490->unk28 - 1;
        gUnk_03002490->unk28 = w;
        if (w == 0)
        {
            gUnk_03002490->unk28 = 2;
            gUnk_03002490->unk3C++;
            if ((s16)gUnk_03002490->unk3C > 3)
                gUnk_03002490->unk3C = w;
        }
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}

void sub_080abf94(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gUnk_03004CA0;
    t = gUnk_03002490;
    i = t->unk44;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gUnk_03002790[i];

        if (o->unk76 == 8 && gUnk_02007D00[0] == 0)
        {
            t->unk48 = o->unk48;
            t->unk4A = o->unk4A;
        }
        else
            sub_08005654(gCurTaskIdx);
    }
    else
        sub_08005654(gCurTaskIdx);
}

void sub_080ac020(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_080651b4;
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk38 = gUnk_08754738;
    gUnk_03002490->unk04 = (u32)sub_080ac08c;
    sub_08064a60();
    sub_08006338(10);
    TaskYieldTrampoline(85);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_02007D00[7] = 1;
    sub_08006338(12);
    sub_08006138();
}

void sub_080ac08c(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gUnk_03004CA0;
    t = gUnk_03002490;
    i = t->unk44;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gUnk_03002790[i];

        if (o->unk76 == 8 && gUnk_02007D00[0] == 0)
        {
            if (gUnk_02007D00[7] == 1)
            {
                t->unk4C = o->unk48 << 16;
                t->unk50 = o->unk4A << 16;
            }
            else if (gUnk_02007D00[7] == 2)
                sub_08005654(gCurTaskIdx);
        }
        else
            sub_08005654(gCurTaskIdx);
    }
    else
        sub_08005654(gCurTaskIdx);
}

void sub_080ac124(void)
{
    s32 w;
    s32 w2;

    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk0C = (u32)sub_080aa16c;
    gUnk_03002490->unk42 = 9;
    gUnk_03002490->unk38 = gUnk_08754718;
    gUnk_03002490->unk04 = (u32)sub_080ac1f0;
    sub_08064a60();
    gUnk_03002490->unk28 = 2;
    sub_08006338(0);
    while (gUnk_02007D00[7] == 0)
    {
        w = gUnk_03002490->unk28 - 1;
        gUnk_03002490->unk28 = w;
        if (w == 0)
        {
            gUnk_03002490->unk28 = 2;
            gUnk_03002490->unk3C++;
            if ((s16)gUnk_03002490->unk3C > 3)
                gUnk_03002490->unk3C = w;
        }
        TaskYieldTrampoline(1);
    }
    gUnk_03002490->unk28 = 2;
    sub_08006338(4);
    while (gUnk_02007D00[7] == 1)
    {
        w2 = gUnk_03002490->unk28 - 1;
        gUnk_03002490->unk28 = w2;
        if (w2 == 0)
        {
            gUnk_03002490->unk28 = 2;
            gUnk_03002490->unk3C++;
            if ((s16)gUnk_03002490->unk3C > 7)
                gUnk_03002490->unk3C = 4;
        }
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}

void sub_080ac1f0(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gUnk_03004CA0;
    t = gUnk_03002490;
    i = t->unk44;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gUnk_03002790[i];

        if (o->unk76 == 8 && gUnk_02007D00[0] == 0)
        {
            t->unk48 = o->unk48;
            t->unk4A = o->unk4A;
        }
        else
            sub_08005654(gCurTaskIdx);
    }
    else
        sub_08005654(gCurTaskIdx);
}

void sub_080ac27c(void)
{
    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk0C = (u32)sub_080651b4;
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk38 = gUnk_08754738;
    gUnk_03002490->unk04 = (u32)sub_080ac30c;
    sub_08064a60();
    sub_08006338(4);
    gUnk_03002490->unk28 = 0;
    for (;;)
    {
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk28--;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 2);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk28++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 2);
    }
}

void sub_080ac30c(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gUnk_03004CA0;
    t = gUnk_03002490;
    i = t->unk44;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gUnk_03002790[i];

        if (o->unk76 == 8 && o->unk14 == 5 && gUnk_02007D00[0] == 0)
        {
            t->unk48 = o->unk48;
            t->unk4A = o->unk4A + t->unk28;
        }
        else
            sub_08005654(gCurTaskIdx);
    }
    else
        sub_08005654(gCurTaskIdx);
}

void sub_080ac3a4(void)
{
    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk0C = (u32)sub_080651b4;
    gUnk_03002490->unk42 = 8;
    gUnk_03002490->unk38 = gUnk_08754738;
    gUnk_03002490->unk04 = (u32)sub_080ac30c;
    gUnk_03002490->unk28 = 0;
    sub_08064a60();
    for (;;)
    {
        sub_08006338(5);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
    }
}

void sub_080ac410(void)
{
    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk0C = (u32)sub_080aa16c;
    gUnk_03002490->unk42 = 9;
    gUnk_03002490->unk38 = gUnk_087546F8;
    gUnk_03002490->unk04 = (u32)sub_080ac47c;
    sub_08064a60();
    for (;;)
    {
        sub_08006338(0);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
    }
}

void sub_080ac47c(void)
{
    vs16 *arr;
    struct Task *t;
    s16 i;

    arr = gUnk_03004CA0;
    t = gUnk_03002490;
    i = t->unk44;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gUnk_03002790[i];

        if (o->unk76 == 8 && o->unk14 == 5 && gUnk_02007D00[0] == 0)
        {
            t->unk48 = o->unk48;
            t->unk4A = o->unk4A;
        }
        else
            sub_08005654(gCurTaskIdx);
    }
    else
        sub_08005654(gCurTaskIdx);
}

void sub_080ac510(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_080ac530;
    sub_08006138();
}

void sub_080ac530(void)
{
    vs16 *arr;
    register struct Task *t asm("ip");
    struct Task *o;
    s16 i;
    struct Task *u;
    s32 v;
    s32 w;
    register s32 wv0 asm("r0");

    arr = gUnk_03004CA0;
    t = gUnk_03002490;
    i = t->unk44;
    if ((s16)arr[i] != -1)
    {
        o = &gUnk_03002790[i];

        if (o->unk76 == 8 && gUnk_02007D00[0] != 2)
        {
            if (o->unk3C == -1)
                return;
            t->unk48 = o->unk48;
            wv0 = *(u16 *)((u8 *)o + 74);
    t->unk4A = wv0;
            t->unk43 = o->unk43;
            gUnk_03002490->unk28 = gUnk_087494C8[o->unk3C];
            if (gUnk_03002490->unk28 == -1)
                return;
            sub_08068cf8((s32)gUnk_08749458[gUnk_03002490->unk28]);
            u = gUnk_03002490;
            v = u->unk7C;
            if (v == 6 && u->unk82 == 9)
            {
                o = &gUnk_03002790[u->unk7E];
                o->unk7C = v;
            }
            else
            {
                w = gUnk_08749490[gUnk_03002490->unk28];
                if (w == 0)
                    return;
                sub_08068cf8(w);
                u = gUnk_03002490;
                v = u->unk7C;
                if (v == 6 && u->unk82 == 9)
                {
                    o = &gUnk_03002790[u->unk7E];
                    o->unk7C = v;
                }
            }
        }
        else
            sub_08005654(gCurTaskIdx);
    }
    else
        sub_08005654(gCurTaskIdx);
}

s32 sub_080ac678(void)
{
    sub_08006338(0);
}

void sub_080ac684(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk38 = gUnk_08753DA0;
    sub_08064a60();
    t = gUnk_03002490;
    switch (t->unk73)
    {
    case 0:
        t->unk42 = 12;
        gUnk_03002490->unk04 = (u32)sub_080ac72c;
        sub_0806395c(0);
        break;
    case 1:
        t->unk42 = 3;
        gUnk_03002490->unk04 = (u32)sub_080ac82c;
        sub_0806395c(1);
        break;
    case 2:
        t->unk42 = 12;
        gUnk_03002490->unk04 = (u32)sub_080ac84c;
        sub_0806395c(2);
        break;
    }
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08749B8C);
}

void sub_080ac72c(void)
{
    struct PlayerState *p;
    register struct Task **c asm("r4");
    struct Task *u;
    struct Task *u2;
    u16 *pu;
    u8 *pPb;
    struct Task *u3;
    register s32 *d asm("r4");
    register u16 *ac asm("r6");
    register u16 *acA asm("r0");
    register struct Task **c5 asm("r5");
    s32 n;
    s32 dd;
    s32 k;

    sub_0806956c();
    c = &gUnk_03002490;
    sub_08002e98((*c)->unk15, 3, gUnk_08749B98);
    if ((*c)->unk28 > 0)
        sub_08068e04();
    u2 = *c;
    if (u2->unk7C == 7)
    {
        pu = (u16 *)((u8 *)u2 + 112);
        n = 0;
        *pu = n;
        acA = &gUnk_030023AC;
        asm("" : "+r"(acA));
        c5 = c;
        ac = acA;
        if (n >= *ac)
            goto post;
        d = gUnk_02007D00;
lp:
        asm("" ::: "memory");
        u = *c5;
        dd = u->unk7F;
        k = (s16)u->unk70;
        if ((dd >> k) & 1)
        {
            pPb = (u8 *)gUnk_03002170;
            asm("" : "+r"(pPb));
            p = (struct PlayerState *)(116 * k + (u32)pPb);
            if (p->unk0D == 24)
            {
                if (gUnk_03002790[k].unk73 <= 6)
                    goto skip;
            }
            else
            {
                if (p->unk42 & 2)
                    goto skip;
                if (p->unk04 == 13)
                    goto skip;
                if (p->unk04 == 10)
                    goto skip;
            }
            sub_08040858(k);
            d[2]++;
            goto post;
        }
skip:
        u3 = *c5;
        u3->unk70++;
        if ((s16)u3->unk70 < *ac)
            goto lp;
post:
        if ((s16)gUnk_03002490->unk70 != gUnk_030023AC)
            sub_08063fe0();
    }
}

void sub_080ac82c(void)
{
    sub_0806956c();
    sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08749B98);
}

void sub_080ac84c(void)
{
    sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08749B98);
}

void sub_080ac868(void)
{
    struct ActorSpawn sp;
    s32 w;
    u8 v74;

    gUnk_03002490->unk15 = 0;
    sub_0800622c(0, 168 << 5, 192 << 10);
    gUnk_03002490->unk28 = 0;
    gUnk_03002490->unk7A = 0;
    gUnk_03002490->unk2C = 2;
    sub_08006338(10);
    while (gUnk_03002490->unk7A == 0)
    {
        TaskYieldTrampoline(1);
        w = gUnk_03002490->unk2C - 1;
        gUnk_03002490->unk2C = w;
        if (w == 0)
        {
            gUnk_03002490->unk2C = 2;
            gUnk_03002490->unk3C++;
            if ((s16)gUnk_03002490->unk3C > 11)
                gUnk_03002490->unk3C = 4;
        }
    }
    sub_080062c4();
    sub_08006338(12);
    gUnk_03002490->unk28 = 1;
    v74 = gUnk_03002490->unk74;
    if (v74 == 0)
    {
        sp.unk00 = 18;
        sp.unk04 = 120;
        sp.unk08 = 2;
        sp.unk09 = v74;
        sp.unk0C = gUnk_03002158[0] + 120;
        sp.unk0E = gUnk_03002158[2] + 72;
        sp.unk10 = (128 << 5) + gUnk_03002490->unk40;
        sp.unk0A = 0;
        sub_08064ba8(&sp, 1);
    }
    sub_08006138();
}

void sub_080ac94c(void)
{
}

void sub_080ac950(void)
{
    s32 w;

    gUnk_03002490->unk15 = 1;
    if ((sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()) <= 15)
    {
        sub_0806493c();
        if (gUnk_030023B4 <= 119)
            gUnk_03002490->unk54 = 128 << 8;
        else
            gUnk_03002490->unk54 = -0x8000;
    }
    sub_0800622c(-0x40000, 192 << 6, 192 << 10);
    gUnk_03002490->unk7A = 0;
    gUnk_03002490->unk28 = 2;
    sub_08006338(22);
    while (gUnk_03002490->unk7A == 0)
    {
        TaskYieldTrampoline(1);
        w = gUnk_03002490->unk28 - 1;
        gUnk_03002490->unk28 = w;
        if (w == 0)
        {
            gUnk_03002490->unk28 = 2;
            gUnk_03002490->unk3C++;
            if ((s16)gUnk_03002490->unk3C > 23)
                gUnk_03002490->unk3C = 16;
        }
        if (gUnk_03002490->unk58 > 0)
            gUnk_03002490->unk42 = 12;
    }
    sub_080062c4();
    sub_08006338(24);
    sub_08006138();
}

void sub_080aca38(void)
{
}

void sub_080aca3c(void)
{
    gUnk_03002490->unk15 = 2;
    gUnk_03002490->unk3C = 35;
    TaskYieldTrampoline(216);
    TaskDispatchTrampoline();
}

void sub_080aca60(void)
{
    if ((s16)gUnk_03004CA0[gUnk_03002490->unk44] == -1)
        sub_08063fe0();
}

void sub_080aca90(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 12;
    gUnk_03002490->unk38 = gUnk_0875447C;
    sub_08063e14();
    gUnk_02007D00[2]++;
    gUnk_03002490->unk04 = (u32)sub_080acaf0;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08749BA4);
}

void sub_080acaf0(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08749BA8);
    sub_08068e04();
    sub_08069b44();
}

void sub_080acb20(void)
{
    s32 w;

    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk7A = 0;
    sub_0800622c(0, 148 << 10, 192 << 10);
    sub_080061c0(192 << 8, 0x5A5A5A5A);
    gUnk_03002490->unk28 = 6;
    sub_08006338(11);
    while (gUnk_03002490->unk7A == 0)
    {
        w = gUnk_03002490->unk28 - 1;
        gUnk_03002490->unk28 = w;
        if (w == 0)
        {
            gUnk_03002490->unk3C++;
            if ((s16)gUnk_03002490->unk3C > 13)
                gUnk_03002490->unk3C = 11;
            gUnk_03002490->unk28 = 6;
        }
        TaskYieldTrampoline(1);
    }
    sub_08006244();
    sub_080061c0(128 << 9, 0x5A5A5A5A);
    for (;;)
    {
        sub_08006338(4);
        TaskYieldTrampoline(4);
        sub_08006338(8);
        TaskYieldTrampoline(6);
        sub_08006338(10);
        TaskYieldTrampoline(10);
        sub_08006338(9);
        TaskYieldTrampoline(6);
        sub_08006338(4);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
    }
}

void sub_080acc18(void)
{
    vs16 *arr;
    s16 i;

    arr = gUnk_03004CA0;
    i = gUnk_03002490->unk44;
    if ((s16)arr[i] != -1)
    {
        struct Task *o = &gUnk_03002790[i];

        if (o->unk76 == 6 && o->unk34 == 0)
            return;
        sub_08006148(sub_0806a344, gCurTaskIdx);
    }
    else
        sub_08006148(sub_0806a344, gCurTaskIdx);
}

void sub_080acc8c(void)
{
    gUnk_02007D00[2]--;
}

void sub_080acc9c(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 9;
    gUnk_03002490->unk38 = gUnk_08754738;
    gUnk_03002490->unk04 = (u32)sub_080accf0;
    gUnk_03002490->unk28 = 0;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08749BE4);
}

void sub_080accf0(void)
{
    if ((u8)sub_0806951c() == 1)
        sub_08006148(sub_0806a344, gCurTaskIdx);
    else
    {
        sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08749BE8);
        sub_08068e04();
        sub_08069b44();
    }
}

void sub_080acd38(void)
{
    struct Task *t;
    s32 w;
    s32 a2;
    register struct Task *u asm("r1");
    struct Task *u2;
    register struct Task **c asm("r0");
    struct Task **c2;
    struct Task **c3;

    gUnk_03002490->unk15 = 0;
    sub_08064a60();
    gUnk_03002490->unk30 = sub_080640dc((struct AnimCmd *)gUnk_08749BD0);
    gUnk_03002490->unk7A = 0;
    t = gUnk_03002490;
    if (t->unk73 == 3)
    {
        a2 = gUnk_08749BB1[t->unk74] + (160 << 2);
        t->unk2C = (a2 - (t->unk43 << 7)) & 0x1FF;
        sub_0806421c(t->unk2C, 128 << 3);
        u = gUnk_03002490;
        u->unk54 = gUnk_030023B4;
        u->unk58 = gUnk_030023D4;
        sub_080031b8(0x22F);
    }
    else if (t->unk73 == 4)
    {
        sub_0806421c(gUnk_08749BAC[t->unk74], 128 << 3);
        u = gUnk_03002490;
        u->unk54 = gUnk_030023B4;
        u->unk58 = gUnk_030023D4;
        sub_080031b8(0x22F);
    }
    else
    {
        sub_080061c0(gUnk_08749BB8[t->unk73], 0x5A5A5A5A);
        u2 = gUnk_03002490;
        u2->unk58 = gUnk_08749BC4[u2->unk73];
    }
    TaskYieldTrampoline(255);
    c = &gUnk_03002490;
    u = *c;
    c2 = c;
    u->unk34 = 255;
    c3 = c2;
    do
    {
        if (((*c3)->unk34 & 3) == 0)
            sub_080645a4(154 << 7, 0x18100);
        TaskYieldTrampoline(1);
        u = *c2;
        w = u->unk34 - 1;
        u->unk34 = w;
    } while (w != 0);
    gUnk_03002490->unk28++;
    sub_08006138();
}

void sub_080ace60(void)
{
    gUnk_03002490->unk30 = sub_08064188(gUnk_03002490->unk30);
    if (gUnk_02007D00[0] == 2)
        gUnk_03002490->unk28++;
    if (gUnk_03002490->unk28 != 0)
        sub_08006148(sub_0806a344, gCurTaskIdx);
}

void sub_080acea8(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08065438;
    gUnk_03002490->unk38 = gUnk_08753994;
    gUnk_03002490->unk42 = 11;
    sub_08066088(0);
    sub_08063a00((u32)gUnk_0874B3A8);
    gUnk_03002490->unk34 = 0;
    gUnk_02007D00[0] = 1;
    gUnk_02007D00[2] = 1;
    gUnk_02007D00[3] = 0x10001;
    gUnk_02007D00[4] = 0;
    gUnk_02007D00[5] = -1;
    gUnk_02007D00[9] = sub_08065f5c();
    sub_080ad32c();
    sub_0806395c(0);
    gUnk_03002490->unk28 = 0;
    gUnk_03002490->unk04 = (u32)sub_080acf3c;
    sub_080664e0((struct AnimCmd *)gUnk_08749CEC);
    sub_0806395c(1);
    sub_08006138();
}

void sub_080acf3c(void)
{
    sub_080ad0f8();
}

void sub_080acf48(void)
{
    struct Task *t = gUnk_03002490;
    register s32 v asm("r1");
    register s32 v2 asm("r0");

    v = t->unk14;
    t->unk28 = v;
    v2 = v;
    sub_08002e98(v2, 3, gUnk_08749D10);
}

void sub_080acf68(void)
{
    gUnk_03002490->unk04 = (u32)sub_080ad0f8;
    gUnk_02007D00[5] = -1;
    sub_080ad32c();
    gUnk_02007D00[1] = 0;
    gUnk_03002490->unk30 = 16;
    sub_0806cc90(1, 3, 8, 10);
    while (gUnk_03002490->unk30 > 0)
        sub_080ad278();
    sub_080ad170();
    while (gUnk_03002490->unk30 > 0)
        sub_080ad278();
    sub_080062c4();
    sub_080ad32c();
    if (sub_080ad37c() != 0)
    {
        gUnk_02007D00[5] = 2;
        sub_080ad08c();
    }
    sub_0806395c(2);
    sub_08006138();
}

void sub_080acffc(void)
{
    s32 w;

    gUnk_02007D00[5] = -1;
    gUnk_02007D00[1] = 7;
    gUnk_03002490->unk30 = 10;
    while (gUnk_03002490->unk30 > 0)
        sub_080ad278();
    if (sub_080ad37c() != 0)
        TaskYieldTrampoline(32);
    sub_080ad3e8();
    gUnk_02007D00[1] = 8;
    gUnk_03002490->unk30 = 27;
    while (gUnk_03002490->unk30 > 0)
        sub_080ad278();
    sub_080ad3a0();
    gUnk_02007D00[5] = 1;
    if (gUnk_03002490->unk30 > 0)
    {
        do
        {
            sub_080ad08c();
            w = gUnk_03002490->unk30 - 1;
            gUnk_03002490->unk30 = w;
        } while (w > 0);
    }
    sub_0806395c(1);
    sub_08006138();
}

void sub_080ad08c(void)
{
    sub_080062c4();
    sub_08006338(20);
    TaskYieldTrampoline(4);
    sub_08006338(22);
    TaskYieldTrampoline(3);
    sub_08006338(24);
    TaskYieldTrampoline(4);
    sub_08006338(26);
    TaskYieldTrampoline(11);
    sub_08006338(24);
    TaskYieldTrampoline(4);
    sub_08006338(22);
    TaskYieldTrampoline(3);
    sub_08006338(20);
    TaskYieldTrampoline(4);
    sub_08006338(18);
    TaskYieldTrampoline(11);
}

void sub_080ad0f8(void)
{
    struct Task *t;

    sub_08068f68();
    sub_08069b44();
    t = gUnk_03002490;
    if (t->unk28 != t->unk14)
        sub_08006148(sub_080acf48, gCurTaskIdx);
}

void sub_080ad128(void)
{
    sub_08064d34(142, 0);
    sub_080ad458();
}

s32 sub_080ad13c(void)
{
    sub_080639a4((u32)gUnk_0874B4EC);
    sub_08006148(sub_0806a344, gCurTaskIdx);
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
        gUnk_030023D4 = sub_08002ee8(2);
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
        w2 = sub_08002ee8(3);
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
    gUnk_03002490->unk30 = gUnk_08749D44[gUnk_02007D00[1]];
}

void sub_080ad278(void)
{
    s32 w;

    w = gUnk_03002490->unk30 - 1;
    gUnk_03002490->unk30 = w;
    gUnk_030023D4 = gUnk_08749D4C[gUnk_02007D00[1]];
    sub_08006338(*(s16 *)(gUnk_030023D4 + w * 2));
    gUnk_030023D4 = gUnk_08749D70[gUnk_02007D00[1]];
    sub_080061c0(*(s32 *)(gUnk_030023D4 + gUnk_03002490->unk30 * 4), 0x5A5A5A5A);
    gUnk_030023D4 = gUnk_08749D94[gUnk_02007D00[1]];
    gUnk_03002490->unk58 = *(s32 *)(gUnk_030023D4 + gUnk_03002490->unk30 * 4);
    gUnk_030023D4 = gUnk_08749DB8[gUnk_02007D00[1]];
    sub_080031b8(*(s32 *)(gUnk_030023D4 + gUnk_03002490->unk30 * 4));
    gUnk_030023D4 = gUnk_08749DDC[gUnk_02007D00[1]];
    TaskYieldTrampoline(*(u8 *)(gUnk_030023D4 + gUnk_03002490->unk30));
}

void sub_080ad32c(void)
{
    struct Task *t;

    gUnk_03002490->unk43 = gUnk_0874AAD0[gUnk_02007D00[0]];
    t = gUnk_03002490;
    t->unk4C = gUnk_0874AAD4[gUnk_02007D00[0]] << 16;
    t->unk50 = gUnk_0874AADC[gUnk_02007D00[0]] << 16;
}

s32 sub_080ad37c(void)
{
    s32 r = 0;

    if ((s16)gUnk_03002490->unk78 >= gUnk_02007D00[9] >> 1)
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
    gUnk_03002490->unk30 = gUnk_02007D00[2] + gUnk_030023D4;
}

void sub_080ad3e8(void)
{
    struct ActorSpawn sp;
    struct Task *t;
    struct Actor *a;

    t = gUnk_03002490;
    a = t->unk8C;
    sp.unk00 = 14;
    sp.unk04 = 116;
    sp.unk08 = t->unk73;
    sp.unk09 = t->unk74;
    sp.unk0C = gUnk_0874AAE4[gUnk_02007D00[0]];
    sp.unk0E = gUnk_0874AAEC[gUnk_02007D00[0]];
    sp.unk10 = a->unk20;
    sp.unk0A = 0;
    gUnk_03002490->unk46 = sub_08064ba8(&sp, 1);
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
            sub_0806395c((u16)gUnk_02007D00[5]);
            sub_08006148(sub_080acf48, gCurTaskIdx);
        }
    }
}

void sub_080ad4b8(void)
{
    struct ActorSpawn sp;
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080ad650;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk38 = gUnk_08753E3C;
    gUnk_03002490->unk42 = 10;
    sub_08066088(0);
    gUnk_03002490->unk40 |= 128 << 4;
    sub_08066144();
    gUnk_03002490->unk43 = 1;
    sub_08063a00((u32)gUnk_0874B3FC);
    t = gUnk_03002490;
    t->unk1C = gUnk_03002398 << 16;
    t->unk4C = 176 << 16;
    t->unk48 = gUnk_03002398 + 56;
    t->unk50 = gUnk_03001F00 << 16;
    t->unk4A = t->unk50 >> 16;
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
    gUnk_02007D00[9] = sub_08065f5c();
    sp.unk00 = 19;
    sp.unk04 = 121;
    sp.unk08 = gUnk_03002490->unk73;
    sp.unk09 = gUnk_03002490->unk74;
    sp.unk0A = 0;
    sub_08064b28(&sp, 0);
    sp.unk00 = 19;
    sp.unk04 = 122;
    sp.unk08 = gUnk_03002490->unk73;
    sp.unk09 = gUnk_03002490->unk74;
    sp.unk0A = 0;
    sub_08064b28(&sp, 0);
    sub_08064d34(190, 1);
    sub_08064d34(189, 1);
    gUnk_02007D00[8] = sub_08064d34(191, 1);
    sub_08064d34(192, 1);
    gUnk_03002490->unk8C->unk3C = (u32)sub_080ae380;
    sub_08066544();
    gUnk_03002490->unk04 = (u32)sub_080ad710;
    for (;;)
    {
        gUnk_03002490->unk3C = 4;
        sub_080ad630();
        gUnk_03002490->unk3C = 5;
        sub_080ad630();
        gUnk_03002490->unk3C = 6;
        sub_080ad630();
        gUnk_03002490->unk3C = 7;
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
    register s16 *sp5 asm("r4");
    s32 v;
    s32 w;
    s32 v2;
    s32 w2;

    if (gUnk_02006190[3] <= 0)
        sub_080059a0();
    else
        gUnk_030023D4 = (s16)gUnk_03002490->unk78;
    ta = gUnk_03002490;
    v = ta->unk50 >> 16;
    sp5 = gUnk_03005628;
    w = sp5[2] + 90;
    if (v < w)
        ta->unk50 = (w << 16) + (128 << 8);
    tb = gUnk_03002490;
    v2 = tb->unk50 >> 16;
    w2 = sp5[3] - 90;
    if (v2 >= w2)
        tb->unk50 = (w2 << 16) + -0x8000;
    t = gUnk_03002490;
    t->unk1C = (u16)t->unk1C + (gUnk_03002398 << 16) + t->unk18;
    gUnk_03002398 = t->unk1C >> 16;
    gUnk_03001F00 = t->unk50 >> 16;
    t->unk48 = gUnk_03002398 + (t->unk4C >> 16) - 120;
    t->unk4A = t->unk50 >> 16;
    gUnk_02006190[0] = t->unk48;
    gUnk_02006190[1] = t->unk4A;
}

void sub_080ad710(void)
{
    s32 w;
    s32 w2;

    if (gUnk_0200AFF8 != 0)
        sub_08066564();
    sub_08068f68();
    sub_08069b44();
    sub_080308e8((u32)gUnk_0874B538);
    w = gUnk_03002490->unk28 - 1;
    gUnk_03002490->unk28 = w;
    if (w <= 0)
        sub_080ad7f0();
    w2 = gUnk_03002490->unk2C - 1;
    gUnk_03002490->unk2C = w2;
    if (w2 == 0)
    {
        sub_080ad788();
        gUnk_03002490->unk2C = 120;
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
    register s32 *qa asm("r1");
    register s32 *q asm("r5");
    register s32 v4 asm("r4");
    register s32 v3 asm("r3");
    register s32 v asm("r0");
    s32 v0;
    s32 *pDh;

    qa = &gUnk_030023D4;
    pDh = gUnk_02007D00;
    v4 = pDh[2];
    v3 = v4 >> 16;
    *qa = v3;
    v0 = gUnk_03002490->unk54;
    q = qa;
    asm("" : "+r"(q));
    if (v0 > 0)
    {
        v = v3 + 1;
        *q = v;
        if (v > 2)
            *q = 2;
    }
    else if (v0 < 0)
    {
        v = v3 - 1;
        *q = v;
        if (v < 0)
            *q = 0;
    }
    else if (v3 != 1)
        *q = 1;
    else if ((v4 &= 255) == 0)
        *q = 2;
    else
        *q = 0;
    pDh[2] = ((s16 *)pDh)[5] + (*q << 16);
}

void sub_080ad7f0(void)
{
    struct Task *t;
    register struct Task **c8 asm("r8");
    s32 d;
    s32 va;
    s32 r;
    struct Task *u2;

    gUnk_030023D4 = gUnk_0874AAF7[gUnk_02007D00[1]];
    gUnk_03002490->unk28 = gUnk_0874AB26[gUnk_030023D4];
    gUnk_030023B4 = gUnk_0874AB50[gUnk_030023D4] + gUnk_02007D00[3];
    gUnk_03002490->unk54 = gUnk_0874ABA0[gUnk_030023B4];
    gUnk_03002490->unk58 = gUnk_0874AC24[gUnk_030023B4];
    switch (gUnk_030023D4)
    {
    case 0:
        sub_080ad9dc();
        c8 = &gUnk_03002490;
        t = *c8;
        t->unk18 = gUnk_0874ACBC[gUnk_02007D00[3]];
        gUnk_030023D4 = va = gUnk_0874ACA8[gUnk_02007D00[3]];
        gUnk_030023B4 = gUnk_0874ACB4[gUnk_02007D00[3]];
        d = (gUnk_030023B4 << 16) - t->unk4C;
        if (d < 0)
            d = t->unk4C - (gUnk_030023B4 << 16);
        r = Div(d, va);
        t = *c8;
        t->unk28 = r;
        if (t->unk4C >> 16 > gUnk_030023B4)
            gUnk_030023D4 = -gUnk_030023D4;
        t->unk54 = gUnk_030023D4;
        gUnk_02007D00[1]++;
        if (gUnk_02007D00[7] > 0)
            gUnk_02007D00[7]--;
        break;
    case 1:
        sub_080062c4();
        gUnk_030023D4 = gUnk_0874ACB4[gUnk_02007D00[3]];
        gUnk_03002490->unk4C = (gUnk_030023D4 << 16) + (128 << 8);
        sub_080ada20();
        if (gUnk_02007D00[7] > 0)
            gUnk_02007D00[7]--;
        break;
    case 4:
    case 5:
        sub_080062c4();
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
            if (sub_08002ee8(2) == 0)
                break;
        case 2:
            gUnk_03002490->unk58 = -gUnk_03002490->unk58;
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
    register s32 v asm("r5");
    register s32 *p asm("r4");
    s32 x;
    register s32 r asm("r0");

    v = gUnk_03002490->unk78;
    p = gUnk_02007D00;
    x = p[9];
    if (v < x >> 1)
    {
        r = 2;
        goto out;
    }
    if (v < Div(x * 5, 6) + 1)
    {
        r = 1;
        goto out;
    }
    r = 0;
out:
    p[3] = r;
}

void sub_080ada20(void)
{
    register s32 *pd asm("r5");
    register u8 *tb asm("r8");
    register s32 *p asm("r4");
    register s32 *pb asm("r6");
    struct Task **c;
    struct Task *t;
    struct Task *u;
    s32 x;
    s16 *a;
    register s32 *pd2 asm("r9");
    register s32 *pp asm("ip");
    register u8 *tb2 asm("sl");
    s32 *pb2;
    s32 w;
    s32 w2;
    register s32 w3 asm("r0");
    register s32 w6 asm("r0");
    register s32 w7 asm("r0");
    s32 *pr;
    s32 m;
    register u8 *te asm("r3");
    s32 w4;
    s32 w5;
    struct Task *t2;
    register s32 x2 asm("r0");
    register s32 *pd3 asm("r3");
    struct Task **c2;
    s16 v;

    pd = &gUnk_030023D4;
    tb = gUnk_0874ACD4;
    p = gUnk_02007D00;
    w2 = p[3];
    x = 255;
    x -= tb[w2 * 4 + (u8)p[0]];
    *pd = x;
    pb = &gUnk_030023B4;
    *pb = sub_08002ee8(x);
    *pd = 0;
    gUnk_03002490->unk6C = 0;
    c = &gUnk_03002490;
    pp = p;
    tb2 = tb;
    pd2 = pd;
    c2 = &gUnk_03002490;
top:
    t = *c;
    a = (s16 *)&t->unk6C;
    w = *a;
    if (w != p[0])
    {
        w4 = tb2[p[3] * 4 + w];
        x2 = *pd + w4;
        *pd = x2;
        w5 = *pb;
        pd3 = pd2;
        pb2 = pb;
        if (x2 >= w5)
            goto out;
        if (*a == 3)
            goto out;
    }
    u = *c;
    u->unk6C++;
    goto top;
out:
    t2 = *c2;
    v = (s16)t2->unk6C;
    *pd3 = v;
    pp[0] = v;
    te = gUnk_0874ACE4;
    m = gUnk_0874ACE0[v];
    w6 = *pb2 & m;
    w7 = w6 + v * 2;
    w3 = *(u8 *)(w7 + (u32)te);
    pr = pp;
    pr[1] = w3;
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
    v4 = gUnk_03002490->unk50 >> 16;
    p5 = gUnk_03005628;
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
    sub_08064d34(142, 0);
    sub_0806619c(23, (u32)sub_080adb90, (u32)gUnk_082F65D4, 32, 0);
    sub_080055b0(4, gCurTaskIdx);
}

void sub_080adb90(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;
    s32 *p;

    sub_080308e8(gUnk_0874B538);
    c = &gUnk_03002490;
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

    t = gUnk_03002490;
    t->unk4C = t->unk48 << 16;
    sub_080639a4((u32)gUnk_0874B504);
    tb = gUnk_03002790;
    u = &tb[gUnk_02007D00[8]];
    u->unk3C = 13;
    sub_08006148(sub_0806a344, gCurTaskIdx);
    return 1;
}

void sub_080adc44(void)
{
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(150);
    if (gUnk_030023D8 == 8 && gUnk_03001F30 == 0)
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
            sub_080258e0();
        }
    }
    sub_08006138();
}

void sub_080adca4(void)
{
    struct Task **c;
    s32 *p;
    s32 *p2;
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08005d9c;
    t->unk38 = gUnk_08753E3C;
    t->unk42 = 9;
    u = gUnk_03002490;
    u->unk08 = (u32)sub_080add48;
    c = &gUnk_03002490;
    p = gUnk_02007D00;
top:
    (*c)->unk3C = 14;
    if (p[7] > 0)
    {
        p2 = gUnk_02007D00;
        do
        {
            TaskYieldTrampoline(1);
        } while (p2[7] > 0);
    }
    sub_080031b8(131 << 2);
    (*c)->unk3C++;
    TaskYieldTrampoline(4);
    (*c)->unk3C++;
    TaskYieldTrampoline(48);
    sub_080addf8();
    TaskYieldTrampoline(64);
    (*c)->unk3C--;
    TaskYieldTrampoline(4);
    (*c)->unk3C--;
    TaskYieldTrampoline(2);
    p[7] = 3;
    goto top;
}

void sub_080add48(void)
{
    register struct Task *t asm("ip");
    s16 *a;
    s32 w;
    s32 i;
    u32 o;
    struct Task *tt;
    struct Task *tt2;
    s32 i2;
    u32 o2;
    s32 w2;

    t = gUnk_03002490;
    a = &t->unk44;
    i = *a;
    o = i * 144;
    tt = (struct Task *)((u8 *)gUnk_03002790 + o);
    w = tt->unk48 << 16;
    t->unk4C = w;
    t->unk48 = w >> 16;
    i2 = *a;
    o2 = i2 * 144;
    tt2 = (struct Task *)((u8 *)gUnk_03002790 + o2);
    w2 = tt2->unk4A << 16;
    t->unk50 = w2;
    t->unk4A = w2 >> 16;
    if (gUnk_0200D120[*a - 32].unk05 == 2)
        sub_08006148(sub_080ade98, gCurTaskIdx);
    else if (gUnk_02006190[3] != 0)
        sub_080055b0(1, gCurTaskIdx);
    else
        sub_080055b0(0, gCurTaskIdx);
}

void sub_080addf8(void)
{
    struct ActorSpawn sp;
    s32 n;

    sub_080031b8(0x20B);
    if (gUnk_02007D00[4] == 0)
        n = sub_08002ee8(4);
    else if (gUnk_02007D00[4] == 1)
    {
        n = 1;
        gUnk_02007D00[4] = 2;
    }
    else
        n = sub_08002ee8(3);
    if (n == 0)
    {
        gUnk_02007D00[4] = 1;
        sp.unk00 = 21;
        sp.unk04 = 124;
        sp.unk08 = gUnk_03002490->unk73;
        sp.unk09 = gUnk_03002490->unk74;
        sp.unk0A = 1;
        sub_08064b28(&sp, 0);
    }
    else
    {
        sp.unk00 = 20;
        sp.unk04 = 123;
        sp.unk08 = gUnk_03002490->unk73;
        sp.unk09 = gUnk_03002490->unk74;
        sub_08064b28(&sp, 0);
    }
}

void sub_080ade98(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_080adebc;
    t->unk08 = 0;
    sub_080062c4();
    sub_08006138();
}

void sub_080adebc(void)
{
    struct Task *tb;
    struct Task *t;

    tb = gUnk_03002790;
    t = gUnk_03002490;
    if (tb[t->unk44].unk3C == -1)
        t->unk0C = 0;
    else
        t->unk0C = (u32)sub_08005d9c;
}

void sub_080adefc(void)
{
    struct Task **c;
    s32 k;
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08005d9c;
    t->unk38 = gUnk_08753E3C;
    t->unk42 = 7;
    u = gUnk_03002490;
    u->unk08 = (u32)sub_080adf50;
    c = &gUnk_03002490;
    k = 8;
top:
    (*c)->unk3C = k;
    sub_080ad630();
    (*c)->unk3C = 9;
    sub_080ad630();
    (*c)->unk3C = 10;
    sub_080ad630();
    goto top;
}

void sub_080adf50(void)
{
    register struct Task *t asm("ip");
    s16 *a;
    s32 w;
    s32 i;
    u32 o;
    struct Task *tt;
    struct Task *tt2;
    s32 i2;
    u32 o2;
    s32 w2;

    t = gUnk_03002490;
    a = &t->unk44;
    i = *a;
    o = i * 144;
    tt = (struct Task *)((u8 *)gUnk_03002790 + o);
    w = tt->unk48 << 16;
    t->unk4C = w;
    t->unk48 = w >> 16;
    i2 = *a;
    o2 = i2 * 144;
    tt2 = (struct Task *)((u8 *)gUnk_03002790 + o2);
    w2 = tt2->unk4A << 16;
    t->unk50 = w2;
    t->unk4A = w2 >> 16;
    if (gUnk_0200D120[*a - 32].unk05 == 2)
        sub_08006148(sub_080adfd4, gCurTaskIdx);
}

void sub_080adfd4(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_080adff8;
    t->unk08 = 0;
    sub_080062c4();
    sub_08006138();
}

void sub_080adff8(void)
{
    struct Task *tb;
    struct Task *t;

    tb = gUnk_03002790;
    t = gUnk_03002490;
    if (tb[t->unk44].unk3C == -1)
        t->unk0C = 0;
    else
        t->unk0C = (u32)sub_08005d9c;
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

    t = gUnk_03002490;
    t->unk00 = 0;
    t->unk0C = (u32)sub_08005d9c;
    t->unk38 = gUnk_08753E3C;
    t->unk42 = 9;
    u = gUnk_03002490;
    u->unk08 = (u32)sub_080ae0b4;
    u->unk28 = 96;
    c = &gUnk_03002490;
    k = 11;
top:
    t2 = *c;
    w = t2->unk28;
    if (w >= 0)
    {
        t2->unk28 = w - 1;
        t2->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        goto top;
    }
    t2->unk3C = k;
    TaskYieldTrampoline(4);
    u2 = *c;
    u2->unk3C = 12;
    TaskYieldTrampoline(4);
    u3 = *c;
    u3->unk3C = k;
    TaskYieldTrampoline(4);
    u4 = *c;
    u4->unk28 = 96;
    goto top;
}

void sub_080ae0b4(void)
{
    register struct Task *t asm("ip");
    s16 *a;
    s32 w;
    s32 i;
    register s32 k1 asm("r0");
    register s32 k2 asm("r0");
    u32 o;
    struct Task *tt;
    struct Task *tt2;
    s32 i2;
    u32 o2;
    s32 w2;

    t = gUnk_03002490;
    a = &t->unk44;
    i = *a;
    o = i * 144;
    tt = (struct Task *)((u8 *)gUnk_03002790 + o);
    w = tt->unk48 << 16;
    t->unk4C = w;
    t->unk48 = w >> 16;
    i2 = *a;
    o2 = i2 * 144;
    tt2 = (struct Task *)((u8 *)gUnk_03002790 + o2);
    w2 = tt2->unk4A << 16;
    t->unk50 = w2;
    t->unk4A = w2 >> 16;
    if (gUnk_0200D120[*a - 32].unk05 == 2)
    {
        k1 = 13;
        t->unk3C = k1;
        sub_08006148(sub_080ae174, gCurTaskIdx);
    }
    else if (gUnk_02006190[3] != 0)
    {
        k2 = 13;
        t->unk3C = k2;
        sub_080055b0(1, gCurTaskIdx);
    }
    else
        sub_080055b0(0, gCurTaskIdx);
}

void sub_080ae174(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_080ae198;
    t->unk08 = 0;
    sub_080062c4();
    sub_08006138();
}

void sub_080ae198(void)
{
    vs16 *arr;
    struct Task *t;
    s32 i;

    arr = gUnk_03004CA0;
    t = gUnk_03002490;
    i = t->unk44;
    if ((u16)arr[i] != 62)
        sub_08063fe0();
    else
    {
        if (gUnk_03002790[i].unk3C == -1)
            t->unk0C = 0;
        else
            t->unk0C = (u32)sub_08005d9c;
    }
}

void sub_080ae1f0(void)
{
    register struct Task **c asm("r9");
    register s32 *p asm("sl");
    s32 *p2;
    s32 *p3;
    register struct Task **c3 asm("r3");
    register s32 *p4 asm("r2");
    register s32 *p5 asm("r1");
    register s32 *pd asm("r8");
    struct Task *tb;
    struct Task **c5;
    register u16 *tf asm("r6");
    register struct Task *t4 asm("r4");
    struct Task *t5;
    register struct Task *u9 asm("r0");
    register struct Task **cw3 asm("r1");
    register s16 *a asm("r3");
    s32 w;
    s32 n;
    s32 k;
    register struct Task **cw1 asm("r1");
    register struct Task *tA asm("r0");
    register struct Task **cw2 asm("r3");
    register struct Task *tB asm("r0");
    s32 w7;
    register s32 *pj asm("r3");
    register s32 *pdA asm("r1");
    register s32 *pdB asm("r3");
    s32 w5;
    s32 wc;
    register s32 wn asm("r1");
    register s32 *a23d asm("r2");

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk38 = gUnk_08753E3C;
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk08 = (u32)sub_080ae37c;
    c = &gUnk_03002490;
    p = gUnk_02007D00;
top:
    sub_080062c4();
    cw1 = c;
    tA = *cw1;
    tA->unk3C = 0xFFFF;
    pj = p;
    if (pj[7] <= 0)
    {
        p2 = gUnk_02007D00;
        do
        {
            TaskYieldTrampoline(1);
        } while (p2[7] <= 0);
    }
    p5 = p;
    if (p5[6] > 0)
    {
        p3 = gUnk_02007D00;
        do
        {
            TaskYieldTrampoline(1);
        } while (p3[6] > 0);
    }
    cw2 = c;
    tB = *cw2;
    tB->unk2C = 20;
    p5 = p;
    w7 = p5[7];
    c3 = &gUnk_03002490;
    p4 = gUnk_02007D00;
    if (w7 > 0)
    {
        pd = &gUnk_030023D4;
        tb = gUnk_03002790;
        asm("" :: "r"(tb));
        asm("" :: "r"(tb));
inner:
        w5 = p5[5];
        if (w5 == 0)
        {
            wc = ((*c3)->unk2C & 1) + 1;
            pdA = pd;
            *pdA = wc;
        }
        else
        {
            pdB = pd;
            *pdB = w5;
        }
        a23d = (s32 *)&gUnk_030023D4;
        t4 = *c;
        w = *a23d >> 1;
        t4->unk28 = w;
        a = (s16 *)((u8 *)t4 + 68);
        t4->unk4C = (tb[*a].unk48 - 24) << 16;
        t4->unk50 = (tb[*a].unk4A + gUnk_0874ACFA[w]) << 16;
        t4->unk30 = 3;
        c5 = &gUnk_03002490;
        tf = gUnk_0874ACFE;
        do
        {
            t5 = *c5;
            n = t5->unk30 - 1;
            t5->unk30 = n;
            k = n * 2 + t5->unk28;
            t5->unk3C = tf[k];
            t5->unk54 = gUnk_0874AD0C[n];
            t5->unk58 = gUnk_0874AD18[k];
            TaskYieldTrampoline(gUnk_0874AD30[n]);
        } while ((*c5)->unk30 > 0);
        cw3 = c;
        u9 = *cw3;
        wn = u9->unk2C - 1;
        u9->unk2C = wn;
        c3 = &gUnk_03002490;
        p4 = gUnk_02007D00;
        if (wn > 0)
        {
            p5 = p4;
            if (p4[7] > 0)
                goto inner;
        }
    }
    p4[6] = 240;
    goto top;
}

void sub_080ae37c(void)
{
}

s32 sub_080ae380(s32 i)
{
    s32 v;

    v = (s16)(u16)gUnk_03004CA0[i];
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
