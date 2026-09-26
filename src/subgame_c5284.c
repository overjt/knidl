#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* subgame_c5284.c (0x080C5284-0x080C5B83, issue #98).
 *
 * Sub-game 2: the course builder.
 * 
 *   sub_080c59d8   called by M35's sub_080b9f34 as sub_080c59d8(level, 1)
 *       before the race screen loads: the BG control and scroll shadows, the
 *       course record gUnk_0201B0E0 (scroll 240, start line 1000, finish line
 *       6000/8500/12000 for levels 0-2, the four racers at 0), cleared VRAM,
 *       the BG palette from gUnk_080D0198, then sub_080c5678 and a first
 *       render by sub_080c5b84 (still asm, 0x080C5B84-0x080C623B).
 *   sub_080c5678   lays the course out: per lane the distance table
 *       gUnk_02017980[lane][500] (the running sum of 0x4000 / (depth + 512),
 *       scaled to 16000) and its inverse gUnk_02019140, then 2n + 1
 *       alternating segment lengths gUnk_0201B690[] from the LCG (n from the
 *       level table gUnk_080D075A), the bitmap gUnk_02018920 with a bit per
 *       pixel of the odd segments, and each lane's segment boundaries
 *       gUnk_0201B200[lane][] (terminated by 0x7D000).
 *   sub_080c52c4 / sub_080c5284   a lane's lateral offset, depth and third
 *       coordinate at a course position (curves from the sine table
 *       gUnk_080D0398).
 *   sub_080c54a4 / sub_080c553c / sub_080c5580   segment queries: whether a
 *       position is on an even segment of the lane (the course record's
 *       unk14), the next boundary after it, and the set/clear bit counts of
 *       gUnk_02018920 over a span (the racers' scores).
 *   sub_080c55d8 / sub_080c5628   linear interpolation in gUnk_02017980 /
 *       gUnk_02019140 at 32-pixel steps. */

/* per-player records of gUnk_0201B0E0, M37Course.unk018[4] (0x3C bytes) */
struct M37CoursePlayer
{
    /*0x00*/ s32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ s32 unk0C;
    /*0x10*/ s32 unk10;
    /*0x14*/ s32 unk14;
    /*0x18*/ s32 unk18;
    /*0x1C*/ s32 unk1C;
    /*0x20*/ s32 unk20;
    /*0x24*/ s32 unk24;
    /*0x28*/ s32 unk28;
    /*0x2C*/ s32 unk2C;
    /*0x30*/ s32 unk30;
    /*0x34*/ s32 unk34;
    /*0x38*/ s32 unk38;
};

/* gUnk_0201B0E0, reached through gUnk_0201716C (and directly by the
   0x080C5284-0x080C623C builder) */
struct M37Course
{
    /*0x000*/ s32 unk000;
    /*0x004*/ s32 unk004;
    /*0x008*/ s32 unk008;
    /*0x00C*/ s32 unk00C;
    /*0x010*/ s32 unk010;
    /*0x014*/ s32 unk014;
    /*0x018*/ struct M37CoursePlayer unk018[4];
    /*0x108*/ s32 unk108;
    /*0x10C*/ s32 unk10C;
    /*0x110*/ s32 unk110;
};

extern struct M37Course gUnk_0201B0E0;
extern s16 gUnk_080D0398[];
extern s32 gUnk_0201BFC0;
extern s32 gUnk_0201B200[4][73];
extern s32 gUnk_0201B690[];
extern u32 gUnk_02018920[];
extern s16 gUnk_02017980[4][500];
extern s16 gUnk_02019140[4][500];
extern s16 gUnk_0201B1F4;
extern s16 gUnk_080D075A[];
extern s16 gUnk_080D0760[];
extern vu16 gUnk_03001188;
extern vu16 gUnk_03000B14;
extern vu16 gUnk_03000B10;
extern vu16 gUnk_03001EB4;
extern u32 gUnk_0300117C;
extern vs32 gUnk_03001EE0;
extern vs32 gUnk_03000F8C;
extern vs32 gUnk_03000B78;
extern u32 gUnk_03000010;
extern vs32 gUnk_03000FC0;
extern vs32 gUnk_03001E94;
extern vs32 gUnk_03000FA8;
extern u16 gUnk_03001270[];
extern u16 gUnk_080D0198[];

u32 sub_08002ec0(void);                                      /* LCG step */
void sub_080c5b84(void);

s32 sub_080c5284(s32 angle)
{
    s32 i = angle & 0xFF;

    if (angle & 0x100)
        i = 0x100 - i;
    return (angle & 0x200) ? -gUnk_080D0398[i] : gUnk_080D0398[i];
}

void sub_080c52c4(s32 lane, s32 x, s32 *px, s32 *py, s32 *pz)
{
    s32 d = 0;
    s32 amp;
    s32 phase;
    s32 k;
    s32 v;
    s32 off;

    if (x < gUnk_0201B0E0.unk00C)
    {
        amp = 0;
        switch (lane)
        {
        case 0:
            d = 0;
            break;
        case 1:
            d = 900 - x;
            if (d < 0)
                d = 0;
            d = -d * d / 1024;
            break;
        case 2:
            d = 800 - x;
            if (d < 0)
                d = 0;
            d = d * d / 1024;
            break;
        case 3:
            d = 700 - x;
            if (d < 0)
                d = 0;
            d = -d * d / 1024;
            break;
        }
        if (d < -90)
            d = -90;
        if (d > 100)
            d = 100;
    }
    else if (x < gUnk_0201B0E0.unk00C + 1000)
        amp = Div((x - 1000) << 8, 1000);
    else if (x < gUnk_0201B0E0.unk010 - 1000)
        amp = 256;
    else if (x < gUnk_0201B0E0.unk010)
        amp = Div((gUnk_0201B0E0.unk010 - x) << 8, 1000);
    else
        amp = 0;

    switch (lane)
    {
    case 0:
        phase = x;
        k = 51;
        break;
    case 1:
        phase = Div((x + 152) * 6, 5);
        k = 85;
        break;
    case 2:
        phase = Div((x + 854) * 6, 5);
        k = 85;
        break;
    case 3:
        phase = x + 681;
        k = 85;
        break;
    }
    phase += gUnk_0201BFC0;
    v = sub_080c5284(phase) * k * amp / 0x100000 - lane * 16;
    off = d + 16;
    *px = v + off;
    *py = sub_080c5284(phase + 256) * k * amp * 3 / 0x100000 + lane * 64;
    *pz = sub_080c5284(phase) * k * amp / 0x100000;
}

void sub_080c54a4(s32 lane, s32 *idx, s32 x, s32 *out)
{
    if (x < 1000)
    {
        *out = 1;
        return;
    }
    while (x < gUnk_0201B200[lane][*idx])
        (*idx)--;
    while (gUnk_0201B200[lane][*idx] <= x)
        (*idx)++;
    *out = (*idx % 2) == 0;
}

s32 sub_080c553c(s32 i, s32 x)
{
    while (x < gUnk_0201B690[i])
        i--;
    while (gUnk_0201B690[i] <= x)
        i++;
    return gUnk_0201B690[i];
}

void sub_080c5580(s32 start, s32 end, s32 *set, s32 *clear)
{
    s32 i;

    *clear = 0;
    *set = 0;
    for (i = start; i < end; i++)
    {
        if (gUnk_02018920[i / 32] & (1 << (i - (i / 32) * 32)))
            (*set)++;
        else
            (*clear)++;
    }
}

s32 sub_080c55d8(s32 lane, s32 x)
{
    s32 lo;
    s32 hi;

    if (x < 0)
        x = 0;
    lo = gUnk_02017980[lane][x / 32];
    hi = gUnk_02017980[lane][x / 32 + 1];
    x -= (x / 32) * 32;
    return (hi - lo) * x + lo * 32;
}

s32 sub_080c5628(s32 lane, s32 x)
{
    s32 lo;
    s32 hi;

    if (x < 0)
        x = 0;
    lo = gUnk_02019140[lane][x / 32];
    hi = gUnk_02019140[lane][x / 32 + 1];
    x -= (x / 32) * 32;
    return (hi - lo) * x + lo * 32;
}

void sub_080c5678(s32 a)
{
    s32 x, y, z;
    s32 sums[2];
    s32 i;
    s32 j;
    s32 n;
    s32 m;
    s32 v;

    gUnk_0201BFC0 = sub_08002ec0();
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 500; j++)
        {
            sub_080c52c4(i, j * 32, &x, &y, &z);
            gUnk_02017980[i][j] = 0x4000 / (y + 512);
            if (j > 0)
                gUnk_02017980[i][j] += gUnk_02017980[i][j - 1];
        }
        for (j = 0; j < 500; j++)
            gUnk_02017980[i][j] = gUnk_02017980[i][j] * 16000 / gUnk_02017980[i][499];
    }
    for (i = 0; i < 4; i++)
    {
        z = 0;
        for (j = 1; j < 500; j++)
        {
            while (z * 32 < gUnk_02017980[i][j])
            {
                gUnk_02019140[i][z] = (z * 32 - gUnk_02017980[i][j - 1]) * 32
                    / (gUnk_02017980[i][j] - gUnk_02017980[i][j - 1]) + (j - 1) * 32;
                z++;
            }
        }
    }
    n = gUnk_080D075A[a];
    m = gUnk_080D0760[a];
    sums[0] = sums[1] = 0;
    gUnk_0201B0E0.unk014 = n;
    for (i = 0; i < n * 2 + 1; i++)
    {
        if (!(i & 1))
        {
            gUnk_0201B690[i] = (sub_08002ec0() & 63) + 224;
            gUnk_0201B690[i] -= 200 * i / (n * 2);
        }
        else
            gUnk_0201B690[i] = sub_08002ec0() % 320 + 96;
        sums[i % 2] += gUnk_0201B690[i];
    }
    for (i = 0; i < n * 2 + 1; i++)
    {
        v = gUnk_0201B690[i];
        if (!(i & 1))
            gUnk_0201B690[i] = (gUnk_0201B0E0.unk010 - gUnk_0201B0E0.unk00C - m) * v;
        else
            gUnk_0201B690[i] = m * v;
        gUnk_0201B690[i] /= sums[i % 2];
    }
    gUnk_0201B690[0] += 1000;
    j = 0;
    for (i = 1; i < n * 2 + 1; i++)
    {
        if (i & 1)
        {
            while (j < gUnk_0201B690[i - 1])
            {
                gUnk_02018920[j / 32] = (gUnk_02018920[j / 32] >> 1) | 0x80000000;
                j++;
            }
        }
        else
        {
            while (j < gUnk_0201B690[i - 1])
            {
                gUnk_02018920[j / 32] >>= 1;
                j++;
            }
        }
        gUnk_0201B690[i] += gUnk_0201B690[i - 1];
    }
    gUnk_0201B1F4 = n * 2;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < gUnk_0201B1F4; j++)
            gUnk_0201B200[i][j] = sub_080c55d8(i, gUnk_0201B690[j]);
        gUnk_0201B200[i][j] = 0x7D000;
    }
}

void sub_080c59d8(s32 a, s32 b)
{
    s32 i;
    struct M37CoursePlayer *p;

    gUnk_03001188 = 0x1C80;
    gUnk_03000B14 = 0x1D81;
    gUnk_03000B10 = 0x1E82;
    gUnk_03001EB4 = 0x1F83;
    gUnk_0300117C = gUnk_03001EE0 = gUnk_03000F8C = gUnk_03000B78 = 0;
    gUnk_03000010 = gUnk_03000FC0 = gUnk_03001E94 = gUnk_03000FA8 = 0x300000;
    gUnk_0201B0E0.unk110 = a;
    gUnk_0201B0E0.unk10C = b;
    gUnk_0201B0E0.unk108 = 0;
    gUnk_0201B0E0.unk000 = 240;
    gUnk_0201B0E0.unk004 = 0;
    gUnk_0201B0E0.unk008 = 0;
    for (i = 0; i < 4; i++)
    {
        p = &gUnk_0201B0E0.unk018[i];
        p->unk34 = p->unk00 = gUnk_0201B0E0.unk108;
        p->unk30 = 0;
        p->unk04 = 0;
        p->unk38 = 1;
        p->unk24 = 0;
        p->unk20 = 0;
        p->unk2C = 0;
    }
    gUnk_0201B0E0.unk00C = 1000;
    switch (a)
    {
    case 0:
        gUnk_0201B0E0.unk010 = 6000;
        break;
    case 1:
        gUnk_0201B0E0.unk010 = 8500;
        break;
    case 2:
        gUnk_0201B0E0.unk010 = 12000;
        break;
    }
    for (i = 0; i < 0x1000; i++)
        ((u16 *)0x0600E000)[i] = 3;
    for (i = 0; i < 64; i++)
        ((vu8 *)0x06000000)[i] = 0;
    for (i = 0; i < 256; i++)
        gUnk_03001270[i] = gUnk_080D0198[i];
    sub_080c5678(a);
    for (i = 0; i < 64; i++)
        ((vu8 *)0x0600C000)[i] = 8;
    for (i = 0; i < 0x400; i++)
        ((u16 *)0x0600F800)[i] = 0;
    *(u16 *)0x0600FC20 = 0x300;
    gUnk_0201B0E0.unk000 = 360;
    sub_080c5b84();
}
