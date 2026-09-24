#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* level_23948.c (0x08023948-0x080242CF, issue #93).
 *
 * Room loaders, part 2: sub_08023948 and sub_08023ca0 (M02's screen
 * setups sub_0800b788 and sub_0800b87c) and sub_08023fd4, plus the
 * room-task variants 1 and 2 of task type #3 (sub_08023e34, sub_08023e78)
 * and their per-frame bodies (sub_08023ea0 ... sub_08023fa0).
 * sub_08023948 and sub_08023ca0 build their map in the second buffer
 * gUnk_02006AA0 through sub_08027a6c instead of gUnk_02000040, spawn the
 * door objects and set up the multi-player cameras; sub_08023fd4 loads
 * the fixed room gUnk_087E1D58[8][7][0] with the player at (136, 928) and
 * BGM 1.  Every loader ends with the per-player loop that refills health,
 * rebuilds the player mask gUnk_03002340 and restarts the player tasks
 * (sub_0803d034 and sub_0803d0a0/sub_0803d1c4). */

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};

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

extern s8 gUnk_03002444;
extern s8 gUnk_030023B8;
extern s8 gUnk_030023EC;
extern u8 gUnk_03001F20;
extern u8 gUnk_0200AF08;
extern u8 gUnk_020069F0;
extern u16 gUnk_030055C0;
extern struct RoomDef *gUnk_030055EC;
extern struct RoomDef **gUnk_087E1D58[][8];
extern s8 gUnk_0300238C;
extern s8 gUnk_03002468;
extern s8 gUnk_02007D64;
extern s8 gUnk_02000000;
extern s16 gUnk_03005620;
extern s16 gUnk_0300561C;
extern s16 gUnk_030055E4;
extern u16 gUnk_03005600[2];
extern u16 gUnk_03005608[2];
extern struct MapCell *gUnk_03005660;
extern struct MapCell gUnk_02006AA0[];
extern u8 gUnk_0200B050;
extern s16 *gUnk_0300558C;
extern s16 gUnk_0873A318[];
extern u8 gUnk_02005574[];
extern u8 gUnk_02000020;
extern u8 gUnk_0200B078;
extern u8 gUnk_0200B040;
extern u16 gUnk_03005624;
extern u8 gUnk_03002340;
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
extern u16 gUnk_03002360;
extern u16 gUnk_03005604[2];
extern struct MapCell gUnk_02000040[];
extern u16 gUnk_0200B080[];
extern u16 gUnk_02007FB0;
extern s16 gUnk_030023CC;
extern s16 gUnk_03002388;

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_080022ac(void);
s32 sub_08003110(s32 songId);
void sub_08006138(void);
void sub_08008c64(u16 a0);
void sub_08009b2c(s32 i);
s32 sub_08009e60(s32 a, u32 b);
void sub_0800a778(void);
void sub_080235ec(s32 a);
void sub_080236d4(void);
s32 sub_08026834(void);
void sub_08026b60(void);
void sub_08026ca4(void);
void sub_08027a6c(void);
void sub_08028320(void);
void sub_0802885c(void);
void sub_08028948(void);
void sub_08028b1c(void);
void sub_08028e4c(void);
void sub_08029034(void);
void sub_080290dc(void);
void sub_08029194(void);
void sub_080291d0(void);
void sub_08029204(void);
void sub_0802923c(void);
void sub_08029270(void);
void sub_08029318(void);
void sub_08029444(void);
void sub_08029708(void);
void sub_080297dc(void);
void sub_08029930(void);
void sub_08029a4c(void);
void sub_08029ab4(void);
void sub_0802ac30(s32 px, s32 py);
void sub_0802ad78(s32 px, s32 py);
void sub_0802c550(void);
void sub_0802c680(void);
void sub_0802c7f4(void);
void sub_0802cab0(void);
void sub_0802cbb8(void);
void sub_0802cc90(void);
void sub_0802cd24(void);
void sub_0802cd38(void);
void sub_0802d0f4(void);
void sub_08030724(void);
void sub_0803d034(s32 a0);
void sub_0803d0a0(s32 a0);
void sub_0803d1c4(s32 a0);
void sub_08040788(void);
void sub_08023ea0(void);
void sub_08023efc(void);
void sub_08023f18(void);
void sub_08023f5c(void);
void sub_08023fa0(void);

void sub_08023948(void)
{
    s32 i;
    u32 a;

    sub_080022ac();
    gUnk_03002444 = 1;
    gUnk_030023B8 = gUnk_030023EC;
    gUnk_03001F20 = 16;
    if (gUnk_0200AF08 != 0 || gUnk_020069F0 == 2)
        gUnk_030055C0 = 4;
    else
        gUnk_030055C0 = 0;
    sub_08008c64(1);
    sub_080235ec(1);
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
    gUnk_03005660 = gUnk_02006AA0;
    sub_08027a6c();
    gUnk_0200B050 = 0;
    gUnk_0300558C = gUnk_0873A318;
    *gUnk_02005574 = 0;
    gUnk_02000020 = 0;
    gUnk_0200B078 = 0;
    gUnk_0200B040 = 0;
    gUnk_03005624 = 31;
    sub_08030724();
    sub_0802cd24();
    sub_0802885c();
    sub_08028948();
    sub_08029318();
    sub_08028e4c();
    sub_08028320();
    sub_08029194();
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
        sub_0803d1c4(i);
        gUnk_03000F98[i] = gUnk_03001EB8[i] = 0;
        gUnk_03002458[i] = gUnk_030023C0[i] = 0;
    }
    sub_08028b1c();
    sub_08009b2c(gUnk_03002360);
    switch (gUnk_030055C0)
    {
    default:
    case 0:
    case 3:
        sub_0802c550();
        break;
    case 2:
    case 4:
        sub_0802cbb8();
        break;
    case 1:
        sub_0802c680();
        break;
    }
    sub_080290dc();
    sub_08029930();
    a = 0;
    CpuFastSet(&a, (u32 *)0x06002000, 0x01000400);
    if (gUnk_0200AF08 != 0)
        sub_0802ac30(gUnk_03005604[0], gUnk_03005604[1]);
    else
        sub_0802ad78(gUnk_03005604[0], gUnk_03005604[1]);
}

void sub_08023ca0(void)
{
    u8 z;
    u32 w;
    u32 zero;

    gUnk_03002444 = 1;
    gUnk_030023B8 = gUnk_030023EC;
    gUnk_03001F20 = 16;
    gUnk_030055C0 = 2;
    sub_08008c64(1);
    sub_080235ec(2);
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
    gUnk_03005660 = gUnk_02006AA0;
    z = 0;
    w = 0;
    sub_08027a6c();
    gUnk_0200B050 = z;
    gUnk_0300558C = gUnk_0873A318;
    gUnk_02005574[0] = z;
    gUnk_02000020 = z;
    gUnk_0200B078 = z;
    gUnk_0200B040 = z;
    gUnk_03005624 = 3;
    sub_08030724();
    sub_0802cd24();
    sub_0802885c();
    sub_08028948();
    sub_08029318();
    sub_08028320();
    sub_08029034();
    sub_08026b60();
    sub_08026834();
    sub_08028b1c();
    sub_0802cab0();
    sub_080290dc();
    sub_08029930();
    zero = w;
    CpuFastSet(&zero, (void *)0x06002000, 0x01000400);
    sub_0802ac30(gUnk_03005604[0], gUnk_03005604[1]);
}

void sub_08023e34(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = 0;
    t->unk0C = (u32)sub_080236d4;
    t->unk04 = (u32)sub_08023ea0;
    if (gUnk_0200AF08 != 0)
        t->unk08 = (u32)sub_08023f5c;
    else
        t->unk08 = (u32)sub_08023f18;
    sub_08006138();
}

void sub_08023e78(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_08023efc;
    t->unk08 = (u32)sub_08023fa0;
    sub_08006138();
}

void sub_08023ea0(void)
{
    if (gUnk_03005624 & 1)
    {
        switch (gUnk_030055C0)
        {
        default:
        case 0:
            sub_0802c550();
            break;
        case 2:
        case 4:
            sub_0802cbb8();
            break;
        case 3:
            sub_0802c7f4();
            break;
        case 1:
            sub_0802c680();
            break;
        }
    }
}

void sub_08023efc(void)
{
    if (gUnk_03005624 & 1)
        sub_0802cab0();
}

void sub_08023f18(void)
{
    if (gUnk_03005624 & 2)
        sub_0802cd38();
    if (gUnk_03005624 & 1)
    {
        sub_08029444();
        sub_080297dc();
    }
    sub_08029a4c();
    if (gUnk_03005624 & 16)
        sub_08026ca4();
    sub_0800a778();
}

void sub_08023f5c(void)
{
    if (gUnk_03005624 & 2)
        sub_0802cd38();
    if (gUnk_03005624 & 1)
    {
        sub_08029444();
        sub_08029708();
    }
    sub_08029ab4();
    if (gUnk_03005624 & 16)
        sub_08026ca4();
    sub_0800a778();
}

void sub_08023fa0(void)
{
    if (gUnk_03005624 & 2)
        sub_0802cd38();
    if (gUnk_03005624 & 1)
    {
        sub_08029444();
        sub_08029708();
    }
    sub_08029ab4();
}

void sub_08023fd4(void)
{
    s32 i;
    u32 a;

    gUnk_030055C0 = 0;
    sub_08008c64(1);
    sub_080235ec(4);
    gUnk_030055EC = gUnk_087E1D58[8][7][0];
    gUnk_02007D64 = gUnk_030055EC->unk57;
    gUnk_02000000 = gUnk_030055EC->unk55;
    sub_0802923c();
    sub_080291d0();
    sub_08029204();
    sub_08029270();
    gUnk_03002444 = 0;
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
    gUnk_02007FB0 = 0;
    sub_08030724();
    sub_0802cd24();
    sub_0802885c();
    sub_08028948();
    sub_0802d0f4();
    gUnk_030023CC = 136;
    gUnk_03002388 = 928;
    sub_08003110(1);
    gUnk_0200B050 = 0;
    gUnk_0200B078 = 0;
    gUnk_02000020 = 1;
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
        }
        else
        {
            gUnk_0300560C[i] = 0;
        }
        sub_0803d034(i);
        sub_0803d0a0(i);
        gUnk_03000F98[i] = gUnk_03001EB8[i] = 0;
        gUnk_03002458[i] = gUnk_030023C0[i] = 0;
    }
    sub_08040788();
    sub_08028b1c();
    sub_08009b2c(gUnk_03002360);
    sub_0802cc90();
    sub_080290dc();
    sub_08029930();
    a = 0;
    CpuFastSet(&a, (u32 *)0x06002000, 0x01000400);
    sub_0802ad78(gUnk_03005604[0], gUnk_03005604[1]);
}
