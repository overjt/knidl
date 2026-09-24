#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* menu_0c09c.c (0x0800C09C-0x0800CA0F, issue #99).
 *
 * Main-menu screens, first part.  sub_0800c09c is the file-select input
 * loop (screen 0: three save slots, A/START loads or creates the slot,
 * B returns to the title screen); sub_0800c20c opens the file menu
 * (screen 1) and sub_0800c34c is its input loop over four entries
 * (gUnk_020055E4: start, the mode list, the sound test, erase);
 * sub_0800c558 and sub_0800c610 run the two-choice screens 2/3 that
 * lead into a game, and sub_0800c8a0 the two-step erase confirmation
 * that clears the slot with sub_080b7d74. */

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

extern s8 gUnk_02004B44;
extern u32 gUnk_02004B70;
extern s8 gUnk_020055E4;
extern s8 gUnk_020060D0;
extern s8 gUnk_02006180;
extern u8 gUnk_0200618C;
extern s8 gUnk_02007D34;
extern s8 gUnk_02007E88;
extern s8 gUnk_0200B074;
extern struct SaveSlot gUnk_0200E600[];
extern vu16 gUnk_03000038;
extern vu16 gUnk_03000B70;
extern vs32 gUnk_03000FA8;
extern vs32 gUnk_03001E94;
extern vu16 gUnk_03001ED8;
extern vu16 gUnk_03001EF4;
extern u8 gUnk_03001F30;
extern u16 gUnk_03002150;
extern u16 gUnk_030023D8;
extern s32 gUnk_030023E8;
extern u8 gUnk_03002464;

void sub_0800214c(void);
void sub_080021dc(void);
void sub_080022e4(void);
void sub_08002d54(s32 count);
void sub_08002db4(void);
s32 sub_080031b8(s32 id);
s32 sub_080058e4(u32 type, s32 idx);
void sub_08008c4c(s32 a0);
void sub_08008c64(u16 a0);
void sub_0800bcf0(void);
void sub_0800bda4(s32 slot);
void sub_0800bf6c(s32 slot, s32 value, s32 mode);
void sub_0800ca10(void);
void sub_0800d280(void);
void sub_0800da9c(s32 mode);
void sub_0800e28c(void);
void sub_0800fd24(s32 speed, s32 dist, s32 bg);
s32 sub_0800fe54(s32 speed);
void sub_0800fe94(void);
void sub_08010020(s32 a, s32 b, s32 c, s32 d);
void sub_080b6e60(void);
void sub_080b6e6c(s32 a);
void sub_080b798c(s32 a);
void sub_080b7d74(s32 a);
void sub_080b8070(s32 a);
s32 sub_080b8290(void);
void sub_080c680c(s32 a);
void sub_0800c20c(void);

void sub_0800c09c(void)
{
    vu16 *keys = &gUnk_03000038;
    s8 *state = &gUnk_020060D0;
    s32 i;
    s32 done;

    while (1)
    {
        if (*keys & 9)
        {
            sub_080031b8(102);
            gUnk_030023E8 = gUnk_0200B074;
            if (gUnk_0200E600[gUnk_030023E8].unk04 == 0x99999999)
                sub_080b798c(gUnk_030023E8);
            done = 0;
            for (i = 0; i < 2; i++)
            {
                gUnk_03002464 = i;
                sub_080b8070(gUnk_030023E8);
                if (sub_080b8290() & 2)
                {
                    if (done == 0)
                    {
                        sub_080021dc();
                        sub_08002db4();
                    }
                    sub_080c680c(i);
                    done = 1;
                }
            }
            if (done)
            {
                sub_08008c4c(2);
                sub_0800bcf0();
                sub_08008c64(19);
                gUnk_03001ED8 &= 0xE0FF;
                gUnk_03001ED8 |= 0x1C00;
                sub_0800214c();
                sub_08002db4();
            }
            sub_0800fe54(0x80000);
            gUnk_02007E88 = *state;
            *state = 1;
            sub_0800c20c();
            return;
        }
        if (*keys & 2)
        {
            sub_080031b8(215);
            *state = 10;
            gUnk_03002150 = 4;
            gUnk_030023D8 = 3;
            return;
        }
        if (gUnk_03000B70 & 0x40)
        {
            sub_080031b8(101);
            if (--gUnk_0200B074 < 0)
                gUnk_0200B074 = 2;
        }
        else if (gUnk_03000B70 & 0x80)
        {
            sub_080031b8(101);
            if (++gUnk_0200B074 > 2)
                gUnk_0200B074 = 0;
        }
        sub_080022e4();
    }
}

void sub_0800c20c(void)
{
    gUnk_0200618C = 0;
    sub_080058e4(241, 32);
    sub_0800e28c();
    sub_08008c64(21);
    sub_0800bda4(gUnk_030023E8);
    sub_0800bf6c(gUnk_030023E8, gUnk_0200E600[gUnk_030023E8].unk12[0], 1);
    sub_08010020(66, 12, 13, 3);
    switch (gUnk_02007E88)
    {
    case 0:
        gUnk_020055E4 = 0;
        sub_08008c64(25);
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1E00;
        sub_080b6e6c(0);
        sub_08002d54(8);
        gUnk_02006180 = 9;
        break;
    case 4:
        gUnk_020055E4 = 1;
        sub_08008c64(25);
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1F00;
        sub_080058e4(242, 32);
        sub_080b6e6c(3);
        gUnk_02006180 = 17;
        break;
    case 8:
        gUnk_020055E4 = 0;
        sub_08008c64(26);
        gUnk_02006180 = 17;
        break;
    case 7:
        gUnk_020055E4 = 2;
        sub_08008c64(25);
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1A00;
        sub_080b6e6c(0);
        gUnk_02006180 = 17;
        break;
    }
}

void sub_0800c34c(void)
{
    gUnk_02004B70 = 0;
    while (1)
    {
        if (gUnk_02006180 != 0)
        {
            if (--gUnk_02006180 != 0 && (gUnk_03000038 & 11))
            {
                gUnk_02006180 = 0;
                sub_080b6e60();
                sub_0800fe94();
                gUnk_02004B70 = gUnk_03000038;
            }
            if (gUnk_02006180 == 0)
            {
                if (gUnk_02007E88 != 4)
                    sub_080058e4(242, 32);
                if (gUnk_02004B70 != 0)
                    sub_08002d54(4);
            }
        }
        if ((gUnk_03000038 & 9) || (gUnk_02004B70 & 9))
        {
            sub_080031b8(102);
            switch (gUnk_020055E4)
            {
            case 0:
                gUnk_03001F30 = 0;
                gUnk_03002464 = 0;
                if (gUnk_0200E600[gUnk_030023E8].unk10 & 4)
                {
                    gUnk_020060D0 = 2;
                    gUnk_02007D34 = 0;
                    sub_080058e4(245, 32);
                    sub_08002d54(10);
                }
                else
                {
                    gUnk_020060D0 = 3;
                    gUnk_02004B44 = 0;
                    sub_080058e4(246, 32);
                    sub_08002d54(10);
                }
                break;
            case 3:
                gUnk_020060D0 = 6;
                break;
            case 1:
                sub_0800ca10();
                break;
            case 2:
                sub_0800d280();
                break;
            }
            return;
        }
        if ((gUnk_03000038 & 2) || (gUnk_02004B70 & 2))
        {
            sub_080031b8(215);
            gUnk_020060D0 = 0;
            sub_0800bcf0();
            sub_0800da9c(1);
            gUnk_03001ED8 &= 0xE0FF;
            gUnk_03001ED8 |= 0x1E00;
            sub_080b6e6c(1);
            sub_08008c64(25);
            sub_0800fe54(0xFFF80000);
            sub_08002d54(18);
            return;
        }
        if (gUnk_02006180 == 0)
        {
            if (gUnk_03000B70 & 0x40)
            {
                sub_080031b8(101);
                if (--gUnk_020055E4 < 0)
                    gUnk_020055E4 = 3;
            }
            else if (gUnk_03000B70 & 0x80)
            {
                sub_080031b8(101);
                if (++gUnk_020055E4 > 3)
                    gUnk_020055E4 = 0;
            }
        }
        sub_080022e4();
    }
}

void sub_0800c558(void)
{
    vu16 *keys = &gUnk_03000038;
    s8 *state = &gUnk_020060D0;

    while (!(*keys & 9))
    {
        if (*keys & 2)
        {
            sub_080031b8(215);
            *state = 1;
            sub_08002d54(10);
            return;
        }
        if ((gUnk_03001EF4 & 0x80) && gUnk_02007D34 == 0)
        {
            sub_080031b8(101);
            gUnk_02007D34 = 1;
        }
        else if ((gUnk_03001EF4 & 0x40) && gUnk_02007D34 == 1)
        {
            sub_080031b8(101);
            gUnk_02007D34 = 0;
        }
        sub_080022e4();
    }
    sub_080031b8(102);
    gUnk_03002464 = gUnk_02007D34;
    *state = 3;
    gUnk_02004B44 = 0;
    sub_080058e4(246, 32);
    sub_08002d54(10);
}

void sub_0800c610(void)
{
    gUnk_02004B70 = 0;
    while (1)
    {
        if (gUnk_02006180 != 0)
        {
            if (--gUnk_02006180 != 0 && (gUnk_03000038 & 11))
            {
                gUnk_02006180 = 0;
                sub_080b6e60();
                sub_0800fe94();
                gUnk_02004B70 = gUnk_03000038;
            }
            if (gUnk_02006180 == 0)
            {
                gUnk_03001ED8 &= 0xE0FF;
                gUnk_03001ED8 |= 0x1F00;
                sub_080058e4(242, 32);
                if (gUnk_0200E600[gUnk_030023E8].unk10 & 4)
                {
                    gUnk_02007D34 = gUnk_03002464;
                    sub_080058e4(245, 32);
                }
                gUnk_02004B44 = 1;
                sub_080058e4(246, 32);
                if (gUnk_02004B70 != 0)
                    sub_08002d54(1);
            }
        }
        if ((gUnk_03000038 & 9) || (gUnk_02004B70 & 9))
        {
            sub_080031b8(102);
            if (gUnk_02004B44 == 0)
            {
                gUnk_020060D0 = 9;
                gUnk_03001F30 = 0;
                gUnk_030023D8 = 5;
            }
            else
            {
                gUnk_02007E88 = gUnk_020060D0;
                gUnk_020060D0 = 8;
                sub_08008c64(36);
                gUnk_03001ED8 &= 0xE0FF;
                gUnk_03001ED8 |= 0x1A00;
                gUnk_03001E94 = gUnk_03000FA8 = 0;
                sub_0800fd24(0xFFF00000, 256, 2);
                sub_0800fd24(0xFFF00000, 256, 3);
                sub_080058e4(250, 32);
                sub_08008c64(26);
                sub_080b6e6c(4);
                sub_08002d54(16);
                sub_08008c64(39);
                sub_08008c64(37);
                gUnk_03001ED8 &= 0xE0FF;
                gUnk_03001ED8 |= 0x1E00;
                sub_080058e4(249, 32);
                sub_08002d54(8);
                sub_08008c64(40);
                sub_080058e4(251, 32);
            }
            return;
        }
        if ((gUnk_03000038 & 2) || (gUnk_02004B70 & 2))
        {
            sub_080031b8(215);
            if (gUnk_0200E600[gUnk_030023E8].unk10 & 4)
                gUnk_020060D0 = 2;
            else
                gUnk_020060D0 = 1;
            sub_08002d54(10);
            return;
        }
        if (gUnk_02006180 == 0)
        {
            if ((gUnk_03001EF4 & 0x80) && gUnk_02004B44 == 0)
            {
                sub_080031b8(101);
                gUnk_02004B44 = 1;
            }
            else if ((gUnk_03001EF4 & 0x40) && gUnk_02004B44 == 1)
            {
                sub_080031b8(101);
                gUnk_02004B44 = 0;
            }
        }
        sub_080022e4();
    }
}

void sub_0800c8a0(void)
{
    vu16 *keys;

    gUnk_0200618C = 0;
    gUnk_02007D34 = 1;
    sub_08008c64(29);
    sub_080058e4(243, 32);
    sub_08002d54(10);
    keys = &gUnk_03000038;
    while (1)
    {
        if ((gUnk_03000038 & 9) && gUnk_02007D34 == 0)
        {
            sub_080031b8(102);
            if (++gUnk_0200618C == 1)
            {
                sub_08008c64(30);
                gUnk_02007D34 = 1;
                sub_08002d54(10);
            }
            else
            {
                sub_080058e4(244, 32);
                sub_08002d54(10);
                sub_080031b8(268);
                sub_080b7d74(gUnk_030023E8);
                gUnk_020060D0 = 0;
                sub_0800bcf0();
                gUnk_03001ED8 &= 0xE0FF;
                gUnk_03001ED8 |= 0x1E00;
                sub_080b6e6c(1);
                sub_08008c64(25);
                sub_0800fe54(0xFFF80000);
                sub_08002d54(28);
                sub_0800da9c(1);
                sub_08002d54(16);
                return;
            }
        }
        else if (*keys & 11)
        {
            if (*keys & 9)
                sub_080031b8(102);
            else
                sub_080031b8(215);
            gUnk_020060D0 = 1;
            sub_08002d54(10);
            return;
        }
        if ((gUnk_03001EF4 & 0x20) && gUnk_02007D34 == 1)
        {
            sub_080031b8(101);
            gUnk_02007D34 = 0;
        }
        else if ((gUnk_03001EF4 & 0x10) && gUnk_02007D34 == 0)
        {
            sub_080031b8(101);
            gUnk_02007D34 = 1;
        }
        sub_080022e4();
    }
}
