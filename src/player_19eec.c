#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_19eec.c (0x08019EEC-0x0801A07B, issue #125).
 *
 * Animation script 62, the last entry (70) of gUnk_08731FA8: a timed blend
 * sequence.  It turns alpha blending on (the BLDCNT shadows gUnk_0300118C and
 * gUnk_03000040), holds for 272 frames, cross-fades the BLDALPHA shadows
 * (gUnk_03000B08 down, gUnk_03001EAC up) over 64 frames, holds for 798 more,
 * then blends the 128 colours of the palette buffer gUnk_03001370 from
 * gUnk_085E2920 to gUnk_085E2A20, back, and from gUnk_085E2920 to
 * gUnk_085E2B20, 16 frames each (the same three palettes M38's ending uses,
 * src/ending_c9004.c), and ends the task.
 *
 * Matching note (issue #125): the four blend shadows are vu8, as everywhere
 * else in src/; the ROM re-reads gUnk_03001EAC right after storing it
 * (lesson 3.472). */

extern vu8 gUnk_0300118C;          /* BLDCNT lo shadow */
extern vu8 gUnk_03000040;          /* BLDCNT hi shadow */
extern vu8 gUnk_03000B08;          /* BLDALPHA lo shadow (EVA) */
extern vu8 gUnk_03001EAC;          /* BLDALPHA hi shadow (EVB) */
extern u16 gUnk_03001370[];
extern u16 gUnk_085E2920[];
extern u16 gUnk_085E2A20[];
extern u16 gUnk_085E2B20[];

void TaskYieldTrampoline(s32 frames);
void sub_08003014(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void TaskMove(void);
void TaskSleepForever(void);

void sub_08019eec(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = (u32)TaskMove;
    t->unk0C = 0;
    gUnk_0300118C = 66;
    gUnk_03000040 = 29;
    gUnk_03000B08 = 16;
    gUnk_03001EAC = 0;
    TaskYieldTrampoline(112);
    TaskYieldTrampoline(160);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03001EAC = (s16)gUnk_03002490->unk6C >> 2;
        gUnk_03000B08 = 16 - gUnk_03001EAC;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 63);
    gUnk_03000B08 = 0;
    gUnk_03001EAC = 16;
    TaskYieldTrampoline(64);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(200);
    TaskYieldTrampoline(134);
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_08003014(gUnk_085E2920, gUnk_085E2A20, (u16)((s16)gUnk_03002490->unk6C * 16), 128, gUnk_03001370);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 15);
    TaskYieldTrampoline(86);
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_08003014(gUnk_085E2A20, gUnk_085E2920, (u16)((s16)gUnk_03002490->unk6C * 16), 128, gUnk_03001370);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 15);
    gUnk_03002490->unk6C = 0;
    do
    {
        sub_08003014(gUnk_085E2920, gUnk_085E2B20, (u16)((s16)gUnk_03002490->unk6C * 16), 128, gUnk_03001370);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 15);
    TaskSleepForever();
}
