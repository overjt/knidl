#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern s16 gUnk_02004B6C[];
extern s16 gUnk_02007FA0[];
extern u16 gUnk_03002458[];
extern u8 gUnk_0873BEEC[];
extern u8 gUnk_0873CC54[];

void sub_08006148(void *func, u32 arg);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);
u16 sub_08030898(void *table, s32 id);
void sub_0803e4a8(void);
s32 sub_0803e4ec(s32 a0);
void sub_0803e650(s32 a0);
void sub_0803f9c0(void);
s32 sub_0803fce4(s32 a0);
void sub_08040b40(s32 a0, s32 a1);
void sub_080413a4(s32 a0);
void sub_0804e0e0(void);
s32 sub_08065100(s16 x, s16 y, u32 p2, u8 p3, u8 p4);

void sub_0804e3a0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct PlayerState *p;
    register struct PlayerState *q asm("r3");
    struct PlayerState *r;
    register u16 *table asm("r1");
    register s32 index asm("r0");
    register u32 flags asm("r1");
    register u32 two asm("r3");

    if (sub_0803fce4(0) != 0)
    {
        sub_0803e4a8();
        if ((s8)gUnk_03002490->unk88->unk07 != 0)
            gUnk_03002490->unk88->unk16 = 0xFF;
        gUnk_03002490->unk88->unk01 = 23;
    }
    else
    {
        t = gUnk_03002490;
        switch (t->unk73)
        {
        case 0:
            break;
        case 1:
            p = t->unk88;
            if (p->unk09 == 0)
            {
                if (t->unk28 == 0)
                {
                    if (sub_08030898(gUnk_0873CC54, p->unk00) != 0)
                    {
                        /* Preserve the full-width call shape used at this site. */
                        ((s32 (*)(s32, s32, u32, u8, u8))sub_08065100)(
                            gUnk_02007FA0[0] + 8,
                            gUnk_02004B6C[0] + 8,
                            gCurTaskIdx, 4, 2);
                        gUnk_03002490->unk88->unk09 = 2;
                    }
                }
                else
                {
                    t->unk28--;
                }

                u = gUnk_03002490;
                if (u->unk88->unk09 == 0)
                {
                    sub_0801a828((u8)gCurTaskIdx, u->unk48, u->unk4A,
                                 gUnk_0873BEEC);
                }
            }

            v = gUnk_03002490;
            q = v->unk88;
            if ((s8)q->unk07 == 0)
            {
                if (v->unk2C == 0)
                {
                    table = gUnk_03002458;
                    index = q->unk00;
                    flags = table[index];
                    two = 2;
                    if ((flags & 2) == 0)
                    {
                        v->unk73 = two;
                        sub_08006148(sub_0804e0e0, gCurTaskIdx);
                    }
                }
                else
                {
                    v->unk2C--;
                }
            }
            else if (v->unk30 == 0)
            {
                sub_0803e650(1);
                gUnk_03002490->unk30++;
            }
            break;
        case 2:
            break;
        case 3:
            r = t->unk88;
            if ((s8)r->unk07 != 0)
                goto Lcase3_54;
            if (t->unk7A & 1)
                goto Lcase3_1;
            r->unk01 = 7;
            break;
        }
    }

Lcommon:
    w = gUnk_03002490;
    if ((w->unk7A & 1) == 0)
        goto Lno_flag;
    if (w->unk54 == 0)
        goto Lflag_done;
    if ((w->unk7B & 1) == 0)
        goto Lflag_zero;
    sub_08040b40(8, 72);
    goto Lflag_done;

Lcase3_1:
    r->unk01 = 1;
    goto Lcommon;
Lcase3_54:
    r->unk01 = 54;
    goto Lcommon;
Lflag_zero:
    sub_08040b40(0, 72);
Lflag_done:
    sub_0803e4ec(1);
    goto Ldone;

Lno_flag:
    if ((w->unk7B & 1) == 0)
    {
        sub_080413a4(2);
        sub_08040b40(11, 2);
    }
    else
    {
        sub_080413a4(13);
        sub_08040b40(11, 4);
    }
Ldone:
    sub_0803f9c0();
}
