#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* credits_cd330.c (0x080CD330-0x080CD89B, issue #100).
 *
 * AgbMain state 12, part 1: the staff credits (skipped in link play and
 * after AgbMain state 20).  The credits text scrolls up BG0 over a run of
 * recorded demos of the game, one per scene.
 *   sub_080cd330   the sequence: per scene of gUnk_087583CC[n] (n by
 *       gUnk_03002464), load the recorded input (gUnk_0200EC50,
 *       sub_080b6f38) and the room (sub_080cd674), fade in, play it for
 *       gUnk_0875841E[n][scene] frames and fade out; then fade the music and
 *       the screen and put back the player's score the demos overwrote.
 *   sub_080cd674   load a scene's room and reset the per-scene state (a twin
 *       of M02's sub_0800b788).
 *   sub_080cd70c / sub_080cd75c / sub_080cd828   the text layer: load it,
 *       stream the 14 compressed pages gUnk_087583B4[] into the two BG0 map
 *       halves, and scroll BG0 from the per-frame callback gUnk_03000AF4. */

extern vu16 gUnk_03001ED8;          /* DISPCNT shadow */
extern vs32 gUnk_03000010;          /* BG0 16.16 scroll shadows ... */
extern vs32 gUnk_0300117C;
extern u16 gUnk_03002360;           /* this player's index */
extern u8 gUnk_03002464;
extern u16 gUnk_02000028;
extern s32 gUnk_02006020[];         /* score per player */
extern vu16 gUnk_03001EB8[];        /* keys pressed per player */
extern vu8 gUnk_0300118C;
extern vu8 gUnk_03000040;
extern vu8 gUnk_03000B08;
extern vu8 gUnk_03001EAC;
extern u16 gUnk_03001EEC;
extern vu16 gUnk_03001EDC;
extern s32 gUnk_0201C1A4;           /* credits: the score saved over the demos */
extern u8 gUnk_030023B0;
extern u8 gUnk_0201C1B0;            /* credits: current demo scene */
extern u32 gUnk_087583CC[][8];      /* credits: per variant, the scenes' recorded demos, 0-terminated */
extern u16 gUnk_0875841E[][7];      /* credits: per variant, the scenes' lengths in frames */
extern vu16 gUnk_03001270[];
extern s16 gUnk_0200EC58;
extern u32 gUnk_0200EC50;
extern u16 gUnk_02008008[];
extern u16 gUnk_02007FA8[];
extern u16 gUnk_08758334[];
extern u16 gUnk_08758374[];
extern vu16 gUnk_030004A4;
extern vs16 gUnk_03000FB8;
extern vu16 gUnk_03001E90;
extern u32 gUnk_03000AF4;
extern u8 gUnk_020061E0;
extern u16 gUnk_02000008;
extern u8 gUnk_02006178;
extern u8 gUnk_02007CF0;
extern s8 gUnk_02007FB8[];
extern vu16 gUnk_03000F98[];
extern u8 gUnk_03001F34;
extern u16 gUnk_030023C0[];
extern u16 gUnk_03002458[];
extern s32 gUnk_0201C1A0;           /* credits: BG0 vertical scroll, 16.16 */
extern s32 gUnk_0201C1AC;           /* credits: BG0 horizontal scroll, 16.16 */
extern u8 gUnk_0201C1A8;
extern u8 gUnk_0201C19C;
extern s32 gUnk_0201C1B4;           /* credits: scroll since the last page copy, 1/16 pixel */
extern u32 *gUnk_087583B4[];        /* credits: the 14 compressed text pages */
extern u16 gUnk_02005600[];

u32 sub_080008e8(u16 steps, s16 delta, u16 *mask);           /* delta is signed: the ROM passes -2 as movs/negs */
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_08001fd0(void);
void sub_080022ac(void);
void sub_08002d18(void);                                     /* run one frame */
void sub_08002d74(s32 count);
s32 sub_08003110(s32 songId);
void sub_08003484(void);
void sub_0800374c(s32 speed);
void sub_08003770(u16 volume);
void sub_08008c4c(s32 a0);                                   /* load palette set */
void sub_08008c64(u16 a0);                                   /* load screen graphics */
void sub_0800b4a8(void);
void sub_0800b514(void);
void sub_0801a7b4(void);
void sub_0802497c(void);
void sub_08040788(void);
void sub_08066144(void);
void sub_080b6f38(void);
void sub_080b77d4(void);
void sub_080cd674(void);
void sub_080cd70c(void);
void sub_080cd75c(void);
void sub_080cd828(void);

/* AgbMain state 12, part 1: the staff credits.  The credits text scrolls up
   BG0 (sub_080cd70c, sub_080cd75c) over a run of recorded demos, one per
   scene of gUnk_087583CC[n] (n = 2 when gUnk_03002464 is 1, else 1), each
   faded in, played for its length gUnk_0875841E[n][scene] and faded out;
   then the music and the screen fade out and the player's score, which the
   demos overwrite, is put back. */
void sub_080cd330(void)
{
    u32 *scenes;
    u16 *frames;
    u16 n;

    gUnk_03001ED8 |= 0x80;
    gUnk_0300118C = gUnk_03000040 = gUnk_03000B08 = gUnk_03001EAC = gUnk_03001EEC = 0;
    sub_080022ac();
    sub_08008c4c(17);
    gUnk_03001EDC = 1;
    gUnk_0201C1A4 = gUnk_02006020[gUnk_02000028];
    gUnk_03002360 = 0;
    if (gUnk_03002464 == 1)
        gUnk_030023B0 = 2;
    else
        gUnk_030023B0 = 1;
    gUnk_0201C1B0 = 0;
    scenes = gUnk_087583CC[gUnk_030023B0];
    frames = gUnk_0875841E[gUnk_030023B0];
    sub_080cd70c();
    sub_0800b4a8();
    sub_0800b514();
    sub_08008c64(0);
    gUnk_03001270[0] = 0;
    gUnk_0200EC58 = 3;
    gUnk_03001ED8 &= ~0x80;
    sub_08003110(20);
    for (;;) {
        gUnk_0200EC50 = scenes[gUnk_0201C1B0];
        sub_080b6f38();
        gUnk_02008008[gUnk_03002360] = 0;
        gUnk_02007FA8[gUnk_03002360] = 0xFFFF;
        sub_080cd674();
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1D00;
        if (gUnk_0201C1B0 == 0)
            sub_080008e8(15, -2, gUnk_08758334);
        else
            sub_080008e8(15, 2, gUnk_08758334);
        n = 15;
        while (n-- != 0) {
            sub_08002d18();
            if ((s16)gUnk_030004A4 < 0) {
                if (gUnk_03000FB8 < -5) {
                    gUnk_03001E90 = 0;
                    gUnk_03000FB8 = -5;
                }
            } else if (gUnk_03000FB8 > -5) {
                gUnk_03001E90 = 0;
                gUnk_03000FB8 = -5;
            }
            sub_080cd75c();
            sub_080b77d4();
        }
        gUnk_03000FB8 = -5;
        n = frames[gUnk_0201C1B0];
        while (--n != 0) {
            sub_0801a7b4();
            sub_08002d18();
            sub_080cd75c();
            sub_080b77d4();
            sub_08040788();
        }
        if (scenes[++gUnk_0201C1B0] != 0) {
            sub_080008e8(15, -2, gUnk_08758334);
            n = 15;
            while (n-- != 0) {
                sub_08002d18();
                sub_080cd75c();
                sub_080b77d4();
            }
            gUnk_03001ED8 &= 0xE0FF;
            gUnk_03001ED8 |= 0x100;
            sub_080022ac();
            sub_08002d18();
            continue;
        }
        break;
    }
    sub_0800374c(8);
    gUnk_03001270[0] = 0xFFFF;
    sub_080008e8(30, 2, gUnk_08758374);
    n = 30;
    while (n-- != 0) {
        sub_08002d18();
        sub_080b77d4();
        sub_08040788();
    }
    gUnk_03001ED8 |= 0x80;
    gUnk_03000AF4 = 0;
    gUnk_03001EDC = 0;
    sub_08003484();
    sub_080022ac();
    sub_08002d74(2);
    sub_08003770(255);
    gUnk_02006020[gUnk_02000028] = gUnk_0201C1A4;
}

/* Load the staff credits' next demo scene: the room, palette set 17, and
   the per-scene state M02's stage loaders reset (sub_0800b788's twin). */
void sub_080cd674(void)
{
    s32 i;
    s8 *b;
    s8 *p;
    s8 zero;

    sub_08001fd0();
    sub_0801a7b4();
    sub_08008c4c(17);
    sub_0802497c();
    b = gUnk_02007FB8;
    zero = 0;
    p = b + 2;
    do {
        *p = zero;
        p--;
    } while ((s32)p >= (s32)b);
    gUnk_020061E0 = 0;
    sub_08066144();
    gUnk_02000008 = 0;
    gUnk_02006178 = 0;
    gUnk_02007CF0 = 0;
    gUnk_03001F34 = 1;
    for (i = 0; i < 4; i++)
        gUnk_03000F98[i] = gUnk_03001EB8[i] = gUnk_03002458[i] = gUnk_030023C0[i] = 0;
}

/* Start the staff credits' text layer: load its screen, reset the BG0
   scroll and the page counters and install the scroll callback. */
void sub_080cd70c(void)
{
    sub_08008c64(72);
    gUnk_0201C1A0 = 0x400000;
    gUnk_0201C1AC = 0x40000;
    gUnk_03000AF4 = (u32)sub_080cd828;
    gUnk_0201C1A8 = 0;
    gUnk_0201C19C = 0;
    gUnk_0201C1B4 = gUnk_0201C1A0 >> 12;
}

/* Stream the staff credits' text into BG0: stage the next page (a blank one
   after the 14 pages) in gUnk_02005600, and each time another 256 pixels
   have scrolled by copy it into one of the two BG0 map halves (0x06001000
   for odd pages, 0x06001800 for even ones). */
void sub_080cd75c(void)
{
    u16 zero;

    if (gUnk_0201C19C == 0) {
        if (gUnk_0201C1A8 < 14) {
            LZ77UnCompWram(gUnk_087583B4[gUnk_0201C1A8], gUnk_02005600);
        } else {
            zero = 0;
            CpuSet(&zero, gUnk_02005600, 0x01000400);
        }
        gUnk_0201C19C = 1;
        gUnk_0201C1A8++;
    }
    if ((gUnk_0201C1B4 & 0x1000) && gUnk_0201C19C != 0) {
        if (gUnk_0201C1A8 & 1)
            sub_080017e4(1, (u32)gUnk_02005600, 0x06001000, 0x800);
        else
            sub_080017e4(1, (u32)gUnk_02005600, 0x06001800, 0x800);
        gUnk_0201C19C = 0;
        gUnk_0201C1B4 = 0;
    }
}

/* The staff credits' per-frame callback (installed in gUnk_03000AF4 by
   sub_080cd70c): scroll BG0 up by half a pixel a frame until the text has
   gone by, then park it through the BG0 scroll shadows. */
void sub_080cd828(void)
{
    if (gUnk_0201C1A0 < 0x0F580000) {
        gUnk_0201C1A0 += 0x8000;
        gUnk_0201C1B4 += 8;
        REG_BG0VOFS = gUnk_0201C1A0 >> 16;
        REG_BG0HOFS = gUnk_0201C1AC >> 16;
    } else {
        gUnk_03000010 = 0x0F580000;
        gUnk_0300117C = 0x40000;
        REG_BG0VOFS = gUnk_03000010 >> 16;
        REG_BG0HOFS = gUnk_0300117C >> 16;
    }
}
