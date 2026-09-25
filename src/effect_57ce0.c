#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_57ce0.c (0x08057CE0-0x0805880F, issue #89).
 *
 * Task type #7: variants 35-39. */

/* M08's per-player camera positions (src/camera_28b8c.c) */
struct CamPos { u16 x, y; };

extern u32 gUnk_08751D88[];
extern u32 gUnk_08751DB0[];
extern u16 gUnk_0873BAEE[];
extern struct CamPos gUnk_030055D0[4];
extern u8 gUnk_02006178;
extern s8 gUnk_03002444;
extern vs16 gUnk_03004CA0[];
extern vu16 gUnk_03001ED8;              /* DISPCNT shadow */
extern u16 gUnk_0200B000[];
extern u16 gUnk_03001270[];
extern u8 gUnk_0873BAFA[];
extern u8 gUnk_0873C04C[];
extern u32 gUnk_08751DD0[];
extern u32 gUnk_08751DBC[];
extern u16 gUnk_0873BAFC[];
extern u32 gUnk_08751E00[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void sub_080055b0(u8 val, s32 idx);
void sub_08005654(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void sub_080059d8(void);
void sub_080059fc(void);
void sub_08005d9c(void);
void sub_0800617c(s16 a);
void sub_080061c0(s32 a, s32 b);
void sub_08006338(s32 a);
void sub_0800663c(u32 idx);
void sub_08006664(u32 idx);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void sub_0803dfc8(void);
void sub_080670f0(u32 src);
void sub_08058410(void);
void sub_080586fc(void);

void sub_08057ce0(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_08751D88;
    t->unk40 = (((struct Task *)t->unk8C)->unk40 + 0x1800) | 12;
    switch (t->unk18 & 15)
    {
    case 0:
        if (t->unk43 == 1)
            t->unk4C = (t->unk48 + 32) << 16;
        else
            t->unk4C = (t->unk48 - 20) << 16;
        gUnk_03002490->unk43 = 1;
        break;
    case 1:
        if (t->unk43 == 1)
            t->unk4C = (t->unk48 + 22) << 16;
        else
            t->unk4C = (t->unk48 - 32) << 16;
        gUnk_03002490->unk43 = -1;
        break;
    }
    u = gUnk_03002490;
    u->unk50 = u->unk4A << 16;
    if (!(((struct Task *)u->unk8C)->unk7B & 1))
    {
        sub_080061c0(0x30000, -0x2800);
        gUnk_03002490->unk58 = 0;
        gUnk_03002490->unk60 = -0x2000;
        sub_0800617c(0);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
    }
    else
    {
        sub_080061c0(0x18000, -0x1400);
        gUnk_03002490->unk58 = 0;
        gUnk_03002490->unk60 = -0x1000;
        sub_0800617c(0);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
    }
    TaskDispatchTrampoline();
}

void sub_08057e90(void)
{
    struct Task *t;
    struct Task *u;
    u16 *p;
    s32 a;
    s32 b;
    s32 c;
    s32 d;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_08751DB0;
    t->unk40 = ((struct Task *)t->unk8C)->unk40 | 0xF00C;
    if (t->unk43 == 1)
    {
        t->unk4C = (((struct Task *)t->unk8C)->unk48 + 40) << 16;
        t->unk50 = (((struct Task *)t->unk8C)->unk4A + 4) << 16;
    }
    else
    {
        t->unk4C = (((struct Task *)t->unk8C)->unk48 - 40) << 16;
        t->unk50 = (((struct Task *)t->unk8C)->unk4A + 4) << 16;
    }
    p = &gUnk_0873BAEE[(gUnk_03002490->unk18 & 3) * 2];
    a = p[0];
    b = a << 8;
    if (a & 0x8000)
        b |= 0xFF000000;
    sub_080061c0(b, -0x1000);
    u = gUnk_03002490;
    c = p[1];
    d = c << 8;
    if (c & 0x8000)
        d |= 0xFF000000;
    u->unk58 = d;
    u->unk60 = 0;
    u->unk3C = 0;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(5);
    TaskDispatchTrampoline();
}

void sub_08057f90(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *q;
    s16 *p;
    s32 i;
    s32 n;
    s32 m;
    s32 k;
    s32 r;
    u16 x0;
    u16 y0;
    u16 pal;

    t = gUnk_03002490;
    if ((t->unk18 & 15) != 1)
    {
        t->unk00 = 0;
        t->unk0C = 0;
        t->unk04 = (u32)sub_08058410;
        t->unk80 = 7;
        while ((s8)gUnk_03002490->unk88->unk16 == 0)
            TaskYieldTrampoline(1);
        u = gUnk_03002490;
        u->unk28 = gUnk_030055D0[u->unk44].x;
        u->unk2C = gUnk_030055D0[u->unk44].y;
        gUnk_02006178 = 0;
        for (i = 0; i < 20; i++)
            gUnk_0200B000[i] |= 0xFFFF;
        k = 0;
        for (i = 0; i <= 62; i++)
        {
            if (gUnk_03002444 == 0 && gUnk_03004CA0[i] != -1)
            {
                switch (gUnk_03002790[i].unk72)
                {
                case 1:
                case 2:
                case 7:
                case 8:
                    gUnk_0200B000[k++] = i;
                    break;
                }
            }
        }
        m = 0;
        n = 0;
        while ((s16)gUnk_0200B000[n] != -1 && n != 20)
        {
            gUnk_02006178 = 1;
            sub_0800663c((s16)gUnk_0200B000[n++]);
            m++;
        }
        TaskYieldTrampoline(1);
        while (n != 0)
        {
            n--;
            switch (gUnk_03002790[(s16)gUnk_0200B000[n]].unk72)
            {
            case 1:
            case 2:
            case 7:
            case 8:
                sub_08006664((s16)gUnk_0200B000[n]);
                break;
            default:
                gUnk_03002790[(s16)gUnk_0200B000[n]].unk13 = 0;
                sub_08006664((s16)gUnk_0200B000[n]);
                break;
            }
            sub_080055b0(15, (s16)gUnk_0200B000[n]);
        }
        if (m != 0)
            TaskYieldTrampoline(3);
        gUnk_02006178 = 0;
        x0 = gUnk_030055D0[gUnk_03002490->unk44].x - 120;
        y0 = gUnk_030055D0[gUnk_03002490->unk44].y - 80;
        for (i = 32; i <= 62; i++)
        {
            if (gUnk_03002444 != 0)
                continue;
            if (gUnk_03004CA0[i] == -1)
                continue;
            q = &gUnk_03002790[i];
            if (q->unk13 == 0)
                continue;
            if (q->unk0C == 0)
                continue;
            if (q->unk48 >= (s16)x0 && q->unk48 < (s16)x0 + 240
                && q->unk4A >= (s16)y0 && q->unk4A < (s16)y0 + 160)
                gUnk_02006178 = 1;
            switch (gUnk_03002790[i].unk72)
            {
            case 6:
                r = 0;
                if (gUnk_03002790[i].unk76 != 5)
                {
                    sub_0800663c(i);
                    m++;
                    TaskYieldTrampoline(1);
                    r = 1;
                }
                break;
            case 0:
            case 3:
            case 4:
            case 9:
                sub_0800663c(i);
                m++;
                TaskYieldTrampoline(2);
                r = 2;
                break;
            default:
                continue;
            }
            if (r != 0)
            {
                sub_08006664(i);
                sub_080055b0(15, i);
                TaskYieldTrampoline(2);
                gUnk_02006178 = 0;
            }
        }
        if (m == 0)
            TaskYieldTrampoline(1);
        gUnk_03002490->unk88->unk16--;
        gUnk_02006178 = 0;
        TaskDispatchTrampoline();
    }
    pal = gUnk_03001270[0];
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1000;
        sub_080670f0((u32)gUnk_0873BAFA);
        TaskYieldTrampoline(3);
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1D00;
        sub_080670f0((u32)&pal);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1000;
        sub_080670f0((u32)gUnk_0873BAFA);
        TaskYieldTrampoline(1);
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1D00;
        sub_080670f0((u32)&pal);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1D00;
        sub_080670f0((u32)&pal);
        TaskYieldTrampoline(2);
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1000;
        sub_080670f0((u32)gUnk_0873BAFA);
        TaskYieldTrampoline(1);
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1D00;
        sub_080670f0((u32)&pal);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    sub_080670f0((u32)&pal);
    TaskDispatchTrampoline();
}

void sub_08058410(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk88->unk04 != 13)
        sub_08005654(gCurTaskIdx);
    else if ((s8)t->unk88->unk16 != 0)
        sub_0801a828((u8)gCurTaskIdx, t->unk28, t->unk2C, gUnk_0873C04C);
}

void sub_08058460(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    u16 *p;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_080586fc;
    switch (t->unk18 & 15)
    {
    case 0:
    case 1:
    case 2:
        t->unk00 = (u32)sub_080059fc;
        t->unk0C = (u32)sub_08005d9c;
        t->unk42 = 5;
        u = gUnk_03002490;
        u->unk38 = gUnk_08751DD0;
        u->unk40 = (((struct Task *)u->unk8C)->unk40 + 0x1800) | 8;
        if (u->unk43 == 1)
            u->unk4C = 0x60000;
        else
            u->unk4C = -0x60000;
        u->unk50 = -0x20000;
        p = &gUnk_0873BAFC[(gUnk_03002490->unk18 & 15) * 3];
        sub_080061c0((p[0] & 0x8000) ? (p[0] << 8) | 0xFF000000 : p[0] << 8,
                     (p[1] & 0x8000) ? (p[1] << 8) | 0xFF000000 : p[1] << 8);
        v = gUnk_03002490;
        v->unk58 = -0x8000;
        v->unk60 = 0x200;
        v->unk6C = 0;
        do
        {
            sub_0800617c(0);
            TaskYieldTrampoline(6);
            sub_080061c0(0x5A5A5A5A, (p[2] & 0x8000) ? (p[2] << 8) | 0xFF000000 : p[2] << 8);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(4);
            sub_080061c0(0x5A5A5A5A, (p[1] & 0x8000) ? (p[1] << 8) | 0xFF000000 : p[1] << 8);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(6);
            gUnk_03002490->unk3C -= 2;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 1);
        sub_0800617c(6);
        TaskYieldTrampoline(6);
        sub_080061c0(0x5A5A5A5A, (p[2] & 0x8000) ? (p[2] << 8) | 0xFF000000 : p[2] << 8);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(4);
        sub_080061c0(0x5A5A5A5A, (p[1] & 0x8000) ? (p[1] << 8) | 0xFF000000 : p[1] << 8);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C -= 2;
        TaskYieldTrampoline(4);
        break;
    case 3:
        t->unk00 = (u32)sub_080059fc;
        t->unk0C = (u32)sub_0803dfc8;
        t->unk42 = 8;
        u = gUnk_03002490;
        u->unk38 = gUnk_08751DBC;
        u->unk40 = (((struct Task *)u->unk8C)->unk40 + 0x1800) | 12;
        u->unk4C = 0;
        u->unk50 = -0x80000;
        u->unk58 = -0x20000;
        u->unk60 = 0x2000;
        sub_08006338(0);
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk6C = 0;
        do
        {
            sub_08006338(3);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 3);
        break;
    }
    TaskDispatchTrampoline();
}

void sub_080586fc(void)
{
    if (gUnk_03002490->unk88->unk04 != 13)
        sub_08005654(gCurTaskIdx);
}

void sub_08058720(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_08751E00;
    if ((t->unk18 & 15) == 0)
    {
        t->unk43 = 1;
        gUnk_03002490->unk3C = 1;
    }
    else
    {
        t->unk43 = -1;
        gUnk_03002490->unk3C = 0;
    }
    gUnk_03002490->unk50 = (gUnk_03002490->unk4A + 4) << 16;
    sub_080061c0(0x80000, 0x5A5A5A5A);
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    sub_080061c0(0x20000, -0x1000);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C += 2;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}
