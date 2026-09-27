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
s32 sub_08065100(s32 x, s32 y, u32 p2, u8 p3, u8 p4);

void sub_0804e3a0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct PlayerState *p;
    struct PlayerState *r;

    while (1)
    {
        if (sub_0803fce4(0) != 0)
        {
            sub_0803e4a8();
            if ((s8)gUnk_03002490->unk88->unk07 != 0)
                gUnk_03002490->unk88->unk16 = 0xFF;
            gUnk_03002490->unk88->unk01 = 23;
            break;
        }
        t = gUnk_03002490;
        switch (t->unk73)
        {
        case 1:
            p = t->unk88;
            if (p->unk09 == 0)
            {
                if (t->unk28 == 0)
                {
                    if (sub_08030898(gUnk_0873CC54, p->unk00) != 0)
                    {
                        sub_08065100(gUnk_02007FA0[0] + 8, gUnk_02004B6C[0] + 8, gCurTaskIdx, 4, 2);
                        gUnk_03002490->unk88->unk09 = 2;
                    }
                }
                else
                {
                    t->unk28--;
                }
                u = gUnk_03002490;
                if (u->unk88->unk09 == 0)
                    sub_0801a828(gCurTaskIdx, u->unk48, u->unk4A, gUnk_0873BEEC);
            }
            v = gUnk_03002490;
            if ((s8)v->unk88->unk07 == 0)
            {
                if (v->unk2C == 0)
                {
                    if (!(gUnk_03002458[v->unk88->unk00] & 2))
                    {
                        v->unk73 = 2;
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
        case 0:
        case 2:
            break;
        case 3:
            r = t->unk88;
            if ((s8)r->unk07 != 0)
                r->unk01 = 54;
            else if (t->unk7A & 1)
                r->unk01 = 1;
            else
                r->unk01 = 7;
            break;
        }
        break;
    }
    w = gUnk_03002490;
    if (w->unk7A & 1)
    {
        if (w->unk54 != 0)
        {
            if (w->unk7B & 1)
                sub_08040b40(8, 72);
            else
                sub_08040b40(0, 72);
        }
        sub_0803e4ec(1);
    }
    else if (!(w->unk7B & 1))
    {
        sub_080413a4(2);
        sub_08040b40(11, 2);
    }
    else
    {
        sub_080413a4(13);
        sub_08040b40(11, 4);
    }
    sub_0803f9c0();
}
