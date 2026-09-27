#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern struct Task *gCurTask;
extern u32 gUnk_08732104[];

s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
s32 RandomRange(s32 a);
u32 IsWorldPosOnScreen(s16 x, s16 y);

/* 16-byte per-slot record, 3 slots per player, at gUnk_02007E90.
   unk00/unk04 are 16.16 (x,y) offsets whose high halves are read directly,
   unk08 is the 16.16 y-delta, unk0C a down-counter, unk0D a frame id.
   hdr.c only has `extern u32 gUnk_02007E90[]`, so the real 2-D shape and the
   s16 pair-table at 0x087320C4 are aliased in here. */
struct M04Spark
{
    /*0x00*/ s32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ u8 unk0C;
    /*0x0D*/ u8 unk0D;
    /*0x0E*/ u16 unk0E;
};

extern struct M04Spark gUnk_02007E90[][3];
extern s16 gUnk_087320C4[][16];

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
        if (p->unk00 == 0)
        {
            p->unk08 = 0;
            n = RandomRange(16);
            p->unk00 = -(gUnk_087320C4[0][n] << 16);
            p->unk04 = gUnk_087320C4[1][n] << 16;
        }
        if (abs(p->unk00) <= 0xF0000)
        {
            p->unk00 = 0;
            p->unk0C = 1;
            p->unk0D = 0;
        }
        else
        {
            if (p->unk00 > 0)
                p->unk08 -= 0x6000;
            else
                p->unk08 += 0x6000;
            /* The two `volatile` reads are placeholders for a source shape not
               yet identified: the ROM re-reads unk00 here and unk04 below
               instead of reusing the value it already holds, and a plain read
               is folded into the earlier one. */
            p->unk00 = *(volatile s32 *)&p->unk00 + p->unk08;
            /* the shift count reuses n (twin sub_0803c9b4): its own local swaps r0/r1 at the RNG call */
            n = (abs(p->unk00) >> 20) + 1;
            u = *(volatile s32 *)&p->unk04;
            t = (abs(u) & 0xFFFF0000) >> n;
            if (p->unk04 > 0)
                t = -t;
            p->unk04 += t;
            if (--p->unk0C == 0)
            {
                if (p->unk0D == 0)
                    p->unk0D = 3;
                else if (p->unk0D <= 9)
                    p->unk0D += 2;
                p->unk0C = 1;
            }
            d = p->unk0D;
            x = gCurTask->pixelX + ((s16 *)&p->unk00)[1];
            y = gCurTask->pixelY + ((s16 *)&p->unk04)[1] + 4;
            if (IsWorldPosOnScreen(x, y))
                QueueSprite(gCurTask->layer - 1, gUnk_08732104[d], 0, 12, x, (s16)y);
        }
        i++;
    } while (i <= 2);
}
