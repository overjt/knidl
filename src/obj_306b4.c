#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* obj_306b4.c (0x080306B4-0x08030803, issue #86).
 *
 * sub_080306b4 draws one OAM sprite relative to the BG camera
 * (gUnk_03002348/gUnk_030023E4) when it is on screen, and returns garbage
 * when it is not.  The rest manage the per-frame stage hook gUnk_030004A0:
 * sub_08030724 clears it and the 64 records gUnk_020061F0[] (unk6 =
 * 0x7FFF), sub_08030758 re-installs it from its id gUnk_02000024,
 * sub_080307a4 clears it and sub_080307b0/cc/e8 install one of three M09
 * routines (ids 1-3).
 * 
 * Its own file, not the tail of obj_30238.c: compiled as one translation
 * unit, sub_08030724 swaps r3/r4, because gcse orders its hash table by
 * the addresses of the .LC pool-label strings, which depend on every pool
 * constant compiled earlier in the unit. */

struct Unk020061F0
{
    /*0x00*/ u8 filler00[6];
    /*0x06*/ u16 unk6;
    /*0x08*/ u8 filler08[0x18];
};

extern s16 gUnk_03002348;
extern s16 gUnk_030023E4;
extern struct Unk020061F0 gUnk_020061F0[];
extern u32 gUnk_030004A0;
extern u8 gUnk_02000024;

s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
void sub_080318b4(void);
void sub_08031de4(void);
void sub_08032428(void);

s32 sub_080306b4(u8 a, s32 b, u16 c, u16 d, s16 x, s16 y)
{
    s16 sx;
    s16 sy;

    sx = x - gUnk_03002348;
    sy = y - gUnk_030023E4;
    if ((u16)(sx + 63) <= 366 && sy > -64 && sy <= 223)
        return sub_08001a94(a, b, c, d, sx, sy);
}

void sub_08030724(void)
{
    s32 i;

    for (i = 0; i < 64; i++)
        gUnk_020061F0[i].unk6 = 0x7FFF;
    gUnk_030004A0 = 0;
    gUnk_02000024 = 0;
}

void sub_08030758(void)
{
    switch (gUnk_02000024)
    {
    case 0:
        break;
    case 1:
        gUnk_030004A0 = (u32)sub_080318b4;
        break;
    case 2:
        gUnk_030004A0 = (u32)sub_08031de4;
        break;
    case 3:
        gUnk_030004A0 = (u32)sub_08032428;
        break;
    }
}

void sub_080307a4(void)
{
    gUnk_030004A0 = 0;
}

void sub_080307b0(void)
{
    gUnk_030004A0 = (u32)sub_080318b4;
    gUnk_02000024 = 1;
}

void sub_080307cc(void)
{
    gUnk_030004A0 = (u32)sub_08031de4;
    gUnk_02000024 = 2;
}

void sub_080307e8(void)
{
    gUnk_030004A0 = (u32)sub_08032428;
    gUnk_02000024 = 3;
}
