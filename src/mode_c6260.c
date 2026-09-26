#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* mode_c6260.c (0x080C6260-0x080C641F, issue #98).
 *
 * AgbMain state 11 (src/main.c calls sub_080c6260 once and moves to
 * state 12, which runs M38's sub_080c6420).
 * 
 *   sub_080c6260   copies gUnk_03002360, gUnk_03001F38, gUnk_0300243C and
 *       gUnk_030023AC into EWRAM cells, runs the SIO teardown sub_0800293c
 *       (after sub_08002358/sub_08002668 when gUnk_03002150 is 20, the value
 *       AgbMain's state 20 leaves there), stops the sound (sub_08003484),
 *       plays the two scenes below unless gUnk_03002150 is 20 or
 *       gUnk_03001F30 is 1, and ends with sub_080b8070(gUnk_030023E8).
 *   sub_080c62f0 / sub_080c6388   one scene each: preset and load a room
 *       (sub_08024610(0, 0) / sub_08024654(632, 248)), palette set 15 / 16
 *       (sub_08008c4c), DISPCNT BG bits 0x1D00 / 0x1C00, spawn the scene's
 *       task and run frames (sub_08002d18) until it clears gUnk_02008018,
 *       then tear the level down (sub_08027178).
 *   sub_080c6354 / sub_080c63ec   spawn M38's task type #100 / #101,
 *       retrying every frame until a slot is free, with Task.unk73 = 0. */

extern u16 gUnk_02000028;
extern u16 gUnk_03002360;
extern u16 gUnk_02007D3C;
extern u16 gUnk_03001F38;
extern u16 gUnk_0200616C;
extern u16 gUnk_0300243C;
extern u16 gUnk_02004C94;
extern u16 gUnk_030023AC;
extern u16 gUnk_03002150;
extern u8 gUnk_03001F30;
extern s32 gUnk_030023E8;
extern u8 gUnk_02008018;
extern vu16 gUnk_03001ED8;

void sub_0800214c(void);
void sub_080021dc(void);
void sub_08002358(void);
void sub_08002668(void);
void sub_0800293c(void);
void sub_08002d18(void);
void sub_08002e0c(void);
void sub_08003484(void);
s32 sub_080058e4(u32 type, s32 idx);                         /* spawn a task */
void sub_08008c4c(s32 a0);
void sub_08024610(s32 x, s32 y);
void sub_08024654(s32 x, s32 y);
void sub_08027178(void);
void sub_080b8070(s32 a);
void sub_080c62f0(void);
void sub_080c6354(void);
void sub_080c6388(void);
void sub_080c63ec(void);

void sub_080c6260(void)
{
    gUnk_02000028 = gUnk_03002360;
    gUnk_02007D3C = gUnk_03001F38;
    gUnk_0200616C = gUnk_0300243C;
    gUnk_02004C94 = gUnk_030023AC;
    if (gUnk_03002150 == 20) {
        sub_08002358();
        sub_08002668();
    }
    sub_0800293c();
    sub_08003484();
    if (gUnk_03002150 != 20 && gUnk_03001F30 != 1) {
        sub_080c62f0();
        sub_080c6388();
    }
    sub_08003484();
    sub_080b8070(gUnk_030023E8);
}

void sub_080c62f0(void)
{
    gUnk_02008018 = 1;
    sub_08024610(0, 0);
    sub_08008c4c(15);
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1D00;
    sub_080c6354();
    sub_0800214c();
    sub_08002e0c();
    while (gUnk_02008018 != 0)
        sub_08002d18();
    sub_080021dc();
    sub_08002e0c();
    sub_08027178();
}

void sub_080c6354(void)
{
    s32 id;
    struct Task *t;

    while ((id = sub_080058e4(100, 32)) == -1)
        sub_08002d18();
    t = &gUnk_03002790[id];
    t->unk73 = 0;
}

void sub_080c6388(void)
{
    gUnk_02008018 = 1;
    sub_08024654(632, 248);
    sub_08008c4c(16);
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1C00;
    sub_080c63ec();
    sub_0800214c();
    sub_08002e0c();
    while (gUnk_02008018 != 0)
        sub_08002d18();
    sub_080021dc();
    sub_08002e0c();
    sub_08027178();
}

void sub_080c63ec(void)
{
    s32 id;
    struct Task *t;

    while ((id = sub_080058e4(101, 32)) == -1)
        sub_08002d18();
    t = &gUnk_03002790[id];
    t->unk73 = 0;
}
