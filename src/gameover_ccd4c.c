#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* gameover_ccd4c.c (0x080CCD4C-0x080CD32F, issue #100).
 *
 * The game-over screen, task type #264 variants 2-5.
 *   sub_080ccd4c / sub_080cce98   variant 2, a sprite that follows variant 0
 *       (gUnk_02007D28) through three motion sets gUnk_087582F4[] and ends
 *       once variant 0 leaves sub-state 0.
 *   sub_080ccec8   variant 3: sub-state gUnk_08758324[Task.unk14] (1 when the
 *       cursor gUnk_02006164 is set), handlers gUnk_0875832C[Task.unk15]
 *       (sub_080ccf10): sub_080ccf2c / sub_080cd0c8 and sub_080cd0cc /
 *       sub_080cd248, two scripted sprites with empty handlers.
 *   sub_080cd24c / sub_080cd2f8   variants 4 and 5. */

extern u16 gUnk_030023AC;           /* number of players */
extern s8 gUnk_02006164;            /* game-over screen: cursor (continue = 0?) */
extern s16 gUnk_02007D28;           /* game-over screen: the #264 variant-0 task's index */
extern u32 gUnk_087549FC[];
extern s32 gUnk_087582F4[];
extern void (*gUnk_08758324[])(void);
extern void (*gUnk_0875832C[])(void);
extern u32 gUnk_08754984[];
extern u32 gUnk_08754908[];
extern u32 gUnk_087548B8[];
extern u32 gUnk_087548A8[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_08002e98(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
s32 sub_080031b8(s32 id);                                    /* play a sound effect */
void sub_08005654(s32 id);                                   /* kill task */
void sub_08006138(void);                                     /* end the running task */
void sub_080061c0(s32 a, s32 b);
void sub_0800625c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void sub_080062c4(void);
void sub_08006338(s32 a);
void sub_080cce98(void);
void sub_080ccf10(void);

/* Task type #264 variant 2. */
void sub_080ccd4c(void)
{
    gUnk_03002490->unk42 = 7;
    gUnk_03002490->unk38 = gUnk_087549FC;
    gUnk_03002490->unk04 = (u32)sub_080cce98;
    gUnk_03002490->unk43 = gUnk_03002790[gUnk_02007D28].unk43;
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(64);
    gUnk_03002490->unk24 = 0;
    for (;;) {
        gUnk_03002490->unk4C = (gUnk_03002490->unk43 << 19) + (120 << 16);
        gUnk_03002490->unk50 = 129 << 16;
        sub_080061c0(gUnk_087582F4[gUnk_03002490->unk24], gUnk_087582F4[gUnk_03002490->unk24 + 1]);
        gUnk_03002490->unk58 = gUnk_087582F4[gUnk_03002490->unk24 + 2];
        gUnk_03002490->unk60 = gUnk_087582F4[gUnk_03002490->unk24 + 3];
        gUnk_03002490->unk24 += 4;
        if (gUnk_03002490->unk24 > 11)
            gUnk_03002490->unk24 = 0;
        gUnk_03002490->unk6C = 0;
        do {
            sub_08006338(0);
            TaskYieldTrampoline(6);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(6);
            gUnk_03002490->unk3C--;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 1);
        sub_08006338(3);
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(4);
        sub_080062c4();
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(64);
    }
}

/* Task type #264 variant 2's callback: end once the variant-0 task leaves
   sub-state 0. */
void sub_080cce98(void)
{
    struct Task *t = &gUnk_03002790[gUnk_02007D28];

    if (t->unk14 != 0)
        sub_08005654(gCurTaskIdx);
}

/* Task type #264 variant 3: sub-state gUnk_08758324[Task.unk14], 1 when
   gUnk_02006164 is set, per-frame handlers gUnk_0875832C[Task.unk15]
   (sub_080ccf10). */
void sub_080ccec8(void)
{
    gUnk_03002490->unk04 = (u32)sub_080ccf10;
    if (gUnk_02006164 != 0)
        gUnk_03002490->unk14 = 1;
    else
        gUnk_03002490->unk14 = 0;
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08758324);
    sub_08006138();
}

void sub_080ccf10(void)
{
    sub_08002e98(gUnk_03002490->unk15, 2, gUnk_0875832C);
}

/* Task type #264 variant 3, sub-state 0. */
void sub_080ccf2c(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk38 = gUnk_08754984;
    gUnk_03002490->unk42 = 8;
    gUnk_03002490->unk4C = 120 << 16;
    gUnk_03002490->unk50 = 129 << 16;
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(1);
    sub_0800625c(0, -0x2B00, 0x5A5A5A5A, -0x10000, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = -0x30000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C = 2;
    gUnk_03002490->unk58 = -0x40000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    sub_0800625c(0x5A5A5A5A, -0x22B00, 0x5A5A5A5A, -0x70000, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(1);
    sub_0800625c(0x5A5A5A5A, 0x20000, 0x5A5A5A5A, -0x40000, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(1);
    gUnk_03002490->unk5C = -0x2B00;
    TaskYieldTrampoline(2);
    sub_0800625c(-0x30000, 0, 0x5A5A5A5A, 0x5A5A5A5A, 0x4000, 0x5A5A5A5A);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 5);
    sub_08006138();
}

void sub_080cd0c8(void)
{
}

/* Task type #264 variant 3, sub-state 1. */
void sub_080cd0cc(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk38 = gUnk_08754908;
    gUnk_03002490->unk42 = 5;
    gUnk_03002490->unk4C = 120 << 16;
    gUnk_03002490->unk50 = 129 << 16;
    gUnk_03002490->unk3C = 0;
    gUnk_03002490->unk54 = -0x40000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x60000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = -0x30000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0x20000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = -0x10000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk54 = 0;
    TaskYieldTrampoline(2);
    sub_0800625c(0, 0, 0x5A5A5A5A, -0x88000, 0x10000, 0x5A5A5A5A);
    TaskYieldTrampoline(16);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(8);
    sub_0800625c(0x38000, -0x10000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(6);
    sub_0800625c(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    TaskYieldTrampoline(22);
    gUnk_03002490->unk3C++;
    sub_0800625c(0, 0, 0x5A5A5A5A, -0x10000, 0x4000, 0x5A5A5A5A);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

void sub_080cd248(void)
{
}

/* Task type #264 variant 4. */
void sub_080cd24c(void)
{
    gUnk_03002490->unk42 = 5;
    gUnk_03002490->unk38 = gUnk_087548B8;
    gUnk_03002490->unk4C = 120 << 16;
    gUnk_03002490->unk50 = 129 << 16;
    gUnk_03002490->unk3C = 19;
    gUnk_03002490->unk58 = -0x38000;
    gUnk_03002490->unk60 = 0x10000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C = 0xFFFF;
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk60 = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 19;
    gUnk_03002490->unk58 = -0x38000;
    gUnk_03002490->unk60 = 0x10000;
    TaskYieldTrampoline(6);
    gUnk_03002490->unk3C = -1;
    gUnk_03002490->unk58 = 0;
    gUnk_03002490->unk60 = 0;
    TaskYieldTrampoline(10);
    gUnk_03002490->unk3C = 19;
    gUnk_03002490->unk54 = 0x38000;
    gUnk_03002490->unk5C = -0x10000;
    TaskYieldTrampoline(6);
    TaskDispatchTrampoline();
}

/* Task type #264 variant 5. */
void sub_080cd2f8(void)
{
    gUnk_03002490->unk42 = 8;
    gUnk_03002490->unk38 = gUnk_087548A8;
    gUnk_03002490->unk4C = 120 << 16;
    gUnk_03002490->unk50 = 120 << 16;
    gUnk_03002490->unk3C = gUnk_030023AC - 1;
    sub_08006138();
}
