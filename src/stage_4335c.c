#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* stage_4335c.c (0x0804335C-0x08043653, issue #85).
 *
 * Per-frame player handler 20 of gUnk_0873B4A4[27], the handler table M09's
 * player task uses instead of gPlayerActionHandlers while gUnk_03001F30 is non-zero,
 * and the copy of M10's handler 20 sub_0803afcc (src/player_3aa64.c): it
 * re-picks the four-way state Task.unk73 from the latched held keys
 * gLatchedHeldKeys[] (left or right = 3, A or up = 1, down = 2, else 0; state 1
 * also looks at the newly pressed keys gLatchedPressedKeys[] and the counter
 * Task.unk28), applies the motion presets of PlayerSetMotionXPreset and PlayerSetMotionYPreset
 * in state 3, re-binds the coroutine sub_08043014 when the state changed
 * and then, unless M11's predicates sub_080400c0/PlayerCheckEnterDoor take over,
 * requests the next action through PlayerState.unk01 (9 or 5 on the ground,
 * 24 or 25 in the air).
 *
 * Matching notes (issue #85): case 1 sits in a zero-code do/while (0) (lesson
 * 3.383, as in the M10 twin) and reads the key mask inline; the tail's `m`
 * and `tp` locals are stand-ins for an address copy agbcc's gcse cannot
 * produce across the calls (lesson 3.477). */

extern u16 gLatchedPressedKeys[];   /* newly pressed keys, latched per player */
extern u16 gLatchedHeldKeys[];   /* held keys, latched per player */

void TaskSetEntry(void *a, u32 i);
s32 PlayerLand(s32 a0);
void PlayerTurnToHeldDirection(void);
void PlayerStopAtCeilingAndWall(void);
s32 sub_080400c0(void);
s32 PlayerCheckEnterDoor(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);
void PlayerSetMotionYPreset(s32 a0);
void sub_08043014(void);

/* Same shape as M10's twin sub_0803afcc (src/player_3aa64.c):
   `st = &t->unk73` for the five unk73
   stores in multi-predecessor blocks, the stores as labels at the end of the
   switch (set2/Lstore, set0, set1, set3) so the ROM's layout and
   cross-jumps come out, case 3 entering the `PlayerSetMotionXPreset(11, 3)` arm
   directly, and `L25` for the tail's out-of-line 25 store.
   * Case 1 is wrapped in `do { } while (0)` (zero code; lessons 3.383/3.412,
     exactly as in the twin): it counts case 1's references one loop level
     deeper, so the HImode key-mask value gLatchedPressedKeys[...] (3 refs over 13
     insns, 0.23) is allocated before the switch value (7 refs over 42,
     0.33) and gets r3, the switch value r4.  The key mask is read inline
     twice (cse merges it): a `u16 v` local makes the zero-extend temp an
     SImode pseudo and regmove then ANDs in place (`ands r2, r0`, 7 bytes).
   * The tail's `m` (the unk7B & 1 test cached) and `tp = &gCurTask`
     between it and the test are still stand-ins: the ROM's `adds r3, r4, #0`
     is a copy of the post-switch address pseudo placed before the branch.
     In the twin that copy is a gcse PRE insertion at the end of the block
     (anticipated in both arms), but here the address was loaded before
     three calls, and calls kill every MEM expression in agbcc's gcse, so no
     plain spelling (x/q/w locals as in the twin, all-global) produces it
     (190 bytes); tp without m places the copy before the load (16 bytes). */
void sub_0804335c(void)
{
    struct Task *t;
    struct PlayerState *u;
    u8 *st;
    s32 k;
    s32 m;
    struct Task **tp;

    PlayerTurnToHeldDirection();
    t = gCurTask;
    st = &t->unk73;
    t->unk2C = *st;
    switch (*st)
    {
    case 0:
        if (gLatchedHeldKeys[t->unk88->unk00] & 0x30)
            goto Lset3;
        if (gLatchedHeldKeys[t->unk88->unk00] & 0x41)
            goto Lset1;
        if (!(gLatchedHeldKeys[t->unk88->unk00] & 0x80))
            break;
        goto Lset2;
    case 1:
        /* zero-code do/while (0): counts case 1's references one loop level
           deeper so the key mask wins r3 over the switch value (3.383) */
        do
        {
            if ((gLatchedHeldKeys[t->unk88->unk00] & 0xC1) == 0x80)
                goto Lset2;
            if (gLatchedPressedKeys[t->unk88->unk00] & 0x30)
                goto Lset3;
            if (t->unk28 == -1)
                goto Lset0;
            if (gLatchedPressedKeys[t->unk88->unk00] & 0x41)
                t->unk28 = 1;
        } while (0);
        break;
    case 2:
        if (gLatchedHeldKeys[t->unk88->unk00] & 0x41)
            goto Lset1;
        if (gLatchedHeldKeys[t->unk88->unk00] & 0x30)
            goto Lset3;
        k = gLatchedHeldKeys[t->unk88->unk00] & 0xF0;
        if (k == 0)
            goto Lstore;
        if ((s16)t->unk88->unk14 != 0)
            t->unk88->unk14--;
        break;
    Lc3set1:
        *st = 1;
        goto Lmerge;
    Lc3set2:
        *st = 2;
        goto Lmerge;
    Lc3dec:
        u->unk14--;
        goto Lmerge;
    case 3:
        if (gLatchedHeldKeys[(u = t->unk88)->unk00] & 0x30)
            goto Lb403;
        if (gLatchedHeldKeys[u->unk00] & 0x41)
            goto Lc3set1;
        if (gLatchedHeldKeys[u->unk00] & 0x80)
            goto Lc3set2;
        if ((s16)u->unk14 != 0)
            goto Lc3dec;
        if ((gLatchedHeldKeys[u->unk00] & 0xF1) == 0 && t->unk58 >= 0)
            *st = 0;
    Lmerge:
        if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 0x30)
        {
        Lb403:
            PlayerSetMotionXPreset(11, 3);
        }
        else
        {
            PlayerSetMotionXPreset(11, 4);
        }
        if (gCurTask->unk28 != 0)
            goto Ldec28;
        if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 0x41)
            PlayerSetMotionYPreset(12);
        else
            PlayerSetMotionYPreset(13);
        break;
    Lset2:
        k = 2;
    Lstore:
        *st = k;
        break;
    Lset0:
        *st = 0;
        break;
    Lset1:
        k = 1;
        goto Lstore;
    Lset3:
        k = 3;
        goto Lstore;
    L25:
        gCurTask->unk88->unk01 = 25;
        goto L2a;
    Ldec28:
        gCurTask->unk28--;
        PlayerSetMotionYPreset(13);
        break;
    }
    if (gCurTask->unk2C != gCurTask->unk73)
        TaskSetEntry(sub_08043014, gCurTaskIdx);
    if (!sub_080400c0() && !PlayerCheckEnterDoor())
    {
        m = gCurTask->unk7B & 1;
        tp = &gCurTask;
        if (m == 0)
        {
            if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 0x40)
                gCurTask->unk88->unk01 = 9;
            else
                gCurTask->unk88->unk01 = 5;
            (*tp)->unk88->unk3D = 0;
            ((u8 *)(*tp)->unk88)[15] = 0;
        }
        else
        {
            if (gCurTask->unk73 != 1
             || !(gLatchedHeldKeys[gCurTask->unk88->unk00] & 0x41))
            {
                if (gCurTask->unk58 != 0 && (gCurTask->unk7A & 1))
                {
                    if (gCurTask->unk54 != 0)
                        goto L25;
                    gCurTask->unk88->unk01 = 24;
                L2a: ;
                }
            }
        }
    }
    PlayerStopAtCeilingAndWall();
    if (gCurTask->unk7A & 1)
        PlayerLand(0);
}
