#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "cutscene.h"
#include "player.h"

/* Declared here, not through a header: the calls in this file pass other
   types than the definition takes (lessons 3.428, 3.517). */
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
s32 RandomRange(s32 a);
u32 IsWorldPosOnScreen(s16 x, s16 y);

void sub_080109c8(void)
{
    struct M04Spark *p;
    s32 i;
    s32 u;
    s32 t;
    s32 d;
    s32 x;
    s32 y;
    s32 n;

    i = 0;
    do
    {
        p = &gUnk_02007E90[gCurTask->player->playerIndex][i];
        if (p->offsetX == 0)
        {
            p->velX = 0;
            n = RandomRange(16);
            p->offsetX = -(gUnk_087320C4[0][n] << 16);
            p->offsetY = gUnk_087320C4[1][n] << 16;
        }
        if (abs(p->offsetX) <= 0xF0000)
        {
            p->offsetX = 0;
            p->frameTimer = 1;
            p->frame = 0;
        }
        else
        {
            if (p->offsetX > 0)
                p->velX -= 0x6000;
            else
                p->velX += 0x6000;
            /* The two `volatile` reads are placeholders for a source shape not
               yet identified: the ROM re-reads unk00 here and unk04 below
               instead of reusing the value it already holds, and a plain read
               is folded into the earlier one. */
            p->offsetX = *(volatile s32 *)&p->offsetX + p->velX;
            /* the shift count reuses n (twin sub_0803c9b4): its own local swaps r0/r1 at the RNG call */
            n = (abs(p->offsetX) >> 20) + 1;
            u = *(volatile s32 *)&p->offsetY;
            t = (abs(u) & 0xFFFF0000) >> n;
            if (p->offsetY > 0)
                t = -t;
            p->offsetY += t;
            if (--p->frameTimer == 0)
            {
                if (p->frame == 0)
                    p->frame = 3;
                else if (p->frame <= 9)
                    p->frame += 2;
                p->frameTimer = 1;
            }
            d = p->frame;
            x = gCurTask->pixelX + ((s16 *)&p->offsetX)[1];
            y = gCurTask->pixelY + ((s16 *)&p->offsetY)[1] + 4;
            if (IsWorldPosOnScreen(x, y))
                QueueSprite(gCurTask->layer - 1, gUnk_08732104[d], 0, 12, x, (s16)y);
        }
        i++;
    } while (i <= 2);
}
