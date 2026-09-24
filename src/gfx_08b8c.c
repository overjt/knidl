#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* gfx_08b8c.c (0x08008B8C-0x080091AB, issue #96).
 *
 * Screen/asset loaders (LZ77/Huffman decompression into VRAM). */

extern u32 gUnk_02020000[];
extern u32 gUnk_02028000[];
extern u32 gUnk_02030000[];
extern void (*gUnk_03000014)(void);
extern vu16 gUnk_03000038;
extern vu16 gUnk_03000048;
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000FA8;
extern vs16 gUnk_03000FB8;
extern u16 gUnk_03001270[];
extern u8 gUnk_03001370[];
extern u8 gUnk_030013B0[];
extern u32 gUnk_03001610[];
extern vu16 gUnk_03001ED8;
extern u8 gUnk_03001F30;
extern u32 gUnk_06010000[];
extern u8 gUnk_080D1B78[];
extern u8 gUnk_080D2AD0[];
extern u8 gUnk_0856F2A8[];
extern u8 gUnk_0856F308[];
extern u8 gUnk_0857014C[];
extern u8 gUnk_085704CC[];
extern u8 gUnk_085707D4[];
extern u8 gUnk_085708A8[];
extern u8 gUnk_085709EC[];
extern u8 gUnk_08570B28[];
extern u8 gUnk_08570F1C[];
extern u8 gUnk_0857111C[];
extern u8 gUnk_08571248[];
extern u8 gUnk_0857172C[];
extern u8 gUnk_08571838[];
extern u8 gUnk_08571BE0[];
extern u8 gUnk_08571D74[];
extern u8 gUnk_08572164[];
extern u8 gUnk_085A3CB8[];
extern u8 gUnk_085A4904[];
extern u8 gUnk_085A49E8[];
extern u8 gUnk_085CCB58[];
extern u16 *gUnk_08730884[];
extern struct TransferNode *gUnk_0873185C[];
extern u32 gUnk_08731980[][2][2];
extern u16 gUnk_087319B0[][2][2];
extern u32 gUnk_087319C8[][3];
extern u32 gUnk_08731A28[][3];
extern u8 gUnk_08731A88[];
extern u32 gUnk_08731A90[][2];
extern u32 gUnk_08731B70[];
extern u16 gUnk_08731B88[][2];
extern u32 gUnk_08731BA0[][2];

void SoftReset(u32 resetFlags);
void sub_0800157c(struct TransferNode *node);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_08002028(void);
void sub_0800214c(void);
void sub_080021dc(void);
void sub_080022ac(void);
void sub_080022d0(void);
void sub_08002d74(s32 count);
void sub_08002de0(void);
void sub_08002e38(u16 *p);
void sub_080031b8(s32 id);
void sub_08003484(void);
void sub_08006868(void);
void sub_080b6e60(void);
void sub_08008c4c(s32 a0);
void sub_08008c64(u16 a0);

void sub_08008b8c(void)
{
    sub_08002028();
    sub_080022ac();
    sub_08003484();
    sub_08006868();
    sub_080b6e60();
    gUnk_03000014 = 0;
    gUnk_03000FB8 = 31;
    sub_080022d0();
    sub_08008c4c(3);
    sub_08008c64(65);
    gUnk_03000B78 = gUnk_03000FA8 = 0;
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x800;
    sub_0800214c();
    sub_08002d74(32);
    gUnk_03000048 = 0;
    for (;;) {
        if (gUnk_03000038 & 9) {
            sub_080031b8(102);
            break;
        }
        if (gUnk_03000038 & 2) {
            sub_080031b8(215);
            break;
        }
        sub_080022d0();
    }
    sub_080021dc();
    sub_08002de0();
    SoftReset(0x1C);
}

void sub_08008c4c(s32 a0)
{
    sub_08002e38(gUnk_08730884[a0]);
}

void sub_08008c64(u16 a0)
{
    sub_0800157c(gUnk_0873185C[a0]);
}

void sub_08008c7c(void)
{
    u32 src;

    if (gUnk_03001F30 == 1) {
        src = (u32)gUnk_080D1B78;
        sub_080017e4(3, src, 0x060160C0, 64);
        src += 64;
        sub_080017e4(3, src, 0x060164C0, 64);
    }
}

void sub_08008cb8(void)
{
    u32 src;

    src = (u32)gUnk_080D2AD0;
    sub_080017e4(3, src, 0x060170E0, 224);
    sub_080017e4(3, src + 224, 0x060174E0, 224);
    sub_080017e4(3, src + 448, 0x060178E0, 224);
    sub_080017e4(3, src + 672, 0x06017CE0, 224);
}

void sub_08008d10(s32 a0, s32 a1)
{
    if (gUnk_08731980[a0][a1][0] != 0) {
        LZ77UnCompWram((void *)gUnk_08731980[a0][a1][0], gUnk_02020000);
        sub_080017e4(3, (u32)gUnk_02020000, (u32)gUnk_06010000, gUnk_087319B0[a0][a1][0] << 5);
    }
    if (gUnk_08731980[a0][a1][1] != 0) {
        LZ77UnCompWram((void *)gUnk_08731980[a0][a1][1], gUnk_02020000);
        sub_080017e4(4, (u32)gUnk_02020000, (u32)gUnk_06010000, gUnk_087319B0[a0][a1][1] << 5);
    }
}

void sub_08008d98(s32 a0)
{
    sub_080017e4(6, 0, 0x06001000, 0x800);
    sub_080017e4(6, 0, 0x06001800, 0x800);
    sub_080017e4(2, gUnk_087319C8[a0][0], (u32)gUnk_03001270, 64);
    LZ77UnCompVram((void *)gUnk_087319C8[a0][1], (void *)0x06000000);
    LZ77UnCompVram((void *)gUnk_087319C8[a0][2], (void *)0x06001000);
    if (a0 == 7)
        LZ77UnCompVram(gUnk_085CCB58, (void *)0x06001800);
}

void sub_08008e1c(s32 a0)
{
    sub_080017e4(2, gUnk_08731A28[a0][0], (u32)gUnk_03001370, gUnk_08731A88[a0] << 5);
    LZ77UnCompVram((void *)gUnk_08731A28[a0][1], (void *)0x06008000);
    LZ77UnCompVram((void *)gUnk_08731A28[a0][2], (void *)0x06003000);
}

void sub_08008e6c(s32 a0)
{
    sub_080017e4(2, (u32)gUnk_085A4904 + (a0 << 5), (u32)gUnk_030013B0, 32);
    sub_080017e4(8, (u32)gUnk_085A49E8, (u32)gUnk_02020000, 0);
    sub_080017e4(1, (u32)gUnk_02020000 + (a0 << 11), 0x06007800, 0x800);
}

void sub_08008ebc(void)
{
    sub_080017e4(6, 0, 0x06000400, 0x3E0);
}

void sub_08008ed4(s32 a0)
{
    sub_080017e4(2, gUnk_08731A90[a0][0] + 2, (u32)&gUnk_03001270[1], 30);
    sub_080017e4(1, gUnk_08731A90[a0][1], 0x06000400, 0x3E0);
}

/* The 0x02020000 / 0x02020100 buffer addresses must stay integer literals:
   as gUnk_02020000 symbols CSE keeps them in callee-saved registers across
   the calls and the if-block (-8 bytes, r7/r8 permutation). */
void sub_08008f10(s32 a0)
{
    sub_080017e4(2, gUnk_08731B70[a0], (u32)gUnk_03001610, 32);
    sub_080017e4(8, (u32)gUnk_085A3CB8, 0x02020000, 0);
    sub_080017e4(4, gUnk_08731B88[a0][0] + 0x02020000, 0x06017800, 0x100);
    sub_080017e4(4, gUnk_08731B88[a0][0] + 0x02020100, 0x06017C00, 0x100);
    if (gUnk_08731B88[a0][1] != 0xFFFF) {
        sub_080017e4(4, gUnk_08731B88[a0][1] + 0x02020000, 0x06017900, 0x100);
        sub_080017e4(4, gUnk_08731B88[a0][1] + 0x02020100, 0x06017D00, 0x100);
    }
}

void sub_08008fc4(s32 a0, s32 a1)
{
    switch (a0) {
    case 28:
        sub_080017e4(2, (u32)gUnk_0856F2A8, (u32)gUnk_03001370, 96);
        HuffUnComp(gUnk_0856F308, gUnk_02020000);
        LZ77UnCompVram(gUnk_02020000, (void *)0x06008800);
        HuffUnComp(gUnk_085707D4, gUnk_02028000);
        LZ77UnCompVram(gUnk_02028000, (void *)0x06008000);
        if (a1 != 0) {
            LZ77UnCompVram(gUnk_0857014C, (void *)0x0600F000);
            LZ77UnCompVram(gUnk_085708A8, (void *)0x0600F800);
        } else {
            LZ77UnCompVram(gUnk_085704CC, (void *)0x0600F000);
            LZ77UnCompVram(gUnk_085709EC, (void *)0x0600F800);
        }
        break;
    case 27:
        sub_080017e4(2, (u32)gUnk_0857111C, (u32)gUnk_03001370, 0x100);
        LZ77UnCompVram(gUnk_08570F1C, (void *)0x0600F800);
        HuffUnComp(gUnk_08570B28, gUnk_02020000);
        LZ77UnCompVram(gUnk_02020000, (void *)0x06008000);
        break;
    default:
        sub_080017e4(2, (u32)gUnk_0857111C, (u32)gUnk_03001370, 0x100);
        LZ77UnCompVram(gUnk_0857172C, (void *)0x0600E800);
        if (a1 != 0) {
            LZ77UnCompVram(gUnk_08571838, (void *)0x0600F000);
            LZ77UnCompVram(gUnk_08571BE0, (void *)0x0600F800);
        } else {
            LZ77UnCompVram(gUnk_08571D74, (void *)0x0600F000);
            LZ77UnCompVram(gUnk_08572164, (void *)0x0600F800);
        }
        HuffUnComp((void *)gUnk_08731BA0[a0][0], gUnk_02020000);
        LZ77UnCompVram(gUnk_02020000, (void *)0x06008000);
        HuffUnComp(gUnk_08571248, gUnk_02028000);
        LZ77UnCompVram(gUnk_02028000, (void *)0x06009600);
        HuffUnComp((void *)gUnk_08731BA0[a0][1], gUnk_02030000);
        LZ77UnCompVram(gUnk_02030000, (void *)0x0600B400);
        sub_080017e4(2, gUnk_08731A90[a0][0], (u32)gUnk_030013B0, 32);
        sub_080017e4(1, gUnk_08731A90[a0][1], 0x0600A800, 0x3E0);
        break;
    }
}
