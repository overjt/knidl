#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* menu_0b920.c (0x0800B920-0x0800C09B, issue #99).
 *
 * AgbMain state 4 (sub_0800b920), the main menu: it resets the menu cells,
 * spawns the background tasks #256-#259, picks the first screen from the
 * return state gUnk_03002150 (3 = file select, 14-16/20/21 = back from an
 * extra mode, straight to the mode list) and dispatches on the menu
 * screen gUnk_020060D0 until it reaches 9 (start a game: state 5 or 13)
 * or 10 (back to the title).  The rest draws the file-select screen's
 * three save slots: sub_0800bcf0 all three, sub_0800bda4 a slot's label
 * (empty, finished, or its number through the digit buffer gUnk_03001F08),
 * sub_0800be8c/sub_0800bf10 its picture and palette, sub_0800bf6c its
 * second number. */

struct SaveSlot
{
    /*0x00*/ u32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ u32 unk08;
    /*0x0C*/ s32 unk0C;
    /*0x10*/ u16 unk10;
    /*0x12*/ u16 unk12[2];
    /*0x16*/ u8 unk16[2];
    /*0x18*/ u8 unk18[2];
    /*0x1A*/ u8 unk1A[2];
    /*0x1C*/ u8 unk1C[2];
    /*0x1E*/ u8 pad1E[2];
    /*0x20*/ u32 unk20[2];
    /*0x28*/ u8 unk28[8][7];
    /*0x60*/ u16 unk60[4];
    /*0x68*/ u16 unk68[4];
    /*0x70*/ u32 unk70;
    /*0x74*/ u8 filler74[0x8C];
};

extern s8 gUnk_02000004;
extern u32 gUnk_02004B70;
extern u8 gUnk_02006090;
extern s8 gUnk_020060D0;
extern s8 gUnk_02006180;
extern u8 gUnk_02007FC0;
extern s16 gUnk_0200A6E0[];
extern s8 gUnk_0200B074;
extern struct SaveSlot gUnk_0200E600[];
extern u32 gUnk_0200EC48;
extern vu16 gUnk_03000048;
extern vs16 gUnk_030004A4;
extern vs32 gUnk_03000FA8;
extern vs16 gUnk_03000FB8;
extern vu16 gUnk_03000FC8;
extern vu16 gUnk_03001174;
extern u16 gUnk_03001490[];
extern vu16 gUnk_03001E90;
extern vu16 gUnk_03001E98;
extern vu16 gUnk_03001ECC;
extern u16 *gUnk_03001ED4;
extern vu16 gUnk_03001ED8;
extern s8 gUnk_03001F08[];
extern u8 gUnk_03001F30;
extern u16 gUnk_03002150;
extern u16 gUnk_030023AC;
extern u16 gUnk_030023D8;
extern s32 gUnk_030023E8;
extern s16 gUnk_0300244C;
extern u32 gUnk_03004D70;
extern u32 gUnk_06010000[];
extern u8 gUnk_08551110[];
extern u8 gUnk_08553210[];
extern u8 gUnk_08553510[];
extern u8 gUnk_08553810[];
extern u16 gUnk_08554B78[];
extern u16 gUnk_08731E18[];
extern u16 gUnk_08731E1E[2][3];
extern u8 *gUnk_08731E2C[2];

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_08002028(void);
void sub_080021dc(void);
void sub_080022ac(void);
void sub_080022e4(void);
void sub_08002338(void);
void sub_08002d54(s32 count);
void sub_08002db4(void);
void sub_08002e0c(void);
void sub_08002f14(s16 n);
s32 sub_08003110(s32 songId);
void sub_0800374c(s32 speed);
void sub_08003964(void);
void sub_08004000(u16 a);
s32 sub_080058e4(u32 type, s32 idx);
void sub_08006868(void);
void sub_08008c4c(s32 a0);
void sub_08008c64(u16 a0);
void sub_0800b4a8(void);
void sub_0800c09c(void);
void sub_0800c34c(void);
void sub_0800c558(void);
void sub_0800c610(void);
void sub_0800c8a0(void);
void sub_0800ca10(void);
void sub_0800cd60(void);
void sub_0800d0f4(void);
void sub_0800d450(void);
void sub_0800d85c(void);
void sub_0800da9c(s32 mode);
void sub_0800fcbc(void);
void sub_0800fdb8(s32 speed, s32 dist, s32 bg);
void sub_08010020(s32 a, s32 b, s32 c, s32 d);
void sub_080b6e44(void);
void sub_080b78e4(void);
void sub_080b8070(s32 a);
void sub_080b81a0(s32 a);
s32 sub_080b8290(void);
void sub_080b8888(void);
void sub_080b8918(void);
void sub_0800bcf0(void);
void sub_0800bda4(s32 slot);
void sub_0800be8c(s32 slot, u32 pal);
s32 sub_0800bf10(s32 slot, u32 pal);
void sub_0800bf6c(s32 slot, s32 value, s32 mode);

void sub_0800b920(void)
{
    sub_08003964();
    sub_08006868();
    gUnk_03004D70 = 0;
    sub_080022ac();
    sub_08002028();
    sub_080b6e44();
    if (gUnk_030023E8 == -1 || gUnk_030023E8 == 3)
        sub_080b78e4();
    gUnk_0200A6E0[0] = 0;
    gUnk_0200A6E0[1] = 0;
    gUnk_03000FC8 = 10;
    gUnk_03001ECC = 6;
    gUnk_02006180 = 0;
    sub_08008c4c(2);
    sub_08008c64(22);
    sub_0800fcbc();
    sub_080058e4(0x101, 32);
    sub_080058e4(0x102, 32);
    sub_080058e4(0x103, 32);
    sub_080058e4(0x100, 32);
    switch (gUnk_03002150)
    {
    case 3:
        gUnk_03000FA8 = 64;
        sub_0800fdb8(0x80000, 80, 3);
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1C00;
        gUnk_020060D0 = 0;
        sub_0800bcf0();
        sub_0800da9c(0);
        break;
    case 14:
    case 15:
    case 16:
    case 20:
    case 21:
        gUnk_03000FA8 = 64;
        sub_0800fdb8(0x80000, 80, 3);
        if (gUnk_03002150 <= 16)
            gUnk_0200B074 = gUnk_03002150 - 14;
        else if (gUnk_03002150 == 20)
            gUnk_0200B074 = 3;
        else
            gUnk_0200B074 = 4;
        gUnk_020060D0 = 11;
        sub_0800ca10();
        gUnk_02004B70 = 0;
        break;
    }
    sub_08003110(40);
    gUnk_03001E90 = 10;
    gUnk_03000FB8 = 30;
    gUnk_030004A4 = -3;
    gUnk_03001174 = 0;
    gUnk_03001E98 = 1;
    gUnk_03000048 = 1;
    gUnk_03001ED4 = 0;
    sub_08002db4();
    sub_08002d54(6);
    do
    {
        switch (gUnk_020060D0)
        {
        case 0:
            sub_0800c09c();
            break;
        case 1:
            sub_0800c34c();
            break;
        case 2:
            sub_0800c558();
            break;
        case 3:
            sub_0800c610();
            break;
        case 4:
            sub_0800cd60();
            break;
        case 5:
            sub_0800d0f4();
            break;
        case 6:
            sub_0800c8a0();
            break;
        case 7:
            sub_0800d450();
            break;
        case 8:
            sub_0800d85c();
            break;
        }
    } while (gUnk_020060D0 != 9 && gUnk_020060D0 != 10);
    sub_080021dc();
    if (gUnk_0200EC48 == 2)
    {
        sub_0800374c(16);
        while (gUnk_03001E90 != 0)
        {
            sub_080022e4();
            sub_08004000(gUnk_02000004);
        }
        gUnk_03000048 = 0;
        gUnk_03001F30 = 0;
        gUnk_030023D8 = 13;
    }
    else
    {
        if (gUnk_030023D8 == 5 || gUnk_030023D8 == 3)
            sub_0800374c(16);
        if (gUnk_030023AC > 1)
        {
            if (gUnk_030023D8 == 5)
            {
                sub_080b8888();
                sub_080b8918();
            }
            sub_08002338();
            sub_08002e0c();
        }
        else
        {
            gUnk_0300244C = 1;
            sub_08002db4();
        }
    }
    sub_08010020(0, 0, 0, 0);
    gUnk_03001ED8 &= 0xDFFF;
    switch (gUnk_030023D8)
    {
    case 3: /* empty but load-bearing: it adds the `cmp #5; ble` split */
        break;
    case 5:
        sub_080b8070(gUnk_030023E8);
        sub_080b8290();
        sub_0800b4a8();
        gUnk_02007FC0 = 1;
        break;
    case 13:
        if (gUnk_02006090 == 6 || gUnk_02006090 == 7)
        {
            sub_080b8070(gUnk_030023E8);
            sub_080b8290();
            if (gUnk_02006090 == 7)
                sub_080b81a0(gUnk_030023E8);
        }
        gUnk_03002150 = 4;
        break;
    }
}

void sub_0800bcf0(void)
{
    s32 i;

    gUnk_0200B074 = gUnk_030023E8;
    sub_08008c64(18);
    for (i = 0; i < 3; i++)
    {
        sub_0800bda4(i);
        sub_0800be8c(i, (s8)gUnk_0200E600[i].unk16[gUnk_0200E600[i].unk12[1] ? (gUnk_0200E600[i].unk04 != 0x99999999) : 0]);
        sub_0800bf6c(i, gUnk_0200E600[i].unk12[gUnk_0200E600[i].unk12[1] ? (gUnk_0200E600[i].unk04 != 0x99999999) : 0], 0);
    }
}

void sub_0800bda4(s32 slot)
{
    s32 n;

    if (gUnk_0200E600[slot].unk04 == 0x99999999)
    {
        sub_080017e4(3, (u32)gUnk_08553510, (u32)gUnk_06010000 + ((slot * 64 + 576) << 5), 0x180);
        sub_080017e4(3, (u32)&gUnk_08553510[0x180], (u32)gUnk_06010000 + ((slot * 64 + 608) << 5), 0x180);
    }
    else if (gUnk_0200E600[slot].unk10 & 2)
    {
        sub_080017e4(3, (u32)gUnk_08553210, (u32)gUnk_06010000 + ((slot * 64 + 576) << 5), 0x180);
        sub_080017e4(3, (u32)&gUnk_08553210[0x180], (u32)gUnk_06010000 + ((slot * 64 + 608) << 5), 0x180);
    }
    else
    {
        sub_08002f14(gUnk_0200E600[slot].unk12[0]);
        n = (gUnk_03001F08[1] + gUnk_03001F08[2] * 10) * 2;
        sub_080017e4(3, (u32)&gUnk_08551110[n * 0x180], (u32)gUnk_06010000 + ((slot * 64 + 576) << 5), 0x180);
        n++;
        sub_080017e4(3, (u32)&gUnk_08551110[n * 0x180], (u32)gUnk_06010000 + ((slot * 64 + 608) << 5), 0x180);
    }
}

void sub_0800be8c(s32 slot, u32 pal)
{
    s32 i;

    if (pal > 6)
        pal = 7;
    i = pal * 15;
    sub_080017e4(3, (u32)&gUnk_08553810[i * 32], (u32)gUnk_06010000 + (gUnk_08731E18[slot] << 5), 160);
    sub_080017e4(3, (u32)&gUnk_08553810[(i + 5) * 32], (u32)gUnk_06010000 + ((gUnk_08731E18[slot] + 32) << 5), 160);
    sub_080017e4(3, (u32)&gUnk_08553810[(i + 10) * 32], (u32)gUnk_06010000 + ((gUnk_08731E18[slot] + 64) << 5), 160);
    sub_0800bf10(slot, pal);
}

s32 sub_0800bf10(s32 slot, u32 pal)
{
    if (pal > 6)
        pal = 7;
    if (slot == gUnk_0200B074)
        sub_080017e4(2, (u32)&gUnk_08554B78[pal * 16], (u32)&gUnk_03001490[slot * 16], 32);
    else
        sub_080017e4(2, (u32)&gUnk_08554B78[(pal + 8) * 16], (u32)&gUnk_03001490[slot * 16], 32);
}

void sub_0800bf6c(s32 slot, s32 value, s32 mode)
{
    sub_08002f14(value);
    if (value < 0 || value > 100)
    {
        gUnk_03001F08[1] = 10;
        gUnk_03001F08[2] = 10;
        gUnk_03001F08[0] = 0;
    }
    else if (value < 10)
    {
        gUnk_03001F08[1] = 10;
        gUnk_03001F08[2] = 10;
    }
    else if (value < 100)
    {
        gUnk_03001F08[2] = 10;
    }

    switch (mode)
    {
    case 0:
        if (value == 100)
        {
            gUnk_03001F08[1] = 11;
            gUnk_03001F08[0] = 12;
        }
        sub_080017e4(3, (u32)(gUnk_08731E2C[0] + gUnk_03001F08[1] * 32), (u32)gUnk_06010000 + ((gUnk_08731E1E[0][slot] + 1) << 5), 32);
        sub_080017e4(3, (u32)(gUnk_08731E2C[0] + gUnk_03001F08[0] * 32), (u32)gUnk_06010000 + ((gUnk_08731E1E[0][slot] + 2) << 5), 32);
        break;
    case 1:
        sub_080017e4(3, (u32)(gUnk_08731E2C[1] + gUnk_03001F08[2] * 32), (u32)gUnk_06010000 + (gUnk_08731E1E[mode][slot] << 5), 32);
        sub_080017e4(3, (u32)(gUnk_08731E2C[1] + gUnk_03001F08[1] * 32), (u32)gUnk_06010000 + ((gUnk_08731E1E[mode][slot] + 1) << 5), 32);
        sub_080017e4(3, (u32)(gUnk_08731E2C[1] + gUnk_03001F08[0] * 32), (u32)gUnk_06010000 + ((gUnk_08731E1E[mode][slot] + 2) << 5), 32);
        break;
    }
}
