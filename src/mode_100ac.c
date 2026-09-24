#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* mode_100ac.c (0x080100AC-0x08010357, issue #99).
 *
 * AgbMain state 7 (sub_080100ac), entered instead of states 5/6 while
 * gUnk_02007FC0 is set: the scripted sequence of stage gUnk_030023B8.
 * It clears the blend and window shadows, loads the sequence's palette
 * set and pictures (sub_08008d98, and sub_080102c0: the sprite sheet
 * gUnk_08731F78[stage] plus, in link play, the player palette), opens
 * window 0 (full width for sequence 7), spawns M04's director, task
 * type #91, and pumps frames until the director leaves state 7. */

extern u8 gUnk_0200AF04;
extern u32 gUnk_02020000[];
extern vs32 gUnk_03000010;
extern vu8 gUnk_03000040;
extern vu16 gUnk_03000044;
extern vu8 gUnk_03000B08;
extern vu8 gUnk_03000B18;
extern vs32 gUnk_03000B78;
extern vu8 gUnk_03000F7C;
extern vs32 gUnk_03000F8C;
extern vs32 gUnk_03000FA8;
extern vs32 gUnk_03000FC0;
extern vu16 gUnk_03000FD4;
extern vs32 gUnk_0300117C;
extern vu8 gUnk_0300118C;
extern u16 gUnk_03001570[];
extern vs32 gUnk_03001E94;
extern vu8 gUnk_03001EAC;
extern vu16 gUnk_03001ED8;
extern vs32 gUnk_03001EE0;
extern vu16 gUnk_03001EEC;
extern u8 gUnk_03001F30;
extern u16 gUnk_03002360;
extern u16 gUnk_030023AC;
extern s8 gUnk_030023B8;
extern u16 gUnk_030023D8;
extern vu16 gUnk_03005274;
extern u32 gUnk_06010000[];
extern u16 gUnk_080DC628[][16];
extern struct GfxHeader *const gUnk_08731F78[];

void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_0800214c(void);
void sub_080021dc(void);
void sub_080022ac(void);
void sub_080022fc(void);
void sub_08002338(void);
void sub_08002358(void);
void sub_08002378(void);
void sub_08002d18(void);
void sub_08002e0c(void);
s32 sub_080058e4(u32 type, s32 idx);
void sub_08008c4c(s32 a0);
void sub_08008c64(u16 a0);
void sub_08008d98(s32 a0);
void sub_08024300(void);
void sub_08027178(void);
void sub_0803d0a0(s32 a0);
void sub_080102c0(void);

void sub_080100ac(void)
{
    s32 i;

    sub_080022ac();
    if (gUnk_03001F30 != 1) {
        gUnk_0300118C = gUnk_03000040 = gUnk_03000B08 = gUnk_03001EAC = gUnk_03001EEC = 0;
        sub_08008c64(0);
        gUnk_0200AF04 = 1;
        sub_08024300();
        if (gUnk_030023B8 != 7)
            sub_08008c4c(8);
        else
            sub_08008c4c(9);
        sub_08008d98(gUnk_030023B8);
        gUnk_03000F8C = gUnk_03000B78 = 0;
        gUnk_03001E94 = gUnk_03000FA8 = 0;
        gUnk_03000010 = 0x280000;
        gUnk_0300117C = 0x100000;
        gUnk_03000FC0 = 0x280000;
        gUnk_03001EE0 = 0x100000;
        if (gUnk_030023B8 == 7) {
            gUnk_03001ED8 &= 0xE0FF;
            gUnk_03001ED8 |= 0x3F00;
        } else {
            gUnk_03001ED8 &= 0xE0FF;
            gUnk_03001ED8 |= 0x3D00;
        }
        if (gUnk_030023B8 == 7) {
            gUnk_03000FD4 = 240;
            gUnk_03000044 = 0x1090;
            gUnk_03000B18 = 63;
            gUnk_03000F7C = 47;
        } else {
            gUnk_03000FD4 = 0x28D0;
            gUnk_03000044 = 0x1090;
            gUnk_03000B18 = 63;
            gUnk_03000F7C = 47;
        }
        for (i = 0; i <= 3; i++)
            sub_0803d0a0(i);
        sub_080102c0();
        sub_080058e4(91, 32);
        sub_08002358();
        sub_08002378();
        sub_080022fc();
        sub_0800214c();
        sub_08002e0c();
        gUnk_03005274 = 0x8800;
        do
            sub_08002d18();
        while (gUnk_030023D8 == 7);
        sub_08002338();
        sub_080021dc();
        sub_08002e0c();
        gUnk_03001ED8 &= 0xDFFF;
        gUnk_03000FD4 = gUnk_03000044 = gUnk_03000B18 = gUnk_03000F7C = 0;
        gUnk_0300118C = gUnk_03000040 = gUnk_03000B08 = gUnk_03001EAC = 0;
        sub_08027178();
    }
}

void sub_080102c0(void)
{
    struct GfxHeader *h = gUnk_08731F78[gUnk_030023B8];

    if (h != NULL) {
        LZ77UnCompWram(h->unk0C, gUnk_02020000);
        sub_080017e4(4, (u32)gUnk_02020000, (u32)gUnk_06010000, h->unk02 << 5);
        sub_080017e4(2, (u32)h->unk08, (u32)gUnk_03001570, h->unk00 << 5);
        if (gUnk_030023AC > 1)
            sub_080017e4(2, (u32)gUnk_080DC628[gUnk_03002360], (u32)gUnk_03001570, 22);
    }
    if (gUnk_030023B8 == 7)
        LZ77UnCompWram((void *)0x085E0090, gUnk_02020000);
}
