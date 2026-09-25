#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* effect_5a358.c (0x0805A358-0x0805AFAB, issue #89).
 *
 * Task type #7 (the player's effect objects, see src/effect_53af4.c):
 * variants 45-48.  Variant 45 (sub_0805a358, M13) rides on its spawner
 * through three sub-states (gUnk_08751FCC); sub_0805a508 kills it once the
 * player leaves mode 13.  Variant 46 (sub_0805a52c, M13) is the twin of
 * variant 37 (src/effect_57ce0.c): the same stop and release of the tasks of
 * kinds 1, 2, 7 and 8 through gUnk_0200B000 and the task skip mask, around a
 * palette effect - it saves the palette buffer (CpuSet of gUnk_03001570 into
 * gUnk_0200AF20) and calls M11's sub_0803f834/ sub_0803e3e4, M17's
 * sub_08065e6c and a VRAM transfer.  Its callbacks are sub_0805ab04, which
 * blends the saved palette towards gUnk_0873BC3E (while gUnk_03002444 is
 * set) or gUnk_0873BB7E with sub_08003014 (80 or 96 colours by
 * gUnk_02007D64), raising the ratio Task.unk2C by 10 up to 0x100 in state 1
 * (with the collider row gUnk_0873C2B4 at the player's camera position
 * gUnk_030055D0) and lowering it by 46 to 0 in state 2 (then calling M17's
 * sub_08065ed0), and the draw hook sub_0805ac50.  Variant 47 (sub_0805acec,
 * M13) is an animation in world space; its callback sub_0805ae00 clears
 * Task.unk28 in sub-state 0 when the player leaves mode 13 or the spawner's
 * Task.unk73 is not 4 (and copies the spawner's facing), and in the other
 * sub-states tests the block hit-box set gUnk_0873CF8C (sub_08030804) at the
 * spawner's position offset by PlayerState.unk24/unk26 (8.8).  Variant 48
 * (sub_0805ae94, M14's action 55) rides on its spawner with the draw hook
 * sub_0805af80 (shared with variant 34: M11's sub_0803dfc8 in player mode
 * 13, otherwise the task dies) and the callback sub_0805af44, which kills it
 * once the player leaves mode 13 or releases both A and B. */

/* M08's per-player camera positions (src/camera_28b8c.c) */
struct CamPos { u16 x, y; };

extern u32 gUnk_08751FCC[];
extern struct CamPos gUnk_030055D0[4];
extern s16 gUnk_03002348;               /* scalar, read with ldrsh (33 landed files) */
extern s16 gUnk_030023E4;               /* scalar, read with ldrsh (32 landed files) */
extern s8 gUnk_03002444;
extern u8 gUnk_02006178;
extern vs16 gUnk_03004CA0[];
extern u32 gUnk_08752020[];
extern u8 gUnk_082030D8[];
extern u8 gUnk_0873BB3E[];
extern s8 gUnk_02007D64;
extern u16 gUnk_02007F60[];
extern u16 gUnk_03001570[];
extern u16 gUnk_0200AF20[];
extern u16 gUnk_0200B000[];             /* 20 task indices, 0xFFFF = empty; non-volatile, signed reads cast (s16) (variant 37 in effect_57ce0.c) */
extern vs16 gUnk_03000FB8;
extern u8 gUnk_03001F34;
extern u16 gUnk_0873BC3E[];
extern u16 gUnk_0873BB7E[];
extern u8 gUnk_0873C2B4[];
extern s16 gUnk_0873BB26[];
extern u32 gUnk_0874C600[];
extern u32 gUnk_0873CF8C[];             /* hit-box set, passed as (struct HitBoxSet *) */
extern u32 gUnk_08752090[];
extern u16 gUnk_03002458[];

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
u32 sub_080008e8(u16 steps, s16 delta, u16 *mask);   /* callers pass -4 as movs/negs (src/player_47fe8.c spells it s16 too) */
void sub_080017e4(u32 mode, void *src, void *dst, u32 size);   /* early_1518; effect_5afac's pointer spelling */
s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);   /* callers pass f sign-extended (lsls/asrs #16); the early_1518 definition says u16 */
s32 sub_08001cc8(s32 a, s16 b, s16 c, s32 d);
void sub_08003014(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void sub_080055b0(u8 val, s32 idx);
void sub_08005654(s32 id);                         /* kill task (M09+ spelling, 49 landed files) */
void sub_080059d8(void);
void sub_080059fc(void);
void sub_08005d9c(void);
void sub_08005ea8(void);
void sub_0800617c(s16 a);
void sub_080061c0(s32 a, s32 b);
void sub_080062c4(void);
u32 sub_0800641c(s16 a, s16 b);   /* the ROM tests r0 unnarrowed (src callers spell u32) */
u32 sub_08006464(s16 x, s16 y);
void sub_0800663c(u32 idx);
void sub_08006664(u32 idx);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u16 sub_08030804(struct HitBoxSet *p, s32 x, s32 y, s32 e);
void sub_0803dfc8(void);
void sub_0803e3e4(s32 a0);
void sub_0803f834(u16 a0, void *src);
void sub_08065e6c(void);
void sub_08065ed0(void);
void sub_0805a508(void);
void sub_0805ab04(void);
void sub_0805ac50(void);
void sub_0805ae00(void);
void sub_0805af44(void);
void sub_0805af80(void);

void sub_0805a358(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk00 = (u32)sub_080059fc;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk04 = (u32)sub_0805a508;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_08751FCC;
    switch (t->unk18 & 15)
    {
    case 0:
        t->unk4C = 0;
        t->unk50 = -0x80000;
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C = 0;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 10);
        u = gUnk_03002490;
        u->unk4C = 0;
        u->unk50 = -0x180000;
        u->unk3C = 0;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 10);
        break;
    case 1:
        t->unk4C = 0;
        t->unk50 = 0;
        t->unk3C = 0;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 10);
        u = gUnk_03002490;
        u->unk4C = 0;
        u->unk50 = -0x100000;
        u->unk3C = 0;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 10);
        break;
    case 2:
        t->unk4C = 0;
        t->unk50 = -0x180000;
        t->unk3C = 0xFFFF;
        TaskYieldTrampoline(20);
        gUnk_03002490->unk3C = 14;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 5);
        break;
    }
    TaskDispatchTrampoline();
}

void sub_0805a508(void)
{
    if (gUnk_03002490->unk88->unk04 != 13)
        sub_08005654(gCurTaskIdx);
}

void sub_0805a52c(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *q;
    s32 i;
    s32 n;
    s32 k;
    s32 o;
    s32 m;
    s32 r;
    u16 x0;
    u16 y0;

    switch (t->unk18 & 15)
    {
    case 0:
        t->unk00 = (u32)sub_080059fc;
        t->unk0C = (u32)sub_08005ea8;
        t->unk42 = 8;
        u = gUnk_03002490;
        u->unk38 = gUnk_08752020;
        u->unk40 = ((struct Task *)u->unk8C)->unk40 | 0xF008;
        u->unk4C = 0;
        u->unk50 = 0;
        u->unk3C = 0xFFFF;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk3C = 6;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 3;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = -1;
        TaskYieldTrampoline(12);
        if (gUnk_03002490->unk4A <= 60)
        {
            gUnk_03002490->unk3C = 9;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C |= -1;
            TaskYieldTrampoline(12);
            gUnk_03002490->unk3C = 10;
            TaskYieldTrampoline(3);
        }
        else
        {
            gUnk_03002490->unk3C = 1;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C |= -1;
            TaskYieldTrampoline(12);
            gUnk_03002490->unk3C = 2;
            TaskYieldTrampoline(3);
        }
        o = (((struct Task *)gUnk_03002490->unk8C)->unk40 & 0x7FF) << 5;
        sub_080017e4(1, gUnk_082030D8, (void *)(o + 0x06010080), 0x180);
        sub_080017e4(1, gUnk_082030D8 + 0x180, (void *)(o + 0x06010480), 0x180);
        sub_080017e4(1, gUnk_082030D8 + 0x300, (void *)(o + 0x06010880), 0x180);
        sub_080017e4(1, gUnk_082030D8 + 0x480, (void *)(o + 0x06010C80), 0x180);
        break;
    case 1:
        t->unk00 = 0;
        t->unk0C = (u32)sub_0805ac50;
        t->unk04 = (u32)sub_0805ab04;
        t->unk42 = 15;
        w = gUnk_03002490;
        w->unk38 = gUnk_08752020;
        w->unk40 = (((struct Task *)w->unk8C)->unk40 + 0x800) | 4;
        x0 = gUnk_030055D0[w->unk44].x;
        x0 -= 120;
        y0 = gUnk_030055D0[w->unk44].y - 80;
        w->unk48 = ((struct Task *)w->unk8C)->unk48 - gUnk_03002348;
        w->unk4A = ((struct Task *)w->unk8C)->unk4A - gUnk_030023E4;
        w->unk4C = ((struct Task *)w->unk8C)->unk48;
        w->unk50 = ((struct Task *)w->unk8C)->unk4A;
        sub_0803f834(w->unk88->unk00, gUnk_0873BB3E);
        if (gUnk_02007D64 == 2 || gUnk_02007D64 == 3)
            gUnk_02007F60[29] = 0;
        v = gUnk_03002490;
        v->unk28 = 0;
        v->unk2C = 0;
        sub_08065e6c();
        CpuSet(gUnk_03001570, gUnk_0200AF20, 96);
        gUnk_03002490->unk3C = 0;
        gUnk_03002490->unk6C = 0;
        do
        {
            if ((s16)gUnk_03002490->unk6C == 10)
                sub_080008e8(6, 5, gUnk_02007F60);
            if ((s16)gUnk_03002490->unk6C == 4)
                gUnk_03002490->unk28 = 1;
            TaskYieldTrampoline(4);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 10);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        gUnk_02006178 = 0;
        for (i = 0; i < 20; i++)
            gUnk_0200B000[i] |= -1;
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
            case 5:
                r = 0;
                if (gUnk_03002790[i].unk76 == 2)
                {
                    sub_0800663c(i);
                    TaskYieldTrampoline(1);
                    r = 1;
                }
                break;
            case 6:
                r = 0;
                if (gUnk_03002790[i].unk76 != 5)
                {
                    sub_0800663c(i);
                    TaskYieldTrampoline(1);
                    r = 1;
                }
                break;
            case 0:
            case 3:
            case 4:
            case 9:
                sub_0800663c(i);
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
        gUnk_02006178 = 0;
        gUnk_03002490->unk88->unk16 = 0;
        while (gUnk_03002490->unk88->unk68 == 0)
            TaskYieldTrampoline(1);
        TaskYieldTrampoline(10);
        gUnk_03002490->unk28 = 2;
        sub_080008e8(7, -4, gUnk_02007F60);
        TaskYieldTrampoline(8);
        gUnk_03000FB8 = 0;
        sub_0803e3e4(0);
        gUnk_03001F34 = 0;
        break;
    }
    TaskDispatchTrampoline();
}

void sub_0805ab04(void)
{
    s32 n;
    struct Task *t;
    struct Task *u;

    n = 6;
    if (gUnk_02007D64 == 2 || gUnk_02007D64 == 3)
        n = 5;
    switch (gUnk_03002490->unk28)
    {
    case 0:
        break;
    case 1:
        if (gUnk_03002444 != 0)
            sub_08003014(gUnk_0200AF20, gUnk_0873BC3E, (u16)gUnk_03002490->unk2C, n << 4, gUnk_03001570);
        else
            sub_08003014(gUnk_0200AF20, gUnk_0873BB7E, (u16)gUnk_03002490->unk2C, n << 4, gUnk_03001570);
        t = gUnk_03002490;
        if (t->unk2C != 0x100)
        {
            t->unk2C += 10;
            if (t->unk2C > 0x100)
                t->unk2C = 0x100;
        }
        sub_0801a828((u8)gCurTaskIdx, gUnk_030055D0[gUnk_03002490->unk88->unk00].x,
                     gUnk_030055D0[gUnk_03002490->unk88->unk00].y, gUnk_0873C2B4);
        break;
    case 2:
        if (gUnk_03002444 != 0)
            sub_08003014(gUnk_0200AF20, gUnk_0873BC3E, (u16)gUnk_03002490->unk2C, n << 4, gUnk_03001570);
        else
            sub_08003014(gUnk_0200AF20, gUnk_0873BB7E, (u16)gUnk_03002490->unk2C, n << 4, gUnk_03001570);
        u = gUnk_03002490;
        if (u->unk2C != 0)
        {
            u->unk2C -= 46;
            if (u->unk2C < 0)
                u->unk2C = 0;
        }
        else
        {
            u->unk28 = 0;
            sub_08065ed0();
        }
        break;
    }
}

void sub_0805ac50(void)
{
    struct Task *t;
    s32 g;
    s16 x;

    if (gUnk_03002490->unk3C != -1 && sub_08006464(gUnk_03002490->unk4C, gUnk_03002490->unk50)
        && sub_0800641c(gUnk_03002490->unk4C, gUnk_03002490->unk50))
    {
        x = gUnk_0873BB26[(s16)gUnk_03002490->unk6C];
        g = sub_08001cc8(*gUnk_03002490->unk38, x, x, 0);
        t = gUnk_03002490;
        sub_08001a94(t->unk42, g, t->unk3E, t->unk40, t->unk48, t->unk4A);
    }
}

void sub_0805acec(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    if ((t->unk18 & 15) == 0)
    {
        t->unk00 = (u32)sub_080059d8;
        t->unk0C = (u32)sub_08005d9c;
        t->unk04 = (u32)sub_0805ae00;
        t->unk42 = 5;
        u = gUnk_03002490;
        u->unk38 = gUnk_0874C600;
        u->unk28 = 1;
        do
        {
            v = gUnk_03002490;
            if (((struct Task *)v->unk8C)->unk7A != 0)
            {
                v->unk50 = (((struct Task *)v->unk8C)->unk4A + 10) << 16;
                if (v->unk43 == 1)
                    v->unk4C = (((struct Task *)v->unk8C)->unk48 - 8) << 16;
                else
                    v->unk4C = (((struct Task *)v->unk8C)->unk48 + 8) << 16;
                sub_080062c4();
                sub_080061c0(-0x30000, 0x6000);
                gUnk_03002490->unk58 = -0x20000;
                gUnk_03002490->unk60 = 0x4000;
                sub_0800617c(0);
                TaskYieldTrampoline(4);
                sub_0800617c(10);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C += 2;
                TaskYieldTrampoline(2);
            }
            else
            {
                v->unk3C = 0xFFFF;
                TaskYieldTrampoline(1);
            }
        } while (gUnk_03002490->unk28 != 0);
    }
    else
    {
        t->unk00 = 0;
        t->unk0C = 0;
        t->unk08 = (u32)sub_0805ae00;
        TaskYieldTrampoline(6);
    }
    TaskDispatchTrampoline();
}

void sub_0805ae00(void)
{
    struct Task *t = gUnk_03002490;
    s32 s = t->unk18 & 15;

    if (s == 0)
    {
        if (t->unk28 != 0 && (t->unk88->unk04 != 13 || ((struct Task *)t->unk8C)->unk73 != 4))
            t->unk28 = 0;
        gUnk_03002490->unk43 = ((struct Task *)gUnk_03002490->unk8C)->unk43;
    }
    else
    {
        sub_08030804((struct HitBoxSet *)gUnk_0873CF8C,
                     ((struct Task *)t->unk8C)->unk48 + ((s16)t->unk88->unk24 >> 8),
                     ((struct Task *)t->unk8C)->unk4A + ((s16)t->unk88->unk26 >> 8), t->unk44);
    }
}

void sub_0805ae94(void)
{
    struct Task *t;

    gUnk_03002490->unk00 = (u32)sub_080059fc;
    gUnk_03002490->unk0C = (u32)sub_0805af80;
    gUnk_03002490->unk04 = (u32)sub_0805af44;
    gUnk_03002490->unk42 = 5;
    t = gUnk_03002490;
    t->unk38 = gUnk_08752090;
    t->unk40 = ((struct Task *)t->unk8C)->unk40 | 0xF004;
    t->unk4C = 0;
    t->unk50 = 0;
    t->unk3C = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    TaskDispatchTrampoline();
}

void sub_0805af44(void)
{
    struct PlayerState *p = gUnk_03002490->unk88;

    if (p->unk04 != 13 || !(gUnk_03002458[p->unk00] & 3))
        sub_08005654(gCurTaskIdx);
}

void sub_0805af80(void)
{
    if (gUnk_03002490->unk88->unk04 != 13)
        sub_08005654(gCurTaskIdx);
    else
        sub_0803dfc8();
}
