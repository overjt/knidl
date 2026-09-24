#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* obj_30238.c (0x08030238-0x080306B3, issue #86).
 *
 * Task type #236 (class 4): sub_08030238 dispatches on Task.unk14 into the
 * six bodies of the anchor table gUnk_087328D8 (sub_080301e8 spawns it).
 * They are short sprite animations that step Task.unk3C, the frame of the
 * Task.unk38 graphics table, every one to four frames; sub_08030404 also
 * falls (Task.unk60 = -0x400) and sub_080304ec rises (Task.unk58 =
 * -0x10000). */

extern void (*gUnk_087328D8[])(void);
extern u32 gUnk_0874CD54[];
extern u32 gUnk_0874CD68[];
extern u32 gUnk_08752548[];
extern u32 gUnk_0874CDE0[];
extern u32 gUnk_0874C804[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(u32 frames);
void sub_08002e98(u32 idx, u32 count, void (**fns)(void));
void sub_080059d8(void);
void sub_08005a74(void);
void sub_08005d9c(void);
void sub_08006138(void);
void sub_080062c4(void);

void sub_08030238(void)
{
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_087328D8);
}

void sub_08030254(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk0C = (u32)sub_08005d9c;
    t->unk42 = 12;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874CD54;
    u->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_080302cc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08005d9c;
    t->unk42 = 12;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874CD68;
    sub_080062c4();
    gUnk_03002490->unk3C = 0;
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
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    v = gUnk_03002490;
    v->unk18 = 1;
    v->unk3C++;
    TaskYieldTrampoline(1);
    sub_08006138();
}

void sub_08030404(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08005d9c;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874CD68;
    sub_080062c4();
    v = gUnk_03002490;
    v->unk60 = -0x400;
    v->unk3C = 18;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_080304ec(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08005d9c;
    t->unk42 = 12;
    u = gUnk_03002490;
    u->unk38 = gUnk_08752548;
    sub_080062c4();
    v = gUnk_03002490;
    v->unk58 = -0x10000;
    v->unk3C = 0;
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
    TaskDispatchTrampoline();
}

void sub_08030580(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk0C = (u32)sub_08005d9c;
    t->unk42 = 12;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874CDE0;
    u->unk3C = 0;
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
    TaskDispatchTrampoline();
}

void sub_08030604(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_08005a74;
    t->unk0C = (u32)sub_08005d9c;
    t->unk42 = 12;
    u = gUnk_03002490;
    u->unk38 = gUnk_0874C804;
    u->unk3C = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}
