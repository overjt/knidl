#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* menu_0ca10.c (0x0800CA10-0x0800D44F, issue #99).
 *
 * Main-menu screens, second part (gUnk_020060D0 is the menu screen,
 * gUnk_0200B074 the cursor, gUnk_03000038/gUnk_03000B70 the newly pressed
 * and auto-repeat keys).  sub_0800ca10 opens the mode list (screen 4)
 * and draws its 3-5 rows with sub_0800cc30 according to the save slot
 * unlock bits; sub_0800cd60 is its input loop (A/START picks a mode and
 * sets gUnk_02007FCC, row 4 leaves for game state 13, B goes back to the
 * file menu).  sub_0800d0f4 runs screen 5 (one player, or link play
 * through sub_0800cff4, which opens the link-play screen 8),
 * sub_0800d280 opens the sound test (screen 7), and
 * sub_0800d310/sub_0800d35c/sub_0800d404 draw its cursor and three-digit
 * numbers and play the chosen sound. */

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

struct SongEntry
{
    struct SongHeader *header;
    u8 prio;
    u8 chans;
    u8 pad[2];
};

extern s8 gUnk_02000018;
extern u32 gUnk_02004B70;
extern u8 gUnk_02006090;
extern s8 gUnk_020060D0;
extern s8 gUnk_02006168;
extern s8 gUnk_02006180;
extern s8 gUnk_02007D34;
extern s8 gUnk_02007E88;
extern u8 gUnk_02007FCC;
extern s16 gUnk_0200A6E0[];
extern s8 gUnk_0200B074;
extern struct SaveSlot gUnk_0200E600[];
extern vu16 gUnk_03000038;
extern vu16 gUnk_03000B70;
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000F8C;
extern vs32 gUnk_03000FA8;
extern u16 gUnk_030015F0[];
extern vs32 gUnk_03001E94;
extern vu16 gUnk_03001ED8;
extern vu16 gUnk_03001EF4;
extern s8 gUnk_03001F08[];
extern u8 gUnk_03001F30;
extern u16 gUnk_030023D8;
extern s32 gUnk_030023E8;
extern u8 gUnk_03002464;
extern u32 gUnk_06010000[];
extern const u8 gUnk_0855A5F8[];
extern u16 gUnk_08564F34[];
extern const u8 gUnk_085653C4[];
extern const struct SongEntry gUnk_0872EB38[];

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_080022e4(void);
void sub_08002d54(s32 count);
void sub_08002f14(s16 n);
s32 sub_080031b8(s32 id);
void sub_0800374c(s32 speed);
s32 sub_080058e4(u32 type, s32 idx);
void sub_08008c64(u16 a0);
void sub_0800c20c(void);
void sub_0800fd24(s32 speed, s32 dist, s32 bg);
s32 sub_0800fe54(s32 speed);
void sub_0800fe94(void);
void sub_08010020(s32 a, s32 b, s32 c, s32 d);
void sub_08010048(s32 in, s32 out, s32 h, s32 v, s32 win);
void sub_080b6e60(void);
void sub_080b6e6c(s32 a);
void sub_0800cc30(void);

void sub_0800ca10(void)
{
    s32 i;

    gUnk_02007E88 = gUnk_020060D0;
    if (gUnk_02007E88 == 11) {
        sub_08008c64(18);
        sub_08008c64(21);
        gUnk_020060D0 = 4;
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1F00;
        sub_08010020(66, 12, 13, 3);
    } else if (gUnk_02007E88 == 1) {
        gUnk_0200B074 = 0;
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1E00;
        gUnk_020060D0 = 4;
        sub_0800fe54(0x80000);
        sub_08008c64(25);
        sub_080b6e6c(2);
        sub_08002d54(8);
        gUnk_02004B70 = 0;
        for (i = 0; i < 8; i++) {
            if (gUnk_03000038 & 11) {
                sub_080b6e60();
                sub_0800fe94();
                gUnk_02004B70 = gUnk_03000038;
                sub_08010048(61, 63, 255, 136, 0x2000);
                sub_080022e4();
                gUnk_03001ED8 &= 0xDFFF;
                break;
            }
            sub_080022e4();
        }
        if (gUnk_02007E88 != 11)
            gUnk_02006180 = 16;
        else
            gUnk_02006180 = 0;
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1F00;
    } else {
        gUnk_020060D0 = 5;
        sub_080b6e6c(7);
        sub_08002d54(8);
        gUnk_03001E94 = gUnk_03000FA8 = 0;
        sub_0800fd24(0x100000, 256, 2);
        sub_0800fd24(0x100000, 256, 3);
        sub_08008c64(41);
        gUnk_02006180 = 16;
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1A00;
    }
    sub_08008c64(31);
    if (gUnk_0200E600[gUnk_030023E8].unk10 & 2)
        gUnk_02000018 = 2;
    else if (gUnk_0200E600[gUnk_030023E8].unk10 & 1)
        gUnk_02000018 = 1;
    else
        gUnk_02000018 = 0;
    sub_0800cc30();
    if (gUnk_020060D0 != 5)
        sub_080058e4(247, 32);
}

void sub_0800cc30(void)
{
    sub_080017e4(6, 0, 0x06003800, 0x800);
    switch (gUnk_02000018) {
    case 0:
        sub_080017e4(1, (u32)&gUnk_0855A5F8[0], 0x060039C0, 128);
        sub_080017e4(1, (u32)&gUnk_0855A5F8[128], 0x06003A80, 128);
        sub_080017e4(1, (u32)&gUnk_0855A5F8[256], 0x06003B40, 128);
        break;
    case 1:
        sub_080017e4(1, (u32)&gUnk_0855A5F8[0], 0x060039C0, 128);
        sub_080017e4(1, (u32)&gUnk_0855A5F8[128], 0x06003A40, 128);
        sub_080017e4(1, (u32)&gUnk_0855A5F8[256], 0x06003AC0, 128);
        sub_080017e4(1, (u32)&gUnk_0855A5F8[384], 0x06003B40, 128);
        break;
    case 2:
        sub_080017e4(1, (u32)&gUnk_0855A5F8[0], 0x06003980, 128);
        sub_080017e4(1, (u32)&gUnk_0855A5F8[128], 0x06003A00, 128);
        sub_080017e4(1, (u32)&gUnk_0855A5F8[256], 0x06003A80, 128);
        sub_080017e4(1, (u32)&gUnk_0855A5F8[384], 0x06003B00, 128);
        sub_080017e4(1, (u32)&gUnk_0855A5F8[512], 0x06003B80, 128);
        break;
    }
}

void sub_0800cd60(void)
{
    s32 i;

    while (1) {
        if (gUnk_02006180 != 0) {
            sub_08010048(61, 63, ((16 - gUnk_02006180) << 12) | 0xFF, 135, 0x2000);
            if ((gUnk_03000038 & 11) || (gUnk_02004B70 & 11)) {
                gUnk_02006180 = 1;
                gUnk_02004B70 |= gUnk_03000038;
            }
            if (--gUnk_02006180 == 0)
                gUnk_03001ED8 &= 0xDFFF;
        }
        if ((gUnk_03000038 & 9) || (gUnk_02004B70 & 9)) {
            gUnk_02004B70 = 0;
            sub_080031b8(102);
            if (gUnk_0200B074 == 4) {
                gUnk_020060D0 = 9;
                gUnk_03002464 = 0;
                gUnk_03001F30 = 1;
                gUnk_030023D8 = 13;
                gUnk_02006090 = 7;
                gUnk_02007FCC = 7;
                return;
            }
            if (gUnk_0200B074 <= 2) {
                gUnk_02006090 = gUnk_0200B074;
                gUnk_02007FCC = gUnk_0200B074;
            } else if (gUnk_0200B074 == 3) {
                gUnk_03001F30 = 0;
                gUnk_03002464 = 0;
                gUnk_02006090 = 6;
                gUnk_02007FCC = 6;
            }
            gUnk_02006168 = 0;
            gUnk_020060D0 = 5;
            gUnk_02007D34 = 0;
            sub_080058e4(248, 32);
            sub_08002d54(6);
            return;
        }
        if ((gUnk_03000038 & 2) || (gUnk_02004B70 & 2)) {
            gUnk_02004B70 = 0;
            sub_080031b8(215);
            gUnk_02007E88 = gUnk_020060D0;
            gUnk_020060D0 = 1;
            gUnk_03001ED8 &= 0xE0FF;
            gUnk_03001ED8 |= 0x1E00;
            sub_0800fe54(0xFFF80000);
            sub_08008c64(33);
            for (i = 1; i < 16; i++) {
                sub_08010048(60, 63, (0x10000 - (i << 12)) | 0xFF, 135, 0x2000);
                sub_080022e4();
                if (i > 8 && (gUnk_03000038 & 11)) {
                    sub_0800fe94();
                    break;
                }
            }
            gUnk_03001ED8 &= 0xDFFF;
            sub_0800c20c();
            return;
        }
        if (gUnk_03000B70 & 64) {
            sub_080031b8(101);
            if (--gUnk_0200B074 < 0)
                gUnk_0200B074 = gUnk_02000018 + 2;
        } else if (gUnk_03000B70 & 128) {
            sub_080031b8(101);
            if (++gUnk_0200B074 >= gUnk_02000018 + 3)
                gUnk_0200B074 = 0;
        }
        sub_080022e4();
    }
}

void sub_0800cff4(void)
{
    s32 i;

    gUnk_02007E88 = gUnk_020060D0;
    gUnk_020060D0 = 8;
    sub_08008c64(36);
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1A00;
    gUnk_03001E94 = gUnk_03000FA8 = 0;
    sub_0800fd24(0xFFF00000, 256, 2);
    sub_0800fd24(0xFFF00000, 256, 3);
    sub_080058e4(250, 32);
    sub_08008c64(33);
    sub_080b6e6c(5);
    for (i = 0; i < 16; i++) {
        if (i != 0)
            sub_08010048(60, 63, (0x10000 - (i << 12)) | 0xFF, 135, 0x2000);
        sub_080022e4();
    }
    gUnk_03001ED8 &= 0xDFFF;
    sub_08008c64(41);
    sub_08008c64(37);
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1E00;
    sub_080058e4(249, 32);
    sub_08002d54(8);
    sub_08008c64(42);
    sub_080058e4(251, 32);
}

void sub_0800d0f4(void)
{
    gUnk_02004B70 = 0;
    while (1) {
        if (gUnk_02006180 != 0) {
            if (--gUnk_02006180 != 0 && (gUnk_03000038 & 11)) {
                gUnk_02006180 = 0;
                sub_080b6e60();
                sub_0800fe94();
                gUnk_02004B70 = gUnk_03000038;
            }
            if (gUnk_02006180 == 0) {
                gUnk_03001ED8 &= 0xE0FF;
                gUnk_03001ED8 |= 0x1F00;
                sub_080058e4(247, 32);
                gUnk_02007D34 = 1;
                sub_080058e4(248, 32);
                if (gUnk_02004B70 != 0)
                    sub_08002d54(1);
            }
        }
        if ((gUnk_03000038 & 9) || (gUnk_02004B70 & 9)) {
            gUnk_02004B70 = 0;
            sub_080031b8(102);
            if (gUnk_02007D34 == 0) {
                gUnk_020060D0 = 9;
                gUnk_03001F30 = 0;
                gUnk_030023D8 = 13;
                return;
            }
            sub_0800cff4();
            return;
        }
        if ((gUnk_03000038 & 2) || (gUnk_02004B70 & 2)) {
            gUnk_02004B70 = 0;
            sub_080031b8(215);
            gUnk_020060D0 = 4;
            sub_08002d54(8);
            return;
        }
        if (gUnk_02006180 == 0) {
            if ((gUnk_03001EF4 & 128) && gUnk_02007D34 == 0) {
                sub_080031b8(101);
                gUnk_02007D34 = 1;
            } else if ((gUnk_03001EF4 & 64) && gUnk_02007D34 == 1) {
                sub_080031b8(101);
                gUnk_02007D34 = 0;
            }
        }
        sub_080022e4();
    }
}

void sub_0800d280(void)
{
    sub_08002d54(2);
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1A00;
    sub_080022e4();
    sub_0800374c(32);
    gUnk_020060D0 = 7;
    sub_08008c64(45);
    sub_080b6e6c(1);
    sub_08008c64(25);
    gUnk_03000FA8 = gUnk_03000B78 = 0;
    sub_0800fd24(0xFFF00000, 256, 3);
    gUnk_03000F8C = 0x1000000;
    sub_08002d54(8);
    gUnk_02006180 = 8;
}

void sub_0800d310(void)
{
    u16 *src = gUnk_08564F34;

    sub_080017e4(2, (u32)src, (u32)gUnk_030015F0 + ((gUnk_0200B074 * 16 + 1) * 2), 2);
    src++;
    sub_080017e4(2, (u32)src, (u32)gUnk_030015F0 + (((s8)(gUnk_0200B074 ^ 1) * 16 + 1) * 2), 2);
}

void sub_0800d35c(s32 a)
{
    s32 i;

    sub_08002f14(gUnk_0200A6E0[a]);
    if (gUnk_0200A6E0[a] < 100)
        gUnk_03001F08[2] = 10;
    if (gUnk_0200A6E0[a] < 10)
        gUnk_03001F08[1] = 10;
    for (i = 0; i < 3; i++) {
        sub_080017e4(3, (u32)&gUnk_085653C4[gUnk_03001F08[2 - i] * 128], (u32)gUnk_06010000 + (a * 6 + i * 2 + 4) * 32, 64);
        sub_080017e4(3, (u32)&gUnk_085653C4[(gUnk_03001F08[2 - i] * 4 + 2) * 32], (u32)gUnk_06010000 + (a * 6 + i * 2 + 36) * 32, 64);
    }
}

void sub_0800d404(void)
{
    s32 n = 0;
    s32 i = 0;
    s16 target = gUnk_0200A6E0[1];

    if (i != target) {
        i = 1;
        do {
            if (gUnk_0872EB38[i].header != NULL)
                n++;
            if (n == target)
                break;
            i++;
        } while (i != 0x1DF);
    }
    sub_080031b8(i + 100);
}
