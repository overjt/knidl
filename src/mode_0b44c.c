#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* mode_0b44c.c (0x0800B44C-0x0800B91F, issue #96).
 *
 * The game-state setup helpers AgbMain and the state bodies call:
 * sub_0800b44c (reset the game-state cells), sub_0800b4a8 (scores, the
 * maximum health 24 or 48, the HUD mode), sub_0800b514 (three lives and
 * cleared records per player), sub_0800b5dc/sub_0800b628, and the three
 * screen setups sub_0800b648/sub_0800b788/sub_0800b87c. */

struct Unk02005E00
{
    /*0x00*/ s32 unk00;
    /*0x04*/ u8 unk04[4];
    /*0x08*/ u8 unk08[4];
};

extern u16 gNextActorSerial;
extern u8 gUnk_02000020;
extern u16 gUnk_02004B50[];
extern u8 gUnk_02004B64;
extern s16 gUnk_02005580;
extern s16 gUnk_02005588[];
extern u8 gUnk_020055C4;
extern u8 gUnk_020055CC;
extern struct Unk02005E00 gUnk_02005E00;
extern s32 gUnk_02006020[];
extern s8 gUnk_02006094;
extern u8 gUnk_020060CC;
extern s8 gUnk_02006160;
extern u8 gUnk_02006178;
extern u8 gUnk_020061E0;
extern u8 gUnk_02007CF0;
extern u8 gUnk_02007CF4[];
extern s16 gPlayerLives[];
extern u32 gUnk_02007F50;
extern vs16 gUnk_02007FA8[];
extern s8 gUnk_02007FB8[];
extern u16 gUnk_02008008[];
extern s8 gUnk_02008010;
extern vu16 gUnk_0200AF18[];
extern u32 gUnk_0200EC50;
extern s16 gInputRecorderMode;
extern vs32 gBg0ScrollY;
extern vu16 gUnk_03000B24;
extern vs32 gBg3ScrollX;
extern vs32 gBg2ScrollX;
extern vu16 gPlayerHeldKeys[];
extern vs32 gBg3ScrollY;
extern vs32 gBg1ScrollY;
extern vs32 gBg0ScrollX;
extern vs32 gBg2ScrollY;
extern vu16 gPlayerPressedKeys[];
extern vu16 gDispCnt;
extern vs32 gBg1ScrollX;
extern u8 gUnk_03001F30;
extern u8 gUnk_03001F34;
extern u16 gLinkIsMaster;
extern u8 gActivePlayerMask;
extern u8 gActivePlayerCount;
extern u16 gLocalPlayer;
extern u16 gPlayerCount;
extern u8 gUnk_030023B0;
extern u16 gLatchedPressedKeys[];
extern s8 gStageRequest;
extern u16 gLinkPlayerCount;
extern s16 gUnk_0300244C;
extern u16 gLatchedHeldKeys[];
extern u8 gExtraMode;
extern vu16 gLinkCommand;

void ResetBgScroll(void);
void ResetFadeAndBlend(void);
void ResetTasksAndOam(void);
void ResetPlayTime(void);
void sub_08008c64(u16 a0);
void sub_08008c7c(void);
void sub_08008cb8(void);
void sub_08009aa0(void);
void sub_08009ab8(void);
void ClearColliderLists(void);
void sub_0802296c(void);
void sub_08022f98(void);
void sub_08022f9c(void);
void sub_08022fa8(void);
void sub_080233e0(void);
void sub_08023948(void);
void sub_08023ca0(void);
void InitPlayerState(s32 a0);
void sub_08066144(void);

void sub_0800b44c(void)
{
    gUnk_03000B24 = 1;
    ResetTasksAndOam();
    gLocalPlayer = 0;
    gLinkIsMaster = 0;
    gLinkPlayerCount = 1;
    gPlayerCount = 1;
    gUnk_03001F30 = 0;
    gInputRecorderMode = 0;
    gUnk_030023B0 = 0;
    gUnk_0200EC50 = 0;
}

void sub_0800b4a8(void)
{
    s32 i;

    ResetTasksAndOam();
    for (i = 0; i <= 3; i++) {
        gUnk_02006020[i] = 0;
        InitPlayerState(i);
    }
    ResetPlayTime();
    if (gUnk_03001F30 == 0) {
        if (gExtraMode == 1)
            gUnk_02005580 = 24;
        else
            gUnk_02005580 = 48;
        sub_08009aa0();
    } else {
        gUnk_02005580 = 24;
        sub_08009ab8();
    }
    sub_0802296c();
}

void sub_0800b514(void)
{
    s32 i;

    ResetTasksAndOam();
    for (i = 0; i <= 3; i++) {
        gPlayerLives[i] = 3;
        gUnk_02005588[i] = 0;
        gUnk_02008008[i] = gUnk_02004B50[i] = 0;
        /* Volatile all-ones stores reuse their dead pre-read (lesson 3.68);
         * the two cells differ in signedness, so the chain stores the
         * constant twice instead of re-reading the inner cell (3.361). */
        gUnk_02007FA8[i] = gUnk_0200AF18[i] = 0xFFFF;
        InitPlayerState(i);
        gUnk_02005E00.unk04[i] = 0;
        gUnk_02005E00.unk08[i] = 0;
    }
    gActivePlayerMask = 0;
    gActivePlayerCount = 0;
    gUnk_02005E00.unk00 = 0;
    gUnk_02006178 = 0;
    gUnk_020055CC = 0;
    gUnk_03001F34 = 0;
}

void sub_0800b5dc(void)
{
    s32 i;

    sub_08022f98();
    gUnk_02007CF0 = 0;
    for (i = 0; i < gPlayerCount; i++) {
        gUnk_02005E00.unk08[i] = 0;
        InitPlayerState(i);
    }
    gUnk_020055C4 = 0;
}

void sub_0800b628(void)
{
    s32 i;

    ResetTasksAndOam();
    sub_08022f9c();
    for (i = 0; i <= 3; i++)
        InitPlayerState(i);
}

void sub_0800b648(void)
{
    s32 i;
    s8 *b;
    s8 *p;
    s8 zero;
    u8 *b2;
    u8 *p2;
    u8 zero2;
    u8 *q1;
    s8 *q2;
    s8 *q3;
    u8 *q4;

    gDispCnt |= 0x80;
    ResetFadeAndBlend();
    ResetBgScroll();
    ClearColliderLists();
    sub_08008c64(0);
    sub_08008c7c();
    gUnk_02007F50 = -1;
    if (gUnk_02004B64 == 0)
        sub_08022fa8();
    else
        sub_080233e0();
    if ((u8)(gUnk_02000020 - 2) <= 1)
        sub_08008cb8();
    /* The ROM loads these four addresses before the clear loop below; only
     * pointer locals assigned here reproduce that order. */
    q1 = &gUnk_020061E0;
    q2 = &gUnk_02006160;
    q3 = &gUnk_02006094;
    q4 = &gUnk_020060CC;
    b = gUnk_02007FB8;
    zero = 0;
    p = b + 2;
    do {
        *p = zero;
        p--;
    } while ((s32)p >= (s32)b);
    *q1 = 0;
    *q2 = -1;
    *q3 = -1;
    *q4 = 0;
    sub_08066144();
    gNextActorSerial = 0;
    gLinkCommand = 0;
    gUnk_03001F34 = 0;
    gUnk_02006178 = 0;
    gUnk_02007CF0 = 0;
    gUnk_02008010 = -1;
    gUnk_020055C4 = 0;
    for (i = 0; i < 4; i++)
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = gLatchedHeldKeys[i] = gLatchedPressedKeys[i] = 0;
    gStageRequest = 0;
    if (gUnk_0300244C != 0) {
        b2 = gUnk_02007CF4;
        zero2 = 0;
        p2 = b2 + 3;
        do {
            *p2 = zero2;
            p2--;
        } while ((s32)p2 >= (s32)b2);
    }
}

void sub_0800b788(void)
{
    s32 i;
    s8 *b;
    s8 *p;
    s8 zero;

    gDispCnt |= 0x80;
    ResetFadeAndBlend();
    ResetBgScroll();
    ResetTasksAndOam();
    gBg0ScrollX = gBg1ScrollX = gBg2ScrollX = gBg3ScrollX = 0;
    gBg0ScrollY = gBg1ScrollY = gBg2ScrollY = gBg3ScrollY = 0;
    sub_08008c64(0);
    sub_08008c7c();
    sub_08023948();
    b = gUnk_02007FB8;
    zero = 0;
    p = b + 2;
    do {
        *p = zero;
        p--;
    } while ((s32)p >= (s32)b);
    gUnk_020061E0 = 0;
    gUnk_02007F50 = -1;
    sub_08066144();
    for (i = 0; i < 4; i++)
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = gLatchedHeldKeys[i] = gLatchedPressedKeys[i] = 0;
    gLinkCommand = 0;
    gStageRequest = 0;
}

void sub_0800b87c(void)
{
    s32 i;
    s8 *b;
    s8 *p;
    s8 zero;

    gDispCnt |= 0x80;
    ResetFadeAndBlend();
    ResetBgScroll();
    sub_08008c64(0);
    sub_08008c7c();
    sub_08023ca0();
    /* A reversed clear loop only strength-reduces as a do/while over a
     * signed pointer compare with a zero variable (lesson 3.30). */
    b = gUnk_02007FB8;
    zero = 0;
    p = b + 2;
    do {
        *p = zero;
        p--;
    } while ((s32)p >= (s32)b);
    gUnk_020061E0 = 0;
    gUnk_02007F50 = -1;
    sub_08066144();
    for (i = 0; i < 4; i++)
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = gLatchedHeldKeys[i] = gLatchedPressedKeys[i] = 0;
    gLinkCommand = 0;
    gStageRequest = 0;
}
