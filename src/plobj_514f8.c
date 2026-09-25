#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* plobj_514f8.c (0x080514F8-0x0805239B, issue #90).
 *
 * Task type #6, variants 3-6, each variant body followed by the callbacks
 * only it installs; variants 3-5 read their spawner's task through
 * Task.unk8C (position, facing, OAM flags).  Variant 3 (sub_080514f8,
 * animation table gUnk_087519E8) is a four-way `switch (Task.unk28)` of
 * endless loops that the ROM lays out in the order 3, 1, 0, 2, with M11's
 * callback sub_0803dfc8 and the collision callback sub_0805176c (sound
 * 133).  Variant 4 (sub_0805181c, gUnk_08751A28/gUnk_08751CA4) installs
 * sub_08051b0c, which registers the collider row gUnk_0873BDD4 and calls
 * the hit test sub_08030804 at the spawner's position with only three
 * arguments.  Variant 5 (sub_08051c1c, gUnk_08751A98) and its callback
 * sub_08051d84 re-bind the body or the shared exit sub_08050814.  Variant
 * 6 (sub_08051f4c, gUnk_08751AF8, the animation/velocity pairs
 * gUnk_0873B7B0) and its callback sub_080520dc (a nine-way `switch` on
 * the collision result gUnk_03005550.unk4, sounds 173 and 211) turn the
 * object into variant 10's body sub_08052f6c or the burst sub_0805091c on
 * contact. */

/* M11's per-player records (src/stage_43654.c spells them the same way) */
struct M11R8 { u8 unk00; u8 unk01; u8 unk02; u8 unk03; u8 *unk04; };

struct M11R20 { u32 w[5]; };

/* gUnk_03005550: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh).  A 16-bit test of
   unk0/unk1 together is `*(u16 *)&gUnk_03005550` (M12's sub_08045a50). */
struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

extern struct M11R8 gUnk_02005550[];
extern struct M11R20 gUnk_020060E0[];
extern u32 gUnk_087519E8[];
extern u32 gUnk_0873BDA0[];
extern u32 gUnk_0873BDB4[];
extern u32 gUnk_0873CBAC[];
extern u32 gUnk_0873CBB4[];
extern u32 gUnk_08751A28[];
extern u32 gUnk_08751CA4[];
extern u32 gUnk_0873BDD4[];
extern u32 gUnk_0873CBDC[];
extern u32 gUnk_08751A98[];
extern s8 gUnk_0873CB54[];
extern struct Unk03005550 gUnk_03005550;
extern u16 gUnk_03001EA4;
extern u32 gUnk_0873CBEC[];
extern u32 gUnk_0873BDE8[];
extern u32 gUnk_08751AF8[];
extern u16 gUnk_0873B7B0[][2];
extern s8 gUnk_0873CB5C[];
extern u32 gUnk_0873CBFC[];
extern u32 gUnk_0873CC0C[];
extern s8 gUnk_0873CB64[];
extern u32 gUnk_0873BDFC[];
extern u32 gUnk_0873BE10[];

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(s32 frames);
u32 sub_08002ee8(u32 range);
void sub_08005654(s32 id);
void sub_080059d8(void);
void sub_080059fc(void);
void sub_08005d9c(void);
void sub_08005ea8(void);
void sub_08005f30(void);
void sub_08006148(void *a, u32 i);
void sub_0800617c(s16 a);
void sub_080061c0(s32 a, s32 b);
void sub_08006214(void);
void sub_08006244(void);
void sub_080062c4(void);
u16 sub_080064ac(u16 base, u8 scale, u8 amount);
s16 sub_080064dc(u16 base, u8 scale, u8 amount);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
void sub_0801c230(const s8 *p);
void sub_0801c444(const s8 *p);
void sub_0802205c(s8 *box);
void sub_0802233c(s8 *off);
void sub_080224b0(void);
u16 sub_08030804(struct HitBoxSet *p, s32 x, s32 y);   /* this file's call passes only x and y: the ROM leaves r3 as it was (the definition in src/block_30804.c takes a fourth, `e`) */
s32 sub_08030848(struct HitBoxSet *p, s32 e);   /* M14's callers test r0 unnarrowed (good/sub_08050c48.c); landed M09/M12/M13 files spell it u16 */
void sub_0803dfc8(void);
s32 sub_0803e34c(s32 a0, u16 a1);
s32 sub_0803e5c0(s32 playerIdx, u8 *src6);
s32 sub_0803e5f8(s32 a0, s32 a1);
void sub_08050814(void);
void sub_0805091c(void);
void sub_08052f6c(void);
void sub_0805176c(void);
void sub_08051b0c(void);
void sub_08051d84(void);
void sub_080520dc(void);

void sub_080514f8(void)
{
    {
        struct Task *t = gUnk_03002490;
        t->unk00 = (u32)sub_080059fc;
        t->unk0C = (u32)sub_0803dfc8;
        t->unk04 = (u32)sub_0805176c;
        t->unk42 = 7;
    }
    {
        struct Task *t = gUnk_03002490;
        t->unk38 = gUnk_087519E8;
        t->unk40 = ((struct Task *)t->unk8C)->unk40 | 0xE006;
        t->unk28 = ((struct Task *)t->unk8C)->unk28;
        t->unk2C = 1;
        gUnk_020060E0[t->unk88->unk00] = *(struct M11R20 *)gUnk_0873BDA0;
    }
    sub_0803e5c0(gUnk_03002490->unk88->unk00, (u8 *)gUnk_0873BDB4 + gUnk_03002490->unk28 * 8);
    {
        struct Task *t;
        gUnk_02005550[(t = gUnk_03002490)->unk88->unk00] = *(struct M11R8 *)gUnk_0873CBAC;
        sub_0803e5f8(t->unk88->unk00, (s32)((u8 *)gUnk_0873CBB4 + t->unk28 * 8));
    }
    switch (gUnk_03002490->unk28)
    {
    case 3:
        while (1)
        {
            {
                struct Task *t = gUnk_03002490;
                t->unk4C = 0;
                t->unk50 = -0x100000;
            }
            sub_08006244();
            {
                struct Task *t = gUnk_03002490;
                t->unk58 = -0x30000;
                t->unk60 = 0x8000;
                t->unk3C = 8;
            }
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
        }
    case 1:
        while (1)
        {
            {
                struct Task *t = gUnk_03002490;
                t->unk4C = 0;
                t->unk50 = 0x100000;
            }
            sub_08006244();
            {
                struct Task *t = gUnk_03002490;
                t->unk58 = 0x30000;
                t->unk60 = -0x8000;
                t->unk3C = 9;
            }
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
        }
    case 0:
        while (1)
        {
            {
                struct Task *t = gUnk_03002490;
                t->unk4C = 0x100000;
                t->unk50 = 0;
            }
            sub_08006214();
            {
                struct Task *t = gUnk_03002490;
                t->unk54 = 0x30000;
                t->unk5C = -0x8000;
                t->unk3C = 0;
            }
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
        }
    case 2:
        while (1)
        {
            {
                struct Task *t = gUnk_03002490;
                t->unk4C = -0x100000;
                t->unk50 = 0;
            }
            sub_08006214();
            {
                struct Task *t = gUnk_03002490;
                t->unk54 = -0x30000;
                t->unk5C = 0x8000;
                t->unk3C = 1;
            }
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(2);
        }
    }
}

void sub_0805176c(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk88->unk04 != 11 || t->unk28 != ((struct Task *)t->unk8C)->unk28)
    {
        sub_08005654(gCurTaskIdx);
        return;
    }
    if (--t->unk2C == 0)
    {
        t->unk2C = 4;
        sub_0803e34c(133, t->unk44);
    }
    sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                 (u8 *)gUnk_020060E0 + gUnk_03002490->unk88->unk00 * 20);
    sub_08030848((struct HitBoxSet *)&gUnk_02005550[gUnk_03002490->unk88->unk00], gUnk_03002490->unk44);
}

void sub_0805181c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk04 = (u32)sub_08051b0c;
    t->unk28 = 0;
    t->unk80 = 1;
    t = gUnk_03002490;
    switch (t->unk18 & 15)
    {
    case 0:
        t->unk0C = (u32)sub_0803dfc8;
        t->unk38 = gUnk_08751A28;
        t->unk42 = 7;
        {
            struct Task *u = gUnk_03002490;
            u->unk40 = (((struct Task *)u->unk8C)->unk40 + 0x1800) | 8;
            u->unk3C = 0xFFFF;
        }
        TaskYieldTrampoline(18);
        while (gUnk_03002490->unk28 == 0)
        {
            gUnk_03002490->unk4C = (sub_080064dc(24, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(0, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            sub_080061c0(0x10000, 0x4000);
            gUnk_03002490->unk58 = 0;
            gUnk_03002490->unk60 = (sub_08002ee8(36) - 24) << 8;
            sub_0800617c(0);
            TaskYieldTrampoline(2);
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
            {
                gUnk_03002490->unk3C += 2;
                TaskYieldTrampoline(2);
            }
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(1);
            if (gUnk_03002490->unk28 != 0)
                break;
            gUnk_03002490->unk4C = (sub_080064dc(24, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk48) << 16;
            gUnk_03002490->unk50 = (sub_080064ac(0, 1, 8) + ((struct Task *)gUnk_03002490->unk8C)->unk4A) << 16;
            sub_080061c0(0x10000, 0x4000);
            gUnk_03002490->unk58 = 0;
            gUnk_03002490->unk60 = (sub_08002ee8(36) - 24) << 8;
            sub_0800617c(14);
            TaskYieldTrampoline(2);
            for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 4; gUnk_03002490->unk6C++)
            {
                gUnk_03002490->unk3C += 2;
                TaskYieldTrampoline(2);
            }
            gUnk_03002490->unk3C += 2;
            TaskYieldTrampoline(1);
        }
        break;
    case 1:
        t->unk0C = (u32)sub_08005d9c;
        t->unk38 = gUnk_08751CA4;
        t->unk42 = 5;
        {
            struct Task *u = gUnk_03002490;
            u->unk43 = ((struct Task *)u->unk8C)->unk43;
        }
        {
            struct Task *u = gUnk_03002490;
            u->unk40 = (((struct Task *)u->unk8C)->unk40 + 0x1800) | 12;
        }
        do
        {
            {
                struct Task *u = gUnk_03002490;
                if (u->unk43 == 1)
                {
                    u->unk4C = (((struct Task *)u->unk8C)->unk48 + 24) << 16;
                    u->unk50 = ((struct Task *)u->unk8C)->unk4A << 16;
                }
                else
                {
                    u->unk4C = (((struct Task *)u->unk8C)->unk48 - 24) << 16;
                    u->unk50 = ((struct Task *)u->unk8C)->unk4A << 16;
                }
            }
            sub_080061c0(0x10000, (sub_08002ee8(32) + 16) << 8);
            gUnk_03002490->unk58 = 0;
            gUnk_03002490->unk60 = -(sub_08002ee8(32) << 8);
            gUnk_03002490->unk3C = 0;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(3);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(2);
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(1);
        } while (gUnk_03002490->unk28 == 0);
        break;
    }
    TaskDispatchTrampoline();
}

void sub_08051b0c(void)
{
    {
        struct Task *t = gUnk_03002490;
        if (!(t->unk88->unk40 & 0x100) && (((struct Task *)t->unk8C)->unk7B & 1))
        {
            sub_08005654(gCurTaskIdx);
            return;
        }
    }
    {
        struct Task *t = gUnk_03002490;
        if (t->unk88->unk0D != 1)
        {
            sub_08005654(gCurTaskIdx);
            return;
        }
        if ((t->unk18 & 15) == 1)
        {
            s32 dx;
            t->unk78 = 127;
            sub_0801a828(gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873BDD4);
            sub_08030848((struct HitBoxSet *)gUnk_0873CBDC, gUnk_03002490->unk44);
            t = gUnk_03002490;
            dx = (t->unk43 == 1) ? 12 : -12;
            /* the ROM passes no 4th argument: r3 is whatever the last ldrsh left (0) */
            sub_08030804((struct HitBoxSet *)gUnk_0873CBDC, ((struct Task *)t->unk8C)->unk48 + dx,
                         ((struct Task *)t->unk8C)->unk4A + 2);
        }
    }
    {
        struct Task *t = gUnk_03002490;
        if (t->unk28 == 0)
        {
            if (t->unk88->unk04 != 13 || t->unk43 != ((struct Task *)t->unk8C)->unk43)
                t->unk28 = 1;
        }
    }
}

void sub_08051c1c(void)
{
    {
        struct Task *t = gUnk_03002490;
        if (t->unk38 == NULL)
        {
            t->unk00 = (u32)sub_080059d8;
            t->unk0C = (u32)sub_08005ea8;
            t->unk04 = (u32)sub_08051d84;
            t->unk42 = 5;
            {
                struct Task *u = gUnk_03002490;
                u->unk38 = gUnk_08751A98;
                u->unk40 = (((struct Task *)u->unk8C)->unk40 + 0x1800) | 8;
            }
            sub_0802205c(gUnk_0873CB54);
            gUnk_03002490->unk80 = 3;
            {
                struct Task *u = gUnk_03002490;
                u->unk28 = 0;
                u->unk2C = 0;
            }
        }
    }
    {
        struct Task *t = gUnk_03002490;
        switch (t->unk28)
        {
        case 0:
            if (t->unk43 == 1)
                t->unk4C = (t->unk48 + 8) << 16;
            else
                t->unk4C = (t->unk48 - 8) << 16;
            sub_080061c0(0x68000, -0x5000);
            gUnk_03002490->unk64 = 0x68000;
            while (1)
            {
                sub_0800617c(0);
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C += 2;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C += 2;
                TaskYieldTrampoline(2);
                gUnk_03002490->unk3C += 2;
                TaskYieldTrampoline(2);
            }
        case 1:
            gUnk_03002490->unk2C = 20;
            sub_08006214();
            sub_080061c0(-0x15500, 0x5A5A5A5A);
            {
                struct Task *u = gUnk_03002490;
                u->unk58 = -0x20000;
                u->unk60 = 0x4000;
            }
            while (1)
            {
                sub_0800617c(8);
                TaskYieldTrampoline(2);
                for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C <= 6; gUnk_03002490->unk6C++)
                {
                    gUnk_03002490->unk3C += 2;
                    TaskYieldTrampoline(2);
                }
            }
        }
    }
    TaskDispatchTrampoline();
}

void sub_08051d84(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk88->unk0D != 3)
    {
        sub_08006148(sub_08050814, gCurTaskIdx);
        return;
    }
    switch (t->unk28)
    {
    case 0:
        if (sub_08030848((struct HitBoxSet *)gUnk_0873CBEC, t->unk44))
            gUnk_03002490->unk7C = 1;
        if (gUnk_03002490->unk7C != 0)
        {
            sub_08006148(sub_08050814, gCurTaskIdx);
            return;
        }
        sub_0801c230(gUnk_0873CB54);
        if ((*(u32 *)&gUnk_03005550 & 0xFFFFFF) != 0)
        {
            {
                struct Task *u = gUnk_03002490;
                if (u->unk54 > 0)
                    u->unk43 = 1;
                else
                    u->unk43 = -1;
            }
            gUnk_03002490->unk28 = 1;
            sub_08006148(sub_08051c1c, gCurTaskIdx);
            break;
        }
        {
            struct Task *u = gUnk_03002490;
            if (u->unk2C == 0)
            {
                s8 f = u->unk43;
                if ((f == 1 && u->unk54 < 0) || (f == -1 && u->unk54 > 0))
                {
                    struct Task *v = gUnk_03002490;
                    v->unk0C = (u32)sub_08005f30;
                    v->unk2C++;
                }
                if (gUnk_03002490->unk2C == 0)
                    break;
            }
        }
        {
            struct Task *u = gUnk_03002490;
            struct Task *p = (struct Task *)u->unk8C;
            if (p->unk48 + 8 > u->unk48 && u->unk48 > p->unk48 - 8
                && p->unk4A + 8 > u->unk4A && u->unk4A > p->unk4A - 8)
            {
                sub_08005654(gCurTaskIdx);
                return;
            }
        }
        break;
    case 1:
        if (t->unk7C != 0)
        {
            sub_08006148(sub_08050814, gCurTaskIdx);
            return;
        }
        if (--t->unk2C == 0)
        {
            sub_08005654(gCurTaskIdx);
            return;
        }
        if (t->unk2C <= 7)
        {
            if (gUnk_03001EA4 & 1)
                t->unk38 = gUnk_08751A98;
            else
                t->unk38 = NULL;
        }
        break;
    }
    sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A, gUnk_0873BDE8);
}

void sub_08051f4c(void)
{
    u16 *e;

    if (gUnk_03002490->unk38 == NULL)
    {
        {
            struct Task *t = gUnk_03002490;
            t->unk00 = (u32)sub_080059d8;
            t->unk0C = (u32)sub_08005f30;
            t->unk42 = 7;
        }
        {
            struct Task *t = gUnk_03002490;
            t->unk38 = gUnk_08751AF8;
            t->unk40 = (((struct Task *)t->unk8C)->unk40 + 0x800) | 12;
            if (t->unk43 == 1)
            {
                t->unk4C = (((struct Task *)t->unk8C)->unk48 + 6) << 16;
                t->unk28 = 1;
            }
            else
            {
                t->unk4C = (((struct Task *)t->unk8C)->unk48 - 6) << 16;
                t->unk28 = 3;
            }
        }
        {
            struct Task *t = gUnk_03002490;
            t->unk50 = (((struct Task *)t->unk8C)->unk4A + 2) << 16;
            t->unk2C = 3;
        }
        sub_080224b0();
        sub_0802233c(gUnk_0873CB5C);
        gUnk_03002490->unk80 = 6;
    }
    else
    {
        gUnk_03002490->unk04 = 0;
        sub_080062c4();
        gUnk_03002490->unk3C = 16;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = 0xFFFF;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C = 17;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk3C = -1;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk7A = 0;
    }
    {
        struct Task *t = gUnk_03002490;
        s32 v;
        e = gUnk_0873B7B0[t->unk28];
        if (t->unk28 == 1 || t->unk28 == 3)
        {
            v = e[1] << 8;
            if (e[1] & 0x8000)
                v |= 0xFF000000;
            t->unk54 = v;
        }
        else
        {
            v = e[1] << 8;
            if (e[1] & 0x8000)
                v |= 0xFF000000;
            t->unk58 = v;
        }
    }
    gUnk_03002490->unk04 = (u32)sub_080520dc;
    while (1)
    {
        gUnk_03002490->unk3C = e[0];
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
        gUnk_03002490->unk3C += 2;
        TaskYieldTrampoline(1);
    }
}

void sub_080520dc(void)
{
    s32 hit;

    if (gUnk_03002490->unk88->unk0D == 0)
        goto rebind;
    hit = 0;
    gUnk_03005550.unk2 = 0;
    switch (gUnk_03002490->unk28)
    {
    case 1:
    case 3:
        if (sub_08030848((struct HitBoxSet *)gUnk_0873CBFC, gUnk_03002490->unk44))
        {
            gUnk_03002490->unk7C = 1;
            hit = 1;
        }
        else
            sub_0801c444(gUnk_0873CB5C);
        break;
    case 0:
    case 2:
        if (sub_08030848((struct HitBoxSet *)gUnk_0873CC0C, gUnk_03002490->unk44))
        {
            gUnk_03002490->unk7C = 1;
            hit = 1;
        }
        else
            sub_0801c444(gUnk_0873CB64);
        break;
    }
    if (gUnk_03005550.unk4 != 0)
    {
        struct Task *t = gUnk_03002490;
        if (t->unk2C-- == 0)
            t->unk7C = 1;
        else if (t->unk88->unk0D == 6)
            sub_08006148(sub_08051f4c, gCurTaskIdx);
        else
            sub_08006148(sub_08052f6c, gCurTaskIdx);
        hit = 1;
    }
    else if (*(u32 *)&gUnk_03005550 & 0xFFFFFF)
    {
        gUnk_03002490->unk7C = 1;
        hit = 1;
    }
    if (hit != 0)
    {
        {
            struct Task *t = gUnk_03002490;
            if (t->unk88->unk0D == 6)
            {
                if (!(t->unk88->unk42 & 128))
                    sub_0803e34c(173, t->unk44);
            }
            else
            {
                if (!(t->unk88->unk42 & 128))
                    sub_0803e34c(211, t->unk44);
            }
        }
        switch (gUnk_03002490->unk28)
        {
        case 1:
        case 3:
            gUnk_03002490->unk24 = (s32)gUnk_0873BDFC;
            break;
        case 0:
        case 2:
            gUnk_03002490->unk24 = (s32)gUnk_0873BE10;
            break;
        }
    }
    if (gUnk_03002490->unk7C != 0)
    {
    rebind:
        sub_08006148(sub_0805091c, gCurTaskIdx);
        return;
    }
    switch (gUnk_03005550.unk4)
    {
    case 0:
        switch (gUnk_03002490->unk28)
        {
        case 1:
        case 3:
            sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A, gUnk_0873BDFC);
            break;
        case 0:
        case 2:
            sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A, gUnk_0873BE10);
            break;
        }
        break;
    case 1:
    case 3:
        if (gUnk_03002490->unk28 == 1)
            gUnk_03002490->unk28 = 0;
        else if (gUnk_03002490->unk28 == 2)
            gUnk_03002490->unk28 = 3;
        break;
    case 2:
    case 4:
        if (gUnk_03002490->unk28 == 3)
            gUnk_03002490->unk28 = 0;
        else if (gUnk_03002490->unk28 == 2)
            gUnk_03002490->unk28 = 1;
        break;
    case 6:
    case 8:
        if (gUnk_03002490->unk28 == 3)
            gUnk_03002490->unk28 = 2;
        else if (gUnk_03002490->unk28 == 0)
            gUnk_03002490->unk28 = 1;
        break;
    case 5:
    case 7:
        if (gUnk_03002490->unk28 == 1)
            gUnk_03002490->unk28 = 2;
        else if (gUnk_03002490->unk28 == 0)
            gUnk_03002490->unk28 = 3;
        break;
    }
}
