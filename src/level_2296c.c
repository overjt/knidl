#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* level_2296c.c (0x0802296C-0x08023617, issue #93).
 *
 * Level resets and room loaders, part 1.  sub_0802296c (M02's
 * sub_0800b4a8) and its twin sub_08022c3c clear the level state, rebuild
 * the per-stage door masks gUnk_02007D58[] and the cleared-stage mask
 * gUnk_0200B04C from the save flags (gUnk_08732348[level][6] names each
 * stage's flag) and place the player at the matching door of the hub
 * room gUnk_087E1D58[8][stage][0]; sub_08022f50 (AgbMain) resets level,
 * stage and room; sub_08022f98/sub_08022f9c are M02's screen-setup hooks;
 * sub_08022fa8 and sub_080233e0 are the loaders of M02's first screen
 * setup sub_0800b648 (see level_242d0.c); sub_080235ec spawns task type
 * #3 with its variant index. */

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

extern struct RoomDef **gUnk_087E1D58[][8];
extern s8 gUnk_0300238C;
extern s8 gUnk_030023EC;
extern s8 gUnk_03002468;
extern s8 gUnk_030023B8;
extern u8 gUnk_03001F20;
extern s8 gUnk_030023E0;
extern s8 gUnk_03002384;
extern u16 gUnk_0200001C;
extern s8 gUnk_02004C98;
extern u8 gUnk_02005578;
extern s16 gUnk_020055E0;
extern u8 gUnk_02006170;
extern u8 gUnk_020069F0;
extern u8 gUnk_02007D58[];
extern u16 gUnk_02007D60;
extern u8 gUnk_02007FC0;
extern u16 gUnk_02007FF0;
extern s8 gUnk_02007FF8;
extern s16 gUnk_0200AEF0;
extern u8 gUnk_0200AF00;
extern u8 gUnk_0200AF04;
extern u8 gUnk_0200AF08;
extern s8 gUnk_0200B038;
extern u8 gUnk_0200B04C;
extern u8 gUnk_03001F30;
extern u32 gUnk_030023C8[];
extern u8 gUnk_03002400[8][7];
extern u16 gUnk_08732348[][9];
extern u8 gUnk_02004B64;
extern s16 gUnk_0200AF0C;
extern u32 gUnk_02007BF0[8][8];
extern s8 gUnk_03002438;
extern u8 gUnk_08334EB4[];
extern u8 gUnk_020055C8;
extern s8 gUnk_03002444;
extern u16 gUnk_030055C0;
extern struct RoomDef *gUnk_030055EC;
extern s8 gUnk_02007D64;
extern s8 gUnk_02000000;
extern s16 gUnk_03005620;
extern s16 gUnk_0300561C;
extern s16 gUnk_030055E4;
extern u16 gUnk_03005600[2];
extern u16 gUnk_03005608[2];
extern struct Unk020055D8 gUnk_020055D8;
extern struct MapCell *gUnk_03005660;
extern struct MapCell gUnk_02000040[];
extern u16 gUnk_02008160[];
extern u16 gUnk_0200B080[];
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
extern u8 gUnk_0200B050;
extern u16 gUnk_03005604[2];
extern u16 gUnk_03005690[2];
extern u16 gUnk_02004B80;
extern u8 gUnk_02007E8C;

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_080022ac(void);
void sub_080055b0(u8 val, s32 idx);
void sub_080055c4(u16 val, s32 idx);
s32 sub_080058e4(u32 type, s32 idx);
void sub_08008c64(u16 a0);
void sub_08009ab8(void);
void sub_08009b2c(s32 i);
s32 sub_08009e60(s32 a, u32 b);
void sub_08027e28(void);
void sub_08028130(void);
void sub_08028304(void);
void sub_0802885c(void);
void sub_08028948(void);
void sub_08028990(void);
void sub_08028b1c(void);
void sub_08028e4c(void);
void sub_080290ac(void);
void sub_080290dc(void);
void sub_08029110(void);
void sub_080291d0(void);
void sub_08029204(void);
void sub_0802923c(void);
void sub_08029270(void);
void sub_080292b0(void);
void sub_080292d0(void);
void sub_08029318(void);
void sub_08029930(void);
void sub_0802a9cc(s32 px, s32 py);
void sub_0802ab30(s32 px, s32 py);
void sub_0802aeac(s32 py);
void sub_0802b074(s32 px);
void sub_0802b4bc(void);
void sub_0802be80(void);
void sub_0802c42c(void);
void sub_0802cd24(void);
void sub_0802d0f4(void);
void sub_08030724(void);
void sub_080307b0(void);
void sub_0803d034(s32 a0);
void sub_0803d1c4(s32 a0);
void sub_08077d38(s32 id);
void sub_080b4e40(void);
void sub_080b4ea8(void);
void sub_080b5024(void);
void sub_080235ec(s32 a);

void sub_0802296c(void)
{
    s32 i;
    s32 j;
    struct RoomDef *room;
    struct Door *d;

    gUnk_02004C98 = 0;
    gUnk_0200AF08 = 0;
    gUnk_0200001C = 0;
    gUnk_02004B64 = 0;
    gUnk_0200B038 = 0;
    gUnk_02007FF0 = 0;
    gUnk_02007D60 = 0;
    gUnk_0200AF0C = -1;
    gUnk_0200B04C = 0;
    for (i = 0; i <= 7; i++)
        for (j = 7; j >= 0; j--)
            gUnk_02007BF0[i][j] = 0;
    for (i = 0; i <= 6; i++)
    {
        if (gUnk_03001F30)
            gUnk_02007D58[i] = 15;
        else
            gUnk_02007D58[i] = 0;
        if (gUnk_08732348[i][6] & 0x100)
        {
            if (gUnk_030023C8[0] & (1 << (gUnk_08732348[i][6] & 0xFF)))
                gUnk_0200B04C |= 1 << i;
        }
        else if (gUnk_03002400[i][gUnk_08732348[i][6]] != 0)
        {
            gUnk_0200B04C |= 1 << i;
        }
    }
    gUnk_0300238C = 8;
    gUnk_030023EC = gUnk_030023B8;
    gUnk_03002468 = 0;
    gUnk_0200AF04 = 0;
    gUnk_02005578 = gUnk_030023EC;
    gUnk_02007FF8 = gUnk_03001F20;
    room = gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_03002468];
    d = room->unk44;
    if (gUnk_030023E0 == 0 && gUnk_03002384 == 0)
    {
        gUnk_02006170 = 0;
        gUnk_020069F0 = 2;
    }
    else
    {
        if ((s8)gUnk_03001F20 == 16)
        {
            for (i = 0; i < room->unk3A; d++, i++)
            {
                if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 1)
                    break;
            }
            gUnk_020055E0 = d->unk2 * 16 + 22;
            gUnk_0200AEF0 = d->unk4 * 16 + 5;
            gUnk_020069F0 = 1;
            gUnk_0200AF00 = 0;
        }
        else if ((s8)gUnk_03001F20 == 32)
        {
            for (i = 0; i < room->unk3A; d++, i++)
            {
                if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 2)
                    break;
            }
            gUnk_020055E0 = d->unk2 * 16 + 16;
            gUnk_0200AEF0 = d->unk4 * 16 + 53;
            gUnk_020069F0 = 0;
        }
        else
        {
            for (i = 0; i < room->unk3A; d++, i++)
            {
                if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 0 && d->unk8 == (s8)gUnk_03001F20)
                    break;
            }
            gUnk_020055E0 = d->unk2 * 16 + 22;
            gUnk_0200AEF0 = d->unk4 * 16 + 5;
            gUnk_020069F0 = 1;
            gUnk_0200AF00 = 0;
        }
        gUnk_02006170 = 1;
    }
    gUnk_02007FC0 = 1;
}

void sub_08022c3c(void)
{
    s32 i;
    s32 j;
    struct RoomDef *room;
    struct Door *d;

    gUnk_02004C98 = 0;
    gUnk_0200AF08 = 0;
    gUnk_0200001C = 0;
    gUnk_02004B64 = 0;
    gUnk_0200B038 = 0;
    gUnk_02007FF0 = 0;
    gUnk_02007D60 = 0;
    gUnk_0200AF0C = -1;
    gUnk_0200B04C = 0;
    for (i = 0; i <= 7; i++)
        for (j = 7; j >= 0; j--)
            gUnk_02007BF0[i][j] = 0;
    for (i = 0; i <= 6; i++)
    {
        if (gUnk_03001F30)
            gUnk_02007D58[i] = 15;
        else
            gUnk_02007D58[i] = 0;
        if (gUnk_08732348[i][6] & 0x100)
        {
            if (gUnk_030023C8[0] & (1 << (gUnk_08732348[i][6] & 0xFF)))
                gUnk_0200B04C |= 1 << i;
        }
        else if (gUnk_03002400[i][gUnk_08732348[i][6]] != 0)
        {
            gUnk_0200B04C |= 1 << i;
        }
    }
    if (gUnk_030023B8 == 7)
    {
        gUnk_0300238C = 7;
        gUnk_030023EC = 0;
        gUnk_03002468 = 0;
        gUnk_02006170 = 0;
        gUnk_03002438 = 2;
        gUnk_0200AF00 = 0;
        gUnk_020069F0 = 0;
    }
    else
    {
        gUnk_0300238C = 8;
        gUnk_030023EC = gUnk_02005578;
        gUnk_03002468 = 0;
        gUnk_0200AF04 = 0;
        if (gUnk_030023E0 == 0 && gUnk_03002384 == 0)
        {
            gUnk_020055E0 = 70;
            gUnk_0200AEF0 = 0x105;
            gUnk_020069F0 = 0;
            gUnk_0200AF00 = 0;
        }
        else
        {
            room = gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_03002468];
            d = room->unk44;
            if (gUnk_02007FF8 == 16)
            {
                for (i = 0; i < room->unk3A; d++, i++)
                {
                    if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 1)
                        break;
                }
                gUnk_020055E0 = d->unk2 * 16 + 22;
                gUnk_0200AEF0 = d->unk4 * 16 + 5;
                gUnk_020069F0 = 1;
                gUnk_0200AF00 = 0;
            }
            else if (gUnk_02007FF8 == 32)
            {
                for (i = 0; i < room->unk3A; d++, i++)
                {
                    if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 2)
                        break;
                }
                gUnk_020055E0 = d->unk2 * 16 + 16;
                gUnk_0200AEF0 = d->unk4 * 16 + 53;
                gUnk_020069F0 = 0;
                gUnk_0200AF00 = 0;
            }
            else
            {
                for (i = 0; i < room->unk3A; d++, i++)
                {
                    if (d->unk0 == 0x270F && *(u8 *)&d->unk6 == 0 && d->unk8 == gUnk_02007FF8)
                        break;
                }
                gUnk_020055E0 = d->unk2 * 16 + 22;
                gUnk_0200AEF0 = d->unk4 * 16 + 5;
                gUnk_020069F0 = 1;
                gUnk_0200AF00 = 0;
            }
        }
        gUnk_03002438 = 1;
        gUnk_02006170 = 1;
    }
    gUnk_0200B038 = 0;
    gUnk_02007FC0 = 1;
}

void sub_08022f50(void)
{
    gUnk_0300238C = 0;
    gUnk_030023EC = gUnk_08334EB4[gUnk_0300238C] - 1;
    gUnk_03002468 = 0;
    gUnk_020069F0 = 0;
    gUnk_02006170 = 0;
    gUnk_02007FC0 = 0;
    sub_08009ab8();
}

void sub_08022f98(void)
{
}

void sub_08022f9c(void)
{
    gUnk_020055C8 = 0;
}

void sub_08022fa8(void)
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
    sub_08027e28();
    sub_080b4e40();
    sub_08028e4c();
    sub_08029110();
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
        sub_0803d1c4(i);
        gUnk_03000F98[i] = gUnk_03001EB8[i] = 0;
        gUnk_03002458[i] = gUnk_030023C0[i] = 0;
    }
    sub_080b5024();
    if (gUnk_030023AC == 1)
        sub_08028b1c();
    else
        sub_08028990();
    sub_08009b2c(gUnk_03002360);
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

void sub_080233e0(void)
{
    u32 a;

    sub_080055b0(0, gUnk_02007E8C);
    sub_080055c4(15, gUnk_02007E8C);
    sub_08077d38(gUnk_02007E8C);
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
    sub_080017e4(8, (u32)gUnk_030055EC->unk20, (u32)gUnk_0200B080, 0);
    gUnk_03005624 = 0;
    sub_08030724();
    sub_0802cd24();
    sub_0802885c();
    sub_08028948();
    sub_08028130();
    sub_080b4ea8();
    sub_08029318();
    sub_080307b0();
    sub_080290ac();
    if (gUnk_030023AC == 1)
        sub_08028b1c();
    else
        sub_08028990();
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
    if (gUnk_0200B050 == 2)
        gUnk_03005690[0] = gUnk_02004B80;
    sub_08029930();
    a = 0;
    CpuFastSet(&a, (u32 *)0x06002000, 0x01000400);
    if (gUnk_0200B050 == 1)
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

void sub_080235ec(s32 a)
{
    s32 id = sub_080058e4(3, 63);
    struct Task *t;

    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk14 = a;
    }
}
