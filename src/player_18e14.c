#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_18e14.c (0x08018E14-0x08018FFF, issue #125).
 *
 * Animation script 52 of the sequence bank (entry 60 of gUnk_08731FA8, run by
 * task type #92, src/player_10358.c): three sparkles that close in on the
 * anchor task gTasks[Task.unk44].  The particle array gUnk_02006040
 * holds per sparkle i an x offset [i], a y offset [i + 3] and an x
 * acceleration [i + 6], all 16.16.  For the first 50 frames a free slot is
 * refilled from the 16-entry start tables gUnk_08732150[0] (x, mirrored by the
 * facing Task.unk43) and gUnk_08732150[1] (y); every frame the x offset closes
 * in (the slot is freed within 10 pixels of the anchor), the y offset shrinks
 * in proportion to it, and each live sparkle is drawn around the anchor, its
 * position left in gUnk_03001F2C/gUnk_03002448.  The script ends after 61
 * frames.
 *
 * Matching notes (issue #125): gUnk_08732150 is a 2-D table (the ROM reaches
 * row 1 from the row-0 register plus 32), the outer loop is `while (1)`, and
 * the shift amount of the y step is its own statement reusing the dead index
 * variable r (lesson 3.473). */

extern s32 gUnk_02006040[];
extern s16 gUnk_08732150[][16];
extern s32 gUnk_03001F2C;
extern s32 gUnk_03002448;
extern u32 gUnk_080D2148[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s16 f);
s32 RandomRange(s32 n);

void sub_08018e14(void)
{
    s32 i;
    s32 r;
    s32 a;
    s32 v;

    for (i = 0; i < 10; i++)
        gUnk_02006040[i] = 0;
    gCurTask->unk6C = 0;
    while (1)
    {
        for (i = 0; i < 3; i++)
        {
            if ((s16)gCurTask->unk6C < 50 && gUnk_02006040[i] == 0)
            {
                gUnk_02006040[i + 6] = 0;
                r = RandomRange(16);
                if ((s8)gCurTask->unk43 == 1)
                    gUnk_02006040[i] = gUnk_08732150[0][r] << 16;
                else
                    gUnk_02006040[i] = -(gUnk_08732150[0][r] << 16);
                gUnk_02006040[i + 3] = gUnk_08732150[1][r] << 16;
            }
            if (abs(gUnk_02006040[i]) <= 0xA0000)
            {
                gUnk_02006040[i] = 0;
                continue;
            }
            if (gUnk_02006040[i] >= 0)
                gUnk_02006040[i] -= 0x6000;
            else
                gUnk_02006040[i + 6] += 0x6000;
            gUnk_02006040[i] += gUnk_02006040[i + 6];
            a = abs(gUnk_02006040[i]);
            r = (a >> 20) + 2;      /* reuses r: a fresh variable swaps registers */
            v = (abs(gUnk_02006040[i + 3]) & 0xFFFF0000) >> r;
            if (gUnk_02006040[i + 3] > 0)
                v = -v;
            gUnk_02006040[i + 3] += v;
            if (gUnk_02006040[i] != 0)
            {
                gUnk_03001F2C = gTasks[gCurTask->unk44].unk48 + (gUnk_02006040[i] >> 16) - 8;
                gUnk_03002448 = gTasks[gCurTask->unk44].unk4A + (gUnk_02006040[i + 3] >> 16) + 16;
                QueueSprite(gCurTask->unk42, (u32)gUnk_080D2148, 0, 0, gUnk_03001F2C, gUnk_03002448);
            }
        }
        gCurTask->unk6C++;
        if ((s16)gCurTask->unk6C > 60)
            break;
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}
