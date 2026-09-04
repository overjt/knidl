/* game_code_and_rodata 0x08077AE0-0x08078B68 (issue #79, module M19 batch 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08077AE0 0x08078B68 src/actor_77ae0.c --newpb
 *
 * M19 batch 5: task types #77 (sub_08077c64), #78 (sub_08077f0c), #79
 * (sub_08078598, the credits particle system over gUnk_03000FE0) and #8
 * (sub_0807893c), whose states hand off to module M20.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"


/* The eight 4-byte records at 0x03000FE0 that M19's credits tasks animate:
   a frame table index (unk00), the frame within it (unk01), the timer
   (unk02) and the countdown sub_080782b4 draws on (unk03). */
struct M19Particle
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u8 unk02;
    /*0x03*/ u8 unk03;
};

/* The 24-entry animation rows at 0x08740320 / 0x087404A0 the credits
   particles walk: `unk00` indexes the gfx pointer table, `unk02` is the
   step's delay. */
struct M19Frame
{
    /*0x00*/ u8 unk00;
    /*0x01*/ u8 unk01;
    /*0x02*/ u8 unk02;
    /*0x03*/ u8 unk03;
};


/* RAM cells and ROM tables */
extern s16 gUnk_03002348;
extern s16 gUnk_030023E4;
extern s8 gUnk_02006094;
extern struct AnimCmd gUnk_087406A0[];
extern struct M19Frame gUnk_08740320[][24];
extern struct M19Frame gUnk_087404A0[][24];
extern struct M19Particle gUnk_03000FE0[];
extern struct PlayerState gUnk_03002170[];
extern struct Task * gUnk_03002490;
extern u16 gUnk_030023AC;
extern u32 gUnk_087402D4[];
extern u32 gUnk_087402D8[];
extern u32 gUnk_087402E4[];
extern u32 gUnk_087402F0[];
extern u32 gUnk_087402F4[];
extern u32 gUnk_087402F8[];
extern u32 gUnk_087402FC[];
extern u32 gUnk_08740630[];
extern u32 gUnk_08740BD4[];
extern u32 gUnk_08752118[];
extern u32 gUnk_08752D40[];
extern u32 gUnk_08752D48[];
extern u32 gUnk_08752DB8[];
extern u32 gUnk_08752E00[];
extern u8 gUnk_02004B64;
extern u8 gUnk_03002340;
extern u8 gUnk_08740620[];
extern vs16 gUnk_03000FB8;
extern vs32 gCurTaskIdx;

/* callees */
extern s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
extern s32 sub_080031b8(u32 a);
extern s32 sub_08009e14();
extern s32 sub_08009e20();
extern s32 sub_08021b18(u16 x, u16 y);
extern s32 sub_08025bc8();
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_080b4204();
extern u32 sub_08002ee8(u32 range);
extern u32 sub_08068e04(void);
extern u32 sub_080692fc(void);
extern void TaskYieldTrampoline(u32 a);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_080037f8(u16 speed);
extern void sub_0800381c(s32 speed);
extern void sub_08006138(void);
extern void sub_08006148(void *a, u32 i);
extern void sub_080062c4(void);
extern void sub_080261d4(u32 a);
extern void sub_08027204(u32);
extern void sub_0806395c(u8 v);
extern void sub_08063974(u32 i, u8 v);
extern void sub_08063fe0(void);
extern void sub_08063ff4(void);
extern void sub_080651b4(void);
extern void sub_0806523c(void);
extern void sub_080656b4(void);
extern void sub_08066b70(void);
extern void sub_08066c08(u32 *p, s32 b);
extern void sub_08066c3c(u32 *p);
extern void sub_080670ac(u16 a);
extern void sub_080670d4(void);
extern void sub_08067108(void);
extern void sub_08067114(void);
extern void sub_0806a0f0(s32 a);
extern void sub_0806a344(void);
extern void sub_0806ee2c(void);
extern void sub_0807775c(void);
extern void sub_08077830(void);
extern void sub_08077980(void);
extern void sub_08077a48(void);
extern void sub_08077ac4(void);
extern void sub_08078c64(void);
extern void sub_08078d6c(void);
extern void sub_08078e80(void);
extern void sub_08079178(void);
extern void sub_0807938c(void);

/* defined below */
void sub_08077e30(void);
void sub_08077e08(void);
void sub_08077f7c(void);
void sub_080781fc(struct M19Particle *p);
void sub_080782b4(struct M19Particle *p);
void sub_0807831c(struct M19Particle *p);
u8 sub_080783e0(s16 x, s16 y);
void sub_080786b4(void);
void sub_08078734(void);
void sub_080787b8(void);
void sub_0807883c(void);
void sub_080788e0(void);

void sub_08077ae0(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk43 = 1;
    sub_080062c4();
    gUnk_02006094 = -1;
    {
        struct Task *t = gUnk_03002490;

        t->unk4C = t->unk30 << 16;
        t->unk50 = t->unk2C << 16;
        t->unk3C = 42;
    }
    sub_08006138();
}

void sub_08077b24(void)
{
    if (sub_08068e04())
    {
        gUnk_02006094 = 0;
        gUnk_03002490->unk24 = 1;
        sub_0806395c(1);
        sub_08006148(sub_08077ac4, gCurTaskIdx);
    }
}

void sub_08077b60(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080062c4();
    sub_0807775c();
    sub_08077a48();
    while (gUnk_02006094 == 0)
    {
        sub_080031b8(230);
        TaskYieldTrampoline(3);
    }
    sub_08006138();
}

void sub_08077ba8(void)
{
    if (gUnk_03002490->unk20 != -1)
    {
        sub_08077830();
    }
    else if (gUnk_02006094 == 0)
    {
        sub_0806395c(2);
        sub_08006148(sub_08077ac4, gCurTaskIdx);
    }
}

void sub_08077bf0(void)
{
    gUnk_03002490->unk15 = 2;
    sub_080062c4();
    gUnk_03002490->unk24 = -1;
    sub_0807775c();
    {
        struct Task *t = gUnk_03002490;

        t->unk4A += 16;
        t->unk50 = t->unk4A << 16;
    }
    sub_08006138();
}

void sub_08077c2c(void)
{
    if (gUnk_03002490->unk20 != -1)
        sub_08077980();
    if (gUnk_03002490->unk14 != 2)
        sub_08006148(sub_08077ac4, gCurTaskIdx);
}

void sub_08077c64(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk00 = (u32)sub_080656b4;
        t->unk0C = (u32)sub_080651b4;
        t->unk42 = 11;
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk38 = gUnk_08752D40;
        sub_08002e98(t->unk73, 1, gUnk_087402D4);
    }
}

s32 sub_08077ca4(void)
{
    struct PlayerState *p = &gUnk_03002170[gUnk_03002490->unk7E];

    if (p->unk0D == 7 && p->unk04 == 13)
        return 0;
    return 1;
}

void sub_08077cd4(void)
{
    gUnk_03002490->unk3C = 1;
    sub_080670ac(15);
    sub_08027204(2);
}

void sub_08077cf4(void)
{
    sub_08067108();
    gUnk_02004B64 = 1;
    sub_080031b8(226);
    sub_080261d4(4);
    sub_08077cd4();
    sub_08009e14();
    sub_08063974(gCurTaskIdx, 1);
    sub_08006148(sub_08077e30, gCurTaskIdx);
}

void sub_08077d38(s32 id)
{
    sub_08063974(id, 2);
    sub_08006148(sub_08077e30, id);
}

void sub_08077d54(void)
{
    u8 i;

    sub_08067108();
    TaskYieldTrampoline(15);
    sub_080037f8(16);
    TaskYieldTrampoline(15);
    sub_080261d4(4);
    gUnk_02004B64 = 0;
    for (i = 0; i < gUnk_030023AC; i++)
    {
        if ((gUnk_03002340 >> i) & 1)
        {
            u8 done;

            do
            {
                sub_080031b8(221);
                done = sub_080b4204(i);
                TaskYieldTrampoline(8);
            } while (done == 0);
        }
    }
    sub_080670d4();
    sub_08009e20();
    sub_08067114();
    sub_0806a344();
}

void sub_08077dd8(void)
{
    gUnk_03002490->unk04 = (u32)sub_08077e08;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087402D8);
}

void sub_08077e08(void)
{
    if ((u8)sub_080692fc() == 0)
        sub_08002e98(gUnk_03002490->unk15, 3, gUnk_087402E4);
}

void sub_08077e30(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_087402D8);
}

void sub_08077e4c(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk43 = 1;
    sub_080062c4();
    gUnk_03002490->unk3C = 0;
    sub_08006138();
}

void sub_08077e74(void)
{
    if (gUnk_02004B64 == 0 && sub_08068e04() && (u8)sub_08077ca4())
        sub_08077cf4();
}

void sub_08077e9c(void)
{
    gUnk_03002490->unk15 = 1;
    TaskYieldTrampoline(8);
    sub_0800381c(16);
    sub_08025bc8(gCurTaskIdx);
    sub_08006138();
}

void sub_08077ecc(void)
{
}

void sub_08077ed0(void)
{
    gUnk_03002490->unk15 = 2;
    while (gUnk_03000FB8 != 0)
        TaskYieldTrampoline(1);
    sub_08077d54();
    sub_08006138();
}

void sub_08077f08(void)
{
}

void sub_08077f0c(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk00 = (u32)sub_080656b4;
        t->unk0C = (u32)sub_080651b4;
        t->unk42 = 11;
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk38 = gUnk_08752D48;
        sub_08002e98(t->unk73, 1, gUnk_087402F0);
    }
}

void sub_08077f4c(void)
{
    gUnk_03002490->unk04 = (u32)sub_08077f7c;
    sub_0806395c(0);
    sub_08002e98(gUnk_03002490->unk14, 1, gUnk_087402F4);
}

void sub_08077f7c(void)
{
    sub_08002e98(gUnk_03002490->unk15, 1, gUnk_087402F8);
}

void sub_08077f98(void)
{
    gUnk_03002490->unk15 = 0;
    gUnk_03002490->unk3C = 0;
    while (sub_08021b18(gUnk_03002490->unk48, gUnk_03002490->unk4A) == 51)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(5);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = 0xFFFF;
    sub_08063fe0();
}

void sub_08077ff4(void)
{
}

void sub_08077ff8(void)
{
    gUnk_03002490->unk28 = 0;
    do
    {
        sub_080781fc(&gUnk_03000FE0[gUnk_03002490->unk28]);
        gUnk_03002490->unk28++;
    } while (gUnk_03002490->unk28 <= 7);
}

void sub_0807802c(void)
{
    if (gUnk_030023E4 > gUnk_03002490->unk34)
    {
        for (gUnk_03002490->unk28 = 0;
             gUnk_03002490->unk28 < gUnk_03002490->unk30;
             gUnk_03002490->unk28++)
        {
            struct M19Particle *base = gUnk_03000FE0;
            struct M19Particle *p = &base[gUnk_03002490->unk28];

            if (p->unk03 > 8)
                sub_080782b4(p);
        }
    }
    else
    {
        for (gUnk_03002490->unk28 = 0;
             gUnk_03002490->unk28 < gUnk_03002490->unk30;
             gUnk_03002490->unk28++)
        {
            struct M19Particle *base = gUnk_03000FE0;
            struct M19Particle *p = &base[gUnk_03002490->unk28];

            if (p->unk03 > gUnk_03002490->unk34 - gUnk_030023E4)
                sub_080782b4(p);
        }
    }
}

void sub_080780c4(void)
{
    gUnk_03002490->unk28 = 0;
    do
    {
        sub_0807831c(&gUnk_03000FE0[gUnk_03002490->unk28]);
        gUnk_03002490->unk28++;
    } while (gUnk_03002490->unk28 <= 2);
    gUnk_03002490->unk28 = 3;
    do
    {
        sub_0807831c(&gUnk_03000FE0[gUnk_03002490->unk28]);
        gUnk_03002490->unk28++;
    } while (gUnk_03002490->unk28 <= 5);
}

void sub_0807811c(struct M19Particle *p)
{
    p->unk01++;
    if (gUnk_08740320[p->unk00][p->unk01].unk00 == 255)
        p->unk01 = 0;
    {
        s32 j = p->unk01 * 4;
        s32 k = p->unk00 * 96;

        p->unk02 += ((u8 *)gUnk_08740320)[j + k + 2];
    }
    if (p->unk02 > 240)
    {
        p->unk02 = 0;
        p->unk03 = sub_08002ee8(132) + 8;
    }
}

void sub_0807817c(struct M19Particle *p, u8 a, u8 b)
{
    switch (a)
    {
    case 0:
    case 1:
        p->unk02 = sub_08002ee8(64) + 100;
        break;
    case 2:
        p->unk02 = sub_08002ee8(48) + 170;
        break;
    case 3:
        switch (b)
        {
        case 0:
            p->unk02 = sub_08002ee8(48) + 120;
            break;
        case 1:
            p->unk02 = sub_08002ee8(48) + 40;
            break;
        }
        break;
    }
    p->unk03 = sub_08002ee8(132) + 8;
    p->unk01 = sub_08002ee8(10);
    p->unk00 = gUnk_08740620[gUnk_03002490->unk28];
}

void sub_080781fc(struct M19Particle *p)
{
    struct Task *t = gUnk_03002490;

    sub_08001a94(t->unk42,
                 gUnk_08752DB8[gUnk_08740320[p->unk00][p->unk01].unk00],
                 t->unk3E, t->unk40, p->unk02, p->unk03);
}

void sub_08078258(struct M19Particle *p)
{
    struct Task *t = gUnk_03002490;

    sub_08001a94(t->unk42,
                 gUnk_08752E00[gUnk_087404A0[p->unk00][p->unk01].unk00],
                 t->unk3E, t->unk40, p->unk02, p->unk03);
}

void sub_080782b4(struct M19Particle *p)
{
    struct Task *t = gUnk_03002490;

    sub_08001a94(t->unk42,
                 gUnk_08752E00[gUnk_087404A0[p->unk00][p->unk01].unk00],
                 t->unk3E, t->unk40, p->unk02 - gUnk_03002348, p->unk03);
}

void sub_0807831c(struct M19Particle *p)
{
    gUnk_03002490->unk2C = 0;
    do
    {
        if (sub_080783e0(p->unk02 - gUnk_03002348 + gUnk_03002490->unk2C * 192,
                         p->unk03))
        {
            struct Task *t = gUnk_03002490;

            sub_08001a94(t->unk42,
                         gUnk_08752E00[gUnk_087404A0[p->unk00][p->unk01].unk00],
                         t->unk3E, t->unk40,
                         p->unk02 - gUnk_03002348 + t->unk2C * 192,
                         p->unk03);
        }
        gUnk_03002490->unk2C++;
    } while (gUnk_03002490->unk2C <= 4);
}

u8 sub_080783e0(s16 x, s16 y)
{
    u16 v = y;

    if ((u32)((x << 16) + 0x3F0000) > 0x16E0000)
        return 0;
    if ((s16)v <= -64)
        return 0;
    if ((s16)v > 223)
        return 0;
    return 1;
}

void sub_0807840c(struct M19Particle *p, u8 a)
{
    register u8 *tab asm("r3");
    register s32 q asm("r0");
    s32 i;

    p->unk01++;
    tab = (u8 *)gUnk_087404A0;
    i = p->unk01 * 4 + p->unk00 * 96;
    if (*(u8 *)(i + (s32)tab) == 255)
        p->unk01 = 0;
    switch (a)
    {
    case 0:
        {
            s32 i = p->unk01 * 4 + p->unk00 * 96;

            q = (s32)tab + 3;
            p->unk03 -= *(u8 *)(i + q);
        }
        if (p->unk03 <= 7)
        {
            p->unk02 = sub_08002ee8(250);
            p->unk03 = 160;
        }
        break;
    case 2:
        {
            s32 i = p->unk01 * 4 + p->unk00 * 96;

            q = (s32)tab + 3;
            p->unk03 -= *(u8 *)(i + q);
        }
        if (p->unk03 <= 7)
        {
            {
                s32 v = sub_08002ee8(48) + 232;

                p->unk02 = v + (u8)gUnk_03002490->unk48;
            }
            p->unk03 = 160;
        }
        break;
    case 1:
        {
            s32 i = p->unk01 * 4 + p->unk00 * 96;

            q = (s32)tab + 3;
            p->unk03 -= *(u8 *)(i + q);
        }
        if (p->unk03 <= 7)
        {
            {
                s32 v = sub_08002ee8(64) + 224;

                p->unk02 = v + (u8)gUnk_03002490->unk48;
            }
            p->unk03 = 160;
        }
        break;
    case 3:
        {
            s32 i = p->unk01 * 4 + p->unk00 * 96;

            q = (s32)tab + 3;
            p->unk03 -= *(u8 *)(i + q);
        }
        if (p->unk03 <= 7)
        {
            {
                s32 v = sub_08002ee8(48) + 232;

                p->unk02 = v + (u8)gUnk_03002490->unk48 + 96;
            }
            p->unk03 = 160;
        }
        break;
    case 4:
        {
            s32 i = p->unk01 * 4 + p->unk00 * 96;

            q = (s32)tab + 3;
            p->unk03 += *(u8 *)(i + q);
        }
        if (p->unk03 > 160)
        {
            {
                s32 v = sub_08002ee8(48) + 232;

                p->unk02 = v + (u8)gUnk_03002490->unk48;
            }
            p->unk03 = 0;
        }
        break;
    }
}

void sub_08078598(void)
{
    switch (gUnk_03002490->unk73)
    {
    case 0:
        {
            struct Task *t = gUnk_03002490;

            t->unk0C = (u32)sub_08077ff8;
            t->unk38 = gUnk_08752DB8;
        }
        break;
    case 1:
        {
            struct Task *t = gUnk_03002490;

            t->unk0C = (u32)sub_0807802c;
            t->unk38 = gUnk_08752E00;
        }
        break;
    case 2:
        {
            struct Task *t = gUnk_03002490;

            t->unk0C = (u32)sub_0807802c;
            t->unk38 = gUnk_08752E00;
        }
        break;
    case 3:
        {
            struct Task *t = gUnk_03002490;

            t->unk0C = (u32)sub_0807802c;
            t->unk38 = gUnk_08752E00;
        }
        break;
    case 4:
        {
            struct Task *t = gUnk_03002490;

            t->unk0C = (u32)sub_080780c4;
            t->unk38 = gUnk_08752E00;
        }
        break;
    default:
        sub_0806ee2c();
        break;
    }
    gUnk_03002490->unk42 = 4;
    {
        struct Task *t = gUnk_03002490;

        t->unk40 = 0xF000 | t->unk40;
        sub_08002e98(t->unk73, 5, gUnk_087402FC);
    }
}

void sub_08078670(void)
{
    gUnk_03002490->unk04 = (u32)sub_080786b4;
    gUnk_03002490->unk28 = 0;
    do
    {
        sub_0807817c(&gUnk_03000FE0[gUnk_03002490->unk28], 0, 0);
        gUnk_03002490->unk28++;
    } while (gUnk_03002490->unk28 <= 7);
    sub_08006138();
}

void sub_080786b4(void)
{
    gUnk_03002490->unk28 = 0;
    do
    {
        sub_0807811c(&gUnk_03000FE0[gUnk_03002490->unk28]);
        gUnk_03002490->unk28++;
    } while (gUnk_03002490->unk28 <= 7);
}

void sub_080786e8(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk04 = (u32)sub_08078734;
        t->unk30 = 8;
        t->unk34 = 0;
    }
    gUnk_03002490->unk28 = 0;
    do
    {
        sub_0807817c(&gUnk_03000FE0[gUnk_03002490->unk28], 0, 0);
        gUnk_03002490->unk28++;
    } while (gUnk_03002490->unk28 <= 7);
    sub_08006138();
}

void sub_08078734(void)
{
    gUnk_03002490->unk28 = 0;
    do
    {
        sub_0807840c(&gUnk_03000FE0[gUnk_03002490->unk28], 0);
        gUnk_03002490->unk28++;
    } while (gUnk_03002490->unk28 <= 7);
}

void sub_0807876c(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk04 = (u32)sub_080787b8;
        t->unk30 = 3;
        t->unk34 = 208;
    }
    gUnk_03002490->unk28 = 0;
    do
    {
        sub_0807817c(&gUnk_03000FE0[gUnk_03002490->unk28], 2, 0);
        gUnk_03002490->unk28++;
    } while (gUnk_03002490->unk28 <= 2);
    sub_08006138();
}

void sub_080787b8(void)
{
    gUnk_03002490->unk28 = 0;
    do
    {
        sub_0807840c(&gUnk_03000FE0[gUnk_03002490->unk28], 2);
        gUnk_03002490->unk28++;
    } while (gUnk_03002490->unk28 <= 2);
}

void sub_080787f0(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk04 = (u32)sub_0807883c;
        t->unk30 = 4;
        t->unk34 = 64;
    }
    gUnk_03002490->unk28 = 0;
    do
    {
        sub_0807817c(&gUnk_03000FE0[gUnk_03002490->unk28], 1, 0);
        gUnk_03002490->unk28++;
    } while (gUnk_03002490->unk28 <= 3);
    sub_08006138();
}

void sub_0807883c(void)
{
    gUnk_03002490->unk28 = 0;
    do
    {
        sub_0807840c(&gUnk_03000FE0[gUnk_03002490->unk28], 1);
        gUnk_03002490->unk28++;
    } while (gUnk_03002490->unk28 <= 3);
}

void sub_08078874(void)
{
    gUnk_03002490->unk04 = (u32)sub_080788e0;
    gUnk_03002490->unk28 = 0;
    do
    {
        sub_0807817c(&gUnk_03000FE0[gUnk_03002490->unk28], 3, 0);
        gUnk_03002490->unk28++;
    } while (gUnk_03002490->unk28 <= 2);
    gUnk_03002490->unk28 = 3;
    do
    {
        sub_0807817c(&gUnk_03000FE0[gUnk_03002490->unk28], 3, 1);
        gUnk_03002490->unk28++;
    } while (gUnk_03002490->unk28 <= 5);
    sub_08006138();
}

void sub_080788e0(void)
{
    gUnk_03002490->unk28 = 0;
    do
    {
        sub_0807840c(&gUnk_03000FE0[gUnk_03002490->unk28], 3);
        gUnk_03002490->unk28++;
    } while (gUnk_03002490->unk28 <= 2);
    gUnk_03002490->unk28 = 3;
    do
    {
        sub_0807840c(&gUnk_03000FE0[gUnk_03002490->unk28], 4);
        gUnk_03002490->unk28++;
    } while (gUnk_03002490->unk28 <= 5);
}

void sub_0807893c(void)
{
    {
        struct Task *t = gUnk_03002490;

        t->unk00 = (u32)sub_080656b4;
        t->unk0C = (u32)sub_0806523c;
        t->unk42 = 11;
    }
    {
        struct Task *t = gUnk_03002490;

        t->unk38 = gUnk_08752118;
        t->unk8C->unk1A = 4;
        sub_08002e98(t->unk73, 6, gUnk_08740630);
    }
}

s32 sub_08078984(void)
{
    sub_08066c08(gUnk_08740BD4, 0);
    sub_08006148(sub_0806a344, gCurTaskIdx);
    return 1;
}

s32 sub_080789ac(void)
{
    s32 r = 0;

    switch (gUnk_03002490->unk73)
    {
    case 0:
        sub_0806395c(1);
        sub_08006148(sub_08078c64, gCurTaskIdx);
        r = 1;
        break;
    case 1:
        sub_0806395c(1);
        sub_08006148(sub_08078d6c, gCurTaskIdx);
        r = 1;
        break;
    case 2:
        sub_0806395c(2);
        sub_08006148(sub_08078e80, gCurTaskIdx);
        r = 1;
        break;
    case 3:
        sub_0806395c(1);
        sub_08006148(sub_08079178, gCurTaskIdx);
        r = 1;
        break;
    case 5:
        sub_0806395c(1);
        sub_08006148(sub_0807938c, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 sub_08078a48(void)
{
    s32 r = 0;

    switch (gUnk_03002490->unk73)
    {
    case 0:
        sub_0806395c(0);
        sub_08006148(sub_08078c64, gCurTaskIdx);
        r = 1;
        break;
    case 1:
        sub_0806395c(0);
        sub_08006148(sub_08078d6c, gCurTaskIdx);
        r = 1;
        break;
    case 2:
        {
            struct Task *t = gUnk_03002490;

            if (t->unk28 <= 0)
                t->unk28 = 30;
        }
        gUnk_03002490->unk34 = sub_080640dc(gUnk_087406A0);
        sub_0806395c(0);
        sub_08006148(sub_08078e80, gCurTaskIdx);
        r = 1;
        break;
    case 3:
        sub_08066c3c(gUnk_08740BD4);
        sub_0806395c(0);
        sub_08006148(sub_08079178, gCurTaskIdx);
        r = 1;
        break;
    case 5:
        sub_0806395c(0);
        sub_08006148(sub_0807938c, gCurTaskIdx);
        r = 1;
        break;
    }
    return r;
}

s32 sub_08078b08(void)
{
    u8 v = gUnk_03002490->unk73;

    if (v == 3 || v == 5)
        sub_08066c08(gUnk_08740BD4, 0);
    sub_0806a0f0(-2);
    return 1;
}

s32 sub_08078b38(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk73 == 3 && t->unk14 == 1)
        sub_08066b70();
    else
        sub_08063ff4();
    return 0;
}

s32 sub_08078b64(void)
{
    return 0;
}
