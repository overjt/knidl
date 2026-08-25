#include "gba/gba.h"
#include "global.h"

/* Link-cable session drivers, 0x08002378-0x0800293C (issue #32, batch C1).
 *
 * Two sibling blocking loops that drive the multiplayer handshake state
 * machine in gUnk_03005274 while pumping the link layer once per frame:
 * sub_080022d0 (frame wait), sub_08006914(&gUnk_03004D88, gUnk_03004D90,
 * gUnk_03004D50) (SIO transfer step), sub_080072e0 (abort poll) and
 * sub_08008b8c (error/reset path, taken after 31 frames without the counter
 * at gUnk_03004D7C moving).
 *
 *  - sub_08002378 / sub_08002668 run the 0x7700-series handshake.  They are
 *    the same function except for where the 4-halfword payload lives: 0x2378
 *    packs it into gUnk_03000FB4 (u32, read twice per use => volatile) plus
 *    gUnk_03001EA4, 0x2668 mirrors it to/from the record at gUnk_02006068.
 *    (sub_0800293c, the 0xAA00-series teardown twin, lives in src/early_293c.c.)
 *
 * gUnk_03004D50 is the 4x4 halfword mailbox shared with sub_08006914: row 0
 * = per-player state word, row 1 = per-player payload byte, rows 2/3 = extra
 * payload halfwords.  gUnk_03004D90 is this console's outgoing 4-halfword
 * record, gUnk_0300243C the player count, gUnk_030023A8[] the per-player
 * bytes and gUnk_0300244C their negotiated minimum.
 *
 * Matching notes (agbcc -O2 -mthumb-interwork, docs/lessons-learned.md):
 *  - Switch CASE ORDER is SOURCE order (lesson 3.14): the bodies must be
 *    written 0x7755, 0xBB00-04, 0x7700, 0x7701, 0x7703-05, 0x7702/0x7706 to
 *    reproduce the ROM body VMAs.  The empty `case 0x9900:` (outer switch)
 *    and the empty `case 0x7755:` + `case 0x9900:` (inner switch) emit no
 *    code but are REQUIRED (lesson 3.42): balance_case_nodes bisects the
 *    case list by cost, so dropping them moves the decision tree root.
 *  - The trailing clamp loop is a `do {...} while (...)` whose *body* holds
 *    the `clamp:` label the 0x7706 handler jumps to.  Putting the label
 *    INSIDE the loop gives it two entry points, so loop.c prints "Loop at N
 *    ignored due to multiple entry points" and runs neither strength
 *    reduction nor invariant hoisting on it - which is exactly what the ROM
 *    shows (`adds r2,r4,r5` recomputed every iteration, `movs #1; negs`
 *    materialised inside the body).  The loop NOTES still exist, so flow.c
 *    still weights the loop's references at depth 2, which is what keeps `i`
 *    ahead of the &gUnk_03004D50 base in global-alloc priority (r4 vs r5).
 *    A plain `goto`-style trailing loop (lesson 3.21) kills the notes too
 *    and swaps those two registers.
 *  - `arr` must be hoisted by hand: the invalid loop cannot hoist
 *    `&gUnk_030023A8` into its own preheader, and the ROM has it there.
 *  - The index must be written `*(s8 *)(i + (int)arr)`, not `arr[i]`: C's
 *    pointer arithmetic always builds `ptr + int`, so `arr[i]` emits
 *    `adds r2,r5,r4` while the ROM has `adds r2,r4,r5` (lesson 3.32).
 *
 * STATUS: sub_08002668 byte-matches (724/724).  sub_08002378 does NOT
 * (732 vs 752).  It is instruction-for-instruction identical to the ROM
 * except for ONE allocator decision, pinned with `agbcc -da` (lesson 3.34):
 *   - ROM's i-loop preheader hoists `&gUnk_0300244C` into sl (pool word
 *     0x08002524) and uses it for both the `== -1` test (`mov r1,sl`) and
 *     the `= 1` store (`mov r2,sl`); the 0x7700 constant is then NOT in a
 *     register, so all four `gUnk_0300244C`-series stores re-materialise it
 *     (`movs #238; lsls #7`) and the outer dispatch derives it with
 *     `subs r0,#1` off the pooled 0x7705.
 *   - We hoist the 0x7700 constant instead: `.greg` ranks that pseudo at
 *     floor_log2(14)*14/396 = 1060, above `a` (14/408 = 1029) and far above
 *     the &gUnk_0300244C pseudo (5/116 = 862), so it takes sl.
 *   Root cause (verified, not guessed): in OUR RTL the &gUnk_0300244C
 *   address is a single pseudo (cse1 merges the read's and the write's pool
 *   loads - see `x.rtl` insns 632/647 collapsing to insn 632 in `x.cse`),
 *   and that pseudo is live across the `bne`, so loop.c's scan_loop rejects
 *   it outright (`! reg_in_basic_block_p && maybe_never`) and it never even
 *   appears in the `.loop` movable list.  Removing the store makes it a
 *   movable but only `life 1, savings 1` - `threshold*savings*lifetime`
 *   (16*1*1) is nowhere near the loop's `insn_count` of 98, so loop.c can
 *   never hoist it either.  The ROM's preheader load must therefore come
 *   from GCSE's PRE (our gcse inserts the same expression one block later,
 *   at the top of the 0x7706 handler, where it becomes the clamp loop's
 *   `ldr r3`).  Whatever source shape moves that PRE insertion up to the
 *   i-loop preheader is the remaining unknown; ~40 shapes were tried
 *   (pointer locals for the read/write/clamp in every combination, inverted
 *   conditions, volatile casts, statement reordering, and dropping the
 *   clamp's and the outer 0x7755 case's uses) and none moved it.
 */

extern void sub_080022d0(void);
extern void sub_08006914(void *, void *, void *);
extern int sub_080072e0(void);
extern void sub_08008b8c(void);

extern u16 gUnk_02006068[4];
extern vu32 gUnk_03000FB4;
extern u16 gUnk_03001EA4;
extern u32 gUnk_03001EFC;
extern u16 gUnk_03001F38;
extern s8 gUnk_030023A8[];
extern u16 gUnk_03002360;
extern u16 gUnk_0300243C;
extern s16 gUnk_0300244C;
extern u16 gUnk_03004D50[4][4];
extern u32 gUnk_03004D7C;
extern u16 gUnk_03004D88;
extern u16 gUnk_03004D90[4];
extern u16 gUnk_03005274;

void sub_08002668(void)
{
    int a;
    u32 b;
    int c;
    int i;
    u32 old;
    s8 *arr;

    if (gUnk_0300243C <= 1)
        return;

    a = 0;
    b = 0;
    c = 0;
    for (;;) {
        switch (gUnk_03005274) {
        case 0x7755:
            gUnk_03004D90[0] = 0x7755;
            if (gUnk_0300244C == -1)
                gUnk_03005274 = 0xBB00;
            else
                gUnk_03005274 = 0x7700;
            break;
        case 0xBB00:
        case 0xBB01:
        case 0xBB02:
        case 0xBB03:
        case 0xBB04:
            gUnk_03004D90[0] = 0xBB00;
            gUnk_03004D90[1] = 1;
            if (gUnk_03002360 == 0) {
                gUnk_03005274++;
                if (gUnk_03005274 > 0xBB04)
                    gUnk_03005274 = 0x7700;
            } else {
                gUnk_03005274 = 0x9900;
            }
            break;
        case 0x7700:
            c = 30;
            gUnk_03004D90[0] = 0x7700;
            gUnk_03005274 = 0x9900;
            break;
        case 0x7701:
            gUnk_03004D90[0] = 0x7701;
            gUnk_03005274 = 0x9900;
            break;
        case 0x7703:
        case 0x7704:
        case 0x7705:
            gUnk_03005274 = gUnk_03005274 + 1;
            break;
        case 0x7702:
        case 0x7706:
            gUnk_03004D90[0] = 0x7706;
            gUnk_03004D90[1] = gUnk_02006068[1];
            gUnk_03004D90[2] = gUnk_02006068[2];
            gUnk_03004D90[3] = gUnk_02006068[3];
            gUnk_03005274 = 0x9900;
            break;
        case 0x9900:
            break;
        }
        old = gUnk_03004D7C;
        sub_080022d0();
        sub_08006914(&gUnk_03004D88, gUnk_03004D90, gUnk_03004D50);
        if (sub_080072e0() != 0)
            sub_08008b8c();
        if (old == gUnk_03004D7C) {
            if (++b > 30)
                sub_08008b8c();
        }
        for (i = 0; i < 4; i++) {
            switch (gUnk_03004D50[0][i]) {
            case 0xBB00:
                gUnk_030023A8[i] = gUnk_03004D50[1][i];
                if (gUnk_03002360 != 0) {
                    if (i == 0)
                        gUnk_03005274 = 0xBB00;
                }
                break;
            case 0x7700:
                gUnk_03005274 = 0x7701;
                break;
            case 0x7701:
                if (gUnk_03001F38 != 0) {
                    if (++a >= gUnk_0300243C)
                        gUnk_03005274 = 0x7702;
                }
                break;
            case 0x7706:
                gUnk_02006068[1] = gUnk_03004D50[1][0];
                gUnk_02006068[2] = gUnk_03004D50[2][0];
                gUnk_02006068[3] = gUnk_03004D50[3][0];
                gUnk_03001EFC = 0;
                if (gUnk_0300244C == -1) {
                    gUnk_0300244C = 1;
                    i = 0;
                    if (i < gUnk_0300243C) {
                        arr = gUnk_030023A8;
                        goto clamp;
                    }
                }
                return;
            case 0x7755:
                break;
            case 0x9900:
                break;
            }
        }
        if (c != 0) {
            if (a == gUnk_0300243C)
                c = 0;
            else if (--c == 0) {
                gUnk_03005274 = 0x7700;
                a = 0;
            }
        }
    }
    do {
    clamp:
        if ((*(s8 *)(i + (int)arr)) == -1)
            (*(s8 *)(i + (int)arr)) = 0;
        if ((*(s8 *)(i + (int)arr)) < gUnk_0300244C)
            gUnk_0300244C = (*(s8 *)(i + (int)arr));
        i++;
    } while (i < gUnk_0300243C);
}
