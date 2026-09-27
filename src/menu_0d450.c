#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* menu_0d450.c (0x0800D450-0x0800DAF7, issue #99).
 *
 * The sound-test input loop sub_0800d450 (two columns in
 * gUnk_0200A6E0[]: music 0-43, mapped to song ids through gUnk_08731DC0,
 * and sound effects 0-273; A plays or stops, B returns to the file menu)
 * and the link-play connection screen sub_0800d85c, which starts the SIO
 * multi-play session for the mode sub_0800da18 picks, waits for the
 * partners (sub_0800da74) and leaves for game state 5 or 13.
 * sub_0800da9c spawns the file-select sprite tasks #238-#240. */

extern s8 gUnk_02000004;
extern s8 gUnk_02006010;
extern s8 gUnk_020060D0;
extern s8 gUnk_02006180;
extern s8 gUnk_02007E88;
extern s8 gUnk_02007FC8;
extern u8 gUnk_02007FCC;
extern s16 gUnk_0200A6E0[];
extern s8 gUnk_0200B074;
extern vu8 gUnk_0200EBC0[];
extern u32 gUnk_0200EC48;
extern vu16 gPressedKeys;
extern vs16 gCurrentBgm;
extern vu16 gRepeatedKeys;
extern vs32 gBg2ScrollX;
extern vs32 gBg3ScrollY;
extern vu16 gKeyRepeatDelay;
extern vs32 gBg2ScrollY;
extern vu16 gKeyRepeatInterval;
extern vu16 gDispCnt;
extern vu16 gHeldKeys;
extern s32 gUnk_03001F2C;
extern u8 gUnk_03001F30;
extern u16 gGameState;
extern u8 gExtraMode;
extern const s16 gUnk_08731DC0[];
extern u8 gUnk_0876B1FC[];
extern u8 gUnk_0876F690[];

void RunFrame(void);
void RunFrames(s32 count);
s32 PlayBgm(s32 songId);
s32 PlaySfx(s32 id);
void StopAllSfx(void);
void FadeOutBgm(s32 speed);
void sub_08003888(void);
void sub_08003964(void);
void sub_08003a34(u8 *start, u8 *end);
void sub_08003a98(void);
void sub_08004000(u16 a);
s32 TaskCreateFrom(u32 type, s32 idx);
u32 ConnectLink(void);
void sub_08008b8c(void);
void sub_08008c64(u16 a0);
void sub_0800c20c(void);
void sub_0800ca10(void);
void sub_0800d310(void);
void sub_0800d35c(s32 a);
void sub_0800d404(void);
void sub_0800fd24(s32 speed, s32 dist, s32 bg);
void sub_0800fe94(void);
void StopHBlankScroll(void);
void StartHBlankScroll(s32 a);
void sub_080b83b8(void);
void sub_0800da18(void);
s32 sub_0800da74(void);

void sub_0800d450(void)
{
    while (1) {
        if (gUnk_02006180 != 0) {
            if (--gUnk_02006180 != 0 && (gPressedKeys & 2)) {
                gUnk_02006180 = 0;
                StopHBlankScroll();
                sub_0800fe94();
            }
            if (gUnk_02006180 == 0) {
                gUnk_0200B074 = 0;
                sub_0800d35c(0);
                sub_0800d35c(1);
                sub_0800d310();
                TaskCreateFrom(254, 32);
                TaskCreateFrom(255, 32);
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1D00;
            }
        }
        gUnk_03001F2C = gUnk_08731DC0[gUnk_0200A6E0[gUnk_0200B074]];
        if (gPressedKeys & 1) {
            if (gUnk_0200B074 == 0) {
                if (gUnk_03001F2C == gCurrentBgm
                    && (s32)gMPlayTable[gSongTable[gUnk_03001F2C].ms].info->status >= 0) {
                    m4aSongNumStop(gUnk_03001F2C);
                    sub_08008c64(49);
                } else {
                    PlayBgm(gUnk_03001F2C | 0x800);
                    sub_08008c64(48);
                }
            } else {
                StopAllSfx();
                sub_0800d404();
                sub_08008c64(48);
            }
            RunFrames(3);
            sub_08008c64(47);
        } else if (gPressedKeys & 2) {
            gKeyRepeatDelay = 10;
            gKeyRepeatInterval = 6;
            StopAllSfx();
            PlaySfx(215);
            sub_08008c64(50);
            RunFrames(3);
            sub_08008c64(47);
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x1A00;
            sub_0800fd24(0x100000, 256, 3);
            gBg2ScrollX = 0;
            gUnk_02007E88 = gUnk_020060D0;
            gUnk_020060D0 = 1;
            if (gUnk_03001F2C != 40
                && (s32)gMPlayTable[gSongTable[gUnk_03001F2C].ms].info->status >= 0)
                FadeOutBgm(32);
            RunFrames(8);
            PlayBgm(40);
            sub_0800c20c();
            return;
        }
        if (gUnk_02006180 == 0) {
            if ((gPressedKeys & 16) && gUnk_0200B074 == 0) {
                gUnk_0200B074 = 1;
                sub_0800d310();
                gUnk_02006010 = 0;
                gKeyRepeatDelay = 10;
                gKeyRepeatInterval = 6;
            } else if ((gPressedKeys & 32) && gUnk_0200B074 == 1) {
                gUnk_0200B074 = 0;
                sub_0800d310();
                gUnk_02006010 = 0;
                gKeyRepeatDelay = 10;
                gKeyRepeatInterval = 6;
            } else if (gRepeatedKeys & 128) {
                gUnk_0200A6E0[gUnk_0200B074]--;
                if (++gUnk_02006010 == 5) {
                    gKeyRepeatDelay = 10;
                    gKeyRepeatInterval = 3;
                }
                if (gUnk_0200A6E0[gUnk_0200B074] < 0) {
                    if (gUnk_0200B074 == 0)
                        gUnk_0200A6E0[gUnk_0200B074] = 43;
                    else
                        gUnk_0200A6E0[gUnk_0200B074] = 0x111;
                }
                sub_0800d35c(gUnk_0200B074);
            } else if (gRepeatedKeys & 64) {
                gUnk_0200A6E0[gUnk_0200B074]++;
                if (++gUnk_02006010 == 5) {
                    gKeyRepeatDelay = 10;
                    gKeyRepeatInterval = 3;
                }
                if (gUnk_0200B074 == 0 && gUnk_0200A6E0[gUnk_0200B074] > 43)
                    gUnk_0200A6E0[gUnk_0200B074] = 0;
                else if (gUnk_0200B074 == 1 && gUnk_0200A6E0[gUnk_0200B074] > 0x111)
                    gUnk_0200A6E0[gUnk_0200B074] = 0;
                sub_0800d35c(gUnk_0200B074);
            }
            if (!(gHeldKeys & 0xC0)) {
                gUnk_02006010 = 0;
                gKeyRepeatDelay = 10;
                gKeyRepeatInterval = 6;
            }
        }
        RunFrame();
    }
}

void sub_0800d85c(void)
{
    sub_080b83b8();
    sub_0800da18();
    sub_08003888();
    sub_08003a34(gUnk_0876B1FC, gUnk_0876F690);
    gUnk_02007FC8 = 0;
    RunFrames(4);
    while (1) {
        if (gPressedKeys & 2) {
            PlaySfx(215);
            sub_08003964();
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x1A00;
            if (gUnk_02007FC8 == 1)
                sub_08008c64(37);
            if (gUnk_02007E88 == 3) {
                sub_08008c64(39);
                gUnk_02007E88 = gUnk_020060D0;
                gUnk_020060D0 = 3;
                gBg2ScrollY = gBg3ScrollY = 0;
                sub_0800fd24(0x100000, 256, 2);
                sub_0800fd24(0x100000, 256, 3);
                StartHBlankScroll(6);
                RunFrames(8);
                sub_0800c20c();
                return;
            }
            sub_08008c64(41);
            sub_0800ca10();
            return;
        }
        if (gUnk_02007FC8 == 0) {
            sub_08004000(gUnk_02000004);
            if (gUnk_0200EBC0[2] == 3) {
                gUnk_020060D0 = 9;
                gUnk_03001F30 = 0;
                if (gUnk_02007E88 == 3)
                    gGameState = 5;
                else
                    gGameState = 13;
                if (ConnectLink())
                    sub_08008b8c();
                return;
            }
            if (gUnk_0200EBC0[2] == 2 && gUnk_0200EBC0[0] == 0 && (gPressedKeys & 9)) {
                if (gUnk_0200EC48 != 1) {
                    gUnk_020060D0 = 9;
                    return;
                }
                sub_08003a98();
            }
            if ((gUnk_0200EBC0[3] & 1) || sub_0800da74() == 1) {
                sub_08003964();
                gUnk_02007FC8 = 1;
                sub_08008c64(38);
            }
        }
        RunFrame();
    }
}

void sub_0800da18(void)
{
    if (gUnk_02007E88 == 3) {
        if (gExtraMode == 0)
            gUnk_02000004 = 1;
        else
            gUnk_02000004 = 2;
    } else if (gUnk_02007FCC <= 2) {
        gUnk_02000004 = gUnk_02007FCC + 4;
    } else if (gUnk_02007FCC == 6) {
        gUnk_02000004 = 3;
    }
}

s32 sub_0800da74(void)
{
    s32 r = 0;

    switch (gUnk_02000004) {
    case 1:
    case 2:
    case 3:
        if (gUnk_0200EC48 == 2)
            r = 1;
        break;
    case 4:
        break;
    }
    return r;
}

void sub_0800da9c(s32 mode)
{
    s32 i;
    struct Task *t;
    s32 id;

    id = TaskCreateFrom(240, 32);
    t = &gTasks[id];
    t->unk18 = mode;
    for (i = 0; i <= 2; i++) {
        id = TaskCreateFrom(238, 32);
        t = &gTasks[id];
        t->unk18 = mode;
        t->unk1C = i;
        id = TaskCreateFrom(239, 32);
        t = &gTasks[id];
        t->unk18 = mode;
        t->unk1C = i;
    }
}
