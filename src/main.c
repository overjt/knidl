#include "gba/gba.h"
#include "global.h"

/* AgbMain (0x08007300-0x080075B7, issue #33): the game's main loop, entered
 * from crt0 (`Start`) via `bx 0x08007301` after AgbInit (rom-map.md §4).  An
 * infinite dispatch loop over a u16 game-state variable (gUnk_030023D8) with
 * 23 states; the jump table lives at 0x08007328 inside the function range.
 * Not named `main`: gcc 2.9 inserts a `bl __gccmain` into any function with
 * that literal name, and the ROM has none (lessons-learned.md §3.13).
 *
 * Case bodies appear in source order (this order reproduces the ROM layout);
 * the compiler cross-jumps identical tails (e.g. the "state = 3" stores of
 * cases 1/3 and the "state = 5" tails of cases 10/21). */

extern u16 gUnk_030023D8; /* current game state (main dispatch) */
extern u16 gUnk_03002150; /* requested/next game state */
extern u8 gUnk_02007FC0;
extern vu32 gUnk_03000B00;
extern u8 gUnk_03001F30;
extern u16 gUnk_02005588[4];
extern u16 gUnk_02007D48[4];

void sub_080b7800(void);
void sub_0800b44c(void);
void sub_080091ac(void);
void sub_080096e0(void);
void sub_0800b920(void);
void sub_0800b4a8(void);
void sub_0800b514(void);
void sub_080100ac(void);
void sub_0800b5dc(void);
void sub_08007624(void);
void sub_0800b628(void);
void sub_0800791c(void);
void sub_0800783c(void);
void sub_0805b110(void);
void sub_08007f9c(void);
void sub_080cacf0(void);
void sub_080ba354(void);
void sub_080022bc(void);
void sub_08022f50(void);
void sub_08008a00(void);
void sub_080c6260(void);
void sub_080cd330(void);
void sub_080c6420(void);

void AgbMain(void)
{
    s32 i;

    gUnk_030023D8 = 0;
    sub_080b7800();
    while (1) {
        switch (gUnk_030023D8) {
        case 0:
            sub_0800b44c();
            gUnk_030023D8 = 1;
            break;
        case 1:
            if (gUnk_03000B00 == 0)
                sub_080091ac();
            gUnk_030023D8 = 3;
            break;
        case 3:
            sub_080096e0();
            gUnk_030023D8 = 4;
            gUnk_03002150 = 3;
            break;
        case 4:
            sub_0800b920();
            sub_0800b4a8();
            sub_0800b514();
            break;
        case 7:
            if (gUnk_02007FC0 != 0)
                sub_080100ac();
            gUnk_02007FC0 = 0;
            gUnk_030023D8 = gUnk_03002150;
            break;
        case 5:
            if (gUnk_02007FC0 != 0) {
                gUnk_03002150 = 5;
                gUnk_030023D8 = 7;
            } else {
                sub_0800b5dc();
                while (gUnk_030023D8 == 5) {
                    sub_0800b5dc();
                    sub_08007624();
                }
            }
            break;
        case 6:
            if (gUnk_02007FC0 != 0) {
                gUnk_03002150 = 6;
                gUnk_030023D8 = 7;
            } else {
                sub_0800b628();
                gUnk_030023D8 = 8;
            }
            break;
        case 8:
            while (gUnk_030023D8 == 8)
                sub_0800791c();
            gUnk_03002150 = 8;
            break;
        case 9:
            while (gUnk_030023D8 == 9)
                sub_0800783c();
            break;
        case 10:
            if (gUnk_03001F30 == 0)
                sub_0805b110();
            else
                sub_0800b628();
            gUnk_030023D8 = 5;
            break;
        case 13:
            sub_08007f9c();
            break;
        case 19:
            while (gUnk_030023D8 == 19)
                sub_0800791c();
            break;
        case 18:
            while (gUnk_030023D8 == 18)
                sub_0800791c();
            break;
        case 17:
            while (gUnk_030023D8 == 17)
                sub_0800791c();
            break;
        case 22:
            sub_080cacf0();
            break;
        case 14:
        case 15:
        case 16:
            sub_080ba354();
            break;
        case 20:
            for (i = 0; i < 4; i++) {
                gUnk_02007D48[i] = 1;
                gUnk_02005588[i] = 0;
            }
            sub_080022bc();
            sub_08022f50();
            while (gUnk_030023D8 == 20)
                sub_08008a00();
            gUnk_03002150 = 20;
            break;
        case 21:
            sub_0800b4a8();
            gUnk_02007FC0 = 1;
            gUnk_030023D8 = 5;
            break;
        case 11:
            sub_080c6260();
            gUnk_030023D8 = 12;
            break;
        case 12:
            if (gUnk_03001F30 != 1 && gUnk_03002150 != 20)
                sub_080cd330();
            sub_080c6420();
            gUnk_030023D8 = 0;
            break;
        case 2:
            gUnk_030023D8 = 3;
            break;
        }
    }
}
