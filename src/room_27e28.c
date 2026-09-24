#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* room_27e28.c (0x08027E28-0x0802831F, issue #93).
 *
 * Room start-up, part 1.  sub_08027e28 (the loaders' second step) sets up
 * the scroll speeds, the BG layout gUnk_0200B050/gUnk_0200B078 for the
 * room (a 7-way switch on RoomDef.unk54, table 0x08027F28), the
 * metatile-map edits of the special rooms and the bottom bound
 * (sub_08025e0c); sub_08028130 is its reduced form for sub_080233e0,
 * sub_08028280 its layout-only form for sub_08024698, and
 * sub_08028304 picks the tile-upload routine for the layout.
 * sub_08027a6c, the first function of the range (the second map buffer
 * gUnk_02006AA0 for sub_08023948/sub_08023ca0), is still asm. */

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

/* M08's view of a map cell (src/bgmap_2a9cc.c): the metatile index is a u16 */
struct MapTile
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};

extern s32 gUnk_03005610;
extern s32 gUnk_03005664;
extern s32 gUnk_030055F0;
extern s32 gUnk_03005618;
extern u8 gUnk_0200B050;
extern s16 *gUnk_0300558C;
extern s16 gUnk_0873A318[];
extern s16 gUnk_0873A398[];
extern u8 gUnk_02005574[];
extern struct RoomDef *gUnk_030055EC;
extern struct Unk020055D8 gUnk_020055D8;
extern s32 gUnk_087322C0[][2];
extern u8 gUnk_020055B0;
extern u8 gUnk_0200B078;
extern s32 gUnk_030055E8;
extern struct MapCell gUnk_02000040[];
extern s16 gUnk_03005620;
extern s16 gUnk_0300561C;
extern struct MapCell *gUnk_03005660;
extern u16 gUnk_02008160[];
extern s16 gUnk_030055E4;
extern s16 gUnk_03005628[4];
extern u16 gUnk_03005600[2];
extern s8 gUnk_030023B8;
extern u8 gUnk_02000020;
extern u16 gUnk_030055C0;
extern u8 gUnk_020069F0;
extern s8 gUnk_02006098[];
extern s8 gUnk_02007D64;
extern struct RoomDef **gUnk_087E1D58[][8];
extern s8 gUnk_030023EC;
extern s8 gUnk_0300238C;
extern s8 gUnk_03002468;
extern u16 gUnk_030012B0[];

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_08008c64(u16 a0);
void sub_08025e0c(void);
void sub_0802b460(u16 a);
void sub_0802b49c(u16 a);
s32 sub_08030074(s32 a);
s32 sub_08030100(u8 a);
void sub_080307b0(void);
void sub_080307cc(void);
void sub_080b6ea0(s32 a);

void sub_08027e28(void)
{
    s8 *e;
    struct MapCell *p;
    s32 i, j;
    s16 idx;

    gUnk_03005610 = 0;
    gUnk_03005664 = 0;
    gUnk_030055F0 = 0;
    gUnk_03005618 = 0;
    gUnk_0200B050 = 0;
    gUnk_0300558C = gUnk_0873A318;
    gUnk_02005574[0] = 0;
    if ((s16)gUnk_030055EC->unk38 != -1)
    {
        e = (s8 *)&gUnk_020055D8.unk4[(s16)gUnk_030055EC->unk38];
        if (e[1] == 7)
        {
            gUnk_030055F0 = gUnk_087322C0[e[2]][0];
            gUnk_03005618 = gUnk_087322C0[e[2]][1];
        }
        else if (e[1] == 8)
        {
            gUnk_0300558C = gUnk_0873A398;
        }
    }
    if (gUnk_030055EC->unk56 != 0)
        gUnk_0200B050 = 2;
    else if (gUnk_020055B0 == 2)
        gUnk_0200B050 = 1;
    else if (gUnk_020055B0 == 1)
        sub_0802b49c(0x8000);
    switch (gUnk_030055EC->unk54)
    {
    case 1:
        gUnk_0200B078 = 1;
        gUnk_0200B050 = 3;
        *gUnk_02005574 = 1;
        sub_0802b460(0x8000);
        gUnk_030055E8 = 0;
        i = 13;
        p = gUnk_02000040;
        for (; i <= 18; i++)
        {
            for (j = 0; j <= 16; j++)
            {
                idx = gUnk_03005620 * i + j;
                ((struct MapTile *)gUnk_03005660)[idx].unk0 = 0;
                ((struct MapTile *)gUnk_03005660)[idx].unk2 = 0;
                ((struct MapTile *)gUnk_03005660)[idx].unk3 = 0;
                gUnk_02008160[idx] = 0;
            }
        }
        CpuSet(p, p + 2048, (gUnk_030055E4 * 2) & 0x1FFFFF);
        CpuSet(gUnk_02008160, gUnk_02008160 + 2048, gUnk_030055E4 & 0x1FFFFF);
        gUnk_030055C0 = 5;
        break;
    case 2:
        gUnk_0200B078 = 2;
        break;
    case 3:
        gUnk_0200B078 = 3;
        gUnk_03005628[2] = gUnk_0300561C * 16 - gUnk_03005600[1] - 80;
        sub_08008c64(3);
        sub_08030074(gUnk_030023B8);
        break;
    case 4:
        gUnk_0200B078 = 6;
        gUnk_0200B050 = 4;
        *gUnk_02005574 = 1;
        sub_0802b460(0);
        sub_080b6ea0(8);
        gUnk_02000020 = 2;
        gUnk_030055C0 = 5;
        break;
    case 5:
        gUnk_0200B078 = 7;
        gUnk_0200B050 = 5;
        gUnk_03005628[1] = gUnk_03005600[0] + 120;
        sub_080b6ea0(9);
        gUnk_02000020 = 3;
        if (gUnk_020069F0 != 2)
            sub_08025e0c();
        break;
    case 6:
        gUnk_0200B078 = 4;
        gUnk_02006098[0] = 0;
        break;
    case 7:
        gUnk_0200B078 = 5;
        gUnk_02006098[0] = 1;
        break;
    }
    if (gUnk_02007D64 == 3)
        sub_08030100(gUnk_030023B8);
}

void sub_08028130(void)
{
    s8 *e;
    struct RoomDef *next;

    gUnk_03005610 = 0;
    gUnk_03005664 = 0;
    gUnk_030055F0 = 0;
    gUnk_03005618 = 0;
    gUnk_0200B050 = 0;
    gUnk_02005574[0] = 0;
    if ((s16)gUnk_030055EC->unk38 != -1)
    {
        e = (s8 *)&gUnk_020055D8.unk4[(s16)gUnk_030055EC->unk38];
        if (e[1] == 7)
        {
            gUnk_030055F0 = gUnk_087322C0[e[2]][0];
            gUnk_03005618 = gUnk_087322C0[e[2]][1];
        }
    }
    if (gUnk_030055EC->unk56 != 0)
        gUnk_0200B050 = 2;
    else if (gUnk_020055B0 == 2)
        gUnk_0200B050 = 1;
    else if (gUnk_020055B0 == 1)
        sub_0802b49c(0x8000);
    if (gUnk_030055EC->unk54 != 0)
    {
        gUnk_0200B078 = 4;
        if (gUnk_02006098[0] == 1)
        {
            next = gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_03002468 + 1];
            sub_080017e4(2, (u32)(next->unk18 + 1), (u32)gUnk_030012B0, *gUnk_030055EC->unk18);
            sub_080017e4(2, (u32)(next->unk28 + 1), (u32)gUnk_030012B0 + 0x1C0 - *gUnk_030055EC->unk28, *gUnk_030055EC->unk28);
        }
    }
}

void sub_08028280(s32 a)
{
    gUnk_03005610 = 0;
    gUnk_03005664 = 0;
    gUnk_030055F0 = 0;
    gUnk_03005618 = 0;
    gUnk_0200B050 = 0;
    gUnk_02005574[0] = 0;
    if (a == 0)
    {
        gUnk_0200B078 = 6;
        gUnk_0200B050 = 4;
        sub_0802b460(0);
        sub_080b6ea0(10);
    }
    else if (gUnk_020055B0 == 2)
    {
        gUnk_0200B050 = 1;
    }
    else if (gUnk_020055B0 == 1)
    {
        sub_0802b49c(0x8000);
    }
    gUnk_030055C0 = 5;
}

void sub_08028304(void)
{
    if (gUnk_0200B078 == 1)
        sub_080307cc();
    else
        sub_080307b0();
}
