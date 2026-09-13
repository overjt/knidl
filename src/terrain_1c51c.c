#include "gba/gba.h"
#include "global.h"
#include "task.h"

/*
 * M06 terrain / collision query (issue #84), range 0x0801C51C-0x0801C690.
 *
 * sub_0801c51c copies the actor's six signed box offsets and the task fields
 * into the room-descriptor cells and clears the probe result block at
 * gUnk_03005530; sub_0801c5c8 writes the probe results back into the task
 * (re-seating Task.unk4C/unk50 when the probe moved the actor) and mirrors the
 * result block into gUnk_03005550.
 */


/* ROM pointer tables: one entry per tile set, each pointing at a byte table. */
extern u8 *const gUnk_08734BF0[];
extern u8 *const gUnk_08733BF0[];
extern u8 *const gUnk_08733FF0[];
extern u8 *const gUnk_087343F0[];
extern u8 *const gUnk_087347F0[];
extern s8 *const gUnk_087330F0[];
extern s8 *const gUnk_087328F0[];

/* ROM byte tables indexed by tile set. */
extern u8 gUnk_08732CF0[];
extern u8 gUnk_087337F0[];
extern u8 gUnk_087334F0[];
extern s8 gUnk_087336F0[];
extern s8 gUnk_08732FF0[];
extern s8 gUnk_08733AF0[];
extern s8 gUnk_08732EF0[];
extern s8 gUnk_087338F0[];
extern u8 gUnk_08732DF0[];

/* IWRAM room descriptor cells. */
extern u16 gUnk_03005504;
extern s16 gUnk_0300550C;
extern u16 gUnk_03005508;
extern u16 gUnk_03005510;
extern s32 gUnk_03005514;
extern s16 gUnk_03005518;
extern s16 gUnk_03005520;
extern s16 gUnk_0300551C;
extern s16 gUnk_03005560;
extern u8 gUnk_03005564;
extern u16 gUnk_0300556C;
extern s16 gUnk_03005570;
extern u16 gUnk_03005574;
extern u16 gUnk_03005578;
extern s16 gUnk_0300557C;
extern s16 gUnk_03005584;
extern u16 gUnk_03005588;
extern s16 gUnk_03005590;
extern u16 gUnk_03005594;
extern s16 gUnk_0300559C;
extern s8 *gUnk_030055A0;
extern s16 gUnk_030055A4;
extern u16 gUnk_030055AC;
extern s16 gUnk_030055B0;
extern s16 gUnk_030055E4;
extern s32 gUnk_03005580;
extern s32 gUnk_03005598;
extern s32 gUnk_030055A8;
extern s16 gUnk_0300561C;
extern s16 gUnk_03005620;

struct MapCell
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
};
extern struct MapCell *gUnk_03005660;

extern s16 *gUnk_0300558C;

struct Unk03005530
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
    /*0x0E*/ u8 unkE;
    /*0x0F*/ u8 unkF;
    /*0x10*/ u8 unk10;
};
extern struct Unk03005530 gUnk_03005530;

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
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};
extern struct Unk03005550 gUnk_03005550;

s32 sub_080217dc(u32 x, u32 y);
void sub_0801c51c(const s8 *p);
void sub_080207a0(void);
void sub_080214e0(void);
u32 sub_0802069c(void);
s32 sub_08021990(u16 a);
s32 sub_080219d0(u16 a);
s32 sub_08021634(u32 x, u32 y);

void sub_0801c51c(const s8 *p)
{
    gUnk_03005560 = (gUnk_03002490->unk4C >> 16) + p[0];
    gUnk_03005570 = (gUnk_03002490->unk50 >> 16) + p[1];
    gUnk_0300557C = p[2];
    gUnk_03005584 = p[3];
    gUnk_0300551C = p[4];
    gUnk_0300559C = p[5];
    gUnk_03005564 = gUnk_03002490->unk43;
    gUnk_03005530.unk0 = gUnk_03005530.unk1 = gUnk_03005530.unk2 = gUnk_03005530.unk3 = gUnk_03005530.unk4 = gUnk_03005530.unk5 = gUnk_03005530.unkE = gUnk_03005530.unkF = gUnk_03005530.unk10 = 0;
    gUnk_03005530.unk6 = gUnk_03002490->unk7A;
    gUnk_03005530.unk7 = gUnk_03002490->unk7B;
    gUnk_03005530.unkB = gUnk_03002490->unk84;
    gUnk_03005530.unkC = gUnk_03002490->unk84 >> 8;
}

void sub_0801c5c8(const s8 *p)
{
    gUnk_03002490->unk7A = gUnk_03005530.unk6;
    gUnk_03002490->unk7B = gUnk_03005530.unk7;
    if (gUnk_03002490->unk4C >> 16 != gUnk_03005560 - p[0])
    {
        gUnk_03002490->unk4C = ((gUnk_03005560 - p[0]) << 16) + 0x8000;
        gUnk_03002490->unk48 = gUnk_03005560 - p[0];
    }
    if (gUnk_03002490->unk50 >> 16 != gUnk_03005570 - p[1])
    {
        gUnk_03002490->unk50 = ((gUnk_03005570 - p[1]) << 16) + 0x8000;
        gUnk_03002490->unk4A = gUnk_03005570 - p[1];
    }
    gUnk_03005550.unk0 = gUnk_03005530.unk0;
    gUnk_03005550.unk1 = gUnk_03005530.unk1;
    gUnk_03005550.unk2 = gUnk_03005530.unk2;
    gUnk_03005550.unk3 = gUnk_03005530.unk3;
    gUnk_03005550.unk4 = gUnk_03005530.unk4;
    gUnk_03005550.unk5 = gUnk_03005530.unk5;
    gUnk_03005550.unk8 = gUnk_03005530.unk8;
    gUnk_03005550.unkB = gUnk_03005530.unkE;
    gUnk_03005550.unkC = gUnk_03005530.unkF;
    gUnk_03005550.unkD = gUnk_03005530.unk10;
    gUnk_03002490->unk84 = (gUnk_03005530.unkC << 8) | gUnk_03005530.unkB;
}
