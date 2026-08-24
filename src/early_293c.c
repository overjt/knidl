#include "gba/gba.h"
#include "global.h"

extern void sub_080022d0(void);
extern void sub_08006914(void *, void *, void *);
extern int sub_080072e0(void);
extern void sub_08008b8c(void);
extern void sub_08002d54(int);
extern void sub_08006868(void);

extern u16 gUnk_03001F38;
extern u16 gUnk_03005274;
extern u16 gUnk_0300243C;
extern u16 gUnk_03004D50[4][4];
extern u32 gUnk_03004D70;
extern u32 gUnk_03004D7C;
extern u16 gUnk_03004D88;
extern u16 gUnk_03004D90[4];
extern u32 gUnk_0200EBA0;

void sub_0800293c(void)
{
    int a;
    u32 b;
    int c;
    int i;
    u32 old;

    if (gUnk_03001F38 != 0)
        gUnk_03005274 = 0x7755;
    else
        gUnk_03005274 = 0x9900;

    if (gUnk_0300243C <= 1)
        return;

    a = 0;
    b = 0;
    c = 0;
    for (;;) {
        switch (gUnk_03005274) {
        case 0x7755:
            gUnk_03004D90[0] = 0x7755;
            gUnk_03005274 = 0xAA00;
            break;
        case 0xAA00:
            c = 30;
            gUnk_03004D90[0] = 0xAA00;
            gUnk_03005274 = 0x9900;
            break;
        case 0xAA01:
            gUnk_03004D90[0] = 0xAA01;
            gUnk_03005274 = 0x9900;
            break;
        case 0xAA02:
            gUnk_03004D90[0] = 0xAA02;
            break;
        }
        old = gUnk_03004D7C;
        sub_080022d0();
        sub_08006914(&gUnk_03004D88, gUnk_03004D90, gUnk_03004D50);
        if (sub_080072e0() != 0)
            sub_08008b8c();
        if (old == gUnk_03004D7C) {
            if (++b > 30)
                sub_08008b8c();
        }
        for (i = 0; i < 4; i++) {
            switch (gUnk_03004D50[0][i]) {
            case 0x9900:
                break;
            case 0xAA00:
                gUnk_03005274 = 0xAA01;
                break;
            case 0xAA01:
                if (gUnk_03001F38 != 0) {
                    if (++a >= gUnk_0300243C)
                        gUnk_03005274 = 0xAA02;
                }
                break;
            case 0xAA02:
                goto done;
            }
        }
        if (c != 0) {
            if (a == gUnk_0300243C)
                c = 0;
            else if (--c == 0) {
                gUnk_03005274 = 0xAA00;
                a = 0;
            }
        }
    }
done:
    if (gUnk_03001F38 != 0)
        gUnk_0200EBA0 = 0;
    sub_08002d54(5);
    sub_08006868();
    gUnk_03004D70 = 0;
}
