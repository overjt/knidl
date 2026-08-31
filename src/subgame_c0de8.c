/* game_code_and_rodata_080653ec_0806ef5c 0x080C0DE8-0x080C1FFC
 * (issue #66, module M36 batch 3).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080C0DE8 0x080C1FFC src/subgame_c0de8.c --newpb
 *
 * Tail of M36: the sub-game's presentation layer.
 *
 *   sub_080c0de8 .. sub_080c173c   fourteen near-identical class-4 sprite
 *       bodies, one per on-screen element; each sets Task.unk15/unk38 from
 *       its own gUnk_08755Exx animation script and walks a fixed 16.16
 *       position list with TaskYieldTrampoline.
 *   sub_080c17b0 / sub_080c183c    the two dispatch bodies (switch on
 *       Task.unk1C / Task.unk20).
 *   sub_080c18c8 / sub_080c1950    slot placement: Task.unk4C/unk50 from
 *       gUnk_08757250/gUnk_08757260, and the per-slot horizontal offset
 *       switch over gUnk_0200B044[Task.unk20].
 *   sub_080c1b30 / sub_080c1b78    the results task.
 *   sub_080c1be8 / sub_080c1cec / sub_080c1d84   the three ranking markers.
 *   sub_080c1ebc   walks the 4-direction path script at gUnk_08757280 in
 *       6.0 steps, writing gUnk_03000B78/gUnk_03000FA8 (terminator 128).
 *   sub_080c1f88 / sub_080c1f9c / sub_080c1fdc   the score-record reset:
 *       four 60-byte records at gUnk_0201B0E0 + 0x18, the player count into
 *       gUnk_02016C40, and the final dispatcher hand-off.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern s8 gUnk_02006168;
extern u8 gUnk_020061DC;
extern s8 gUnk_02006A10[];
extern u8 gUnk_0200AF10;
extern u8 gUnk_0200AFF0;
extern u8 gUnk_0200B044[];
extern u16 gUnk_03001E90;
extern u8 gUnk_03001470[];
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000FA8;
extern vu16 gUnk_03001EB8[];
extern u16 gUnk_03002150;
extern u16 gUnk_03002360;
extern u16 gUnk_030023AC;

extern s8 gUnk_08756560[];
extern s8 gUnk_08756564[];
extern u32 gUnk_08755E00[];
extern u32 gUnk_08755E0C[];
extern u32 gUnk_08755E44[];
extern u32 gUnk_08755E7C[];
extern vu16 gUnk_03001270[];
extern vu16 gUnk_03001ED8;
extern s32 gUnk_08756540[];
extern s32 gUnk_08756550[];
extern s16 gUnk_08756798[];
extern s32 gUnk_087567A8[][3];
extern s32 *gUnk_08756D3C[][2];
extern u32 gUnk_08755EB4;
extern u32 gUnk_08755EFC[];
extern u32 gUnk_08755F1C[];
extern u32 gUnk_08755F2C[];
extern u32 gUnk_08755F3C[];
extern u32 gUnk_08757238[];
extern u32 gUnk_08757244[];
extern s32 gUnk_08757250[];
extern s32 gUnk_08757260[];
extern u32 gUnk_08757270[];
extern u32 gUnk_08757278[];
extern u8 gUnk_08757280[];
extern u32 gUnk_087572CC[];
extern u8 gUnk_02016C40[];
extern s32 gUnk_0201B0E0[];
extern u8 gUnk_02007D2C;
extern u32 gUnk_08755EB8[];
extern u32 gUnk_08755EC4[];
extern u32 gUnk_087571F8[];
extern s16 gUnk_08757014[];
extern s32 *gUnk_0875716C[];
extern s32 gUnk_08757178[][4];
extern s32 gUnk_087571B8[][4];
extern s32 gUnk_08756D74[];
extern s32 gUnk_08756DC8[];
extern s32 gUnk_08756E1C[];
extern s32 gUnk_08756E38[];
extern s32 gUnk_08756E54[][4];
extern s32 gUnk_08756EC4[][4];
extern s32 gUnk_08756F34[][4];
extern s32 gUnk_08756FA4[][4];
extern s16 gUnk_08756770[];
extern u16 gUnk_08756778[];
extern s8 gUnk_0875673C[];
extern s16 gUnk_087567A0[];
extern u8 gUnk_0875676C[];
extern u32 gUnk_08756780[];
extern u32 gUnk_0875678C[];
extern u32 *gUnk_0875671C[];
extern u32 gUnk_0875674C[];
extern u32 gUnk_0875675C[];
extern u32 gUnk_08756528[];
extern u8 gUnk_08756570[];
extern u8 gUnk_0875665C[];
extern u8 gUnk_0875665F[];
extern u8 gUnk_08756662[];
extern u32 gUnk_08756668[];
extern u32 gUnk_08756670[];
extern u32 gUnk_08756678[];
extern u32 gUnk_08756680[];
extern u32 gUnk_08756688[];
extern u32 gUnk_08755DC0;
extern u16 gUnk_08756538[];
extern u8 gUnk_087565E0[];
extern u8 *gUnk_08756650[];
extern u8 *gUnk_087565F4[];
extern u32 *gUnk_0875670C[];
extern s16 gUnk_0875672C[];
extern s16 gUnk_08756734[];
extern s8 gUnk_08756740[];
extern s8 gUnk_08756744[];
extern s8 gUnk_08756748[];
extern u32 gUnk_087566A4[];
extern u32 gUnk_087566D8[];

extern void TaskYieldTrampoline(u32 frames);

extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern void sub_0800214c(void);
extern void sub_080021dc(void);
extern s32 sub_080058e4(u32 type, s32 idx);
extern void sub_08006148(void *a, u32 i);
extern void sub_08005a90(void);
extern void sub_08005ca0(void);
extern void sub_080059d8(void);
extern void TaskDispatchTrampoline(void);
extern void sub_080bdf9c(u32 a);
extern void sub_080c0ca4(void);
extern void sub_080c17ac(void);
extern void sub_080c1804(void);
extern void sub_080c1b78(void);
extern void sub_08005654(u32 a);
extern void sub_08006338(s32 a);
extern void sub_080031b8(u32 a);
extern void sub_080060c0(void);
extern void sub_08009eb8(u32 a, u32 b);
extern void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
extern s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern s32 sub_08001cc8(s32 a, s16 b, s16 c, s32 d);
extern void sub_080062c4(void);
extern u32 sub_08002ee8(u32 range);
extern void sub_08003110(u32 a);
extern void sub_080034d0(void);
extern void sub_08006138(void);
extern void sub_080b9d0c(s32 a);
extern void sub_080b9d24(void);
extern void sub_080b9e30(void);

extern void sub_080bda78(void);
extern void sub_080bda98(void);
extern u32 sub_080bdd28(void);
extern u32 sub_080bdd70(void);
extern void sub_080bddb8(void);
extern void sub_080bde0c(void);
extern void sub_080bde78(u32 a);
extern void sub_080be04c(void);
extern void sub_080be1b0(void);
extern void sub_080be1d0(void);
extern void sub_080be4a4(void);
extern void sub_080be550(void);
extern void sub_080be5fc(void);
extern void sub_080be6b4(u32 a);
extern void sub_080be714(u32 a);
extern void sub_080be774(void);
extern void sub_080be850(void);
extern u32 sub_080bf0ac(void);
extern void sub_080bf154(void);
extern void sub_080be8a8(void);
extern void sub_080bfdb0(void);
extern void sub_080bfdcc(void);
extern void sub_080be010(void);
extern void sub_08006244(void);
extern s32 Div(s32 numerator, s32 denominator);
extern void sub_080c05f0(u32 a);
extern void sub_080c0704(u32 a);
extern void sub_080c0b18(u32 a);
extern void sub_080061c0(s32 a, s32 b);
extern s32 sub_080c1ebc(s32 a, s32 b);
extern void sub_080c061c(s32 a, s32 b, s32 c, s32 d);
extern void sub_080c072c(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
extern void sub_080c0a10(s32 a, s32 b, s32 c, s32 d, s16 e);
/* NOTE: sub_080bdebc's third parameter is `u16` at its definition
   (src/subgame_bda2c.c) but the ROM's call sites here sign-extend the
   argument, so the declaration visible here is the wider `s32` - the
   original source had the same prototype mismatch. */
extern void sub_080bdebc(s32 a, s32 b, s32 c, s32 d);
extern void sub_080bdf3c(s32 a, s32 b, u32 c, u32 d);
extern void sub_080be7c0(u32 a);



void sub_080c0de8(void)
{
    struct Task *t;

    sub_080061c0(0x80 << 10, 0xFFFFF400);
    t = gUnk_03002490;
    t->unk58 = 0xFFFF1000;
    t->unk60 = 0xA0 << 3;
    sub_08006338(0);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 6);
    sub_080062c4();
    sub_08006338(4);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    TaskDispatchTrampoline();
}

void sub_080c0e88(void)
{
    struct Task *t;

    sub_080061c0(0x80 << 10, 0xFFFFF400);
    t = gUnk_03002490;
    t->unk58 = 0xF0 << 8;
    t->unk60 = 0xFFFFFB00;
    sub_08006338(4);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    sub_08006338(0);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    sub_080062c4();
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    TaskDispatchTrampoline();
}

void sub_080c0f54(void)
{
    struct Task *t;

    sub_080061c0(0xFFFE0000, 0xC0 << 4);
    t = gUnk_03002490;
    t->unk58 = 0xF0 << 8;
    t->unk60 = 0xFFFFFB00;
    sub_08006338(0);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 5);
    sub_080062c4();
    sub_08006338(7);
    TaskYieldTrampoline(2);
    sub_08006338(4);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

void sub_080c0fe4(void)
{
    struct Task *t;

    sub_080061c0(0xFFFE0000, 0xC0 << 4);
    t = gUnk_03002490;
    t->unk58 = 0xFFFF1000;
    t->unk60 = 0xA0 << 3;
    sub_08006338(0);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 6);
    sub_080062c4();
    sub_08006338(4);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

void sub_080c1068(void)
{
    struct Task *t;

    sub_080061c0(0xFFFF1000, 0xA0 << 3);
    t = gUnk_03002490;
    t->unk58 = 0xFFFE0000;
    t->unk60 = 0xC0 << 4;
    sub_08006338(0);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    sub_08006338(1);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_080062c4();
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    TaskDispatchTrampoline();
}

void sub_080c1114(void)
{
    struct Task *t;

    sub_080061c0(0x80 << 10, 0xFFFFF400);
    t = gUnk_03002490;
    t->unk58 = 0xFFFF1000;
    t->unk60 = 0xA0 << 3;
    sub_08006338(4);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    sub_08006338(0);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_080062c4();
    sub_08006338(6);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    TaskDispatchTrampoline();
}

void sub_080c11cc(void)
{
    struct Task *t;

    sub_080061c0(0x80 << 10, 0xFFFFF400);
    t = gUnk_03002490;
    t->unk58 = 0xF0 << 8;
    t->unk60 = 0xFFFFFB00;
    sub_08006338(4);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 4);
    sub_080062c4();
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    TaskDispatchTrampoline();
}

void sub_080c1260(void)
{
    struct Task *t;

    sub_080061c0(0xFFFE0000, 0xC0 << 4);
    t = gUnk_03002490;
    t->unk58 = 0xF0 << 8;
    t->unk60 = 0xFFFFFB00;
    sub_08006338(8);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    sub_08006338(4);
    TaskYieldTrampoline(2);
    sub_080062c4();
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    TaskDispatchTrampoline();
}

void sub_080c1300(void)
{
    struct Task *t;

    sub_080061c0(0xFFFE0000, 0xC0 << 4);
    t = gUnk_03002490;
    t->unk58 = 0xFFFF1000;
    t->unk60 = 0xA0 << 3;
    sub_08006338(4);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    sub_080062c4();
    sub_08006338(0);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_08006338(6);
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

void sub_080c1390(void)
{
    struct Task *t;

    sub_080061c0(0xFFFF1000, 0xA0 << 3);
    t = gUnk_03002490;
    t->unk58 = 0xFFFE0000;
    t->unk60 = 0xC0 << 4;
    sub_08006338(4);
    TaskYieldTrampoline(3);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    sub_080062c4();
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    TaskDispatchTrampoline();
}

void sub_080c1424(void)
{
    struct Task *t;

    sub_080061c0(0x80 << 10, 0xFFFFF400);
    t = gUnk_03002490;
    t->unk58 = 0xFFFF1000;
    t->unk60 = 0xA0 << 3;
    sub_08006338(4);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 3);
    sub_080062c4();
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    TaskDispatchTrampoline();
}

void sub_080c14b8(void)
{
    struct Task *t;

    sub_080061c0(0x80 << 10, 0xFFFFF400);
    t = gUnk_03002490;
    t->unk58 = 0xF0 << 8;
    t->unk60 = 0xFFFFFB00;
    sub_08006338(4);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    sub_080062c4();
    sub_08006338(4);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    TaskDispatchTrampoline();
}

void sub_080c1558(void)
{
    struct Task *t;

    sub_080061c0(0xFFFE0000, 0xC0 << 4);
    t = gUnk_03002490;
    t->unk58 = 0xF0 << 8;
    t->unk60 = 0xFFFFFB00;
    sub_08006338(4);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_08006338(8);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    sub_080062c4();
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    TaskDispatchTrampoline();
}

void sub_080c1608(void)
{
    struct Task *t;

    sub_080061c0(0xFFFE0000, 0xC0 << 4);
    t = gUnk_03002490;
    t->unk58 = 0xFFFF1000;
    t->unk60 = 0xA0 << 3;
    sub_08006338(4);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_08006338(10);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    sub_080062c4();
    sub_08006338(4);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

void sub_080c168c(void)
{
    struct Task *t;

    sub_080061c0(0xFFFF1000, 0xA0 << 3);
    t = gUnk_03002490;
    t->unk58 = 0xFFFE0000;
    t->unk60 = 0xC0 << 4;
    sub_08006338(4);
    TaskYieldTrampoline(2);
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 2);
    sub_080062c4();
    gUnk_03002490->unk6C = 0;
    do {
        gUnk_03002490->unk3C++;
        TaskYieldTrampoline(2);
        gUnk_03002490->unk6C++;
    } while ((s16)gUnk_03002490->unk6C <= 1);
    sub_08006338(4);
    TaskYieldTrampoline(12);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    TaskDispatchTrampoline();
}

void sub_080c173c(void)
{
    struct Task *t;
    struct Task *u;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08005ca0;
    t->unk04 = (u32)sub_080c17ac;
    t->unk42 = 6;
    u = gUnk_03002490;
    u->unk38 = gUnk_08755EFC;
    u->unk4C = 0xF0 << 15;
    u->unk50 = 0xE0 << 15;
    u->unk3C = 0;
    TaskYieldTrampoline(46);
    gUnk_03002490->unk3C = 0xFFFF;
    TaskYieldTrampoline(16);
    gUnk_03002490->unk3C = 1;
    TaskYieldTrampoline(30);
    TaskDispatchTrampoline();
}

void sub_080c17ac(void)
{
}

void sub_080c17b0(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08005ca0;
    t->unk04 = (u32)sub_080c1804;
    switch (t->unk1C) {
    case 0:
        t->unk14 = t->unk1C;
        break;
    case 1:
        t->unk14 = t->unk1C;
        break;
    case 2:
        t->unk14 = t->unk1C;
        break;
    }
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08757238);
    sub_08006138();
}

void sub_080c1804(void)
{
    sub_08002e98(gUnk_03002490->unk15, 3, gUnk_08757244);
}

void sub_080c1820(void)
{
    sub_08002e98(gUnk_03002490->unk14, 3, gUnk_08757238);
}

void sub_080c183c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk15 = 0;
    gUnk_03002490->unk42 = 9;
    u = gUnk_03002490;
    u->unk38 = gUnk_08755F1C;
    u->unk40 = (u->unk18 << 12) | (0x80 << 4);
    if (gUnk_030023AC == 1) {
        u->unk4C = 0xF0 << 15;
        u->unk50 = 0xC0 << 15;
    } else {
        u->unk4C = gUnk_08757250[u->unk18] << 16;
        u->unk50 = gUnk_08757260[u->unk18] << 16;
    }
    v = gUnk_03002490;
    v->unk3C = gUnk_0200B044[v->unk20];
    sub_08006138();
}

void sub_080c18c4(void)
{
}

void sub_080c18c8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    s32 n;

    t = gUnk_03002490;
    t->unk15 = 2;
    u = gUnk_03002490;
    u->unk28 = 0;
    if (gUnk_030023AC == 1) {
        u->unk4C = 0xF0 << 15;
        u->unk50 = 0xC0 << 15;
    } else {
        u->unk4C = gUnk_08757250[u->unk18] << 16;
        u->unk50 = gUnk_08757260[u->unk18] << 16;
    }
    n = gUnk_0200B044[gUnk_03002490->unk20];
    switch (n) {
    case 3:
        gUnk_03002490->unk2C = -16;
        break;
    case 2:
        gUnk_03002490->unk2C = -8;
        break;
    case 0:
    case 1:
        gUnk_03002490->unk2C = 0;
        break;
    }
    sub_08006138();
}

void sub_080c1950(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *z;
    s32 r;

    switch (gUnk_0200B044[gUnk_03002490->unk20]) {
    case 3:
        t = gUnk_03002490;
        if (t->unk28 > 39) {
            if (t->unk28 == 40 && t->unk20 == gUnk_03002360)
                sub_080031b8(220);
            sub_08001a94(7, gUnk_08755EFC[6], 0, 0,
                         (s16)gUnk_03002490->unk48 + gUnk_03002490->unk2C + 32,
                         (s16)(gUnk_03002490->unk4A - 24));
        }
        /* fall through */
    case 2:
        v = gUnk_03002490;
        if (v->unk28 > 19) {
            if (v->unk28 == 20 && v->unk20 == gUnk_03002360)
                sub_080031b8(220);
            sub_08001a94(7, gUnk_08755EFC[6], 0, 0,
                         (s16)gUnk_03002490->unk48 + gUnk_03002490->unk2C + 16,
                         (s16)(gUnk_03002490->unk4A - 24));
        }
        /* fall through */
    case 1:
        v = gUnk_03002490;
        if (v->unk28 >= 0) {
            if (v->unk28 == 0 && v->unk20 == gUnk_03002360)
                sub_080031b8(220);
            sub_08001a94(7, gUnk_08755EFC[6], 0, 0,
                         (s16)gUnk_03002490->unk48 + gUnk_03002490->unk2C,
                         (s16)(gUnk_03002490->unk4A - 24));
        }
        break;
    case 0:
        t = gUnk_03002490;
        if (t->unk28 >= 0)
            sub_08001a94(7, gUnk_08755EFC[7], 0, 0,
                         (s16)t->unk48 + t->unk2C,
                         (s16)(t->unk4A - 24));
        break;
    }
    z = gUnk_03002490;
    if (z->unk28 <= 40)
        z->unk28++;
}

void sub_080c1ab8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gUnk_03002490;
    t->unk15 = 1;
    gUnk_03002490->unk42 = 7;
    u = gUnk_03002490;
    u->unk38 = gUnk_08755EFC;
    if (gUnk_030023AC == 1) {
        u->unk4C = 0xF0 << 15;
        u->unk50 = 0x90 << 15;
    } else {
        u->unk4C = gUnk_08757250[u->unk18] << 16;
        u->unk50 = (gUnk_08757260[u->unk18] - 24) << 16;
    }
    v = gUnk_03002490;
    v->unk3C = gUnk_0200B044[v->unk20] + 2;
    sub_08006138();
}

void sub_080c1b2c(void)
{
}

void sub_080c1b30(void)
{
    struct Task *t;
    s32 n;

    t = gUnk_03002490;
    t->unk00 = (u32)sub_080059d8;
    t->unk0C = (u32)sub_08005ca0;
    t->unk04 = (u32)sub_080c1b78;
    n = t->unk74;
    if (n == 0)
        t->unk14 = n;
    else
        t->unk14 = 1;
    sub_08002e98(gUnk_03002490->unk14, 2, gUnk_08757270);
    sub_08006138();
}

void sub_080c1b78(void)
{
    sub_08002e98(gUnk_03002490->unk15, 2, gUnk_08757278);
}

void sub_080c1b94(void)
{
    struct Task *t;

    gUnk_03002490->unk15 = 0;
    t = gUnk_03002490;
    t->unk38 = gUnk_08755F2C;
    t->unk3C = t->unk18;
    t->unk28 = 0;
    if (t->unk18 == t->unk1C) {
        t->unk4C = 0xDC << 15;
        t->unk50 = 0xFE << 15;
        t->unk42 = 3;
    } else {
        t->unk4C = 0x82 << 16;
        t->unk50 = 0x89 << 16;
        t->unk42 = 4;
    }
    sub_08006138();
}

void sub_080c1be8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 n;

    t = gUnk_03002490;
    u = &gUnk_03002790[t->unk44];
    if (u->unk18 == 4)
        n = 0;
    else
        n = t->unk28 + 1;
    t->unk28 = n;
    v = gUnk_03002490;
    if (u->unk2C != v->unk18) {
        if ((s16)v->unk4A <= 136) {
            v->unk50 += 0x80 << 9;
            v->unk4A = v->unk50 >> 16;
            v->unk4C += 0x80 << 10;
            v->unk48 = v->unk4C >> 16;
            if (u->unk30 == 0)
                v->unk42 = 4;
            else
                v->unk42 = 3;
        } else {
            v->unk42 = 4;
        }
        w = gUnk_03002490;
        w->unk3C = w->unk18 << 1;
    } else {
        if ((s16)v->unk4A > 127) {
            v->unk50 += 0xFFFF0000;
            v->unk4A = v->unk50 >> 16;
            v->unk4C += 0xFFFE0000;
            v->unk48 = v->unk4C >> 16;
            v->unk3C = v->unk18 << 1;
            if (u->unk30 == 0)
                v->unk42 = 3;
            else
                v->unk42 = 4;
        } else {
            v->unk3C = (v->unk18 << 1) + ((v->unk28 >> 1) & 1);
            v->unk42 = 3;
        }
    }
    if (u->unk28 != 0)
        sub_08005654(gCurTaskIdx);
}

void sub_080c1cec(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;

    t = gUnk_03002490;
    t->unk15 = 1;
    u = gUnk_03002490;
    u->unk34 = 0;
    u->unk38 = gUnk_08755F3C;
    u->unk6C = 0;
    do {
        v = gUnk_03002490;
        if (v->unk18 == v->unk1C) {
            v->unk42 = (u8)v->unk6C + 3;
            w = gUnk_03002490;
            w->unk3C = w->unk18 << 1;
            w->unk4C = (((s16)w->unk6C * 5) << 18) + (0xC8 << 15);
            w->unk50 = (((s16)w->unk6C * 5) << 17) + (0xF4 << 15);
        }
        x = gUnk_03002490;
        x->unk1C++;
        if (x->unk1C == 3)
            x->unk1C = 0;
        y = gUnk_03002490;
        y->unk6C++;
    } while ((s16)y->unk6C <= 2);
    sub_08006138();
}

void sub_080c1d84(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    s32 n;

    t = gUnk_03002490;
    u = &gUnk_03002790[t->unk44];
    if (u->unk18 == 4)
        n = 0;
    else
        n = t->unk34 + 1;
    t->unk34 = n;
    v = gUnk_03002490;
    v->unk28 = u->unk2C;
    v->unk6C = 0;
    do {
        w = gUnk_03002490;
        if (w->unk18 == w->unk28) {
            w->unk2C = (((s16)w->unk6C * 5) << 1) + 122;
            w->unk30 = (s16)w->unk6C + 3;
        }
        x = gUnk_03002490;
        x->unk28++;
        if (x->unk28 == 3)
            x->unk28 = 0;
        y = gUnk_03002490;
        y->unk6C++;
    } while ((s16)y->unk6C <= 2);
    z = gUnk_03002490;
    if ((s16)z->unk4A > z->unk2C) {
        z->unk50 += 0xFFFE0000;
        z->unk4A = z->unk50 >> 16;
        z->unk4C += 0xFFFC0000;
        z->unk48 = z->unk4C >> 16;
        z->unk3C = z->unk18 << 1;
        if (u->unk30 == 0)
            z->unk42 = z->unk30;
    } else if ((s16)z->unk4A < z->unk2C) {
        z->unk50 += 0x80 << 10;
        z->unk4A = z->unk50 >> 16;
        z->unk4C += 0x80 << 11;
        z->unk48 = z->unk4C >> 16;
        z->unk3C = z->unk18 << 1;
        if (u->unk30 == 0)
            z->unk42 = z->unk30;
    } else {
        if (z->unk18 == u->unk2C)
            z->unk3C = (z->unk18 << 1) + ((z->unk34 >> 1) & 1);
        gUnk_03002490->unk42 = gUnk_03002490->unk30;
    }
    if (u->unk28 != 1)
        sub_08005654(gCurTaskIdx);
}

s32 sub_080c1ebc(s32 a, s32 b)
{
    u16 x;
    u16 y;
    u8 *p;
    u8 *base;

    base = gUnk_08757280;
    p = base + b * 2;
    if (p[0] == 128) {
        gUnk_03000B78 = gUnk_08756540[a];
        gUnk_03000FA8 = gUnk_08756550[a];
        return b;
    }
    x = (s8)p[0];
    y = (s8)p[1];
    switch (a) {
    case 0:
        y = ((u32)(y << 16) + 0xFFFA0000) >> 16;
        break;
    case 1:
        x = ((u32)(x << 16) + (0xC0 << 11)) >> 16;
        break;
    case 2:
        y = ((u32)(y << 16) + (0xC0 << 11)) >> 16;
        break;
    case 3:
        x = ((u32)(x << 16) + 0xFFFA0000) >> 16;
        break;
    }
    gUnk_03000B78 = gUnk_08756540[a] + (x << 16);
    gUnk_03000FA8 = gUnk_08756550[a] + (y << 16);
    return b + 1;
}
void sub_080c1f88(void)
{
    u8 *p;

    p = gUnk_02016C40;
    *(u16 *)(p + 772) = 0;
}

void sub_080c1f9c(void)
{
    s32 i;
    s32 zero;
    s32 j;
    u8 *p;
    s8 *q;
    s32 *r;

    p = gUnk_02016C40;
    q = &gUnk_02006168;
    r = gUnk_0201B0E0;
    zero = 0;
    j = 51;
    for (i = 3; i >= 0; i--) {
        r[j] = zero;
        j -= 15;
    }
    *(s32 *)p = *q;
    p[1104] = 0;
    p[1105] = 0;
}

void sub_080c1fdc(void)
{
    sub_08002e98(gUnk_02007D2C, 2, gUnk_087572CC);
    sub_08006138();
}

