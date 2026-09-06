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

void sub_080b2fe8(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk6C = 6;
    t->unk30 = t->unk48;
    t->unk34 = t->unk4A;
}

void sub_080b3010(u8 a)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;
    u8 *tb;

    c = &gUnk_03002490;
    t = *c;
    if (t->unk30 > 7)
        t->unk30 = 0;
    tb = (u8 *)gUnk_0874C24C;
    sub_08006338(tb[(*c)->unk30]);
    u = *c;
    u->unk30++;
    TaskYieldTrampoline(a);
}

void sub_080b3050(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gUnk_03002490;
    t = *c;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 9;
    u = *c;
    u->unk38 = gUnk_08754308;
    u->unk28 = 1;
    sub_08002e98(u->unk73, 1, gUnk_0874C21C);
}

void sub_080b3090(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gUnk_03002490;
    t = *c;
    t->unk04 = (u32)sub_080b30c8;
    t->unk43 = t->unk74;
    sub_0806395c(0);
    u = *c;
    sub_08002e98(u->unk14, 4, gUnk_0874C220);
}

void sub_080b30c8(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gUnk_03002490;
    t = *c;
    if (t->unk28 != 0)
    {
        if ((u8)sub_080692fc() == 0)
        {
            u = *c;
            sub_08002e98(u->unk15, 4, gUnk_0874C230);
        }
    }
    else
        sub_08002e98(t->unk15, 4, gUnk_0874C230);
    sub_08068e04();
    sub_08069b44();
}

void sub_080b3110(void)
{
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_0874C220);
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

    c = &gUnk_03002490;
    t = *c;
    z = 0;
    t->unk15 = z;
    sub_08063e14();
    u = *c;
    u->unk7A = z;
    u2 = *c;
    u2->unk6C = z;
    do
    {
        q1 = *c;
        q1->unk3C = 4;
        TaskYieldTrampoline(4);
        q2 = *c;
        q2->unk3C = 0xFFFF;
        TaskYieldTrampoline(4);
        q3 = *c;
        q3->unk6C++;
    } while ((s16)q3->unk6C <= 5);
    u3 = gUnk_03002490;
    u3->unk60 = 148 << 6;
    u3->unk68 = 128 << 11;
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

    c = &gUnk_03002490;
    t = *c;
    z = 0;
    t->unk15 = 1;
    sub_08063e14();
    u = *c;
    u->unk7A = z;
    sub_080061c0(128 << 9, 0x5A5A5A5A);
    u2 = *c;
    u2->unk58 = 0xFFFE0000;
    u2->unk60 = 168 << 5;
    for (;;)
        sub_080b3010(4);
}

void sub_080b31e0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk4A > t->unk2C)
    {
        sub_0806395c(2);
        sub_08006148(sub_080b3110, gCurTaskIdx);
    }
}

void sub_080b3214(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;

    c = &gUnk_03002490;
    t = *c;
    z = 0;
    t->unk15 = 2;
    sub_0800634c(4);
    u = *c;
    u->unk7A = z;
    sub_080061c0(160 << 9, 0x5A5A5A5A);
    u2 = *c;
    u2->unk58 = 0xFFFF0000;
    u2->unk60 = 192 << 4;
    for (;;)
        sub_080b3010(8);
}

void sub_080b3258(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk4A > t->unk2C)
    {
        sub_0806395c(3);
        sub_08006148(sub_080b3110, gCurTaskIdx);
    }
}

void sub_080b328c(void)
{
    struct Task **c;
    s32 z;
    struct Task *t;
    struct Task *u;
    struct Task *u2;

    c = &gUnk_03002490;
    t = *c;
    z = 0;
    t->unk15 = 3;
    sub_0800634c(4);
    u = *c;
    u->unk7A = z;
    sub_080061c0(0x1CD00, 0x5A5A5A5A);
    u2 = *c;
    u2->unk58 = 0xFFFFC000;
    u2->unk60 = 224 << 3;
    for (;;)
        sub_080b3010(8);
}

void sub_080b32d0(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;

    c = &gUnk_03002490;
    t = *c;
    if (t->unk4A > t->unk2C)
    {
        t->unk7A = 0;
        sub_080061c0(0x1CD00, 0x5A5A5A5A);
        u = *c;
        u->unk58 = 0xFFFFC000;
        u->unk60 = 224 << 3;
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

    c = &gUnk_03002490;
    t = *c;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_0806523c;
    b42 = &t->unk42;
    z = 0;
    *b42 = 9;
    u = *c;
    u->unk38 = gUnk_0874C568;
    u->unk40 = z;
    u->unk43 = 255;
    u2 = *c;
    sub_08002e98(u2->unk73, 1, gUnk_0874C254);
}

void sub_080b3368(void)
{
    struct Task **c;
    struct Task *u;

    c = &gUnk_03002490;
    (*c)->unk04 = (u32)sub_080b3398;
    sub_0806395c(0);
    u = *c;
    sub_08002e98(u->unk14, 1, gUnk_0874C258);
}

void sub_080b3398(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_0874C25C);
    sub_08068e04();
    sub_08069b44();
}

void sub_080b33bc(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_0874C258);
}

void sub_080b33d8(void)
{
    struct Task **c;
    struct Task *t;
    struct Task *u;
    register s32 v5 asm("r5");
    register s32 v6 asm("r6");
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

    t = gUnk_03002490;
    z = 0;
    t->unk15 = z;
    u = gUnk_03002490;
    u->unk7A = z;
    c = &gUnk_03002490;
    asm("" : "+r"(c));
    v5 = 128 << 9;
    v6 = 0xFFFD0000;
top:
    u0 = *c;
    u0->unk54 = 0xFFFC0000;
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_08006364(2);
    TaskYieldTrampoline(1);
    sub_080b2fe8();
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_08006364(4);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_08006364(2);
    TaskYieldTrampoline(1);
    sub_080b2fe8();
    sub_08006364(0);
    TaskYieldTrampoline(1);
    u1 = *c;
    u1->unk58 = v5;
    sub_08006364(0);
    TaskYieldTrampoline(1);
    sub_08006364(4);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    sub_08006364(2);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_08006364(4);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    u2 = *c;
    u2->unk54 = v6;
    sub_080b2fe8();
    sub_08006364(2);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_08006364(4);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    sub_08006364(2);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(1);
    u3 = *c;
    u3->unk58 = 128 << 8;
    sub_08006364(0);
    TaskYieldTrampoline(1);
    sub_08006364(4);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    sub_08006364(2);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_08006364(4);
    TaskYieldTrampoline(1);
    u4 = *c;
    u4->unk54 = v6;
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    sub_08006364(2);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(1);
    sub_08006364(4);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    u5 = *c;
    u5->unk58 = v5;
    sub_080b2fe8();
    sub_08006364(2);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_08006364(4);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    sub_08006364(2);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(1);
    u6 = *c;
    u6->unk54 = 0xFFFE0000;
    sub_08006364(0);
    TaskYieldTrampoline(1);
    sub_08006364(4);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    sub_08006364(2);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_08006364(4);
    TaskYieldTrampoline(1);
    u7 = *c;
    u7->unk58 = 0;
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    sub_08006364(2);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(1);
    sub_08006364(4);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    u8 = *c;
    u8->unk54 = 0xFFFF0000;
    sub_080b2fe8();
    sub_08006364(2);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_08006364(4);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    sub_08006364(2);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(1);
    u9 = *c;
    u9->unk58 = v5;
    sub_08006364(0);
    TaskYieldTrampoline(1);
    sub_08006364(4);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_080b2fe8();
    sub_08006364(2);
    TaskYieldTrampoline(1);
    sub_08006364(0);
    TaskYieldTrampoline(2);
    sub_08006364(4);
    TaskYieldTrampoline(1);
    goto top;
}

void sub_080b3758(void)
{
    struct Task *t;
    register struct Task *t6 asm("r6");
    s16 *a;
    u32 h;
    s32 w;
    u8 *t46;
    u32 *t68;
    u8 *t40;
    s32 d;
    u8 dv;
    s32 a0v;

    t = gUnk_03002490;
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
        a0v = t->unk42 + 1;
        t68 = (u32 *)gUnk_0874C568;
        t40 = (u8 *)gUnk_0874C240;
        sub_08001a94(a0v, t68[t40[*a]], t->unk3E, t->unk40,
                     w - gUnk_03002348,
                     (s16)(t->unk34 - (u16)gUnk_030023E4));
    }
}

void sub_080b37ec(void)
{
    struct Task **c;
    register struct Task *t asm("r4");
    struct Task *t2;
    struct Task *t3;
    struct Task *t4;
    register s32 k5 asm("r5");
    register s32 k6 asm("r6");
    register s8 *ab asm("r2");
    s8 *ad;
    register u8 *av asm("r0");
    u8 *av2;
    register s32 w1 asm("r1");
    s32 w24;
    s32 w70;
    s32 w24b;
    s32 w3;
    s32 va;
    s32 vb;
    s32 vc;
    s32 vd;
    s32 ve;
    s32 va2;
    s32 vb2;
    register s32 vc2 asm("r3");
    s32 vd2;
    s32 ve2;

    c = &gUnk_03002490;
    t = *c;
    ab = (s8 *)((u8 *)t + 128);
    w1 = *ab;
    av = (u8 *)t + 120;
    *(s16 *)av = w1;
    av -= 50;
    *(u8 *)ab = *(u16 *)av;
    t2 = *c;
    av2 = (u8 *)t2 + 112;
    w70 = *(u16 *)av2;
    *(u16 *)((u8 *)t2 + 70) = w70;
    *(u16 *)av2 = *(u16 *)((u8 *)t2 + 38);
    k5 = 0xFFFF;
    w24 = *(u16 *)&t2->unk24;
    va = t2->unk20;
    t2->unk24 = w24 + (((u32)va >> 16) << 16);
    va &= k5;
    vb = t2->unk34;
    t2->unk20 = va + (((u32)vb >> 16) << 16);
    vb &= k5;
    vc = t2->unk30;
    t2->unk34 = vb + (((u32)vc >> 16) << 16);
    vc &= k5;
    vd = t2->unk2C;
    t2->unk30 = vc + (((u32)vd >> 16) << 16);
    vd &= k5;
    ve = t2->unk28;
    k6 = 0xFFFF0000;
    t2->unk2C = vd + (((u32)ve >> 16) << 16);
    ve &= k5;
    t2->unk28 = ve + (t2->unk48 << 16);
    ab = (s8 *)((u8 *)t2 + 127);
    w1 = *ab;
    av = (u8 *)t2 + 132;
    *(s16 *)av = w1;
    av -= 64;
    *(u8 *)ab = *(u16 *)av;
    t3 = *c;
    ad = &t3->unk43;
    w3 = *ad;
    *(s16 *)((u8 *)t3 + 68) = w3;
    *ad = t3->unk24;
    t4 = *c;
    w24b = t4->unk24 & k6;
    va2 = t4->unk20;
    t4->unk24 = w24b + (va2 & k5);
    va2 &= k6;
    vb2 = t4->unk34;
    t4->unk20 = va2 + (vb2 & k5);
    vb2 &= k6;
    vc2 = t4->unk30;
    t4->unk34 = vb2 + (vc2 & k5);
    vc2 &= k6;
    vd2 = t4->unk2C;
    t4->unk30 = vc2 + (vd2 & k5);
    vd2 &= k6;
    ve2 = t4->unk28;
    k5 &= ve2;
    t4->unk2C = vd2 + k5;
    ve2 &= k6;
    t4->unk28 = ve2 + t4->unk4A;
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

    c = &gUnk_03002490;
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

    r = sub_08002ee8(30);
    c = &gUnk_03002490;
    u1 = *c;
    u1->unk75 = r - 10;
    r2 = sub_08002ee8(30);
    u2 = *c;
    u2->unk7A = r2 - 10;
    r3 = sub_08002ee8(30);
    u3 = *c;
    w3 = r3 - 10;
    *(u8 *)((u8 *)u3 + 123) = w3;
    r4 = sub_08002ee8(30);
    u4 = *c;
    w4 = r4 - 10;
    *(u8 *)((u8 *)u4 + 124) = w4;
    r5 = sub_08002ee8(30);
    u5 = *c;
    w5 = r5 - 10;
    *(u8 *)((u8 *)u5 + 125) = w5;
    r6 = sub_08002ee8(30);
    u6 = *c;
    w6 = r6 - 10;
    *(u16 *)((u8 *)u6 + 130) = w6;
}

void sub_080b3a64(void)
{
    struct Task *t;
    struct Task *t3;
    register s8 *e asm("r1");
    s32 w;
    s32 neg;

    e = (s8 *)gUnk_0874C260;
    asm("" : "+r"(e));
    t = gUnk_03002490;
    w = 65 - *(s16 *)((u8 *)t + 108);
    e = (s8 *)(w + (u32)e);
    w = *e;
    neg = -1;
    if (w != neg)
    {
        asm("" ::: "memory");
        sub_08001a94(t->unk42,
                     gUnk_0874CE68[(s8)*(u8 *)e],
                     t->unk3E, t->unk40,
                     *(s16 *)((u8 *)t + 72) - gUnk_03002348 + (s8)*(u8 *)((u8 *)t + 117),
                     (s16)(*(u16 *)((u8 *)t + 74) - (u16)gUnk_030023E4 + (s8)*(u8 *)((u8 *)t + 124)));
    }
    e = (s8 *)gUnk_0874C2A6;
    asm("" : "+r"(e));
    t = gUnk_03002490;
    w = 65 - *(s16 *)((u8 *)t + 108);
    e = (s8 *)(w + (u32)e);
    w = *e;
    if (w != neg)
    {
        asm("" ::: "memory");
        sub_08001a94(t->unk42,
                     gUnk_0874CE68[(s8)*(u8 *)e],
                     t->unk3E, t->unk40,
                     *(u16 *)((u8 *)t + 34) - gUnk_03002348 + (s8)*(u8 *)((u8 *)t + 122),
                     (s16)(t->unk20 - (u16)gUnk_030023E4 + (s8)*(u8 *)((u8 *)t + 125)));
    }
    e = (s8 *)gUnk_0874C2EC;
    asm("" : "+r"(e));
    t3 = gUnk_03002490;
    w = 65 - *(s16 *)((u8 *)t3 + 108);
    e = (s8 *)(w + (u32)e);
    w = *e;
    if (w != neg)
    {
        asm("" ::: "memory");
        sub_08001a94(t3->unk42,
                     gUnk_0874CE68[(s8)*(u8 *)e],
                     t3->unk3E, t3->unk40,
                     *(s16 *)((u8 *)t3 + 120) - gUnk_03002348 + (s8)*(u8 *)((u8 *)t3 + 123),
                     (s16)(*(u16 *)((u8 *)t3 + 132) - (u16)gUnk_030023E4 + (s8)*(u8 *)((u8 *)t3 + 130)));
    }
    e = (s8 *)gUnk_0874C332;
    asm("" : "+r"(e));
    t = gUnk_03002490;
    w = 65 - *(s16 *)((u8 *)t + 108);
    e = (s8 *)(w + (u32)e);
    w = *e;
    if (w != neg)
    {
        asm("" ::: "memory");
        sub_08001a94(t->unk42,
                     gUnk_0874CE68[(s8)*(u8 *)e],
                     t->unk3E, t->unk40,
                     t->unk18 - gUnk_03002348,
                     (s16)(t->unk1C - (u16)gUnk_030023E4));
    }
}

void sub_080b3c68(void)
{
    register s32 m asm("r5");
    register s32 z8 asm("r8");
    register s32 v6 asm("r6");
    s32 v7;
    register s32 z9 asm("r9");
    register s32 va asm("sl");
    register struct Task **c asm("r4");
    register struct Task *t asm("r0");
    struct Task *ta;
    struct Task *tc;
    struct Task *tb;
    struct Task *u;
    u8 *su;
    s32 w;
    register s32 w2 asm("r1");
    register s32 w4 asm("r1");

    c = &gUnk_03002490;
    ta = *c;
    ta->unk00 = (u32)sub_080656b4;
    *(u16 *)((u8 *)ta + 64) = 0;
    ta->unk0C = (u32)sub_080b3a64;
    *(u8 *)((u8 *)ta + 66) = 8;
    tb = *c;
    tb->unk38 = (u32 *)gUnk_0874CE68;
    tb->unk04 = (u32)sub_080b3e30;
    sub_080b38f0();
    sub_080b3a00();
    tc = *c;
    *(s16 *)((u8 *)tc + 108) = 66;
    *(u16 *)((u8 *)tc + 110) = 0;
    m = 128 << 8;
    z8 = 0;
    v6 = 128 << 7;
    v7 = 128 << 6;
    do
    {
        t = *c;
        t->unk58 = z8;
        TaskYieldTrampoline(2);
        t = *c;
        w4 = 192 << 9;
        asm("" : "+r"(w4));
        t->unk54 = w4;
        t->unk58 = m;
        TaskYieldTrampoline(2);
        t = *c;
        w4 = 128 << 9;
        asm("" : "+r"(w4));
        t->unk54 = w4;
        t->unk58 = m;
        TaskYieldTrampoline(2);
        t = *c;
        t->unk54 = m;
        t->unk58 = v6;
        TaskYieldTrampoline(2);
        t = *c;
        t->unk54 = v6;
        t->unk58 = v7;
        TaskYieldTrampoline(2);
        t = *c;
        t->unk54 = z8;
        t->unk58 = m;
        TaskYieldTrampoline(2);
        t = *c;
        w4 = 0xFFFE8000;
        asm("" : "+r"(w4));
        t->unk54 = w4;
        t->unk58 = m;
        TaskYieldTrampoline(2);
        t = *c;
        w4 = 0xFFFF0000;
        asm("" : "+r"(w4));
        t->unk54 = w4;
        t->unk58 = m;
        TaskYieldTrampoline(2);
        t = *c;
        w4 = 0xFFFF8000;
        asm("" : "+r"(w4));
        t->unk54 = w4;
        t->unk58 = v6;
        TaskYieldTrampoline(2);
        t = *c;
        va = 0xFFFFC000;
        t->unk54 = va;
        t->unk58 = v7;
        TaskYieldTrampoline(2);
        u = *c;
        su = (u8 *)u + 110;
        w = *(u16 *)su + 1;
        *(u16 *)su = w;
    } while ((s16)w <= 2);
    c = &gUnk_03002490;
    t = *c;
    z9 = 0;
    t->unk54 = z9;
    m = 128 << 8;
    t->unk58 = m;
    TaskYieldTrampoline(1);
    t = *c;
    w2 = 192 << 9;
    asm("" : "+r"(w2));
    t->unk54 = w2;
    t->unk58 = m;
    TaskYieldTrampoline(1);
    t = *c;
    w2 = 128 << 9;
    asm("" : "+r"(w2));
    t->unk54 = w2;
    t->unk58 = m;
    TaskYieldTrampoline(1);
    t = *c;
    t->unk54 = m;
    v6 = 128 << 7;
    t->unk58 = v6;
    TaskYieldTrampoline(1);
    t = *c;
    t->unk54 = v6;
    z8 = 128 << 6;
    t->unk58 = z8;
    TaskYieldTrampoline(1);
    t = *c;
    t->unk54 = z9;
    t->unk58 = m;
    TaskYieldTrampoline(1);
    t = *c;
    w2 = 0xFFFE8000;
    asm("" : "+r"(w2));
    t->unk54 = w2;
    t->unk58 = m;
    TaskYieldTrampoline(1);
    t = *c;
    w2 = 0xFFFF0000;
    asm("" : "+r"(w2));
    t->unk54 = w2;
    t->unk58 = m;
    TaskYieldTrampoline(1);
    t = *c;
    w2 = 0xFFFF8000;
    asm("" : "+r"(w2));
    t->unk54 = w2;
    t->unk58 = v6;
    TaskYieldTrampoline(1);
    t = *c;
    t->unk54 = va;
    t->unk58 = z8;
    TaskYieldTrampoline(1);
    sub_08006138();
}

void sub_080b3e30(void)
{
    struct Task **c;
    u8 *su;
    s32 w;

    sub_080b37ec();
    c = &gUnk_03002490;
    su = (u8 *)*c + 108;
    w = *(u16 *)su - 1;
    *(u16 *)su = w;
    if ((u16)w == 0)
        sub_08063fe0();
}

void sub_080b3e54(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gUnk_03002490;
    ta = *c;
    ta->unk00 = (u32)sub_080059d8;
    ta->unk0C = (u32)sub_08065640;
    *(u8 *)((u8 *)ta + 66) = 11;
    tb = *c;
    tb->unk38 = (u32 *)gUnk_0874CCF4;
    sub_08002e98(*(u8 *)((u8 *)tb + 115), 2, gUnk_0875607C);
}

void sub_080b3e94(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gUnk_03002490;
    ta = *c;
    ta->unk00 = (u32)sub_080059d8;
    ta->unk0C = (u32)sub_08065640;
    *(u8 *)((u8 *)ta + 66) = 11;
    tb = *c;
    tb->unk38 = (u32 *)gUnk_0874CD0C;
    sub_08002e98(*(u8 *)((u8 *)tb + 115), 2, gUnk_0875607C);
}

void sub_080b3ed4(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gUnk_03002490;
    ta = *c;
    ta->unk00 = (u32)sub_080059d8;
    ta->unk0C = (u32)sub_08065640;
    *(u8 *)((u8 *)ta + 66) = 11;
    tb = *c;
    tb->unk38 = (u32 *)gUnk_0874CD24;
    sub_08002e98(*(u8 *)((u8 *)tb + 115), 2, gUnk_0875607C);
}

void sub_080b3f14(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gUnk_03002490;
    ta = *c;
    ta->unk00 = (u32)sub_080059d8;
    ta->unk0C = (u32)sub_08065640;
    *(u8 *)((u8 *)ta + 66) = 11;
    tb = *c;
    tb->unk38 = (u32 *)gUnk_0874CD3C;
    sub_08002e98(*(u8 *)((u8 *)tb + 115), 2, gUnk_0875607C);
}

void sub_080b3f54(void)
{
    register struct Task **c asm("r4");
    struct Task *t;
    struct Actor *a;
    register u16 *h asm("r3");
    register s32 k asm("r6");
    register s32 w asm("r0");
    register s32 w2 asm("r2");
    s32 r;
    register s32 u1 asm("r1");
    register s32 u2 asm("r1");
    register s32 z asm("r0");
    register s32 v asm("r4");

    c = &gUnk_03002490;
    a = *(struct Actor **)((u8 *)*c + 140);
    if (a->unk2C == 0)
    {
        r = sub_080640dc((struct AnimCmd *)gUnk_08756084);
        (*c)->unk34 = r;
    }
    t = *c;
    h = (u16 *)((u8 *)t + 64);
    w = *h;
    k = 0xFFF;
    w2 = k;
    asm("" : "+r"(w2));
    w2 &= w;
    v = 240 << 8;
    asm("" : "+r"(v));
    w = v;
    v = 0;
    asm volatile("" : "+r"(v));
    w2 |= w;
    *h = w2;
    if (gUnk_03001F30 == 1 && *(u16 *)((u8 *)t + 118) == 1)
    {
        w2 &= k;
        u1 = 224 << 8;
        asm("" : "+r"(u1));
        w = u1;
        asm("" : "+r"(w));
        w2 |= w;
        *h = w2;
    }
    z = 0;
    *(u8 *)((u8 *)a + 3) = 1;
    u2 = 240 << 8;
    asm("" : "+r"(u2));
    *(u16 *)((u8 *)a + 30) = u2;
    *(u16 *)((u8 *)a + 24) = z;
    *(u16 *)((u8 *)a + 22) = z;
}

void sub_080b3fcc(void)
{
    struct Task **c;
    struct Task *t;
    struct Actor *a;
    register s32 u asm("r0");

    c = &gUnk_03002490;
    t = *c;
    a = *(struct Actor **)((u8 *)t + 140);
    if (t->unk3C == -1)
    {
        *(u16 *)((u8 *)a + 26) = 5;
        t->unk3C = 4;
    }
    else
    {
        u = 0xFFFF;
        *(u16 *)((u8 *)a + 26) = u;
    }
}

void sub_080b3ffc(void)
{
    struct Task **c;
    register struct Task *t asm("r2");
    register s32 wv asm("r1");
    u16 *h;
    register s32 w asm("r0");
    register s32 m asm("r3");

    c = &gUnk_03002490;
    t = *c;
    if (t->unk3C == 5)
        goto arm5;
    if (gUnk_03001F30 == 1 && *(u16 *)((u8 *)t + 118) == 1)
    {
        h = (u16 *)((u8 *)t + 64);
        wv = *h;
        w = 0xFFF;
        w &= wv;
        m = 224 << 8;
        goto join;
    }
    return;
arm5:
    h = (u16 *)((u8 *)t + 64);
    wv = *h;
    w = 0xFFF;
    w &= wv;
    m = 240 << 8;
join:
    wv = m;
    asm("" : "+r"(wv));
    w |= wv;
    *h = w;
}

s32 sub_080b404c(void)
{
    sub_0806395c(1);
    sub_08006148(sub_080b4158, gCurTaskIdx);
    return 1;
}

s32 sub_080b406c(void)
{
    sub_0806395c(0);
    sub_08006148(sub_080b4158, gCurTaskIdx);
    return 1;
}

s32 sub_080b408c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk60 = 128 << 5;
    t->unk68 = 160 << 9;
    return 0;
}

void sub_080b40a4(void)
{
    struct Task **c;
    struct Task *ta;
    struct Task *tb;

    c = &gUnk_03002490;
    ta = *c;
    ta->unk04 = (u32)sub_080b4100;
    *(u8 *)((u8 *)ta + 122) = 0;
    sub_080224b0();
    tb = *c;
    if ((s8)*(u8 *)((u8 *)tb + 123) == 3)
        sub_0806395c(2);
    else
        sub_0806395c(1);
    sub_080b3f54();
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087560A0);
}

void sub_080b4100(void)
{
    register struct Task **c asm("r4");
    struct Task *t;
    s32 r;
    s32 v;

    c = &gUnk_03002490;
    if (*(u8 *)((u8 *)*c + 115) == 0 && (u8)sub_08069888() == 0)
        sub_08002e98((*c)->unk15, 3, gUnk_087560AC);
    c = &gUnk_03002490;
    v = (*c)->unk34;
    r = sub_08064188(v);
    t = *c;
    t->unk34 = r;
    if (v <= 0)
    {
        sub_080b3fcc();
        sub_080b3ffc();
    }
    sub_08068e04();
    sub_08069bbc();
}

void sub_080b4158(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087560A0);
}

void sub_080b4174(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08006244();
    sub_08006138();
}

void sub_080b4190(void)
{
}

void sub_080b4194(void)
{
    struct Task **c;
    struct Task *t;

    c = &gUnk_03002490;
    (*c)->unk15 = 1;
    t = *c;
    if (*(u8 *)((u8 *)t + 115) == 0)
    {
        t->unk60 = 128 << 6;
        t->unk68 = 160 << 10;
    }
    else
    {
        sub_080062c4();
    }
    sub_08006138();
}

void sub_080b41c8(void)
{
}

void sub_080b41cc(void)
{
    struct Task **c;
    struct Task *t;

    c = &gUnk_03002490;
    (*c)->unk15 = 2;
    t = *c;
    if (*(u8 *)((u8 *)t + 115) == 0)
    {
        t->unk60 = 128 << 5;
        t->unk68 = 160 << 9;
    }
    else
    {
        sub_080062c4();
    }
    sub_08006138();
}

void sub_080b4200(void)
{
}

s32 sub_080b4204(u32 a)
{
    s32 r;

    if ((gUnk_03002340 >> a) & 1)
    {
        r = sub_08009ee8(8, a);
        if (r <= *(s16 *)gUnk_02005580 - 1)
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

    c = &gUnk_03002490;
    t = *c;
    t->unk00 = 0;
    t->unk04 = 0;
    t->unk08 = 0;
    w = (s8)*(u8 *)((u8 *)t + 126);
    h = (s16 *)((u8 *)t + 68);
    *h = w;
    e = (u8 *)gUnk_02005588;
    if (*(s16 *)((*h << 1) + (u32)e) != 0)
    {
        sub_080670ac(15);
        sub_08002e98((*c)->unk14, 2, gUnk_087560B8);
        sub_080670d4();
    }
    sub_08063fe0();
}

void sub_080b429c(void)
{
    struct Task **c;
    u8 k4;

    sub_08067108();
    c = &gUnk_03002490;
    do
    {
        if (gUnk_03002360 == *(s16 *)((u8 *)*c + 68))
            sub_080031b8(221);
        k4 = sub_080b4204(*(s16 *)((u8 *)*c + 68));
        TaskYieldTrampoline(8);
    } while (k4 == 0);
    sub_08040894(*(s16 *)((u8 *)gUnk_03002490 + 68), 1);
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

    w0 = *(u8 *)gUnk_03002464;
    n = 1;
    if (w0 == 0 && gUnk_03001F30 == 0)
        n = 2;
    sub_08067108();
    c2 = &gUnk_03002490;
    t = *c2;
    h0 = (u8 *)t + 108;
    z = 0;
    *(u16 *)h0 = z;
    if (z >= n)
        goto xend;
    c = c2;
xbody:
    if (gUnk_03002360 == *(s16 *)((u8 *)*c + 68))
        sub_080031b8(221);
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
    sub_08040894(*(s16 *)((u8 *)gUnk_03002490 + 68), 2);
    sub_08067114();
}

s32 sub_080b4390(void)
{
    struct Task **c;
    struct Task *t;
    s32 r;

    c = &gUnk_03002490;
    t = *c;
    if (t->unk14 == 1)
    {
        sub_08006148(sub_0806a344, gCurTaskIdx);
        r = 1;
    }
    else
    {
        *(u8 *)((u8 *)t + 122) = 0;
        (*c)->unk58 = 0xFFFD0000;
        sub_080031b8(157);
        r = 0;
    }
    return r;
}

s32 sub_080b43d4(void)
{
    sub_0806395c(1);
    sub_08006148(sub_080b4754, gCurTaskIdx);
    return 1;
}

s32 sub_080b43f4(void)
{
    s32 r;

    if (gUnk_03002490->unk14 == 1)
    {
        sub_08006148(sub_0806a344, gCurTaskIdx);
        r = 1;
    }
    else
    {
        sub_0806406c();
        sub_080031b8(157);
        r = 0;
    }
    return r;
}

s32 sub_080b442c(void)
{
    struct Task *t;
    s32 r;

    t = gUnk_03002490;
    if (t->unk14 == 1)
    {
        sub_08006148(sub_0806a344, gCurTaskIdx);
        r = 1;
    }
    else
    {
        t->unk58 = 0;
        if ((u8)(gUnk_03005550[4] - 5) <= 3)
            sub_0806406c();
        sub_080031b8(157);
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

    c = &gUnk_03002490;
    t = *c;
    b3 = (u8 *)gUnk_03002790;
    i = *(s16 *)((u8 *)t + 68);
    o = i * 144;
    w = *(u8 *)(b3 + o + 67);
    w = -w;
    *(u8 *)((u8 *)t + 67) = w;
    e = (u8 *)gUnk_087560C0;
    sub_080061c0(*(s32 *)((*(u8 *)((u8 *)*c + 116) << 2) + (u32)e), 0x5A5A5A5A);
    sub_0800622c(0, 168 << 5, 192 << 10);
    t2 = *c;
    t2->unk28 = 130 << 1;
    t2->unk2C = 2;
    t2->unk30 = 48;
    z = 0;
    t2->unk3C = 4;
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

    c = &gUnk_03002490;
    t = *c;
    w0 = t->unk2C;
    c2 = c;
    if (w0 <= 0)
    {
        w = *(u16 *)((u8 *)t + 60) + 1;
        *(u16 *)((u8 *)t + 60) = w;
        if ((s16)w > 19)
            t->unk3C = 4;
        (*c2)->unk2C = 2;
    }
    t3 = *c2;
    t3->unk2C = t3->unk2C - 1;
}

s32 sub_080b4524(void)
{
    u32 buf[2];
    register struct Task *t asm("ip");
    register u8 *h5 asm("r5");
    register u8 *h6 asm("r6");
    register s32 m asm("r3");
    register u32 kh asm("r4");
    register u32 k asm("r2");
    register s32 w asm("r0");
    register s32 w1 asm("r1");
    register s32 wv asm("r1");
    register s32 wv2 asm("r2");
    register s32 wm asm("r0");
    s32 w0;
    register s32 w5 asm("r5");
    register s32 w6 asm("r6");
    s32 r;

    t = gUnk_03002490;
    h5 = (u8 *)t + 72;
    wm = -640;
    asm("" : "+r"(wm));
    m = wm;
    wv = *(u16 *)h5;
    w = (u16)(m + wv);
    kh = 0xFFFF0000;
    w1 = buf[0];
    w1 &= kh;
    w1 |= w;
    buf[0] = w1;
    h6 = (u8 *)t + 74;
    wv2 = *(u16 *)h6;
    m += wv2;
    m <<= 16;
    k = 0xFFFF;
    w0 = k;
    w0 &= w1;
    w0 |= m;
    buf[0] = w0;
    wm = 160 << 2;
    asm("" : "+r"(wm));
    m = wm;
    w5 = *(u16 *)h5;
    w = (u16)(m + w5);
    w1 = buf[1];
    w1 &= kh;
    w1 |= w;
    buf[1] = w1;
    w6 = *(u16 *)h6;
    m += w6;
    m <<= 16;
    k &= w1;
    k |= m;
    buf[1] = k;
    if (sub_08063eb0((struct Rect *)buf, *(s16 *)((u8 *)t + 68)) != 0)
        r = 0;
    else
    {
        sub_08006148(sub_0806a344, gCurTaskIdx);
        r = 1;
    }
    return r;
}

s32 sub_080b45c0(void)
{
    struct Task *t;
    u8 *p;
    s32 r;

    t = gUnk_03002490;
    p = *(u8 **)((u8 *)t + 136);
    if (((gUnk_03002340 >> *(s16 *)((u8 *)t + 68)) & 1) && *(s8 *)(p + 13) == 0)
        r = 0;
    else
    {
        sub_08006148(sub_0806a344, gCurTaskIdx);
        r = 1;
    }
    return r;
}

void sub_080b460c(void)
{
    register u8 *q0 asm("r0");
    s32 w;
    s32 w2;

    q0 = (u8 *)gUnk_03005568;
    w = *q0;
    if (1 & w)
        goto docall;
    w2 = 2;
    w2 &= w;
    if (w2 == 0)
        goto skip;
docall:
    sub_0806406c();
skip:
    if (*(u8 *)gUnk_03005568 & 4)
        gUnk_03002490->unk58 = 0;
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
    t = gUnk_03002490;
    wl = t->unk28;
    w = wl;
    wl = wl - 1;
    t->unk28 = wl;
    if (w <= 0)
    {
        sub_08006148(sub_0806a344, gCurTaskIdx);
        return;
    }
    if (sub_0802294c(t) != 0)
    {
        sub_08063fe0();
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

    c = &gUnk_03002490;
    ta = *c;
    ta->unk00 = (u32)sub_080059d8;
    ta->unk0C = (u32)sub_080651b4;
    *(u8 *)((u8 *)ta + 66) = 5;
    tb = *c;
    tb->unk38 = (u32 *)gUnk_0874CAEC;
    tb->unk04 = (u32)sub_080b4714;
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
    sub_0806395c(1);
    goto after;
elsecall:
    sub_0806395c(0);
after:
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087560D0);
}

void sub_080b4714(void)
{
    struct Task *t;
    s32 w;

    if ((u8)sub_080696a0() == 0)
        sub_08002e98(gUnk_03002490->unk15, 2, gUnk_087560D8);
    t = gUnk_03002490;
    w = t->unk30;
    if (w <= 0)
        sub_08068e04();
    else
        t->unk30 = w - 1;
    sub_08069b84();
}

void sub_080b4754(void)
{
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087560D0);
}

void sub_080b4770(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08006138();
}

void sub_080b4788(void)
{
    sub_080b4648();
}

void sub_080b4794(void)
{
    struct Task **c;

    c = &gUnk_03002490;
    (*c)->unk15 = 1;
    sub_080062c4();
    (*c)->unk60 = 128 << 3;
    TaskYieldTrampoline(60);
    sub_0806a344();
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

    c = &gUnk_03002490;
    ta = *c;
    ta->unk00 = (u32)sub_080656b4;
    ta->unk0C = (u32)sub_080651b4;
    *(u8 *)((u8 *)ta + 66) = 11;
    tb = *c;
    tb->unk38 = (u32 *)gUnk_08754780;
    sub_08002e98(*(u8 *)((u8 *)tb + 115), 3, gUnk_087560E0);
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
        e = (vu16 *)gUnk_03004CA0;
        e = (vu16 *)((i << 1) + (u32)e);
        if ((s16)*e != n && *e == 68)
        {
            t = (struct Task *)((u8 *)gUnk_03002790 + o);
            if (*(u8 *)(*(u8 **)((u8 *)t + 140) + 4) != 0)
            {
                t->unk4C = *(s16 *)((u8 *)t + 72) << 16;
                t->unk50 = *(s16 *)((u8 *)t + 74) << 16;
            }
            sub_08006148(sub_0806a344, i);
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
    if (i < gUnk_030023AC)
    {
        b5 = (u8 *)gUnk_03002170;
        do
        {
            if ((gUnk_03002340 >> i) & 1)
            {
                sub_0803e68c(i);
                w = (s8)*(u8 *)(b5 + 116 * i + 13);
                if (w == 24 || w == 11)
                    sub_08009fcc(0, -1, i);
            }
            i++;
        } while (i < gUnk_030023AC);
    }
    sub_080b480c();
    sub_08067108();
}

void sub_080b48e0(void)
{
    struct Task *t;

    sub_080062c4();
    t = gUnk_03002490;
    t->unk1C = 0;
    t->unk18 = 0;
}

void sub_080b48f8(void)
{
    s32 i;
    register s32 k asm("r3");
    s32 n;
    register u16 *pa asm("ip");
    s32 n2;
    s32 one;
    register u16 *pb asm("r1");
    s32 m5;
    u8 *p2;
    u8 *pc7;

    i = 0;
    k = 0;
    n2 = gUnk_030023AC;
    pa = &gUnk_030023AC;
    pc7 = &gUnk_03002350;
    if (k < n2)
    {
        m5 = gUnk_03002340;
        n = n2;
        one = 1;
        p2 = (u8 *)gUnk_03002170;
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
        sub_08063fe0();
    }
}

void sub_080b4968(void)
{
    register s32 i asm("r5");
    register s32 o asm("r9");
    register s32 one asm("r8");
    register s32 b asm("r4");
    struct Task **c;
    struct Task *t;
    struct Task *t3;
    u8 *p1;
    s32 w1;
    s32 w2;

    i = 0;
    if (i >= gUnk_030023AC)
        return;
    o = i;
    one = 1;
    do
    {
        if ((gUnk_03002340 >> i) & one)
        {
            b = one << i;
            c = &gUnk_03002490;
            if (!((*c)->unk18 & b))
            {
                t3 = (struct Task *)((u8 *)gUnk_03002790 + o);
                w1 = i * 116;
                p1 = (u8 *)((u32)gUnk_03002170 + w1);
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
                    *(u32 *)(p1 + 104) = (u32)gUnk_0873CB1C;
                }
            }
        }
        o += 144;
        i++;
    } while (i < gUnk_030023AC);
}

void sub_080b4a34(void)
{
    if (gUnk_03002490->unk1C == gUnk_03002350)
        sub_080b48f8();
    else
        sub_080b4968();
}

void sub_080b4a5c(void)
{
    struct Task **c;

    c = &gUnk_03002490;
    (*c)->unk04 = (u32)sub_080b4a8c;
    sub_0806395c(0);
    sub_08002e98((*c)->unk14, 2, gUnk_08756150);
}

void sub_080b4a8c(void)
{
    struct Task **c;

    c = &gUnk_03002490;
    sub_08002e98((*c)->unk15, 2, gUnk_08756158);
    if ((*c)->unk14 == 0 && sub_08068e04() != 0)
    {
        sub_080b4878();
        sub_0806d4e4(0, 0);
        if (gUnk_03002360 == (s8)*(u8 *)((u8 *)*c + 126))
            sub_080031b8(198);
        sub_0806395c(1);
        sub_08006148(sub_080b4afc, gCurTaskIdx);
    }
}

void sub_080b4afc(void)
{
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08756150);
}

void sub_080b4b18(void)
{
    struct Task **c;
    struct Task *t;
    register s32 v6 asm("r6");
    register s32 v5 asm("r5");
    s32 r;

    c = &gUnk_03002490;
    (*c)->unk15 = 0;
    *(u8 *)((u8 *)*c + 67) = 1;
    sub_080062c4();
    r = sub_080640a8((struct AnimCmd *)gUnk_087560EC);
    t = *c;
    t->unk34 = r;
    v6 = 0xFFFFC000;
    v5 = 128 << 7;
    for (;;)
    {
        (*c)->unk58 = v6;
        TaskYieldTrampoline(16);
        (*c)->unk58 = 0xFFFFA000;
        TaskYieldTrampoline(16);
        (*c)->unk58 = v6;
        TaskYieldTrampoline(16);
        (*c)->unk58 = v5;
        TaskYieldTrampoline(16);
        (*c)->unk58 = 192 << 7;
        TaskYieldTrampoline(16);
        (*c)->unk58 = v5;
        TaskYieldTrampoline(16);
    }
}

void sub_080b4b94(void)
{
    struct Task **c;
    struct Task *t;
    s32 r;

    c = &gUnk_03002490;
    r = sub_08064188((*c)->unk34);
    t = *c;
    t->unk34 = r;
}

void sub_080b4bb0(void)
{
    struct Task **c;

    c = &gUnk_03002490;
    (*c)->unk15 = 1;
    sub_080b48e0();
    (*c)->unk3C = 0xFFFF;
    sub_08006138();
}

void sub_080b4bd8(void)
{
    sub_080b4a34();
}

void sub_080b4be4(void)
{
    struct Task **c;

    c = &gUnk_03002490;
    (*c)->unk04 = (u32)sub_080b4c14;
    sub_0806395c(0);
    sub_08002e98((*c)->unk14, 3, gUnk_08756160);
}

void sub_080b4c14(void)
{
    struct Task **c;

    c = &gUnk_03002490;
    sub_08002e98((*c)->unk15, 3, gUnk_0875616C);
    if ((*c)->unk14 == 1 && sub_08068e04() != 0)
    {
        sub_080b4878();
        sub_0806d4e4(0, 0);
        if (gUnk_03002360 == (s8)*(u8 *)((u8 *)*c + 126))
            sub_080031b8(198);
        sub_0806395c(2);
        sub_08006148(sub_080b4c84, gCurTaskIdx);
    }
}

void sub_080b4c84(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08756160);
}

void sub_080b4ca0(void)
{
    struct Task **c;
    struct Task *t;
    s32 r;

    c = &gUnk_03002490;
    (*c)->unk15 = 0;
    *(u8 *)((u8 *)*c + 67) = 255;
    sub_080062c4();
    r = sub_080640a8((struct AnimCmd *)gUnk_087560EC);
    t = *c;
    t->unk34 = r;
    t->unk54 = 0xFFFC0000;
    TaskYieldTrampoline(8);
    (*c)->unk54 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    (*c)->unk54 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    (*c)->unk54 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    sub_080062c4();
    sub_0806395c(1);
    sub_08006138();
}

void sub_080b4d1c(void)
{
    struct Task **c;
    struct Task *t;
    s32 r;

    c = &gUnk_03002490;
    r = sub_08064188((*c)->unk34);
    t = *c;
    t->unk34 = r;
    if (t->unk14 != 0)
        sub_08006148(sub_080b4c84, gCurTaskIdx);
}

void sub_080b4d50(void)
{
    struct Task **c;
    struct Task *t;
    register s32 v6 asm("r6");
    register s32 v5 asm("r5");
    s32 r;

    c = &gUnk_03002490;
    (*c)->unk15 = 1;
    sub_080062c4();
    v6 = 0xFFFFC000;
    v5 = 128 << 7;
    for (;;)
    {
        (*c)->unk58 = v6;
        TaskYieldTrampoline(16);
        (*c)->unk58 = 0xFFFFA000;
        TaskYieldTrampoline(16);
        (*c)->unk58 = v6;
        TaskYieldTrampoline(16);
        (*c)->unk58 = v5;
        TaskYieldTrampoline(16);
        (*c)->unk58 = 192 << 7;
        TaskYieldTrampoline(16);
        (*c)->unk58 = v5;
        TaskYieldTrampoline(16);
    }
}

void sub_080b4db4(void)
{
    struct Task **c;
    struct Task *t;
    s32 r;

    c = &gUnk_03002490;
    r = sub_08064188((*c)->unk34);
    t = *c;
    t->unk34 = r;
}

void sub_080b4dd0(void)
{
    struct Task **c;

    c = &gUnk_03002490;
    (*c)->unk15 = 2;
    sub_080b48e0();
    (*c)->unk3C = 0xFFFF;
    sub_08006138();
}

void sub_080b4df8(void)
{
    sub_080b4a34();
}

void sub_080b4e04(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = 0;
    if (*(u8 *)((u8 *)t + 116) != 2)
        TaskYieldTrampoline(60);
    if (sub_08066394() != 0)
    {
        sub_080670ac(15);
        sub_080258e0();
    }
    sub_08006138();
}

void sub_080b4e40(void)
{
    register u8 *p1 asm("r1");
    register u8 *q asm("r1");
    register s32 w asm("r0");
    s32 m5;
    s32 z4;
    s32 z3;
    s32 k2;
    s32 i2;

    m5 = 255;
    asm("" : "+r"(m5));
    z4 = 0;
    asm("" : "+r"(z4));
    z3 = 0;
    asm("" : "+r"(z3));
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
