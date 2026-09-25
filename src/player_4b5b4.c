#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4b5b4.c (0x0804B5B4-0x0804B857, issue #88).
 *
 * Player action bodies, part 20: action 52 and per-frame handler 49.
 * sub_0804b5b4 (action 52, mode 13) is a linear yield script with the
 * stage frozen (gUnk_03001F34 = 1): M14's sub_08053940(player, 9, 0),
 * the animations 0xDFD-0xE05 in Task.unk6C counter loops and velocity
 * presets 54/55; it then restores the default script gUnk_0873CB1C in
 * PlayerState.unk68, resets the HUD ability panel (sub_0800a008(0, -1,
 * player)) and plays 0xE06 on the ground.  Its handler sub_0804b818
 * requests action 1 (on the ground) or 7 once Task.unk28 is set; the
 * ROM keeps a dead `ldr [t, #84]` of a test whose two arms were
 * merged. */

extern u8 gUnk_03001F34;
extern u32 gUnk_0873CB1C[];

void TaskYieldTrampoline(s32 frames);
void sub_08006138(void);
void sub_08006338(s32 a);
s32 sub_0800a008(s32 a, s32 b, u32 c);       /* landed (hud_099fc.c); M12 calls it as sub_0800a008(0, -1, p->unk00) */
void sub_0803e050(s32 a0);
void sub_0803e3e4(s32 a0);
void sub_0803f9c0(void);
void sub_080413a4(s32 a0);
void sub_08053940(s32 a, s32 b, s32 c);      /* M14, src/plobj_52f6c.c (defined s32 (s8, u8, s32); the result is unused here) */

void sub_0804b5b4(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 49;
    gUnk_03001F34 = 1;
    gUnk_03002490->unk80 = 0;
    sub_0803e050(3);
    {
        struct Task *t = gUnk_03002490;
        t->unk28 = 0;
        t->unk2C = 0;
        t->unk88->unk42 |= 0x700;
        t->unk88->unk68 = 0;
    }
    sub_0803e3e4(15);
    sub_08053940(gUnk_03002490->unk88->unk00, 9, 0);
    gUnk_03002490->unk6C = 0;
    do {
        sub_08006338(0xDFD);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 3);
    gUnk_03002490->unk6C = 0;
    do {
        sub_08006338(0xDFF);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 1);
    sub_08006338(0xE01);
    TaskYieldTrampoline(1);
    sub_080413a4(54);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk6C = 0;
    do {
        sub_08006338(0xE03);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 3);
    sub_0803e050(2);
    gUnk_03002490->unk6C = 0;
    do {
        sub_08006338(0xE03);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(1);
        sub_08006338(0xE05);
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 4);
    sub_080413a4(55);
    gUnk_03002490->unk6C = 0;
    do {
        sub_08006338(0xE03);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 15);
    {
        struct Task *t = gUnk_03002490;
        t->unk88->unk68 = (u32)gUnk_0873CB1C;
        sub_0800a008(0, -1, t->unk88->unk00);
    }
    sub_0803e3e4(0);
    if (gUnk_03002490->unk7A & 1) {
        sub_08006338(0xE06);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
    }
    {
        struct Task *t = gUnk_03002490;
        t->unk28++;
        gUnk_03001F34 = 0;
        t->unk88->unk42 &= 0xF8FF;
    }
    sub_08006138();
}

void sub_0804b818(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk28 != 0) {
        if (t->unk7A & 1) {
            /* Both arms store 1: jump2 cross-jumps them after reload and
               deletes the branch, but the `ldr [t, #84]` that fed the test
               stays in the ROM (0x0804B834) as a dead load. */
            if (t->unk54 == 0)
                t->unk88->unk01 = 1;
            else
                t->unk88->unk01 = 1;
        } else {
            t->unk88->unk01 = 7;
        }
    }
    sub_0803f9c0();
}
