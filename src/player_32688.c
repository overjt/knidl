#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_32688.c (0x08032688-0x080337F3, issue #92).
 *
 * Task type #5 (class 1), the player task, and its callbacks.
 * sub_08032688 is the body: it binds the task to its player record
 * (Task.unk88 = &gUnk_03002170[gCurTaskIdx]), kills it when the player
 * has no lives and no health left, installs the callbacks (Task.unk00 =
 * M11's sub_0803d494, unk04 = sub_08032d48, unk08 = sub_0803332c, unk0C =
 * M11's sub_0803ddc0), sets up the ability (PlayerState.unk0D) and the
 * stage entry mode (gUnk_02000020, gUnk_020069F0), and starts the first
 * action.  The actions are two tables of void (*)(void) dispatched
 * through sub_08002e98(index, count, table), entry 0 NULL: the "enter"
 * coroutine of action PlayerState.unk02 from gUnk_0873A748[62] (M11's
 * gUnk_0873B42C[30] when gUnk_03001F30 != 0) and the "per-frame" handler
 * Task.unk15 from gUnk_0873A840[57] (M11's gUnk_0873B4A4[27]).  A handler requests
 * the next action in PlayerState.unk01; sub_08032bd0 is the coroutine
 * that switches to it (unk03 = previous, unk02 = new, unk01 = 0).
 * sub_08032d48 (Task.unk04) runs every frame: the attack hit-boxes
 * (sub_08030848 on PlayerState.unk6C), the collision registry, the
 * per-frame handler and the damage and star-block reactions;
 * sub_0803332c (Task.unk08) handles the hurt / invincibility timer
 * PlayerState.unk2B; sub_08033414 (called by M11's sub_0803ddc0)
 * updates the carried/swallowed state and the 16.16 position unpack. */

struct M11R8 { u8 unk00; u8 unk01; u8 unk02; u8 unk03; u8 *unk04; };

struct M11R20 { u32 w[5]; };

/* A hit-box set: unk0 & 0x8000 = mirror with the task's facing, unk0 & 0xFFF
   = the attack id passed to sub_0803111c; unk2/unk3 = (x, y) offset of the
   set; unk4 = the boxes, {y0, y1, x0, x1} each (x mirrored as -x1..-x0),
   terminated by y0 == 127. */
struct HitBoxSet
{
    /*0x00*/ u16 unk0;
    /*0x02*/ s8 unk2;
    /*0x03*/ s8 unk3;
    /*0x04*/ s8 (*unk4)[4];
};

/* gUnk_03005550: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh). */
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

struct Unk02005E00
{
    /*0x00*/ s32 unk00;
    /*0x04*/ u8 unk04[4];
    /*0x08*/ u8 unk08[4];
};

extern struct PlayerState gUnk_03002170[];
extern s16 gUnk_02007D48[];
extern s16 gUnk_02005588[];
extern u8 gUnk_030023B0;
extern u16 gUnk_030023D8;
extern u32 gUnk_0874CFEC[];
extern u16 gUnk_030023AC;
extern u16 gUnk_03002360;
extern u8 gUnk_03001F30;
extern u32 gUnk_0873BD00[];               /* stored to PlayerState.unk64 as (u32)gUnk_0873BD00 */
extern u32 gUnk_0873CA54[];
extern u32 gUnk_0873CB1C[];
extern struct M11R20 gUnk_020060E0[];
extern u32 gUnk_0873C358[];
extern struct M11R8 gUnk_02005550[];
extern u32 gUnk_0873CF94[];
extern u8 gUnk_02000020;
extern u8 gUnk_020069F0;
extern u8 gUnk_020061E0;
extern u8 gUnk_03001F34;
extern void (*gUnk_0873A748[])(void);
extern void (*gUnk_0873B42C[])(void);
extern u8 gUnk_03005568;
extern u8 gUnk_02005574[];
extern struct Unk03005550 gUnk_03005550;
extern u16 gUnk_03005544;
extern struct Unk03005530 gUnk_03005530;
extern s16 gUnk_03002348;
extern s16 gUnk_030023E4;
extern void (*gUnk_0873A840[])(void);
extern void (*gUnk_0873B4A4[])(void);
extern s16 gUnk_0300244C;
extern u16 gUnk_0200AFE8[];
extern u32 gUnk_0873C36C[];
extern u32 gUnk_0873CF9C[];
extern struct Unk02005E00 gUnk_02005E00;
extern u16 gUnk_03000F98[];
extern u8 gUnk_02007CF0;
extern u16 gUnk_02004B50[];
extern u16 gUnk_02008008[];
extern u16 gUnk_02007FA8[];
extern u16 gUnk_0200AF18[];

void sub_08002e98(u32 idx, u32 count, void (**fns)(void));
u32 sub_08002ee8(u32 range);
s32 sub_08003110(s32 songId);
void sub_080055b0(u8 val, s32 idx);
void sub_08006138(void);
void sub_08006148(void *a, u32 i);
s32 sub_08009ee8(s32 a, u32 b);
s32 sub_08009fcc(s32 a, s32 b, u32 c);
s32 sub_0800a008(s32 a, s32 b, u32 c);
u32 sub_0801a828(u8 idx, s16 x, s16 y, u8 *p);   /* M09's callers pass ldrsh values unextended (LESSONS 8) */
void sub_0801baa4(u32 a);
void sub_08021c74(s8 *box, s32 id);
void sub_080224b0(void);
s32 sub_080260b0(void);
void sub_08026264(s32 x, s32 y);
u16 sub_08030848(struct HitBoxSet *p, s32 e);
void sub_0803c9b4(s32 a);                     /* M10: mov r8, r0 on entry, void epilogue */
void sub_0803cbd8(void);                      /* M10: no argument read, void epilogue */
void sub_0803ce98(void);
void sub_0803d494(void);
void sub_0803ddc0(void);
void sub_0803e080(void);
void sub_0803e1b8(s32 a0, s32 a1, s32 a2);
void sub_0803e2d4(void);
void sub_0803e3ac(void);
s32 sub_0803e5c0(s32 playerIdx, u8 *src6);
s32 sub_0803e5f8(s32 a0, s32 a1);
void sub_0803e650(s32 a0);
s32 sub_0803fa74(void);
void sub_0803fb54(void);
s32 sub_08040514(void);
void sub_08040808(s32 a0);
void sub_08040894(s32 a0, u8 a1);
void sub_08049738(void);
void sub_08049a58(void);
void sub_0804fe68(void);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);
s32 sub_0805b088(s32 a0, s32 a1, s32 a2);
void sub_0805b278(void);
void sub_0806ee30(void);
void sub_08071cc0(int x, int y, int c);
void sub_08076318(void);
void sub_080b8ebc(void);
void sub_080b9118(void);
void sub_080b9610(void);
void sub_08032d48(void);
void sub_0803332c(void);

void sub_08032688(void)
{
    struct Task *t;

    gUnk_03002490->unk44 = gCurTaskIdx;
    gUnk_03002490->unk88 = &gUnk_03002170[gCurTaskIdx];
    if (gUnk_02007D48[gUnk_03002490->unk88->unk00] == 0 && gUnk_02005588[gUnk_03002490->unk88->unk00] == 0)
    {
        gUnk_03002490->unk04 = 0;
        gUnk_03002490->unk12 = 4;
        if (gUnk_030023B0 == 0)
        {
            if (gUnk_030023D8 != 20)
                sub_080b9610();
            else
                sub_080b9118();
            sub_08006138();
        }
        sub_08006138();
    }
    else
    {
        sub_080b8ebc();
    }
    gUnk_03002490->unk43 = 1;
    t = gUnk_03002490;
    t->unk00 = (u32)sub_0803d494;
    t->unk0C = (u32)sub_0803ddc0;
    t->unk04 = (u32)sub_08032d48;
    t->unk08 = (u32)sub_0803332c;
    t->unk38 = gUnk_0874CFEC;
    if (gUnk_030023AC > 1 && gUnk_03002360 == t->unk88->unk00)
        t->unk42 = 6;
    else
        t->unk42 = 7;
    gUnk_03002490->unk40 = (gUnk_03002490->unk88->unk00 << 13) | (gUnk_03002490->unk88->unk00 << 7);
    gUnk_03002490->unk76 = 0;
    gUnk_03002490->unk88->unk01 = 0;
    if (gUnk_03001F30 == 0)
        gUnk_03002490->unk88->unk64 = (u32)gUnk_0873BD00;
    else
        gUnk_03002490->unk88->unk64 = (u32)gUnk_0873CA54;
    gUnk_03002490->unk88->unk68 = (u32)gUnk_0873CB1C;
    gUnk_03002490->unk88->unk6C = 0;
    gUnk_03002490->unk88->unk5E = gUnk_03002490->unk4C >> 16;
    gUnk_03002490->unk88->unk60 = gUnk_03002490->unk50 >> 16;
    gUnk_03002490->unk78 = gUnk_02005588[gUnk_03002490->unk88->unk00];
    if (gUnk_03002490->unk88->unk0D != 0)
    {
        sub_08049738();
        switch (gUnk_03002490->unk88->unk0D)
        {
        case 1:
        case 2:
            sub_0805afac(gUnk_03002490->unk88->unk00, 15, 0);
            sub_08049a58();
            break;
        case 10:
            {
                struct M11R20 *d = gUnk_020060E0;

                d[gUnk_03002490->unk88->unk00] = *(struct M11R20 *)gUnk_0873C358;
            }
            gUnk_02005550[gUnk_03002490->unk88->unk00] = *(struct M11R8 *)gUnk_0873CF94;
            break;
        case 7:
        case 20:
        case 21:
            gUnk_03002490->unk88->unk22 = 2;
            break;
        case 24:
            if (gUnk_030023D8 != 5)
                break;
        case 11:
            sub_0800a008(0, -1, gUnk_03002490->unk88->unk00);
            break;
        case 25:
            if (gUnk_02000020 != 2 && gUnk_02000020 != 3)
                sub_08009fcc(0, -1, gUnk_03002490->unk88->unk00);
            break;
        }
    }
    gUnk_03002490->unk88->unk36 = 0;
    switch (gUnk_02000020)
    {
    case 0:
        break;
    case 1:
        gUnk_03002490->unk04 = 0;
        gUnk_03002490->unk08 = 0;
        gUnk_03002490->unk88->unk64 = 0;
        gUnk_03002490->unk88->unk68 = 0;
        gUnk_03002490->unk88->unk6C = 0;
        sub_0805b278();
        sub_08006138();
    case 2:
        gUnk_03002490->unk88->unk37 = 2;
        sub_0800a008(25, -1, gUnk_03002490->unk88->unk00);
        gUnk_03002490->unk88->unk06 = 3;
        sub_0804fe68();
        sub_08006138();
    case 3:
        gUnk_03002490->unk88->unk37 = 3;
        sub_08009fcc(25, -1, gUnk_03002490->unk88->unk00);
    }
    switch (gUnk_020069F0)
    {
    case 2:
        gUnk_03002490->unk88->unk64 = 0;
        gUnk_03002490->unk88->unk68 = 0;
        gUnk_03002490->unk88->unk6C = 0;
        if (gUnk_03002490->unk88->unk00 == 0 || gUnk_020061E0 == 0)
            sub_08071cc0(gUnk_03002490->unk4C, gUnk_03002490->unk50, sub_080260b0());
        sub_0806ee30();
        sub_08006138();
    case 3:
        gUnk_03002490->unk88->unk64 = 0;
        gUnk_03002490->unk88->unk68 = 0;
        gUnk_03002490->unk88->unk6C = 0;
        gUnk_03001F34 = 1;
        gUnk_03002490->unk14 = 4;
        sub_08076318();
        sub_08006138();
    case 1:
        gUnk_03001F34 = 1;
        gUnk_03002490->unk88->unk02 = 21;
        break;
    case 0:
    default:
        sub_080224b0();
        sub_08021c74((s8 *)gUnk_0873CB1C, gCurTaskIdx);
        gUnk_03002490->unk88->unk5C = gUnk_03002490->unk7B;
        if (!(gUnk_03002490->unk7B & 1))
        {
            if (gUnk_03002490->unk7A & 1)
                gUnk_03002490->unk88->unk02 = 1;
            else
                gUnk_03002490->unk88->unk02 = 7;
        }
        else
        {
            if (gUnk_03002490->unk7A & 1)
                gUnk_03002490->unk88->unk02 = 24;
            else
                gUnk_03002490->unk88->unk02 = 23;
        }
        if (gUnk_03002490->unk88->unk0D == 24)
            gUnk_03002490->unk88->unk02 = 55;
        gUnk_03002490->unk88->unk04 = 21;
        sub_08040808(gUnk_03002490->unk88->unk00);
        break;
    }
    if (gUnk_03001F30 == 0)
    {
        struct PlayerState *p = gUnk_03002490->unk88;
        gUnk_03002490->unk14 = p->unk02;
        sub_08002e98(p->unk02, 62, gUnk_0873A748);
    }
    else
    {
        struct PlayerState *p = gUnk_03002490->unk88;
        gUnk_03002490->unk14 = p->unk02;
        sub_08002e98(p->unk02, 30, gUnk_0873B42C);
    }
}

void sub_08032bd0(void)
{
    if (gUnk_030023AC > 1)
    {
        if (gUnk_03002360 == gUnk_03002490->unk88->unk00)
            gUnk_03002490->unk42 = 6;
        else
            gUnk_03002490->unk42 = 7;
    }
    gUnk_03002490->unk88->unk03 = gUnk_03002490->unk88->unk02;
    gUnk_03002490->unk88->unk02 = gUnk_03002490->unk88->unk01;
    gUnk_03002490->unk88->unk01 = 0;
    if (gUnk_03002490->unk88->unk06 == 1)
    {
        gUnk_03002490->unk88->unk64 = (u32)gUnk_0873BD00;
        gUnk_03002490->unk88->unk68 = (u32)gUnk_0873CB1C;
        gUnk_03002490->unk88->unk6C = 0;
    }
    else if (gUnk_03002490->unk88->unk37 != 2)
    {
        if (gUnk_03001F30 == 0)
            gUnk_03002490->unk88->unk64 = (u32)gUnk_0873BD00;
        else
            gUnk_03002490->unk88->unk64 = (u32)gUnk_0873CA54;
        gUnk_03002490->unk88->unk68 = (u32)gUnk_0873CB1C;
        gUnk_03002490->unk88->unk6C = 0;
    }
    gUnk_03002490->unk88->unk24 = gUnk_03002490->unk88->unk26 = 0;
    if (gUnk_03002490->unk88->unk3F == 3 && (s16)gUnk_03002490->unk88->unk12 == -0x8000)
        sub_0803e1b8(255, 0, gUnk_03002490->unk88->unk00);
    if (gUnk_03001F30 == 0)
    {
        if (gUnk_03002490->unk88->unk03 == 28 && gUnk_03002490->unk88->unk0D != 0)
            sub_08049738();
        if (gUnk_03002490->unk88->unk0D != 0)
            gUnk_03002490->unk88->unk36 = 1;
    }
    if (gUnk_03001F30 == 0)
    {
        struct PlayerState *p = gUnk_03002490->unk88;
        gUnk_03002490->unk14 = p->unk02;
        sub_08002e98(p->unk02, 62, gUnk_0873A748);
    }
    else
    {
        struct PlayerState *p = gUnk_03002490->unk88;
        gUnk_03002490->unk14 = p->unk02;
        sub_08002e98(p->unk02, 30, gUnk_0873B42C);
    }
}

void sub_08032d48(void)
{
    s32 x;
    s32 y;
    s32 r;
    struct PlayerState *p;

    if ((gUnk_03002490->unk88->unk40 & 1) && (gUnk_03002490->unk13 & 1))
        goto post;
    if (gUnk_03002490->unk88->unk0D == 10 && gUnk_03002490->unk88->unk04 == 5)
    {
        gUnk_03002490->unk2C += gUnk_03002490->unk28;
        gUnk_03002490->unk4C += gUnk_03002490->unk2C;
        gUnk_03002490->unk48 = gUnk_03002490->unk4C >> 16;
    }
    if (gUnk_03002490->unk88->unk6C != 0)
    {
        gUnk_03002490->unk88->unk44 = sub_08030848(gUnk_03002490->unk88->unk6C, gUnk_03002490->unk88->unk00);
        if (gUnk_03002490->unk88->unk44 != 0)
            gUnk_03002490->unk76 |= 1;
    }
    else
    {
        gUnk_03002490->unk88->unk44 = 0;
    }
    gUnk_03002490->unk88->unk5C = gUnk_03002490->unk7B;
    gUnk_03002490->unk88->unk4E = 0xFFFF;
    if (gUnk_03002490->unk88->unk68 != 0)
    {
        sub_0801baa4(gUnk_03002490->unk88->unk68);
        gUnk_03002490->unk88->unk48 = gUnk_03005568;
        if (gUnk_02005574[0] == 0 && (gUnk_03005568 & 4) && gUnk_03005550.unk0 != 0)
            gUnk_03002490->unk88->unk4E = gUnk_03005544;
        gUnk_03002490->unk88->unk70 = (u32 *)gUnk_03002490->unk88->unk68;
        if (gUnk_03005550.unkC != 0 && !(gUnk_03002490->unk88->unk42 & 0x200)
         && gUnk_03002490->unk88->unk3F != 1 && gUnk_03002490->unk88->unk17 == 0)
        {
            r = sub_08009ee8(-8, gUnk_03002490->unk88->unk00);
            if (r != 0)
            {
                gUnk_03002490->unk82 = gUnk_03005550.unkC | 0x80;
                gUnk_03002490->unk7C = 2;
            }
            else
            {
                gUnk_03002490->unk7C = 1;
                gUnk_03002490->unk82 = 0;
                goto post;
            }
        }
    }
    else
    {
        gUnk_03005550.unk0 = gUnk_03005550.unk1 = gUnk_03005550.unk2 = 0;
        gUnk_03005550.unk3 = gUnk_03005550.unk4 = gUnk_03005550.unk5 = 0;
        gUnk_03005550.unk8 = gUnk_03005550.unkB = gUnk_03005550.unkC = 0;
        gUnk_03005550.unkA = 0;
        gUnk_03005530.unkE = 0;
    }
    gUnk_03002490->unk88->unk4A = gUnk_03005550.unk0;
    gUnk_03002490->unk88->unk4B = gUnk_03005550.unk4;
    gUnk_03002490->unk88->unk49 = gUnk_03005530.unkE;
    gUnk_03002490->unk88->unk4C = gUnk_03005550.unkA;
    if (sub_0803fa74() != 0)
        goto tail;
    p = gUnk_03002490->unk88;
    if (p->unk64 != 0)
    {
        x = gUnk_03002490->unk48;
        y = gUnk_03002490->unk4A;
        if (p->unk37 == 2)
        {
            x += gUnk_03002348;
            y += gUnk_030023E4;
        }
        sub_0801a828(gCurTaskIdx, x, y, (u8 *)p->unk64);
    }
    if (gUnk_03001F30 == 0)
        sub_08002e98(gUnk_03002490->unk15, 57, gUnk_0873A840);
    else
        sub_08002e98(gUnk_03002490->unk15, 27, gUnk_0873B4A4);
    sub_0803e2d4();
post:
    gUnk_03002490->unk88->unk45 = 0;
    sub_0803fb54();
    if (!(gUnk_03002490->unk88->unk42 & 32))
        sub_0803e080();
    if ((gUnk_03001F30 == 1 || gUnk_0300244C != 0)
     && (gUnk_03002490->unk88->unk40 & 1) && (gUnk_03002490->unk13 & 1))
        goto check;
    if (gUnk_03002490->unk58 >= 0)
    {
        if (gUnk_03002490->unk7B & 0x80)
            sub_0805afac(gUnk_03002490->unk88->unk00, 9, gUnk_03005550.unk8);
    }
    else if (gUnk_03002490->unk88->unk06 == 2)
    {
        if (gUnk_03002490->unk7B & 0x80)
            sub_0805afac(gUnk_03002490->unk88->unk00, 10, gUnk_03005550.unk8);
    }
    else if ((gUnk_03002490->unk88->unk5C & 1) && !(gUnk_03002490->unk7B & 1))
    {
        sub_0805afac(gUnk_03002490->unk88->unk00, 10, gUnk_03005550.unk8);
    }
    if ((gUnk_03002490->unk7B & 65) == 1)
    {
        if (--gUnk_0200AFE8[gUnk_03002490->unk88->unk00] == 0)
        {
            gUnk_0200AFE8[gUnk_03002490->unk88->unk00] = sub_08002ee8(90) + 120;
            sub_0805b088(gUnk_03002490->unk88->unk00, 11, 0);
        }
    }
    else
    {
        gUnk_0200AFE8[gUnk_03002490->unk88->unk00] = 60;
    }
check:
    if (gUnk_03001F30 == 0)
    {
        if (gUnk_03002490->unk88->unk0D == 10)
        {
            x = gUnk_03002490->unk3C - 0x808;
            if (x >= 0 && sub_0803e5c0(gUnk_03002490->unk88->unk00, (u8 *)gUnk_0873C36C + x * 8) != 0)
            {
                if (gUnk_03002490->unk3C <= 0x8D1)
                {
                    struct M11R20 *d = gUnk_020060E0;
                    ((u8 *)&d[gUnk_03002490->unk88->unk00])[12] = 2;
                }
                else
                {
                    struct M11R20 *d = gUnk_020060E0;
                    ((u8 *)&d[gUnk_03002490->unk88->unk00])[12] = 5;
                }
                sub_0801a828(gCurTaskIdx, gUnk_03002490->unk48, gUnk_03002490->unk4A,
                             (u8 *)gUnk_020060E0 + gUnk_03002490->unk88->unk00 * 20);
            }
            x = gUnk_03002490->unk3C - 0x8D2;
            if (x >= 0 && sub_0803e5f8(gUnk_03002490->unk88->unk00, (s32)((u8 *)gUnk_0873CF9C + x * 8)) != 0)
                sub_08030848((struct HitBoxSet *)&gUnk_02005550[gUnk_03002490->unk88->unk00], gUnk_03002490->unk88->unk00);
        }
        if (gUnk_02005E00.unk04[gCurTaskIdx] & 1)
        {
            if ((gUnk_03000F98[gUnk_03002490->unk88->unk00] & 0x300) == 0x300)
                gUnk_02005E00.unk04[gCurTaskIdx] = (gUnk_02005E00.unk04[gCurTaskIdx] & 0xF0) | 2;
        }
    }
    else if (gUnk_03002490->unk88->unk10 != 0)
    {
        gUnk_03002490->unk88->unk10--;
    }
tail:
    if (gUnk_03002490->unk88->unk37 != 2 && gUnk_03002360 == gUnk_03002490->unk88->unk00)
        sub_08026264(gUnk_03002490->unk4C >> 16, gUnk_03002490->unk50 >> 16);
    if (gUnk_03002490->unk88->unk68 != 0 && !(gUnk_03002490->unk13 & 2))
    {
        gUnk_03002490->unk88->unk5E = gUnk_03002490->unk48;
        gUnk_03002490->unk88->unk60 = gUnk_03002490->unk4A;
    }
}

void sub_0803332c(void)
{
    struct Task *t;
    struct PlayerState *p;

    t = gUnk_03002490;
    if (t->unk88->unk01 == 0)
    {
        if (t->unk76 & 1)
        {
            t->unk76 &= 0xFFFE;
            if ((s8)t->unk88->unk2B == 0)
            {
                sub_0803e650(0);
                gUnk_03002490->unk88->unk2B = 10;
            }
        }
        p = gUnk_03002490->unk88;
        if (p->unk40 & 1)
        {
            sub_0803cbd8();
            p = gUnk_03002490->unk88;
            if (!(p->unk40 & 1) && gUnk_03001F30 == 0 && p->unk04 == 7 && p->unk44 == 0)
            {
                sub_08006148(sub_08032bd0, gCurTaskIdx);
                gUnk_03002490->unk88->unk01 = 18;
            }
        }
        else if ((s8)p->unk2B != 0)
        {
            p->unk2B--;
        }
    }
    if (gUnk_03002490->unk88->unk01 == 0)
        sub_08040514();
}

void sub_08033414(void)
{
    struct Task *t;
    struct PlayerState *p;

    if (gUnk_03002490->unk88->unk01 > 31 && gUnk_02007CF0 == 1)
        gUnk_03002490->unk88->unk01 = 0;
    switch (gUnk_03002490->unk7C)
    {
    default:
        if (gUnk_03001F30 == 0 && (gUnk_03002490->unk88->unk40 & 32))
        {
            if ((s16)gUnk_02004B50[gUnk_03002490->unk88->unk00] != 0)
            {
                gUnk_02008008[gUnk_03002490->unk88->unk00] = gUnk_02004B50[gUnk_03002490->unk88->unk00];
                gUnk_02007FA8[gUnk_03002490->unk88->unk00] = gUnk_0200AF18[gUnk_03002490->unk88->unk00];
            }
            else
            {
                gUnk_02008008[gUnk_03002490->unk88->unk00] = 4;
                gUnk_02007FA8[gUnk_03002490->unk88->unk00] = 0xFFFF;
            }
            gUnk_03002490->unk88->unk0B = 4;
            gUnk_03002490->unk88->unk0C = 255;
            gUnk_03002490->unk88->unk37 = 1;
            gUnk_03002490->unk88->unk01 = 29;
        }
        else if (gUnk_03002490->unk88->unk40 & 64)
        {
            sub_0803e1b8(5, 0, gUnk_03002490->unk88->unk00);
            gUnk_03002490->unk88->unk40 &= 0xFFBF;
            sub_08003110(19);
            sub_08040894(gUnk_03002490->unk88->unk00, 3);
        }
        break;
    case 1:
        gUnk_03002490->unk88->unk01 = 17;
        gUnk_03002490->unk88->unk22 = 0;
        gUnk_03002490->unk88->unk1E = gUnk_03002490->unk88->unk20 = 0;
        break;
    case 2:
        if (gUnk_03002490->unk88->unk37 != 2)
        {
            gUnk_03002490->unk88->unk01 = 16;
            gUnk_03002490->unk88->unk16 = 255;
        }
        else
        {
            gUnk_03002490->unk88->unk01 = 58;
            gUnk_03002490->unk73 = 3;
        }
        gUnk_03002490->unk88->unk22 = 0;
        gUnk_03002490->unk88->unk1E = gUnk_03002490->unk88->unk20 = 0;
        break;
    }
    gUnk_03002490->unk7C = 0;
    if (gUnk_03002490->unk88->unk01 != 0)
    {
        if (gUnk_03002490->unk88->unk40 & 1)
        {
            gUnk_03002490->unk88->unk28 = 0;
            gUnk_03002490->unk88->unk29 = 1;
            gUnk_03002490->unk88->unk2B = 0;
            gUnk_03002490->unk88->unk24 = gUnk_03002490->unk88->unk26 = 0;
            sub_080055b0(0, gCurTaskIdx);
        }
        if (gUnk_03002490->unk88->unk2C != -1)
            sub_0803e3ac();
        gUnk_03002490->unk76 = 0;
        gUnk_03002490->unk88->unk40 = 0;
        gUnk_03002490->unk88->unk50 = 0;
        gUnk_03002490->unk80 = 0;
    }
    else if (gUnk_03001F30 == 0)
    {
        if (gUnk_03002490->unk76 & 2)
        {
            gUnk_03002490->unk88->unk40 |= 2;
            gUnk_03002490->unk73 = 1;
            gUnk_03002490->unk88->unk01 = 8;
            gUnk_03002490->unk76 &= 0xFFFD;
        }
        if (!(gUnk_03002490->unk88->unk40 & 128))
            sub_0803ce98();
    }
    if (gUnk_03002490->unk88->unk01 != 0)
        sub_08006148(sub_08032bd0, gCurTaskIdx);
    if (gUnk_03001F30 == 0)
    {
        if (gUnk_03002490->unk88->unk40 & 4)
        {
            if (gUnk_03002490->unk88->unk0D == 0)
                sub_0803c9b4(0);
            else
                sub_0803c9b4(1);
        }
        if ((s8)gUnk_03002490->unk88->unk36 != 0)
        {
            switch (gUnk_03002490->unk88->unk03)
            {
            case 32:
            case 33:
                sub_0805afac(gUnk_03002490->unk88->unk00, 15, 0);
                sub_08049a58();
                break;
            }
            gUnk_03002490->unk88->unk36 = 0;
        }
    }
    t = gUnk_03002490;
    if (t->unk88->unk37 == 2)
    {
        if (t->unk4A + gUnk_030023E4 > 900)
            t->unk50 = (t->unk4A = 900 - gUnk_030023E4) << 16;
    }
    t = gUnk_03002490;
    t->unk48 = (t->unk4C + ((s16)t->unk88->unk24 & 0x8000 ? ((s16)t->unk88->unk24 << 8) | 0xFF000000 : (s16)t->unk88->unk24 << 8)) >> 16;
    t = gUnk_03002490;
    t->unk4A = (t->unk50 + ((s16)t->unk88->unk26 & 0x8000 ? ((s16)t->unk88->unk26 << 8) | 0xFF000000 : (s16)t->unk88->unk26 << 8)) >> 16;
}
