/* game_code_early 0x08005D9C-0x08006464 (issue #32, batch G1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08005D9C 0x08006464 pending/early_5d9c.c --newpb
 * `--newpb` is a NEW recipe flag; see the report / build/scratch/fn_early_5d9c/
 * fnmatch.patch.  Without -fprologue-bugfix agbcc caches
 * current_function_has_far_jump across the far-jump scan and forces a spurious
 * leaf `push {lr}`; old_agbcc -O2 reproduces the missing push but narrows
 * `(s8)mem == 1` down to a bare `ldrb`+`cmp`, losing the ROM's
 * `lsls #24 / asrs #24`.  Only this recipe gets both right.
 *
 * The range holds 26 functions; symbols.csv lists 23 (sub_08005e1c,
 * sub_080063f0 and sub_0800641c are unreferenced dead exports).
 */
#include "gba/gba.h"
#include "global.h"

/* Sprite / draw-request record.  gUnk_03002490 points at the entry that is
 * currently being filled in; gUnk_03002790[] is the array those entries live
 * in (stride 0x90, indexed by the same slot number used by sub_08006148).
 *
 * Evidence for the field types is in the ROM itself:
 *   unk38  ldr  [p,#0x38] + ldr [base + idx*4]   -> array of pointers
 *   unk3C  ldrsh, compared against -1            -> signed slot index
 *   unk43  ldrb + lsls #24 + asrs #24            -> s8 (see the recipe note)
 *   unk48  ldrsh / ldrh                          -> s16 world X
 *   unk4A  ldrsh / ldrh                          -> s16 world Y
 *   unk54/58/5C/60/64/68 are s32 pairs that are cleared to 0 / 0x80000000.
 */
struct Sprite
{
    /*0x00*/ u8 filler00[0x10];
    /*0x10*/ u16 unk10;
    /*0x12*/ u8 filler12[0x38 - 0x12];
    /*0x38*/ void **unk38;
    /*0x3C*/ s16 unk3C;
    /*0x3E*/ u16 unk3E;
    /*0x40*/ u16 unk40;
    /*0x42*/ u8 unk42;
    /*0x43*/ s8 unk43;
    /*0x44*/ u8 filler44[4];
    /*0x48*/ s16 unk48;
    /*0x4A*/ s16 unk4A;
    /*0x4C*/ s32 unk4C;
    /*0x50*/ u8 filler50[4];
    /*0x54*/ s32 unk54;
    /*0x58*/ s32 unk58;
    /*0x5C*/ s32 unk5C;
    /*0x60*/ s32 unk60;
    /*0x64*/ s32 unk64;
    /*0x68*/ s32 unk68;
    /*0x6C*/ u8 filler6C[0x90 - 0x6C];
};

extern struct Sprite *gUnk_03002490;
extern struct Sprite gUnk_03002790[];
extern void *gTaskFlagsTable[];
extern u32 gUnk_03004B90[];
/* Camera scroll origin: subtracted from the world coordinates to get the
 * screen coordinates handed to sub_08001a94. */
extern s16 gUnk_03002348;
extern s16 gUnk_030023E4;
extern u32 gCurTaskIdx;

extern s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern void sub_08005654(u32 a);
extern u8 sub_08005acc(void);
extern u32 sub_08005bc4(u32 a);
extern u8 sub_08005c4c(void);
extern void TaskYieldTrampoline(u32 a);

void sub_08005d9c(void)
{
    struct Sprite *p;
    struct Sprite *q;
    void **tbl;

    p = gUnk_03002490;
    if (p->unk38 == NULL) return;
    if (p->unk3C == -1) return;
    if (sub_08005acc() == 0) return;
    q = gUnk_03002490;
    tbl = q->unk38;
    sub_08001a94(q->unk42, (u32)tbl[q->unk3C], q->unk3E, q->unk40,
                 q->unk48 - gUnk_03002348, (s16)(q->unk4A - gUnk_030023E4));
}

void sub_08005e1c(void)
{
    struct Sprite *p;
    struct Sprite *q;
    void **tbl;

    p = gUnk_03002490;
    if (p->unk38 == NULL) return;
    if (p->unk3C == -1) return;
    if (sub_08005acc() != 0) {
        q = gUnk_03002490;
        tbl = q->unk38;
        sub_08001a94(q->unk42, (u32)tbl[q->unk3C], q->unk3E, q->unk40,
                     q->unk48 - gUnk_03002348,
                     (s16)(q->unk4A - gUnk_030023E4));
    } else {
        sub_08005654(gCurTaskIdx);
    }
}

void sub_08005ea8(void)
{
    struct Sprite *p;
    struct Sprite *q;
    void **tbl;

    p = gUnk_03002490;
    if (p->unk38 == NULL) return;
    if (p->unk3C == -1) return;
    if (sub_08005c4c() == 0) return;
    if (sub_08005acc() == 0) return;
    q = gUnk_03002490;
    tbl = q->unk38;
    sub_08001a94(q->unk42, (u32)tbl[q->unk3C], q->unk3E, q->unk40,
                 q->unk48 - gUnk_03002348,
                 (s16)(q->unk4A - gUnk_030023E4));
}

void sub_08005f30(void)
{
    struct Sprite *p;
    struct Sprite *q;
    void **tbl;

    p = gUnk_03002490;
    if (p->unk38 == NULL) return;
    if (p->unk3C == -1) return;
    if (sub_08005c4c() != 0) {
        if (sub_08005acc() == 0) return;
        q = gUnk_03002490;
        tbl = q->unk38;
        sub_08001a94(q->unk42, (u32)tbl[q->unk3C], q->unk3E, q->unk40,
                     q->unk48 - gUnk_03002348,
                     (s16)(q->unk4A - gUnk_030023E4));
    } else {
        sub_08005654(gCurTaskIdx);
    }
}

void sub_08005fc8(void)
{
    struct Sprite *p;
    struct Sprite *q;
    u32 v;

    p = gUnk_03002490;
    if (p->unk38 == NULL) return;
    if (p->unk3C == -1) return;
    if (sub_08005acc() == 0) return;
    v = sub_08005bc4(0);
    q = gUnk_03002490;
    sub_08001a94(q->unk42, v, q->unk3E, q->unk40,
                 q->unk48 - gUnk_03002348,
                 (s16)(q->unk4A - gUnk_030023E4));
}

void sub_08006040(void)
{
    struct Sprite *p;
    struct Sprite *q;
    void **tbl;
    u32 *r;

    p = gUnk_03002490;
    if (p->unk38 == NULL) return;
    if (p->unk3C == -1) return;
    if (sub_08005acc() == 0) return;
    q = gUnk_03002490;
    tbl = q->unk38;
    r = tbl[q->unk3C];
    sub_08001a94(q->unk42, *r, q->unk3E, q->unk40,
                 q->unk48 - gUnk_03002348,
                 (s16)(q->unk4A - gUnk_030023E4));
}

void sub_080060c0(void)
{
    struct Sprite *p;
    void **tbl;
    u16 t;

    p = gUnk_03002490;
    if (p->unk38 == NULL) return;
    if (p->unk3C == -1) return;
    t = p->unk48 + 31;
    if (t > 302) return;
    if (p->unk4A <= -32) return;
    if (p->unk4A > 191) return;
    tbl = p->unk38;
    sub_08001a94(p->unk42, (u32)tbl[p->unk3C], p->unk3E, p->unk40,
                 p->unk48, p->unk4A);
}

void sub_08006138(void)
{
    while (1)
        TaskYieldTrampoline(0x7FFF);
}

void sub_08006148(void *a, u32 i)
{
    gUnk_03002790[i].unk10 = 0;
    gTaskFlagsTable[i] = a;
    gUnk_03004B90[i] = 0x0203BFE0 + (i << 8);
}

void sub_0800617c(s16 a)
{
    struct Sprite *p;

    p = gUnk_03002490;
    if (p->unk43 == 1)
        p->unk3C = a;
    else
        p->unk3C = a | 1;
}

void sub_080061a8(s32 a, s32 b, s32 c)
{
    struct Sprite *p;

    p = gUnk_03002490;
    p->unk54 = a;
    p->unk5C = b;
    p->unk64 = abs(c);
}

void sub_080061c0(s32 a, s32 b)
{
    struct Sprite *p;

    p = gUnk_03002490;
    if (p->unk43 == 1) {
        if (a != 0x5A5A5A5A) p->unk54 = a;
        if (b != 0x5A5A5A5A) gUnk_03002490->unk5C = b;
    } else {
        if (a != 0x5A5A5A5A) p->unk54 = -a;
        if (b != 0x5A5A5A5A) gUnk_03002490->unk5C = -b;
    }
}

void sub_08006214(void)
{
    struct Sprite *p;

    p = gUnk_03002490;
    p->unk5C = 0;
    p->unk54 = 0;
    p->unk64 = 0x80000000;
}

void sub_0800622c(s32 a, s32 b, s32 c)
{
    struct Sprite *p;

    p = gUnk_03002490;
    p->unk58 = a;
    p->unk60 = b;
    p->unk68 = abs(c);
}

void sub_08006244(void)
{
    struct Sprite *p;

    p = gUnk_03002490;
    p->unk60 = 0;
    p->unk58 = 0;
    p->unk68 = 0x80000000;
}

void sub_0800625c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f)
{
    if (a != 0x5A5A5A5A) gUnk_03002490->unk54 = a;
    if (b != 0x5A5A5A5A) gUnk_03002490->unk5C = b;
    if (c != 0x5A5A5A5A) gUnk_03002490->unk64 = abs(c);
    if (d != 0x5A5A5A5A) gUnk_03002490->unk58 = d;
    if (e != 0x5A5A5A5A) gUnk_03002490->unk60 = e;
    if (f != 0x5A5A5A5A) gUnk_03002490->unk68 = abs(f);
}

void sub_080062c4(void)
{
    struct Sprite *p;

    p = gUnk_03002490;
    p->unk60 = 0;
    p->unk58 = 0;
    p->unk5C = 0;
    p->unk54 = 0;
    p->unk68 = 0x80000000;
    p->unk64 = 0x80000000;
}

void sub_080062e0(u32 i)
{
    struct Sprite *p;

    p = &gUnk_03002790[i];
    p->unk60 = 0;
    p->unk58 = 0;
    p->unk5C = 0;
    p->unk54 = 0;
    p->unk68 = 0x80000000;
    p->unk64 = 0x80000000;
}

void sub_08006304(void)
{
    struct Sprite *p;

    p = gUnk_03002490;
    if (p->unk43 == 1)
        p->unk3E &= 0x7FFF;
    else
        p->unk3E |= 0x8000;
}

void sub_08006338(s32 a)
{
    gUnk_03002490->unk3C = a;
    sub_08006304();
}

void sub_0800634c(s32 a)
{
    struct Sprite *p;

    p = gUnk_03002490;
    p->unk3E &= 0x7FFF;
    p->unk3C = a;
}

void sub_08006364(s32 a)
{
    struct Sprite *p;

    p = gUnk_03002490;
    p->unk3E |= 0x8000;
    p->unk3C = a;
}

void sub_08006384(u16 a)
{
    struct Sprite *p;

    p = gUnk_03002490;
    if (p->unk43 == 1)
        p->unk4C = a << 16;
    else
        p->unk4C = -(a << 16);
}

void sub_080063ac(s16 a)
{
    struct Sprite *p;

    p = gUnk_03002490;
    if (p->unk43 == 1)
        p->unk4C = (p->unk48 + a) << 16;
    else
        p->unk4C = (p->unk48 - a) << 16;
}

u8 sub_080063f0(s16 a, s16 b)
{
    u16 t;

    t = a + 63;
    if (t > 366) return 0;
    if (b <= -64) return 0;
    if (b > 223) return 0;
    return 1;
}

u8 sub_0800641c(s16 a, s16 b)
{
    s16 x;
    s16 y;
    u16 t;

    x = a - gUnk_03002348;
    y = b - gUnk_030023E4;
    t = x + 63;
    if (t > 366) return 0;
    if (y <= -64) return 0;
    if (y > 223) return 0;
    return 1;
}
