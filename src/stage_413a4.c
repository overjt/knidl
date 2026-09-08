#include "gba/gba.h"
#include "global.h"
#include "task.h"

struct M11R8 { u8 unk00; u8 unk01; u8 unk02; u8 unk03; u8 *unk04; };
struct M11Buf { u8 unk00[4]; u8 unk04[4]; };
struct M11R20 { u32 w[5]; };

extern u8 gUnk_020055E8;
extern u8 gUnk_0200AF00;
extern vu16 gUnk_03001E90;
extern vu16 gUnk_03001EA4;
extern vu16 gUnk_03001ED8;
extern u8 gUnk_03001F34;
extern u8 gUnk_03002340;
extern s16 gUnk_03002348;
extern u8 gUnk_03002350;
extern u16 gUnk_030023C0[];
extern s16 gUnk_030023E4;
extern u8 gUnk_03002438;
extern s8 gUnk_03002444;
extern u16 gUnk_03002458[];
extern struct Task *gUnk_03002490;
extern u8 gUnk_03005550[];
extern u32 gUnk_0873B204[];
extern u32 gUnk_0873BD28[];
extern u32 gUnk_0873CA68[];
extern u32 gUnk_0873CB1C[];
extern u32 gUnk_0873CB24[];
extern u32 gUnk_0873D03C[];
extern s16 gUnk_0873D206[];
extern s16 gUnk_0873D5C0[];
extern u32 gUnk_0873D986[];

void TaskYieldTrampoline(s32 frames);
u32 sub_08002ee8(u32 range);
s32 sub_08003110(s32 songId);
s32 sub_080031b8(s32 id);
void sub_08003484(void);
void sub_08003688(void);
void sub_080055b0(u8 val, s32 idx);
void sub_08005654(s32 id);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_0800617c(s16 a);
void sub_08006338(s32 a);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);
void sub_08021c74(s32 a, s32 b);
void sub_080224b0(void);
s32 sub_08022624(u16 a, u16 b);
s32 sub_08022760(struct Task *t);
void sub_08025024(void);
void sub_080261d4(u32 a);
s32 sub_080264b0(void);
void sub_0802651c(s32 a);
s32 sub_0802653c(void);
void sub_08026584(void);
void sub_08026704(s32 a);
void sub_0802672c(void);
void sub_08027204(u32 a);
void sub_08027548(void);
void sub_080276ac(s32 a);
void sub_08027a60(void);
void sub_0803cd60(void);
void sub_0803e050(s32 a0);
void sub_0803e080(void);
void sub_0803e1b8(s32 a0, s32 a1, s32 a2);
void sub_0803e374(s32 a, u16 b);
void sub_0803e3e4(s32 a0);
void sub_0803e4a8(void);
s32 sub_0803e4ec(s32 a0);
void sub_0803e650(s32 a0);
void sub_0803f870(void);
void sub_0803f8e8(void);
void sub_0803f9c0(void);
s32 sub_0803f9e8(void);
s32 sub_0803fce4(s32 a);
s32 sub_0803fd20(s32 a0);
s32 sub_0803fd90(void);
s32 sub_0803fdf4(void);
s32 sub_0803fe68(void);
s32 sub_0803fec4(void);
s32 sub_0803ff7c(void);
s32 sub_080400c0(void);
s32 sub_08040264(void);
s32 sub_08040298(void);
s32 sub_0804042c(void);
void sub_08040710(void);
void sub_08040b40(s32 a0, s32 a1);
void sub_08042c50(void);
s32 sub_0805afac(s32 band, s32 id, s32 payload);

/* gUnk_0873B204 is a table of 8-byte records; the three used halfwords are
   signed 8.8 velocities for Task.unk58 / unk60 / unk68, and 0x9999 is the
   "leave this axis alone" sentinel.  Field 0's sign bit additionally clears
   Task.unk7A.  Same unpack as sub_08040b40's case 10 (which writes
   unk54/unk5C/unk64), except that block 1 needs its own sentinel local: the
   `u16 c` truncation keeps cse2 from folding the copy into the shift, and the
   separate result `s` lets the loaded value die at that copy so the shifted
   result can reuse its register. */
void sub_080413a4(s32 a0)
{
    u16 *e = (u16 *)gUnk_0873B204 + a0 * 4;
    s32 v = e[0];

    if (v != 0x9999)
    {
        struct Task *t = gUnk_03002490;
        u16 c = v;
        s32 s = c << 8;

        if (c & 0x8000)
            s |= 0xFF000000;
        t->unk58 = s;
        if (e[0] & 0x8000)
            t->unk7A = 0;
    }

    if (e[1] != 0x9999)
    {
        struct Task *t2 = gUnk_03002490;
        s32 v2 = e[1] << 8;

        if (e[1] & 0x8000)
            v2 |= 0xFF000000;
        t2->unk60 = v2;
    }

    if (e[2] != 0x9999)
    {
        struct Task *t3 = gUnk_03002490;
        s32 v3 = e[2] << 8;

        if (e[2] & 0x8000)
            v3 |= 0xFF000000;
        t3->unk68 = v3;
    }
}

void sub_08041438(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 0;
    gUnk_03002490->unk15 = 1;

    if (gUnk_03002490->unk88->unk05 != 0)
    {
        struct Task *t;
        struct Task *t2;

        sub_0803e050(3);
        t = gUnk_03002490;
        t->unk28 = (u16)t->unk88->unk4E;
        t->unk2C = t->unk88->unk4B;
        if (t->unk88->unk4A != 0)
            t->unk88->unk46 = t->unk88->unk4A;
        gUnk_03002490->unk88->unk3D = 0;
        t2 = gUnk_03002490;
        t2->unk88->unk40 &= 0xFFEF;
        t2->unk88->unk0F = 0;
        sub_0803cd60();
    }

    {
        s16 *p = (s16 *)gUnk_0873D206;

        sub_08006338(p[sub_0803fd20(gUnk_03002490->unk88->unk00)]);
    }
    sub_08006138();
}

void sub_080414e8(void)
{
    struct PlayerState *p;
    struct Task *t;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 1;
    gUnk_03002490->unk15 = 2;
    sub_08040b40(1, 72);
    p = gUnk_03002490->unk88;
    if (p->unk05 != 1)
    {
        p->unk3D = 0;
        gUnk_03002490->unk88->unk46 = 0;
        gUnk_03002490->unk88->unk0F = 0;
        gUnk_03002490->unk28 = 0;
        sub_0803cd60();
    }
    while (1)
    {
        sub_08006338(0x11D5);
        TaskYieldTrampoline(gUnk_03002490->unk28 + 2);
        t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 10);
        t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 5);
        t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 5);
        t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 2);
        t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 10);
        t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 5);
        t = gUnk_03002490; t->unk3C++; TaskYieldTrampoline(t->unk28 + 5);
    }
}

void sub_080415c8(void)
{
    while (sub_0803fd90() == 0 && sub_0803fdf4() == 0 && sub_0803fe68() == 0
           && sub_08040298() == 0 && sub_0803ff7c() == 0 && sub_0803fec4() == 0
           && sub_080400c0() == 0)
    {
        struct Task *t = gUnk_03002490;

        if (t->unk54 == 0 && t->unk64 == 0)
        {
            t->unk88->unk01 = 1;
        }
        else if (gUnk_03005550[0] != 0)
        {
            sub_0803f8e8();
            gUnk_03002490->unk88->unk01 = 1;
        }
        else
        {
            struct Task *t2 = gUnk_03002490;
            struct PlayerState *p = t2->unk88;
            s32 v = p->unk3D;

            if (v != 0)
            {
                p->unk01 = 3;
            }
            else if ((gUnk_03002458[p->unk00] & 48) == 0)
            {
                s32 d = t2->unk54;

                if (d < 0)
                    d = -d;
                if ((u32)d <= 0xFFFF)
                    t2->unk28 = 2;
            }
            else
            {
                t2->unk28 = v;
            }
        }
        break;
    }
    sub_08040b40(2, 72);
}

void sub_080416a0(void)
{
    struct PlayerState *p;
    u16 *q;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 2;
    gUnk_03002490->unk15 = 3;
    gUnk_03002490->unk28 = 0;
    sub_08040b40(3, 72);
    if (gUnk_03002490->unk88->unk05 != 2)
    {
        ((u8 *)gUnk_03002490->unk88)[70] = 0;
        q = gUnk_03002458;
        p = gUnk_03002490->unk88;
        if (q[p->unk00] & 48)
        {
            if (((u8 *)p)[62] == 2)
                ((u8 *)p)[62] = 0;
        }
        sub_0803cd60();
        sub_0803e374(117, gUnk_03002490->unk88->unk00);
        sub_0805afac(gUnk_03002490->unk88->unk00, 7, 0);
    }
    while (1)
    {
        sub_08006338(0x11DD);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 4);
    }
}

void sub_08041778(void)
{
    struct Task *t;
    struct Task *t3;
    struct PlayerState *p;
    struct PlayerState *p4;
    u16 *q;
    s32 x;
    s32 m;
    s32 m2;
    s32 y;
    s32 m3;

    t = gUnk_03002490;
    if (t->unk28 == 0)
    {
        m2 = gUnk_03005550[13];
        if (m2 != 0)
        {
            t->unk88->unk14 = 5;
            t->unk28 = 1;
        }
        else
        {
            t->unk88->unk14 = m2;
        }
    }
    else
    {
        p = t->unk88;
        if ((s16)p->unk14 == 0)
        {
            if (sub_08022624(((u16 *)t)[36],
                             (y = ((u16 *)t)[37], m3 = -16, m3 &= y, m3 + 16)) != 0)
                gUnk_03002490->unk7A = 1;
        }
        else
        {
            p->unk14--;
        }
    }
    t = gUnk_03002490;
    if ((t->unk7A & 1) != 0 || (((u8 *)t->unk88)[72] & 3) != 0)
    {
        t->unk28 = 0;
        t->unk88->unk14 = 0;
    }
    while (sub_0803fd90() == 0 && sub_0803fdf4() == 0)
    {
        if (gUnk_03002490->unk28 == 0 && sub_0803fe68() != 0)
            break;
        if (sub_08040298() != 0)
            break;
        if (sub_0803ff7c() != 0)
            break;
        if (sub_0803fec4() != 0)
            goto end;
        if (sub_080400c0() != 0)
            goto end;
        q = gUnk_03002458;
        t3 = gUnk_03002490;
        p4 = t3->unk88;
        m = q[p4->unk00] & 48;
        if (m == 0)
        {
            x = t3->unk54;
            if (x < 0)
                x = -x;
            if ((u32)x <= 0x1CBFF)
            {
                p4->unk3D = m;
                gUnk_03002490->unk88->unk01 = 2;
                goto end;
            }
        }
        if (gUnk_03005550[0] == 0)
            goto end;
        sub_0803f8e8();
        gUnk_03002490->unk88->unk01 = 1;
        goto end;
    }
end:
    sub_08040b40(3, 72);
}

void sub_080418dc(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 3;
    gUnk_03002490->unk15 = 4;
    sub_08040b40(4, 72);
    if (gUnk_03002490->unk88->unk05 != 3)
    {
        sub_080031b8(119);
        sub_0805afac(gUnk_03002490->unk88->unk00, 6, 0);
    }
    sub_08006338(0x11E3);
    sub_08006138();
}

void sub_08041940(void)
{
    struct Task *t;
    struct PlayerState *p;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 4;
    gUnk_03002490->unk15 = 5;
    t = gUnk_03002490;
    if (t->unk88->unk05 != 4)
    {
        t->unk28 = 1;
        p = t->unk88;
        if (p->unk05 == 9)
        {
            p->unk14 = 4;
        }
        else
        {
            t->unk73 = 0;
            sub_08006338(0x11ED);
            TaskYieldTrampoline(3);
            gUnk_03002490->unk88->unk14 = 20;
        }
        sub_080413a4(4);
        sub_080031b8(264);
        gUnk_03002490->unk73 = 1;
        ((s8 *)gUnk_03002490->unk88)[16] = 10;
    }
    sub_0803cd60();
    sub_08006338(0x11E4);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk28 = 0;
    sub_08006338(0x11E5);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 5);
    gUnk_03002490->unk28 = 1;
    sub_08006138();
}

void sub_08041a2c(void)
{
    struct Task *t;
    struct PlayerState *p;
    struct PlayerState *p3;
    s32 k;

    sub_0803f870();
    while (sub_08040264() == 0 && sub_0803ff7c() == 0)
    {
        if (sub_080400c0() != 0)
            break;
        if (sub_08040298() != 0)
            break;
        t = gUnk_03002490;
        k = t->unk73;
        if (k == 0)
            return;
        if (t->unk7A & 1)
        {
            sub_0803f8e8();
            sub_0803e4ec(0);
            sub_0804042c();
            break;
        }
        if (gUnk_03005550[1] != 0)
        {
            sub_0803f8e8();
            sub_0803e050(2);
            gUnk_03002490->unk88->unk01 = 7;
            break;
        }
        if (k == 1)
        {
            p = t->unk88;
            p->unk14--;
            if (p->unk14 == 0 || (k &= gUnk_03002458[t->unk88->unk00]) == 0)
            {
                t->unk73 = 2;
                sub_080413a4(5);
            }
        }
        else if (t->unk28 != 0 && t->unk58 >= 0)
        {
            sub_080413a4(6);
            gUnk_03002490->unk88->unk01 = 7;
        }
        p3 = gUnk_03002490->unk88;
        if (((s8 *)p3)[16] == 0)
        {
            if (gUnk_030023C0[p3->unk00] & 1)
            {
                p3->unk01 = 9;
                break;
            }
        }
        if (gUnk_03005550[0] == 0)
            break;
        sub_0803f8e8();
        if (((u8 *)gUnk_03002490->unk88)[62] & 7)
            sub_08006148(sub_08041940, gCurTaskIdx);
        break;
    }
    sub_08040b40(7, 72);
    sub_0803f9e8();
}

void sub_08041b8c(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 4;
    gUnk_03002490->unk15 = 6;
    if (gUnk_03002490->unk88->unk05 != 4)
    {
        sub_080413a4(4);
        sub_080031b8(264);
        ((s8 *)gUnk_03002490->unk88)[16] = 10;
    }
    gUnk_03002490->unk73 = 1;
    sub_08006338(0x11E4);
    sub_08006138();
}

void sub_08041bf0(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 5;
    gUnk_03002490->unk15 = 7;
    sub_080413a4(6);
    sub_0803cd60();
    sub_08006338(0x11EC);
    sub_08006138();
}

void sub_08041c30(void)
{
    struct Task *t;
    struct PlayerState *p;

    sub_0803f870();
    while (sub_08040264() == 0 && sub_0803ff7c() == 0)
    {
        if (sub_08040298() != 0)
            break;
        if (sub_080400c0() != 0)
            break;
        t = gUnk_03002490;
        if (t->unk7A & 1)
        {
            sub_0803f8e8();
            sub_0803e4ec(0);
            sub_0804042c();
            break;
        }
        p = t->unk88;
        if (((s8 *)p)[16] == 0)
        {
            if (gUnk_030023C0[p->unk00] & 1)
            {
                p->unk01 = 9;
                break;
            }
        }
        if (gUnk_03005550[0] == 0)
            break;
        sub_0803f8e8();
        if (((u8 *)gUnk_03002490->unk88)[62] & 7)
            sub_08006148(sub_08041bf0, gCurTaskIdx);
        break;
    }
    sub_08040b40(7, 72);
    if (gUnk_03002490->unk7A & 1)
        sub_0803e4ec(1);
    sub_0803f9c0();
}

void sub_08041d14(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 14;
    gUnk_03002490->unk15 = 9;
    gUnk_03002490->unk88->unk3D = 0;
    ((s8 *)gUnk_03002490->unk88)[16] = 7;
    sub_080413a4(11);
    sub_080031b8(0x109);
    sub_08006338(0x11EE);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_08006338(0x11E4);
    sub_08006138();
}

void sub_08041dc8(void)
{
    struct Task *t;
    struct PlayerState *p;
    s32 v;

    sub_0803f870();
    while (sub_0803ff7c() == 0 && sub_080400c0() == 0)
    {
        if ((v = sub_08040298()) != 0)
            break;
        t = gUnk_03002490;
        if (t->unk7A & 1)
        {
            if (t->unk58 >= 0)
            {
                sub_0803f8e8();
                sub_0803e4ec(0);
                sub_0804042c();
                goto end;
            }
            t->unk7A = v;
        }
        p = gUnk_03002490->unk88;
        if (((s8 *)p)[16] == 0)
        {
            if (gUnk_030023C0[p->unk00] & 1)
            {
                sub_08006148(sub_08041d14, gCurTaskIdx);
                break;
            }
        }
        if (gUnk_03002490->unk58 > 0x10000)
        {
            sub_080413a4(6);
            gUnk_03002490->unk88->unk01 = 7;
        }
        break;
    }
end:
    sub_08040b40(6, 72);
    sub_0803f9e8();
}

void sub_08041e8c(void)
{
    struct Task *t;
    s16 *q;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 6;
    gUnk_03002490->unk15 = 10;
    t = gUnk_03002490;
    if (t->unk88->unk05 != 6)
    {
        ((u32 **)t->unk88)[25] = gUnk_0873BD28;
        ((u32 **)t->unk88)[26] = gUnk_0873CB24;
        t->unk2C = ((u8 *)t->unk88)[75];
        sub_08040b40(0, 72);
    }
    gUnk_03002490->unk28 = 8;
    q = (s16 *)gUnk_0873D5C0;
    sub_08006338(q[sub_0803fd20(gUnk_03002490->unk88->unk00)]);
    sub_08006138();
}

void sub_08041f10(void)
{
    struct Task *t;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 7;
    gUnk_03002490->unk15 = 11;
    t = gUnk_03002490;
    if (t->unk88->unk05 != 7)
    {
        t->unk28 = 0;
        t->unk73 = 0;
    }
    switch (gUnk_03002490->unk73)
    {
    case 0:
        sub_08040b40(11, 1);
        sub_08006338(0x1201);
        TaskYieldTrampoline(1);
        sub_0803e050(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk73 = 1;
        /* fallthrough */
    case 1:
        gUnk_03002490->unk80 = 4;
        gUnk_03002490->unk88->unk14 = 10;
        sub_08040b40(11, 0);
        gUnk_03002490->unk88->unk6C = gUnk_0873D03C;
        sub_0803e374(118, gUnk_03002490->unk88->unk00);
        sub_0805afac(gUnk_03002490->unk88->unk00, 8, 0);
        sub_08006338(0x1203);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        while ((u32)(gUnk_03002490->unk54 < 0 ? -gUnk_03002490->unk54 : gUnk_03002490->unk54) > 0x7FFF)
            TaskYieldTrampoline(1);
        gUnk_03002490->unk73 = 2;
        /* fallthrough */
    case 2:
        gUnk_03002490->unk88->unk6C = 0;
        gUnk_03002490->unk80 = 0;
        break;
    }
    gUnk_03002490->unk28++;
    sub_08006138();
}

void sub_08042050(void)
{
    struct Task *t;
    struct Task *t2;
    struct PlayerState *p;
    s32 v;
    s32 x;
    s32 k;

    v = sub_0803fe68();
    if (v == 0)
    {
        t = gUnk_03002490;
        k = t->unk73;
        switch (k)
        {
        case 0:
            break;
        case 1:
            if (gUnk_03005550[0] != 0)
            {
                sub_0803f8e8();
                sub_0803e050(1);
                gUnk_03002490->unk88->unk01 = 1;
                break;
            }
            p = t->unk88;
            if ((s16)p->unk14 != 0)
            {
                p->unk14--;
                if (p->unk14 == 0)
                    sub_08040b40(5, 72);
            }
            t2 = gUnk_03002490;
            x = t2->unk54;
            if (x < 0)
                x = -x;
            if ((u32)x > 0xA000)
                sub_0801a828((u8)gCurTaskIdx, t2->unk48, t2->unk4A, gUnk_0873CA68);
            break;
        case 2:
            if (t->unk28 != 0)
            {
                t->unk88->unk6C = (void *)v;
                t->unk88->unk01 = 1;
            }
            break;
        }
    }
    else
    {
        if (gUnk_03002490->unk5C == 0)
            sub_08040b40(5, 72);
    }
}

/* MATCH (512 bytes).  `xa` is load-bearing, not cosmetic.  It is the FIRST x
   of loop 2 and is referenced only inside that loop`s entry block, so it adds
   a fourth block-local quantity there.  With three quantities (the unk28
   index; the combined i*2/+i/*2 chain; the &gUnk_0873D986 pool address)
   local-alloc takes gcc 2.9`s hand-rolled `case 3:` sort in block_alloc,
   whose comparisons use the literal qty numbers 0/1/2 while EXCHANGE permutes
   qty_order - it swaps twice and leaves the identity order, so the index is
   allocated first and takes r0 while the *6 chain takes r1.  The record
   pointer is then `(set r (plus <chain> <pool>))`, set_preference reads
   XEXP(src,0) = the chain = hard r1, so the pointer allocno prefers r1;
   prune_preferences copies that into regs_someone_prefers[v] (v conflicts
   with it and is higher priority), find_reg pass 0 refuses r1 for v, v takes
   r2 and the pointer takes r1 - the r1/r2 swap seen in BOTH loops.  A fourth
   quantity pushes block_alloc onto the qsort path, which sorts correctly:
   chain->r0, pool->r1, index->r1, xa->r0; the pointer`s preference becomes
   r0, which it conflicts with and is pruned, so v is free to take r1. */
void sub_08042128(void)
{
    struct Task *h1;
    struct Task *h2;
    struct Task *h3;
    struct Task *d;
    struct Task *a1;
    struct Task *b1;
    struct Task *c2;
    struct Task *a2;
    struct Task *b2;
    u16 *q;
    u16 *r;
    s32 v;
    s32 x;
    s32 xa;
    s32 k;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 9;
    gUnk_03002490->unk15 = 12;
    if (gUnk_03002490->unk88->unk05 != 9)
    {
        sub_0803e050(3);
        gUnk_03002490->unk43 = 1;
        h1 = gUnk_03002490;
        h1->unk3E &= 0x7FFF;
        h1->unk88->unk3D = 0;
        ((u8 *)gUnk_03002490->unk88)[80] = 1;
        q = gUnk_03002458;
        h2 = gUnk_03002490;
        if (q[h2->unk88->unk00] & 64)
        {
            h2->unk73 = 1;
            gUnk_03002490->unk28 = 0;
        }
        else
        {
            h2->unk73 = 2;
            gUnk_03002490->unk28 = 10;
        }
        h3 = gUnk_03002490;
        h3->unk2C = h3->unk43;
        h3->unk4C = ((h3->unk48 & 0xFFF0) | 8) << 16;
    }
    d = gUnk_03002490;
    d->unk46 = 0x11F4;
    k = d->unk73;
    switch (k)
    {
    case 1:
        d->unk30 = k;
        for (;;)
        {
            if (gUnk_03002490->unk28 == 0 || gUnk_03002490->unk28 == 6)
                sub_080031b8(123);
            a1 = gUnk_03002490;
            r = &((u16 *)gUnk_0873D986)[a1->unk28 * 3];
            x = -r[2];
            v = x << 8;
            if (x & 0x8000)
                v |= 0xFF000000;
            a1->unk58 = v;
            x = r[2];
            v = x << 8;
            if (x & 0x8000)
                v |= 0xFF000000;
            a1->unk68 = v;
            a1->unk3C = r[0] + ((u16 *)a1)[35];
            TaskYieldTrampoline(r[1]);
            b1 = gUnk_03002490;
            b1->unk28++;
            if (b1->unk28 > 9)
                b1->unk28 = 0;
        }
    case 2:
        c2 = gUnk_03002490;
        c2->unk30 = c2->unk73;
        c2->unk88->unk14 = 0;
        sub_080031b8(124);
        for (;;)
        {
            a2 = gUnk_03002490;
            r = &((u16 *)gUnk_0873D986)[a2->unk28 * 3];
            xa = r[2];
            v = xa << 8;
            if (xa & 0x8000)
                v |= 0xFF000000;
            a2->unk58 = v;
            x = r[2];
            v = x << 8;
            if (x & 0x8000)
                v |= 0xFF000000;
            a2->unk68 = v;
            a2->unk3C = r[0] + ((u16 *)a2)[35];
            TaskYieldTrampoline(r[1]);
            b2 = gUnk_03002490;
            b2->unk28++;
            if (b2->unk28 > 13)
                b2->unk28 = 10;
        }
    case 0:
        sub_0803e050(2);
        break;
    }
    sub_08006138();
}

/* MATCH (600 bytes).  The two `unk01 = 5` / `unk01 = 1` arms are NOT written
   as an if/else-if chain in the tail: they are labelled statements INSIDE
   case 0, sitting between the `else if ((...&64) != 0)` arm and the third
   arm (which is therefore reached by `goto arm3`).  The tail branches to them
   with the un-inverted conditions (`beq set5` / `bne set1`, both backward),
   and each returns.  gcc emits basic blocks in source order, so that source
   position is what puts the 11 instructions in the hole at 0x080424AE and
   makes the four literal pools land where the ROM has them - the previous
   4-byte residue was pool alignment padding caused purely by block order.
   Corroboration: at 0x08042562 the `movs r3,#1` for the `unk7A & 1` mask is
   still live inside set1 (`strb r3,[r0,#1]`), i.e. the constant 1 is CSEd
   across the goto, which only happens if set1 is a jump target. */
void sub_08042328(void)
{
    struct Task *t;
    struct Task *ta;
    struct Task *tb;
    struct Task *tc;
    struct Task *td;
    struct Task *te;
    struct Task *tf;
    struct PlayerState *p;
    u16 *qa;
    u16 *qb;
    struct Task *tg;
    u16 *qd;
    s32 k;
    s32 n;

    t = gUnk_03002490;
    k = t->unk73;
    switch (k)
    {
    case 1:
        if (gUnk_03005550[1] != 0 || (((u8 *)t->unk88)[72] & 4) != 0)
            t->unk58 = 0;
        qa = gUnk_03002458;
        ta = gUnk_03002490;
        if ((qa[ta->unk88->unk00] & 192) == 0)
            ta->unk73 = 0;
        else if ((qa[ta->unk88->unk00] & 128) != 0)
            ta->unk73 = 2;
        if (gUnk_03002490->unk73 == 1)
            break;
        sub_08006148(sub_08042128, gCurTaskIdx);
        break;
    case 2:
        p = t->unk88;
        k &= p->unk14;
        if (k != 0)
        {
            sub_080031b8(124);
            gUnk_03002490->unk88->unk14 = 0;
        }
        else
        {
            p->unk14++;
        }
        qb = gUnk_03002458;
        tb = gUnk_03002490;
        if ((qb[tb->unk88->unk00] & 192) == 0)
            tb->unk73 = 0;
        else if ((qb[tb->unk88->unk00] & 64) != 0)
            tb->unk73 = 1;
        if (gUnk_03002490->unk73 == 2)
            break;
        sub_08006148(sub_08042128, gCurTaskIdx);
        break;
    case 0:
        if ((gUnk_030023C0[t->unk88->unk00] & 192) == 0)
            break;
        n = t->unk30;
        if (n == 1)
        {
            if ((gUnk_030023C0[t->unk88->unk00] & 64) != 0)
            {
                t->unk73 = n;
                tc = gUnk_03002490;
                tc->unk28++;
                if (tc->unk28 > 9)
                    tc->unk28 = k;
            }
            else
            {
                t->unk73 = 2;
                gUnk_03002490->unk28 = 10;
            }
        }
        else if ((gUnk_030023C0[t->unk88->unk00] & 64) != 0)
        {
            t->unk73 = 1;
            gUnk_03002490->unk28 = k;
        }
        else
            goto arm3;
        goto callit;
    set5:
        tg->unk88->unk01 = 5;
        return;
    set1:
        tg->unk88->unk01 = 1;
        return;
    arm3:
        t->unk73 = 2;
        td = gUnk_03002490;
        td->unk28++;
        if (td->unk28 > 13)
            td->unk28 = 10;
    callit:
        sub_08006148(sub_08042128, gCurTaskIdx);
        break;
    }
    qd = gUnk_030023C0;
    te = gUnk_03002490;
    if ((qd[te->unk88->unk00] & 48) != 0)
    {
        te->unk43 = te->unk2C;
        tf = gUnk_03002490;
        tf->unk3E &= 0x7FFF;
        if (tf->unk58 != 0)
            sub_0803e050(2);
        sub_0804042c();
    }
    else if ((gUnk_03005550[6] & 3) == 0)
    {
        te->unk43 = te->unk2C;
        tg = gUnk_03002490;
        if (tg->unk73 == 1)
            goto set5;
        if (tg->unk7A & 1)
            goto set1;
        tg->unk88->unk01 = 7;
    }
}

void sub_08042580(void)
{
    struct Task *t;
    struct Task *t5;
    struct Task *tj;
    struct Task *tt;
    struct PlayerState *p;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 17;
    gUnk_03002490->unk15 = 16;
    t = gUnk_03002490;
    p = t->unk88;
    if (p->unk05 != 17)
    {
        p->unk42 &= 0xFFEF;
        t->unk73 = 5;
    }
    while (1)
    {
        switch (gUnk_03002490->unk73)
        {
        case 5:
            gUnk_03002490->unk88->unk3D = 0;
            sub_080261d4(2);
            t5 = gUnk_03002490;
            if (t5->unk82 & 128)
                t5->unk73 = 4;
            else
                t5->unk73 = t5->unk82 & 15;
            ((u8 *)gUnk_03002490->unk88)[63] = 1;
            gUnk_03002490->unk88->unk12 = 0x8000;
            sub_0803e050(3);
            continue;
        case 6:
            if (gUnk_03002490->unk7B & 1)
                sub_0803e4a8();
            sub_0803e1b8(1, 96, gUnk_03002490->unk88->unk00);
            sub_08006138();
            /* fallthrough */
        case 0:
            sub_080031b8(0x107);
            if ((s8)gUnk_03002490->unk7D == 0)
                sub_08040b40(10, 32);
            else
                sub_08040b40(10, 33);
            sub_08006338(0x123B);
            TaskYieldTrampoline(16);
            break;
        case 1:
            sub_080031b8(0x107);
            sub_0803e650(16);
            sub_0805afac(gUnk_03002490->unk88->unk00, 22, 0);
            gUnk_03002490->unk6C = 0;
            do
            {
                sub_08006338(0x1244);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk6C++;
            } while ((s16)gUnk_03002490->unk6C <= 3);
            sub_08006338(0x1244);
            TaskYieldTrampoline(1);
            tj = gUnk_03002490;
            goto spawn25;
        case 2:
            sub_080031b8(0x107);
            sub_0805afac(gUnk_03002490->unk88->unk00, 23, 0);
            gUnk_03002490->unk6C = 0;
            do
            {
                sub_0803e650(17);
                sub_08006338(0x123B);
                TaskYieldTrampoline(2);
                sub_08006338(0x1249);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk6C++;
            } while ((s16)gUnk_03002490->unk6C <= 7);
            sub_08006338(0x123B);
            TaskYieldTrampoline(1);
            tj = gUnk_03002490;
        spawn25:
            sub_0805afac(tj->unk88->unk00, 25, 0);
            break;
        case 3:
            sub_080031b8(0x107);
            sub_0805afac(gUnk_03002490->unk88->unk00, 24, 0);
            sub_0803e650(18);
            sub_08006338(0x124B);
            TaskYieldTrampoline(44);
            sub_08006338(0x123B);
            TaskYieldTrampoline(1);
            break;
        case 4:
            sub_080031b8(0x107);
            gUnk_03002490->unk7A = 0;
            sub_080413a4(30);
            sub_08006338(0x123C);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk6C = 0;
            do
            {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk6C++;
            } while ((s16)gUnk_03002490->unk6C <= 5);
            sub_08006338(0x11EC);
            TaskYieldTrampoline(2);
            goto again;
        }
        if ((s8)gUnk_03002490->unk7D == 0)
            sub_08040b40(10, 36);
        else
            sub_08040b40(10, 37);
        sub_0803e650(15);
        sub_08006338(0x123C);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 4);
        if ((s8)gUnk_03002490->unk7D == 0)
            sub_08040b40(10, 34);
        else
            sub_08040b40(10, 35);
        sub_0803e650(15);
        tt = gUnk_03002490;
        if (tt->unk7A & 1)
        {
            sub_08006338(0x11EC);
            TaskYieldTrampoline(3);
        }
        else
        {
            tt->unk3C++;
            TaskYieldTrampoline(3);
        }
        sub_0803e050(3);
    again:
        gUnk_03002490->unk73 = 6;
    }
}

void sub_08042980(void)
{
    switch (gUnk_03002490->unk73)
    {
    case 6:
        sub_08040710();
        break;
    case 5:
        break;
    case 4:
        sub_08040b40(7, 72);
    case 0:
    case 1:
    case 2:
    case 3:
        if (sub_0803fce4(0))
        {
            gUnk_03002490->unk73 = 6;
            sub_08006148(sub_08042580, gCurTaskIdx);
        }
        break;
    }
    sub_0803f9c0();
}

/* Stage entry (issue #85).  Clears the player's bit in gUnk_03002340,
   decrements gUnk_03002350, installs sub_08042c50 as Task.unk04, spawns five
   sub-tasks through sub_0805afac (id 12 four times, then id 13) and picks a
   random signed 8.8 value into Task.unk54 from the camera x (gUnk_03002348)
   and gUnk_03001EA4.

   Four shapes were load-bearing in the +/-1 chain:
     * `k` is a REAL LOCAL holding the 1 stored into PlayerState.unk22, not a
       literal.  A literal store leaves the RTL as `(set (mem:QI) (const_int 1))`
       and reload invents the register only after cse, so there is no pseudo for
       cse to reuse; with a local, cse follows the TAKEN side of the `beq` into
       this else-arm (that label has LABEL_NUSES == 1) and still knows k == 1 at
       the `& 1`, rewriting `movs r0,#1` into `adds r0,r5,#0` (lesson 3.327).
     * The store must be the FIELD spelling `->unk22 = k = 1`.  `((u8 *)p)[34]`
       or a separate `k = 1;` statement emits the `movs` before the address
       computation; the field form emits address-then-`movs` (the offset 34
       exceeds strb's #31 immediate, so it still becomes `adds r0,#34`).
     * `m` caches `gUnk_03001EA4 & 1` BEFORE `sign = 1`, which is what puts the
       `movs r5,#1` between the `ands` and the `cmp`.
     * `dx` (the camera delta) must be its own local: the ROM keeps it in r0 and
       the later `x << 8` in r2, and two registers mean two variables (3.322).
       Reusing one `d` for both puts the delta in r2.
   Sharing `k` with `sign` instead lets cse delete `sign = 1` in the first arm.

   `PlayerState.unk12` (u16 at 0x12) had to exist as a real field: the ROM's
   `movs r0,#128; lsls r0,r0,#8; strh r0,[r1,#18]` only comes out of a struct
   member - every `*(u16 *)((u8 *)p + 18)` spelling adds an `adds r0,rN,#0`
   copy (lesson 3.323).  include/task.h now carries it, so the throw-away
   stand-in this body used is gone. */
void sub_080429fc(void)
{
    struct Task *t;
    u16 *q;
    s32 i;
    s32 sign;
    s32 k;
    s32 m;
    s32 dx;
    s32 r;
    s32 d;
    s32 x;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 18;
    gUnk_03002490->unk04 = (u32)sub_08042c50;
    gUnk_03002490->unk08 = 0;
    gUnk_03002350--;
    gUnk_03002340 &= ~(1 << gUnk_03002490->unk88->unk00);
    if (gUnk_03002490->unk82 == 512)
        sub_08027548();
    gUnk_03002490->unk88->unk40 |= 8;
    gUnk_03002490->unk88->unk42 |= 0x100;
    gUnk_03002490->unk73 = 0;
    gUnk_03002490->unk88->unk42 &= 0xFFEF;
    sub_0803e1b8(255, 0, gUnk_03002490->unk88->unk00);
    ((u8 *)gUnk_03002490->unk88)[23] = 0;
    ((u16 *)gUnk_03002490->unk88)[12] = 0;
    q = (u16 *)gUnk_03002490->unk88;
    q[14] = 0;
    q[13] = 0;
    sub_0803e050(3);
    gUnk_03002490->unk42 = 4;
    sub_080276ac(gUnk_03002490->unk88->unk00);
    sub_08003688();
    sub_08003484();
    gUnk_03001F34 = 1;
    sub_08006338(0x123B);
    sub_0803e3e4(15);
    sub_08027204(2);
    if ((gUnk_03001ED8 & 0x400) == 0)
    {
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1D00;
    }
    sub_080261d4(4);
    TaskYieldTrampoline(1);
    sub_080031b8(158);
    TaskYieldTrampoline(59);
    gUnk_03002490->unk73 = 1;
    for (i = 0; i < 4; i++)
        sub_0805afac(gUnk_03002490->unk88->unk00, 12, i);
    sub_0805afac(gUnk_03002490->unk88->unk00, 13, 0);
    gUnk_03002490->unk88->unk22 = k = 1;
    gUnk_03002490->unk88->unk12 = 0x8000;
    sub_08003110(3);
    if (sub_08022760(gUnk_03002490))
    {
        sub_080413a4(32);
    }
    else
    {
        dx = gUnk_03002490->unk48 - gUnk_03002348;
        if (dx <= 55)
            sign = 1;
        else if (dx > 160)
            sign = -1;
        else
        {
            m = gUnk_03001EA4 & 1;
            sign = 1;
            if (m == 0)
                sign = -1;
        }
        r = sub_08002ee8(3) << 8;
        r |= sub_08002ee8(16) << 4;
        if (r <= 255)
            r |= 256;
        t = gUnk_03002490;
        x = r * sign;
        d = x << 8;
        if (x & 0x8000)
            d |= 0xFF000000;
        t->unk54 = d;
        t->unk64 = 0x40000;
        sub_080413a4(33);
    }
    sub_08006338(0x124C);
    sub_08006138();
}

void sub_08042c50(void)
{
    switch (gUnk_03002490->unk73)
    {
    case 0:
        break;
    case 1:
        if (gUnk_03002490->unk58 > 0)
        {
            gUnk_03002490->unk68 = 0x40000;
            if (gUnk_03002490->unk4A - gUnk_030023E4 > 199)
            {
                sub_0803e050(2);
                gUnk_03002490->unk88->unk14 = 90;
                gUnk_03002490->unk73 = 2;
            }
        }
        if ((gUnk_03002490->unk88->unk42 & 32) == 0)
            sub_0803e080();
        break;
    case 2:
        if (--gUnk_03002490->unk88->unk14 == 0)
        {
            gUnk_03002438 = 6;
            sub_08005654(gCurTaskIdx);
        }
        break;
    }
}

void sub_08042cfc(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 5;
    gUnk_03002490->unk15 = 17;
    sub_08040b40(11, 16);
    sub_080413a4(22);
    gUnk_03002490->unk7A = 0;
    sub_08006138();
}

void sub_08042d40(void)
{
    gUnk_03002490->unk88->unk01 = 7;
}

void sub_08042d54(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 19;
    gUnk_03002490->unk04 = 0;
    gUnk_03002490->unk08 = 0;
    gUnk_03002490->unk88->unk3D = 0;
    gUnk_03001F34 = 1;
    sub_0803e050(3);
    gUnk_03002490->unk88->unk42 |= 0x100;
    sub_080261d4(0);
    if (gUnk_03002444 == 0)
    {
        sub_08003688();
        sub_0803e3e4(15);
        if ((gUnk_03001ED8 & 0x400) == 0)
        {
            gUnk_03001ED8 &= 0xE0FF;
            gUnk_03001ED8 |= 0x1D00;
        }
        TaskYieldTrampoline(1);
    }
    sub_08025024();
    if (gUnk_03002444 != 0)
        ((void (*)(void))sub_080264b0)();
    if (gUnk_03002444 == 0)
        sub_080031b8(181);
    if ((gUnk_03002490->unk7B & 1) == 0)
    {
        sub_08006338(0x1208);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
    }
    else
    {
        sub_08006338(0x1208);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
    }
    gUnk_03002490->unk3C++;
    sub_08006138();
}

void sub_08042e98(void)
{
    s32 a;

    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 19;
    gUnk_03002490->unk15 = 19;
    sub_080224b0();
    sub_08021c74((s32)gUnk_0873CB1C, gCurTaskIdx);
    gUnk_03002490->unk28 = 0;
    gUnk_03002490->unk3C = -1;
    if (gUnk_0200AF00 == 1)
    {
        ((void (*)(void))sub_08027a60)();
        a = ((s32 (*)(void))sub_0802653c)();
        TaskYieldTrampoline(1);
        sub_08026704(a);
    }
    while (gUnk_03001E90 != 0)
        TaskYieldTrampoline(1);
    a = ((s32 (*)(void))sub_080264b0)();
    gUnk_03002490->unk3C = -1;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3E = 0x2000;
    sub_08040b40(10, 9);
    gUnk_03002490->unk3C = 0x120B;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(3);
    sub_0802651c(a);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(4);
    sub_0803e050(1);
    sub_0800617c(0x1208);
    TaskYieldTrampoline(2);
    {
        s16 *p = (s16 *)gUnk_0873D206;
        sub_08006338(p[((s32 (*)(s32))sub_0803fd20)((s8)gUnk_03002490->unk88->unk00)]);
    }
    if (gUnk_0200AF00 == 1)
    {
        sub_080055b0(14, gCurTaskIdx);
        sub_0802672c();
        while (gUnk_020055E8 == 0)
            TaskYieldTrampoline(1);
        ((void (*)(void))sub_08027a60)();
        sub_080055b0(0, gCurTaskIdx);
    }
    gUnk_03002490->unk3E = 0x4000;
    ((void (*)(void))sub_08026584)();
    gUnk_03002490->unk28++;
    sub_08006138();
}

void sub_08043014(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 15;
    gUnk_03002490->unk15 = 20;
    if (gUnk_03002490->unk88->unk05 != 15)
    {
        if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 0x30)
            gUnk_03002490->unk73 = 3;
        else if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 0x41)
            gUnk_03002490->unk73 = 1;
        else if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 0x80)
            gUnk_03002490->unk73 = 2;
        else
            gUnk_03002490->unk73 = 0;
        gUnk_03002490->unk2C = gUnk_03002490->unk73;
        sub_0803e4a8();
        ((u8 *)gUnk_03002490->unk88)[61] = 0;
    }
    switch (gUnk_03002490->unk73)
    {
    case 0:
        sub_080413a4(13);
        sub_08040b40(11, 4);
        if (gUnk_03002490->unk2C == 2)
        {
            sub_08006338(0x1239);
            TaskYieldTrampoline(3);
        }
        sub_08006338(0x1238);
        break;
    case 1:
        sub_080413a4(13);
        sub_08040b40(11, 4);
        while (1)
        {
        lab1:
            gUnk_03002490->unk28 = 0;
            sub_080031b8(120);
            if (((u8 *)gUnk_03002490->unk88)[92] & 1)
            {
                sub_080413a4(15);
                sub_08006338(0x1234);
                TaskYieldTrampoline(4);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(4);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(4);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(4);
                sub_08006338(0x1230);
                TaskYieldTrampoline(4);
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(4);
                gUnk_03002490->unk3C++;
                gUnk_03002490->unk6C = 0;
                do
                {
                    if (gUnk_03002490->unk28 != 0)
                        goto lab1;
                    TaskYieldTrampoline(1);
                    if (gUnk_03002490->unk28 != 0)
                        goto lab1;
                    TaskYieldTrampoline(1);
                } while ((s16)++gUnk_03002490->unk6C <= 4);
                gUnk_03002490->unk68 = 0x10000;
            }
            sub_08006338(0x1233);
            gUnk_03002490->unk6C = 0;
            do
            {
                if (gUnk_03002490->unk28 != 0)
                    goto lab1;
                TaskYieldTrampoline(1);
            } while ((s16)++gUnk_03002490->unk6C <= 14);
            {
                if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 0x41)
                    goto lab1;
            }
            gUnk_03002490->unk6C = 0;
            do
            {
                if (gUnk_03002490->unk28 != 0)
                    goto lab1;
                TaskYieldTrampoline(1);
            } while ((s16)++gUnk_03002490->unk6C <= 44);
            gUnk_03002490->unk28 = -1;
            sub_08006138();
        }
    case 2:
        sub_080413a4(14);
        sub_08040b40(11, 4);
        gUnk_03002490->unk88->unk14 = 15;
        sub_08006338(0x1239);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        break;
    case 3:
        sub_08040b40(11, 3);
        gUnk_03002490->unk28 = 0;
        if (((u8 *)gUnk_03002490->unk88)[92] & 1)
        {
            if (gUnk_03002458[gUnk_03002490->unk88->unk00] & 0x41)
                sub_080413a4(12);
            else
                sub_080413a4(13);
        }
        else
        {
            sub_080413a4(13);
            gUnk_03002490->unk28 = 10;
        }
        gUnk_03002490->unk88->unk14 = 15;
        while (1)
        {
            sub_080031b8(120);
            sub_08006338(0x1228);
            TaskYieldTrampoline(5);
            gUnk_03002490->unk6C = 0;
            do
            {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(5);
            } while ((s16)++gUnk_03002490->unk6C <= 6);
        }
    }
    sub_08006138();
}
