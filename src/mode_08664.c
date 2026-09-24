#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* mode_08664.c (0x08008664-0x08008B8B, issue #96).
 *
 * Stage entry and AgbMain state 20. */

extern u16 gUnk_02004B60;
extern s16 gUnk_02005588[];
extern s16 gUnk_02007D48[];
extern s8 gUnk_02007D64;
extern s16 gUnk_0200EC58;
extern vu16 gUnk_03000048;
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000F8C;
extern vs32 gUnk_03000FA8;
extern u8 gUnk_03001390[];
extern vu16 gUnk_03001E90;
extern vs32 gUnk_03001E94;
extern vu16 gUnk_03001EB8[];
extern vu16 gUnk_03001ED8;
extern s8 gUnk_03001F20;
extern u8 gUnk_03001F30;
extern struct PlayerState gUnk_03002170[];
extern u8 gUnk_03002340;
extern u16 gUnk_03002360;
extern u16 gUnk_030023AC;
extern s8 gUnk_030023B8;
extern u16 gUnk_030023D8;
extern u8 gUnk_03002400[8][7];
extern s8 gUnk_03002438;
extern s8 gUnk_03002444;
extern s16 gUnk_0300244C;
extern u16 gUnk_0857121C[][11];

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_08002028(void);
void sub_0800214c(void);
void sub_080021dc(void);
void sub_080022fc(void);
void sub_08002338(void);
void sub_08002358(void);
void sub_08002378(void);
void sub_08002d18(void);
void sub_08002d94(s32 count);
void sub_08002e0c(void);
void sub_080031b8(s32 id);
void sub_080034f0(s32 player, s32 songId);
void sub_08003688(void);
void sub_080037f8(u16 speed);
void sub_0800381c(s32 speed);
void sub_080055b0(u8 val, s32 idx);
void sub_080075b8(void);
void sub_08008c4c(s32 a0);
void sub_08008fc4(s32 a0, s32 a1);
void sub_0800b648(void);
void sub_0801a7b4(void);
void sub_08027128(void);
void sub_080271ec(void);
void sub_08027210(void);
void sub_08027228(void);
void sub_08027240(void);
void sub_0803d0a0(s32 a0);
void sub_08040788(void);
void sub_080b6eec(void);
void sub_080b6f04(void);
void sub_080b6f38(void);
void sub_080b77d4(void);
void sub_080089e0(s32 n);

void sub_08008664(void)
{
    s32 i;
    s32 id;
    s32 flag;
    s32 sel;
    s32 pressed;
    u16 mode;

    id = gUnk_03002170[gUnk_03002360].unk0D;
    sub_080031b8(232);
    for (i = 0; i < 64; i++)
        sub_080055b0(15, i);
    sub_080271ec();
    sub_080021dc();
    sub_08002d94(8);
    sub_0800381c(32);
    sub_08002e0c();
    for (i = 0; i < 64; i++)
        sub_080055b0(31, i);
    sub_080b6eec();
    sub_08027228();
    mode = gUnk_030023D8;
    flag = 0;
    if (mode == 8 && gUnk_03002400[gUnk_030023B8][gUnk_03001F20] != 0)
        flag = gUnk_02007D64 != 5;
    sub_08008c4c(5);
    if (gUnk_03001F30 == 1) {
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0xC00;
        id = 28;
    } else if (gUnk_02007D48[gUnk_03002360] == 0 && gUnk_02005588[gUnk_03002360] == 0) {
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x800;
        id = 27;
    } else {
        if (!((gUnk_03002340 >> gUnk_03002360) & 1))
            id = 26;
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0xE00;
    }
    sub_08008fc4(id, flag);
    gUnk_03000F8C = gUnk_03001E94 = 0;
    gUnk_03000B78 = gUnk_03000FA8 = 0;
    sel = 0;
    sub_080089e0(0);
    sub_08002358();
    sub_08002378();
    sub_080022fc();
    sub_08002028();
    sub_0800214c();
    sub_08002e0c();
    do {
        sub_08002d18();
        pressed = 0;
        if (gUnk_0300244C == 0) {
            for (i = 0; i < gUnk_030023AC; i++) {
                if (gUnk_02007D48[i] != 0 || gUnk_02005588[i] != 0) {
                    if (gUnk_03001EB8[i] & 9) {
                        if (sel != 0) {
                            gUnk_030023D8 = 5;
                            sub_08003688();
                        }
                        sub_080031b8(0x11F);
                        pressed++;
                    } else if (gUnk_03001EB8[i] & 2) {
                        sub_080031b8(215);
                        pressed++;
                    }
                    if (flag != 0 && (gUnk_03001EB8[i] & 0xC0)) {
                        sub_080031b8(286);
                        sel ^= 1;
                        sub_080089e0(sel);
                    }
                }
            }
        } else {
            if (flag != 0 && (gUnk_03001EB8[gUnk_02004B60] & 0xC0)) {
                sub_080031b8(286);
                sel ^= 1;
                sub_080089e0(sel);
            }
            if (gUnk_03001EB8[gUnk_02004B60] & 9) {
                if (sel != 0) {
                    gUnk_030023D8 = 5;
                    sub_08003688();
                }
                sub_080031b8(0x11F);
                pressed++;
            } else if (gUnk_03001EB8[gUnk_02004B60] & 2) {
                sub_080031b8(215);
                pressed++;
            }
        }
    } while (pressed == 0);
    if (gUnk_03002444 != 0 || gUnk_030023D8 != 5) {
        sub_08002338();
        sub_080021dc();
        sub_08002e0c();
        sub_08008c4c(3);
        sub_08027240();
        for (i = 0; i < 64; i++)
            sub_080055b0(15, i);
        sub_080b6f04();
        sub_08002358();
        sub_08002378();
        sub_080022fc();
        sub_080037f8(16);
        sub_0800214c();
        sub_08002e0c();
        for (i = 0; i < 64; i++)
            sub_080055b0(0, i);
        sub_08027210();
    }
}

void sub_080089e0(s32 n)
{
    sub_080017e4(2, (u32)gUnk_0857121C[n], (u32)gUnk_03001390, 22);
}

void sub_08008a00(void)
{
    s32 done = 0;
    s32 i;

    gUnk_02004B60 = 0;
    sub_08008c4c(3);
    gUnk_0200EC58 = 0;
    sub_080b6f38();
    sub_0800b648();
    for (i = 0; i < gUnk_030023AC; i++)
        sub_0803d0a0(i);
    sub_08002358();
    sub_08002378();
    sub_080022fc();
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1D00;
    sub_0800214c();
    while (gUnk_03001E90 != 0) {
        sub_08002d18();
        sub_080b77d4();
    }
    gUnk_03000048 = 0;
    do {
        sub_0801a7b4();
        sub_08002d18();
        sub_080b77d4();
        sub_08040788();
        sub_080075b8();
        switch (gUnk_03002438) {
        case 0:
            break;
        case 1:
            done = 1;
            break;
        case 2:
            break;
        case 3:
            done = 1;
            break;
        case 4:
            break;
        case 5:
            sub_08008664();
            gUnk_03002438 = 0;
            break;
        case 6:
            gUnk_030023D8 = 22;
            done = 1;
            break;
        case 7:
            gUnk_030023D8 = 11;
            done = 1;
            break;
        }
    } while (done == 0);
    sub_08002338();
    sub_080021dc();
    while (gUnk_03001E90 != 0) {
        sub_08002d18();
        sub_080b77d4();
    }
    gUnk_03000048 = 0;
    for (i = 0; i < gUnk_030023AC; i++) {
        if (gUnk_03002170[i].unk2C != -1) {
            sub_080034f0(gUnk_03002170[i].unk2C, gUnk_03002170[i].unk2E);
            gUnk_03002170[i].unk2C = -1;
        }
    }
    sub_08027128();
}
