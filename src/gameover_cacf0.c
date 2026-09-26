#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* gameover_cacf0.c (0x080CACF0-0x080CB353, issue #100).
 *
 * AgbMain state 22, the game-over / continue screen.
 *   sub_080cacf0   the state body: one of three screens - sub_080cad8c
 *       outside link play, sub_080caeec in link play, sub_080cb058 after
 *       AgbMain state 20 - then, when the choice set game state 5 (continue),
 *       back into the stage (state 6 unless gUnk_03002438 is 1), else the SIO
 *       session is torn down.
 *   sub_080cb21c / sub_080cb2cc   the screen's graphics and objects: with
 *       one player task type #261 and #264 variants 0-2 (variant 0's task
 *       index goes to gUnk_02007D28), else #264 variant 5.
 *   sub_080cb030   redraw the clock for n frames.
 *   sub_080cb0e8 / sub_080cb108   up or down flips the cursor gUnk_02006164.
 *   sub_080cb12c / sub_080cb178 / sub_080cb1d8   A or START (or the end of
 *       the 480-frame count gUnk_0200557C) ends the screen (gUnk_02007BE0)
 *       and picks the next game state.
 *   sub_080cb2b0   reset the done flag and the count. */

extern vu16 gUnk_03001ED8;          /* DISPCNT shadow */
extern vs32 gUnk_03000FC0;
extern vs32 gUnk_03001EE0;
extern vs32 gUnk_03001E94;
extern vs32 gUnk_03000F8C;
extern vs32 gUnk_03000FA8;
extern vs32 gUnk_03000B78;          /* ... BG3 */
extern u8 gUnk_03001F30;            /* link-play mode */
extern u16 gUnk_03002150;           /* previous game state */
extern u16 gUnk_030023D8;           /* game state (AgbMain dispatch) */
extern u16 gUnk_03002360;           /* this player's index */
extern u16 gUnk_030023AC;           /* number of players */
extern s32 gUnk_02006020[];         /* score per player */
extern u16 gUnk_02006068[];         /* clock (four fields) */
extern vu16 gUnk_03001EB8[];        /* keys pressed per player */
extern u32 gUnk_02020000[];         /* decompression buffer */
extern u8 gUnk_02007BE0;            /* game-over screen: done flag */
extern s16 gUnk_0200557C;           /* game-over screen: frames left */
extern s8 gUnk_02006164;            /* game-over screen: cursor (continue = 0?) */
extern s16 gUnk_02007D28;           /* game-over screen: the #264 variant-0 task's index */
extern u32 gUnk_06010000[];         /* OBJ VRAM */
extern vu16 gUnk_03000FC8;
extern vu16 gUnk_03001ECC;
extern u8 gUnk_02007FC0;
extern s8 gUnk_03002438;
extern u32 gUnk_085E2C20[];
extern u32 gUnk_085E2CE0[];
extern u32 gUnk_085E4064[];
extern u32 gUnk_085E5BC4[];
extern u16 gUnk_030014F0[];

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_08002028(void);
void sub_0800214c(void);
void sub_080021dc(void);
void sub_080022ac(void);
void sub_080022fc(void);
void sub_08002338(void);
void sub_08002358(void);
void sub_08002378(void);
void sub_08002668(void);
void sub_0800293c(void);
void sub_08002d18(void);                                     /* run one frame */
void sub_08002d94(s32 count);
void sub_08002e0c(void);
s32 sub_08003110(s32 songId);
s32 sub_080031b8(s32 id);                                    /* play a sound effect */
s32 sub_080058e4(u32 type, s32 idx);                         /* spawn a task */
void sub_08008c4c(s32 a0);                                   /* load palette set */
void sub_08008c64(u16 a0);                                   /* load screen graphics */
void sub_0800b514(void);
void sub_08022c3c(void);
void sub_080c68b0(s32 v, s32 x, s32 y);
void sub_080c6ab4(u16 *time, s32 x, s32 y);
void sub_080cad8c(void);
void sub_080caeec(void);
void sub_080cb030(s32 n);
void sub_080cb058(void);
void sub_080cb0e8(void);
u8 sub_080cb108(void);
void sub_080cb178(void);
void sub_080cb1d8(void);
void sub_080cb21c(void);
void sub_080cb2b0(void);
void sub_080cb2cc(void);

/* AgbMain state 22: the game-over / continue screen. */
void sub_080cacf0(void)
{
    sub_080022ac();
    gUnk_03000FC8 = 10;
    gUnk_03001ECC = 6;
    gUnk_02006164 = 0;
    if (gUnk_03002150 != 20) {
        if (gUnk_03001F30 == 0)
            sub_080cad8c();
        else
            sub_080caeec();
    } else {
        sub_080cb058();
    }
    sub_08002338();
    sub_080021dc();
    sub_08002e0c();
    if (gUnk_030023D8 == 5) {
        gUnk_02007FC0 = 1;
        sub_0800b514();
        sub_08022c3c();
        if (gUnk_03002438 != 1)
            gUnk_030023D8 = 6;
    } else {
        sub_0800293c();
    }
}

/* The game-over screen outside link play (gUnk_03001F30 == 0): scroll the
   banner in, spawn the eight letters (#260) and the #261/#264 objects,
   then wait for the continue choice. */
void sub_080cad8c(void)
{
    s32 i;

    gUnk_03001EE0 = gUnk_03000FC0 = 0;
    gUnk_03000F8C = gUnk_03000B78 = 240 << 16;
    gUnk_03001E94 = gUnk_03000FA8 = 0;
    sub_08008c4c(6);
    sub_08008c64(4);
    sub_08008c64(51);
    sub_080c68b0(gUnk_02006020[gUnk_03002360], 22, 18);
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1D00;
    sub_080cb21c();
    sub_08002358();
    sub_08002378();
    sub_080022fc();
    sub_08003110(16);
    sub_08002028();
    sub_0800214c();
    while (gUnk_03000F8C != 0) {
        gUnk_03000F8C -= 0x78000;
        if (gUnk_03000B78 != 180 << 16) {
            gUnk_03000B78 -= 0x30000;
            if (gUnk_03000B78 == 180 << 16) {
                gUnk_03001ED8 &= 0xE0FF;
                gUnk_03001ED8 |= 0x1F00;
                sub_080058e4(262, 32);
            }
        }
        sub_08002d18();
    }
    for (i = 0; i < 8; i++) {
        struct Task *t = &gUnk_03002790[sub_080058e4(260, 32)];
        t->unk18 = i;
        sub_08002d94(8);
    }
    sub_080cb2b0();
    sub_080cb2cc();
    sub_08002d94(8);
    do {
        sub_08002d18();
        if (gUnk_030023AC != 1)
            sub_080cb178();
    } while (gUnk_02007BE0 == 0);
}

/* The game-over screen in link play (gUnk_03001F30 != 0): scroll the
   banner in, spawn the eight letters (#260) and the cursor (#261), then
   run the continue choice with the clock on screen. */
void sub_080caeec(void)
{
    s32 i;

    gUnk_03001EE0 = gUnk_03000FC0 = 0;
    gUnk_03000F8C = gUnk_03000B78 = 240 << 16;
    gUnk_03001E94 = gUnk_03000FA8 = 0;
    sub_08008c4c(6);
    sub_08008c64(4);
    sub_08008c64(51);
    sub_08002358();
    sub_08002378();
    sub_080022fc();
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1D00;
    sub_08003110(16);
    sub_08002028();
    sub_0800214c();
    while (gUnk_03000F8C != 0) {
        gUnk_03000F8C -= 0x78000;
        if (gUnk_03000B78 != 180 << 16) {
            gUnk_03000B78 -= 0x30000;
            if (gUnk_03000B78 == 180 << 16) {
                gUnk_03001ED8 &= 0xE0FF;
                gUnk_03001ED8 |= 0x1F00;
                sub_080058e4(262, 32);
            }
        }
        sub_080cb030(1);
    }
    for (i = 0; i < 8; i++) {
        struct Task *t = &gUnk_03002790[sub_080058e4(260, 32)];
        t->unk18 = i;
        sub_080cb030(8);
    }
    sub_080cb2b0();
    sub_080058e4(261, 32);
    sub_080cb030(8);
    do {
        sub_080cb030(1);
        sub_080cb0e8();
        sub_080cb1d8();
    } while (gUnk_02007BE0 == 0);
}

void sub_080cb030(s32 n)
{
    s32 i;

    for (i = 0; i < n; i++) {
        sub_080c6ab4(gUnk_02006068, 22, 18);
        sub_08002d18();
    }
}

/* The game-over screen after game state 20 (gUnk_03002150 == 20): the
   clock on screen until the countdown or a button ends it. */
void sub_080cb058(void)
{
    gUnk_03000B78 = gUnk_03000FA8 = 0;
    sub_08008c4c(6);
    sub_08008c64(4);
    sub_08008c64(52);
    sub_08002358();
    sub_08002668();
    sub_080022fc();
    sub_080c6ab4(gUnk_02006068, 22, 18);
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x900;
    sub_08003110(16);
    sub_08002028();
    sub_0800214c();
    sub_08002e0c();
    sub_080cb2b0();
    do {
        sub_08002d18();
        sub_080cb178();
    } while (gUnk_02007BE0 == 0);
}

void sub_080cb0e8(void)
{
    if (sub_080cb108() == 1)
        gUnk_02006164 ^= 1;
}

u8 sub_080cb108(void)
{
    if (gUnk_03001EB8[0] & 0xC0) {
        sub_080031b8(101);
        return 1;
    }
    return 0;
}

u8 sub_080cb12c(void)
{
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++) {
        if (gUnk_03001EB8[i] & 9) {
            sub_080031b8(102);
            gUnk_02007BE0 = 1;
            return 1;
        }
    }
    return 0;
}

void sub_080cb178(void)
{
    if (gUnk_0200557C <= 0)
        gUnk_02007BE0 = 1;
    else
        sub_080cb12c();
    gUnk_0200557C--;
    if (gUnk_02007BE0 != 0) {
        if (gUnk_03002150 == 20)
            gUnk_030023D8 = 4;
        else
            gUnk_030023D8 = 1;
    }
}

void sub_080cb1d8(void)
{
    if (sub_080cb12c()) {
        if (gUnk_02006164 == 0 && gUnk_030023AC == 1)
            gUnk_030023D8 = 5;
        else
            gUnk_030023D8 = 1;
    }
}

/* Load the game-over screen's palette and sprite tiles (a second set in
   single-player play). */
void sub_080cb21c(void)
{
    sub_080017e4(2, (u32)gUnk_085E2C20, (u32)gUnk_030014F0, 192);
    if (gUnk_030023AC == 1) {
        LZ77UnCompWram(gUnk_085E2CE0, gUnk_02020000);
        sub_080017e4(3, (u32)gUnk_02020000, 0x06012C00, 0x2A00);
        LZ77UnCompWram(gUnk_085E4064, gUnk_02020000);
        sub_080017e4(4, (u32)gUnk_02020000, (u32)gUnk_06010000, 0x3A00);
    } else {
        LZ77UnCompWram(gUnk_085E5BC4, gUnk_02020000);
        sub_080017e4(4, (u32)gUnk_02020000, (u32)gUnk_06010000, 0xC00);
    }
}

void sub_080cb2b0(void)
{
    gUnk_02007BE0 = 0;
    gUnk_0200557C = 480;
}

/* Spawn the game-over screen's objects: in single-player play the cursor
   (#261) and #264 variants 0-2 (variant 0's index goes to gUnk_02007D28),
   otherwise #264 variant 5. */
void sub_080cb2cc(void)
{
    s32 i;
    s32 id;
    struct Task *t;

    if (gUnk_030023AC == 1) {
        sub_080058e4(261, 32);
        for (i = 0; i <= 2; i++) {
            id = sub_080058e4(264, 32);
            if (id != -1) {
                t = &gUnk_03002790[id];
                t->unk73 = i;
                if (i == 0)
                    gUnk_02007D28 = id;
            }
        }
    } else {
        id = sub_080058e4(264, 32);
        if (id != -1) {
            struct Task *t2 = &gUnk_03002790[id];
            t2->unk73 = 5;
        }
    }
}
