#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* Symbolic Thumb reconstruction; the equivalent C below documents the logic. */
#if 1
asm(
    ".text\n"
    ".align 2\n"
    ".thumb\n"
    ".thumb_func\n"
    ".global sub_08018e14\n"
    "sub_08018e14:\n"
    ".include \"src/player_18e14.inc\"\n"
);
#endif

#if 0

extern s32 gUnk_02006040[];
extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];
extern s16 gUnk_08732150[];
extern s32 gUnk_03001F2C;
extern s32 gUnk_03002448;
extern u32 gUnk_080D2148[];

extern s32 sub_08002ee8(s32 n);
extern s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
extern void TaskYieldTrampoline(u32 frames);
extern void TaskDispatchTrampoline(void);

void sub_08018e14(void)
{
    register s32 i asm("r8");
    register s32 yOff asm("sl");
    register s32 xOff asm("r7");
    register s32 velocityOff asm("r9");
    {
        register s32 *base asm("r3") = gUnk_02006040;
        register s32 *lower asm("r1");
        register s32 zero asm("r2");
        register s32 *clearPtr asm("r0");

        asm("" : "+r" (base));
        lower = base;
        zero = 0;
        clearPtr = lower + 9;

        do {
            *clearPtr = zero;
            clearPtr--;
        } while ((s32)clearPtr >= (s32)lower);
    }

    gUnk_03002490->unk6C = 0;

    while (1) {
        i = 0;
        yOff = 12;
        xOff = 0;
        velocityOff = 24;
        asm("" : : "r" (i), "r" (yOff), "r" (xOff), "r" (velocityOff));

        do {
            s32 *x;
            s32 value;
            s32 magnitude;
            s32 shift;
            s32 delta;

            if ((s16)gUnk_03002490->unk6C <= 49) {
                s32 *initX = (s32 *)((u8 *)gUnk_02006040 + xOff);
                s32 current = *initX;

                if (current == 0) {
                    s32 random;
                    s32 generated;

                    *(s32 *)((u8 *)gUnk_02006040 + velocityOff) = current;
                    random = sub_08002ee8(16);

                    if (gUnk_03002490->unk43 == 1)
                        generated = gUnk_08732150[random] << 16;
                    else
                        generated = -(gUnk_08732150[random] << 16);

                    *initX = generated;
                    *(s32 *)((u8 *)gUnk_02006040 + yOff)
                        = gUnk_08732150[random + 16] << 16;
                }
            }

            x = (s32 *)((u8 *)gUnk_02006040 + xOff);
            value = *x;
            magnitude = value;
            if (magnitude < 0)
                magnitude = -magnitude;

            if (magnitude <= 0xA0000) {
                *x = 0;
            } else {
                if (*x >= 0)
                    *x -= 0x6000;
                else
                    *(s32 *)((u8 *)gUnk_02006040 + velocityOff) += 0x6000;

                *x += gUnk_02006040[i + 6];

                magnitude = *x;
                if (magnitude < 0)
                    magnitude = -magnitude;
                shift = (magnitude >> 20) + 2;

                magnitude = *(s32 *)((u8 *)gUnk_02006040 + yOff);
                if (magnitude < 0)
                    magnitude = -magnitude;
                delta = (magnitude & 0xFFFF0000) >> shift;
                if (*(s32 *)((u8 *)gUnk_02006040 + yOff) > 0)
                    delta = -delta;
                *(s32 *)((u8 *)gUnk_02006040 + yOff) += delta;

                if (*x != 0) {
                    struct Task *other = &gUnk_03002790[gUnk_03002490->unk44];

                    gUnk_03001F2C = other->unk48
                        + (*x >> 16) - 8;
                    gUnk_03002448 = other->unk4A
                        + (*(s32 *)((u8 *)gUnk_02006040 + yOff) >> 16) + 16;
                    sub_08001a94(gUnk_03002490->unk42, (u32)gUnk_080D2148,
                                 0, 0, gUnk_03001F2C, (s16)gUnk_03002448);
                }
            }

            asm("" : : "r" (i), "r" (yOff), "r" (xOff), "r" (velocityOff));
            yOff += 4;
            xOff += 4;
            velocityOff += 4;
            i++;
        } while (i <= 2);

        gUnk_03002490->unk6C++;
        if ((s16)gUnk_03002490->unk6C > 60)
            break;
        TaskYieldTrampoline(1);
    }

    TaskDispatchTrampoline();
}
#endif
