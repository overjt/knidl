#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_4e5a4.c (0x0804E5A4-0x0804E78B, issue #90).
 *
 * Player action bodies, part 25: action 49, its re-entry callback and
 * per-frame handler 46.  Action 49 is a move set one level down: its enter
 * body sub_0804e5a4 (mode 13, Task.unk80 = 18) clears Task.unk73/unk74/
 * unk24 and PlayerState.unk3E and dispatches Task.unk73 through its own
 * table of nine sub-actions gUnk_0873B664; sub_0804e600, the callback the
 * sub-handlers re-bind, does the same after clearing PlayerState.unk3D.
 * Its handler sub_0804e640 plays the animation gUnk_0873DB0A[Task.unk46]
 * mirrored by Task.unk6E, registers the body collider (gUnk_0873C28C, and
 * the block hit-box set gUnk_0873CF7C while moving) when the vertical
 * speed exceeds 2 pixels a frame, requests action 23 through M11's
 * sub_0803fce4 and otherwise runs the sub-handler Task.unk73 of
 * gUnk_0873B688. */

extern void (*gUnk_0873B664[])(void);   /* enter 49's sub-actions [9], indexed by Task.unk73 */
extern void (*gUnk_0873B688[])(void);   /* handler 46's per-frame sub-handlers [9] */
extern u16 gUnk_0873DB0A[];
extern u32 gUnk_0873CF7C[];
extern u32 gUnk_0873C28C[];

/* task / sprite services (landed prototypes) */
void sub_08002e98(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
void sub_0800634c(s32 a);
void sub_08006364(s32 a);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void sub_0803e1b8(s32 a0, s32 a1, s32 a2);
s32 sub_0803fce4(s32 a);

void sub_0804e5a4(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 46;
    gUnk_03002490->unk74 = 0;
    gUnk_03002490->unk88->unk3E = 0;
    gUnk_03002490->unk24 = 0;
    gUnk_03002490->unk73 = 0;
    gUnk_03002490->unk80 = 18;
    sub_08002e98(gUnk_03002490->unk73, 9, gUnk_0873B664);
}

void sub_0804e600(void)
{
    gUnk_03002490->unk88->unk3D = 0;
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    sub_08002e98(gUnk_03002490->unk73, 9, gUnk_0873B664);
}

void sub_0804e640(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk46 != -1)
    {
        if (t->unk24 == 0)
        {
            if (t->unk6E == 1)
                sub_0800634c((s16)gUnk_0873DB0A[t->unk46]);
            else
                sub_08006364(gUnk_0873DB0A[t->unk46]);
        }
        if (gUnk_03002490->unk46 == 9)
            gUnk_03002490->unk6E = gUnk_03002490->unk43;
    }
    if (abs(gUnk_03002490->unk58) > 0x20000)
    {
        sub_0803e1b8(3, 0, gUnk_03002490->unk88->unk00);
        if ((u32)abs(gUnk_03002490->unk54) > 0x8000)
            gUnk_03002490->unk88->unk6C = gUnk_0873CF7C;
        else
            gUnk_03002490->unk88->unk6C = 0;
        sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A, gUnk_0873C28C);
    }
    else
    {
        gUnk_03002490->unk88->unk6C = 0;
        sub_0803e1b8(0xFF, 0, gUnk_03002490->unk88->unk00);
    }
    if (sub_0803fce4(0) != 0)
        gUnk_03002490->unk88->unk01 = 23;
    else
        sub_08002e98(gUnk_03002490->unk73, 9, gUnk_0873B688);
}
