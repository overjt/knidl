/* game_code_and_rodata 0x080763E8-0x08077AE0 (issue #79, module M19 batch 4).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080763E8 0x08077AE0 src/actor_763e8.c --newpb
 *
 * M19 batch 4: task type #99's cutscene director tail plus type #75
 * (sub_0807705c) and type #76 (sub_0807771c) - the ending-pose and
 * script-walker families over struct M19Script.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"


/* The 0x087401E4 script records M19's ending-sequence tasks walk: a pointer
   table indexed by Task.unk20, each entry a header plus two `s16` step lists
   (the "forward" list at +6 and the "reverse" one at +18) that
   sub_0807777c picks between on Task.unk18/unk24. */
struct M19Script
{
    /*0x00*/ u8 unk00[4];
    /*0x04*/ u16 unk04;
    /*0x06*/ u16 unk06[6];
    /*0x12*/ u16 unk12[1];
};


/* RAM cells and ROM tables */
extern s16 gUnk_02005588[];
extern s16 gUnk_0300244C;
extern s16 gUnk_0873D384[];
extern s32 gUnk_030023B4;
extern s32 gUnk_030023D4;
extern s8 gUnk_02006094;
extern s8 gUnk_0300238C;
extern s8 gUnk_087401CC[];
extern struct M19Script *gUnk_087401E4[];
extern struct PlayerState gUnk_03002170[];
extern struct Task * gUnk_03002490;
extern struct Task gUnk_03002790[];
extern u16 gUnk_03002360;
extern u16 gUnk_030023AC;
extern u16 gUnk_0874009C[];
extern u16 gUnk_087400A6[];
extern u16 gUnk_087400E4[];
extern u16 gUnk_08740124[];
extern u32 gUnk_0873F5E4[];
extern u32 gUnk_087400E0[];
extern u32 gUnk_08740100[];
extern u32 gUnk_08740110[];
extern u32 gUnk_08740120[];
extern u32 gUnk_087402BC[];
extern u32 gUnk_087402C8[];
extern u32 gUnk_08752C38[];
extern u32 gUnk_08752C74[];
extern u8 gUnk_03001F30;
extern u8 gUnk_03002340;
extern u8 gUnk_03002350;
extern vs32 gCurTaskIdx;
extern vu16 gUnk_03000F98[4];
extern vu16 gUnk_03001EB8[4];

/* callees */
extern s32 sub_080031b8(u32 a);
extern s32 sub_0800a130();
extern s32 sub_08022540();
extern s32 sub_0802610c();
extern s32 sub_080269d8();
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern u32 sub_08068e04(void);
extern u32 sub_080692fc(void);
extern void TaskYieldTrampoline(u32 a);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_08005654(s32 id);
extern void sub_080059fc(void);
extern void sub_08006138(void);
extern void sub_08006148(void *a, u32 i);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_080062e0(u32 i);
extern void sub_08006338(s32 a);
extern void sub_0801bcac(u32 *p);
extern void sub_080261d4(u32 a);
extern void sub_08026264(s32 a, s32 b);
extern void sub_0806395c(u8 v);
extern void sub_0806421c(s16 t, s16 mag);
extern void sub_08064970(void);
extern void sub_080651b4(void);
extern void sub_080656b4(void);
extern void sub_08067108(void);
extern void sub_08068a8c(s32 i, u8 flag);
extern void sub_08068b88(s32 i, u16 b, u8 c, u8 d);
extern void sub_0806d4e4(u32 a, s32 b);
extern void sub_0806da3c(u32 a, u32 b);
extern void sub_0807022c(void);
extern void sub_08070264(void);
extern void sub_0807029c(void);
extern void sub_080702d8(void);
extern void sub_08070334(void);
extern void sub_080703a8(void);
extern void sub_08070614(u32 a);
extern void sub_08076318(void);
extern void sub_080763c4(void);

/* defined below */
void sub_08076f50(s32 a);
void sub_08076f04(s32 a);
void sub_08077270(void);
void sub_08077254(void);
void sub_08077898(struct M19Script *p);
void sub_080779dc(void);
void sub_08077aa8(void);

void sub_080763e8(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk28 <= 0)
    {
        if (t->unk24 != 0)
        {
            if (t->unk2C > 0)
                t->unk2C--;
        }
        else
        {
            if (t->unk2C <= 3)
                t->unk2C++;
        }
        if (gUnk_03001F30 == 0)
            gUnk_03002490->unk3C = gUnk_0874009C[gUnk_03002490->unk2C];
        else
            gUnk_03002490->unk3C = gUnk_087400A6[gUnk_03002490->unk2C];
        gUnk_03002490->unk28 = 1;
    }
    gUnk_03002490->unk28--;
}

void sub_08076454(void)
{
    struct Task *t = gUnk_03002490;

    t->unk4C = 0;
    t->unk50 = -0x180000;
    t->unk18 = 0;
    t->unk28 = 1;
    t->unk24 = 1;
    t->unk2C = 2;
    if (gUnk_0300244C != 0 && gUnk_03001F30 == 1)
        t->unk3C = 0x1265;
    else
        t->unk3C = 0x11C7;
    {
        u16 id = gUnk_03002360;
        struct Task *u = gUnk_03002490;

        if (id == u->unk88->unk00)
        {
            s32 x = u->unk4C + gUnk_03002790[u->unk44].unk4C;
            s32 y = u->unk50 + gUnk_03002790[u->unk44].unk50;

            sub_08026264(x >> 16, y >> 16);
        }
    }
}

void sub_080764f8(void)
{
    vu16 *p = gUnk_03001EB8;
    struct Task *t = gUnk_03002490;

    if (p[t->unk88->unk00] & 1)
    {
        sub_08076f50(gCurTaskIdx);
    }
    else
    {
        if (gUnk_03000F98[t->unk88->unk00] & 0x80)
            t->unk24 = 0;
        else
            t->unk24 = 1;
        sub_080763e8();
    }
}

void sub_08076558(u16 a)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk30)
    {
    case 0:
        if (t->unk34 == 2 || t->unk34 == 4)
            a = a + 8;
        break;
    case 1:
        a = a + 8;
        break;
    case 2:
    case 3:
        a = a + 8;
        if (t->unk34 == 4)
            a = a + 16;
        break;
    }
    if (gUnk_03002490->unk30 & 1)
        gUnk_03002490->unk43 = 255;
    else
        gUnk_03002490->unk43 = 1;
    sub_0806421c((s16)a, 1024);
    sub_080061c0(gUnk_030023B4, 0x5A5A5A5A);
    gUnk_03002490->unk58 = gUnk_030023D4;
}

void sub_080765f4(u16 a)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk30)
    {
    case 0:
        if (t->unk34 == 2 || t->unk34 == 4)
            a = a + 8;
        break;
    case 1:
        a = a - 8;
        break;
    case 2:
    case 3:
        if (t->unk30 & 1)
            a = a - 8;
        else
            a = a + 8;
        if (gUnk_03002490->unk34 == 4)
        {
            if (gUnk_03002490->unk30 & 1)
                a = a - 16;
            else
                a = a + 16;
        }
        break;
    }
    if (gUnk_03002490->unk30 & 1)
        gUnk_03002490->unk43 = 255;
    else
        gUnk_03002490->unk43 = 1;
    sub_0806421c((s16)a, 1024);
    {
        struct Task *u = gUnk_03002490;

        u->unk54 = gUnk_030023B4;
        u->unk58 = gUnk_030023D4;
    }
}

void sub_080766ac(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk4C = t->unk48 << 16;
        t->unk50 = t->unk4A << 16;
        t->unk3C = 0xFFFF;
    }
    TaskYieldTrampoline(25);
    sub_08076558(384);
    sub_0807029c();
    sub_08064970();
    if (gUnk_030023D4 > 63)
        gUnk_03002490->unk18 = 1;
    {
        struct Task *t = gUnk_03002490;

        t->unk24 = 0;
        t->unk70 = 8;
    }
}

void sub_08076710(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk48 += 13;
        t->unk4A += 8;
        t->unk4C = t->unk48 << 16;
        t->unk50 = t->unk4A << 16;
        t->unk43 = 1;
    }
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(25);
    sub_080765f4(448);
    sub_0807029c();
    sub_08064970();
    if (gUnk_030023B4 > 95 || gUnk_030023D4 > 63)
        gUnk_03002490->unk18 = 1;
    {
        struct Task *t = gUnk_03002490;

        t->unk24 = 0;
        t->unk70 = 8;
    }
}

void sub_08076798(void)
{
    sub_08076558(384);
    {
        struct Task *t = gUnk_03002490;

        t->unk4C = t->unk48 << 16;
        t->unk50 = t->unk4A << 16;
        if (gUnk_03001F30 == 0)
            sub_08006338(gUnk_0873D384[t->unk88->unk0D]);
        else
            sub_08006338(0x11E4);
    }
    sub_08064970();
    if (gUnk_030023D4 > 63)
        gUnk_03002490->unk18 = 1;
    {
        struct Task *t = gUnk_03002490;

        t->unk24 = 0;
        t->unk2C = 10;
    }
}

void sub_08076828(void)
{
    sub_08064970();
    if (gUnk_03002490->unk24 == 0 && gUnk_030023D4 <= 0)
    {
        sub_0802610c();
        {
            struct Task *t = gUnk_03002490;

            t->unk18 = 0;
            t->unk24 = 1;
        }
    }
}

void sub_0807685c(s32 a)
{
    struct Task *t = gUnk_03002490;

    t->unk1C = t->unk48;
    t->unk20 = t->unk4A;
    if (t->unk3C != -1)
    {
        if (gUnk_03001F30 == 0)
            sub_08070334();
        {
            struct Task *u = gUnk_03002490;

            if ((s16)u->unk70 <= 0)
            {
                u->unk70 = 8;
                sub_0806da3c(0, a);
            }
        }
        gUnk_03002490->unk70--;
        sub_08076828();
    }
}

void sub_080768c8(void)
{
    gUnk_03002490->unk43 = 1;
    gUnk_03002490->unk18 = 1;
    sub_0800622c(0x18000, 0x8000, 0x40000);
    {
        struct Task *t = gUnk_03002490;
        u16 x;

        t->unk3E |= 0x8000;
        t->unk24 = 0;
        t->unk34 = 1;
        x = t->unk4C >> 16;
        switch (t->unk88->unk00)
        {
        case 0:
            break;
        case 3:
            x = ((x << 16) + 0x80000) >> 16;
        case 2:
            x = ((x << 16) + 0x80000) >> 16;
        case 1:
            x = ((x << 16) + 0x80000) >> 16;
            break;
        }
        gUnk_03002490->unk4C = x << 16;
    }
    sub_08067108();
}

void sub_08076958(void)
{
    u8 k = gUnk_03002490->unk88->unk0D;

    if (k == 1 || k == 2 || (s8)k == 4 || (s8)k == 5 || (s8)k == 9
     || (s8)k == 10 || (s8)k == 15 || (s8)k == 16 || (s8)k == 17
     || (s8)k == 19 || (s8)k == 22 || (s8)k == 23)
    {
        s32 v = (u16)gUnk_03002490->unk3C << 16;

    loop:
        sub_08006338(v >> 16);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        goto loop;
    }
    sub_08006138();
}

void sub_080769d0(void)
{
    while (1)
    {
        sub_08006338(0x124D);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 6);
    }
}

void sub_08076a14(void)
{
    while (1)
    {
        sub_08006338(0x124D);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C = 0;
        do
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk6C++;
        } while ((s16)gUnk_03002490->unk6C <= 6);
    }
}

void sub_08076a58(void)
{
    sub_080061c0(0x40000, -0x10000);
    sub_08006338(0x1255);
    TaskYieldTrampoline(4);
    gUnk_03002490->unk7A = 0;
    sub_080061c0(0x10000, 0);
    {
        struct Task *t = gUnk_03002490;

        t->unk58 = -0x49800;
        t->unk60 = 0x3800;
        t->unk6C = 0;
    }
    do
    {
        sub_08006338(0x1256);
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    sub_080061c0(0x50000, -0x20000);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    sub_080061c0(-0x30000, 0x20000);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    sub_080061c0(0x50000, -0x20000);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    sub_080061c0(-0x30000, 0x20000);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(3);
    sub_080061c0(0x30000, 0);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_080061c0(0x8000, 0x5A5A5A5A);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    if (gUnk_03002490->unk24 == 1)
    {
        do
        {
            TaskYieldTrampoline(1);
        } while (gUnk_03002490->unk24 == 1);
    }
    TaskYieldTrampoline(3);
    sub_08070614(gCurTaskIdx);
    sub_08006138();
}

void sub_08076c00(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk58 != 0 && (t->unk7A & 1))
    {
        t->unk24++;
        sub_080062c4();
        gUnk_03002490->unk7A = 0;
        {
            struct Task *u = gUnk_03002490;

            switch (u->unk24)
            {
            case 1:
                sub_080031b8(153);
                sub_080261d4(4);
                sub_0806d4e4(0, 0);
                {
                    struct Task *v = gUnk_03002490;

                    v->unk3C = 0x1255;
                    v->unk14 = 5;
                }
                sub_08006148(sub_080763c4, gCurTaskIdx);
                break;
            case 2:
                u->unk3C = 0x1264;
                break;
            }
        }
    }
}

void sub_08076c88(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk00 = (u32)sub_080059fc;
    sub_08076454();
    sub_08006138();
}

void sub_08076cac(void)
{
    struct Task *t = gUnk_03002490;

    t->unk1C = t->unk48;
    t->unk20 = t->unk4A;
    sub_080764f8();
}

void sub_08076cd4(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080766ac();
    if (gUnk_03001F30 == 1)
        sub_080769d0();
    sub_08006138();
}

void sub_08076d00(void)
{
    sub_0807685c(0);
}

void sub_08076d0c(void)
{
    gUnk_03002490->unk15 = 2;
    sub_08076710();
    if (gUnk_03001F30 == 1)
        sub_080769d0();
    sub_08006138();
}

void sub_08076d38(void)
{
    sub_0807685c(1);
}

void sub_08076d44(void)
{
    gUnk_03002490->unk15 = 3;
    sub_08076798();
    if (gUnk_03001F30 == 0)
        sub_08076958();
    else
        sub_08006138();
}

void sub_08076d70(void)
{
    struct Task *t = gUnk_03002490;

    t->unk1C = t->unk48;
    t->unk20 = t->unk4A;
    if (t->unk2C <= 0)
        sub_08076f04(gCurTaskIdx);
    else
        t->unk2C--;
}

void sub_08076dac(void)
{
    gUnk_03002490->unk15 = 4;
    sub_080768c8();
    if (gUnk_03001F30 == 0)
    {
        sub_08070264();
        sub_0807022c();
    }
    else
    {
        sub_08076a14();
    }
}

void sub_08076ddc(void)
{
    sub_0801bcac(gUnk_0873F5E4);
    if (gUnk_03001F30 == 0)
    {
        sub_080703a8();
        if (gUnk_03002490->unk24 == 0)
            sub_080702d8();
    }
    else
    {
        sub_08076c00();
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk1C = t->unk48;
        t->unk20 = t->unk4A;
    }
}

void sub_08076e30(void)
{
    gUnk_03002490->unk15 = 5;
    sub_08076a58();
}

void sub_08076e48(void)
{
    if ((gUnk_03002490->unk7A & 1) == 0)
        sub_0801bcac(gUnk_0873F5E4);
    sub_08076c00();
    {
        struct Task *t = gUnk_03002490;

        t->unk1C = t->unk48;
        t->unk20 = t->unk4A;
    }
}

void sub_08076e88(s32 id, s32 v)
{
    struct Task *t = &gUnk_03002790[id];

    sub_08068a8c(id, 1);
    t->unk14 = 0;
    t->unk44 = v;
    t->unk7B = 0;
    sub_08006148(sub_08076318, id);
}

void sub_08076ec8(s32 id)
{
    struct Task *u = &gUnk_03002790[id];
    struct Task *t = &gUnk_03002790[u->unk44];
    t->unk18 = 60;
    t->unk1C--;
    t->unk20 &= ~(1 << gCurTaskIdx);
}

void sub_08076f04(s32 id)
{
    struct Task *t = &gUnk_03002790[id];
    struct PlayerState *p = &gUnk_03002170[id];

    sub_08076ec8(id);
    t->unk7A = 0;
    sub_08006244();
    sub_08068b88(id, 0, 1, 0);
    sub_0800a130(p->unk0D, id);
}

void sub_08076f50(s32 id)
{
    struct Task *t = &gUnk_03002790[id];
    struct PlayerState *p = &gUnk_03002170[id];
    struct Task *u = &gUnk_03002790[t->unk44];

    sub_08076ec8(id);
    t->unk43 = 1;
    t->unk7A = 0;
    {
        struct Task *v = gUnk_03002490;

        t->unk4C = v->unk48 << 16;
        t->unk50 = v->unk4A << 16;
    }
    sub_080062e0(id);
    sub_08068b88(id, 6, 1, 0);
    p->unk14 = 8;
    sub_0800a130(p->unk0D, id);
    if (u->unk1C > 0)
        sub_08067108();
}

void sub_08076fe4(int a)
{
    u16 v = a;
    s32 i = 0;
    s32 n = 0;

    for (; i < gUnk_030023AC; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
        {
            struct Task *t = &gUnk_03002790[i];

            if ((gUnk_03002490->unk20 >> i) & 1)
            {
                t->unk30 = n;
                n++;
                t->unk34 = gUnk_03002490->unk1C;
                t->unk14 = v;
                sub_08006148(sub_080763c4, i);
            }
        }
    }
}

void sub_0807705c(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk00 = (u32)sub_080656b4;
        t->unk0C = (u32)sub_080651b4;
        t->unk42 = 13;
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk38 = gUnk_08752C38;
        t->unk18 = 0;
        sub_08002e98(t->unk73, 1, gUnk_087400E0);
    }
}

u16 sub_080770a0(void)
{
    s32 i = gUnk_0300238C * 2 + gUnk_03002490->unk74;
    u16 v;

    if (i > 13)
        i = 0;
    v = gUnk_087400E4[i];
    switch (v)
    {
    case 1:
        sub_0806395c(2);
        break;
    case 2:
        sub_0806395c(3);
        break;
    }
    return v;
}

void sub_080770f0(s32 id)
{
    if (gUnk_0300244C == 0 || gUnk_02005588[id] > 0)
    {
        struct Task *t = &gUnk_03002790[id];
        struct PlayerState *p = &gUnk_03002170[id];

        if (p->unk06 != 1 && t->unk58 > 0)
        {
            sub_08076e88(id, gCurTaskIdx);
            {
                struct Task *u = gUnk_03002490;

                u->unk1C++;
                u->unk18 = 0;
                u->unk20 |= 1 << id;
            }
        }
        else
        {
            gUnk_03002490->unk75 = 0;
        }
    }
}

void sub_0807717c(void)
{
    if (gUnk_03002490->unk1C == 0)
        sub_0806da3c(1, 0);
    sub_0806da3c(2, 2);
    sub_080261d4(4);
    sub_080031b8(296);
}

void sub_080771b0(void)
{
    sub_0806da3c(2, 2);
    sub_080261d4(4);
}

void sub_080771c4(void)
{
    sub_0806da3c(3, 3);
    sub_080261d4(4);
}

void sub_080771d8(void)
{
    if (gUnk_03002490->unk1C == gUnk_03002350)
        sub_08076fe4((s16)sub_080770a0());
    else
    {
        sub_08076fe4(3);
        sub_0806395c(1);
    }
    sub_08006148(sub_08077270, gCurTaskIdx);
}

void sub_08077224(void)
{
    gUnk_03002490->unk04 = (u32)sub_08077254;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_08740100);
}

void sub_08077254(void)
{
    sub_08002e98(gUnk_03002490->unk15, 4, gUnk_08740110);
}

void sub_08077270(void)
{
    sub_08002e98(gUnk_03002490->unk14, 4, gUnk_08740100);
}

void sub_0807728c(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080062c4();
    {
        struct Task *t = gUnk_03002490;

        t->unk1C = 0;
        t->unk20 = 0;
        t->unk3C = 0;
    }
    sub_08006138();
}

void sub_080772b0(void)
{
    if ((u8)sub_080692fc() == 0)
    {
        if (gUnk_02006094 == 1)
        {
            sub_080771d8();
        }
        else
        {
            struct Task *t = gUnk_03002490;

            if (t->unk18 <= 0)
            {
                if (sub_08068e04())
                    sub_080770f0(gUnk_03002490->unk7E);
            }
            else
            {
                t->unk18--;
            }
        }
    }
}

void sub_08077308(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080062c4();
    sub_0807717c();
    gUnk_03002490->unk3C = 12;
    TaskYieldTrampoline(8);
    {
        struct Task *t = gUnk_03002490;

        t->unk3C = 0;
        t->unk58 = -0x60000;
    }
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = -0x30000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x30000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x60000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = -0x20000;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk58 = 0x20000;
    TaskYieldTrampoline(2);
    sub_080062c4();
    gUnk_02006094 = 0;
    sub_0806395c(0);
    sub_08006138();
}

void sub_080773a8(void)
{
    if (gUnk_03002490->unk14 != 1)
        sub_08006148(sub_08077270, gCurTaskIdx);
}

void sub_080773d0(void)
{
    gUnk_03002490->unk15 = 2;
    sub_080062c4();
    {
        struct Task *t = gUnk_03002490;

        t->unk58 = -0x24000;
        t->unk60 = 0x4000;
        t->unk3C = 1;
    }
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
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_080062c4();
    gUnk_03002490->unk3C = 12;
    TaskYieldTrampoline(8);
    sub_080031b8(225);
    sub_080771b0();
    {
        struct Task *t = gUnk_03002490;

        t->unk3C = 0;
        t->unk6C = 0;
    }
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk58 = 0x20000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = -0x20000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk58 = 0x10000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    sub_080062c4();
    sub_08006138();
}

void sub_08077564(void)
{
}

void sub_08077568(void)
{
    gUnk_03002490->unk15 = 3;
    sub_080062c4();
    {
        struct Task *t = gUnk_03002490;

        t->unk58 = -0x24000;
        t->unk60 = 0x4000;
        t->unk3C = 1;
    }
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
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_080062c4();
    gUnk_03002490->unk3C = 14;
    TaskYieldTrampoline(8);
    sub_080031b8(225);
    sub_080771c4();
    {
        struct Task *t = gUnk_03002490;

        t->unk3C--;
        t->unk6C = 0;
    }
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk58 = 0x20000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = -0x20000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk58 = 0x10000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = -0x10000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    gUnk_03002490->unk6C = 0;
    do
    {
        gUnk_03002490->unk58 = 0x8000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk58 = -0x8000;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    sub_080062c4();
    sub_08006138();
}

void sub_08077718(void)
{
}

void sub_0807771c(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk00 = (u32)sub_080656b4;
        t->unk0C = (u32)sub_080651b4;
        t->unk42 = 13;
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk38 = gUnk_08752C74;
        sub_08002e98(t->unk73, 1, gUnk_08740120);
    }
}

void sub_0807775c(void)
{
    struct Task *t = gUnk_03002490;

    t->unk3C = 0xFFFF;
    t->unk34 = 0;
    t->unk20 = 0;
    t->unk1C = 3;
    t->unk18 = 0;
}

s32 sub_0807777c(struct M19Script *p)
{
    struct Task *t = gUnk_03002490;
    u8 *q;
    register s32 i asm("r1");
    register u16 v asm("r0");

    if (t->unk24 == 1)
    {
        if (t->unk18 != 0)
        {
            i = t->unk1C * 2;
            q = (u8 *)p + 18;
            q += i;
            v = *(u16 *)q;
            return (s16)v;
        }
    }
    else
    {
        if (t->unk18 == 0)
        {
            i = t->unk1C * 2;
            q = (u8 *)p + 18;
            q += i;
            v = *(u16 *)q;
            return (s16)v;
        }
    }
    i = t->unk1C * 2;
    q = (u8 *)p + 6;
    q += i;
    v = *(u16 *)q;
    return (s16)v;
}

void sub_080777bc(s32 x, s32 y, s32 d)
{
    struct M19Script *p = gUnk_087401E4[gUnk_03002490->unk20];
    struct Task *t = gUnk_03002490;

    {
        s32 k = d + t->unk20 * 4;

        t->unk18 = gUnk_087401CC[k];
    }
    if (t->unk24 == 1)
    {
        sub_080269d8(x, y, 0);
        {
            struct Task *u = gUnk_03002490;

            u->unk1C = 0;
            u->unk3C = p->unk04;
        }
    }
    else
    {
        sub_080269d8(t->unk48, t->unk4A, 1);
        {
            struct Task *u = gUnk_03002490;

            u->unk1C = p->unk00[2] - 1;
            u->unk3C = 0xFFFF;
        }
    }
}

void sub_08077830(void)
{
    struct M19Script *p = gUnk_087401E4[gUnk_03002490->unk20];
    struct Task *t = gUnk_03002490;

    if (t->unk34 <= 0)
    {
        gUnk_03002490->unk3C = sub_0807777c(p);
        if (gUnk_03002490->unk3C != -1)
        {
            struct Task *u = gUnk_03002490;

            u->unk1C++;
            sub_080779dc();
        }
        else
        {
            sub_08077898(p);
        }
        gUnk_03002490->unk34 = 2;
    }
    gUnk_03002490->unk34--;
}

void sub_08077898(struct M19Script *p)
{
    struct Task *t = gUnk_03002490;
    u8 d = p->unk00[t->unk18];
    s32 x = t->unk48;
    s32 y = t->unk4A;
    s32 id;

    switch (d)
    {
    case 0:
        y -= 16;
        break;
    case 1:
        y += 16;
        break;
    case 2:
        x -= 16;
        break;
    case 3:
        x += 16;
        break;
    }
    id = sub_08022540(x, y);
    gUnk_03002490->unk20 = id;
    if (id != -1)
    {
        sub_080777bc(x, y, d);
    }
    else
    {
        struct Task *u = gUnk_03002490;

        if (u->unk24 == 1)
        {
            gUnk_02006094 = u->unk24;
            sub_08005654(u->unk46);
        }
        else
        {
            sub_080269d8(u->unk48, u->unk4A, 1);
            gUnk_02006094 = id;
            gUnk_03002490->unk3C = 42;
            sub_0806395c(0);
        }
    }
    {
        struct Task *v = gUnk_03002490;

        v->unk48 = x;
        v->unk4A = y;
        v->unk4C = v->unk48 << 16;
        v->unk50 = v->unk4A << 16;
    }
}

void sub_08077980(void)
{
    struct M19Script *p = gUnk_087401E4[gUnk_03002490->unk20];
    struct Task *t = gUnk_03002490;

    if (t->unk34 <= 0)
    {
        if (t->unk1C < 0)
        {
            sub_08077898(p);
        }
        else
        {
            struct Task *u;

            gUnk_03002490->unk3C = sub_0807777c(p);
            u = gUnk_03002490;
            u->unk1C--;
        }
        gUnk_03002490->unk34 = 2;
    }
    gUnk_03002490->unk34--;
}

void sub_080779dc(void)
{
    struct Task *t = gUnk_03002490;
    s32 i = t->unk3C;
    s32 j = i * 2;
    struct Task *u = &gUnk_03002790[t->unk46];

    u->unk48 = t->unk48 + gUnk_08740124[i * 2];
    u->unk4A = t->unk4A + gUnk_08740124[j + 1];
    u->unk4C = u->unk48 << 16;
    u->unk50 = u->unk4A << 16;
}

void sub_08077a48(void)
{
    gUnk_03002490->unk46 = sub_08064d34(157, 1);
}

void sub_08077a64(void)
{
    struct Task *t = gUnk_03002490;

    t->unk04 = (u32)sub_08077aa8;
    t->unk30 = t->unk48;
    t->unk2C = t->unk4A;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087402BC);
}

void sub_08077aa8(void)
{
    sub_08002e98(gUnk_03002490->unk15, 3, gUnk_087402C8);
}

void sub_08077ac4(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087402BC);
}
