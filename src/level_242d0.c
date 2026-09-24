#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* level_242d0.c (0x080242D0-0x080261BF, issue #93).
 *
 * Room loaders, part 3: sub_08024300 (M02's stage sequence state,
 * sub_080100ac), sub_08024698 (entered through sub_08024610 and
 * sub_08024654, which preset the level/stage/room and the player
 * position) and sub_0802497c, plus the room-task variants 3-6 of task type
 * #3 (sub_080242d0, sub_08024540, sub_08024904, sub_08024da4) and their
 * per-frame bodies.  A loader looks the room header up in
 * gUnk_087E1D58[level][stage][room] and stores it in gUnk_030055EC, copies
 * its size, origin, BG3 origin and object list into the camera cells,
 * decompresses (or CpuSet-copies) its metatile map into gUnk_02000040 and
 * its metatile table into gUnk_0200B080, resets the camera (M08) and the
 * players, streams the whole view and spawns task type #3.  sub_0802497c is
 * the same shape as sub_08022fa8.
 * 
 * The doors, from sub_08024e40 on (one translation unit with the loaders:
 * split at 0x08024E40, sub_08025024 and sub_080258e0 swap hoisted address
 * registers, lesson 4.79).  sub_08024e40(x, y) finds an enterable door at a
 * pixel - the door metatiles 16/144, 54/182 and 55/183, the door records
 * RoomDef.unk44 and their locks gUnk_02007D58[]/gUnk_0200B04C - and records
 * it in gUnk_02000030 (type << 8 | index); sub_08025024 enters it, a 9-way
 * switch on the door kind (RoomDef door byte +6) that sets the next
 * level/stage/room, the arrival position gUnk_020055E0/gUnk_0200AEF0 and
 * the stage request gUnk_03002438 for M02's state bodies.  sub_080258e0
 * (a stage cleared: the hub's next stage door, or on to the next level),
 * sub_08025a30, sub_08025acc, sub_08025b0c, sub_08025b5c, sub_08025bc8 and
 * sub_08025dc4 are the other exits, each setting the level/stage/room and a
 * stage request; sub_08025e0c lowers the room's bottom bound, sub_08025e88
 * reads an object-list entry's parameter, and sub_08025f00, sub_080260b0
 * and sub_0802610c pick the position the player arrives at. */

struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6[0];
};

struct Door
{
    /*0x00*/ s16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u16 unkA;
};

struct RoomDef
{
    /*0x00*/ u8 filler00[4];
    /*0x04*/ s8 unk04;
    /*0x05*/ u8 unk05;
    /*0x06*/ u8 filler06[2];
    /*0x08*/ void *unk08;
    /*0x0C*/ void *unk0C;
    /*0x10*/ void *unk10;
    /*0x14*/ u16 unk14;
    /*0x16*/ u16 unk16;
    /*0x18*/ u16 *unk18;
    /*0x1C*/ void *unk1C;
    /*0x20*/ void *unk20;
    /*0x24*/ u16 unk24;
    /*0x26*/ u16 unk26;
    /*0x28*/ u16 *unk28;
    /*0x2C*/ void *unk2C;
    /*0x30*/ struct BgMap *unk30;
    /*0x34*/ u16 unk34;
    /*0x36*/ u16 unk36;
    /*0x38*/ u16 unk38;
    /*0x3A*/ u16 unk3A;
    /*0x3C*/ u16 unk3C;
    /*0x3E*/ u16 unk3E;
    /*0x40*/ u16 unk40;
    /*0x42*/ u16 unk42;
    /*0x44*/ struct Door *unk44;
    /*0x48*/ void *unk48;
    /*0x4C*/ u8 filler4C[4];
    /*0x50*/ u16 unk50;
    /*0x52*/ u16 unk52;
    /*0x54*/ u8 unk54;
    /*0x55*/ u8 unk55;
    /*0x56*/ u8 unk56;
    /*0x57*/ u8 unk57;
};

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};

struct Unk020055D8Entry
{
    /*0x00*/ u8 filler0[4];
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6;
};

struct Unk020055D8
{
    /*0x00*/ s16 unk0;
    /*0x02*/ s16 unk2;
    /*0x04*/ struct Unk020055D8Entry *unk4;
};

struct Unk02004B90
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 filler02[2];
    /*0x04*/ u8 unk4_0:4;
    /*0x04*/ u8 unk4_4:4;
    /*0x05*/ u8 filler05[3];
};

struct CamRect { s16 x0, x1, y0, y1; };

extern struct RoomDef **gUnk_087E1D58[][8];
extern struct RoomDef *gUnk_030055EC;
extern s8 gUnk_0300238C;
extern s8 gUnk_030023EC;
extern s8 gUnk_030023B8;
extern u8 gUnk_03001F20;
extern struct MapCell *gUnk_03005660;
extern struct MapCell gUnk_02000040[];
extern s16 gUnk_03005620;
extern s16 gUnk_0300561C;
extern s16 gUnk_030055E4;
extern u16 gUnk_0200B080[];
extern s16 *gUnk_0300558C;
extern s16 gUnk_0873A318[];
extern u16 gUnk_030055C0;
extern u16 gUnk_03005624;
extern u16 gUnk_03005600[2];
extern u16 gUnk_03005604[2];
extern u16 gUnk_03005608[2];
extern u16 gUnk_03005690[2];
extern u8 gUnk_03002340;
extern s16 gUnk_030023CC;
extern s16 gUnk_03002388;
extern s8 gUnk_02000000;
extern u8 gUnk_02000020;
extern u8 gUnk_02005574[];
extern u8 gUnk_020055B0;
extern s8 gUnk_02007D64;
extern u8 gUnk_0200B040;
extern u8 gUnk_0200B050;
extern u8 gUnk_0200B078;
extern u8 gUnk_08732630[];
extern s8 gUnk_03002468;
extern s16 gUnk_020055E0;
extern s16 gUnk_0200AEF0;
extern u8 gUnk_02006170;
extern s8 gUnk_03002444;
extern struct Unk020055D8 gUnk_020055D8;
extern u16 gUnk_02008160[];
extern u8 gUnk_020069F0;
extern u8 gUnk_03002350;
extern u8 gUnk_0300234C;
extern u16 gUnk_030023AC;
extern s16 gUnk_02007D48[];
extern s16 gUnk_02005588[];
extern s16 gUnk_02005580;
extern u16 gUnk_02008008[];
extern u16 gUnk_02007FA8[];
extern u16 gUnk_02004B50[];
extern u16 gUnk_0200AF18[];
extern u8 gUnk_0300560C[];
extern vu16 gUnk_03000F98[];
extern vu16 gUnk_03001EB8[];
extern u16 gUnk_03002458[];
extern u16 gUnk_030023C0[];
extern struct Unk02004B90 gUnk_02004B90[];
extern u8 gUnk_02007D58[];
extern s8 gUnk_0200B038;
extern u8 gUnk_0200B04C;
extern u16 gUnk_02000030;
extern s8 gUnk_030023E0;
extern s8 gUnk_03002384;
extern s8 gUnk_03002438;
extern u8 gUnk_08334EB4[];
extern u16 gUnk_030023D8;
extern u8 gUnk_03001F30;
extern u32 gUnk_030023C8[];
extern u8 gUnk_03002400[8][7];
extern u16 gUnk_08732348[][9];
extern s8 gUnk_08732302[][6];
extern u8 gUnk_087323E2[][3][2];
extern u16 gUnk_0200001C;
extern u8 gUnk_02005578;
extern u16 gUnk_02007D60;
extern s8 gUnk_02007FF8;
extern u16 gUnk_02008050;
extern u16 gUnk_02008054;
extern u8 gUnk_0200AF00;
extern u8 gUnk_0200AF08;
extern u16 gUnk_0200AFF4;
extern s8 gUnk_0200B034;
extern s8 gUnk_02006168;
extern u8 gUnk_02007FC0;
extern u8 gUnk_03002464;
extern u16 gUnk_02007FF0;
extern u16 gUnk_02007FB0;
extern s8 gUnk_02004C98;
extern u16 gUnk_03002364;
extern s8 gUnk_02005590[];
extern u8 gUnk_02008000;
extern u8 gUnk_02007FB4;
extern u16 gUnk_02007D50;
extern u8 gUnk_02007E8C;
extern u16 gUnk_02004B80;
extern s16 gUnk_03005628[4];
extern s16 gUnk_030055F8[4];
extern s16 gUnk_03002158[4];
extern struct CamRect gUnk_03005640[4];
extern s8 gUnk_0200B02C;

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_080022ac(void);
void sub_08006138(void);
void sub_08008c64(u16 a0);
void sub_08009adc(void);
s32 sub_08009e60(s32 a, u32 b);
void sub_0800a778(void);
s32 sub_08021b18(u16 x, u16 y);
void sub_080235ec(s32 a);
void sub_080236d4(void);
void sub_080236e4(void);
void sub_08023748(void);
void sub_080237a4(void);
void sub_080237fc(void);
void sub_0802385c(void);
void sub_080238a4(void);
void sub_080238ec(void);
void sub_0802695c(void);
void sub_08027e28(void);
void sub_08028280(s32 a);
void sub_08028304(void);
void sub_0802885c(void);
void sub_08028948(void);
void sub_08028990(void);
void sub_08028b1c(void);
void sub_08028b8c(void);
void sub_08028e4c(void);
void sub_080290dc(void);
void sub_080291d0(void);
void sub_08029204(void);
void sub_0802923c(void);
void sub_08029270(void);
void sub_080292b0(void);
void sub_080292d0(void);
void sub_08029318(void);
void sub_080293d8(void);
void sub_080294d0(void);
void sub_080295b8(void);
void sub_08029930(void);
void sub_0802a4ec(void);
void sub_0802a9cc(s32 px, s32 py);
void sub_0802ab30(s32 px, s32 py);
void sub_0802aeac(s32 py);
void sub_0802b074(s32 px);
void sub_0802b4bc(void);
void sub_0802be80(void);
void sub_0802c42c(void);
void sub_0802cc90(void);
void sub_0802cd24(void);
void sub_0802cd38(void);
void sub_0802d0f4(void);
void sub_0802d188(void);
void sub_08030724(void);
void sub_0803d034(s32 a0);
void sub_0803d0a0(s32 a0);
void sub_080b4e40(void);
void sub_080b5024(void);
void sub_0802457c(void);
void sub_08024598(void);
void sub_080245d0(void);
void sub_08024698(s32 a0);

void sub_080242d0(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = 0;
    t->unk0C = (u32)sub_080236d4;
    t->unk04 = (u32)sub_0802457c;
    t->unk08 = (u32)sub_080245d0;
    sub_08006138();
}

void sub_08024300(void)
{
    u32 a;

    gUnk_030055C0 = 0;
    sub_08008c64(1);
    sub_080235ec(3);
    if (gUnk_0300238C == 8)
    {
        gUnk_030023B8 = gUnk_030023EC;
        gUnk_03001F20 = 48;
    }
    else
    {
        gUnk_030023B8 = gUnk_0300238C;
        gUnk_03001F20 = 48;
    }
    gUnk_030055EC = gUnk_087E1D58[8][gUnk_030023B8][gUnk_08732630[gUnk_030023B8]];
    gUnk_02007D64 = gUnk_030055EC->unk57;
    gUnk_02000000 = gUnk_030055EC->unk55;
    sub_0802923c();
    sub_080291d0();
    sub_08029204();
    sub_08029270();
    gUnk_03005620 = gUnk_030055EC->unk14;
    gUnk_0300561C = gUnk_030055EC->unk16;
    gUnk_030055E4 = gUnk_03005620 * gUnk_0300561C;
    gUnk_03005600[0] = gUnk_030055EC->unk24;
    gUnk_03005600[1] = gUnk_030055EC->unk26;
    gUnk_03005608[0] = gUnk_030055EC->unk34;
    gUnk_03005608[1] = gUnk_030055EC->unk36;
    gUnk_03005660 = gUnk_02000040;
    if (gUnk_030055EC->unk05 != 0)
        sub_080017e4(8, (u32)gUnk_030055EC->unk08, (u32)gUnk_02000040, 0);
    else
        CpuSet(gUnk_030055EC->unk08, gUnk_02000040, (gUnk_030055E4 * 2) & 0x1FFFFF);
    sub_080017e4(8, (u32)gUnk_030055EC->unk20, (u32)gUnk_0200B080, 0);
    gUnk_02000020 = 0;
    gUnk_0200B078 = 0;
    gUnk_0200B040 = 0;
    gUnk_03005624 = 31;
    gUnk_0200B050 = 0;
    gUnk_0300558C = gUnk_0873A318;
    *gUnk_02005574 = 0;
    sub_08030724();
    sub_0802cd24();
    sub_0802885c();
    sub_08027e28();
    sub_08028948();
    sub_0802d0f4();
    gUnk_030023CC = gUnk_03005608[0] + 120;
    gUnk_03002388 = gUnk_03005608[1] + 80;
    gUnk_03002340 = 0;
    gUnk_0200B050 = 0;
    gUnk_0200B078 = 0;
    gUnk_02000020 = 0;
    sub_08028b1c();
    sub_0802cc90();
    sub_080290dc();
    sub_08029930();
    sub_0802a4ec();
    a = 0;
    CpuFastSet(&a, (u32 *)0x06002000, 0x01000400);
    sub_08009adc();
    if (gUnk_020055B0 != 0)
    {
        sub_0802a9cc(gUnk_03005604[0], gUnk_03005604[1]);
        sub_0802ab30(gUnk_03005690[0], gUnk_03005690[1]);
    }
    else
    {
        sub_0802a9cc(gUnk_03005604[0], gUnk_03005604[1]);
        sub_080292b0();
    }
}

void sub_08024540(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_0802457c;
    if (gUnk_020055B0 != 0)
        t->unk08 = (u32)sub_080245d0;
    else
        t->unk08 = (u32)sub_08024598;
    sub_08006138();
}

void sub_0802457c(void)
{
    if (gUnk_03005624 & 1)
        sub_0802cc90();
}

void sub_08024598(void)
{
    if (gUnk_03005624 & 2)
        sub_0802cd38();
    if (gUnk_03005624 & 1)
    {
        sub_0802d188();
        sub_080293d8();
        sub_080294d0();
    }
    sub_08029930();
}

void sub_080245d0(void)
{
    if (gUnk_03005624 & 2)
        sub_0802cd38();
    if (gUnk_03005624 & 1)
    {
        sub_0802d188();
        sub_080293d8();
        sub_080294d0();
        sub_080295b8();
    }
    sub_08029930();
    sub_0800a778();
}

void sub_08024610(s32 x, s32 y)
{
    gUnk_0300238C = 7;
    gUnk_030023EC = 0;
    gUnk_03002468 = 2;
    gUnk_020055E0 = x;
    gUnk_0200AEF0 = y;
    gUnk_02006170 = 1;
    sub_08024698(0);
}

void sub_08024654(s32 x, s32 y)
{
    gUnk_0300238C = 6;
    gUnk_030023EC = 6;
    gUnk_03002468 = 0;
    gUnk_020055E0 = x;
    gUnk_0200AEF0 = y;
    gUnk_02006170 = 1;
    sub_08024698(1);
}

void sub_08024698(s32 a0)
{
    u32 a;

    sub_080022ac();
    gUnk_03002444 = 0;
    gUnk_030023B8 = gUnk_0300238C;
    gUnk_03001F20 = gUnk_030023EC;
    gUnk_030055C0 = 0;
    sub_08008c64(1);
    sub_080235ec(0);
    gUnk_030055EC = gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_03002468];
    gUnk_02007D64 = gUnk_030055EC->unk57;
    gUnk_02000000 = gUnk_030055EC->unk55;
    sub_0802923c();
    sub_080291d0();
    sub_08029204();
    sub_08029270();
    gUnk_03005620 = gUnk_030055EC->unk14;
    gUnk_0300561C = gUnk_030055EC->unk16;
    gUnk_030055E4 = gUnk_03005620 * gUnk_0300561C;
    gUnk_03005600[0] = gUnk_030055EC->unk24;
    gUnk_03005600[1] = gUnk_030055EC->unk26;
    gUnk_03005608[0] = gUnk_030055EC->unk34;
    gUnk_03005608[1] = gUnk_030055EC->unk36;
    gUnk_020055D8.unk0 = gUnk_030055EC->unk3C;
    gUnk_020055D8.unk2 = gUnk_030055EC->unk3E;
    gUnk_020055D8.unk4 = gUnk_030055EC->unk48;
    gUnk_03005660 = gUnk_02000040;
    if (gUnk_030055EC->unk05 != 0)
    {
        sub_080017e4(8, (u32)gUnk_030055EC->unk08, (u32)gUnk_02000040, 0);
        sub_080017e4(8, (u32)gUnk_030055EC->unk0C, (u32)gUnk_02008160, 0);
    }
    else
    {
        CpuSet(gUnk_030055EC->unk08, gUnk_02000040, (gUnk_030055E4 * 2) & 0x1FFFFF);
        CpuSet(gUnk_030055EC->unk0C, gUnk_02008160, gUnk_030055E4 & 0x1FFFFF);
    }
    sub_080017e4(8, (u32)gUnk_030055EC->unk20, (u32)gUnk_0200B080, 0);
    gUnk_02000020 = 0;
    gUnk_0200B078 = 0;
    gUnk_0200B040 = 0;
    gUnk_03005624 = 31;
    sub_08030724();
    sub_0802cd24();
    sub_0802885c();
    sub_08028948();
    sub_08028280(a0);
    sub_08028e4c();
    sub_0802d0f4();
    sub_08028304();
    sub_08028b1c();
    sub_08009adc();
    switch (gUnk_030055C0)
    {
    default:
    case 0:
    case 1:
    case 3:
        sub_0802b4bc();
        break;
    case 5:
        sub_0802c42c();
        break;
    }
    sub_080290dc();
    sub_08029930();
    a = 0;
    CpuFastSet(&a, (u32 *)0x06002000, 0x01000400);
    switch (gUnk_0200B050)
    {
    default:
    case 0:
    case 2:
    case 3:
    case 5:
        sub_0802a9cc(gUnk_03005604[0], gUnk_03005604[1]);
        sub_080292b0();
        break;
    case 1:
    case 4:
        sub_0802a9cc(gUnk_03005604[0], gUnk_03005604[1]);
        sub_0802ab30(gUnk_03005690[0], gUnk_03005690[1]);
        break;
    }
}

void sub_08024904(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_0802457c;
    switch (gUnk_0200B050)
    {
    case 1:
        gUnk_03002490->unk08 = (u32)sub_080237fc;
        break;
    case 4:
        gUnk_03002490->unk08 = (u32)sub_0802385c;
        break;
    default:
    case 0:
    case 2:
    case 3:
    case 5:
        gUnk_03002490->unk08 = (u32)sub_08023748;
        break;
    }
    sub_08006138();
}

void sub_0802497c(void)
{
    s32 i;
    u32 a;

    sub_080022ac();
    gUnk_03002444 = 0;
    if (gUnk_0300238C == 8)
    {
        gUnk_030023B8 = gUnk_030023EC;
        *(s8 *)&gUnk_03001F20 = -1;
    }
    else
    {
        gUnk_030023B8 = gUnk_0300238C;
        gUnk_03001F20 = gUnk_030023EC;
    }
    if (gUnk_020069F0 == 2)
        gUnk_030055C0 = 5;
    else
        gUnk_030055C0 = 0;
    sub_080235ec(6);
    gUnk_030055EC = gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_03002468];
    gUnk_02007D64 = gUnk_030055EC->unk57;
    gUnk_02000000 = gUnk_030055EC->unk55;
    sub_0802923c();
    sub_080291d0();
    sub_08029204();
    sub_08029270();
    gUnk_03005620 = gUnk_030055EC->unk14;
    gUnk_0300561C = gUnk_030055EC->unk16;
    gUnk_030055E4 = gUnk_03005620 * gUnk_0300561C;
    gUnk_03005600[0] = gUnk_030055EC->unk24;
    gUnk_03005600[1] = gUnk_030055EC->unk26;
    gUnk_03005608[0] = gUnk_030055EC->unk34;
    gUnk_03005608[1] = gUnk_030055EC->unk36;
    gUnk_020055D8.unk0 = gUnk_030055EC->unk3C;
    gUnk_020055D8.unk2 = gUnk_030055EC->unk3E;
    gUnk_020055D8.unk4 = gUnk_030055EC->unk48;
    gUnk_03005660 = gUnk_02000040;
    if (gUnk_030055EC->unk05 != 0)
    {
        sub_080017e4(8, (u32)gUnk_030055EC->unk08, (u32)gUnk_02000040, 0);
        sub_080017e4(8, (u32)gUnk_030055EC->unk0C, (u32)gUnk_02008160, 0);
    }
    else
    {
        CpuSet(gUnk_030055EC->unk08, gUnk_02000040, (gUnk_030055E4 * 2) & 0x1FFFFF);
        CpuSet(gUnk_030055EC->unk0C, gUnk_02008160, gUnk_030055E4 & 0x1FFFFF);
    }
    sub_080017e4(8, (u32)gUnk_030055EC->unk20, (u32)gUnk_0200B080, 0);
    gUnk_02000020 = 0;
    gUnk_0200B078 = 0;
    gUnk_0200B040 = 0;
    gUnk_03005624 = 31;
    sub_08030724();
    sub_0802cd24();
    sub_0802885c();
    sub_08028948();
    sub_08027e28();
    sub_080b4e40();
    sub_08028e4c();
    sub_0802d0f4();
    sub_08029318();
    sub_08028304();
    gUnk_03002340 = 0;
    gUnk_03002350 = 0;
    gUnk_0300234C = 0;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if (gUnk_02007D48[i] != 0 || gUnk_02005588[i] != 0)
        {
            if (gUnk_02005588[i] == 0)
            {
                gUnk_02005588[i] = gUnk_02005580;
                gUnk_02008008[i] = 0;
                gUnk_02007FA8[i] = 0xFFFF;
                sub_08009e60(-1, i);
            }
            if ((s16)gUnk_02008008[i] != 0)
            {
                gUnk_02004B50[i] = gUnk_02008008[i];
                gUnk_0200AF18[i] = gUnk_02007FA8[i];
                gUnk_02008008[i] = 0;
                gUnk_02007FA8[i] = 0xFFFF;
            }
            gUnk_03002340 |= 1 << i;
            gUnk_03002350++;
            gUnk_0300234C++;
            gUnk_0300560C[i] = 0;
        }
        else
        {
            gUnk_0300560C[i] = 3;
        }
        sub_0803d034(i);
        sub_0803d0a0(i);
        gUnk_03000F98[i] = gUnk_03001EB8[i] = 0;
        gUnk_03002458[i] = gUnk_030023C0[i] = 0;
    }
    sub_080b5024();
    if (gUnk_030023AC == 1)
        sub_08028b1c();
    else
        sub_08028990();
    sub_08009adc();
    switch (gUnk_030055C0)
    {
    default:
    case 0:
    case 1:
        sub_0802b4bc();
        break;
    case 3:
        sub_0802be80();
        break;
    case 5:
        sub_0802c42c();
        break;
    }
    sub_080290dc();
    sub_08029930();
    sub_080292d0();
    a = 0;
    CpuFastSet(&a, (u32 *)0x06002000, 0x01000400);
    switch (gUnk_0200B050)
    {
    default:
    case 0:
    case 2:
        sub_0802a9cc(gUnk_03005604[0], gUnk_03005604[1]);
        sub_080292b0();
        break;
    case 3:
        sub_0802b074(gUnk_03005604[0]);
        sub_080292b0();
        break;
    case 5:
        sub_0802aeac(gUnk_03005604[1]);
        break;
    case 1:
    case 4:
        sub_0802a9cc(gUnk_03005604[0], gUnk_03005604[1]);
        sub_0802ab30(gUnk_03005690[0], gUnk_03005690[1]);
        break;
    }
}

void sub_08024da4(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_080236e4;
    switch (gUnk_0200B050)
    {
    default:
    case 0:
        gUnk_03002490->unk08 = (u32)sub_08023748;
        break;
    case 1:
        gUnk_03002490->unk08 = (u32)sub_080237fc;
        break;
    case 2:
        gUnk_03002490->unk08 = (u32)sub_080238ec;
        break;
    case 3:
        gUnk_03002490->unk08 = (u32)sub_080237a4;
        break;
    case 4:
        gUnk_03002490->unk08 = (u32)sub_0802385c;
        break;
    case 5:
        gUnk_03002490->unk08 = (u32)sub_080238a4;
        break;
    }
    sub_08006138();
}

s32 sub_08024e40(s32 x, s32 y)
{
    s32 type;
    s32 i;
    struct Door *d;

    gUnk_02000030 = 0xFF;
    if (gUnk_030055EC->unk3A == 0)
        return 0;
    switch (sub_08021b18(x, y))
    {
    case 55:
    case 183:
        x -= 16;
        type = 1;
        break;
    case 54:
    case 182:
        type = 2;
        break;
    case 16:
    case 144:
        type = 1;
        break;
    default:
        return 0;
    }
    d = gUnk_030055EC->unk44;
    for (i = 0; i < gUnk_030055EC->unk3A; d++, i++)
    {
        if (gUnk_03002444 != 0)
        {
            if (d->unk0 != 0x270F)
                continue;
        }
        else
        {
            if (d->unk0 == 0x1A0A || d->unk0 == 0x1E61)
                continue;
        }
        if (d->unk2 == (x >> 4) && d->unk4 == (y >> 4))
        {
            if (d->unk0 == 0x15B3)
            {
                gUnk_0200B038 = 1;
                d++;
                i++;
            }
            else
            {
                gUnk_0200B038 = 0;
            }
            break;
        }
    }
    if (gUnk_02004B90[i].unk1 == 0)
        return 0;
    if (gUnk_03002444 != 0)
    {
        switch ((u8)d->unk6)
        {
        case 3:
            if (gUnk_02007D58[gUnk_030023B8] & 1)
                return 0;
            break;
        case 4:
            if (gUnk_02007D58[gUnk_030023B8] & 2)
                return 0;
            break;
        case 5:
            if (gUnk_02007D58[gUnk_030023B8] & 4)
                return 0;
            break;
        case 6:
            if ((gUnk_0200B04C & ~(1 << gUnk_030023B8)) == 0)
                return 0;
            break;
        }
    }
    gUnk_02000030 = (type << 8) | i;
    return 1;
}

s32 sub_08025024(void)
{
    struct RoomDef *room;
    struct Door *d;
    struct Door *e;
    s32 i;
    s32 idx;
    s32 t;

    idx = gUnk_02000030 & 0xFF;
    if (idx == 0xFF)
        return 0;
    d = &gUnk_030055EC->unk44[idx];
    gUnk_02006170 = 0;
    gUnk_0200AF08 = 0;
    if (gUnk_03002444 != 0)
    {
        if (d->unk0 == 0x270F)
        {
            gUnk_0200B034 = idx;
            switch (d->unk6 & 0xFF)
            {
            case 0:
                gUnk_0300238C = gUnk_030023EC;
                gUnk_030023EC = d->unk8;
                gUnk_03002468 = 0;
                gUnk_03002438 = 2;
                gUnk_020069F0 = 0;
                gUnk_0200AF00 = 0;
                break;
            case 1:
                if (--gUnk_030023EC < 0)
                    gUnk_030023EC = 0;
                gUnk_03002468 = 0;
                room = gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_03002468];
                e = room->unk44;
                for (i = 0; i < room->unk3A; e++, i++)
                {
                    if (e->unk0 == 0x270F && *(u8 *)&e->unk6 == 2)
                        break;
                }
                gUnk_020055E0 = e->unk2 * 16 + 22;
                gUnk_0200AEF0 = e->unk4 * 16 + 5;
                gUnk_02006170 = 1;
                gUnk_03002438 = 1;
                gUnk_020069F0 = 1;
                gUnk_0200AF00 = 0;
                gUnk_02007FC0 = 0;
                break;
            case 2:
                if (gUnk_030023B8 >= gUnk_030023E0)
                {
                    gUnk_0300238C = gUnk_030023EC;
                    gUnk_030023EC = gUnk_08334EB4[gUnk_0300238C] - 1;
                    gUnk_03002468 = 0;
                    gUnk_02008054 = 0x100;
                    gUnk_0200B038 = 1;
                    gUnk_03002438 = 2;
                    gUnk_020069F0 = 0;
                }
                else
                {
                    if (++gUnk_030023EC > 7)
                        gUnk_030023EC = 7;
                    gUnk_03002468 = 0;
                    room = gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_03002468];
                    e = room->unk44;
                    for (i = 0; i < room->unk3A; e++, i++)
                    {
                        if (e->unk0 == 0x270F && *(u8 *)&e->unk6 == 1)
                            break;
                    }
                    gUnk_020055E0 = e->unk2 * 16 + 22;
                    gUnk_0200AEF0 = e->unk4 * 16 + 5;
                    gUnk_02006170 = 1;
                    gUnk_03002438 = 1;
                    gUnk_020069F0 = 1;
                    gUnk_02007FC0 = 0;
                }
                gUnk_0200AF00 = 0;
                break;
            case 3:
                gUnk_02006168 = gUnk_087323E2[gUnk_030023B8][0][gUnk_03002464];
                gUnk_02007D58[gUnk_030023EC] |= 1;
                gUnk_020055E0 = d->unk2 * 16 + 22;
                gUnk_0200AEF0 = d->unk4 * 16 + 5;
                gUnk_02006170 = 1;
                gUnk_03002438 = 10;
                gUnk_020069F0 = 1;
                gUnk_0200AF00 = 2;
                break;
            case 4:
                gUnk_02006168 = gUnk_087323E2[gUnk_030023B8][1][gUnk_03002464];
                gUnk_02007D58[gUnk_030023EC] |= 2;
                gUnk_020055E0 = d->unk2 * 16 + 22;
                gUnk_0200AEF0 = d->unk4 * 16 + 5;
                gUnk_02006170 = 1;
                gUnk_03002438 = 11;
                gUnk_020069F0 = 1;
                gUnk_0200AF00 = 2;
                break;
            case 5:
                gUnk_02006168 = gUnk_087323E2[gUnk_030023B8][2][gUnk_03002464];
                gUnk_02007D58[gUnk_030023EC] |= 4;
                gUnk_020055E0 = d->unk2 * 16 + 22;
                gUnk_0200AEF0 = d->unk4 * 16 + 5;
                gUnk_02006170 = 1;
                gUnk_03002438 = 9;
                gUnk_020069F0 = 1;
                gUnk_0200AF00 = 2;
                break;
            case 6:
            case 7:
            case 8:
                gUnk_03002438 = *(u8 *)&d->unk6 + 6;
                d++;
                gUnk_03002468 = d->unk0;
                gUnk_020055E0 = d->unk6;
                gUnk_0200AEF0 = d->unk8 + 0xFFFD;
                gUnk_02006170 = 1;
                gUnk_02008054 = 0x200;
                gUnk_0200B038 = 1;
                gUnk_020069F0 = 0;
                gUnk_0200AF00 = 0;
                break;
            default:
                return 0;
            }
        }
    }
    else if (gUnk_02007D64 != 0)
    {
        gUnk_03002468 = d->unk0;
        t = gUnk_030023D8 - 11;
        room = gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_03002468];
        e = room->unk44;
        for (i = 0; i < room->unk3A; e++, i++)
        {
            if (e->unk0 == 0x270F && (e->unk6 & 0xFF) == t)
                break;
        }
        gUnk_020055E0 = e->unk2 * 16 + 22;
        gUnk_0200AEF0 = e->unk4 * 16 + 5;
        gUnk_02006170 = 1;
        gUnk_03002438 = 1;
        gUnk_0200AF00 = 0;
        gUnk_020069F0 = 1;
    }
    else
    {
        if (d->unk0 == 0x22B8)
        {
            switch (gUnk_03002400[gUnk_0300238C][gUnk_030023EC])
            {
            case 0:
                gUnk_0200AF00 = 1;
                gUnk_0200AF08 = 1;
                gUnk_0200001C = gUnk_030023EC + 1;
                if (gUnk_08732348[gUnk_0300238C][6] == (s8)gUnk_03001F20)
                {
                    gUnk_0200AF08 = 17;
                    gUnk_0200B04C |= 1 << gUnk_0300238C;
                }
                if (gUnk_08732302[gUnk_0300238C][gUnk_030023EC] == -1
                    || (gUnk_030023C8[0] & (1 << gUnk_08732302[gUnk_0300238C][gUnk_030023EC])))
                    gUnk_03002400[gUnk_0300238C][gUnk_030023EC] = 2;
                else
                    gUnk_03002400[gUnk_0300238C][gUnk_030023EC] = 1;
                if (gUnk_030023E0 <= gUnk_0300238C && gUnk_03002384 <= gUnk_030023EC)
                    gUnk_03002384 = gUnk_030023EC + 1;
                break;
            case 1:
                if (gUnk_08732302[gUnk_0300238C][gUnk_030023EC] == -1
                    || (gUnk_030023C8[0] & (1 << gUnk_08732302[gUnk_0300238C][gUnk_030023EC])))
                    gUnk_03002400[gUnk_0300238C][gUnk_030023EC] = 2;
            case 2:
            default:
                gUnk_0200AF00 = 0;
                break;
            }
            gUnk_0300238C = 8;
            gUnk_030023EC = gUnk_030023B8;
            gUnk_03002468 = 0;
            room = gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_03002468];
            e = room->unk44;
            for (i = 0; i < room->unk3A; e++, i++)
            {
                if (e->unk0 == 0x270F && *(u8 *)&e->unk6 == 0 && e->unk8 == (s8)gUnk_03001F20)
                    break;
            }
            gUnk_020055E0 = e->unk2 * 16 + 22;
            gUnk_0200AEF0 = e->unk4 * 16 + 5;
            gUnk_02006170 = 1;
            if (gUnk_03001F30 == 0)
            {
                gUnk_03002438 = 8;
            }
            else
            {
                gUnk_03002438 = 1;
                gUnk_02005578 = gUnk_030023B8;
                gUnk_02007FF8 = gUnk_03001F20;
            }
            gUnk_020069F0 = 1;
        }
        else
        {
            if (gUnk_0200B038 != 0)
            {
                gUnk_02008054 = gUnk_03002468;
                gUnk_0200AFF4 = d->unk2 * 16 + 16;
                gUnk_02008050 = d->unk4 * 16 + 5;
            }
            gUnk_03002468 = d->unk0;
            gUnk_020055E0 = d->unk6;
            gUnk_0200AEF0 = d->unk8 + 0xFFFD;
            gUnk_02006170 = 1;
            gUnk_03002438 = 3;
            gUnk_0200AF00 = 0;
            gUnk_020069F0 = 0;
        }
        if (gUnk_02007D60 & 0x8000)
            gUnk_02007D60 = 0;
    }
    return gUnk_02000030 >> 8;
}

void sub_080258e0(void)
{
    if (gUnk_030023D8 == 8)
    {
        struct RoomDef *room;
        struct Door *d;
        s32 i;

        gUnk_030023EC = gUnk_030023B8;
        gUnk_0300238C = 8;
        gUnk_030023EC++;
        if (gUnk_030023EC > 7)
            gUnk_030023EC = 7;
        gUnk_03002468 = 0;
        room = gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_03002468];
        d = room->unk44;
        for (i = 0; i < room->unk3A; i++)
        {
            if (d->unk0 == 0x270F && (u8)d->unk6 == 1)
                break;
            d++;
        }
        gUnk_020055E0 = d->unk2 * 16 + 22;
        gUnk_0200AEF0 = d->unk4 * 16 + 5;
        gUnk_02006170 = 1;
        gUnk_03002438 = 1;
        gUnk_0200AF00 = 0;
        gUnk_020069F0 = 1;
        gUnk_02007FC0 = 1;
        sub_0802695c();
        gUnk_02007FF0 = 0;
    }
    else
    {
        gUnk_0300238C = gUnk_030023B8 + 1;
        gUnk_030023EC = gUnk_08334EB4[gUnk_0300238C] - 1;
        gUnk_03002468 = 0;
        gUnk_020069F0 = 0;
        gUnk_02006170 = 0;
        gUnk_03002438 = 1;
    }
    gUnk_0200B038 = 0;
}

void sub_08025a30(void)
{
    if (gUnk_03001F30 == 0)
    {
        gUnk_0300238C = 7;
        gUnk_030023EC = 0;
        gUnk_03002468 = 0;
        gUnk_02006170 = 0;
        if (gUnk_030023D8 == 8)
        {
            gUnk_02007FC0 = 1;
            gUnk_03002438 = 2;
        }
        else
        {
            gUnk_03002438 = 1;
        }
        gUnk_0200AF00 = 0;
        gUnk_020069F0 = 0;
        gUnk_0200B038 = 0;
    }
    else
    {
        gUnk_03002438 = 7;
    }
    gUnk_02007FF0 = 0;
    gUnk_02007FB0 |= 2;
}

void sub_08025acc(void)
{
    gUnk_03002468++;
    gUnk_02006170 = 0;
    gUnk_03002438 = 3;
    gUnk_0200AF00 = 0;
    gUnk_020069F0 = 0;
    gUnk_02007FB0 |= 2;
}

void sub_08025b0c(void)
{
    gUnk_03002468++;
    gUnk_02006170 = 0;
    gUnk_03002438 = 3;
    gUnk_0200AF00 = 0;
    gUnk_02004C98 = 0;
    gUnk_020069F0 = 2;
    gUnk_0200B038 = 0;
    gUnk_02007FF0 = 0;
}

void sub_08025b5c(void)
{
    if (gUnk_030023D8 == 8)
    {
        if (gUnk_03002464 != 0)
            gUnk_03002364 |= 2;
        else
            gUnk_03002364 |= 1;
        gUnk_030023B8 = 6;
        gUnk_03001F20 = 32;
        gUnk_02007FB0 |= 1;
    }
    gUnk_02007FF0 = 0;
    gUnk_03002438 = 7;
}

void sub_08025bc8(s32 id)
{
    struct RoomDef *room;
    struct Door *d;
    s32 i;
    s8 v;
    s32 lvl;

    if (gUnk_02005590[id - 32] == -1)
        return;
    gUnk_02008000 = gUnk_0300238C;
    gUnk_02007FB4 = gUnk_030023EC;
    gUnk_02007D50 = gUnk_03002468;
    gUnk_0300238C = 8;
    gUnk_030023EC = gUnk_030023B8;
    gUnk_03002468 = 0;
    room = gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_03002468];
    d = room->unk44;
    for (i = 0; i < room->unk3A; d++, i++)
    {
        if (d->unk0 == 0x270F && (u8)d->unk6 == 0 && d->unk8 == (s8)gUnk_03001F20)
            break;
    }
    gUnk_02007E8C = id;
    gUnk_0200AF08 = 1;
    v = ((s8 *)gUnk_020055D8.unk4[gUnk_02005590[id - 32]].filler0)[2];
    gUnk_0200001C = v | 0x100;
    gUnk_030023C8[0] |= 1 << v;
    if (gUnk_03002400[gUnk_030023B8][(s8)gUnk_03001F20] == 1)
        gUnk_03002400[gUnk_030023B8][(s8)gUnk_03001F20] = 2;
    lvl = gUnk_030023B8;
    if (gUnk_08732348[lvl][6] == gUnk_0200001C)
    {
        gUnk_0200AF08 |= 16;
        gUnk_0200B04C |= 1 << lvl;
    }
    gUnk_020069F0 = 4;
    gUnk_0200AF00 = 3;
    gUnk_020055E0 = d->unk2 * 16 + 22;
    gUnk_0200AEF0 = d->unk4 * 16 + 5;
    gUnk_02006170 = 1;
    if (gUnk_0200B050 == 2)
        gUnk_02004B80 = gUnk_03005690[0];
    gUnk_03002438 = 4;
    gUnk_02007FB0 |= 5;
}

void sub_08025dc4(void)
{
    gUnk_0300238C = gUnk_02008000;
    gUnk_030023EC = gUnk_02007FB4;
    gUnk_03002468 = gUnk_02007D50;
    gUnk_03002438 = 3;
}

void sub_08025e00(void)
{
    sub_08028b8c();
}

void sub_08025e0c(void)
{
    s32 i;

    gUnk_03005628[2] = gUnk_0300561C * 16 - gUnk_03005600[1] - 80;
    gUnk_030055F8[2] = gUnk_03005628[2];
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
            gUnk_03005640[i].y0 = gUnk_030055F8[2] - 76;
    }
    gUnk_03002158[2] = gUnk_03005628[2] - 80;
}

s32 sub_08025e88(s32 i)
{
    struct Unk020055D8Entry *e = &gUnk_020055D8.unk4[gUnk_02005590[i - 32]];

    if (gUnk_0200B078 == 3)
    {
        gUnk_0200B02C = e->filler0[3] - 1;
        if (gUnk_0200B02C < gUnk_030023EC)
        {
            gUnk_02004C98 = 1;
            return 2;
        }
        gUnk_02004C98 = 2;
        return 0;
    }
    gUnk_02004C98 = 0;
    return ((s8 *)e->filler0)[2];
}

s32 sub_08025f00(void)
{
    s32 i;

    if (gUnk_02004C98 == 0)
    {
        struct Door *d = gUnk_030055EC->unk44;

        for (i = 0; i < gUnk_030055EC->unk3A; i++)
        {
            if (d->unk0 == 0x1E61 || d->unk0 == 0x1A0A)
                break;
            d++;
        }
        gUnk_03002468 = d->unk2;
        gUnk_020055E0 = d->unk6;
        gUnk_0200AEF0 = d->unk8;
        gUnk_02006170 = 1;
        gUnk_03002438 = 3;
        if (gUnk_02007D60 & 0x8000)
            gUnk_02007D60 = 0;
    }
    else
    {
        gUnk_030023EC = gUnk_0200B02C;
        gUnk_03002468 = 0;
        for (i = 0; i < 32; i++)
        {
            if (gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][i]->unk54 == 3)
            {
                gUnk_03002468 = i;
                break;
            }
        }
        if (gUnk_02004C98 == 2)
            gUnk_020055E0 = 0;
        else
            gUnk_020055E0 = gUnk_03005620 * 16;
        gUnk_0200AEF0 = gUnk_0300561C * 16 - gUnk_03005600[1] - 168;
        gUnk_02006170 = 1;
        gUnk_03002438 = 12;
        gUnk_02007FC0 = 0;
    }
    gUnk_020069F0 = 2;
    gUnk_0200B038 = 0;
    return 1;
}

s32 sub_080260b0(void)
{
    s32 r;

    if (gUnk_02004C98 == 0)
    {
        struct Door *d = gUnk_030055EC->unk44;
        s32 i;

        for (i = 0; i < gUnk_030055EC->unk3A; i++)
        {
            if (d->unk0 == 0x1A0A)
                break;
            d++;
        }
        r = d->unk4;
    }
    else
    {
        r = 3;
        if (gUnk_02004C98 == 2)
            r = 1;
    }
    gUnk_02004C98 = 0;
    return r;
}

s32 sub_0802610c(void)
{
    struct Door *d = gUnk_030055EC->unk44;
    s32 i;

    for (i = 0; i < gUnk_030055EC->unk3A; i++)
    {
        if (d->unk0 == 0x1E61)
            break;
        d++;
    }
    gUnk_03002468 = d->unk2;
    gUnk_020055E0 = d->unk6;
    gUnk_0200AEF0 = d->unk8;
    gUnk_02006170 = 1;
    gUnk_03002438 = 3;
    gUnk_020069F0 = 3;
    gUnk_0200B038 = 0;
    if (gUnk_02007D60 & 0x8000)
        gUnk_02007D60 = 0;
    return 1;
}
