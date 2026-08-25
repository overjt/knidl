#include "gba/gba.h"
#include "global.h"

/* Early subsystem: view-bounds test + task skip-mask stack + the SIO
 * MULTI-PLAY link driver (0x08006464-0x08006D17, issue #32 batch G2).
 *
 * Recipe: agbcc -O2 -mthumb-interwork -fprologue-bugfix (fnmatch --newpb).
 * Evidence: the whole range compiles byte-identically under --newpb; under
 * plain agbcc the branching leaves sub_08006a48 / sub_08006cd4 pick up a
 * spurious `push {lr}` (the far_jump_used_p cache bug, lessons 3.75-3.79).
 * Two functions here (sub_080064dc, sub_08006a48) each pin the recipe from
 * one side, so this range is a good regression pair for that flag.
 *
 * ROM order / semantics:
 *   sub_08006464  in-view test: 1 when (x,y) is inside the camera rect
 *                 gUnk_03002158[0..3] widened by 64 px on every side.
 *   sub_080064ac  base + ((rand(256) * amount) >> 8) * scale   (u16)
 *   sub_080064dc  same, signed by the running task's facing byte
 *                 (gUnk_03002490->b43 == 1 -> +, else -).
 *   sub_0800652c  push/pop the per-task "phase skip mask" (Task.b13, see
 *                 pending/early_4fec.c): val==0 pops the snapshot from
 *                 gUnk_0200D090[--gUnk_0200D110], val!=0 pushes one and then
 *                 ORs val into every ALLOCATED task's mask (free slots, i.e.
 *                 gUnk_03004CA0[i] == -1, get 0).  One task id is exempt.
 *   sub_0800663c  restore one task's mask from the current snapshot  (HIDDEN)
 *   sub_08006664  save one task's mask into the current snapshot     (HIDDEN)
 *   sub_0800668c  cold link init (clear the session block, install the SIO
 *                 and timer-3 IRQ handlers at gUnk_030004B0[0]/[1]) (HIDDEN)
 *   sub_08006724  start a MULTI-PLAY session: RCNT=0, SIOCNT=0x2000|0x4003
 *                 (multi-play, 115200 bd, IRQ), enable IE bit 7 (serial),
 *                 clear the session block and the key mirrors.
 *   sub_08006868  stop the session: mask IE bits 6/7, stop timer 3, ack
 *                 IF 0xC0, clear the session block.
 *   sub_08006904  stop + start (HIDDEN)
 *   sub_08006914  the per-frame link driver called from src/early_2b04.c;
 *                 5-state machine on gUnk_03004DA0[1], then packs the link
 *                 status word into gUnk_03004D70.
 *   sub_08006a48  refresh the connection state from SIOCNT bits 2-3
 *                 (8 = "all players ready" and we are the parent).
 *   sub_08006a70  arm timer 3 (0xFF7C, /1024 + IRQ) and enable IE bit 6.
 *   sub_08006ac8  queue one 4-halfword send frame into the send ring
 *                 (gUnk_03004DA0+28, u16[4][30]) and clear the caller's
 *                 buffer.  Overflow (>=30 pending) sets flag byte [20].
 *   sub_08006bb4  dequeue one 4x4 receive frame out of the receive ring
 *                 (gUnk_03004DA0+0x110, u16[4][4][30]); when nothing is
 *                 pending the caller's matrix is zeroed and [12] is set.
 *   sub_08006cd4  timeout tick (called from the timer-3 IRQ path).
 *
 * symbols.csv hides FOUR unreferenced/extra functions in this range
 * (lesson 2.13 / zone lesson 14): sub_08006664 and sub_0800668c inside the
 * 0xE8 recorded for sub_0800663c, and sub_08006904 inside the 0xAC recorded
 * for sub_08006868.  The assigned RANGE is right; only the sizes are wrong.
 *
 * Matching notes (docs/lessons-learned.md 3.x):
 *  - sub_080064ac/sub_080064dc: computing the sum in a `u32` local is what
 *    keeps both `|`-operands SImode registers so the source operand order
 *    survives.  Writing `return base + product;` directly narrows the PLUS
 *    to HImode (convert_to_integer's "shorten"), which turns the product
 *    into `(subreg:SI (reg:HI))`, swaps the operands and shifts the whole
 *    function's allocation by one register.  (New; see the batch report.)
 *  - sub_0800652c's free-slot test must be `!= -1` with the OR branch first:
 *    the reversed spelling flips every branch in the loop.
 *  - sub_08006724: the CpuSet fill source must be `vu32` (3.48) so its zero
 *    is not CSE'd with the plain zero stores; the 4x4 clear loop needs the
 *    row offset in its own local (`k = i * 8;`) so `j = 3` lands between the
 *    shift and the base add, and the outer loop must be the explicit
 *    `n = i + 1; ...; i = n;` do/while shape.
 *  - sub_08006868/a70/ac8/bb4 restore REG_IME by RE-READING the shadow
 *    gUnk_03004D44, not from a local (a local adds a `lsls/lsrs` pair).
 *  - sub_08006914's status word is ONE assignment expression with the two
 *    conditions as ternaries: that is what materialises the destination
 *    address into r8 first (expand_assignment does the LHS first, 3.60) and
 *    what makes gcc evaluate all six shifted bytes before the branch.
 *    Splitting it into `v = ...; if (...) v |= ...; gUnk_03004D70 = v;`
 *    loads the fields lazily and drops the r8 push.
 *
 * STATUS: 14 of the 16 functions are byte-exact.  sub_08006ac8 is 8 bytes
 * off (the hoisted `movs r6,#0` sits after the induction-variable init
 * instead of before it) and sub_08006bb4 is 140 bytes off (same size, same
 * instruction sequence: the i/j loop counters land in r4/r3 instead of the
 * ROM's r3/r4 and the inner bound stays in r5 instead of spilling to
 * [sp,#4]).  See the batch report.
 */

struct Task {
    /* 0x00 */ void (*f00)(void);
    /* 0x04 */ void (*f04)(void);
    /* 0x08 */ void (*f08)(void);
    /* 0x0C */ void (*f0C)(void);
    /* 0x10 */ s16 h10;
    /* 0x12 */ u8  b12;
    /* 0x13 */ u8  b13;
    /* 0x14 */ u8  pad14[0x7C];
};

extern struct Task gUnk_03002790[];
extern vu16 gUnk_03004CA0[];
extern u8  gUnk_0200D090[2][64];
extern u8  gUnk_0200D110;
extern s16 gUnk_03002158[];
extern s8 *gUnk_03002490;
extern u8  gUnk_03004DA0[];
extern vu16 gUnk_03001EF8;
extern vu16 gUnk_03000018;
extern u16 gUnk_03002360;
extern u16 gUnk_03001F38;
extern vu16 gUnk_0300243C;
extern u16 gUnk_030023AC;
extern void (*gUnk_030004B0[])(void);
extern u32 gUnk_0200EBA0;
extern u32 gUnk_03004D7C;
extern u32 gUnk_03004D74;
extern u32 gUnk_03004D70;
extern u32 gUnk_03004D30;
extern u32 gUnk_03004D28;
extern u32 gUnk_03004D2C;
extern u32 gUnk_03004D24;
extern u16 gUnk_03004D80;
extern u16 gUnk_03004D84;
extern u16 gUnk_03004D44;
extern u8  gUnk_03004D40;
extern u8  gUnk_03004D20;
extern u8  gUnk_03004D34;
extern u8  gUnk_03005278;
extern u16 gUnk_03004D90[4];
extern u16 gUnk_03004D50[4][4];

extern u16 gUnk_03004D84;
extern u8  gUnk_03005270;
void sub_08006e8c(void);
u32 sub_08002ee8(u32 range);
void sub_08006d28(void);
void sub_08006d18(void);
void sub_08007124(void);
void sub_08007174(void);
void sub_08006724(void);
void sub_08006868(void);
void sub_08006a48(void);
void sub_08006a70(void);
void sub_08006ac8(u16 *p);
void sub_08006bb4(u16 (*p)[4]);
















void sub_08006cd4(void)
{
    if (gUnk_03004DA0[0] == 0)
        return;
    switch (gUnk_03004DA0[1]) {
    case 4:
        if ((s8)gUnk_03004DA0[13] <= 4) {
            if (gUnk_03004DA0[18] != 1)
                gUnk_03004DA0[21] = 1;
        } else {
            gUnk_03004DA0[13] = 0;
            sub_08006e8c();
        }
        break;
    case 2:
        gUnk_03004DA0[13] = 4;
        sub_08006e8c();
        break;
    }
}
