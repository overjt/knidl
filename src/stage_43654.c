#include "gba/gba.h"
#include "global.h"
#include "task.h"

struct M11R8 { u8 unk00; u8 unk01; u8 unk02; u8 unk03; u8 *unk04; };
struct M11Buf { u8 unk00[4]; u8 unk04[4]; };
struct M11R20 { u32 w[5]; };

extern struct M11R8 gUnk_02005550[];
extern struct M11R20 gUnk_020060E0[];
extern u16 gUnk_030023C0[];
extern u16 gUnk_03002458[];
extern struct Task *gUnk_03002490;
extern u32 gUnk_0873CA7C[];
extern u32 gUnk_0873CA90[];
extern u32 gUnk_0873CAA4[];
extern u32 gUnk_0873D044[];
extern u32 gUnk_0873D04C[];

void TaskYieldTrampoline(s32 frames);
s32 sub_080031b8(s32 id);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
void sub_080061c0(s32 a, s32 b);
void sub_0800622c(s32 a, s32 b, s32 c);
void sub_08006338(s32 a);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);
void sub_080261d4(u32 a);
void sub_0803e050(s32 a0);
void sub_0803e374(s32 a, u16 b);
void sub_0803e3ac(void);
void sub_0803e4a8(void);
s32 sub_0803e4ec(s32 a0);
s32 sub_0803e55c(void);
s32 sub_0803e5c0(s32 playerIdx, u8 *src6);
s32 sub_0803e5f8(s32 a0, s32 a1);
void sub_0803e650(s32 a0);
void sub_0803f870(void);
s32 sub_0803f9e8(void);
s32 sub_0803fa44(void);
s32 sub_0803fce4(s32 a);
s32 sub_0804042c(void);
void sub_08040b40(s32 a0, s32 a1);
void sub_080413a4(s32 a0);
void sub_08053940(s32 a, s32 b, s32 c);
s32 sub_0805afac(s32 band, s32 id, s32 payload);

void sub_08043654(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 0;
    gUnk_03002490->unk15 = 21;
    if (gUnk_03002490->unk88->unk05 != 0)
    {
        sub_0803e050(3);
        gUnk_03002490->unk28 = ((u8 *)gUnk_03002490->unk88)[74];
    }
    sub_08006338(0x11CB);
    sub_08006138();
}

void sub_080436ac(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 1;
    gUnk_03002490->unk15 = 22;
    sub_08040b40(9, 72);
    while (1)
    {
        sub_08006338(0x11D5);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(7);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(7);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(7);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(7);
    }
}

void sub_0804374c(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 23;
Lloop:
    gUnk_03002490->unk80 = 4;
    {
        struct Task *t = gUnk_03002490;

        t->unk30 = 0;
        t->unk88->unk14 = 0;
        {
            struct M11R20 *d = (struct M11R20 *)gUnk_020060E0;

            d[t->unk88->unk00] = *(struct M11R20 *)gUnk_0873CA90;
        }
    }
    {
        struct M11R8 *g8 = (struct M11R8 *)gUnk_02005550;

        {
            struct Task *t = gUnk_03002490;

            g8[t->unk88->unk00] = *(struct M11R8 *)gUnk_0873D044;
            t->unk2C = -1;
            *(u32 *)((u8 *)t->unk88 + 108) = 0;
            t->unk73 = 0;
        }
        sub_080031b8(266);
        sub_08006338(0x1212);
        TaskYieldTrampoline(4);
        {
            struct Task *t = gUnk_03002490;

            t->unk28 = t->unk7A;
            if (t->unk28 != 0)
            {
                sub_0805afac(t->unk88->unk00, 28, 3);
                sub_080061c0(0x10000, -0x1000);
                gUnk_03002490->unk64 = 0;
            }
        }
        {
            struct Task *t = gUnk_03002490;

            *(u32 *)((u8 *)t->unk88 + 108) = (u32)&g8[t->unk88->unk00];
            t->unk6C = 0;
        }
    }
    do
    {
        struct Task *t = gUnk_03002490;

        sub_0803e5f8(t->unk88->unk00,
                     (s32)((u8 *)gUnk_0873D04C + ++t->unk2C * 8));
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 1);
    {
        struct Task *t = gUnk_03002490;

        t->unk88->unk14 = 8;
        t->unk6C = 0;
    }
    do
    {
        struct Task *t = gUnk_03002490;

        sub_0803e5f8(t->unk88->unk00,
                     (s32)((u8 *)gUnk_0873D04C + ++t->unk2C * 8));
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 5);
    if (gUnk_03002490->unk28 != 0)
    {
        sub_080061c0(0x4000, 0);
        gUnk_03002490->unk64 = 0x4000;
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk2C = -1;
        *(u32 *)((u8 *)t->unk88 + 108) = 0;
        t->unk3C++;
    }
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        if (t->unk30 == 0)
            goto Lend;
        t->unk30 = 0;
        t->unk88->unk14 = 0;
        t->unk73 = 1;
    }
    if (gUnk_03002490->unk28 != 0)
        sub_0803e050(1);
    sub_080031b8(0x10B);
    sub_08006338(0x121B);
    TaskYieldTrampoline(4);
    {
        struct Task *t = gUnk_03002490;

        t->unk28 = t->unk7A;
        if (t->unk28 != 0)
        {
            sub_080061c0(-0x800, 0x800);
            gUnk_03002490->unk64 = 0;
        }
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk2C = 8;
        *(u32 *)((u8 *)t->unk88 + 108) =
            (u32)&((struct M11R8 *)gUnk_02005550)[t->unk88->unk00];
        t->unk6C = 0;
    }
    do
    {
        struct Task *t = gUnk_03002490;

        sub_0803e5f8(t->unk88->unk00,
                     (s32)((u8 *)gUnk_0873D04C + --t->unk2C * 8));
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 1);
    {
        struct Task *t = gUnk_03002490;

        t->unk88->unk14 = 8;
        t->unk6C = 0;
    }
    do
    {
        struct Task *t = gUnk_03002490;

        sub_0803e5f8(t->unk88->unk00,
                     (s32)((u8 *)gUnk_0873D04C + --t->unk2C * 8));
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 5);
    if (gUnk_03002490->unk28 != 0)
    {
        sub_080061c0(-0x4000, 0);
        gUnk_03002490->unk64 = 0x4000;
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk2C = -1;
        *(u32 *)((u8 *)t->unk88 + 108) = 0;
        t->unk3C--;
    }
    TaskYieldTrampoline(4);
    if (gUnk_03002490->unk30 == 0)
        goto Lend;
    goto Lloop;
Lend:
    gUnk_03002490->unk73 = 2;
    sub_08006138();
}


void sub_08043a88(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;
    struct Task *v;
    struct PlayerState *p;

    if (t->unk73 != 2) {
        p = t->unk88;
        if ((s16)p->unk14 != 0) {
            p->unk14--;
            {
                u16 *q = (u16 *)gUnk_030023C0;
                if (q[t->unk88->unk00] & 2)
                    t->unk30 = 1;
            }
        }
        u = gUnk_03002490;
        if (u->unk2C != -1) {
            sub_0803e5c0(u->unk88->unk00, (u8 *)gUnk_0873CAA4 + u->unk2C * 8);
            sub_0801a828((u8)gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                         (u8 *)gUnk_020060E0 + gUnk_03002490->unk88->unk00 * 20);
        }
    } else {
        sub_0804042c();
    }
    v = gUnk_03002490;
    if ((v->unk7A & 1) == 0) {
        if ((v->unk7B & 1) == 0)
            sub_080413a4(2);
        else
            sub_080413a4(13);
    } else {
        sub_0803e4ec(1);
    }
    sub_0803f9e8();
}


/* NOTE: fwd.h gives this four s32 args; that is an arity.py artifact -
   the script does not model `ldmia rN!, {r2, r3, r4}` (the 20-byte struct
   copy in this body) as WRITING r2/r3, so it reads them as incoming
   arguments.  The real prototype is `void sub_08043b80(void)`, confirmed by
   the anchor table at 0x0873B46C whose entries are `void (*)(void)`.
   The unused parameters are kept only so this file compiles against the
   current fwd.h; they cost no code. */
void sub_08043b80(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 24;
    gUnk_03002490->unk80 = 4;
    {
        struct Task *t = gUnk_03002490;
        t->unk30 = 0;
        t->unk28 = 0;
        {
            struct M11R20 *d = (struct M11R20 *)gUnk_020060E0;
            d[t->unk88->unk00] = *(struct M11R20 *)gUnk_0873CA90;
        }
    }
    {
        struct M11R8 *g8 = (struct M11R8 *)gUnk_02005550;
        {
            struct Task *t = gUnk_03002490;
            g8[t->unk88->unk00] = *(struct M11R8 *)gUnk_0873D044;
            t->unk2C = -1;
            *(u32 *)((u8 *)t->unk88 + 108) = 0;
        }
        sub_080031b8(266);
        gUnk_03002490->unk5C = 0;
        sub_08006338(0x120C);
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);

        sub_080061c0(0x60000, -0x4000);
        {
            struct Task *t = gUnk_03002490;
            t->unk64 = 0;
            *(u32 *)((u8 *)t->unk88 + 108) = (u32)&g8[t->unk88->unk00];
            sub_0803e5f8(t->unk88->unk00,
                         (s32)((u8 *)gUnk_0873D04C + (t->unk2C = 8) * 8));
        }
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);

        sub_080061c0(0x20000, 0);
        {
            struct Task *t = gUnk_03002490;
            t->unk64 = 0x20000;
            if ((t->unk7A & 1) != 0)
                sub_0805afac(t->unk88->unk00, 28, 3);
        }
        {
            struct Task *t = gUnk_03002490;
            t->unk88->unk14 = 8;
            sub_0803e5f8(t->unk88->unk00,
                         (s32)((u8 *)gUnk_0873D04C + ++t->unk2C * 8));
        }
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        {
            struct Task *t = gUnk_03002490;
            sub_0803e5f8(t->unk88->unk00,
                         (s32)((u8 *)gUnk_0873D04C + ++t->unk2C * 8));
        }
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);

        sub_080061c0(0x8000, 0x5A5A5A5A);
        {
            struct Task *t = gUnk_03002490;
            t->unk2C = -1;
            *(u32 *)((u8 *)t->unk88 + 108) = 0;
            t->unk3C++;
        }
        TaskYieldTrampoline(10);
        sub_0803e050(1);

        if (gUnk_03002490->unk30 == 0) {
            TaskYieldTrampoline(10);
        } else {
            sub_080031b8(0x10B);
            sub_08006338(0x121B);
            TaskYieldTrampoline(4);
            sub_080061c0(-0x800, 0x800);
            {
                struct Task *t = gUnk_03002490;
                t->unk64 = 0;
                t->unk2C = 8;
                *(u32 *)((u8 *)t->unk88 + 108) = (u32)&g8[t->unk88->unk00];
                t->unk6C = 0;
            }
            do {
                struct Task *t = gUnk_03002490;
                sub_0803e5f8(t->unk88->unk00,
                             (s32)((u8 *)gUnk_0873D04C + --t->unk2C * 8));
                gUnk_03002490->unk3C--;
                TaskYieldTrampoline(1);
            } while ((s16)++gUnk_03002490->unk6C <= 7);
            sub_080061c0(-0x4000, 0);
            {
                struct Task *t = gUnk_03002490;
                t->unk64 = 0x4000;
                t->unk2C = -1;
                *(u32 *)((u8 *)t->unk88 + 108) = 0;
                t->unk3C--;
            }
            TaskYieldTrampoline(4);
        }
    }
    gUnk_03002490->unk28++;
    sub_08006138();
}


void sub_08043e28(void)
{
    if (gUnk_03002490->unk28 != 0) {
        if ((gUnk_03002490->unk7A & 1) == 0) {
            if ((gUnk_03002490->unk7B & 1) == 0)
                gUnk_03002490->unk88->unk01 = 7;
            else
                gUnk_03002490->unk88->unk01 = 23;
        } else {
            u16 *q = (u16 *)gUnk_03002458;
            if ((q[gUnk_03002490->unk88->unk00] & 48) != 0) {
                sub_0803f870();
                if ((gUnk_03002490->unk7B & 1) == 0) {
                    *((u8 *)gUnk_03002490->unk88 + 61) = 1;
                    gUnk_03002490->unk88->unk01 = 3;
                } else {
                    *((u8 *)gUnk_03002490->unk88 + 61) = 0;
                    gUnk_03002490->unk88->unk01 = 23;
                }
            } else {
                *((u8 *)gUnk_03002490->unk88 + 61) = 0;
                if ((gUnk_03002490->unk7B & 1) == 0)
                    gUnk_03002490->unk88->unk01 = 1;
                else
                    gUnk_03002490->unk88->unk01 = 24;
            }
        }
    } else {
        if (gUnk_03002490->unk2C != -1) {
            sub_0803e5c0(gUnk_03002490->unk88->unk00,
                         (u8 *)gUnk_0873CAA4 + gUnk_03002490->unk2C * 8);
            sub_0801a828((u8)gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                         (u8 *)gUnk_020060E0 + gUnk_03002490->unk88->unk00 * 20);
        }
        {
            struct Task *t = gUnk_03002490;
            struct PlayerState *p = t->unk88;
            if ((s16)p->unk14 != 0) {
                p->unk14--;
                {
                    u16 *q = (u16 *)gUnk_030023C0;
                    if (q[t->unk88->unk00] & 2)
                        t->unk30 = 1;
                }
            }
        }
    }
    if ((gUnk_03002490->unk7A & 1) == 0)
        sub_080413a4(2);
    else
        sub_0803e4ec(1);
}


/* NOTE: fwd.h gives this four s32 args; that is an arity.py artifact -
   the script does not model `ldmia rN!, {r2, r3, r4}` (the 20-byte struct
   copy in this body) as WRITING r2/r3, so it reads them as incoming
   arguments.  The real prototype is `void sub_08043fa8(void)`, confirmed by
   the anchor table at 0x0873B46C whose entries are `void (*)(void)`.
   The unused parameters are kept only so this file compiles against the
   current fwd.h; they cost no code. */
void sub_08043fa8(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 25;
    gUnk_03002490->unk80 = 4;
    {
        struct Task *t = gUnk_03002490;
        t->unk28 = 0;
        {
            struct M11R20 *d = (struct M11R20 *)gUnk_020060E0;
            d[t->unk88->unk00] = *(struct M11R20 *)gUnk_0873CA90;
        }
    }
    {
        struct M11R8 *g8 = (struct M11R8 *)gUnk_02005550;
        struct Task *t = gUnk_03002490;
        g8[t->unk88->unk00] = *(struct M11R8 *)gUnk_0873D044;
        t->unk2C = -1;
        *(u32 *)((u8 *)t->unk88 + 108) = 0;
    }
    sub_0803e050(2);
    sub_08040b40(0, 72);
    sub_0803e650(19);
    if ((gUnk_03002490->unk7A & 1) != 0) {
        sub_08006338(0x121C);
        TaskYieldTrampoline(6);
    } else {
        sub_08006338(0x1223);
        TaskYieldTrampoline(6);
    }
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk7A = 0;
    sub_080031b8(0x10B);
    sub_0800622c(-0x40000, 0x8000, 0x40000);
    sub_08006338(0x121E);
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    sub_0803e050(2);
    {
        struct Task *t = gUnk_03002490;
        *(u32 *)((u8 *)t->unk88 + 108) =
            (u32)((u8 *)gUnk_02005550 + t->unk88->unk00 * 8);
        sub_0803e5f8(t->unk88->unk00,
                     (s32)((u8 *)gUnk_0873D04C + (t->unk2C = 11) * 8));
    }
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    {
        struct Task *t = gUnk_03002490;
        sub_0803e5f8(t->unk88->unk00,
                     (s32)((u8 *)gUnk_0873D04C + ++t->unk2C * 8));
    }
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    {
        struct Task *t = gUnk_03002490;
        t->unk2C = -1;
        *(u32 *)((u8 *)t->unk88 + 108) = 0;
        t->unk3C++;
    }
    TaskYieldTrampoline(5);
    sub_0800622c(0x40000, -0x8000, 0x40000);
    sub_08006338(0x121F);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk28++;
    sub_08006138();
}


void sub_080441cc(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;

    if (t->unk28 != 0) {
        sub_0804042c();
    } else if (t->unk2C != -1) {
        sub_0803e5c0(t->unk88->unk00, (u8 *)gUnk_0873CAA4 + t->unk2C * 8);
        sub_0801a828((u8)gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                     (u8 *)gUnk_020060E0 + gUnk_03002490->unk88->unk00 * 20);
    }
    u = gUnk_03002490;
    if ((u->unk7A & 1) == 0) {
        if ((u->unk7B & 1) == 0)
            sub_080413a4(2);
        else
            sub_080413a4(13);
    } else {
        sub_0803e4ec(1);
    }
    sub_0803f9e8();
}


/* NOTE: fwd.h gives this four s32 args; that is an arity.py artifact -
   the script does not model `ldmia rN!, {r2, r3, r4}` (the 20-byte struct
   copy in this body) as WRITING r2/r3, so it reads them as incoming
   arguments.  The real prototype is `void sub_08044288(void)`, confirmed by
   the anchor table at 0x0873B46C whose entries are `void (*)(void)`.
   The unused parameters are kept only so this file compiles against the
   current fwd.h; they cost no code. */
void sub_08044288(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 26;
    {
        struct Task *t = gUnk_03002490;
        if (t->unk88->unk05 != 13) {
            t->unk73 = 0;
            {
                struct M11R20 *d = (struct M11R20 *)gUnk_020060E0;
                d[gUnk_03002490->unk88->unk00] = *(struct M11R20 *)gUnk_0873CA90;
            }
            {
                struct M11R8 *g8 = (struct M11R8 *)gUnk_02005550;
                struct Task *u = gUnk_03002490;
                g8[u->unk88->unk00] = *(struct M11R8 *)gUnk_0873D044;
                u->unk2C = -1;
                *(u32 *)((u8 *)u->unk88 + 108) = 0;
            }
            sub_080031b8(0x10B);
            gUnk_03002490->unk80 = 4;
        }
    }
    switch (gUnk_03002490->unk73) {
    case 0:
        sub_0803e050(3);
        {
            struct Task *t = gUnk_03002490;
            t->unk88->unk14 = 10;
            *(u32 *)((u8 *)t->unk88 + 108) =
                (u32)((u8 *)gUnk_02005550 + t->unk88->unk00 * 8);
            sub_0803e5f8(t->unk88->unk00,
                         (s32)((u8 *)gUnk_0873D04C + (t->unk2C = 13) * 8));
        }
        sub_08006338(0x1225);
        TaskYieldTrampoline(2);
        {
            struct Task *t = gUnk_03002490;
            t->unk2C = -1;
            *(u32 *)((u8 *)t->unk88 + 108) = 0;
            t->unk3C++;
        }
        TaskYieldTrampoline(4);
        sub_0800622c(0x60000, 0, 0x60000);
        {
            struct Task *t = gUnk_03002490;
            *(u32 *)((u8 *)t->unk88 + 108) =
                (u32)((u8 *)gUnk_02005550 + t->unk88->unk00 * 8);
            sub_0803e5f8(t->unk88->unk00,
                         (s32)((u8 *)gUnk_0873D04C + (t->unk2C = 14) * 8));
        }
        break;
    case 1:
        sub_080261d4(2);
        sub_080031b8(0x10F);
        sub_0805afac(gUnk_03002490->unk88->unk00, 39, 0);
        sub_0805afac(gUnk_03002490->unk88->unk00, 39, 1);
        {
            struct Task *t = gUnk_03002490;
            t->unk2C = -1;
            *(u32 *)((u8 *)t->unk88 + 108) = 0;
            sub_0801a828((u8)gCurTaskIdx, t->unk48, t->unk4A,
                         (u8 *)gUnk_0873CA7C);
        }
        sub_08006338(0x1227);
        TaskYieldTrampoline(8);
        gUnk_03002490->unk73 = 2;
        break;
    }
    sub_08006138();
}


void sub_08044470(void)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk73) {
    case 0:
        if (sub_0803fa44() != 0) {
            sub_0803e050(3);
            gUnk_03002490->unk73 = 1;
            sub_08006148(sub_08044288, gCurTaskIdx);
            break;
        }
        {
            struct Task *u = gUnk_03002490;
            if (u->unk58 != 0) {
                struct PlayerState *p = u->unk88;
                if ((s16)p->unk14 == 0) {
                    u16 *q = (u16 *)gUnk_030023C0;
                    if ((q[p->unk00] & 2) != 0) {
                        if ((u->unk7B & 1) == 0) {
                            sub_080413a4(2);
                            gUnk_03002490->unk88->unk01 = 7;
                            break;
                        } else {
                            sub_0803e4a8();
                            gUnk_03002490->unk88->unk01 = 23;
                            break;
                        }
                    }
                } else {
                    p->unk14--;
                }
                {
                    u16 *q = (u16 *)gUnk_03002458;
                    struct Task *v = gUnk_03002490;
                    if ((q[v->unk88->unk00] & 48) != 0) {
                        if ((q[v->unk88->unk00] & 16) != 0)
                            v->unk5C = 0x2000;
                        else
                            v->unk5C = -0x2000;
                        v->unk64 = 0x1DE00;
                    } else if ((v->unk7B & 1) == 0) {
                        sub_08040b40(0, 72);
                    } else {
                        sub_08040b40(8, 72);
                    }
                }
            }
            if (sub_0803fce4(0) != 0) {
                gUnk_03002490->unk88->unk01 = 23;
                break;
            }
            {
                struct Task *w = gUnk_03002490;
                if (w->unk2C != -1) {
                    sub_0803e5c0(w->unk88->unk00,
                                 (u8 *)gUnk_0873CAA4 + w->unk2C * 8);
                    sub_0801a828((u8)gCurTaskIdx, gUnk_03002490->unk48,
                                 gUnk_03002490->unk4A,
                                 (u8 *)gUnk_020060E0
                                     + gUnk_03002490->unk88->unk00 * 20);
                }
            }
        }
        break;
    case 1:
        if ((t->unk7A & 1) != 0) {
            sub_0803e4ec(1);
            break;
        }
        /* fallthrough */
    case 2:
        sub_0804042c();
        break;
    }
    sub_0803f9e8();
}


void sub_0804462c(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 29;
    {
        struct Task *t = gUnk_03002490;
        if (t->unk88->unk05 != 13) {
            struct Task *u;
            t->unk73 = 0;
            u = gUnk_03002490;
            u->unk28 = 15;
            u->unk80 = 1;
        }
    }
    switch (gUnk_03002490->unk73) {
    case 0:
        sub_08006338(0x289);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(6);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(2);
        sub_08006338(652);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk73 = 1;
        /* fallthrough */
    case 1:
        {
            struct PlayerState *p = gUnk_03002490->unk88;
            if ((p->unk42 & 128) == 0)
                sub_0803e374(128, p->unk00);
        }
        sub_08053940(gUnk_03002490->unk88->unk00, 4, 0);
        sub_08053940(gUnk_03002490->unk88->unk00, 4, 1);
        sub_0805afac(gUnk_03002490->unk88->unk00, 29, 0);
        sub_0805afac(gUnk_03002490->unk88->unk00, 29, 1);
        sub_0805afac(gUnk_03002490->unk88->unk00, 29, 2);
        sub_0805afac(gUnk_03002490->unk88->unk00, 28, 4);
        while (1) {
            sub_08006338(0x279);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk6C = 0;
            do {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
            } while ((s16)++gUnk_03002490->unk6C <= 14);
        }
    case 2:
        sub_08006338(0x28D);
        TaskYieldTrampoline(3);
        sub_0803e3ac();
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk73 = 3;
        break;
    }
    sub_08006138();
}


void sub_08044800(void)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk73) {
    case 0:
        break;
    case 1:
        if (t->unk28 == 0) {
            u16 *p = (u16 *)gUnk_03002458;
            if ((p[t->unk88->unk00] & 2) == 0) {
                t->unk73 = 2;
                sub_08006148(sub_0804462c, gCurTaskIdx);
            }
        } else {
            t->unk28--;
        }
        break;
    case 2:
        break;
    case 3:
        sub_0804042c();
        break;
    }
    sub_0803e55c();
}


void sub_08044878(void)
{
    gUnk_03002490->unk88->unk05 = gUnk_03002490->unk88->unk04;
    gUnk_03002490->unk88->unk04 = 13;
    gUnk_03002490->unk15 = 30;
    {
        struct Task *t = gUnk_03002490;
        if (t->unk88->unk05 != 13) {
            struct Task *u;
            t->unk73 = 0;
            u = gUnk_03002490;
            u->unk28 = 15;
            u->unk80 = 2;
        }
    }
    switch (gUnk_03002490->unk73) {
    case 0:
        sub_08006338(0x36A);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk73 = 1;
        /* fallthrough */
    case 1:
        {
            struct Task *t = gUnk_03002490;
            t->unk70 = 0;
            t->unk6E = 0;
            sub_0805afac(t->unk88->unk00, 30, 0);
        }
        sub_0805afac(gUnk_03002490->unk88->unk00, 30, 1);
        sub_0805afac(gUnk_03002490->unk88->unk00, 30, 2);
        sub_0803e374(136, gUnk_03002490->unk88->unk00);
        while (1) {
            sub_08006338(0x36B);
            TaskYieldTrampoline(2);
            gUnk_03002490->unk6C = 0;
            do {
                gUnk_03002490->unk3C++;
                TaskYieldTrampoline(2);
            } while ((s16)++gUnk_03002490->unk6C <= 6);
        }
    case 2:
        gUnk_03002490->unk88->unk42 &= 0xFFEF;
        sub_0803e3ac();
        sub_08006338(0x36A);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk73 = 3;
        break;
    }
    sub_08006138();
}
