#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "subgame.h"

/* subgame_c5284.c (0x080C5284-0x080C623B, issue #98).
 *
 * Sub-game 2: the course builder and the course renderer.
 * 
 *   AirGrindBuildCourse   called by M35's SubGameRunScreen as AirGrindBuildCourse(level, 1)
 *       before the race screen loads: the BG control and scroll shadows, the
 *       course record gAirGrindCourse (scroll 240, start line 1000, finish line
 *       6000/8500/12000 for levels 0-2, the four racers at 0), cleared VRAM,
 *       the BG palette from gUnk_080D0198, then AirGrindLayOutCourse and a first
 *       render by AirGrindDrawCourse.
 *   AirGrindLayOutCourse   lays the course out: per lane the distance table
 *       gAirGrindCourseToLane[lane][500] (the running sum of 0x4000 / (depth + 512),
 *       scaled to 16000) and its inverse gAirGrindLaneToCourse, then 2n + 1
 *       alternating segment lengths gAirGrindSegmentEnds[] from the LCG (n from the
 *       level table gUnk_080D075A), the bitmap gAirGrindSegmentBits with a bit per
 *       pixel of the odd segments, and each lane's segment boundaries
 *       gAirGrindLaneSegmentEnds[lane][] (terminated by 0x7D000).
 *   AirGrindCalcLanePoint / AirGrindSin   a lane's lateral offset, depth and third
 *       coordinate at a course position (curves from the sine table
 *       gAirGrindSineTable).
 *   AirGrindFindLaneSegment / AirGrindFindSegmentEnd / AirGrindCountSegmentBits   segment queries: whether a
 *       position is on an even segment of the lane (the course record's
 *       unk14), the next boundary after it, and the set/clear bit counts of
 *       gAirGrindSegmentBits over a span (the racers' scores).
 *   AirGrindCourseToLanePos / AirGrindLaneToCoursePos   linear interpolation in gAirGrindCourseToLane /
 *       gAirGrindLaneToCourse at 32-pixel steps.
 *   AirGrindDrawCourse   the course renderer (called every frame by player 0's
 *       racer step AirGrindRacerUpdateDepth, src/subgame_c3648.c, and once by
 *       AirGrindBuildCourse): per lane, every course
 *       column that scrolled into view since the last frame
 *       (gAirGrindCourse.unk108 -> unk000) gets its BG map column at 0x0600E000
 *       and a vertical strip in its tiles, sized by the depth (gUnk_080D059A)
 *       and shaded by the segment it lies on; the strip's tile address,
 *       height and segment flag go to gAirGrindStripAddrs/gAirGrindStripHeights/
 *       gAirGrindStripOnEvenSegment[lane][256].  Then the lane's racer record is updated
 *       (position, depth, the scroll/scale words gUnk_08757300[lane]/
 *       gUnk_08757310[lane]), the strips of the columns it passed are
 *       brightened, its score counts the segment bits over the passed span
 *       (AirGrindCountSegmentBits), and finally the four lanes are ranked by depth into
 *       the racers' unk18 and the priority bits of *gUnk_08757320[lane].
 *
 * Matching note (issue #98's final campaign, lesson 3.493): AirGrindDrawCourse
 * only matches in this translation unit.  Its PRE spill slots follow
 * gcse's hash buckets, which depend on its locals' pseudo numbers (the
 * declaration order: lane first, p right after x, y, z) and on the pool
 * label of &gAirGrindCourse, numbered after this file's 40 earlier ones; #98
 * had parked it at 22 bytes (or byte-exact with 37 empty asm statements). */

u32 Random(void);                                      /* LCG step */

s32 AirGrindSin(s32 angle)
{
    s32 i = angle & 0xFF;

    if (angle & 0x100)
        i = 0x100 - i;
    return (angle & 0x200) ? -gAirGrindSineTable[i] : gAirGrindSineTable[i];
}

void AirGrindCalcLanePoint(s32 lane, s32 x, s32 *px, s32 *py, s32 *pz)
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
    else if (x < gAirGrindCourse.finishLine - 1000)
        amp = 256;
    else if (x < gAirGrindCourse.finishLine)
        amp = Div((gAirGrindCourse.finishLine - x) << 8, 1000);
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
    phase += gAirGrindCoursePhase;
    v = AirGrindSin(phase) * k * amp / 0x100000 - lane * 16;
    off = d + 16;
    *px = v + off;
    *py = AirGrindSin(phase + 256) * k * amp * 3 / 0x100000 + lane * 64;
    *pz = AirGrindSin(phase) * k * amp / 0x100000;
}

void AirGrindFindLaneSegment(s32 lane, s32 *idx, s32 x, s32 *out)
{
    if (x < 1000)
    {
        *out = 1;
        return;
    }
    while (x < gAirGrindLaneSegmentEnds[lane][*idx])
        (*idx)--;
    while (gAirGrindLaneSegmentEnds[lane][*idx] <= x)
        (*idx)++;
    *out = (*idx % 2) == 0;
}

s32 AirGrindFindSegmentEnd(s32 i, s32 x)
{
    while (x < gAirGrindSegmentEnds[i])
        i--;
    while (gAirGrindSegmentEnds[i] <= x)
        i++;
    return gAirGrindSegmentEnds[i];
}

void AirGrindCountSegmentBits(s32 start, s32 end, s32 *set, s32 *clear)
{
    s32 i;

    *clear = 0;
    *set = 0;
    for (i = start; i < end; i++)
    {
        if (gAirGrindSegmentBits[i / 32] & (1 << (i - (i / 32) * 32)))
            (*set)++;
        else
            (*clear)++;
    }
}

s32 AirGrindCourseToLanePos(s32 lane, s32 x)
{
    s32 lo;
    s32 hi;

    if (x < 0)
        x = 0;
    lo = gAirGrindCourseToLane[lane][x / 32];
    hi = gAirGrindCourseToLane[lane][x / 32 + 1];
    x -= (x / 32) * 32;
    return (hi - lo) * x + lo * 32;
}

s32 AirGrindLaneToCoursePos(s32 lane, s32 x)
{
    s32 lo;
    s32 hi;

    if (x < 0)
        x = 0;
    lo = gAirGrindLaneToCourse[lane][x / 32];
    hi = gAirGrindLaneToCourse[lane][x / 32 + 1];
    x -= (x / 32) * 32;
    return (hi - lo) * x + lo * 32;
}

void AirGrindLayOutCourse(s32 a)
{
    s32 x, y, z;
    s32 sums[2];
    s32 i;
    s32 j;
    s32 n;
    s32 m;
    s32 v;

    gAirGrindCoursePhase = Random();
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < 500; j++)
        {
            AirGrindCalcLanePoint(i, j * 32, &x, &y, &z);
            gAirGrindCourseToLane[i][j] = 0x4000 / (y + 512);
            if (j > 0)
                gAirGrindCourseToLane[i][j] += gAirGrindCourseToLane[i][j - 1];
        }
        for (j = 0; j < 500; j++)
            gAirGrindCourseToLane[i][j] = gAirGrindCourseToLane[i][j] * 16000 / gAirGrindCourseToLane[i][499];
    }
    for (i = 0; i < 4; i++)
    {
        z = 0;
        for (j = 1; j < 500; j++)
        {
            while (z * 32 < gAirGrindCourseToLane[i][j])
            {
                gAirGrindLaneToCourse[i][z] = (z * 32 - gAirGrindCourseToLane[i][j - 1]) * 32
                    / (gAirGrindCourseToLane[i][j] - gAirGrindCourseToLane[i][j - 1]) + (j - 1) * 32;
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
            gAirGrindSegmentEnds[i] = (Random() & 63) + 224;
            gAirGrindSegmentEnds[i] -= 200 * i / (n * 2);
        }
        else
            gAirGrindSegmentEnds[i] = Random() % 320 + 96;
        sums[i % 2] += gAirGrindSegmentEnds[i];
    }
    for (i = 0; i < n * 2 + 1; i++)
    {
        v = gAirGrindSegmentEnds[i];
        if (!(i & 1))
            gAirGrindSegmentEnds[i] = (gAirGrindCourse.finishLine - gAirGrindCourse.unk00C - m) * v;
        else
            gAirGrindSegmentEnds[i] = m * v;
        gAirGrindSegmentEnds[i] /= sums[i % 2];
    }
    gAirGrindSegmentEnds[0] += 1000;
    j = 0;
    for (i = 1; i < n * 2 + 1; i++)
    {
        if (i & 1)
        {
            while (j < gAirGrindSegmentEnds[i - 1])
            {
                gAirGrindSegmentBits[j / 32] = (gAirGrindSegmentBits[j / 32] >> 1) | 0x80000000;
                j++;
            }
        }
        else
        {
            while (j < gAirGrindSegmentEnds[i - 1])
            {
                gAirGrindSegmentBits[j / 32] >>= 1;
                j++;
            }
        }
        gAirGrindSegmentEnds[i] += gAirGrindSegmentEnds[i - 1];
    }
    gUnk_0201B1F4 = n * 2;
    for (i = 0; i < 4; i++)
    {
        for (j = 0; j < gUnk_0201B1F4; j++)
            gAirGrindLaneSegmentEnds[i][j] = AirGrindCourseToLanePos(i, gAirGrindSegmentEnds[j]);
        gAirGrindLaneSegmentEnds[i][j] = 0x7D000;
    }
}

void AirGrindBuildCourse(s32 a, s32 b)
{
    s32 i;
    struct AirGrindCourseRacer *p;

    gBg0Cnt = 0x1C80;
    gBg1Cnt = 0x1D81;
    gBg2Cnt = 0x1E82;
    gBg3Cnt = 0x1F83;
    gBg0ScrollX = gBg1ScrollX = gBg2ScrollX = gBg3ScrollX = 0;
    gBg0ScrollY = gBg1ScrollY = gBg2ScrollY = gBg3ScrollY = 0x300000;
    gAirGrindCourse.unk110 = a;
    gAirGrindCourse.unk10C = b;
    gAirGrindCourse.unk108 = 0;
    gAirGrindCourse.scrollPos = 240;
    gAirGrindCourse.unk004 = 0;
    gAirGrindCourse.unk008 = 0;
    for (i = 0; i < 4; i++)
    {
        p = &gAirGrindCourse.players[i];
        p->prevCoursePos = p->coursePos = gAirGrindCourse.unk108;
        p->prevHoldingA = 0;
        p->holdingA = 0;
        p->prevOnEvenSegment = 1;
        p->unk24 = 0;
        p->unk20 = 0;
        p->segmentIndex = 0;
    }
    gAirGrindCourse.unk00C = 1000;
    switch (a)
    {
    case 0:
        gAirGrindCourse.finishLine = 6000;
        break;
    case 1:
        gAirGrindCourse.finishLine = 8500;
        break;
    case 2:
        gAirGrindCourse.finishLine = 12000;
        break;
    }
    for (i = 0; i < 0x1000; i++)
        ((u16 *)(BG_VRAM + 0xE000))[i] = 3;
    for (i = 0; i < 64; i++)
        ((vu8 *)BG_VRAM)[i] = 0;
    for (i = 0; i < 256; i++)
        gBgPalette[i] = gUnk_080D0198[i];
    AirGrindLayOutCourse(a);
    for (i = 0; i < 64; i++)
        ((vu8 *)(BG_VRAM + 0xC000))[i] = 8;
    for (i = 0; i < 0x400; i++)
        ((u16 *)(BG_VRAM + 0xF800))[i] = 0;
    *(u16 *)(BG_VRAM + 0xFC20) = 0x300;
    gAirGrindCourse.scrollPos = 360;
    AirGrindDrawCourse();
}

void AirGrindDrawCourse(void)
{
    s32 depth[4];
    s32 lane;
    u16 zero;
    s32 x, y, z;
    struct AirGrindCourseRacer *p;
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
        p = &gAirGrindCourse.players[lane];
        lo = AirGrindCourseToLanePos(lane, gAirGrindCourse.scrollPos) / 32;
        hi = AirGrindCourseToLanePos(lane, gAirGrindCourse.unk108) / 32;
        for (k = 0; k < lo - hi; k++)
        {
            j = k + 120;
            col = hi + j;
            t = col / 8;
            c32 = t % 32;
            rem = col % 8;
            AirGrindCalcLanePoint(lane, col, &x, &y, &z);
            AirGrindFindLaneSegment(lane, &p->segmentIndex, col * 32, &flag);
            if (x < -90)
                x = -90;
            if (x > 90)
                x = 90;
            if (rem == 0)
            {
                vp = (u16 *)(BG_VRAM + 0xE000) + (lane * 1024 + c32);
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
                addr = BG_VRAM + tile * 64;
                zero = 0;
                CpuSet(&zero, (void *)addr, 0x010000A0);
            }
            w = AirGrindLaneToCoursePos(lane, col);
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
            vp = (u16 *)(BG_VRAM + 0xE180) + (pos + (x + 80) / 8 * 32);
            addr = *vp * 64 + BG_VRAM;
            addr += rem + ((x + 80) % 8 - h / 2) * 8;
            if (addr < (BG_VRAM + 0x40))
                addr = BG_VRAM + 0x40;
            if (flag == 0 && (col / 2) & 1)
            {
                addr -= 16;
                h += 4;
            }
            gAirGrindStripAddrs[lane][col % 256] = addr;
            gAirGrindStripHeights[lane][col % 256] = h + 1;
            gAirGrindStripOnEvenSegment[lane][col % 256] = flag;
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
        v = AirGrindCourseToLanePos(lane, p->coursePos);
        AirGrindCalcLanePoint(lane, v / 32, &p->screenY, &depth[lane], &p->unk1C);
        p->prevOnEvenSegment = p->onEvenSegment;
        AirGrindFindLaneSegment(lane, &p->segmentIndex, v, &p->onEvenSegment);
        p->segmentEnd = AirGrindFindSegmentEnd(p->segmentIndex, p->coursePos);
        v /= 32;
        p->depth = depth[lane] + 512;
        sn = gUnk_080D059A[p->depth / 4 - 32];
        *gUnk_08757310[lane] = (gAirGrindCourse.unk004 * sn << 8) + 0x300000;
        sn = gUnk_080D059A[p->depth / 4 - 32];
        *gUnk_08757300[lane] += sn * gAirGrindCourse.unk008 << 8;
        old = p->screenX;
        p->screenX = v - lo + 120;
        p->screenY += 128 - *gUnk_08757310[lane] / 65536;
        if (gAirGrindCourse.unk10C != 0)
        {
            if (p->holdingA != 0 || p->prevHoldingA != 0 || p->screenX > 240)
            {
                a = old;
                b = p->screenX;
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
                    if ((p->prevHoldingA != 0 && p->prevOnEvenSegment != 0 && p->holdingA == 0 && p->onEvenSegment == 0)
                        || (p->prevHoldingA == 0 && p->prevOnEvenSegment == 0 && p->holdingA != 0 && p->onEvenSegment != 0))
                        mark = 1;
                    if (p->screenX > 240)
                        mark = 1;
                    for (k = a; k < b; k++)
                    {
                        if (*(u8 *)gAirGrindStripAddrs[lane][k % 256] != 0
                            && (mark == 0 || gAirGrindStripOnEvenSegment[lane][k % 256] != 0))
                        {
                            addr = gAirGrindStripAddrs[lane][k % 256];
                            if (lane != 0)
                                col = 8;
                            else if (gAirGrindStripOnEvenSegment[0][k % 256] != 0)
                                col = 8;
                            else
                                col = 16;
                            if (addr & 1)
                            {
                                addr &= ~1;
                                for (m = 0; m < gAirGrindStripHeights[lane][k % 256]; m++)
                                {
                                    *(vu16 *)addr += col << 8;
                                    addr += 8;
                                }
                            }
                            else
                            {
                                for (m = 0; m < gAirGrindStripHeights[lane][k % 256]; m++)
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
        if (gAirGrindCourse.unk00C <= p->coursePos && p->prevCoursePos <= gAirGrindCourse.finishLine)
        {
            k = p->prevCoursePos;
            m = p->coursePos;
            if (k < gAirGrindCourse.unk00C)
                k = gAirGrindCourse.unk00C;
            if (m > gAirGrindCourse.finishLine)
                m = gAirGrindCourse.finishLine;
            AirGrindCountSegmentBits(k, m, &set, &clear);
            if (p->holdingA != 0)
                p->unk24 += set;
            p->unk20 += set;
        }
        p->prevHoldingA = p->holdingA;
        p->prevCoursePos = p->coursePos;
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
        gAirGrindCourse.players[lane].depthRank = m;
        if (m == 3)
            m = 2;
        *gUnk_08757320[lane] = (*gUnk_08757320[lane] & 0xFFFC) | m;
    }
    gAirGrindCourse.unk108 = gAirGrindCourse.scrollPos;
}
