#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* subgame_c5284.c (0x080C5284-0x080C623B, issue #98).
 *
 * Sub-game 2: the course builder and the course renderer.
 * 
 *   sub_080c59d8   called by M35's sub_080b9f34 as sub_080c59d8(level, 1)
 *       before the race screen loads: the BG control and scroll shadows, the
 *       course record gAirGrindCourse (scroll 240, start line 1000, finish line
 *       6000/8500/12000 for levels 0-2, the four racers at 0), cleared VRAM,
 *       the BG palette from gUnk_080D0198, then sub_080c5678 and a first
 *       render by sub_080c5b84.
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
 *       gUnk_02019140 at 32-pixel steps.
 *   sub_080c5b84   the course renderer (called every frame by player 0's
 *       racer step sub_080c383c, src/subgame_c3648.c, and once by
 *       sub_080c59d8): per lane, every course
 *       column that scrolled into view since the last frame
 *       (gAirGrindCourse.unk108 -> unk000) gets its BG map column at 0x0600E000
 *       and a vertical strip in its tiles, sized by the depth (gUnk_080D059A)
 *       and shaded by the segment it lies on; the strip's tile address,
 *       height and segment flag go to gUnk_0201A0E0/gUnk_0201B7C0/
 *       gUnk_02017180[lane][256].  Then the lane's racer record is updated
 *       (position, depth, the scroll/scale words gUnk_08757300[lane]/
 *       gUnk_08757310[lane]), the strips of the columns it passed are
 *       brightened, its score counts the segment bits over the passed span
 *       (sub_080c5580), and finally the four lanes are ranked by depth into
 *       the racers' unk18 and the priority bits of *gUnk_08757320[lane].
 *
 * Matching note (issue #98's final campaign, lesson 3.493): sub_080c5b84
 * only matches in this translation unit.  Its PRE spill slots follow
 * gcse's hash buckets, which depend on its locals' pseudo numbers (the
 * declaration order: lane first, p right after x, y, z) and on the pool
 * label of &gAirGrindCourse, numbered after this file's 40 earlier ones; #98
 * had parked it at 22 bytes (or byte-exact with 37 empty asm statements). */

/* per-player records of gAirGrindCourse, M37Course.unk018[4] (0x3C bytes) */
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

/* gAirGrindCourse, reached through gAirGrindCoursePtr (and directly by the
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

extern struct M37Course gAirGrindCourse;
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
extern u32 gUnk_0201A0E0[4][256];
extern s16 gUnk_0201B7C0[4][256];
extern s16 gUnk_02017180[4][256];
extern s16 gUnk_080D059A[];
extern s16 gUnk_080D0766[];
extern s32 *gUnk_08757300[];
extern s32 *gUnk_08757310[];
extern u16 *gUnk_08757320[];

u32 Random(void);                                      /* LCG step */
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

    if (x < gAirGrindCourse.unk00C)
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
    else if (x < gAirGrindCourse.unk00C + 1000)
        amp = Div((x - 1000) << 8, 1000);
    else if (x < gAirGrindCourse.unk010 - 1000)
        amp = 256;
    else if (x < gAirGrindCourse.unk010)
        amp = Div((gAirGrindCourse.unk010 - x) << 8, 1000);
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

    gUnk_0201BFC0 = Random();
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
    gAirGrindCourse.unk014 = n;
    for (i = 0; i < n * 2 + 1; i++)
    {
        if (!(i & 1))
        {
            gUnk_0201B690[i] = (Random() & 63) + 224;
            gUnk_0201B690[i] -= 200 * i / (n * 2);
        }
        else
            gUnk_0201B690[i] = Random() % 320 + 96;
        sums[i % 2] += gUnk_0201B690[i];
    }
    for (i = 0; i < n * 2 + 1; i++)
    {
        v = gUnk_0201B690[i];
        if (!(i & 1))
            gUnk_0201B690[i] = (gAirGrindCourse.unk010 - gAirGrindCourse.unk00C - m) * v;
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
    gAirGrindCourse.unk110 = a;
    gAirGrindCourse.unk10C = b;
    gAirGrindCourse.unk108 = 0;
    gAirGrindCourse.unk000 = 240;
    gAirGrindCourse.unk004 = 0;
    gAirGrindCourse.unk008 = 0;
    for (i = 0; i < 4; i++)
    {
        p = &gAirGrindCourse.unk018[i];
        p->unk34 = p->unk00 = gAirGrindCourse.unk108;
        p->unk30 = 0;
        p->unk04 = 0;
        p->unk38 = 1;
        p->unk24 = 0;
        p->unk20 = 0;
        p->unk2C = 0;
    }
    gAirGrindCourse.unk00C = 1000;
    switch (a)
    {
    case 0:
        gAirGrindCourse.unk010 = 6000;
        break;
    case 1:
        gAirGrindCourse.unk010 = 8500;
        break;
    case 2:
        gAirGrindCourse.unk010 = 12000;
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
    gAirGrindCourse.unk000 = 360;
    sub_080c5b84();
}

void sub_080c5b84(void)
{
    s32 depth[4];
    s32 lane;
    u16 zero;
    s32 x, y, z;
    struct M37CoursePlayer *p;
    s32 flag;
    s32 set, clear;
    s32 k;
    s32 pos;
    s32 j;
    s32 col;
    s32 lo, hi;
    s32 t;
    s32 c32;
    s32 rem;
    u16 *vp;
    s32 m;
    s32 tile;
    s32 base;
    s32 w;
    s32 s;
    s32 h;
    u32 addr;
    s32 step;
    s32 v;
    s32 old;
    s32 a, b;
    s32 mark;
    s32 sn;

    for (lane = 0; lane < 4; lane++)
    {
        p = &gAirGrindCourse.unk018[lane];
        lo = sub_080c55d8(lane, gAirGrindCourse.unk000) / 32;
        hi = sub_080c55d8(lane, gAirGrindCourse.unk108) / 32;
        for (k = 0; k < lo - hi; k++)
        {
            j = k + 120;
            col = hi + j;
            t = col / 8;
            c32 = t % 32;
            rem = col % 8;
            sub_080c52c4(lane, col, &x, &y, &z);
            sub_080c54a4(lane, &p->unk2C, col * 32, &flag);
            if (x < -90)
                x = -90;
            if (x > 90)
                x = 90;
            if (rem == 0)
            {
                vp = (u16 *)0x0600E000 + (lane * 1024 + c32);
                for (m = 0; m < 32; m++)
                {
                    *vp = 1;
                    vp += 32;
                }
                vp += ((x + 80) / 8 - 28) * 32;
                base = (lane * 32 + c32) * 5;
                tile = base + 1;
                if (x > -100 && x < 100)
                {
                    *vp = tile;
                    vp += 32;
                    *vp = tile + 1;
                    vp += 32;
                    *vp = tile + 2;
                    vp += 32;
                    *vp = tile + 3;
                    vp += 32;
                    *vp = tile + 4;
                }
                addr = 0x06000000 + tile * 64;
                zero = 0;
                CpuSet(&zero, (void *)addr, 0x010000A0);
            }
            w = sub_080c5628(lane, col);
            if (flag != 0)
            {
                if (lane != 0)
                    s = (y + 256) / 128 * 32 + 64;
                else
                    s = 32;
                if (w & 0x200)
                    s += 16;
            }
            else
            {
                s = 16;
                if (lane == 0)
                    s = 8;
            }
            h = gUnk_080D059A[(y + 512) / 4 - 32] * 6 / 256;
            pos = lane * 1024 + c32;
            vp = (u16 *)0x0600E180 + (pos + (x + 80) / 8 * 32);
            addr = *vp * 64 + 0x06000000;
            addr += rem + ((x + 80) % 8 - h / 2) * 8;
            if (addr < 0x06000040)
                addr = 0x06000040;
            if (flag == 0 && (col / 2) & 1)
            {
                addr -= 16;
                h += 4;
            }
            gUnk_0201A0E0[lane][col % 256] = addr;
            gUnk_0201B7C0[lane][col % 256] = h + 1;
            gUnk_02017180[lane][col % 256] = flag;
            s <<= 8;
            step = gUnk_080D0766[h];
            if (addr & 1)
            {
                addr &= ~1;
                for (m = 0; m < h + 1; m++)
                {
                    *(vu16 *)addr = (s / 256 << 8) | *(vu16 *)addr;
                    addr += 8;
                    s += step;
                }
            }
            else
            {
                for (m = 0; m < h + 1; m++)
                {
                    *(vu16 *)addr = s / 256;
                    addr += 8;
                    s += step;
                }
            }
        }
        *gUnk_08757300[lane] = (lo - 120) << 16;
        v = sub_080c55d8(lane, p->unk00);
        sub_080c52c4(lane, v / 32, &p->unk10, &depth[lane], &p->unk1C);
        p->unk38 = p->unk14;
        sub_080c54a4(lane, &p->unk2C, v, &p->unk14);
        p->unk28 = sub_080c553c(p->unk2C, p->unk00);
        v /= 32;
        p->unk08 = depth[lane] + 512;
        sn = gUnk_080D059A[p->unk08 / 4 - 32];
        *gUnk_08757310[lane] = (gAirGrindCourse.unk004 * sn << 8) + 0x300000;
        sn = gUnk_080D059A[p->unk08 / 4 - 32];
        *gUnk_08757300[lane] += sn * gAirGrindCourse.unk008 << 8;
        old = p->unk0C;
        p->unk0C = v - lo + 120;
        p->unk10 += 128 - *gUnk_08757310[lane] / 65536;
        if (gAirGrindCourse.unk10C != 0)
        {
            if (p->unk04 != 0 || p->unk30 != 0 || p->unk0C > 240)
            {
                a = old;
                b = p->unk0C;
                if (a > -8)
                {
                    if (a > 240)
                        a = 240;
                    if (b > 240)
                        b = 240;
                    j = a - 120;
                    a = j + hi;
                    j = b - 120;
                    b = j + lo;
                    mark = 0;
                    if ((p->unk30 != 0 && p->unk38 != 0 && p->unk04 == 0 && p->unk14 == 0)
                        || (p->unk30 == 0 && p->unk38 == 0 && p->unk04 != 0 && p->unk14 != 0))
                        mark = 1;
                    if (p->unk0C > 240)
                        mark = 1;
                    for (k = a; k < b; k++)
                    {
                        if (*(u8 *)gUnk_0201A0E0[lane][k % 256] != 0
                            && (mark == 0 || gUnk_02017180[lane][k % 256] != 0))
                        {
                            addr = gUnk_0201A0E0[lane][k % 256];
                            if (lane != 0)
                                col = 8;
                            else if (gUnk_02017180[0][k % 256] != 0)
                                col = 8;
                            else
                                col = 16;
                            if (addr & 1)
                            {
                                addr &= ~1;
                                for (m = 0; m < gUnk_0201B7C0[lane][k % 256]; m++)
                                {
                                    *(vu16 *)addr += col << 8;
                                    addr += 8;
                                }
                            }
                            else
                            {
                                for (m = 0; m < gUnk_0201B7C0[lane][k % 256]; m++)
                                {
                                    *(vu16 *)addr += col;
                                    addr += 8;
                                }
                            }
                        }
                    }
                }
            }
        }
        if (gAirGrindCourse.unk00C <= p->unk00 && p->unk34 <= gAirGrindCourse.unk010)
        {
            k = p->unk34;
            m = p->unk00;
            if (k < gAirGrindCourse.unk00C)
                k = gAirGrindCourse.unk00C;
            if (m > gAirGrindCourse.unk010)
                m = gAirGrindCourse.unk010;
            sub_080c5580(k, m, &set, &clear);
            if (p->unk04 != 0)
                p->unk24 += set;
            p->unk20 += set;
        }
        p->unk30 = p->unk04;
        p->unk34 = p->unk00;
    }
    depth[1] += 256;
    depth[2] += 304;
    depth[3] += 352;
    for (lane = 0; lane < 4; lane++)
    {
        m = 0;
        for (k = 0; k < 4; k++)
            if (depth[lane] > depth[k])
                m++;
        gAirGrindCourse.unk018[lane].unk18 = m;
        if (m == 3)
            m = 2;
        *gUnk_08757320[lane] = (*gUnk_08757320[lane] & 0xFFFC) | m;
    }
    gAirGrindCourse.unk108 = gAirGrindCourse.unk000;
}
