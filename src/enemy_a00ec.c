#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern s32 gUnk_02006040[];
extern s32 gUnk_03001F2C;
extern s16 gViewRect[];
extern s32 gUnk_03002448;
extern u32 gUnk_080D2148[];
extern s16 gUnk_08748268[2][16];

/* Externals */
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
extern u32 RandomRange(u32 range);

void sub_080a00ec(void)
{
    s32 i;
    s32 r;
    s32 a;
    s32 v;
    s32 uy;
    s32 sh;
    struct Task *t;
    s32 *pb;

    pb = &gUnk_03002448;
    for (i = 0; i < 3; i++)
    {
        if (gUnk_02006040[i] == 0)
        {
            gUnk_02006040[i + 6] = 0;
            r = RandomRange(16);
            if (gCurTask->facing == 1)
                gUnk_02006040[i] = (gUnk_08748268[0][r] + 20) << 16;
            else
                gUnk_02006040[i] = -((gUnk_08748268[0][r] + 20) << 16);
            gUnk_02006040[i + 3] = gUnk_08748268[1][r] << 16;
        }
        if (abs(gUnk_02006040[i]) <= (224 << 13))
        {
            gUnk_02006040[i] = 0;
            continue;
        }
        if (gUnk_02006040[i] > 0)
            gUnk_02006040[i + 6] -= 0x6000;
        else
            gUnk_02006040[i + 6] += 0x6000;
        gUnk_02006040[i] += gUnk_02006040[i + 6];
        a = abs(gUnk_02006040[i]);
        r = (a >> 20) + 2;
        v = (abs(gUnk_02006040[i + 3]) & 0xFFFF0000) >> r;
        if (gUnk_02006040[i + 3] > 0)
            v = -v;
        gUnk_02006040[i + 3] += v;
        t = gCurTask;
        gUnk_03001F2C = t->pixelX + (gUnk_02006040[i] >> 16) - gViewRect[0];
        uy = t->pixelY;
        sh = gUnk_02006040[i + 3] >> 16;
        sh += 16;
        *pb = uy + sh - gViewRect[2];
        QueueSprite(t->layer, (u32)gUnk_080D2148, 0, 0, gUnk_03001F2C, (s16)*pb);
    }
}
