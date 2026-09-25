#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_45d34.c (0x08045D34-0x0804632F, issue #87).
 *
 * Player action bodies, part 14: action 38 and per-frame handler 35.
 * sub_08045d34 (action 38, mode 13) spends one charge of the ability
 * counter PlayerState.unk0E (the HUD update sub_08009fcc) and plays one
 * of three sequences picked by the charges left (Task.unk73 = unk0E - 1),
 * each with its own song (sub_08003564) and sound; meanwhile it freezes
 * the stage (gUnk_03001F34 = 1), switches the DISPCNT shadow
 * gUnk_03001ED8 to BG0, BG2, BG3 and OBJ unless BG2 is already on, raises
 * PlayerState.unk42 bits 8-10 and repeats the last loop until the effect
 * counter PlayerState.unk16 runs out.  With the last charge spent it
 * drops the ability (sub_0800a130) unless the ability is 7.  Its handler
 * sub_080462f0 requests action 1 or 7 (on the ground or in the air) once
 * it has finished. */

extern vu16 gUnk_03001ED8;              /* DISPCNT shadow */
extern u8 gUnk_03001F34;
extern u32 gUnk_0873CB1C[];

void TaskYieldTrampoline(s32 frames);
s32 sub_080031b8(s32 id);
s32 sub_08003564(s32 songId);
void sub_08006138(void);
void sub_08006338(s32 a);
s32 sub_08009fcc(s32 a, s32 b, u32 c);
void sub_0800a130(s32 a, s32 id);
void sub_080261d4(u16 a);
void sub_08027204(u32 a);
void sub_0803e050(s32 a0);
void sub_0803e1b8(s32 a0, s32 a1, s32 a2);
void sub_0803e3e4(s32 a0);
void sub_0803e650(s32 a0);
void sub_0803f9c0(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_08045d34(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 35;
    gUnk_03002490->unk73 = gUnk_03002490->unk88->unk0E - 1;
    gUnk_03002490->unk88->unk42 &= 0xFFEF;
    if (--gUnk_03002490->unk88->unk0E == 0) {
        sub_08009fcc(0, -1, gUnk_03002490->unk88->unk00);
    } else {
        struct PlayerState *p = gUnk_03002490->unk88;
        sub_08009fcc(p->unk0D, p->unk0E, p->unk00);
    }
    gUnk_03001F34 = 1;
    sub_0803e050(3);
    sub_0803e3e4(15);
    if (!(gUnk_03001ED8 & 0x400)) {
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1D00;
    }
    gUnk_03002490->unk88->unk16 = 0;
    sub_0805afac(gUnk_03002490->unk88->unk00, 37, 0);
    gUnk_03002490->unk28 = 0;
    sub_0803e1b8(3, 0, gUnk_03002490->unk88->unk00);
    gUnk_03002490->unk88->unk42 |= 0x700;
    switch (gUnk_03002490->unk73) {
    case 2:
        sub_08040b40(11, 42);
        sub_08006338(0x66B);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        sub_08040b40(11, 43);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_0803e050(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        sub_08006338(0x670);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        sub_080031b8(160);
        gUnk_03002490->unk88->unk16++;
        sub_080261d4(4);
        sub_08027204(2);
        sub_0805afac(gUnk_03002490->unk88->unk00, 37, 1);
        gUnk_03002490->unk6C = 0;
        do {
            sub_0803e650(4);
            sub_08006338(0x674);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gUnk_03002490->unk6C <= 7);
        while ((s8)gUnk_03002490->unk88->unk16 != 0) {
            sub_0803e650(4);
            sub_08006338(0x674);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
        sub_08006338(0x670);
        TaskYieldTrampoline(4);
        break;
    case 1:
        sub_08006338(0x676);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk6C = 0;
        do {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(4);
        } while ((s16)++gUnk_03002490->unk6C <= 5);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        sub_08003564(160);
        TaskYieldTrampoline(1);
        sub_080031b8(161);
        gUnk_03002490->unk88->unk16++;
        sub_080261d4(4);
        sub_08027204(2);
        sub_0805afac(gUnk_03002490->unk88->unk00, 37, 1);
        gUnk_03002490->unk6C = 0;
        do {
            sub_0803e650(4);
            sub_08006338(0x67D);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gUnk_03002490->unk6C <= 7);
        while ((s8)gUnk_03002490->unk88->unk16 != 0) {
            sub_0803e650(4);
            sub_08006338(0x67D);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
        sub_08006338(0x679);
        TaskYieldTrampoline(4);
        break;
    case 0:
        sub_08006338(0x67F);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(5);
        gUnk_03002490->unk88->unk68 = 0;
        sub_080413a4(35);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(4);
        sub_08006338(0x684);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        sub_08003564(161);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk68 = (u32)gUnk_0873CB1C;
        sub_0803e050(2);
        sub_080031b8(162);
        gUnk_03002490->unk88->unk16++;
        sub_080261d4(4);
        sub_08027204(2);
        sub_0805afac(gUnk_03002490->unk88->unk00, 37, 1);
        gUnk_03002490->unk6C = 0;
        do {
            sub_0803e650(4);
            sub_08006338(0x689);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gUnk_03002490->unk6C <= 7);
        while ((s8)gUnk_03002490->unk88->unk16 != 0) {
            sub_0803e650(4);
            sub_08006338(0x689);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
        }
        sub_08006338(0x683);
        TaskYieldTrampoline(4);
        break;
    }
    sub_0803e1b8(255, 0, gUnk_03002490->unk88->unk00);
    sub_0803e3e4(0);
    {
        struct PlayerState *p = gUnk_03002490->unk88;
        if (p->unk0D != 7)
            sub_0800a130(p->unk0D, p->unk00);
        else
            p->unk22 = 2;
    }
    gUnk_03002490->unk28++;
    gUnk_03001F34 = 0;
    gUnk_03002490->unk88->unk42 &= 0xF8FF;
    sub_08006138();
}

void sub_080462f0(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk28 != 0) {
        if (t->unk7A & 1)
            t->unk88->unk01 = 1;
        else
            t->unk88->unk01 = 7;
    }
    sub_0803f9c0();
}
