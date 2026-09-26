#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* results_c6420.c (0x080C6420-0x080C6C63, issue #100).
 *
 * The screens around the ending and the tile-number helpers.
 *   sub_080c6420   AgbMain state 12, after the staff credits: the final screen
 *       (after AgbMain state 20 sub_080c6600's clock, in link play the clock,
 *       else this player's score), held until START.
 *   sub_080c6600   the clock drawn with large digit tiles.
 *   sub_080c6750 / sub_080c680c   a full-screen picture (screen 57 or 59) held
 *       until A or START; M02's game-state bodies and M03's file menu show it.
 *   sub_080c68b0 / sub_080c6ab4   draw an 8-digit score / a clock into the BG
 *       map at 0x06001000 (M02's src/hud_0aad0.c renderers, drawn to VRAM);
 *       M02's sub_08007f9c calls sub_080c6ab4 too.
 *   sub_080c6c3c   copy n map entries to column x, row y of that BG map. */

extern vu16 gUnk_03001ED8;          /* DISPCNT shadow */
extern vs32 gUnk_03000010;          /* BG0 16.16 scroll shadows ... */
extern vs32 gUnk_0300117C;
extern vs32 gUnk_03001E94;
extern vs32 gUnk_03000F8C;
extern vs32 gUnk_03000FA8;
extern vs32 gUnk_03000B78;          /* ... BG3 */
extern u8 gUnk_03001F30;            /* link-play mode */
extern u16 gUnk_03002150;           /* previous game state */
extern u16 gUnk_030023AC;           /* number of players */
extern u8 gUnk_03002464;
extern u16 gUnk_03002364;
extern u16 gUnk_02000028;
extern s32 gUnk_02006020[];         /* score per player */
extern u16 gUnk_02006068[];         /* clock (four fields) */
extern vu16 gUnk_03001EB8[];        /* keys pressed per player */
extern u32 gUnk_02020000[];         /* decompression buffer */
extern u16 gUnk_085A6F34[2][10];    /* digit tiles, top and bottom rows */
extern u16 gUnk_085A6F5C[];         /* the clock's colon tiles */
extern s8 gUnk_03001F08[];
extern vu16 gUnk_03000038;

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_08002028(void);
void sub_0800214c(void);
void sub_08002198(void);
void sub_080021dc(void);
/* Declared without its parameter: early_1fd0.c defines it as
   `u32 sub_080022a0(u32 arg)` (it returns arg unchanged, lesson 3.391) and
   this call sets up no argument (lesson 3.428). */
void sub_080022a0(void);
void sub_080022ac(void);
void sub_080022d0(void);
void sub_080022fc(void);
void sub_08002338(void);
void sub_08002358(void);
void sub_08002378(void);
void sub_08002d18(void);                                     /* run one frame */
void sub_08002d74(s32 count);
void sub_08002d94(s32 count);
void sub_08002de0(void);
void sub_08002e0c(void);
void sub_08002f14(s16 n);
s32 sub_08003110(s32 songId);
s32 sub_080031b8(s32 id);                                    /* play a sound effect */
void sub_0800374c(s32 speed);
void sub_08003770(u16 volume);
void sub_08008c4c(s32 a0);                                   /* load palette set */
void sub_08008c64(u16 a0);                                   /* load screen graphics */
void sub_080c6600(void);
void sub_080c68b0(s32 v, s32 x, s32 y);
void sub_080c6ab4(u16 *time, s32 x, s32 y);

void sub_080c6c3c(u16 *src, s32 x, s32 y, s32 n);

/* AgbMain state 12, after the staff credits: the final screen.  After
   AgbMain state 20 it shows sub_080c6600's clock screen, in link play the
   clock, otherwise this player's score; then it waits for START, fades out
   and returns (AgbMain goes back to state 0). */
void sub_080c6420(void)
{
    if (gUnk_03002150 != 20) {
        sub_08008c64(4);
        if (gUnk_03001F30 == 0) {
            if (gUnk_03002464 == 0)
                sub_08008c64(56);
            else
                sub_08008c64(58);
            sub_080c68b0(gUnk_02006020[gUnk_02000028], 22, 0);
        } else {
            sub_08008c64(55);
            sub_080c6ab4(gUnk_02006068, 22, 18);
        }
    } else {
        sub_08008c64(54);
        sub_080c6600();
    }
    sub_08002028();
    sub_080022ac();
    sub_08008c4c(7);
    gUnk_0300117C = gUnk_03000010 = 0;
    gUnk_03000F8C = gUnk_03001E94 = 0;
    gUnk_03000B78 = gUnk_03000FA8 = 0;
    if (gUnk_03002150 == 20) {
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x800;
    } else if (gUnk_03001F30 == 1) {
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x900;
    } else if (gUnk_03002364 & (16 << gUnk_03002464)) {
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x900;
    } else {
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0xD00;
    }
    sub_0800214c();
    sub_08002e0c();
    if (gUnk_03002150 == 20 || gUnk_03001F30 == 1) {
        sub_08003110(29);
        sub_08002d94(174);
    } else {
        sub_08003110(40);
        sub_08002d94(32);
    }
    do
        sub_08002d18();
    while (!(gUnk_03001EB8[0] & 8));
    sub_080031b8(102);
    sub_08002338();
    sub_0800374c(8);
    sub_08002198();
    sub_08002e0c();
    sub_08002d74(2);
    sub_08003770(255);
    sub_080022a0();
}

/* The final screen after AgbMain state 20: the clock drawn with the large
   digit tiles decompressed to gUnk_02020000 (two tiles per digit, the
   seconds in a second tile set). */
void sub_080c6600(void)
{
    sub_08008c64(53);
    sub_08002f14(gUnk_02006068[3]);
    sub_080017e4(1, (u32)gUnk_02020000 + (gUnk_03001F08[1] << 5), 0x0600AEA0, 32);
    sub_080017e4(1, (u32)gUnk_02020000 + ((gUnk_03001F08[1] + 16) << 5), 0x0600B0A0, 32);
    sub_080017e4(1, (u32)gUnk_02020000 + (gUnk_03001F08[0] << 5), 0x0600AEC0, 32);
    sub_080017e4(1, (u32)gUnk_02020000 + ((gUnk_03001F08[0] + 16) << 5), 0x0600B0C0, 32);
    sub_08002f14(gUnk_02006068[2]);
    sub_080017e4(1, (u32)gUnk_02020000 + (gUnk_03001F08[1] << 5), 0x0600AF00, 32);
    sub_080017e4(1, (u32)gUnk_02020000 + ((gUnk_03001F08[1] + 16) << 5), 0x0600B100, 32);
    sub_080017e4(1, (u32)gUnk_02020000 + (gUnk_03001F08[0] << 5), 0x0600AF20, 32);
    sub_080017e4(1, (u32)gUnk_02020000 + ((gUnk_03001F08[0] + 16) << 5), 0x0600B120, 32);
    sub_08002f14(gUnk_02006068[1]);
    sub_080017e4(1, (u32)gUnk_02020000 + ((gUnk_03001F08[1] + 32) << 5), 0x0600AF60, 32);
    sub_080017e4(1, (u32)gUnk_02020000 + ((gUnk_03001F08[1] + 48) << 5), 0x0600B160, 32);
    sub_080017e4(1, (u32)gUnk_02020000 + ((gUnk_03001F08[0] + 32) << 5), 0x0600AF80, 32);
    sub_080017e4(1, (u32)gUnk_02020000 + ((gUnk_03001F08[0] + 48) << 5), 0x0600B180, 32);
}

/* A full-screen picture (screen 57 or 59, by gUnk_03002464) shown until a
   player presses A or START; M02's game-state bodies show it once, after a
   stage when sub_080b8290() says so. */
void sub_080c6750(void)
{
    s32 i;

    sub_08002028();
    sub_08008c4c(7);
    if (gUnk_03002464 == 0)
        sub_08008c64(57);
    else
        sub_08008c64(59);
    gUnk_03000B78 = gUnk_03000FA8 = 0;
    sub_08002358();
    sub_08002378();
    sub_080022fc();
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x800;
    sub_0800214c();
    sub_08002e0c();
    sub_08002d94(32);
    do {
        sub_08002d18();
        for (i = 0; i < gUnk_030023AC; i++) {
            if (gUnk_03001EB8[i] & 9) {
                sub_080031b8(102);
                break;
            }
        }
    } while (i == gUnk_030023AC);
    sub_08002338();
    sub_080021dc();
    sub_08002e0c();
}

/* The same picture as sub_080c6750 for save slot `slot` (screen 57 or 59),
   shown from M03's file menu until A or START; the BG3 scroll is kept. */
void sub_080c680c(s32 slot)
{
    s32 x, y;

    sub_08002028();
    x = gUnk_03000B78;
    y = gUnk_03000FA8;
    sub_08008c4c(7);
    if (slot == 0)
        sub_08008c64(57);
    else
        sub_08008c64(59);
    gUnk_03000B78 = gUnk_03000FA8 = 0;
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x800;
    sub_0800214c();
    sub_08002de0();
    sub_08002d74(32);
    do
        sub_080022d0();
    while (!(gUnk_03000038 & 9));
    sub_080031b8(102);
    sub_080021dc();
    sub_08002de0();
    gUnk_03000B78 = x;
    gUnk_03000FA8 = y;
}

/* Draw an 8-digit decimal score at column x, rows y and y + 1 of the BG map. */
void sub_080c68b0(s32 v, s32 x, s32 y)
{
    s32 d;

    if (v >= 0) {
        d = -1;
        while (v >= 0) {
            v -= 10000000;
            d++;
        }
        v += 10000000;
        sub_080c6c3c(&gUnk_085A6F34[0][d], x, y, 1);
        sub_080c6c3c(&gUnk_085A6F34[1][d], x, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 1000000;
            d++;
        }
        v += 1000000;
        sub_080c6c3c(&gUnk_085A6F34[0][d], x + 1, y, 1);
        sub_080c6c3c(&gUnk_085A6F34[1][d], x + 1, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 100000;
            d++;
        }
        v += 100000;
        sub_080c6c3c(&gUnk_085A6F34[0][d], x + 2, y, 1);
        sub_080c6c3c(&gUnk_085A6F34[1][d], x + 2, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 10000;
            d++;
        }
        v += 10000;
        sub_080c6c3c(&gUnk_085A6F34[0][d], x + 3, y, 1);
        sub_080c6c3c(&gUnk_085A6F34[1][d], x + 3, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 1000;
            d++;
        }
        v += 1000;
        sub_080c6c3c(&gUnk_085A6F34[0][d], x + 4, y, 1);
        sub_080c6c3c(&gUnk_085A6F34[1][d], x + 4, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 100;
            d++;
        }
        v += 100;
        sub_080c6c3c(&gUnk_085A6F34[0][d], x + 5, y, 1);
        sub_080c6c3c(&gUnk_085A6F34[1][d], x + 5, y + 1, 1);

        d = -1;
        while (v >= 0) {
            v -= 10;
            d++;
        }
        v += 10;
        sub_080c6c3c(&gUnk_085A6F34[0][d], x + 6, y, 1);
        sub_080c6c3c(&gUnk_085A6F34[1][d], x + 6, y + 1, 1);

        sub_080c6c3c(&gUnk_085A6F34[0][v], x + 7, y, 1);
        sub_080c6c3c(&gUnk_085A6F34[1][v], x + 7, y + 1, 1);
    }
}

/* Draw a clock (time[3] : time[2] : time[1], two digits each) at column x,
   rows y and y + 1 of the BG map. */
void sub_080c6ab4(u16 *time, s32 x, s32 y)
{
    s32 v;
    s32 d;

    v = time[3];
    d = -1;
    while (v >= 0) {
        v -= 10;
        d++;
    }
    v += 10;
    sub_080c6c3c(&gUnk_085A6F34[0][d], x, y, 1);
    sub_080c6c3c(&gUnk_085A6F34[1][d], x, y + 1, 1);
    sub_080c6c3c(&gUnk_085A6F34[0][v], x + 1, y, 1);
    sub_080c6c3c(&gUnk_085A6F34[1][v], x + 1, y + 1, 1);
    sub_080c6c3c(gUnk_085A6F5C, x + 2, y, 1);
    sub_080c6c3c(gUnk_085A6F5C + 1, x + 2, y + 1, 1);

    v = time[2];
    d = -1;
    while (v >= 0) {
        v -= 10;
        d++;
    }
    v += 10;
    sub_080c6c3c(&gUnk_085A6F34[0][d], x + 3, y, 1);
    sub_080c6c3c(&gUnk_085A6F34[1][d], x + 3, y + 1, 1);
    sub_080c6c3c(&gUnk_085A6F34[0][v], x + 4, y, 1);
    sub_080c6c3c(&gUnk_085A6F34[1][v], x + 4, y + 1, 1);
    sub_080c6c3c(gUnk_085A6F5C, x + 5, y, 1);
    sub_080c6c3c(gUnk_085A6F5C + 1, x + 5, y + 1, 1);

    v = time[1];
    d = -1;
    while (v >= 0) {
        v -= 10;
        d++;
    }
    v += 10;
    sub_080c6c3c(&gUnk_085A6F34[0][d], x + 6, y, 1);
    sub_080c6c3c(&gUnk_085A6F34[1][d], x + 6, y + 1, 1);
    sub_080c6c3c(&gUnk_085A6F34[0][v], x + 7, y, 1);
    sub_080c6c3c(&gUnk_085A6F34[1][v], x + 7, y + 1, 1);
}

/* Copy n tiles from src to the BG map at 0x06001000, row y, column x. */
void sub_080c6c3c(u16 *src, s32 x, s32 y, s32 n)
{
    sub_080017e4(1, (u32)src, (x + (y << 5)) * 2 + 0x06001000, n * 2);
}
