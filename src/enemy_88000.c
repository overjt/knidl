/* game_code_and_rodata 0x08088000-0x0808AA68 (issue #80, module M23 batch 2).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08088000 0x0808AA68 src/enemy_88000.c --newpb
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];

/* ROM tables */
extern s16 gUnk_08742824[];
extern s16 gUnk_08742834[];
extern s16 gUnk_08742844[];
extern s32 gUnk_0874269C[];
extern s32 gUnk_087426A4[];
extern s32 gUnk_087426EC[];
extern s32 gUnk_08742734[];
extern s32 gUnk_0874276C[];
extern s32 gUnk_08742818[];
extern u16 gUnk_08742862[];
extern u32 gUnk_0873F500[];
extern u32 gUnk_0874266C[];
extern u32 gUnk_08742684[];
extern u32 gUnk_087426AC[];
extern u32 gUnk_087426B0[];
extern u32 gUnk_087426B4[];
extern u32 gUnk_087426C4[];
extern u32 gUnk_087426D8[];
extern u32 gUnk_087426F4[];
extern u32 gUnk_08742704[];
extern u32 gUnk_08742710[];
extern u32 gUnk_0874271C[];
extern u32 gUnk_08742728[];
extern u32 gUnk_0874273C[];
extern u32 gUnk_08742744[];
extern u32 gUnk_08742758[];
extern u32 gUnk_08742798[];
extern u32 gUnk_087427A0[];
extern u32 gUnk_087427A8[];
extern u32 gUnk_087427B4[];
extern u32 gUnk_087427BC[];
extern u32 gUnk_087427E8[];
extern u32 gUnk_08742C14[];
extern u32 gUnk_08742C30[];
extern u32 gUnk_08742C4C[];
extern u32 gUnk_08742C68[];
extern u32 gUnk_08742C84[];
extern u32 gUnk_08742CA0[];
extern u32 gUnk_08742CBC[];
extern u32 gUnk_08742E50[];
extern u32 gUnk_08742E5C[];
extern u32 gUnk_0875262C[];
extern u32 gUnk_087526A8[];
extern u32 gUnk_087527DC[];
extern u32 gUnk_08752808[];
extern u32 gUnk_08752828[];
extern u8 gUnk_08742778[];
extern u8 gUnk_087427B0[];
extern u8 gUnk_087427B2[];
extern u8 gUnk_08742814[];
extern u8 gUnk_08742820[];
extern u8 gUnk_08742822[];
extern u8 gUnk_08742830[];
extern u8 gUnk_08742854[];
extern u8 gUnk_08742856[];
extern u8 gUnk_0874285C[];

/* Externals */
extern s32 sub_08063b38(void);
extern s32 sub_08063cd0(void);
extern s32 sub_08063d2c(void);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern s8 sub_08064a38(void);
extern u16 sub_08021c14(s16 x, s16 y);
extern u16 sub_08064314(s32 prec);
extern u32 sub_08002ee8(u32 range);
extern u32 sub_08068e04(void);
extern u32 sub_080692fc(void);
extern u32 sub_08069888(void);
extern u32 sub_08069b44(void);
extern u8 sub_08021c4c(s16 x, s16 y);
extern void TaskDispatchTrampoline(void);
extern void TaskYieldTrampoline(u32 frames);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_080031b8(s32 id);
extern void sub_080059d8(void);
extern void sub_08005d9c(void);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, u32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_080062c4(void);
extern void sub_08006338(s32 a);
extern void sub_080224b0(void);
extern void sub_0806395c(u8 v);
extern void sub_080639b4(u32 v);
extern void sub_08063e14(void);
extern void sub_08063ff4(void);
extern void sub_0806421c(s16 t, s16 mag);
extern void sub_0806523c(void);
extern void sub_080656b4(void);
extern void sub_0806a0f0(s32 a);
void sub_0808705c(void);
void sub_08087e60(void);

/* Defined below */
void sub_08088024(void);
void sub_080883b8(void);
void sub_080884d0(void);
void sub_080886d8(void);
void sub_08088b10(void);
void sub_08088d7c(void);
void sub_08088fac(void);
void sub_0808921c(void);
void sub_080896dc(void);
void sub_08089808(u8 a);
void sub_080898dc(void);
void sub_08089bdc(void);
void sub_08089d44(void);
void sub_0808a7e0(void);
void sub_0808a7f4(u8 a);
void sub_0808a84c(u8 *p, s32 b);
void sub_0808a880(s32 a);

void sub_08088000(void)
{
    gUnk_03002490->unk04 = (u32)sub_08088024;
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_0874266C);
}

void sub_08088024(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 6, gUnk_08742684);
    sub_08068e04();
    sub_08069b44();
}

void sub_08088054(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    sub_08063e14();
    sub_08006338(6);
    TaskYieldTrampoline(15);
    sub_08063e14();
    sub_08006338(6);
    switch (sub_08002ee8(8))
    {
    case 0:
    case 1:
        gUnk_03002490->unk54 = 0;
        sub_0806395c(1);
        break;
    case 2:
    case 3:
        sub_080061c0(0x10000, 0x5A5A5A5A);
        sub_0806395c(1);
        break;
    case 4:
        sub_080061c0(0x20000, 0x5A5A5A5A);
        sub_0806395c(1);
        break;
    case 5:
        sub_0806395c(2);
        break;
    case 6:
    case 7:
        sub_0806395c(3);
        break;
    }
    sub_08006138();
}

void sub_080880fc(void)
{
    s32 v;

    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_08088000, gCurTaskIdx);
    v = gUnk_0874269C[gUnk_03002490->unk74];
    if (v > (sub_08063cd0() < 0 ? -sub_08063cd0() : sub_08063cd0()))
    {
        v = gUnk_087426A4[gUnk_03002490->unk74];
        if (v > (sub_08063d2c() < 0 ? -sub_08063d2c() : sub_08063d2c()))
        {
            sub_0806395c(4);
            sub_08006148(sub_08088000, gCurTaskIdx);
        }
    }
}

void sub_080881a0(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk7A = 0;
    if (sub_08002ee8(2) != 0)
        sub_08063ff4();
    sub_0800622c(0xFFFE8000, 0x4000, 0x30000);
    sub_08006138();
}

void sub_080881e0(void)
{
}

void sub_080881e4(void)
{
    gUnk_03002490->unk15 = 2;
    TaskYieldTrampoline(32);
    sub_0806395c(0);
    sub_08006138();
}

void sub_08088208(void)
{
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_08088000, gCurTaskIdx);
}

void sub_08088230(void)
{
    gUnk_03002490->unk15 = 3;
    TaskYieldTrampoline(64);
    sub_0806395c(0);
    sub_08006138();
}

void sub_08088254(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_08088000, gCurTaskIdx);
}

void sub_0808827c(void)
{
    gUnk_03002490->unk15 = 4;
    gUnk_03002490->unk7A = 0;
    sub_080031b8(187);
    sub_080062c4();
    sub_0800622c(0xFFFD0000, 0x1500, 0x30000);
    sub_08063e14();
    while (gUnk_03002490->unk58 < 0)
    {
        sub_08006338(4);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
    }
    gUnk_03002490->unk73 = 4;
    sub_08006138();
}

void sub_080882f0(void)
{
    if (gUnk_03002490->unk73 != 7)
    {
        gUnk_03002490->unk04 = 0;
        sub_08006148(sub_0808705c, gCurTaskIdx);
    }
}

void sub_08088320(void)
{
    gUnk_03002490->unk15 = 5;
    gUnk_03002490->unk60 = 0x1500;
    gUnk_03002490->unk68 = 0x30000;
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
    }
}

void sub_0808835c(void)
{
}

void sub_08088360(void)
{
    gUnk_03002490->unk04 = (u32)sub_080883b8;
    gUnk_03002490->unk7A = 0;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087426AC);
}

void sub_08088394(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_080883b8;
    sub_08002e98(t->unk14, 1, gUnk_087426AC);
}

void sub_080883b8(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087426B0);
    sub_08068e04();
    sub_08069b44();
}

void sub_080883dc(void)
{
    gUnk_03002490->unk15 = 0;
    sub_08063e14();
    gUnk_03002490->unk34 = 8;
    while (1)
    {
        gUnk_03002490->unk58 = 0xFFFF0000;
        sub_08006338(5);
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0xFFFF8000;
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x10000;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0x8000;
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk58 = 0xFFFF8000;
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(8);
    }
}

void sub_08088478(void)
{
    if (--gUnk_03002490->unk34 == 0)
    {
        gUnk_03002490->unk34 = 8;
        sub_08063e14();
    }
}

void sub_08088498(void)
{
    gUnk_03002490->unk04 = (u32)sub_080884d0;
    sub_080639b4((u32)gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_08063e14();
    sub_08006338(6);
    sub_08006138();
}

void sub_080884d0(void)
{
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

s32 sub_080884e4(void)
{
    if (gUnk_03002490->unk73 != 9)
    {
        switch (gUnk_03002490->unk73)
        {
        case 6:
            sub_0806395c(0);
            sub_08006148(sub_08087e60, gCurTaskIdx);
            return 1;
        case 7:
            sub_0806395c(0);
            sub_08006148(sub_08088000, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_08088540(void)
{
    u8 s;

    if (gUnk_03002490->unk73 != 9)
    {
        switch (gUnk_03002490->unk73)
        {
        case 7:
            s = gUnk_03002490->unk14;
            if (s == 0 || s == 2 || s == 3)
            {
                sub_0806395c(5);
                sub_08006148(sub_08088000, gCurTaskIdx);
                return 1;
            }
            return 0;
        }
        return 0;
    }
}

s32 sub_08088590(void)
{
    if (gUnk_03002490->unk73 != 9)
    {
        switch (gUnk_03002490->unk73)
        {
        case 7:
            if (gUnk_03002490->unk14 == 1)
                sub_08063ff4();
            return 0;
        }
        return 0;
    }
}

s32 sub_080885c0(void)
{
    if (gUnk_03002490->unk73 != 9)
    {
        switch (gUnk_03002490->unk73)
        {
        case 6:
            gUnk_03002490->unk58 = 0;
            return 0;
        case 7:
            if (gUnk_03002490->unk14 == 4)
            {
                gUnk_03002490->unk73 = 4;
                sub_08006148(sub_0808705c, gCurTaskIdx);
                return 1;
            }
            break;
        default:
            return 0;
        }
        return 0;
    }
}

void sub_08088610(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_0875262C;
    sub_08063e14();
    sub_08002e98(gUnk_03002490->unk73, 4, gUnk_087426B4);
}

void sub_08088658(void)
{
    gUnk_03002490->unk04 = (u32)sub_080886d8;
    sub_080224b0();
    if (gUnk_03002490->unk7B == 3)
    {
        sub_0806395c(3);
        gUnk_03002490->unk7A = 0;
        sub_08002e98(gUnk_03002490->unk14, 5, gUnk_087426C4);
    }
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 5, gUnk_087426C4);
}

void sub_080886b4(void)
{
    gUnk_03002490->unk04 = (u32)sub_080886d8;
    sub_08002e98(gUnk_03002490->unk14, 5, gUnk_087426C4);
}

void sub_080886d8(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 5, gUnk_087426D8);
    sub_08068e04();
    sub_08069b44();
}

void sub_08088708(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk34 = 100;
    sub_080061c0(gUnk_087426EC[gUnk_03002490->unk74], 0x5A5A5A5A);
    while (1)
    {
        sub_08006338(6);
        TaskYieldTrampoline(gUnk_087426F4[gUnk_03002490->unk74 * 2]);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(gUnk_087426F4[gUnk_03002490->unk74 * 2 + 1]);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(gUnk_087426F4[gUnk_03002490->unk74 * 2]);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(gUnk_087426F4[gUnk_03002490->unk74 * 2 + 1]);
    }
}

void sub_080887a0(void)
{
    s32 n = gUnk_03002490->unk34 - 1;

    gUnk_03002490->unk34 = n;
    switch (n)
    {
    case 20:
    case 40:
    case 60:
    case 80:
        if (sub_08002ee8(4) == 0)
        {
            sub_0806395c(1);
            sub_08006148(sub_080886b4, gCurTaskIdx);
        }
        break;
    case 0:
        sub_0806395c(1);
        sub_08006148(sub_080886b4, gCurTaskIdx);
        break;
    }
}

void sub_0808880c(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk28 = 0;
    sub_080062c4();
    sub_08006338(5);
    TaskYieldTrampoline(24);
    sub_080031b8(188);
    gUnk_03002490->unk7A = 0;
    sub_08063e14();
    sub_080061c0(0xC000, 0x5A5A5A5A);
    sub_0800622c(0xFFFD0000, 0x1500, 0x30000);
    sub_08006338(4);
    while (gUnk_03002490->unk28 == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08006338(5);
    TaskYieldTrampoline(10);
    sub_0806395c(0);
    sub_08006138();
}

void sub_080888a0(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_080886b4, gCurTaskIdx);
}

void sub_080888c8(void)
{
    gUnk_03002490->unk15 = 2;
    gUnk_03002490->unk28 = 0;
    gUnk_03002490->unk60 = 0x1500;
    gUnk_03002490->unk68 = 0x30000;
    sub_08006338(6);
    while (gUnk_03002490->unk28 == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08006338(5);
    TaskYieldTrampoline(10);
    sub_0806395c(0);
    sub_08006138();
}

void sub_08088920(void)
{
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_080886b4, gCurTaskIdx);
}

void sub_08088948(void)
{
    gUnk_03002490->unk15 = 3;
    sub_080062c4();
    gUnk_03002490->unk58 = 0x4000;
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(96);
        gUnk_03002490->unk58 = 0x8000;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk58 = 0xFFFF0000;
        gUnk_03002490->unk7A = 0;
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk58 = 0xFFFF8000;
        TaskYieldTrampoline(10);
        gUnk_03002490->unk58 = 0xFFFFC000;
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(10);
    }
}

void sub_080889c8(void)
{
}

void sub_080889cc(void)
{
    gUnk_03002490->unk15 = 4;
    gUnk_03002490->unk28 = 0;
    sub_08063e14();
    sub_080061c0(0x18000, 0x5A5A5A5A);
    sub_0800622c(0xFFFD0000, 0x1500, 0x30000);
    sub_08006338(4);
    while (gUnk_03002490->unk28 == 0)
        TaskYieldTrampoline(1);
    sub_080062c4();
    sub_08006338(5);
    TaskYieldTrampoline(10);
    sub_0806395c(0);
    sub_08006138();
}

void sub_08088a3c(void)
{
    if (gUnk_03002490->unk14 != 4)
        sub_08006148(sub_080886b4, gCurTaskIdx);
}

void sub_08088a64(void)
{
    gUnk_03002490->unk04 = (u32)sub_08088b10;
    sub_08063e14();
    if (gUnk_03002490->unk43 == 1)
        gUnk_03002490->unk4C = (gUnk_03002790[sub_08063b38()].unk48 - 80) << 16;
    else
        gUnk_03002490->unk4C = (gUnk_03002790[sub_08063b38()].unk48 + 80) << 16;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08742704);
}

void sub_08088aec(void)
{
    gUnk_03002490->unk04 = (u32)sub_08088b10;
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08742704);
}

void sub_08088b10(void)
{
    if (gUnk_03002490->unk28 != 0)
    {
        if ((u8)sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08742710);
    }
    else
    {
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08742710);
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_08088b58(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk28 = 0;
    sub_08063e14();
    sub_080061c0(0x8000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFC0000;
    sub_08006338(5);
    sub_08006138();
}

void sub_08088b98(void)
{
    if (sub_08021c14(gUnk_03002490->unk48, gUnk_03002490->unk4A) == 0)
    {
        sub_0806395c(1);
        sub_08006148(sub_08088aec, gCurTaskIdx);
    }
}

void sub_08088bd8(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk28 = 0;
    sub_08063e14();
    sub_08006338(4);
    sub_080061c0(0x8000, 0x5A5A5A5A);
    gUnk_03002490->unk58 = 0xFFFC0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFF8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x8000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(8);
    sub_0800622c(0x40000, 0x1500, 0x30000);
    gUnk_03002490->unk7A = 0;
    gUnk_03002490->unk28 = 1;
    sub_08006138();
}

void sub_08088ca0(void)
{
}

void sub_08088ca4(void)
{
    gUnk_03002490->unk15 = 2;
    gUnk_03002490->unk28 = 0;
    sub_080062c4();
    sub_08006338(5);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk58 = 0xFFFC0000;
    sub_08006338(4);
    sub_08006138();
}

void sub_08088ce4(void)
{
}

void sub_08088ce8(void)
{
    gUnk_03002490->unk04 = (u32)sub_08088d7c;
    sub_080224b0();
    if (gUnk_03002490->unk7B == 3)
    {
        gUnk_03002490->unk73 = 0;
        gUnk_03002490->unk04 = (u32)sub_080886d8;
        gUnk_03002490->unk7A = 0;
        sub_0806395c(3);
        sub_08002e98(gUnk_03002490->unk14, 5, gUnk_087426C4);
    }
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_0874271C);
}

void sub_08088d58(void)
{
    gUnk_03002490->unk04 = (u32)sub_08088d7c;
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_0874271C);
}

void sub_08088d7c(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08742728);
    sub_08068e04();
    sub_08069b44();
}

void sub_08088dac(void)
{
    struct Task *t;
    struct Task *u;
    s32 a, d;

    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk7A = 0;
    sub_080062c4();
    sub_08006338(5);
    TaskYieldTrampoline(8);
    u = &gUnk_03002790[sub_08063b38()];
    t = gUnk_03002490;
    a = t->unk50;
    d = (a >> 16) - (u->unk50 >> 16);
    if (d > 0)
        t->unk34 = 37;
    else if (d < 0)
        t->unk34 = 43;
    else if ((a & 0xFF) - (u->unk50 & 0xFF) >= 0
                 ? (a & 0xFF) - (u->unk50 & 0xFF) <= 15
                 : (u->unk50 & 0xFF) - (a & 0xFF) <= 15)
        gUnk_03002490->unk34 = 37;
    else
        gUnk_03002490->unk34 = 40;
    sub_08063e14();
    sub_080061c0(gUnk_08742734[gUnk_03002490->unk74], 0x5A5A5A5A);
    sub_0800622c(0xFFFD8000, 0x2000, 0x30000);
    sub_08006338(4);
    sub_08006138();
}

void sub_08088e78(void)
{
    if (--gUnk_03002490->unk34 == 0)
        sub_08006148(sub_08088d58, gCurTaskIdx);
}

void sub_08088ea4(void)
{
    gUnk_03002490->unk15 = 2;
    sub_080062c4();
    sub_08006338(5);
    TaskYieldTrampoline(10);
    gUnk_03002490->unk73 = 0;
    sub_08006138();
}

void sub_08088ed4(void)
{
    if (gUnk_03002490->unk73 != 2)
        sub_08006148(sub_08088610, gCurTaskIdx);
}

void sub_08088efc(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk58 = 0;
    TaskYieldTrampoline(24);
    sub_0806395c(0);
    sub_08006138();
}

void sub_08088f24(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_08088d58, gCurTaskIdx);
}

void sub_08088f4c(void)
{
    gUnk_03002490->unk04 = (u32)sub_08088fac;
    sub_080639b4((u32)gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_08063e14();
    while (1)
    {
        sub_08006338(6);
        TaskYieldTrampoline(12);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
    }
}

void sub_08088fac(void)
{
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

s32 sub_08088fc0(void)
{
    if (gUnk_03002490->unk73 != 3)
    {
        switch (gUnk_03002490->unk73)
        {
        case 0:
            gUnk_03002490->unk28 = 1;
            return 0;
        case 1:
            sub_0806395c(2);
            sub_08006148(sub_08088aec, gCurTaskIdx);
            return 1;
        case 2:
            sub_0806395c(2);
            sub_08006148(sub_08088d58, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_08089024(void)
{
    if (gUnk_03002490->unk73 != 3)
    {
        switch (gUnk_03002490->unk73)
        {
        default:
            return 0;
        case 0:
            sub_0806395c(2);
            sub_08006148(sub_080886b4, gCurTaskIdx);
            return 1;
        }
    }
}

s32 sub_08089064(void)
{
    if (gUnk_03002490->unk73 != 3)
    {
        gUnk_03002490->unk8C->unk5C = (u32)gUnk_08742E5C;
        switch (gUnk_03002490->unk73)
        {
        case 0:
            sub_0806395c(3);
            sub_08006148(sub_080886b4, gCurTaskIdx);
            return 1;
        case 2:
            gUnk_03002490->unk73 = 0;
            sub_0806395c(3);
            sub_08006148(sub_080886b4, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_080890d4(void)
{
    if (gUnk_03002490->unk73 != 3)
    {
        gUnk_03002490->unk8C->unk5C = (u32)gUnk_08742E50;
        if (gUnk_03002490->unk73 != 0)
            return 0;
        sub_0806395c(4);
        sub_08006148(sub_080886b4, gCurTaskIdx);
        return 1;
    }
}

s32 sub_08089120(void)
{
    if (gUnk_03002490->unk73 != 3)
    {
        sub_08063ff4();
        return 0;
    }
}

s32 sub_0808913c(void)
{
    if (gUnk_03002490->unk73 != 3)
    {
        gUnk_03002490->unk58 = 0;
        if (gUnk_03002490->unk73 != 2)
            return 0;
        sub_0806395c(1);
        sub_08006148(sub_08088d58, gCurTaskIdx);
        return 1;
    }
}

void sub_08089180(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_087526A8;
    sub_08002e98(gUnk_03002490->unk73, 2, gUnk_0874273C);
}

void sub_080891c0(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808921c;
    gUnk_03002490->unk28 = 12;
    sub_08063e14();
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 5, gUnk_08742744);
}

void sub_080891f8(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808921c;
    sub_08002e98(gUnk_03002490->unk14, 5, gUnk_08742744);
}

void sub_0808921c(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 5, gUnk_08742758);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808924c(void)
{
    s32 n;

    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk7A = 0;
    if (gUnk_03002490->unk74 == 0 || sub_08002ee8(2) != 0)
    {
        sub_080061c0(0x10000, 0x5A5A5A5A);
    }
    else
    {
        sub_08063e14();
        sub_080061c0(0x18000, 0x5A5A5A5A);
    }
    n = sub_08002ee8(3);
    gUnk_03002490->unk34 = n;
    sub_0800622c(gUnk_0874276C[n], 0x2000, 0x30000);
    sub_080031b8(193);
    while (1)
    {
        sub_08089808(gUnk_03002490->unk28);
        switch (gUnk_03002490->unk34)
        {
        case 0:
            TaskYieldTrampoline(4);
            break;
        case 1:
            TaskYieldTrampoline(8);
            break;
        case 2:
            sub_08006138();
            break;
        }
        if (gUnk_03002490->unk43 == 1)
        {
            gUnk_03002490->unk28++;
            if (gUnk_03002490->unk28 > 15)
                gUnk_03002490->unk28 = 0;
        }
        else
        {
            gUnk_03002490->unk28--;
            if (gUnk_03002490->unk28 < 0)
                gUnk_03002490->unk28 = 15;
        }
    }
}

void sub_08089330(void)
{
}

void sub_08089334(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk7A = 0;
    gUnk_03002490->unk43 = -gUnk_03002490->unk43;
    if (gUnk_03002490->unk43 == 1)
        gUnk_03002490->unk3E = gUnk_03002490->unk3E & 0x7FFF;
    else
        gUnk_03002490->unk3E = gUnk_03002490->unk3E | 0x8000;
    sub_080639b4((u32)gUnk_08742CBC);
    gUnk_03002490->unk3C = 21;
    TaskYieldTrampoline(12);
    sub_080639b4((u32)gUnk_08742C68);
    if (gUnk_03002490->unk28 <= 7)
    {
        gUnk_03002490->unk3C = 20;
        TaskYieldTrampoline(3);
    }
    else
    {
        gUnk_03002490->unk3C = 21;
        TaskYieldTrampoline(3);
    }
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    sub_080061c0(gUnk_03002490->unk54 < 0 ? -gUnk_03002490->unk54
                                          : gUnk_03002490->unk54,
                 0x5A5A5A5A);
    sub_080639b4((u32)gUnk_08742C14);
    while (1)
    {
        sub_08089808(gUnk_03002490->unk28);
        switch (gUnk_03002490->unk34)
        {
        case 0:
            TaskYieldTrampoline(4);
            break;
        case 1:
            TaskYieldTrampoline(8);
            break;
        case 2:
            sub_08006138();
            break;
        }
        if (gUnk_03002490->unk43 == 1)
        {
            gUnk_03002490->unk28++;
            if (gUnk_03002490->unk28 > 15)
                gUnk_03002490->unk28 = 0;
        }
        else
        {
            gUnk_03002490->unk28--;
            if (gUnk_03002490->unk28 < 0)
                gUnk_03002490->unk28 = 15;
        }
    }
}

void sub_0808945c(void)
{
}

void sub_08089460(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk00 = 0;
    sub_080639b4((u32)gUnk_08742C84);
    gUnk_03002490->unk3C = 17;
    TaskYieldTrampoline(12);
    sub_080639b4((u32)gUnk_08742C30);
    if (gUnk_03002490->unk28 <= 7)
    {
        gUnk_03002490->unk3C = 15;
        TaskYieldTrampoline(3);
    }
    else
    {
        gUnk_03002490->unk3C = 13;
        TaskYieldTrampoline(3);
    }
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    sub_080639b4((u32)gUnk_08742C14);
    while (1)
    {
        sub_08089808(gUnk_03002490->unk28);
        switch (gUnk_03002490->unk34)
        {
        case 0:
            TaskYieldTrampoline(4);
            break;
        case 1:
            TaskYieldTrampoline(8);
            break;
        case 2:
            sub_08006138();
            break;
        }
        if (gUnk_03002490->unk54 > 0)
        {
            gUnk_03002490->unk28++;
            if (gUnk_03002490->unk28 > 15)
                gUnk_03002490->unk28 = 0;
        }
        else
        {
            gUnk_03002490->unk28--;
            if (gUnk_03002490->unk28 < 0)
                gUnk_03002490->unk28 = 15;
        }
    }
}

void sub_08089530(void)
{
    gUnk_03002490->unk58 += gUnk_03002490->unk60;
}

void sub_08089544(void)
{
    gUnk_03002490->unk15 = 3;
    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk58 = 0;
    sub_080639b4((u32)gUnk_08742CA0);
    gUnk_03002490->unk3C = 18;
    TaskYieldTrampoline(13);
    sub_080639b4((u32)gUnk_08742C4C);
    if (gUnk_03002490->unk28 <= 7)
    {
        gUnk_03002490->unk3C = 16;
        TaskYieldTrampoline(3);
    }
    else
    {
        gUnk_03002490->unk3C = 14;
        TaskYieldTrampoline(3);
    }
    sub_0806395c(0);
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    sub_080639b4((u32)gUnk_08742C14);
    sub_08006138();
}

void sub_080895c4(void)
{
    if (gUnk_03002490->unk14 != 3)
        sub_08006148(sub_080891f8, gCurTaskIdx);
}

void sub_080895ec(void)
{
    gUnk_03002490->unk15 = 4;
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk60 = 0x1500;
    gUnk_03002490->unk68 = 0x30000;
    sub_080639b4((u32)gUnk_08742C14);
    while (1)
    {
        sub_08089808(gUnk_03002490->unk28);
        switch (gUnk_03002490->unk34)
        {
        case 0:
            TaskYieldTrampoline(4);
            break;
        case 1:
            TaskYieldTrampoline(8);
            break;
        case 2:
            sub_08006138();
            break;
        }
        if (gUnk_03002490->unk54 > 0)
        {
            gUnk_03002490->unk28++;
            if (gUnk_03002490->unk28 > 15)
                gUnk_03002490->unk28 = 0;
        }
        else
        {
            gUnk_03002490->unk28--;
            if (gUnk_03002490->unk28 < 0)
                gUnk_03002490->unk28 = 15;
        }
    }
}

void sub_0808967c(void)
{
}

void sub_08089680(void)
{
    gUnk_03002490->unk04 = (u32)sub_080896dc;
    sub_080639b4((u32)gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_08063e14();
    while (1)
    {
        sub_08006338(4);
        TaskYieldTrampoline(20);
        sub_08006338(14);
        TaskYieldTrampoline(2);
        sub_08006338(18);
        TaskYieldTrampoline(4);
        sub_08006338(14);
        TaskYieldTrampoline(2);
    }
}

void sub_080896dc(void)
{
    sub_08068e04();
    sub_08069b44();
}

s32 sub_080896ec(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        if (gUnk_03002490->unk14 == 3)
            return 0;
        sub_0806395c(3);
        sub_08006148(sub_080891f8, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808972c(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        if (gUnk_03002490->unk14 != 3)
            return 0;
        sub_0806395c(4);
        sub_08006148(sub_080891f8, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808976c(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        sub_0806a0f0(-2);
        return 1;
    }
}

s32 sub_0808978c(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        if (gUnk_03002490->unk54 != 0 && gUnk_03002490->unk58 != 0)
        {
            sub_0806395c(1);
            sub_08006148(sub_080891f8, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_080897d0(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        gUnk_03002490->unk58 = -gUnk_03002490->unk58;
        sub_0806395c(2);
        sub_08006148(sub_080891f8, gCurTaskIdx);
        return 1;
    }
}

void sub_08089808(u8 a)
{
    gUnk_03002490->unk3C = gUnk_08742778[a * 2];
    if (gUnk_08742778[a * 2 + 1] != 0)
        gUnk_03002490->unk3E = gUnk_03002490->unk3E | 0x8000;
    else
        gUnk_03002490->unk3E = gUnk_03002490->unk3E & 0x7FFF;
}

void sub_08089848(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_087527DC;
    sub_08002e98(gUnk_03002490->unk73, 2, gUnk_08742798);
}

void sub_08089888(void)
{
    gUnk_03002490->unk04 = (u32)sub_080898dc;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087427A0);
}

void sub_080898b8(void)
{
    gUnk_03002490->unk04 = (u32)sub_080898dc;
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_087427A0);
}

void sub_080898dc(void)
{
    if ((u8)sub_08069888() == 0)
        sub_08002e98(gUnk_03002490->unk15, 2, gUnk_087427A8);
    sub_08068e04();
    sub_08069b44();
}

void sub_0808990c(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk6C = 0;
    while ((s16)gUnk_03002490->unk6C < gUnk_087427B0[gUnk_03002490->unk74])
    {
        gUnk_03002490->unk3C = 8;
        TaskYieldTrampoline(28);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(7);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk6C++;
    }
    sub_0806395c(1);
    sub_08006138();
}

void sub_080899d4(void)
{
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_080898b8, gCurTaskIdx);
}

void sub_080899fc(void)
{
    struct ActorSpawn sp;
    u8 zero;

    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk6C = 0;
    while ((s16)gUnk_03002490->unk6C < gUnk_087427B2[gUnk_03002490->unk74])
    {
        if (gUnk_03002490->unk74 == 1)
        {
            sp.unk00 = 5;
            sp.unk04 = 107;
            sp.unk08 = zero = 0;
            sp.unk09 = gUnk_03002490->unk74;
            sp.unk0C = zero;
            sp.unk0E = -8;
            sp.unk0A = 1;
            sub_080031b8(195);
            gUnk_03002490->unk46 = sub_08064b5c(&sp, 0);
            sub_08064d34(219, 1);
        }
        gUnk_03002490->unk3C = 4;
        TaskYieldTrampoline(12);
        gUnk_03002490->unk6C++;
    }
    sub_0806395c(0);
    sub_08006138();
}

void sub_08089aac(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_080898b8, gCurTaskIdx);
}

void sub_08089ad4(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk0C = (u32)sub_08005d9c;
    gUnk_03002490->unk42 = 12;
    gUnk_03002490->unk38 = gUnk_08752808;
    gUnk_03002490->unk43 = sub_08064a38();
    gUnk_03002490->unk50 = (gUnk_03002490->unk4A - 8) << 16;
    gUnk_03002490->unk58 = 0xFFFE0000;
    sub_08006338(6);
    TaskYieldTrampoline(6);
    sub_08006338(7);
    TaskYieldTrampoline(6);
    TaskDispatchTrampoline();
}

void sub_08089b44(void)
{
    gUnk_03002490->unk04 = (u32)sub_08089bdc;
    sub_080639b4((u32)gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_08063e14();
    while (1)
    {
        gUnk_03002490->unk3C = 8;
        TaskYieldTrampoline(34);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(4);
        gUnk_03002490->unk3C--;
        TaskYieldTrampoline(18);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(8);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(3);
    }
}

void sub_08089bdc(void)
{
    sub_08069888();
    sub_08068e04();
    sub_08069b44();
}

s32 sub_08089bf0(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        sub_080062c4();
        return 0;
    }
}

s32 sub_08089c0c(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        gUnk_03002490->unk60 = 0x1500;
        gUnk_03002490->unk68 = 0x30000;
        return 0;
    }
}

s32 sub_08089c30(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        sub_080062c4();
        gUnk_03002490->unk58 = 0x4000;
        return 0;
    }
}

void sub_08089c58(void)
{
    gUnk_03002490->unk00 = (u32)sub_080656b4;
    gUnk_03002490->unk0C = (u32)sub_0806523c;
    gUnk_03002490->unk42 = 11;
    gUnk_03002490->unk38 = gUnk_08752828;
    sub_08002e98(gUnk_03002490->unk73, 2, gUnk_087427B4);
}

void sub_08089c98(void)
{
    gUnk_03002490->unk04 = (u32)sub_08089d44;
    sub_080224b0();
    if (gUnk_03002490->unk7B == 3)
    {
        gUnk_03002490->unk28 = 0;
        sub_08063e14();
        if (gUnk_03002490->unk43 == 1)
            gUnk_03002490->unk2C = 0;
        else
            gUnk_03002490->unk2C = 256;
        sub_08006338(7);
        sub_0806395c(6);
    }
    else
    {
        sub_0806395c(0);
        gUnk_03002490->unk7A = 0;
        gUnk_03002490->unk28 = 1;
    }
    sub_08002e98(gUnk_03002490->unk14, 11, gUnk_087427BC);
}

void sub_08089d20(void)
{
    gUnk_03002490->unk04 = (u32)sub_08089d44;
    sub_08002e98(gUnk_03002490->unk14, 11, gUnk_087427BC);
}

void sub_08089d44(void)
{
    if (gUnk_03002490->unk28 != 0)
    {
        if ((u8)sub_080692fc() == 0)
            sub_08002e98(gUnk_03002490->unk15, 11, gUnk_087427E8);
    }
    else
    {
        sub_08002e98(gUnk_03002490->unk15, 11, gUnk_087427E8);
    }
    switch (gUnk_03002490->unk14)
    {
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 9:
        gUnk_03002490->unk7A = 0;
        break;
    }
    sub_08068e04();
    sub_08069b44();
}

void sub_08089da8(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk28 = 1;
    sub_080062c4();
    sub_08063e14();
    sub_08006338(4);
    TaskYieldTrampoline(gUnk_08742814[gUnk_03002490->unk74]);
    sub_0808a7f4(15);
    switch (gUnk_030023D4)
    {
    case 0:
        sub_0806395c(2);
        sub_08006138();
        break;
    case 1:
        sub_0806395c(3);
        sub_08006138();
        break;
    case 2:
        sub_0806395c(2);
        sub_080061c0(gUnk_08742818[gUnk_03002490->unk74], 0x5A5A5A5A);
        sub_08006138();
        break;
    case 3:
        sub_0806395c(3);
        sub_080061c0(gUnk_08742818[gUnk_03002490->unk74], 0x5A5A5A5A);
        sub_08006138();
        break;
    case 4:
        sub_0806395c(2);
        sub_080061c0(gUnk_08742818[gUnk_03002490->unk74], 0x5A5A5A5A);
        sub_08063ff4();
        sub_08006138();
        break;
    case 5:
        sub_0806395c(1);
        break;
    }
    sub_08006138();
}

void sub_08089ea0(void)
{
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_08089d20, gCurTaskIdx);
}

void sub_08089ec8(void)
{
    gUnk_03002490->unk15 = 1;
    gUnk_03002490->unk7A = 1;
    while (1)
    {
        gUnk_030023D4 = sub_08002ee8(3);
        gUnk_03002490->unk6C = 0;
        while ((s16)gUnk_03002490->unk6C < gUnk_08742820[gUnk_03002490->unk74])
        {
            sub_08006338(4);
            TaskYieldTrampoline(4);
            if (gUnk_030023D4 != 0)
            {
                if (gUnk_03002490->unk3E & 0x8000)
                    gUnk_03002490->unk3E = gUnk_03002490->unk3E & 0x7FFF;
                else
                    gUnk_03002490->unk3E = gUnk_03002490->unk3E | 0x8000;
            }
            else
            {
                sub_08006338(5);
            }
            TaskYieldTrampoline(16);
            gUnk_03002490->unk6C++;
        }
        sub_0808a7f4(0);
        switch (gUnk_030023D4)
        {
        case 0:
            sub_0806395c(2);
            sub_08006138();
            break;
        case 1:
            sub_0806395c(3);
            sub_08006138();
            break;
        case 2:
            sub_0806395c(2);
            sub_080061c0(gUnk_08742818[gUnk_03002490->unk74], 0x5A5A5A5A);
            sub_08006138();
            break;
        case 3:
            sub_0806395c(3);
            sub_080061c0(gUnk_08742818[gUnk_03002490->unk74], 0x5A5A5A5A);
            sub_08006138();
            break;
        case 4:
            sub_0806395c(2);
            sub_080061c0(gUnk_08742818[gUnk_03002490->unk74], 0x5A5A5A5A);
            sub_08063ff4();
            sub_08006138();
            break;
        case 5:
            break;
        }
    }
}

void sub_0808a020(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_08089d20, gCurTaskIdx);
}

void sub_0808a048(void)
{
    gUnk_03002490->unk15 = 2;
    gUnk_03002490->unk28 = 1;
    gUnk_03002490->unk7A = 0;
    sub_080031b8(188);
    sub_08006338(8);
    gUnk_03002490->unk58 = 0xFFFB0000;
    gUnk_03002490->unk60 = 0x8000;
    do
        TaskYieldTrampoline(1);
    while (gUnk_03002490->unk58 < 0);
    sub_08006338(6);
    TaskYieldTrampoline(2);
    sub_08006338(5);
    sub_08006138();
}

void sub_0808a0a8(void)
{
}

void sub_0808a0ac(void)
{
    gUnk_03002490->unk15 = 3;
    gUnk_03002490->unk28 = 1;
    gUnk_03002490->unk7A = 0;
    sub_080031b8(188);
    sub_08006338(8);
    gUnk_03002490->unk58 = 0xFFFA0000;
    gUnk_03002490->unk60 = 0x4000;
    do
        TaskYieldTrampoline(1);
    while (gUnk_03002490->unk58 < 0);
    sub_08006338(6);
    TaskYieldTrampoline(2);
    sub_08006338(5);
    sub_08006138();
}

void sub_0808a10c(void)
{
}

void sub_0808a110(void)
{
    gUnk_03002490->unk15 = 4;
    gUnk_03002490->unk28 = 0;
    sub_08063e14();
    sub_080062c4();
    sub_08006338(5);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0x10000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFF0000;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(4);
    sub_08063e14();
    gUnk_03002490->unk58 = 0xFFFB0000;
    gUnk_03002490->unk60 = 0x4000;
    sub_080061c0(gUnk_08742818[gUnk_03002490->unk74], 0x5A5A5A5A);
    sub_08006338(8);
    while (gUnk_03002490->unk58 < 0)
        TaskYieldTrampoline(1);
    sub_08006338(6);
    TaskYieldTrampoline(2);
    sub_08006338(5);
    sub_08006138();
}

void sub_0808a1d0(void)
{
    u8 r;

    r = sub_08021c4c(gUnk_03002490->unk48, gUnk_03002490->unk4A);
    if (r == 0)
    {
        gUnk_03002490->unk28 = 1;
        gUnk_03002490->unk7A = r;
    }
}

void sub_0808a204(void)
{
    gUnk_03002490->unk15 = 5;
    gUnk_03002490->unk28 = 1;
    gUnk_03002490->unk7A = 0;
    sub_080062c4();
    gUnk_03002490->unk58 = 0x8000;
    sub_080061c0(0x8000, 0x5A5A5A5A);
    if (gUnk_03002490->unk43 > 0)
        gUnk_03002490->unk2C = 64;
    else
        gUnk_03002490->unk2C = 192;
    sub_08006338(5);
    TaskYieldTrampoline(40);
    sub_08006338(7);
    sub_0806395c(6);
    sub_08006138();
}

void sub_0808a270(void)
{
    if (gUnk_03002490->unk14 != 5)
        sub_08006148(sub_08089d20, gCurTaskIdx);
}

void sub_0808a298(void)
{
    gUnk_03002490->unk15 = 6;
    gUnk_03002490->unk28 = 1;
    gUnk_03002490->unk7A = 0;
    sub_0806421c((s16)gUnk_03002490->unk2C, 128);
    gUnk_03002490->unk54 = gUnk_030023B4;
    gUnk_03002490->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(gUnk_08742822[gUnk_03002490->unk74]);
    if (sub_08002ee8(4) != 0)
        TaskYieldTrampoline(30);
    sub_08006338(4);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(4);
    gUnk_03002490->unk3C--;
    TaskYieldTrampoline(2);
    sub_08006338(9);
    TaskYieldTrampoline(3);
    switch (sub_08002ee8(3))
    {
    case 0:
        sub_0806395c(7);
        break;
    case 1:
        sub_0806395c(8);
        break;
    case 2:
        sub_0806395c(9);
        break;
    }
    sub_08006138();
}

void sub_0808a36c(void)
{
    if (gUnk_03002490->unk14 != 6)
        sub_08006148(sub_08089d20, gCurTaskIdx);
    if (sub_08021c14(gUnk_03002490->unk48,
                     (u16)gUnk_03002490->unk4A - 8) == 0)
    {
        sub_0806395c(4);
        sub_08006148(sub_08089d20, gCurTaskIdx);
    }
}

void sub_0808a3c4(void)
{
    gUnk_03002490->unk15 = 7;
    gUnk_03002490->unk28 = 1;
    gUnk_03002490->unk7A = 0;
    sub_080062c4();
    sub_0808a880(0);
    sub_08006338(7);
    sub_0806421c((s16)gUnk_03002490->unk2C, gUnk_08742824[0]);
    gUnk_03002490->unk54 = gUnk_030023B4;
    gUnk_03002490->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(8);
    sub_0806421c((s16)gUnk_03002490->unk2C, gUnk_08742824[2]);
    gUnk_03002490->unk54 = gUnk_030023B4;
    gUnk_03002490->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(8);
    sub_0806421c((s16)gUnk_03002490->unk2C, gUnk_08742824[4]);
    gUnk_03002490->unk54 = gUnk_030023B4;
    gUnk_03002490->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(8);
    sub_0806395c(6);
    sub_08006138();
}

void sub_0808a478(void)
{
    if (gUnk_03002490->unk14 != 7)
        sub_08006148(sub_08089d20, gCurTaskIdx);
    if (sub_08021c14(gUnk_03002490->unk48,
                     (u16)gUnk_03002490->unk4A - 8) == 0)
    {
        sub_0806395c(4);
        sub_08006148(sub_08089d20, gCurTaskIdx);
    }
}

void sub_0808a4d0(void)
{
    gUnk_03002490->unk15 = 8;
    gUnk_03002490->unk28 = 1;
    gUnk_03002490->unk7A = 0;
    while (1)
    {
        sub_080062c4();
        sub_08006338(9);
        TaskYieldTrampoline(gUnk_08742830[gUnk_03002490->unk74]);
        sub_0808a880(8);
        sub_08006338(7);
        sub_0806421c((s16)gUnk_03002490->unk2C, gUnk_08742834[0]);
        gUnk_03002490->unk54 = gUnk_030023B4;
        gUnk_03002490->unk58 = gUnk_030023D4;
        TaskYieldTrampoline(8);
        sub_0806421c((s16)gUnk_03002490->unk2C, gUnk_08742834[2]);
        gUnk_03002490->unk54 = gUnk_030023B4;
        gUnk_03002490->unk58 = gUnk_030023D4;
        TaskYieldTrampoline(8);
        sub_0806421c((s16)gUnk_03002490->unk2C, gUnk_08742834[4]);
        gUnk_03002490->unk54 = gUnk_030023B4;
        gUnk_03002490->unk58 = gUnk_030023D4;
        TaskYieldTrampoline(8);
        sub_0806421c((s16)gUnk_03002490->unk2C, gUnk_08742834[6]);
        gUnk_03002490->unk54 = gUnk_030023B4;
        gUnk_03002490->unk58 = gUnk_030023D4;
        TaskYieldTrampoline(8);
        if (sub_08002ee8(3) != 0)
        {
            sub_0806395c(6);
            sub_08006138();
        }
    }
}

void sub_0808a5b8(void)
{
    if (gUnk_03002490->unk14 != 8)
        sub_08006148(sub_08089d20, gCurTaskIdx);
    if (sub_08021c14(gUnk_03002490->unk48,
                     (u16)gUnk_03002490->unk4A - 8) == 0)
    {
        sub_0806395c(4);
        sub_08006148(sub_08089d20, gCurTaskIdx);
    }
}

void sub_0808a610(void)
{
    gUnk_03002490->unk15 = 9;
    gUnk_03002490->unk28 = 1;
    gUnk_03002490->unk7A = 0;
    sub_080062c4();
    sub_08006338(9);
    TaskYieldTrampoline(gUnk_08742830[gUnk_03002490->unk74]);
    sub_0808a880(16);
    sub_08006338(7);
    sub_0806421c((s16)gUnk_03002490->unk2C, gUnk_08742844[0]);
    gUnk_03002490->unk54 = gUnk_030023B4;
    gUnk_03002490->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(8);
    sub_0806421c((s16)gUnk_03002490->unk2C, gUnk_08742844[2]);
    gUnk_03002490->unk54 = gUnk_030023B4;
    gUnk_03002490->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(8);
    sub_0806421c((s16)gUnk_03002490->unk2C, gUnk_08742844[4]);
    gUnk_03002490->unk54 = gUnk_030023B4;
    gUnk_03002490->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(8);
    sub_0806421c((s16)gUnk_03002490->unk2C, gUnk_08742844[6]);
    gUnk_03002490->unk54 = gUnk_030023B4;
    gUnk_03002490->unk58 = gUnk_030023D4;
    TaskYieldTrampoline(8);
    if (sub_08002ee8(3) != 0)
        sub_0806395c(6);
    else
        sub_0806395c(8);
    sub_08006138();
}

void sub_0808a710(void)
{
    if (gUnk_03002490->unk14 != 9)
        sub_08006148(sub_08089d20, gCurTaskIdx);
    if (sub_08021c14(gUnk_03002490->unk48,
                     (u16)gUnk_03002490->unk4A - 8) == 0)
    {
        sub_0806395c(4);
        sub_08006148(sub_08089d20, gCurTaskIdx);
    }
}

void sub_0808a768(void)
{
    gUnk_03002490->unk15 = 10;
    gUnk_03002490->unk28 = 1;
    gUnk_03002490->unk7A = 0;
    sub_080062c4();
    sub_08006338(4);
    gUnk_03002490->unk60 = 0x1500;
    gUnk_03002490->unk68 = 0x30000;
    sub_08006138();
}

void sub_0808a7a4(void)
{
}

void sub_0808a7a8(void)
{
    gUnk_03002490->unk04 = (u32)sub_0808a7e0;
    sub_080639b4((u32)gUnk_0873F500);
    gUnk_03002490->unk78 = 2;
    sub_08063e14();
    sub_08006338(4);
    sub_08006138();
}

void sub_0808a7e0(void)
{
    sub_080692fc();
    sub_08068e04();
    sub_08069b44();
}

void sub_0808a7f4(u8 a)
{
    s32 n;
    u8 v;

    n = gUnk_03002490->unk34 + 1;
    gUnk_03002490->unk34 = n;
    if (a == 0)
        v = gUnk_08742854[n & 1];
    else
        v = a;
    if ((gUnk_03002490->unk34 & 1) == 0)
        sub_0808a84c(gUnk_08742856, v);
    else
        sub_0808a84c(gUnk_0874285C, v);
}

void sub_0808a84c(u8 *p, s32 b)
{
    s32 r;
    s32 i;

    r = sub_08002ee8(b + 1);
    i = 0;
    while (p[i] < r)
        i++;
    gUnk_030023B4 = r;
    gUnk_030023D4 = i;
}

void sub_0808a880(s32 a)
{
    u32 v;

    gUnk_03002490->unk2C = v = gUnk_08742862[(u16)sub_08064314(0) + a];
    if (v < 128 || v > 384)
        gUnk_03002490->unk43 = 1;
    else if (v > 128 && v < 384)
        gUnk_03002490->unk43 = -1;
}

s32 sub_0808a8d4(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        switch (gUnk_03002490->unk14)
        {
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            gUnk_03002490->unk2C = 512 - gUnk_03002490->unk2C;
            gUnk_03002490->unk58 = -gUnk_03002490->unk58;
            return 0;
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 10:
            sub_0806395c(0);
            sub_08006148(sub_08089d20, gCurTaskIdx);
            return 1;
        default:
            return 0;
        }
    }
}

s32 sub_0808a964(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        switch (gUnk_03002490->unk14)
        {
        case 0:
        case 1:
            sub_0806395c(10);
            sub_08006148(sub_08089d20, gCurTaskIdx);
            return 1;
        }
        return 0;
    }
}

s32 sub_0808a9a8(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        sub_0806395c(5);
        sub_08006148(sub_08089d20, gCurTaskIdx);
        return 1;
    }
}

s32 sub_0808a9d8(void)
{
    s32 n;

    if (gUnk_03002490->unk73 != 1)
    {
        switch (gUnk_03002490->unk14)
        {
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            n = 768 - gUnk_03002490->unk2C;
            gUnk_03002490->unk2C = n;
            if (n > 0x1FF)
                gUnk_03002490->unk2C = n - 0x200;
            /* fallthrough */
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            sub_08063ff4();
            return 0;
        default:
            return 0;
        }
    }
}

s32 sub_0808aa28(void)
{
    if (gUnk_03002490->unk73 != 1)
    {
        switch (gUnk_03002490->unk14)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
            gUnk_03002490->unk58 = 0;
            return 0;
        case 5:
        case 6:
        case 7:
        case 8:
        case 9:
            gUnk_03002490->unk2C = 512 - gUnk_03002490->unk2C;
            gUnk_03002490->unk58 = -gUnk_03002490->unk58;
            return 0;
        default:
            return 0;
        }
    }
}
