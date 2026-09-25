#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_49b48.c (0x08049B48-0x08049F97, issue #88).
 *
 * Player action bodies, part 16: actions 44-45 and handlers 41-42, the
 * twins of M11's actions 32-33.  sub_08049b48 (action 44, mode 13) is
 * M11's sub_0804462c with other constants (Task.unk80 = 13, animations
 * 0xA0A-0xA0E, sound 140, M14's sub_08053940(player, 7, 0..1), effects
 * 40 x3 and 28): state 0 winds up, state 1 loops the animation until
 * its handler sub_08049d1c (M11's sub_08044800) re-binds state 2 once
 * Task.unk28 has run out and B is released, and state 2 winds down.
 * sub_08049d94 (action 45) is M11's sub_08044878 likewise (animations
 * 0xA86/0xA8E, effect 41 x4, sound 141); its handler sub_08049edc also
 * registers the collider gUnk_0873C214 and tests the block hit-box set
 * gUnk_0873CF4C every frame in state 1. */

extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern u32 gUnk_0873C214[];             /* collider row passed to sub_0801a828 (4th arg) */
extern u32 gUnk_0873CF4C[];             /* hit-box set, passed as (struct HitBoxSet *) */

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_08006338(s32 a);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u16 sub_08030848(struct HitBoxSet *p, s32 e);
void sub_0803e374(s32 a0, u16 a1);
void sub_0803e3ac(void);
s32 sub_0803e55c(void);
s32 sub_0804042c(void);
void sub_08053940(s32 a, s32 b, s32 c);      /* M14, still asm */
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_08049b48(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 41;
    {
        struct Task *t = gUnk_03002490;
        if (t->unk88->unk05 != 13) {
            struct Task *u;
            t->unk73 = 0;
            u = gUnk_03002490;
            u->unk28 = 15;
            u->unk80 = 13;
        }
    }
    switch (gUnk_03002490->unk73) {
    case 0:
        sub_08006338(0xA0A);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        sub_08006338(0xA0D);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk73 = 1;
        /* fallthrough */
    case 1:
        {
            struct PlayerState *p = gUnk_03002490->unk88;
            if ((p->unk42 & 128) == 0)
                sub_0803e374(140, p->unk00);
        }
        sub_08053940(gUnk_03002490->unk88->unk00, 7, 0);
        sub_08053940(gUnk_03002490->unk88->unk00, 7, 1);
        sub_0805afac(gUnk_03002490->unk88->unk00, 40, 0);
        sub_0805afac(gUnk_03002490->unk88->unk00, 40, 1);
        sub_0805afac(gUnk_03002490->unk88->unk00, 40, 2);
        sub_0805afac(gUnk_03002490->unk88->unk00, 28, 4);
        while (1) {
            sub_08006338(0x9FA);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk6C = 0;
            do {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
            } while ((s16)++gUnk_03002490->unk6C <= 14);
        }
    case 2:
        sub_08006338(0xA0E);
        TaskYieldTrampoline(3);
        sub_0803e3ac();
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk73 = 3;
        break;
    }
    sub_08006138();
}

void sub_08049d1c(void)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk73) {
    case 0:
        break;
    case 1:
        if (t->unk28 == 0) {
            u16 *p = (u16 *)gUnk_03002458;
            if ((p[t->unk88->unk00] & 2) == 0) {
                t->unk73 = 2;
                sub_08006148(sub_08049b48, gCurTaskIdx);
            }
        } else {
            t->unk28--;
        }
        break;
    case 2:
        break;
    case 3:
        sub_0804042c();
        break;
    }
    sub_0803e55c();
}

void sub_08049d94(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 42;
    {
        struct Task *t = gUnk_03002490;
        if (t->unk88->unk05 != 13) {
            struct Task *u;
            t->unk73 = 0;
            u = gUnk_03002490;
            u->unk28 = 15;
            u->unk80 = 14;
        }
    }
    switch (gUnk_03002490->unk73) {
    case 0:
        sub_08006338(0xA8E);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk73 = 1;
        /* fallthrough */
    case 1:
        sub_0805afac(gUnk_03002490->unk88->unk00, 41, 0);
        sub_0805afac(gUnk_03002490->unk88->unk00, 41, 1);
        sub_0805afac(gUnk_03002490->unk88->unk00, 41, 2);
        sub_0805afac(gUnk_03002490->unk88->unk00, 41, 3);
        sub_0803e374(141, gUnk_03002490->unk88->unk00);
        while (1) {
            sub_08006338(0xA86);
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C = 0;
            do {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(1);
            } while ((s16)++gUnk_03002490->unk6C <= 6);
        }
    case 2:
        sub_0803e3ac();
        sub_08006338(0xA8E);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk73 = 3;
        break;
    }
    sub_08006138();
}

void sub_08049edc(void)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk73) {
    case 0:
        break;
    case 1:
        if (t->unk28 == 0) {
            if ((gUnk_03002458[t->unk88->unk00] & 2) == 0) {
                t->unk73 = 2;
                sub_08006148(sub_08049d94, gCurTaskIdx);
            }
        } else {
            t->unk28--;
        }
        sub_0801a828((u8)gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                     gUnk_0873C214);
        sub_08030848((struct HitBoxSet *)gUnk_0873CF4C, gUnk_03002490->unk88->unk00);
        break;
    case 2:
        break;
    case 3:
        sub_0804042c();
        break;
    }
    sub_0803e55c();
}
