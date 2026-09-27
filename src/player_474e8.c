#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_474e8.c (0x080474E8-0x08047FE7, issue #87).
 *
 * Player action bodies, part 17: actions 41-43 and per-frame handlers
 * 38-40, the last entries M12 holds.  sub_080474e8 (action 41, mode 13)
 * plays one of two animation scripts (0x8D2... or 0x8DF..., picked by
 * Task.unk7B bit 0 and remembered in Task.unk2C) with sound 151 and
 * effect 36; its handler sub_080477cc calls sub_0804042c once the script
 * has finished (Task.unk28 != 0) and requests action 23 when
 * sub_0803fce4(0) fires while Task.unk58 > 0.  sub_08047844 (action 42)
 * is a long scripted sequence (animations 0x8E5-0x8F7, sounds 180 and 274, effect
 * 38 steps 0-3) that holds PlayerState.unk42 bit 1 and releases it with
 * the HUD call sub_0800a008(0, -1, player); its handler sub_08047bd8 does
 * the same release when the ability PlayerState.unk0D is 11.
 * sub_08047c30 (action 43) is a re-entrant three-state machine: state 0
 * installs the hit boxes gUnk_0873C1B0 and gUnk_0873CEEC in the player
 * records gUnk_020060E0[]/gUnk_02005550[] and steps the frame index
 * Task.unk46, state 1 plays frames 5-10 and waits while B is held,
 * state 2 plays the release.  Its handler sub_08047e74 shows frame
 * gUnk_0873DADE[Task.unk46][Task.unk28] (Task.unk28 = the ground flag
 * Task.unk7A of the previous frame), registers the matching boxes and
 * re-binds the coroutine when the ground flag changes. */

/* M11's per-player records (src/stage_43654.c spells them the same way) */
struct M11R8 { u8 unk00; u8 unk01; u8 unk02; u8 unk03; u8 *unk04; };

struct M11R20 { u32 w[5]; };

extern struct M11R8 gUnk_02005550[];
extern struct M11R20 gUnk_020060E0[];
extern u16 gUnk_03002458[];             /* held keys, latched per player (M11) */
extern u32 gUnk_0873C1B0[];
extern u32 gUnk_0873CEEC[];
extern u16 gUnk_0873DADE[][2];
extern u32 gUnk_0873C1C4[];
extern u32 gUnk_0873C1EC[];
extern u32 gUnk_0873CEF4[];
extern u32 gUnk_0873CF1C[];

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
s32 sub_0800a008(s32 a, s32 b, u32 c);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u16 sub_08030848(struct HitBoxSet *p, s32 e);
s32 sub_0803e34c(s32 a0, u16 a1);
s32 sub_0803e4ec(s32 a0);
s32 sub_0803e55c(void);
s32 sub_0803e5c0(s32 playerIdx, u8 *src6);
s32 sub_0803e5f8(s32 a0, s32 a1);
void sub_0803f9c0(void);
s32 sub_0803fce4(s32 a);
s32 sub_0804042c(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_080474e8(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 38;
    {
        struct Task *t = gCurTask;
        t->unk28 = 0;
        if (t->unk7B & 1)
            t->unk2C = 1;
        else
            t->unk2C = 0;
    }
    gCurTask->unk80 = 10;
    sub_08040b40(0, 72);
    sub_0803e34c(151, gCurTask->unk88->unk00);
    if (gCurTask->unk2C == 0) {
        TaskSetFrame(0x8D2);
        TaskYieldTrampoline(4);
        if (gCurTask->unk7A & 1)
            sub_0805afac(gCurTask->unk88->unk00, 28, 3);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
        } while ((s16)++gCurTask->unk6C <= 4);
        sub_0805afac(gCurTask->unk88->unk00, 36, 0);
        sub_0805afac(gCurTask->unk88->unk00, 36, 1);
        sub_0805afac(gCurTask->unk88->unk00, 36, 2);
        gCurTask->unk6C = 0;
        do {
            TaskSetFrame(0x8D8);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 2);
        TaskSetFrame(0x8DA);
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
    } else {
        TaskSetFrame(0x8DF);
        TaskYieldTrampoline(4);
        if (gCurTask->unk7A & 1)
            sub_0805afac(gCurTask->unk88->unk00, 28, 3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        TaskSetFrame(0x8D5);
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        sub_0805afac(gCurTask->unk88->unk00, 36, 0);
        sub_0805afac(gCurTask->unk88->unk00, 36, 1);
        sub_0805afac(gCurTask->unk88->unk00, 36, 2);
        gCurTask->unk6C = 0;
        do {
            TaskSetFrame(0x8D8);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 2);
        TaskSetFrame(0x8DA);
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        TaskSetFrame(0x8E2);
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
    }
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_080477cc(void)
{
    struct Task *t = gCurTask;
    struct Task *u;

    if (t->unk28 != 0) {
        sub_0804042c();
    } else if (t->unk58 > 0) {
        if (sub_0803fce4(0) != 0)
            gCurTask->unk88->unk01 = 23;
    }
    u = gCurTask;
    if ((u->unk7A & 1) == 0) {
        if ((u->unk7B & 1) == 0)
            sub_080413a4(2);
        else
            sub_080413a4(13);
    } else {
        sub_0803e4ec(1);
    }
    sub_0803f9c0();
}

void sub_08047844(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 39;
    gCurTask->unk28 = 0;
    gCurTask->unk88->unk42 |= 2;
    sub_08040b40(0, 72);
    TaskSetFrame(0x8E5);
    TaskYieldTrampoline(16);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C--;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C--;
    TaskYieldTrampoline(16);
    TaskSetFrame(0x8EF);
    TaskYieldTrampoline(2);
    TaskSetFrame(0x8E6);
    TaskYieldTrampoline(4);
    gCurTask->unk3C--;
    TaskYieldTrampoline(8);
    TaskSetFrame(0x8EF);
    TaskYieldTrampoline(4);
    TaskSetFrame(0x8E6);
    TaskYieldTrampoline(12);
    sub_0803e34c(180, gCurTask->unk88->unk00);
    TaskSetFrame(0x8F0);
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    TaskSetFrame(0x8E7);
    TaskYieldTrampoline(6);
    gCurTask->unk3C++;
    TaskYieldTrampoline(6);
    gCurTask->unk3C++;
    TaskYieldTrampoline(6);
    sub_0805afac(gCurTask->unk88->unk00, 38, 0);
    TaskSetFrame(0x8EA);
    TaskYieldTrampoline(20);
    gCurTask->unk6C = 0;
    do {
        TaskSetFrame(0x8E9);
        TaskYieldTrampoline(12);
        gCurTask->unk3C--;
        TaskYieldTrampoline(12);
        gCurTask->unk3C--;
        TaskYieldTrampoline(12);
        gCurTask->unk3C++;
        TaskYieldTrampoline(10);
        sub_0803e34c(180, gCurTask->unk88->unk00);
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(12);
        sub_0805afac(gCurTask->unk88->unk00, 38, (s16)gCurTask->unk6C + 1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(12);
    } while ((s16)++gCurTask->unk6C <= 1);
    sub_0803e34c(274, gCurTask->unk88->unk00);
    TaskSetFrame(0x8E9);
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    TaskSetFrame(0x8EB);
    TaskYieldTrampoline(4);
    sub_0805afac(gCurTask->unk88->unk00, 38, 3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(16);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C--;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk88->unk42 &= 0xFFFD;
    sub_0800a008(0, -1, gCurTask->unk88->unk00);
    gCurTask->unk3C--;
    TaskYieldTrampoline(32);
    TaskSetFrame(0x8F6);
    TaskYieldTrampoline(6);
    if (gCurTask->unk7A & 1) {
        sub_080413a4(37);
        TaskSetFrame(0x8F7);
        TaskYieldTrampoline(2);
    }
    TaskSetFrame(244);
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_08047bd8(void)
{
    if (sub_0803e55c() != 0) {
        struct Task *t = gCurTask;
        if (t->unk88->unk0D == 11) {
            t->unk88->unk42 &= 0xFFFD;
            sub_0800a008(0, -1, t->unk88->unk00);
        }
    } else if (gCurTask->unk28 != 0) {
        sub_0804042c();
    }
}

void sub_08047c30(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 40;
    {
        struct Task *t = gCurTask;
        if (t->unk88->unk05 != 13) {
            t->unk73 = 0;
            gCurTask->unk80 = 12;
        }
    }
    switch (gCurTask->unk73) {
    case 0:
        {
            struct Task *t = gCurTask;
            t->unk2C = -1;
            gUnk_020060E0[t->unk88->unk00] = *(struct M11R20 *)gUnk_0873C1B0;
        }
        gUnk_02005550[gCurTask->unk88->unk00] = *(struct M11R8 *)gUnk_0873CEEC;
        gCurTask->unk28 = gCurTask->unk7A;
        gCurTask->unk46 = 0;
        TaskYieldTrampoline(8);
        sub_0803e34c(138, gCurTask->unk88->unk00);
        gCurTask->unk46 = 1;
        TaskYieldTrampoline(4);
        gCurTask->unk2C++;
        gCurTask->unk46 = 2;
        TaskYieldTrampoline(2);
        gCurTask->unk2C++;
        gCurTask->unk46 = 3;
        TaskYieldTrampoline(2);
        gCurTask->unk2C++;
        gCurTask->unk46 = 4;
        TaskYieldTrampoline(2);
        gCurTask->unk73 = 1;
        if (gCurTask->unk7A & 1) {
            sub_0805afac(gCurTask->unk88->unk00, 39, 0);
            sub_0805afac(gCurTask->unk88->unk00, 39, 1);
        }
        /* fallthrough */
    case 1:
        gCurTask->unk2C = 3;
        gCurTask->unk46 = 5;
        TaskYieldTrampoline(2);
        gCurTask->unk2C = 4;
        gCurTask->unk46 = 6;
        TaskYieldTrampoline(2);
        gCurTask->unk46 = 7;
        TaskYieldTrampoline(2);
        gCurTask->unk46 = 8;
        TaskYieldTrampoline(2);
        gCurTask->unk46 = 9;
        TaskYieldTrampoline(2);
        gCurTask->unk46 = 10;
        while (gUnk_03002458[gCurTask->unk88->unk00] & 2)
            TaskYieldTrampoline(1);
        gCurTask->unk73 = 2;
        /* fallthrough */
    case 2:
        gCurTask->unk2C = 1;
        gCurTask->unk46 = 3;
        TaskYieldTrampoline(2);
        gCurTask->unk46 = 0;
        TaskYieldTrampoline(3);
        gCurTask->unk73 = 3;
        break;
    }
    TaskSleepForever();
}

void sub_08047e74(void)
{
    u16 *e = gUnk_0873DADE[gCurTask->unk46];

    sub_0803e55c();
    if (gCurTask->unk88->unk01 != 0)
        return;
    switch (gCurTask->unk73) {
    case 1:
        if (gCurTask->unk28 != gCurTask->unk7A)
            TaskSetEntry(sub_08047c30, gCurTaskIdx);
        /* fallthrough */
    case 0:
    case 2:
        gCurTask->unk3C = e[gCurTask->unk28];
        if (gCurTask->unk2C >= 0) {
            if (gCurTask->unk28 != 0) {
                sub_0803e5c0(gCurTask->unk88->unk00,
                             (u8 *)gUnk_0873C1C4 + gCurTask->unk2C * 8);
                sub_0803e5f8(gCurTask->unk88->unk00,
                             (s32)((u8 *)gUnk_0873CEF4 + gCurTask->unk2C * 8));
            } else {
                sub_0803e5c0(gCurTask->unk88->unk00,
                             (u8 *)gUnk_0873C1EC + gCurTask->unk2C * 8);
                sub_0803e5f8(gCurTask->unk88->unk00,
                             (s32)((u8 *)gUnk_0873CF1C + gCurTask->unk2C * 8));
            }
            sub_0801a828(gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A,
                         (u8 *)gUnk_020060E0 + gCurTask->unk88->unk00 * 20);
            sub_08030848((struct HitBoxSet *)&gUnk_02005550[gCurTask->unk88->unk00],
                         gCurTask->unk88->unk00);
        }
        gCurTask->unk28 = gCurTask->unk7A;
        break;
    case 3:
        sub_0804042c();
        break;
    }
}
