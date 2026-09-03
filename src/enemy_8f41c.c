#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;

/* ROM tables */
extern u32 gUnk_0873F500[];
extern u8 gUnk_08743248[];
extern s16 gUnk_0874325A[][4];
extern u32 gUnk_087432E4[];
extern u32 gUnk_087432E8[];
extern u8 gUnk_087432EC[][4];
extern u32 gUnk_087432F4[];
extern u32 gUnk_087432FC[];
extern u32 gUnk_08743308[];
extern u32 gUnk_08743600[];
extern u32 gUnk_08743604[];
extern u32 gUnk_0874360C[];
extern u8 gUnk_08743614[];
extern s16 gUnk_0874361A[];
extern u32 gUnk_0874362C[];
extern u32 gUnk_0874363C[];
extern u32 gUnk_08743640[];
extern u32 gUnk_0875227C[];
extern u32 gUnk_08752520[];
extern u32 gUnk_08752A70[];

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern s32 sub_080031b8(s32 id);
extern void sub_080059d8(void);
extern void sub_08006138(void);
extern void sub_08006148(void *a, u32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_0800634c(s32 a);
extern void sub_08006364(s32 a);
extern void sub_080224b0(void);
extern void sub_0806395c(u16 v);
extern void sub_080639b4(void *p);
extern void sub_08063e14(void);
extern void sub_08063fe0(void);
extern void sub_08063ff4(void);
extern void sub_080640c8(void);
extern void sub_0806421c(s32 a, s32 b);
extern void sub_08064a60(void);
extern void sub_0806523c(void);
extern void sub_08066ba8(void);
extern void sub_08066bdc(void);
extern void sub_08068e04(void);
extern u8 sub_080692fc(void);
extern u8 sub_08069604(void);
extern u8 sub_08069660(void);
extern u8 sub_08069888(void);
extern u32 sub_08069b44(void);
extern void sub_0806a344(void);
extern void sub_0806ee2c(void);
extern void sub_0808eec4(s32 a);
extern void sub_0808efdc(void);
extern void sub_0808f058(void);
extern void sub_0808f0d0(void);
extern void sub_0808f1b4(u16 a, void *b);
extern void sub_0808f380(void);
extern void sub_0808f39c(void);
extern void sub_0808f3b8(void);

/* Forward declarations */
void sub_0808f41c(void);
void sub_0808f4b4(void);
void sub_0808f4f8(void);
void sub_0808f51c(void);
void sub_0808f528(void);
void sub_0808f578(void);
void sub_0808f5cc(void);
void sub_0808f678(void);
void sub_0808f6c0(void);
void sub_0808f71c(void);
void sub_0808f728(void);
void sub_0808f75c(void);
void sub_0808f7ac(void);
void sub_0808f844(void);
void sub_0808f888(void);
void sub_0808f8dc(void);
void sub_0808f8e8(void);
void sub_0808f930(void);
void sub_0808f954(void);
void sub_0808f974(void);
s32 sub_0808f978(void);
s32 sub_0808f9b8(void);
s32 sub_0808f9d8(void);
s32 sub_0808f9f8(void);
s32 sub_0808fa04(void);
void sub_0808fa10(void);
void sub_0808fa50(void);
void sub_0808fa84(void);
void sub_0808fa98(void);
void sub_0808fab4(void);
void sub_0808fb50(void);
void sub_0808fb80(void);
void sub_0808fbac(void);
void sub_0808fbf0(void);
void sub_0808fc00(void);
void sub_0808fc40(void);
void sub_0808fc90(void);
void sub_0808fcd4(void);
void sub_0808fd1c(void);
void sub_0808fd38(void);
void sub_0808fd5c(void);
void sub_0808fd60(void);
void sub_0808fdb4(void);
void sub_0808fdb8(void);
void sub_0808fdf8(void);
void sub_0808fe28(void);
void sub_0808fe6c(void);
void sub_0808fe88(void);
void sub_0808ffe0(void);

void sub_0808f41c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    gUnk_03002490->unk15 = 2;
    sub_08063e14();
    t = gUnk_03002490;
    t->unk28 = 0;
    t->unk70 = 0;
    sub_080031b8(190);
    sub_0808efdc();
    u = gUnk_03002490;
    u->unk6E = 0;
    u->unk6C = 0;
    do
    {
        v = gUnk_03002490;
        sub_0808eec4(gUnk_0874325A[v->unk34][v->unk6E]);
        w = gUnk_03002490;
        w->unk6E++;
        TaskYieldTrampoline(2);
        sub_080062c4();
        x = gUnk_03002490;
        x->unk6C++;
    } while ((s16)x->unk6C <= 3);
    y = gUnk_03002490;
    y->unk70 = 1;
    y->unk28 = 1;
    y->unk2C = 0;
    sub_08006138();
}

void sub_0808f4b4(void)
{
    if ((s16)gUnk_03002490->unk70 != 0 && sub_08069888() == 0 && gUnk_03002490->unk28 != 0)
    {
        sub_0806395c(0);
        sub_08006148(sub_0808f380, gCurTaskIdx);
    }
}

void sub_0808f4f8(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 1;
    t = gUnk_03002490;
    t->unk60 = 168 << 5;
    t->unk68 = 192 << 10;
    sub_08006138();
}

void sub_0808f51c(void)
{
    sub_08069888();
}

void sub_0808f528(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08006244();
    switch (gUnk_03002490->unk73)
    {
    case 1:
        sub_0800634c(7);
        break;
    case 2:
        sub_0800634c(5);
        break;
    case 3:
        sub_08006364(5);
        break;
    }
    sub_08006138();
}

void sub_0808f578(void)
{
    struct Task *t;

    if (sub_08069888() == 0)
    {
        t = gUnk_03002490;
        switch (t->unk73)
        {
        case 1:
            t->unk34 = 2;
            break;
        case 2:
            t->unk34 = 1;
            break;
        case 3:
            t->unk34 = 3;
            break;
        }
        sub_0806395c(2);
        sub_08006148(sub_0808f39c, gCurTaskIdx);
    }
}

void sub_0808f5cc(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    gUnk_03002490->unk15 = 2;
    t = gUnk_03002490;
    t->unk28 = 0;
    t->unk70 = 1;
    while (1)
    {
        TaskYieldTrampoline(100);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk70 = 0;
            sub_080031b8(190);
            sub_0808f058();
            u = gUnk_03002490;
            u->unk2C = 0;
            u->unk6E = 0;
            do
            {
                v = gUnk_03002490;
                sub_0808eec4(gUnk_0874325A[v->unk73][v->unk2C]);
                w = gUnk_03002490;
                w->unk2C++;
                TaskYieldTrampoline(2);
                sub_080062c4();
                x = gUnk_03002490;
                x->unk6E++;
            } while ((s16)x->unk6E <= 3);
            gUnk_03002490->unk70 = 1;
            TaskYieldTrampoline(7);
            y = gUnk_03002490;
            y->unk6C++;
        } while ((s16)y->unk6C <= 2);
        gUnk_03002490->unk28 = 1;
    }
}

void sub_0808f678(void)
{
    if ((s16)gUnk_03002490->unk70 != 0 && sub_08069888() == 0 && gUnk_03002490->unk28 != 0)
    {
        sub_080062c4();
        sub_0806395c(2);
        sub_08006148(sub_0808f39c, gCurTaskIdx);
    }
}

void sub_0808f6c0(void)
{
    struct Task *t;

    switch (gUnk_03002490->unk73)
    {
    case 1:
        sub_0800634c(7);
        break;
    case 2:
        sub_0800634c(5);
        break;
    case 3:
        sub_08006364(5);
        break;
    }
    gUnk_03002490->unk15 = 1;
    t = gUnk_03002490;
    t->unk60 = 168 << 5;
    t->unk68 = 192 << 10;
    sub_08006138();
}

void sub_0808f71c(void)
{
    sub_08069888();
}

void sub_0808f728(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk74 = 1;
    t = gUnk_03002490;
    t->unk28 = gUnk_08743248[t->unk74];
    sub_080062c4();
    while (1)
        sub_0808f0d0();
}

void sub_0808f75c(void)
{
    if (gUnk_03002490->unk8C->unk1A == -1)
    {
        if (sub_08069888() == 0)
            sub_0808f1b4(1, sub_0808f3b8);
    }
    else if (sub_080692fc() == 0)
        sub_0808f1b4(1, sub_0808f3b8);
}

void sub_0808f7ac(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    gUnk_03002490->unk15 = 1;
    sub_08063e14();
    t = gUnk_03002490;
    t->unk28 = 0;
    t->unk70 = 0;
    sub_080031b8(190);
    sub_0808efdc();
    u = gUnk_03002490;
    u->unk6E = 0;
    u->unk6C = 0;
    do
    {
        v = gUnk_03002490;
        sub_0808eec4(gUnk_0874325A[v->unk34][v->unk6E]);
        w = gUnk_03002490;
        w->unk6E++;
        TaskYieldTrampoline(2);
        sub_080062c4();
        x = gUnk_03002490;
        x->unk6C++;
    } while ((s16)x->unk6C <= 3);
    y = gUnk_03002490;
    y->unk70 = 1;
    y->unk28 = 1;
    y->unk2C = 0;
    sub_08006138();
}

void sub_0808f844(void)
{
    if ((s16)gUnk_03002490->unk70 != 0 && sub_08069888() == 0 && gUnk_03002490->unk28 != 0)
    {
        sub_0806395c(0);
        sub_08006148(sub_0808f3b8, gCurTaskIdx);
    }
}

void sub_0808f888(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 2;
    if (gUnk_03002490->unk8C->unk1A == -1)
    {
        sub_080640c8();
        sub_080062c4();
        t = gUnk_03002490;
        t->unk60 = 168 << 5;
        t->unk68 = 192 << 10;
        sub_08006138();
    }
    else
    {
        sub_08066ba8();
        while (1)
        {
            sub_08066bdc();
            TaskYieldTrampoline(8);
        }
    }
}

void sub_0808f8dc(void)
{
    sub_080692fc();
}

void sub_0808f8e8(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808f930;
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    gUnk_03002490->unk43 = 255;
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087432E4);
}

void sub_0808f930(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087432E8);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808f954(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_0800634c(6);
    sub_08006138();
}

void sub_0808f974(void)
{
}

s32 sub_0808f978(void)
{
    sub_080224b0();
    if (gUnk_03002490->unk7B == 3)
    {
        sub_0806395c(2);
        sub_08006148(sub_0808fa98, gCurTaskIdx);
        return 1;
    }
    else
    {
        sub_0806395c(1);
        sub_08006148(sub_0808fa98, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808f9b8(void)
{
    sub_0806395c(0);
    sub_08006148(sub_0808fa98, gCurTaskIdx);
    return 1;
}

s32 sub_0808f9d8(void)
{
    sub_0806395c(2);
    sub_08006148(sub_0808fa98, gCurTaskIdx);
    return 1;
}

s32 sub_0808f9f8(void)
{
    sub_08063ff4();
    return 0;
}

s32 sub_0808fa04(void)
{
    sub_08063ff4();
    return 0;
}

void sub_0808fa10(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_08752A70;
    sub_08002e98(u->unk73, 2, gUnk_087432F4);
}

void sub_0808fa50(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808fa84;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087432FC);
}

void sub_0808fa84(void)
{
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

void sub_0808fa98(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087432FC);
}

void sub_0808fab4(void)
{
    sub_080062c4();
    while (1)
    {
        sub_080061c0(128 << 9, 0x5A5A5A5A);
        sub_08006338(5);
        TaskYieldTrampoline(gUnk_087432EC[gUnk_03002490->unk74][0]);
        sub_080061c0(128 << 8, 0x5A5A5A5A);
        sub_08006338(6);
        TaskYieldTrampoline(gUnk_087432EC[gUnk_03002490->unk74][1]);
        sub_080061c0(128 << 7, 0x5A5A5A5A);
        sub_08006338(5);
        TaskYieldTrampoline(gUnk_087432EC[gUnk_03002490->unk74][2]);
        gUnk_03002490->unk54 = 0;
        sub_08006338(4);
        TaskYieldTrampoline(gUnk_087432EC[gUnk_03002490->unk74][3]);
    }
}

void sub_0808fb50(void)
{
    sub_080224b0();
    gUnk_03002490->unk60 = 128 << 5;
    sub_080061c0(128 << 7, 0x5A5A5A5A);
    sub_08006338(6);
    sub_08006138();
}

void sub_0808fb80(void)
{
    sub_08006244();
    gUnk_03002490->unk58 = 128 << 7;
    sub_080061c0(128 << 7, 0x5A5A5A5A);
    sub_08006338(6);
    sub_08006138();
}

void sub_0808fbac(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808fbf0;
    sub_08063e14();
    sub_080639b4(gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_08743308);
}

void sub_0808fbf0(void)
{
    sub_08068e04();
    sub_08069b44();
}

void sub_0808fc00(void)
{
    sub_080062c4();
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(gUnk_087432EC[0][0]);
        sub_08006338(5);
        TaskYieldTrampoline(gUnk_087432EC[0][1]);
        sub_08006338(6);
        TaskYieldTrampoline(gUnk_087432EC[0][2]);
        sub_08006338(5);
        TaskYieldTrampoline(gUnk_087432EC[0][3]);
    }
}

void sub_0808fc40(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_0875227C;
    sub_08064a60();
    gUnk_03002490->unk7A = 0;
    u = gUnk_03002490;
    u->unk28 = 0;
    sub_08002e98(u->unk73, 1, gUnk_08743600);
}

void sub_0808fc90(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808fcd4;
    sub_080031b8(165);
    sub_080061c0(128 << 12, 0x5A5A5A5A);
    sub_0806395c(1);
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08743604);
}

void sub_0808fcd4(void)
{
    if (sub_08069604() == 0)
        sub_08002e98(gUnk_03002490->unk15, 2, gUnk_0874360C);
    else
    {
        sub_0806395c(0);
        sub_08006148(sub_0808fd1c, gCurTaskIdx);
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_0808fd1c(void)
{
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08743604);
}

void sub_0808fd38(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk7A = 0;
    gUnk_03002490->unk3C = 0;
    sub_08006138();
}

void sub_0808fd5c(void)
{
}

void sub_0808fd60(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk7A = 0;
    gUnk_03002490->unk04 = 0;
    sub_080062c4();
    t = gUnk_03002490;
    t->unk48 += t->unk43 * 16;
    t->unk4C = t->unk48 << 16;
    t->unk3C = 1;
    TaskYieldTrampoline(2);
    sub_08063fe0();
    sub_08006138();
}

void sub_0808fdb4(void)
{
}

void sub_0808fdb8(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_0806523c;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_08752520;
    sub_08002e98(u->unk73, 4, gUnk_0874362C);
}

void sub_0808fdf8(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808fe28;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_0874363C);
}

void sub_0808fe28(void)
{
    if (sub_08069660() == 0)
        sub_08002e98(gUnk_03002490->unk15, 1, gUnk_08743640);
    else
        sub_08006148(sub_0806a344, gCurTaskIdx);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808fe6c(void)
{
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_0874363C);
}

void sub_0808fe88(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk7A = 0;
    t = gUnk_03002490;
    t->unk28 = gUnk_08743614[t->unk74];
    switch (t->unk30 = (&gUnk_03002790[t->unk44])->unk34)
    {
    case 0:
        sub_0806421c(0, gUnk_0874361A[gUnk_03002490->unk74]);
        break;
    case 1:
        sub_0806421c(224 << 1, gUnk_0874361A[gUnk_03002490->unk74]);
        break;
    case 2:
        sub_0806421c(192 << 1, gUnk_0874361A[gUnk_03002490->unk74]);
        break;
    case 3:
        sub_0806421c(160 << 1, gUnk_0874361A[gUnk_03002490->unk74]);
        break;
    case 4:
        sub_0806421c(128 << 1, gUnk_0874361A[gUnk_03002490->unk74]);
        break;
    default:
        sub_0806ee2c();
        break;
    }
    u = gUnk_03002490;
    u->unk54 = gUnk_030023B4;
    u->unk58 = gUnk_030023D4;
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(1);
    }
}

void sub_0808ffe0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    if (--t->unk28 < 0)
        sub_08006148(sub_0806a344, gCurTaskIdx);
}

