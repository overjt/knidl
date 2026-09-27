#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* obj_306b4.c (0x080306B4-0x08030803, issue #86).
 *
 * QueueWorldSprite draws one OAM sprite relative to the BG camera
 * (gSpriteCameraX/gSpriteCameraY) when it is on screen, and returns garbage
 * when it is not.  The rest manage the per-frame stage hook gUnk_030004A0:
 * ResetBlockAnims clears it and the 64 records gBreakingBlocks[] (unk6 =
 * 0x7FFF), ResumeBlockAnims re-installs it from its id gBlockAnimHookId,
 * PauseBlockAnims clears it and sub_080307b0/cc/e8 install one of three M09
 * routines (ids 1-3).
 * 
 * Its own file, not the tail of obj_30238.c: compiled as one translation
 * unit, ResetBlockAnims swaps r3/r4, because gcse orders its hash table by
 * the addresses of the .LC pool-label strings, which depend on every pool
 * constant compiled earlier in the unit. */

struct Unk020061F0
{
    /*0x00*/ u8 filler00[6];
    /*0x06*/ u16 unk6;
    /*0x08*/ u8 filler08[0x18];
};

extern s16 gSpriteCameraX;
extern s16 gSpriteCameraY;
extern struct Unk020061F0 gBreakingBlocks[];
extern u32 gUnk_030004A0;
extern u8 gBlockAnimHookId;

s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
void sub_080318b4(void);
void sub_08031de4(void);
void sub_08032428(void);

s32 QueueWorldSprite(u8 a, s32 b, u16 c, u16 d, s16 x, s16 y)
{
    s16 sx;
    s16 sy;

    sx = x - gSpriteCameraX;
    sy = y - gSpriteCameraY;
    if ((u16)(sx + 63) <= 366 && sy > -64 && sy <= 223)
        return QueueSprite(a, b, c, d, sx, sy);
}

void ResetBlockAnims(void)
{
    s32 i;

    for (i = 0; i < 64; i++)
        gBreakingBlocks[i].unk6 = 0x7FFF;
    gUnk_030004A0 = 0;
    gBlockAnimHookId = 0;
}

void ResumeBlockAnims(void)
{
    switch (gBlockAnimHookId)
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

void PauseBlockAnims(void)
{
    gUnk_030004A0 = 0;
}

void sub_080307b0(void)
{
    gUnk_030004A0 = (u32)sub_080318b4;
    gBlockAnimHookId = 1;
}

void sub_080307cc(void)
{
    gUnk_030004A0 = (u32)sub_08031de4;
    gBlockAnimHookId = 2;
}

void sub_080307e8(void)
{
    gUnk_030004A0 = (u32)sub_08032428;
    gBlockAnimHookId = 3;
}
