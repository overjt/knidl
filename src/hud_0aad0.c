#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* hud_0aad0.c (0x0800AAD0-0x0800B317, issue #96).
 *
 * HUD/score drawing. */

extern s16 gUnk_02005580;
extern u8 gUnk_02006014;
extern u8 gUnk_02006030;
extern u16 gUnk_02006068[];
extern u8 gUnk_0200B028;
extern u16 gUnk_03000498[4];
extern s8 gUnk_03001F08[];
extern s8 gUnk_030023B8;
extern s8 gUnk_03002444;
extern u16 gUnk_085A5654[];
extern u8 gUnk_085A6714[];
extern u16 gUnk_085A6F34[2][10];
extern u16 gUnk_085A6F5C[];
extern u16 gUnk_085A6F60[];
extern u16 gUnk_085A6F64[];
extern u16 gUnk_085A6F68[][5];
extern u16 gUnk_085A6FA4[2][4][2];
extern u16 gUnk_085A6FC4[];
extern u16 gUnk_085A6FC8[];
extern u16 gUnk_085A6FF0[];
extern u16 gUnk_085A6FF8[];
extern u16 gUnk_085A6FFC[];

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_08002f14(s16 n);
void sub_08008e6c(s32 a0);
void sub_0800b318(u16 *src, s32 x, s32 y, s32 n);
void sub_0800b34c(s32 x, s32 y, s32 n);
void sub_0800af40(u16 *time);

void sub_0800aad0(void)
{
    if (gUnk_03000498[3] > 99) {
        gUnk_02006068[3] = 99;
        gUnk_02006068[2] = 59;
        gUnk_02006068[1] = 59;
        gUnk_02006068[0] = 59;
    } else {
        gUnk_02006068[3] = gUnk_03000498[3];
        gUnk_02006068[2] = gUnk_03000498[2];
        gUnk_02006068[1] = gUnk_03000498[1];
        gUnk_02006068[0] = gUnk_03000498[0];
    }
}

void sub_0800ab08(void)
{
    if (gUnk_02006014 == 1 && gUnk_0200B028 != 1 && gUnk_02006030 == 1)
        sub_0800af40(gUnk_02006068);
}

void sub_0800ab3c(void)
{
    sub_08008e6c(gUnk_030023B8);
    sub_0800b318(gUnk_085A5654, 0, 0, 64);
}

void sub_0800ab64(s32 a)
{
    if (gUnk_02006014 == 1) {
        sub_0800b318(gUnk_085A6FA4[0][a], 7, 18, 2);
        sub_0800b318(gUnk_085A6FA4[1][a], 7, 19, 2);
        sub_0800b318(gUnk_085A6FC4, 9, 18, 1);
        sub_0800b318(gUnk_085A6FC4 + 1, 9, 19, 1);
    }
}

void sub_0800abc0(s32 n)
{
    if (gUnk_02006014 == 1) {
        sub_08002f14(n);
        sub_0800b318(&gUnk_085A6F34[0][gUnk_03001F08[1]], 10, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][gUnk_03001F08[1]], 10, 19, 1);
        sub_0800b318(&gUnk_085A6F34[0][gUnk_03001F08[0]], 11, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][gUnk_03001F08[0]], 11, 19, 1);
    }
}

void sub_0800ac38(s32 n)
{
    s32 i;
    s32 max;

    if (gUnk_02006014 == 1) {
        max = gUnk_02005580 >> 3;
        for (i = 0; i < max; i++) {
            if (i < n) {
                sub_0800b318(gUnk_085A6F60, i + 13, 18, 1);
                sub_0800b318(gUnk_085A6F60 + 1, i + 13, 19, 1);
            } else {
                sub_0800b318(gUnk_085A6F64, i + 13, 18, 1);
                sub_0800b318(gUnk_085A6F64 + 1, i + 13, 19, 1);
            }
        }
    }
}

void sub_0800acbc(s32 a, s32 d)
{
    s32 i;
    s32 max;

    if (gUnk_02006014 == 1 && d != 0) {
        max = gUnk_02005580 >> 3;
        if (d > 0) {
            for (i = a + d; i > a; i--) {
                if (i <= max) {
                    sub_0800b318(gUnk_085A6F60, i + 12, 18, 1);
                    sub_0800b318(gUnk_085A6F60 + 1, i + 12, 19, 1);
                }
            }
        } else {
            for (i = a; i > a + d; i--) {
                if (i <= max) {
                    sub_0800b318(gUnk_085A6F64, i + 12, 18, 1);
                    sub_0800b318(gUnk_085A6F64 + 1, i + 12, 19, 1);
                }
            }
        }
    }
}

void sub_0800ad68(s32 v)
{
    s32 d;

    if (gUnk_02006014 == 1 && v >= 0) {
        d = -1;
        while (v >= 0) {
            v -= 10000000;
            d++;
        }
        v += 10000000;
        sub_0800b318(&gUnk_085A6F34[0][d], 22, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][d], 22, 19, 1);

        d = -1;
        while (v >= 0) {
            v -= 1000000;
            d++;
        }
        v += 1000000;
        sub_0800b318(&gUnk_085A6F34[0][d], 23, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][d], 23, 19, 1);

        d = -1;
        while (v >= 0) {
            v -= 100000;
            d++;
        }
        v += 100000;
        sub_0800b318(&gUnk_085A6F34[0][d], 24, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][d], 24, 19, 1);

        d = -1;
        while (v >= 0) {
            v -= 10000;
            d++;
        }
        v += 10000;
        sub_0800b318(&gUnk_085A6F34[0][d], 25, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][d], 25, 19, 1);

        d = -1;
        while (v >= 0) {
            v -= 1000;
            d++;
        }
        v += 1000;
        sub_0800b318(&gUnk_085A6F34[0][d], 26, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][d], 26, 19, 1);

        d = -1;
        while (v >= 0) {
            v -= 100;
            d++;
        }
        v += 100;
        sub_0800b318(&gUnk_085A6F34[0][d], 27, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][d], 27, 19, 1);

        d = -1;
        while (v >= 0) {
            v -= 10;
            d++;
        }
        v += 10;
        sub_0800b318(&gUnk_085A6F34[0][d], 28, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][d], 28, 19, 1);

        sub_0800b318(&gUnk_085A6F34[0][v], 29, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][v], 29, 19, 1);
    }
}

void sub_0800af40(u16 *time)
{
    s32 v;
    s32 d;

    if (gUnk_02006014 == 1) {
        v = time[3];
        d = -1;
        while (v >= 0) {
            v -= 10;
            d++;
        }
        v += 10;
        sub_0800b318(&gUnk_085A6F34[0][d], 22, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][d], 22, 19, 1);
        sub_0800b318(&gUnk_085A6F34[0][v], 23, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][v], 23, 19, 1);
        sub_0800b318(gUnk_085A6F5C, 24, 18, 1);
        sub_0800b318(gUnk_085A6F5C + 1, 24, 19, 1);

        v = time[2];
        d = -1;
        while (v >= 0) {
            v -= 10;
            d++;
        }
        v += 10;
        sub_0800b318(&gUnk_085A6F34[0][d], 25, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][d], 25, 19, 1);
        sub_0800b318(&gUnk_085A6F34[0][v], 26, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][v], 26, 19, 1);
        sub_0800b318(gUnk_085A6F5C, 27, 18, 1);
        sub_0800b318(gUnk_085A6F5C + 1, 27, 19, 1);

        v = time[1];
        d = -1;
        while (v >= 0) {
            v -= 10;
            d++;
        }
        v += 10;
        sub_0800b318(&gUnk_085A6F34[0][d], 28, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][d], 28, 19, 1);
        sub_0800b318(&gUnk_085A6F34[0][v], 29, 18, 1);
        sub_0800b318(&gUnk_085A6F34[1][v], 29, 19, 1);
    }
}

void sub_0800b0a4(s32 n)
{
    s32 i, j;

    if (gUnk_02006014 == 1) {
        for (i = 0; i < n; i = j) {
            j = i + 1;
            sub_0800b318(gUnk_085A6F68[n - j], 0, 19 - i, 5);
        }
        for (i = n; i <= 5; i++)
            sub_0800b34c(0, 19 - i, 5);
    }
}

void sub_0800b0fc(void)
{
    if (gUnk_02006014 == 1) {
        sub_0800b318(gUnk_085A6FC8, 20, 18, 10);
        sub_0800b318(gUnk_085A6FC8 + 10, 20, 19, 10);
    }
}

void sub_0800b130(s32 x)
{
    s32 i;

    if (gUnk_02006014 == 1) {
        for (i = 0; i < 8; i++) {
            if (i < x >> 2)
                sub_0800b318(gUnk_085A6FF8, i + 21, 19, 1);
            else if (i == x >> 2)
                sub_0800b318(gUnk_085A6FF0 + (x & 3), i + 21, 19, 1);
            else
                sub_0800b318(gUnk_085A6FF0, i + 21, 19, 1);
        }
    }
}

void sub_0800b190(s32 from, s32 to)
{
    s32 i;
    s32 end;
    s32 d;

    if (gUnk_02006014 == 1) {
        if (to > 32)
            to = 8;
        if (to < 0)
            to = 0;
        if (from > 32)
            from = 8;
        if (from < 0)
            from = 0;
        d = to - from;
        if (d != 0) {
            if (d > 0) {
                for (i = from >> 2, end = to >> 2; i < end; i++) {
                    if (i <= 7)
                        sub_0800b318(gUnk_085A6FF8, i + 21, 19, 1);
                }
            } else {
                for (i = from >> 2, end = to >> 2; i < end; i--) {
                    if (i <= 7)
                        sub_0800b318(gUnk_085A6FF0, i + 21, 19, 1);
                }
            }
            if (end <= 7)
                sub_0800b318(gUnk_085A6FF0 + (to & 3), end + 21, 19, 1);
        }
    }
}

void sub_0800b230(s32 a, s32 b)
{
    s32 row;

    if (gUnk_02006014 == 1) {
        row = gUnk_03002444 ? 2 : 0;
        if (b == 0) {
            sub_0800b34c(12, row, 16);
            sub_0800b34c(12, row + 1, 16);
        } else {
            b--;
            sub_080017e4(1, (u32)gUnk_085A6714 + (b << 10), 0x06000C00, 0x400);
            sub_0800b318(gUnk_085A6FFC, 12, row, 16);
            sub_0800b318(gUnk_085A6FFC + 16, 12, row + 1, 16);
            if (b == 0) {
                sub_0800b318(gUnk_085A6FA4[0][a], 23, row, 2);
                sub_0800b318(gUnk_085A6FA4[1][a], 23, row + 1, 2);
            } else {
                sub_0800b318(gUnk_085A6FA4[0][a], 26, row, 2);
                sub_0800b318(gUnk_085A6FA4[1][a], 26, row + 1, 2);
            }
        }
    }
}
