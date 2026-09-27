#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* mode_082d0.c (0x080082D0-0x08008663, issue #96).
 *
 * Task type #265, the two decorations of the state-13 title screen:
 * sub_080082d0 spawns them, sub_08008348 is the body (it dispatches one of
 * two scripts from the anchor table at 0x0873078C through CallTableEntry),
 * and sub_080083b0/sub_08008460/sub_080084dc/sub_08008558 are the four
 * script bodies (a sprite pair, a BG scroll plus palette cycle, a sprite
 * loop and a palette pulse). */

extern u8 gUnk_02006090;
extern s8 gUnk_02006168;
extern vs32 gUnk_0300117C;
extern vu16 gUnk_03001270[];
extern u16 gUnk_03001470[];
extern u16 gUnk_03001490[];
extern u16 gUnk_03001EA4;
extern u16 gUnk_03002150;
extern u16 gUnk_03002360;
extern s32 gUnk_03005280;
extern u8 gUnk_085B6A90[];
extern u8 gUnk_085B6AC0[];
extern u8 gUnk_085B6AC8[];
extern u16 gUnk_085B6E78[][3][16];
extern u16 gUnk_085B6F98[];
extern void (*gUnk_0873078C[])(void);
extern void (*gUnk_08730794[])(void);
extern u8 gUnk_0873079C[];
extern u32 gUnk_08756054[];

void TaskYieldTrampoline(u32 frames);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, u16 f);
s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
void CallTableEntry(u32 idx, u32 count, void (**fns)(void));
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 TaskCreateFrom(u32 type, s32 idx);
void TaskSleepForever(void);
void sub_08008394(void);

void sub_080082d0(void)
{
    s32 idx;
    struct Task *t;

    if (gUnk_02006090 <= 2) {
        idx = TaskCreateFrom(0x109, 32);
        if (idx != -1) {
            t = &gUnk_03002790[idx];
            t->unk44 = gCurTaskIdx;
            t->unk73 = 0;
        }
        idx = TaskCreateFrom(0x109, 32);
        if (idx != -1) {
            t = &gUnk_03002790[idx];
            t->unk44 = gCurTaskIdx;
            t->unk73 = 1;
        }
    }
}

void sub_08008348(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_08008394;
    t->unk2C = 0;
    t->unk30 = 1;
    t->unk34 = 0;
    if (t->unk73 == 0)
        t->unk14 = 0;
    else
        t->unk14 = 1;
    CallTableEntry(gUnk_03002490->unk14, 2, gUnk_0873078C);
    TaskSleepForever();
}

void sub_08008394(void)
{
    CallTableEntry(gUnk_03002490->unk15, 2, gUnk_08730794);
}

void sub_080083b0(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk2C = 0;
    for (;;) {
        if (gUnk_03005280 == 0) {
            QueueSprite(8, (u32)gUnk_085B6AC0, 0, 0, 120, 88);
        } else if (gUnk_03005280 == 1) {
            QueueSprite(8, DrawAffineSprite((s32)gUnk_085B6AC0, 0x100, 0x100, (s16)((16 - (gUnk_03001EA4 & 15)) << 4)), 0, 0, 120, 88);
        } else if (gUnk_03005280 == 2) {
            QueueSprite(8, (u32)gUnk_085B6AC8, 0, 0, 120, 88);
        } else {
            TaskSleepForever();
            return;
        }
        QueueSprite(9, (u32)gUnk_085B6A90, 0, 0, 120, 88);
        TaskYieldTrampoline(1);
    }
}

void sub_08008460(void)
{
    if (gUnk_03005280 == 1) {
        gUnk_0300117C += 0x20000;
        if (gUnk_0300117C > 0x2000000)
            gUnk_0300117C -= 0x2000000;
    }
    if (gUnk_03005280 == 2) {
        BlendColors(gUnk_085B6F98, gUnk_085B6F98 + 16, gUnk_0873079C[gUnk_03002490->unk2C], 16, gUnk_03001470);
        gUnk_03002490->unk2C = (gUnk_03002490->unk2C + 1) & 15;
    }
}

void sub_080084dc(void)
{
    gUnk_03002490->unk15 = 1;
    for (;;) {
        if (gUnk_03002150 == 4 && gUnk_03002360 == 0)
            QueueSprite(8, gUnk_08756054[gUnk_02006168 + 4], 0, 0, 120, 144);
        else
            QueueSprite(8, gUnk_08756054[gUnk_02006168 + 7], 0, 0, 200, 144);
        TaskYieldTrampoline(1);
    }
}

/* The shift is written in BOTH arms of each triangle-wave `if`: jump2's
   cross-jumping merges the two identical `lsls` into the one at the join,
   and because at combine time the shift sits in a different block from the
   call's (u16) zero-extension, it is not folded into `lsls #22`.  The
   `r -= 4; r = 4 - r` pair must stay two statements (a single expression
   folds to `8 - r`; split, CSE reuses the register known to hold 4). */
void sub_08008558(void)
{
    u32 r;
    u32 i;

    if (gUnk_03005280 == 4) {
        r = gUnk_03001EA4 & 7;
        if (r > 3) {
            r -= 4;
            r = 4 - r;
            r <<= 6;
        } else {
            r <<= 6;
        }
        BlendColors(gUnk_085B6E78[gUnk_02006168][0], gUnk_085B6E78[gUnk_02006168][2], (u16)r, 16, gUnk_03001490);
    } else if (gUnk_03005280 == 3) {
        r = gUnk_03001EA4 & 15;
        if (r > 7) {
            r = 16 - r;
            r <<= 5;
        } else {
            r <<= 5;
        }
        BlendColors(gUnk_085B6E78[gUnk_02006168][0], gUnk_085B6E78[gUnk_02006168][1], (u16)r, 16, gUnk_03001490);
    } else {
        for (i = 0; i < 16; i++)
            gUnk_03001270[0x110 + i] = gUnk_085B6E78[0][0][i];
        gUnk_03001270[0x111] = gUnk_085B6E78[1][0][1];
        gUnk_03001270[0x112] = gUnk_085B6E78[1][0][2];
        gUnk_03001270[0x11D] = gUnk_085B6E78[0][0][12];
        gUnk_03001270[0x119] = gUnk_085B6E78[0][0][7];
        gUnk_03001270[0x11B] = gUnk_085B6E78[0][0][8];
    }
}
