#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells / ROM tables */
extern s32 gUnk_02006040[];
extern s32 gUnk_03001F2C;
extern s16 gUnk_03002158[];
extern s32 gUnk_03002448;
extern u32 gUnk_080D2148[];
extern s16 gUnk_08748268[];

/* Externals */
extern s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, s32 e, s32 f);
extern u32 sub_08002ee8(u32 range);

void sub_080a00ec(void)
{
    struct Task *t;
    s32 i;
    register s32 r asm("r3");
    register s32 n asm("r3");
    s32 nn;
    register u32 m asm("r2");
    s32 x;
    s32 y;
    s32 sh;
    s32 uy;
    s32 v;
    s16 *tb;
    register s32 j asm("r2");
    s32 a3;
    register s32 tb2 asm("r0");
    s32 o3;
    register s32 o asm("r2");
    s32 i6;
    register s32 *q6 asm("r1");
    s32 o6;
    s32 *gi;
    s32 *pa;
    s32 *pb;

    pa = &gUnk_03001F2C;
    pb = &gUnk_03002448;
    for (i = 0; i <= 2; i++)
    {
        o3 = i * 4 + 12;
        o6 = i * 4 + 24;
        if (gUnk_02006040[i] == 0)
        {
            gUnk_02006040[i + 6] = 0;
            r = sub_08002ee8(16);
            if (gUnk_03002490->unk43 == 1)
            {
                tb = gUnk_08748268;
                j = r * 2;
                v = (*(s16 *)(j + (s32)tb) + 20) << 16;
            }
            else
            {
                tb = gUnk_08748268;
                j = r * 2;
                v = -((*(s16 *)(j + (s32)tb) + 20) << 16);
            }
            gUnk_02006040[i] = v;
            gUnk_02006040[i + 3] = *(s16 *)(j + (tb2 = (s32)tb + 32)) << 16;
        }
        if (abs(gUnk_02006040[i]) <= (224 << 13))
        {
            gUnk_02006040[i] = 0;
        }
        else
        {
            if (gUnk_02006040[i] > 0)
            {
                q6 = (s32 *)(o6 + (s32)gUnk_02006040);
                *q6 += 0xFFFFA000;
            }
            else
            {
                q6 = (s32 *)(o6 + (s32)gUnk_02006040);
                *q6 += 192 << 7;
            }
            i6 = i + 6;
            gi = &gUnk_02006040[i];
            asm("" : "+r"(i6));
            asm("" :: "r"(i));
            *gi += *(s32 *)(i6 * 4 + (s32)gUnk_02006040);
            nn = abs(*gi) >> 20;
            n = nn + 2;
            asm("" :: "r"(nn));
            a3 = abs((*(s32 *)(o3 + (s32)gUnk_02006040)));
            m = (0xFFFF0000 & a3) >> n;
            asm("" :: "r"(a3));
            if ((*(s32 *)(o3 + (s32)gUnk_02006040)) > 0)
                m = -m;
            o = (*(s32 *)(o3 + (s32)gUnk_02006040)) + m;
            (*(s32 *)(o3 + (s32)gUnk_02006040)) = o;
            t = gUnk_03002490;
            x = t->unk48 + ((s16 *)gi)[1] - gUnk_03002158[0];
            *pa = x;
            uy = t->unk4A;
            sh = o >> 16;
            sh += 16;
            y = uy + sh - gUnk_03002158[2];
            *pb = y;
            sub_08001a94(t->unk42, (u32)gUnk_080D2148, 0, 0, x, (s16)y);
        }
    }
}
