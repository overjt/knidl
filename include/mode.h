#ifndef GUARD_MODE_H
#define GUARD_MODE_H

#include "gba/types.h"

/* mode.h: the RAM cells and ROM tables of the game-state bodies AgbMain
   dispatches into, the boot/title sequence and the screen loaders (M02).  One
   declaration per symbol, with the type its consumers prove (issue #36 phase
   2, docs/header-conventions.md). */

/* One boot-logo sprite object: a command script (BootLogoUpdateObjects) moving a
   sprite in 24.8 fixed point. */
struct M38LogoObj
{
    /*0x00*/ s16 *scriptPos;    /* script cursor */
    /*0x04*/ s16 scriptId;     /* script id, -1 = off */
    /*0x06*/ s16 spriteId;     /* sprite id (gUnk_087554B8), -1 = none */
    /*0x08*/ s16 layer;     /* layer */
    /*0x0A*/ s16 sleepFrames;     /* frames to wait */
    /*0x0C*/ s32 posX;     /* x << 8 */
    /*0x10*/ s32 posY;     /* y << 8 */
    /*0x14*/ s16 velX;     /* x velocity */
    /*0x16*/ s16 velY;     /* y velocity */
    /*0x18*/ s16 accelX;     /* x acceleration */
    /*0x1A*/ s16 accelY;     /* y acceleration */
    /*0x1C*/ s16 loopCount;     /* loop count */
    /*0x1E*/ u16 unk1E;
};

struct TransferNode
{
    u32 cmd; /* mode in bits 0-3, byte size in bits 8-31 */
    u32 src;
    u32 dst;
};

/* EWRAM */
extern s8 gLinkSessionMode;
extern u32 gUnk_02004000[];
extern u16 gPausingPlayer;
extern u8 gUnk_020055CC;
extern u8 gUnk_02006090;
extern s8 gUnk_02006160;
extern u8 gUnk_02007FCC;
extern u32 gUnk_02028000[];
extern struct M38LogoObj gUnk_02030000[];

/* IWRAM */
extern vu16 gUnk_03000B24;
extern u16 gUnk_03001390[];
extern u8 gUnk_030013B0[];
extern u16 gUnk_03001430[];
extern u16 gPrevGameState; /* requested/next game state */
extern s32 gUnk_03005280;

/* ROM */
extern u8 gUnk_080D1B78[];
extern u8 gUnk_080D2AD0[];
extern u16 gUnk_08541D98[][16];
extern u16 gUnk_08541F58[];
extern u8 gUnk_0856F2A8[];
extern u8 gUnk_0856F308[];
extern u8 gUnk_0857014C[];
extern u8 gUnk_085704CC[];
extern u8 gUnk_085707D4[];
extern u8 gUnk_085708A8[];
extern u8 gUnk_085709EC[];
extern u8 gUnk_08570B28[];
extern u8 gUnk_08570F1C[];
extern u8 gUnk_0857111C[];
extern u16 gUnk_0857121C[][11];
extern u8 gUnk_08571248[];
extern u8 gUnk_0857172C[];
extern u8 gUnk_08571838[];
extern u8 gUnk_08571BE0[];
extern u8 gUnk_08571D74[];
extern u8 gUnk_08572164[];
extern u8 gUnk_085A3CB8[];
extern u8 gUnk_085A4904[];
extern u8 gUnk_085A49E8[];
extern u16 gUnk_085B113C[];
extern u16 gUnk_085B119C[];
extern u16 gUnk_085B274C[];
extern u16 gUnk_085B2D0C[];
extern u16 gUnk_085B2D8C[];
extern u16 gUnk_085B450C[];
extern u16 gUnk_085B4ACC[];
extern u16 gUnk_085B4B4C[];
extern u16 gUnk_085B64C8[];
extern u8 gUnk_085B6A90[];
extern u8 gUnk_085B6AC0[];
extern u8 gUnk_085B6AC8[];
extern u16 gUnk_085B6E78[][3][16];
extern u16 gUnk_085B6F98[];
extern u8 gUnk_085CCB58[];
extern void (*gUnk_0873078C[])(void);
extern void (*gUnk_08730794[])(void);
extern u8 gUnk_0873079C[];
extern u16 *gUnk_08730884[];
extern struct TransferNode *gUnk_0873185C[];
extern u32 gUnk_08731980[][2][2];
extern u16 gUnk_087319B0[][2][2];
extern u32 gUnk_087319C8[][3];
extern u32 gUnk_08731A28[][3];
extern u8 gUnk_08731A88[];
extern u32 gAbilityPictures[][2];
extern u32 gUnk_08731B70[];
extern u16 gUnk_08731B88[][2];
extern u32 gUnk_08731BA0[][2];
extern u16 gUnk_08731C88[];
extern u16 gUnk_08731CC8[];
extern u8 gUnk_08731CDC[];
extern u32 gUnk_087555B4[];
extern u32 gUnk_08756054[];
extern u8 gUnk_0876B1FC[];
extern u8 gUnk_0876F690[];
extern u8 gUnk_087954C0[];
extern u8 gUnk_087C0A4C[];

#endif /* GUARD_MODE_H */
