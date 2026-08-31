/* game_code_and_rodata 0x08091F08-0x08091F9C (issue #67, module M25 batch 2c).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x08091F08 0x08091F9C src/enemy_91f08.c --newpb
 *
 * sub_08091f08, the entry of M25's third boss (its states live in
 * src/enemy_91f9c.c): it installs sub_080656b4 / sub_080653ec as the draw and
 * per-frame hooks, points Task.unk38 at gUnk_087535FC, counts the boss into
 * gUnk_02007D00[7], seeds the state block (Task.unk28 = -1, Task.unk34 = 1,
 * Task.unk1C = -1, Task.unk24 = Actor.unk28) and dispatches Task.unk73
 * through the 27-entry anchor table at 0x08743ADC.
 *
 * It sits in its own file only because sub_08091e18, one function earlier in
 * the ROM, is still assembly; it belongs with src/enemy_91f9c.c.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* RAM cells */
extern u32 gUnk_02007D00[];
extern vs16 gUnk_03004CA0[];

/* ROM tables */
extern struct AnimCmd gUnk_0874397C[];
extern u32 gUnk_08743988[];
extern u32 gUnk_087439A4[];
extern u32 gUnk_087440F4[];
extern struct AnimCmd *gUnk_08743A00[];
extern u32 gUnk_08743ADC[];
extern u32 gUnk_087535FC[];
extern u32 gUnk_08753128[];
extern u32 gUnk_08753148[];
extern u8 gUnk_087438DC[];
extern u32 gUnk_087438E4[];
extern u32 gUnk_087438EC[];
extern u32 gUnk_0874391C[];
extern u32 gUnk_0874394C[];
extern struct GfxHeader gUnk_0827565C;
extern u32 gUnk_08275670;

/* Externals */
extern void TaskYieldTrampoline(u32 a);
extern void TaskDispatchTrampoline(void);
extern void sub_08064a60(void);
extern u16 sub_08066630(u16 a);
extern void sub_080651b4(void);
extern void sub_080653ec(void);
extern void sub_080656b4(void);
extern void sub_08002e98(u32 a, u32 b, u32 *c);
extern u32 sub_08002ee8(u32 range);
extern void sub_080031b8(s32 id);
extern void sub_08005654(s32 id);
extern void sub_08006138(void);
extern void sub_08006148(void *fn, s32 i);
extern void sub_080061a8(s32 a, s32 b, s32 c);
extern void sub_080061c0(s32 a, s32 b);
extern void sub_0800622c(s32 a, s32 b, s32 c);
extern void sub_08006214(void);
extern void sub_08006244(void);
extern void sub_080062c4(void);
extern void sub_08006304(void);
extern void sub_08006338(s32 a);
extern void sub_080261d4(s32 a);
extern void sub_0806395c(u16 v);
extern void sub_080639a4(u32 *p);
extern void sub_080639b4(u32 v);
extern void sub_080639f0(u32 v);
extern void sub_08063a00(u32 v);
extern s32 sub_08063cd0(void);
extern s32 sub_08063d2c(void);
extern void sub_08063e14(void);
extern s32 sub_080640dc(struct AnimCmd *p);
extern s32 sub_080640fc(void);
extern s32 sub_08064b5c(struct ActorSpawn *p, u8 keepPrio);
extern s32 sub_08064c1c(u32 type, int xArg, int yArg, int prioArg);
extern s32 sub_08064d34(u32 type, u8 keepPrio);
extern u16 sub_08066088(u32 mode);
extern void sub_080663f4(void *src, u32 size);
extern void sub_08066468(void);
extern void sub_08066480(struct GfxHeader *h, u32 src, u32 size);
extern void sub_080664cc(struct GfxHeader *h);
extern void sub_08066580(void);
extern void sub_080666cc(struct AnimCmd *p);
extern void sub_080667c0(u8 a, u16 b);
extern void sub_0806684c(void);
extern void sub_08066ae0(void);
extern u8 sub_08067060(void);
extern s32 sub_08067120(s16 x, s16 y, u16 dir, u8 p8);
extern void sub_08068f68(void);
extern u32 sub_08068cf8(s32 a);
extern u8 sub_080692fc(void);
extern s32 sub_08069b44(void);
extern u8 sub_0806acf8(void);
extern void sub_0806ad18(void);
extern void sub_0806a344(void);
extern s16 sub_0806caa0(u8 kind, s32 dx, s32 dy);
extern s16 sub_0806cc90(u8 flag, u16 vx, s32 c, s32 d);
extern void sub_0806cffc(s16 dx, s16 dy);
extern s32 Div(s32 numerator, s32 denominator);

/* Defined below */
void sub_0809118c(void);
void sub_080911a8(void);
void sub_08091954(void);
void sub_08091a30(void);
void sub_08091a98(void);
void sub_08091d24(void);
void sub_08091e18(void);

void sub_08091f08(void)
{
    struct Task *t;
    struct Task *u;

    sub_08066088(0);
    t = gUnk_03002490;
    t->unk00 = (u32)sub_080656b4;
    t->unk0C = (u32)sub_080653ec;
    t->unk42 = 11;
    u = gUnk_03002490;
    u->unk38 = gUnk_087535FC;
    gUnk_02007D00[7]++;
    u->unk28 = -1;
    u->unk2C = 0;
    u->unk30 = 0;
    u->unk34 = 1;
    u->unk1C = -1;
    u->unk24 = u->unk8C->unk28;
    if (sub_08067060() == 1)
        gUnk_03002490->unk20 = 24;
    else
        gUnk_03002490->unk20 = 0;
    sub_08066ae0();
    sub_08002e98(gUnk_03002490->unk73, 1, gUnk_08743ADC);
}
