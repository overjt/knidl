#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern struct Task *gUnk_03002490;
extern u32 gUnk_08732104[];

s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
s32 sub_08002ee8(s32 a);
u32 sub_0800641c(s16 x, s16 y);

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
    s32 sh;
    s32 t;
    s32 d;
    s32 x;
    s32 y;
    s32 n;
    /* Pin (3.229): without it the RNG result coalesces into r0 and the whole
       block's r0/r1/r2 assignment rotates.  Pinning the table temp to r0 is
       what forces the ROM's `adds r1, r0, #0` copy of the call result. */
    register s16 *tp asm("r0");

    i = 0;
    do
    {
        p = &gUnk_02007E90[gUnk_03002490->unk88->unk00][i];
        if (p->unk00 == 0)
        {
            p->unk08 = 0;
            n = sub_08002ee8(16);
            tp = &gUnk_087320C4[0][n];
            p->unk00 = -(*tp << 16);
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
            sh = (abs(p->unk00) >> 20) + 1;
            u = *(volatile s32 *)&p->unk04;
            t = (abs(u) & 0xFFFF0000) >> sh;
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
            x = gUnk_03002490->unk48 + ((s16 *)&p->unk00)[1];
            y = gUnk_03002490->unk4A + ((s16 *)&p->unk04)[1] + 4;
            if (sub_0800641c(x, y))
                sub_08001a94(gUnk_03002490->unk42 - 1, gUnk_08732104[d], 0, 12, x, (s16)y);
        }
        i++;
    } while (i <= 2);
}
