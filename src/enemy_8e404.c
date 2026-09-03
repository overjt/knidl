#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;

/* ROM tables */
extern u32 gUnk_0873F500[];
extern u32 gUnk_087431EC[];
extern u32 gUnk_087431FC[];
extern u32 gUnk_0874320C[];
extern u32 gUnk_08743210[];
extern u32 gUnk_08743214[];
extern u32 gUnk_0874321C[];
extern u32 gUnk_08743224[];
extern u32 gUnk_08743230[];
extern u32 gUnk_0874323C[];
extern u32 gUnk_08743240[];
extern u32 gUnk_08743244[];
extern u8 gUnk_08743248[];
extern s8 gUnk_0874324C[];
extern s8 gUnk_08743251[];
extern s8 gUnk_08743256[];
extern u32 gUnk_08743284[];
extern u32 gUnk_0874329C[];
extern u32 gUnk_087432A8[];
extern u32 gUnk_087432B4[];
extern u32 gUnk_087432C0[];
extern u32 gUnk_087432CC[];
extern u32 gUnk_087432D8[];
extern u32 gUnk_08743390[];
extern u32 gUnk_087433E8[];
extern u32 gUnk_08743558[];
extern u32 gUnk_087524A4[];
extern u32 gUnk_087524E4[];

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u32 sub_08002ee8(u32 range);
extern void sub_080059d8(void);
extern void sub_08006138(void);
extern void sub_08006148(void *a, u32 i);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_0800634c(s32 a);
extern void sub_08006364(s32 a);
extern void sub_0806395c(u16 v);
extern void sub_080639a4(void *p);
extern void sub_080639b4(void *p);
extern s32 sub_08063b38(void);
extern s32 sub_08063cd0(void);
extern void sub_08063e14(void);
extern void sub_08064038(void);
extern void sub_0806421c(s32 a, s32 b);
extern u8 sub_08064398(void);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064cdc(u32 type, s16 dx, s16 dy, u8 keepPrio);
extern void sub_0806523c(void);
extern void sub_08066b34(u32 *p);
extern void sub_08066b70(void);
extern void sub_08066c08(u32 *p, s32 b);
extern void sub_08066c3c(u32 *p);
extern void sub_08068e04(void);
extern u8 sub_080692fc(void);
extern u8 sub_08069888(void);
extern u32 sub_08069b44(void);
extern void sub_0806a0f0(s32 a);
extern void sub_0806a344(void);
extern void sub_0806ee2c(void);
extern void sub_0808e070(void);
extern void sub_0808e254(void);
extern void sub_0808e2b4(void);
extern void sub_0808e33c(void);
extern void sub_0808e36c(void);

/* Forward declarations */
void sub_0808e404(void);
void sub_0808e440(void);
void sub_0808e464(void);
void sub_0808e480(void);
void sub_0808e510(void);
void sub_0808e54c(void);
void sub_0808e5cc(void);
void sub_0808e610(void);
void sub_0808e704(void);
void sub_0808e730(void);
void sub_0808e800(void);
void sub_0808e804(void);
void sub_0808e848(void);
void sub_0808e870(void);
void sub_0808e8a0(void);
s32 sub_0808e8a4(void);
s32 sub_0808e8c4(void);
void sub_0808e8d4(void);
void sub_0808e914(void);
void sub_0808e964(void);
void sub_0808e994(void);
void sub_0808e9b0(void);
void sub_0808e9d4(void);
void sub_0808ea00(void);
void sub_0808eb10(void);
void sub_0808eb24(void);
void sub_0808eb64(void);
void sub_0808eb94(void);
void sub_0808ebdc(void);
s32 sub_0808ebe0(void);
s32 sub_0808ec34(void);
s32 sub_0808ec90(void);
s32 sub_0808ecb4(void);
s32 sub_0808ece0(void);
s32 sub_0808ed0c(void);
void sub_0808ed38(void);
void sub_0808ee60(void);
void sub_0808ee9c(void);
void sub_0808eec4(u16 a);
void sub_0808ef88(void);
void sub_0808efdc(void);
void sub_0808f058(void);
void sub_0808f0d0(void);
void sub_0808f1b4(u16 a, void *b);
void sub_0808f224(void);
void sub_0808f26c(void);
void sub_0808f2a0(void);
void sub_0808f2c4(void);
void sub_0808f2fc(void);
void sub_0808f320(void);
void sub_0808f35c(void);
void sub_0808f380(void);
void sub_0808f39c(void);
void sub_0808f3b8(void);
void sub_0808f3d4(void);
void sub_0808f400(void);

void sub_0808e404(void)
{
    gUnk_03002490->unk7A = 0;
    gUnk_03002490->unk04 = (u32)sub_0808e440;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_087431EC);
}

void sub_0808e440(void)
{
    sub_08002e98(gUnk_03002490->unk15, 4, gUnk_087431FC);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808e464(void)
{
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_087431EC);
}

void sub_0808e480(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk30 = 16;
    sub_080062c4();
    while (1)
    {
        switch (gUnk_03002490->unk20 = sub_08064398())
        {
        case 8:
            if (gUnk_03002490->unk1C != 0)
            {
                sub_0800634c(5);
                TaskYieldTrampoline(8);
                sub_08006364(5);
                TaskYieldTrampoline(8);
                gUnk_03002490->unk1C = 0;
            }
            sub_08006364(4);
            TaskYieldTrampoline(8);
            break;
        case 4:
            if (gUnk_03002490->unk1C != 0)
            {
                sub_08006364(5);
                TaskYieldTrampoline(8);
                sub_0800634c(5);
                TaskYieldTrampoline(8);
                gUnk_03002490->unk1C = 0;
            }
            sub_0800634c(4);
            TaskYieldTrampoline(8);
            break;
        default:
            TaskYieldTrampoline(1);
            break;
        }
    }
}

void sub_0808e510(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (t->unk20 != sub_08064398())
        gUnk_03002490->unk1C = 1;
    sub_0808e070();
    sub_0808e2b4();
    u = gUnk_03002490;
    u->unk30--;
    sub_0808e33c();
    sub_0808e36c();
}

void sub_0808e54c(void)
{
    gUnk_03002490->unk15 = 2;
    gUnk_03002490->unk30 = 0;
    sub_080062c4();
    while (1)
    {
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk58 = 0xFFFFF000;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk58 = 0xFFFFF800;
        TaskYieldTrampoline(16);
        gUnk_03002490->unk58 = 128 << 4;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 128 << 5;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 128 << 4;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0xFFFFF800;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0xFFFFF000;
        TaskYieldTrampoline(8);
    }
}

void sub_0808e5cc(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk30++;
    t->unk48 = t->unk2C;
    if (t->unk30 > 5 && sub_08002ee8(2) != 0)
    {
        sub_0806395c(1);
        sub_08006148(sub_0808e464, gCurTaskIdx);
    }
}

void sub_0808e610(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;

    gUnk_03002490->unk15 = 1;
    sub_08063e14();
    gUnk_03002490->unk30 = 0;
    sub_080062c4();
    t = gUnk_03002490;
    switch (t->unk43)
    {
    case 1:
        t->unk34 = 0;
        break;
    case -1:
        t->unk34 = 1;
        break;
    }
    gUnk_03002490->unk6E = 0;
    do
    {
        sub_08006338(6);
        TaskYieldTrampoline(2);
        sub_08006338(7);
        TaskYieldTrampoline(2);
        sub_08006338(4);
        TaskYieldTrampoline(2);
        u = gUnk_03002490;
        u->unk6E++;
    } while ((s16)u->unk6E <= 7);
    gUnk_03002490->unk6C = 0;
    while ((s16)gUnk_03002490->unk6C < sub_08002ee8(3) + 1)
    {
        sub_08006338(6);
        TaskYieldTrampoline(2);
        sub_08006338(7);
        TaskYieldTrampoline(2);
        sub_08006338(4);
        TaskYieldTrampoline(2);
        sub_0808e254();
        sub_08006338(4);
        TaskYieldTrampoline(8);
        w = gUnk_03002490;
        w->unk6C++;
    }
    gUnk_03002490->unk30 = 1;
    sub_08006138();
}

void sub_0808e704(void)
{
    if (gUnk_03002490->unk30 != 0)
    {
        sub_0806395c(3);
        sub_08006148(sub_0808e464, gCurTaskIdx);
    }
}

void sub_0808e730(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    gUnk_03002490->unk15 = 3;
    t = gUnk_03002490;
    t->unk1C = 1;
    t->unk60 = 0xFFFFF000;
    while (1)
    {
        switch (sub_08064398())
        {
        case 4:
            sub_0806421c(160 << 1, 102);
            u = gUnk_03002490;
            u->unk54 = gUnk_030023B4;
            if (u->unk1C != 0)
            {
                sub_0800634c(5);
                TaskYieldTrampoline(8);
                sub_08006364(5);
                TaskYieldTrampoline(8);
                gUnk_03002490->unk1C = 0;
            }
            sub_08006364(4);
            TaskYieldTrampoline(8);
            break;
        case 8:
            sub_0806421c(224 << 1, 102);
            v = gUnk_03002490;
            v->unk54 = gUnk_030023B4;
            if (v->unk1C != 0)
            {
                sub_08006364(5);
                TaskYieldTrampoline(8);
                sub_0800634c(5);
                TaskYieldTrampoline(8);
                gUnk_03002490->unk1C = 0;
            }
            sub_0800634c(4);
            TaskYieldTrampoline(8);
            break;
        default:
            TaskYieldTrampoline(1);
            break;
        }
    }
}

void sub_0808e800(void)
{
}

void sub_0808e804(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808e848;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_0874320C);
}

void sub_0808e848(void)
{
    sub_080692fc();
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08743210);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808e870(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08063e14();
    gUnk_03002490->unk7A = 1;
    sub_080062c4();
    sub_08006338(4);
    sub_08006138();
}

void sub_0808e8a0(void)
{
}

s32 sub_0808e8a4(void)
{
    sub_0806395c(2);
    sub_08006148(sub_0808e994, gCurTaskIdx);
    return 1;
}

s32 sub_0808e8c4(void)
{
    sub_0806a0f0(-2);
    return 1;
}

void sub_0808e8d4(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_087524A4;
    sub_08002e98(u->unk73, 3, gUnk_08743224);
}

void sub_0808e914(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk04 = (u32)sub_0808e964;
    switch (t->unk73)
    {
    case 0:
        t->unk43 = 1;
        break;
    case 1:
        t->unk43 = 255;
        break;
    }
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08743230);
}

void sub_0808e964(void)
{
    if (sub_08069888() == 0)
        sub_08002e98(gUnk_03002490->unk15, 1, gUnk_0874323C);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808e994(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08743230);
}

void sub_0808e9b0(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk7A = 0;
    sub_08006338(5);
    sub_08006138();
}

void sub_0808e9d4(void)
{
    s32 v;

    v = sub_08063cd0();
    if (abs(v) <= 7)
    {
        sub_0806395c(1);
        sub_08006148(sub_0808e994, gCurTaskIdx);
    }
}

void sub_0808ea00(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk60 = gUnk_08743214[t->unk74];
    t->unk68 = gUnk_0874321C[t->unk74];
    while (1)
    {
        sub_08006338(7);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(2);
        sub_08064038();
        gUnk_03002490->unk3C -= 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(2);
        sub_08064038();
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C += 1;
        TaskYieldTrampoline(2);
        sub_08064038();
        gUnk_03002490->unk3C -= 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C -= 1;
        TaskYieldTrampoline(2);
        sub_08064038();
        gUnk_03002490->unk3C -= 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C -= 1;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C -= 1;
        TaskYieldTrampoline(2);
    }
}

void sub_0808eb10(void)
{
    sub_080639a4(gUnk_08743558);
    sub_0806a344();
}

void sub_0808eb24(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808eb64;
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08743240);
}

void sub_0808eb64(void)
{
    if (sub_08069888() == 0)
        sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08743244);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808eb94(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk7A = 1;
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(6);
        sub_08006338(5);
        TaskYieldTrampoline(6);
        sub_08006338(6);
        TaskYieldTrampoline(6);
        sub_08006338(5);
        TaskYieldTrampoline(6);
    }
}

void sub_0808ebdc(void)
{
}

s32 sub_0808ebe0(void)
{
    switch (gUnk_03002490->unk73)
    {
    default:
        sub_0806395c(1);
        sub_08006148(sub_0808f39c, gCurTaskIdx);
        return 1;
    case 0:
        sub_0806395c(1);
        sub_08006148(sub_0808f380, gCurTaskIdx);
        return 1;
    case 4:
        sub_0806395c(2);
        sub_08006148(sub_0808f3b8, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808ec34(void)
{
    switch (gUnk_03002490->unk73)
    {
    default:
        sub_0806395c(0);
        sub_08006148(sub_0808f39c, gCurTaskIdx);
        return 1;
    case 0:
        sub_0806395c(0);
        sub_08006148(sub_0808f380, gCurTaskIdx);
        return 1;
    case 4:
        sub_08066c3c(gUnk_08743390);
        sub_0806395c(0);
        sub_08006148(sub_0808f3b8, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808ec90(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk73 == 4 && t->unk14 == 2)
        sub_08066b70();
    return 0;
}

s32 sub_0808ecb4(void)
{
    if (gUnk_03002490->unk73 == 4)
        sub_08066c08(gUnk_08743390, 0);
    sub_0806a0f0(-2);
    return 1;
}

s32 sub_0808ece0(void)
{
    sub_08066c08(gUnk_08743390, 0);
    sub_0806395c(2);
    sub_08006148(sub_0808f3b8, gCurTaskIdx);
    return 1;
}

s32 sub_0808ed0c(void)
{
    sub_08066c08(gUnk_08743390, 0);
    sub_0806395c(2);
    sub_08006148(sub_0808f3b8, gCurTaskIdx);
    return 1;
}

void sub_0808ed38(void)
{
    s32 dx;
    s32 dy;

    dx = (s16)((u16)(&gUnk_03002790[sub_08063b38()])->unk48 - (u16)gUnk_03002490->unk48);
    dy = (s16)((u16)(&gUnk_03002790[sub_08063b38()])->unk4A - (u16)gUnk_03002490->unk4A);
    switch (gUnk_03002490->unk30 = (u16)ArcTan2(dx, dy) >> 12)
    {
    case 0:
    case 15:
        gUnk_03002490->unk30 = 0;
        gUnk_03002490->unk20 = 1;
        break;
    case 1:
    case 2:
        gUnk_03002490->unk30 = 0;
        gUnk_03002490->unk20 = 0;
        break;
    case 3:
    case 4:
        gUnk_03002490->unk30 = 2;
        gUnk_03002490->unk20 = 0;
        break;
    case 5:
    case 6:
        gUnk_03002490->unk30 = 4;
        gUnk_03002490->unk20 = 0;
        break;
    case 7:
    case 8:
        gUnk_03002490->unk30 = 4;
        gUnk_03002490->unk20 = 1;
        break;
    case 9:
    case 10:
        gUnk_03002490->unk30 = 3;
        gUnk_03002490->unk20 = 1;
        break;
    case 11:
    case 12:
        gUnk_03002490->unk30 = 2;
        gUnk_03002490->unk20 = 1;
        break;
    case 13:
    case 14:
        gUnk_03002490->unk30 = 1;
        gUnk_03002490->unk20 = 1;
        break;
    }
}

void sub_0808ee60(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk34 > t->unk30)
    {
        if (t->unk34 > 0)
        {
            t->unk18 = t->unk34;
            t->unk34--;
        }
        else
        {
            t->unk18 = t->unk34;
        }
    }
    else if (t->unk34 < t->unk30)
    {
        if (t->unk34 <= 3)
        {
            t->unk18 = t->unk34;
            t->unk34++;
        }
        else
        {
            t->unk18 = t->unk34;
        }
    }
    else if (t->unk34 == t->unk30)
    {
        t->unk18 = t->unk34;
        t->unk1C = 1;
    }
}

void sub_0808ee9c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (t->unk34 > t->unk30)
        t->unk1C = 0;
    else if (t->unk34 < t->unk30)
        t->unk1C = 0;
    else if (t->unk34 == t->unk30)
        t->unk1C = 1;
}

void sub_0808eec4(u16 a)
{
    struct Task *t;

    switch (gUnk_03002490->unk73)
    {
    case 0:
    case 4:
        switch (gUnk_03002490->unk34)
        {
        case 0:
            sub_0806421c(0, (s16)a);
            break;
        case 1:
            sub_0806421c(224 << 1, (s16)a);
            break;
        case 2:
            sub_0806421c(192 << 1, (s16)a);
            break;
        case 3:
            sub_0806421c(160 << 1, (s16)a);
            break;
        case 4:
            sub_0806421c(128 << 1, (s16)a);
            break;
        }
        break;
    case 1:
        sub_0806421c(192 << 1, (s16)a);
        break;
    case 2:
        sub_0806421c(224 << 1, (s16)a);
        break;
    case 3:
        sub_0806421c(160 << 1, (s16)a);
        break;
    default:
        sub_0806ee2c();
        break;
    }
    t = gUnk_03002490;
    t->unk54 = gUnk_030023B4;
    t->unk58 = gUnk_030023D4;
}

void sub_0808ef88(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    switch (sub_08064398())
    {
    default:
        t = gUnk_03002490;
        t->unk30 = 0;
        t->unk34 = 0;
        break;
    case 4:
        u = gUnk_03002490;
        u->unk30 = 0;
        u->unk34 = 0;
        sub_0800634c(4);
        break;
    case 8:
        v = gUnk_03002490;
        v->unk30 = 4;
        v->unk34 = 4;
        sub_08006364(4);
        break;
    }
}

void sub_0808efdc(void)
{
    struct ActorSpawn sp;

    sp.unk00 = 7;
    sp.unk04 = 109;
    sp.unk08 = 0;
    sp.unk09 = gUnk_03002490->unk74;
    sp.unk0C = gUnk_0874324C[gUnk_03002490->unk30];
    sp.unk0E = gUnk_08743251[gUnk_03002490->unk30];
    sp.unk0A = 1;
    sub_08064b5c(&sp, 0);
    gUnk_03002490->unk46 = sub_08064cdc(172, gUnk_0874324C[gUnk_03002490->unk30], gUnk_08743251[gUnk_03002490->unk30], 0);
}

void sub_0808f058(void)
{
    struct ActorSpawn sp;

    sp.unk00 = 7;
    sp.unk04 = 109;
    sp.unk08 = 0;
    sp.unk09 = 4;
    sp.unk0C = gUnk_08743256[gUnk_03002490->unk34];
    sp.unk0E = gUnk_08743251[gUnk_03002490->unk34];
    sp.unk0A = 1;
    sub_08064b5c(&sp, 0);
    gUnk_03002490->unk46 = sub_08064cdc(172, gUnk_08743256[gUnk_03002490->unk34], gUnk_08743251[gUnk_03002490->unk34], 0);
}

void sub_0808f0d0(void)
{
    sub_0808ed38();
    sub_0808ee60();
    switch (gUnk_03002490->unk34)
    {
    case 0:
        sub_0800634c(4);
        break;
    case 1:
        if (gUnk_03002490->unk18 == 2)
        {
            sub_0800634c(6);
            TaskYieldTrampoline(4);
        }
        sub_0800634c(5);
        break;
    case 2:
        switch (gUnk_03002490->unk18)
        {
        case 1:
            sub_0800634c(6);
            TaskYieldTrampoline(4);
            break;
        case 3:
            sub_08006364(6);
            TaskYieldTrampoline(4);
            break;
        }
        sub_0800634c(7);
        break;
    case 3:
        if (gUnk_03002490->unk18 == 2)
        {
            sub_08006364(6);
            TaskYieldTrampoline(4);
        }
        sub_08006364(5);
        break;
    case 4:
        sub_08006364(4);
        break;
    }
    TaskYieldTrampoline(gUnk_08743248[gUnk_03002490->unk74]);
}

void sub_0808f1b4(u16 a, void *b)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    if (--t->unk28 <= 0)
    {
        t->unk28 = gUnk_08743248[t->unk74];
        sub_0808ed38();
        sub_0808ee9c();
        u = gUnk_03002490;
        if (u->unk1C != 0)
        {
            u->unk1C = 0;
            if (u->unk2C != 0)
            {
                if (u->unk20 != 0)
                {
                    sub_0806395c(a);
                    sub_08006148(b, gCurTaskIdx);
                }
            }
            else
            {
                u->unk2C = 1;
            }
        }
    }
}

void sub_0808f224(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_087524E4;
    u->unk8C->unk1A = 4;
    sub_08002e98(u->unk73, 6, gUnk_08743284);
}

void sub_0808f26c(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808f2a0;
    sub_0806395c(0);
    sub_0808ef88();
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_0874329C);
}

void sub_0808f2a0(void)
{
    sub_08002e98(gUnk_03002490->unk15, 3, gUnk_087432A8);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808f2c4(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808f2fc;
    sub_0806395c(0);
    gUnk_03002490->unk43 = 255;
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087432B4);
}

void sub_0808f2fc(void)
{
    sub_08002e98(gUnk_03002490->unk15, 3, gUnk_087432C0);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808f320(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808f35c;
    sub_0806395c(0);
    sub_0808ef88();
    sub_08066b34(gUnk_087433E8);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087432CC);
}

void sub_0808f35c(void)
{
    sub_08002e98(gUnk_03002490->unk15, 3, gUnk_087432D8);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808f380(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_0874329C);
}

void sub_0808f39c(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087432B4);
}

void sub_0808f3b8(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087432CC);
}

void sub_0808f3d4(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    t = gUnk_03002490;
    t->unk28 = gUnk_08743248[t->unk74];
    sub_08006244();
    while (1)
        sub_0808f0d0();
}

void sub_0808f400(void)
{
    if (sub_08069888() == 0)
        sub_0808f1b4(2, sub_0808f380);
}

