#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* plobj_507bc.c (0x080507BC-0x080509EB, issue #90).
 *
 * Task type #6 (the objects the player's actions spawn through
 * sub_08053940/sub_08053a44): the body and two shared callbacks.  The body
 * sub_080507bc links the task to its spawner (Task.unk8C =
 * &gUnk_03002790[Task.unk44], the first time only) and dispatches the
 * variant, the top byte of Task.unk18, through the 13 variant bodies
 * gUnk_0873B77C.  sub_08050814 is the common exit most variants re-bind on
 * contact: it installs the sprite (sub_08005ca0 for PlayerState.unk37 == 2,
 * sub_08005f30 otherwise, animation table gUnk_0874C650), registers the
 * collider row Task.unk24 if there is one and steps animation frames
 * 0-10 before TaskDispatchTrampoline.  sub_0805091c is a second, drifting burst
 * (gUnk_0874C7CC). */

extern void (*gUnk_0873B77C[])(void);   /* task type #6's 13 variants, indexed by Task.unk18 >> 24 */
extern u32 gUnk_0874C650[];
extern u32 gUnk_0874C7CC[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
/* task / sprite services (landed prototypes) */
void sub_08002e98(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
void sub_080059d8(void);
void sub_08005ca0(void);
void sub_08005f30(void);
void sub_0800617c(s16 a);
void sub_080062c4(void);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */

void sub_080507bc(void)
{
    if (gUnk_03002490->unk8C == NULL)
    {
        gUnk_03002490->unk80 = 0;
        gUnk_03002490->unk8C = (struct Actor *)&gUnk_03002790[gUnk_03002490->unk44];
        gUnk_03002490->unk78 = 1;
        gUnk_03002490->unk24 = 0;
    }
    sub_08002e98(((u8 *)gUnk_03002490)[27], 13, gUnk_0873B77C);
}

void sub_08050814(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = 0;
    if (t->unk88->unk37 != 2)
        t->unk0C = (u32)sub_08005f30;
    else
        t->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk04 = 0;
    gUnk_03002490->unk08 = 0;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C650;
    t->unk3E = 0;
    t->unk40 = 0;
    if (t->unk24 != 0)
        sub_0801a828(gCurTaskIdx, t->unk48, t->unk4A, (void *)t->unk24);
    sub_080062c4();
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 6;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 2;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 7;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 3;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 8;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 4;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 9;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 5;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C = 10;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_0805091c(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005f30;
    gUnk_03002490->unk04 = 0;
    gUnk_03002490->unk08 = 0;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_0874C7CC;
    t->unk3E = 0;
    t->unk40 = 0;
    t->unk28 = 0;
    if (t->unk58 < 0)
        t->unk28 = 1;
    sub_080062c4();
    if (gUnk_03002490->unk28 == 1)
        gUnk_03002490->unk58 = 0x8000;
    else
        gUnk_03002490->unk58 = -0x8000;
    sub_0800617c(0);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}
