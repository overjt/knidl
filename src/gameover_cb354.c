#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* gameover_cb354.c (0x080CB354-0x080CB64B, issue #100).
 *
 * The game-over screen's object task types, all class 4.
 *   sub_080cb354   #260, a still sprite picked by Task.unk18.
 *   sub_080cb3a8   #261, the cursor, its frame following gUnk_02006164.
 *   sub_080cb418   #262, a palette effect: fade the blend in, then cycle four
 *       colours between two rows of gUnk_08584BB0.
 *   sub_080cb4f4   #263, halve this player's score (rounded down to a
 *       multiple of ten) and count the displayed score down to it.
 *   sub_080cb588   #264: six variants gUnk_08758294[Task.unk73]; variant 0
 *       (sub_080cb5c4, the player character) is a small state machine of
 *       sub-states gUnk_087582AC[Task.unk14] and per-frame handlers
 *       gUnk_087582B8[Task.unk15] (sub_080cb610), re-entered through
 *       sub_080cb62c. */

extern u8 gUnk_03001F30;            /* link-play mode */
extern u16 gUnk_03002360;           /* this player's index */
extern s32 gUnk_02006020[];         /* score per player */
extern s8 gUnk_02006164;            /* game-over screen: cursor (continue = 0?) */
extern u32 gUnk_087556E0[];
extern u16 gUnk_08758274[];
extern u16 gUnk_08758284[];
extern vu8 gUnk_0300118C;
extern vu8 gUnk_03000040;
extern vu8 gUnk_03000B08;
extern vu8 gUnk_03001EAC;
extern u16 gUnk_08584BB0[][4];
extern u16 gUnk_03001390[];
extern void (*gUnk_08758294[])(void);
extern u32 gUnk_08754914[];
extern void (*gUnk_087582AC[])(void);
extern void (*gUnk_087582B8[])(void);

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_08002e98(u32 idx, u32 count, void (**fns)(void));   /* if (idx < count) fns[idx](); */
void sub_08003014(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void sub_080059d8(void);
void sub_08005ca0(void);
void sub_08006138(void);                                     /* end the running task */
void sub_080c68b0(s32 v, s32 x, s32 y);
void sub_080cb610(void);

/* Task type #260 (class 4): a still sprite, frame and position picked by
   Task.unk18. */
void sub_080cb354(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk42 = 9;
    gUnk_03002490->unk38 = gUnk_087556E0;
    gUnk_03002490->unk3C = gUnk_03002490->unk18;
    gUnk_03002490->unk4C = gUnk_08758274[gUnk_03002490->unk18] << 16;
    gUnk_03002490->unk50 = gUnk_08758284[gUnk_03002490->unk18] << 16;
    sub_08006138();
}

/* Task type #261 (class 4): the cursor; its frame follows gUnk_02006164. */
void sub_080cb3a8(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk42 = 8;
    gUnk_03002490->unk38 = gUnk_087556E0;
    if (gUnk_03001F30 == 0) {
        gUnk_03002490->unk4C = 184 << 16;
        gUnk_03002490->unk50 = 94 << 16;
    } else {
        gUnk_03002490->unk4C = 184 << 16;
        gUnk_03002490->unk50 = 96 << 16;
    }
    for (;;) {
        gUnk_03002490->unk3C = gUnk_02006164 + 8;
        TaskYieldTrampoline(1);
    }
}

/* Task type #262 (class 4): fade the blend in over 32 frames, then cycle
   four colours between two palettes of gUnk_08584BB0. */
void sub_080cb418(void)
{
    gUnk_03002490->unk28 = 0;
    gUnk_03002490->unk2C = 0;
    gUnk_03002490->unk30 = 1;
    gUnk_03002490->unk34 = 0;
    gUnk_0300118C = 66;
    gUnk_03000040 = 12;
    gUnk_03000B08 = 0;
    gUnk_03001EAC = 16;
    for (;;) {
        if (gUnk_03002490->unk28 < 32) {
            gUnk_03002490->unk28++;
            if (gUnk_03002490->unk28 == 32) {
                gUnk_0300118C = gUnk_03000040 = gUnk_03000B08 = gUnk_03001EAC = 0;
            } else {
                gUnk_03000B08 = gUnk_03002490->unk28 >> 1;
                gUnk_03001EAC = 16 - gUnk_03000B08;
            }
        }
        if (gUnk_03002490->unk34 == 256) {
            gUnk_03002490->unk2C ^= 1;
            gUnk_03002490->unk30 ^= 1;
            gUnk_03002490->unk34 = 0;
        }
        gUnk_03002490->unk34 += 4;
        if (gUnk_03002490->unk34 > 256)
            gUnk_03002490->unk34 = 256;
        sub_08003014(gUnk_08584BB0[gUnk_03002490->unk2C], gUnk_08584BB0[gUnk_03002490->unk30],
            (u16)gUnk_03002490->unk34, 4, gUnk_03001390);
        TaskYieldTrampoline(1);
    }
}

/* Task type #263 (class 4): halve this player's score, round it down to a
   multiple of 10 and count the displayed score down to it. */
void sub_080cb4f4(void)
{
    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk0C = 0;
    gUnk_03002490->unk28 = gUnk_02006020[gUnk_03002360];
    gUnk_02006020[gUnk_03002360] >>= 1;
    gUnk_03002490->unk2C = Mod(gUnk_02006020[gUnk_03002360], 10);
    gUnk_02006020[gUnk_03002360] -= gUnk_03002490->unk2C;
    while (gUnk_03002490->unk28 != gUnk_02006020[gUnk_03002360]) {
        gUnk_03002490->unk28 -= 10;
        sub_080c68b0(gUnk_03002490->unk28, 22, 18);
        TaskYieldTrampoline(1);
    }
    TaskDispatchTrampoline();
}

/* Task type #264 (class 4): six variants, gUnk_08758294[Task.unk73]. */
void sub_080cb588(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk40 = 0x4800;
    sub_08002e98(gUnk_03002490->unk73, 6, gUnk_08758294);
    sub_08006138();
}

/* Task type #264 variant 0: sub-states gUnk_087582AC[Task.unk14], per-frame
   handlers gUnk_087582B8[Task.unk15] (sub_080cb610). */
void sub_080cb5c4(void)
{
    gUnk_03002490->unk04 = (u32)sub_080cb610;
    gUnk_03002490->unk42 = 8;
    gUnk_03002490->unk38 = gUnk_08754914;
    gUnk_03002490->unk24 = 0;
    gUnk_03002490->unk18 = 0;
    gUnk_03002490->unk43 = 1;
    gUnk_03002490->unk14 = 0;
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087582AC);
    sub_08006138();
}

/* Task type #264 variant 0's per-frame hook: handler gUnk_087582B8[Task.unk15]. */
void sub_080cb610(void)
{
    sub_08002e98(gUnk_03002490->unk15, 3, gUnk_087582B8);
}

/* Re-enter task type #264 variant 0 (sub_08006148 installs this as its
   body): Task.unk24 = 1, then sub-state gUnk_087582AC[Task.unk14]. */
void sub_080cb62c(void)
{
    gUnk_03002490->unk24 = 1;
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087582AC);
}
