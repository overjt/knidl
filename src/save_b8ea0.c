#include "gba/gba.h"
#include "global.h"
#include "task.h"

struct Unk02005E00
{
    /*0x00*/ s32 unk00;
    /*0x04*/ u8 unk04[4];
    /*0x08*/ u8 unk08[4];
};

extern vu16 gPlayerPressedKeys[];    /* per-player keys pressed */
extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern struct Task *gCurTask;
extern void TaskExitTrampoline(void);
extern void HudRedraw(s32 a);
extern void TaskFree(s32 a);
extern void AddPlayerLives(s32 a, s32 b);
extern void HudDrawTiles(u8 *s, s32 a, s32 b, s32 c);
extern void HudClearTiles(s32 a, s32 b, s32 c);
extern void TaskYieldTrampoline(u32 frames);
extern s32 gCurTaskIdx;
extern u16 gUnk_02000010[];
extern struct Unk02005E00 gUnk_02005E00;
extern s16 gPlayerLives[];
extern u32 gBgPalette[];
extern s8 gDigits[];
extern u8 gActivePlayerMask;
extern u16 gLocalPlayer;
extern u16 gPlayerCount;
extern u8 gUnk_085ADD1C[];
extern u8 gUnk_085B09BC[];
extern u8 gUnk_085B09DC[];
extern u8 gUnk_085B0A10[];
extern u8 gUnk_085B0A30[];
extern u8 gUnk_085B0A64[];
extern u8 gUnk_085B0AB0[];
extern u8 gUnk_085B0AD0[];
extern u8 gUnk_085B0AD4[];
extern u8 gUnk_085B0AFC[];
extern u8 gUnk_085B0B00[];
extern u8 gUnk_085B0B08[];
extern u8 gUnk_085B0B10[];
extern u8 gUnk_085B0B28[];
extern u8 gUnk_085B0B5C[];
extern u8 gUnk_085B0BB4[];
extern u8 gUnk_085B0BD4[];
extern u8 gUnk_085B0C2C[];
extern u32 gUnk_0875625C[];
extern u16 gUnk_08756268[];
extern u32 gUnk_08756270[];
extern u32 gUnk_0875628C[];
extern void TaskSetEntry(void *fn, s32 i);
extern s32 IntToDigits(s16 n);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern void TaskSleepForever(void);
extern void HudClearTilemap(void);

void sub_080b8ea0(void);
void sub_080b8ef4(void);
void sub_080b8f8c(s32 a);
void sub_080b8ff0(void);
void sub_080b902c(void);
void sub_080b9064(void);
void sub_080b9090(void);
void sub_080b90c8(void);
void sub_080b90f8(void);
void sub_080b9108(void);
void sub_080b9118(void);
void sub_080b9140(void);
void sub_080b9198(void);
void sub_080b91fc(void);
void sub_080b927c(void);
void sub_080b9344(void);
void sub_080b938c(void);
void sub_080b93d8(void);
s32 sub_080b9424(void);
void sub_080b94b4(s32 a, s32 b);
void sub_080b9578(void);
void sub_080b95ac(void);
void sub_080b95ec(void);
void sub_080b963c(void);
void sub_080b9658(void);
void sub_080b97fc(s32 a);
void sub_080b9878(void);
void sub_080b98c0(void);
void sub_080b9968(s32 a, s32 b);
void sub_080b99e8(s32 a, s32 b, s32 c);
void sub_080b9a88(s32 a);
void sub_080b9b08(s32 a);
void sub_080b9b98(s32 a);
void sub_080b9c28(void);
void sub_080b9c74(void);
void sub_080b9cc0(void);

void sub_080b8ea0(void)
{
    struct Unk02005E00 *s;
    struct Task *t;
    u8 *f;
    u8 *e;
    s32 z;

    s = &gUnk_02005E00;
    t = gCurTask;
    f = (u8 *)s + 4;
    e = &f[t->unk1C];
    z = 0;
    *e = z;
    s->unk00 = z;
}
void sub_080b8ebc(void)
{
    struct Unk02005E00 *s;
    u8 *f;
    u8 *e;
    s32 i;
    s32 v;
    s32 z;

    i = 0;
    s = &gUnk_02005E00;
    f = (u8 *)s + 4;
    z = 0;
    do
    {
        e = (u8 *)(i + (u32)f);
        v = *e << 24;
        if (v != 0 && (u32)v >> 28 == gCurTaskIdx)
        {
            *e = z;
            s->unk00 = z;
        }
        i++;
    } while (i <= 3);
}
void sub_080b8ef4(void)
{
    struct Task *t;
    s32 i;
    s32 n;
    t = gCurTask;
    t->unk00 = 0;
    t->unk0C = 0;
    if (gUnk_02005E00.unk08[gCurTaskIdx] != 0)
    {
        t->unk14 = 6;
    }
    else
    {
        for (i = 0, n = 0; i < 4; i++)
        {
            if (gUnk_02005E00.unk04[i] != 0 && gUnk_02005E00.unk04[i] >> 4 == gCurTaskIdx)
            {
                gCurTask->unk1C = i;
                n++;
            }
        }
        if (n != 0)
        {
            sub_080b8f8c(1);
            if (gPlayerLives[gCurTask->unk1C] > 0)
                gCurTask->unk14 = 2;
            else
                gCurTask->unk14 = 4;
        }
        else
        {
            gCurTask->unk14 = n;
        }
    }
}
void sub_080b8f8c(s32 a)
{
    if (gLocalPlayer == gCurTask->unk88->unk00)
    {
        RequestCopy(2, (u32)gUnk_085ADD1C, (u32)gBgPalette, 64);
        if (a <= 2)
            RequestCopy(1, gUnk_0875625C[a], 192 << 19, gUnk_08756268[a] << 5);
    }
}
void sub_080b8ff0(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk18 = 0;
    if (gLocalPlayer == t->unk88->unk00)
    {
        sub_080b8f8c(0);
        HudClearTilemap();
        sub_080b97fc(gCurTask->unk18);
    }
}
void sub_080b902c(void)
{
    s32 i;

    for (i = 0; i < 4; i++)
        gUnk_02000010[i] |= 0xFFFF;
    gCurTask->unk18 = 0;
    gCurTask->unk24 = 0;
    sub_080b98c0();
}
void sub_080b9064(void)
{
    struct Unk02005E00 *s;

    s = &gUnk_02005E00;
    if (s->unk00 == 0)
        s->unk00 = 1200;
    sub_080b8f8c(1);
    sub_080b9b08(gCurTask->unk1C);
}
void sub_080b9090(void)
{
    AddPlayerLives(-1, gCurTask->unk1C);
    AddPlayerLives(1, gCurTask->unk88->unk00);
    sub_080b8ea0();
    sub_080b9b98(gCurTask->unk1C);
}
void sub_080b90c8(void)
{
    if (gLocalPlayer == gCurTask->unk88->unk00)
        HudRedraw(gLocalPlayer);
    TaskExitTrampoline();
}
void sub_080b90f8(void)
{
    sub_080b8ea0();
    sub_080b9c28();
}
void sub_080b9108(void)
{
    sub_080b8f8c(2);
    sub_080b9c74();
}
void sub_080b9118(void)
{
    struct Unk02005E00 *s;
    vs32 *ip;
    u8 *g;

    sub_080b8f8c(2);
    sub_080b9cc0();
    s = &gUnk_02005E00;
    ip = &gCurTaskIdx;
    g = (u8 *)s + 8;
    g[*ip] = 1;
}
void sub_080b9140(void)
{
    vu16 *k;
    vu16 *e;
    struct Task *t;

    k = gPlayerPressedKeys;
    t = gCurTask;
    e = &k[t->unk88->unk00];
    if ((*e & 0xC0) != 0)
    {
        if ((*e & 0x40) != 0)
        {
            if (t->unk18 == 1)
            {
                t->unk18 = 0;
                sub_080b97fc(0);
            }
        }
        else if (t->unk18 == 0)
        {
            t->unk18 = 1;
            sub_080b97fc(1);
        }
    }
}
void sub_080b9198(void)
{
    vu16 *k;
    struct Task *t;

    k = gPlayerPressedKeys;
    t = gCurTask;
    if ((k[t->unk88->unk00] & 1) != 0)
    {
        if (t->unk18 == 0)
        {
            if (sub_080b9424() != 0)
                gCurTask->unk14 = 1;
            else
                gCurTask->unk14 = 5;
        }
        else
        {
            t->unk14 = 6;
        }
        TaskSetEntry(sub_080b9658, gCurTaskIdx);
    }
}
void sub_080b91fc(void)
{
    vu16 *k;
    vu16 *e;
    struct Task *t;

    if (gCurTask->unk18 > gCurTask->unk20)
    {
        do
        {
            gCurTask->unk18--;
            sub_080b9a88(gCurTask->unk18);
        } while (gCurTask->unk18 > gCurTask->unk20);
    }
    k = gPlayerPressedKeys;
    t = gCurTask;
    e = &k[t->unk88->unk00];
    if ((*e & 0xC0) != 0)
    {
        if ((*e & 0x40) != 0)
        {
            if (t->unk18 != 0)
            {
                t->unk18--;
                sub_080b9a88(t->unk18);
            }
        }
        else if (t->unk18 < t->unk20)
        {
            t->unk18++;
            sub_080b9a88(t->unk18);
        }
    }
}
void sub_080b927c(void)
{
    vu16 *k;
    vu16 *e;
    struct Task *t;
    s32 i;
    s32 j;

    k = gPlayerPressedKeys;
    t = gCurTask;
    e = &k[t->unk88->unk00];
    if ((*e & 11) != 0)
    {
        if ((*e & 1) != 0)
        {
            for (i = 0, j = 0; i < gPlayerCount; i++)
            {
                if (i == gCurTaskIdx)
                    continue;
                if ((1 & (gCurTask->unk24 >> i)) == 0)
                    continue;
                if (j == gCurTask->unk18)
                {
                    gCurTask->unk1C = i;
                    break;
                }
                j++;
            }
            gUnk_02005E00.unk04[gCurTask->unk1C] = (gCurTaskIdx << 4) | 1;
            gCurTask->unk14 = 2;
        }
        else
        {
            t->unk14 = 0;
        }
    }
}
void sub_080b9344(void)
{
    vu16 *k;
    s32 p;

    k = gPlayerPressedKeys;
    p = gCurTask->unk88->unk00;
    if ((k[p] & 1) != 0)
    {
        if (gLocalPlayer == p)
            HudRedraw(p);
        TaskFree(gCurTaskIdx);
    }
}
void sub_080b938c(void)
{
    vu16 *k;
    struct Task *t;

    k = gPlayerPressedKeys;
    t = gCurTask;
    if ((k[t->unk88->unk00] & 1) != 0)
        t->unk14 = 0;
    if (gCurTask->unk14 != 4)
        TaskSetEntry(sub_080b9658, gCurTaskIdx);
}
void sub_080b93d8(void)
{
    vu16 *k;
    struct Task *t;

    k = gPlayerPressedKeys;
    t = gCurTask;
    if ((k[t->unk88->unk00] & 1) != 0)
        t->unk14 = 0;
    if (gCurTask->unk14 != 5)
        TaskSetEntry(sub_080b9658, gCurTaskIdx);
}
s32 sub_080b9424(void)
{
    s32 i;
    s32 n;
    u8 *f;

    gCurTask->unk24 = 0;
    for (i = 0, n = 0; i < gPlayerCount; i++)
    {
        if ((1 & (gActivePlayerMask >> i)) != 0 && gPlayerLives[i] > 0 && gUnk_02005E00.unk04[i] == 0)
        {
            n++;
            gCurTask->unk24 |= 1 << i;
        }
    }
    gCurTask->unk20 = n - 1;
    return n;
}
void sub_080b94b4(s32 a, s32 b)
{
    struct Task *t;
    s32 i;
    s32 j;
    s32 v;
    u16 u;
    u8 *f;
    s16 *e;
    s16 *q;

    t = gCurTask;
    if (a != t->unk20 || b != t->unk24)
    {
        sub_080b98c0();
    }
    else
    {
        for (i = 0, j = 0; i < gPlayerCount; i++)
        {
            if ((1 & (gActivePlayerMask >> i)) != 0 && gUnk_02005E00.unk04[i] == 0)
            {
                e = &gPlayerLives[i];
                u = *e;
                v = *e;
                if (v > 0)
                {
                    if (((gCurTask->unk24 >> i) & 1) != 0)
                    {
                        q = (s16 *)&gUnk_02000010[i];
                        if (*q != v)
                            *q = u;
                    }
                    sub_080b99e8(i, j, gPlayerLives[i]);
                    j++;
                }
            }
        }
    }
}
void sub_080b9578(void)
{
    struct Task *t;
    u8 v;
    u8 *p;

    p = (u8 *)&gUnk_02005E00;
    t = gCurTask;
    p += 4;
    v = p[t->unk1C];
    if ((v & 2) != 0)
    {
        if ((v >> 4) == gCurTaskIdx)
            t->unk14 = 3;
    }
}
void sub_080b95ac(void)
{
    if (gUnk_02005E00.unk00 <= 0)
        gCurTask->unk14 = 4;
    gUnk_02005E00.unk00--;
    if (gCurTask->unk14 != 2)
        TaskSetEntry(sub_080b9658, gCurTaskIdx);
}
void sub_080b95ec(void)
{
    struct Task *t;
    s16 *p;

    p = gPlayerLives;
    t = gCurTask;
    if (p[t->unk1C] <= 0)
        t->unk14 = 4;
}
void sub_080b9610(void)
{
    gCurTask->unk04 = (u32)sub_080b963c;
    sub_080b8ef4();
    CallTableEntry(gCurTask->unk14, 7, gUnk_08756270);
}
void sub_080b963c(void)
{
    CallTableEntry(gCurTask->unk15, 7, gUnk_0875628C);
}
void sub_080b9658(void)
{
    CallTableEntry(gCurTask->unk14, 7, gUnk_08756270);
}
void sub_080b9674(void)
{
    gCurTask->unk15 = 0;
    sub_080b8ff0();
    TaskSleepForever();
}
void sub_080b9690(void)
{
    sub_080b9140();
    sub_080b9198();
}
void sub_080b96a0(void)
{
    gCurTask->unk15 = 1;
    sub_080b902c();
    TaskSleepForever();
}
void sub_080b96bc(void)
{
    s32 a;
    s32 b;

    a = gCurTask->unk20;
    b = gCurTask->unk24;
    if (sub_080b9424() != 0)
    {
        sub_080b94b4(a, b);
        sub_080b91fc();
        sub_080b927c();
    }
    else
    {
        gCurTask->unk14 = 5;
    }
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_080b9658, gCurTaskIdx);
}
void sub_080b9710(void)
{
    gCurTask->unk15 = 2;
    sub_080b9064();
    while (1)
    {
        TaskYieldTrampoline(4);
        sub_080b95ec();
    }
}
void sub_080b9730(void)
{
    sub_080b9578();
    sub_080b95ac();
}
void sub_080b9740(void)
{
    gCurTask->unk15 = 3;
    sub_080b9090();
    TaskYieldTrampoline(180);
    sub_080b90c8();
    TaskSleepForever();
}
void sub_080b9764(void)
{
    sub_080b9344();
}
void sub_080b9770(void)
{
    gCurTask->unk15 = 4;
    sub_080b90f8();
    TaskYieldTrampoline(180);
    gCurTask->unk14 = 0;
    TaskSleepForever();
}
void sub_080b9798(void)
{
    sub_080b938c();
}
void sub_080b97a4(void)
{
    gCurTask->unk15 = 5;
    sub_080b9108();
    TaskYieldTrampoline(300);
    gCurTask->unk14 = 0;
    TaskSleepForever();
}
void sub_080b97d0(void)
{
    sub_080b93d8();
}
void sub_080b97dc(void)
{
    gCurTask->unk15 = 6;
    sub_080b9118();
    TaskSleepForever();
}
void sub_080b97f8(void)
{
}
void sub_080b97fc(s32 a)
{
    u8 *p;
    u8 *q;

    if (gLocalPlayer == gCurTask->unk88->unk00)
    {
        if (a == 0)
        {
            p = gUnk_085B09BC;
            q = gUnk_085B09DC;
        }
        else
        {
            p = gUnk_085B0A10;
            q = gUnk_085B0A30;
        }
        HudDrawTiles(p, 1, 2, 8);
        HudDrawTiles(p + 16, 1, 3, 8);
        HudDrawTiles(q, 1, 4, 13);
        HudDrawTiles(q + 26, 1, 5, 13);
    }
}
void sub_080b9878(void)
{
    u8 *p;

    if (gLocalPlayer == gCurTask->unk88->unk00)
    {
        p = gUnk_085B0A64;
        HudDrawTiles(p, 1, 2, 19);
        p += 38;
        HudDrawTiles(p, 1, 3, 19);
    }
}
void sub_080b98c0(void)
{
    s32 i;
    s32 j;
    s16 *e;
    s16 *pe;
    u8 *f;
    u16 v;

    if (gLocalPlayer == gCurTask->unk88->unk00)
    {
        HudClearTilemap();
        for (i = 0, j = 0; i < gPlayerCount; i++)
        {
            f = gUnk_02005E00.unk04;
            if (((gActivePlayerMask >> i) & 1) != 0)
            {
                pe = gPlayerLives;
                e = &pe[i];
                v = *e;
                if (*e > 0 && f[i] == 0)
                {
                    gUnk_02000010[i] = v;
                    sub_080b9968(i, j);
                    sub_080b99e8(i, j, *e);
                    j++;
                }
            }
        }
        sub_080b9878();
        sub_080b9a88(gCurTask->unk18);
    }
}
void sub_080b9968(s32 a, s32 b)
{
    u8 *p;
    u8 *q;

    s32 off;

    if (gLocalPlayer == gCurTask->unk88->unk00)
    {
        off = a * 4;
        p = gUnk_085B0AB0;
        HudDrawTiles(p + off, 3, b * 2 + 4, 2);
        p += 16;
        HudDrawTiles(p + off, 3, b * 2 + 5, 2);
        q = gUnk_085B0AD0;
        HudDrawTiles(q, 5, b * 2 + 4, 1);
        q += 2;
        HudDrawTiles(q, 5, b * 2 + 5, 1);
    }
}
void sub_080b99e8(s32 a, s32 b, s32 c)
{
    if (gLocalPlayer == gCurTask->unk88->unk00)
    {
        IntToDigits(c);
        HudDrawTiles(&gUnk_085B0AD4[gDigits[1] * 2], 6, b * 2 + 4, 1);
        HudDrawTiles(&gUnk_085B0AD4[20 + gDigits[1] * 2], 6, b * 2 + 5, 1);
        HudDrawTiles(&gUnk_085B0AD4[gDigits[0] * 2], 7, b * 2 + 4, 1);
        HudDrawTiles(&gUnk_085B0AD4[20 + gDigits[0] * 2], 7, b * 2 + 5, 1);
    }
}
void sub_080b9a88(s32 a)
{
    s32 i;
    u8 *p;

    if (gLocalPlayer == gCurTask->unk88->unk00)
    {
        for (i = 0; i < gPlayerCount; i++)
        {
            HudClearTiles(2, i * 2 + 4, 1);
            HudClearTiles(2, i * 2 + 5, 1);
        }
        p = gUnk_085B0AFC;
        HudDrawTiles(p, 2, a * 2 + 4, 1);
        p += 2;
        HudDrawTiles(p, 2, a * 2 + 5, 1);
    }
}
void sub_080b9b08(s32 a)
{
    u8 *p;
    u8 *q;
    u8 *r;

    if (gLocalPlayer == gCurTask->unk88->unk00)
    {
        HudClearTilemap();
        p = gUnk_085B0B00;
        HudDrawTiles(p, 13, 6, 2);
        p += 4;
        HudDrawTiles(p, 13, 7, 2);
        a *= 4;
        q = gUnk_085B0BB4;
        HudDrawTiles(q + a, 16, 8, 2);
        q += 16;
        HudDrawTiles(q + a, 16, 9, 2);
        r = gUnk_085B0B10;
        HudDrawTiles(r, 10, 8, 6);
        r += 12;
        HudDrawTiles(r, 10, 9, 6);
    }
}
void sub_080b9b98(s32 a)
{
    u8 *p;
    u8 *q;
    u8 *r;

    if (gLocalPlayer == gCurTask->unk88->unk00)
    {
        HudClearTilemap();
        p = gUnk_085B0B08;
        HudDrawTiles(p, 13, 6, 2);
        p += 4;
        HudDrawTiles(p, 13, 7, 2);
        a *= 4;
        q = gUnk_085B0BB4;
        HudDrawTiles(q + a, 19, 8, 2);
        q += 16;
        HudDrawTiles(q + a, 19, 9, 2);
        r = gUnk_085B0B28;
        HudDrawTiles(r, 6, 8, 13);
        r += 26;
        HudDrawTiles(r, 6, 9, 13);
    }
}
void sub_080b9c28(void)
{
    u8 *p;

    if (gLocalPlayer == gCurTask->unk88->unk00)
    {
        HudClearTilemap();
        p = gUnk_085B0B5C;
        HudDrawTiles(p, 4, 8, 22);
        p += 44;
        HudDrawTiles(p, 4, 9, 22);
    }
}
void sub_080b9c74(void)
{
    u8 *p;

    if (gLocalPlayer == gCurTask->unk88->unk00)
    {
        HudClearTilemap();
        p = gUnk_085B0BD4;
        HudDrawTiles(p, 4, 8, 22);
        p += 44;
        HudDrawTiles(p, 4, 9, 22);
    }
}
void sub_080b9cc0(void)
{
    u8 *p;

    if (gLocalPlayer == gCurTask->unk88->unk00)
    {
        HudClearTilemap();
        p = gUnk_085B0C2C;
        HudDrawTiles(p, 8, 8, 14);
        p += 28;
        HudDrawTiles(p, 8, 9, 14);
    }
}
