#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "room.h"
#include "player.h"
#include "actor.h"
#include "save.h"

/* gfx_08b8c.c (0x08008B8C-0x080091AB, issue #96).
 *
 * Screen/asset loaders: LZ77/Huffman decompression of palettes, tiles and
 * maps into VRAM from the ROM tables at 0x08731980-0x08731BA8.
 * LoadBgLayout(i) sets the DISPCNT mode bits and the BGnCNT shadows from
 * preset i and LoadGfxSet(i) queues a VRAM transfer list (both called
 * ROM-wide); sub_08008fc4 loads the pause
 * pictures; LinkErrorScreen is the soft-reset prompt. */

void SoftReset(u32 resetFlags);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void PlaySfx(s32 id);

void LinkErrorScreen(void)
{
    ResetFadeAndBlend();
    ResetTasksAndOam();
    StopAllSound();
    DisableSerial();
    StopHBlankScroll();
    gFrameEndCallback = 0;
    gBrightness = 31;
    RunFrameNoTasks();
    LoadBgLayout(3);
    LoadGfxSet(65);
    gBg3ScrollX = gBg3ScrollY = 0;
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x800;
    BeginFastFadeInFromWhite();
    RunFramesNoTasks(32);
    gFadeBlankAtWhite = 0;
    for (;;) {
        if (gPressedKeys & 9) {
            PlaySfx(102);
            break;
        }
        if (gPressedKeys & 2) {
            PlaySfx(215);
            break;
        }
        RunFrameNoTasks();
    }
    BeginFastFadeOutToWhite();
    RunFramesNoTasksUntilFadeDone();
    SoftReset(0x1C);
}

void LoadBgLayout(s32 a0)
{
    ApplyBgLayout(gUnk_08730884[a0]);
}

void LoadGfxSet(u16 a0)
{
    RequestCopyList(gUnk_0873185C[a0]);
}

void sub_08008c7c(void)
{
    u32 src;

    if (gMetaKnightmareMode == 1) {
        src = (u32)gUnk_080D1B78;
        RequestCopy(3, src, 0x060160C0, 64);
        src += 64;
        RequestCopy(3, src, 0x060164C0, 64);
    }
}

void sub_08008cb8(void)
{
    u32 src;

    src = (u32)gUnk_080D2AD0;
    RequestCopy(3, src, 0x060170E0, 224);
    RequestCopy(3, src + 224, 0x060174E0, 224);
    RequestCopy(3, src + 448, 0x060178E0, 224);
    RequestCopy(3, src + 672, 0x06017CE0, 224);
}

void sub_08008d10(s32 a0, s32 a1)
{
    if (gUnk_08731980[a0][a1][0] != 0) {
        LZ77UnCompWram((void *)gUnk_08731980[a0][a1][0], gUnk_02020000);
        RequestCopy(3, (u32)gUnk_02020000, (u32)gObjVram, gUnk_087319B0[a0][a1][0] << 5);
    }
    if (gUnk_08731980[a0][a1][1] != 0) {
        LZ77UnCompWram((void *)gUnk_08731980[a0][a1][1], gUnk_02020000);
        RequestCopy(4, (u32)gUnk_02020000, (u32)gObjVram, gUnk_087319B0[a0][a1][1] << 5);
    }
}

void sub_08008d98(s32 a0)
{
    RequestCopy(6, 0, 0x06001000, 0x800);
    RequestCopy(6, 0, 0x06001800, 0x800);
    RequestCopy(2, gUnk_087319C8[a0][0], (u32)gBgPalette, 64);
    LZ77UnCompVram((void *)gUnk_087319C8[a0][1], (void *)0x06000000);
    LZ77UnCompVram((void *)gUnk_087319C8[a0][2], (void *)0x06001000);
    if (a0 == 7)
        LZ77UnCompVram(gUnk_085CCB58, (void *)0x06001800);
}

void sub_08008e1c(s32 a0)
{
    RequestCopy(2, gUnk_08731A28[a0][0], (u32)gUnk_03001370, gUnk_08731A88[a0] << 5);
    LZ77UnCompVram((void *)gUnk_08731A28[a0][1], (void *)0x06008000);
    LZ77UnCompVram((void *)gUnk_08731A28[a0][2], (void *)0x06003000);
}

void sub_08008e6c(s32 a0)
{
    RequestCopy(2, (u32)gUnk_085A4904 + (a0 << 5), (u32)gUnk_030013B0, 32);
    RequestCopy(8, (u32)gUnk_085A49E8, (u32)gUnk_02020000, 0);
    RequestCopy(1, (u32)gUnk_02020000 + (a0 << 11), 0x06007800, 0x800);
}

void HudClearAbilityPicture(void)
{
    RequestCopy(6, 0, 0x06000400, 0x3E0);
}

void HudLoadAbilityPicture(s32 a0)
{
    RequestCopy(2, gAbilityPictures[a0][0] + 2, (u32)&gBgPalette[1], 30);
    RequestCopy(1, gAbilityPictures[a0][1], 0x06000400, 0x3E0);
}

/* The 0x02020000 / 0x02020100 buffer addresses must stay integer literals:
   as gUnk_02020000 symbols CSE keeps them in callee-saved registers across
   the calls and the if-block (-8 bytes, r7/r8 permutation). */
void LoadMuseumAbilitySignGfx(s32 a0)
{
    RequestCopy(2, gUnk_08731B70[a0], (u32)gUnk_03001610, 32);
    RequestCopy(8, (u32)gUnk_085A3CB8, 0x02020000, 0);
    RequestCopy(4, gUnk_08731B88[a0][0] + 0x02020000, 0x06017800, 0x100);
    RequestCopy(4, gUnk_08731B88[a0][0] + 0x02020100, 0x06017C00, 0x100);
    if (gUnk_08731B88[a0][1] != 0xFFFF) {
        RequestCopy(4, gUnk_08731B88[a0][1] + 0x02020000, 0x06017900, 0x100);
        RequestCopy(4, gUnk_08731B88[a0][1] + 0x02020100, 0x06017D00, 0x100);
    }
}

void sub_08008fc4(s32 a0, s32 a1)
{
    switch (a0) {
    case 28:
        RequestCopy(2, (u32)gUnk_0856F2A8, (u32)gUnk_03001370, 96);
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
        RequestCopy(2, (u32)gUnk_0857111C, (u32)gUnk_03001370, 0x100);
        LZ77UnCompVram(gUnk_08570F1C, (void *)0x0600F800);
        HuffUnComp(gUnk_08570B28, gUnk_02020000);
        LZ77UnCompVram(gUnk_02020000, (void *)0x06008000);
        break;
    default:
        RequestCopy(2, (u32)gUnk_0857111C, (u32)gUnk_03001370, 0x100);
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
        RequestCopy(2, gAbilityPictures[a0][0], (u32)gUnk_030013B0, 32);
        RequestCopy(1, gAbilityPictures[a0][1], 0x0600A800, 0x3E0);
        break;
    }
}
