#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_34f8c.c (0x08034F8C-0x0803627F, issue #92).
 *
 * Player mode bodies, part 3: modes 5, 8 and 14.  Each action has an
 * "enter" coroutine (gUnk_0873A748[], by PlayerState.unk02) and a
 * "per-frame" handler (gUnk_0873A840[], by Task.unk15).  An enter
 * coroutine records the previous mode
 * (PlayerState.unk05 = unk04), sets the new one and the task's animation
 * set (Task.unk15), then plays the animation of the current ability
 * (PlayerState.unk0D, 0..25) out of a per-mode table (gUnk_0873D3B8,
 * gUnk_0873D420, gUnk_0873D7E4) with TaskSetFrame and TaskYieldTrampoline;
 * the per-frame handler runs M11's transition predicates in order and
 * writes the next mode request into PlayerState.unk01, or re-binds the
 * task to another coroutine with TaskSetEntry.  sub_08034f8c/sub_08035458
 * are mode 5, sub_080355d8/sub_08035848 mode 8 (Task.unk73 is its
 * sub-state) and sub_080359f8 mode 14, a six-state loop over Task.unk73
 * whose per-frame handler is M10's sub_08036280. */

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

extern s16 gUnk_0873D3B8[][2];
extern u16 gUnk_03002458[];             /* latched state mask per player (M11) */
extern u8 gUnk_03001F30;
extern struct Unk03005550 gUnk_03005550;
extern s16 gUnk_0873D420[][3];
extern u32 gUnk_0873CC74[];
extern u8 gUnk_0873BEB0[];
extern u32 gUnk_0873BD00[];               /* stored to PlayerState.unk64 as (u32)gUnk_0873BD00 */
extern u32 gUnk_0873CB1C[];
extern u32 gUnk_0873BD14[];
extern s16 gUnk_0873D7E4[][3];

void TaskYieldTrampoline(s32 frames);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
/* src/player_1a76c.c defines it with u16 x/y; the ROM passes the task's
   s16 position unextended, so this file's callers see s16 parameters */
u32 sub_0801a828(u8 idx, s16 x, s16 y, u8 *p);
void sub_0803ccd8(s32 a);                     /* M10: lsls r0, #2 on entry, void epilogue */
void sub_0803cd60(void);
void sub_0803e050(s32 a0);
s32 sub_0803e34c(s32 a0, u16 a1);
s32 sub_0803e4ec(s32 a0);
void sub_0803f870(void);
void sub_0803f8e8(void);
void sub_0803f9c0(void);
s32 sub_0803fa44(void);
s32 sub_0803ff7c(void);
s32 sub_0803ffe0(void);
s32 sub_08040084(void);
s32 sub_080400c0(void);
s32 sub_08040264(void);
s32 sub_08040298(void);
s32 sub_08040340(void);
s32 sub_0804042c(void);
void sub_08040b40(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void sub_080413a4(s32 a0);
void sub_08053940(s32 a, s32 b, s32 c);
s32 sub_0805afac(s32 a0, s32 a1, s32 a2);

void sub_08034f8c(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 5;
    gCurTask->unk15 = 7;
    sub_080413a4(2);
    gCurTask->unk30 = 0;
    if (gCurTask->unk88->unk05 != 5)
        gCurTask->unk88->unk14 = 300;
    if (gCurTask->unk88->unk3E & 1)
        gCurTask->unk30 = 1;
    if (gCurTask->unk88->unk0D == 10)
    {
        gCurTask->unk2C = 0;
        gCurTask->unk28 = 0;
    }
    sub_0803cd60();
    gCurTask->unk88->unk14 = 30;
    if (gCurTask->unk88->unk05 == 4)
    {
        gCurTask->unk88->unk14 = 300;
        if (gCurTask->unk88->unk06 == 1)
        {
            TaskSetFrame(346);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk88->unk14 = 0;
        }
        else if (gCurTask->unk30 != 0)
        {
            gCurTask->unk88->unk14 = 30;
        }
        else
        {
            gCurTask->unk46 = gUnk_0873D3B8[gCurTask->unk88->unk0D][0];
            switch (gCurTask->unk88->unk0D)
            {
            case 0:
            default:
                TaskSetFrame(gCurTask->unk46);
                TaskYieldTrampoline(2);
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
                {
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                }
                gCurTask->unk88->unk14 = 20;
                break;
            case 4:
            case 9:
            case 10:
            case 25:
                TaskSetFrame(gCurTask->unk46);
                TaskYieldTrampoline(4);
                gCurTask->unk88->unk14 = 28;
                break;
            }
        }
    }
    gCurTask->unk30 = 0;
    if (gCurTask->unk88->unk06 == 1)
    {
        while (1)
        {
            TaskSetFrame(0x15D);
            TaskYieldTrampoline(4);
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
        }
    }
    gCurTask->unk46 = gUnk_0873D3B8[gCurTask->unk88->unk0D][1];
    switch (gCurTask->unk88->unk0D)
    {
    case 0:
    default:
        while (1)
        {
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        }
    case 10:
        if (gCurTask->unk88->unk05 != 8)
            gCurTask->unk88->unk14 = 0;
        do
        {
            TaskSetFrame(0x841);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)gCurTask->unk88->unk14 != 0 || gCurTask->unk58 <= 0x1FFFF);
        while (1)
        {
            if (gCurTask->unk43 == 1)
                gCurTask->unk28 = -0x400;
            else
                gCurTask->unk28 = 0x400;
            sub_0803ccd8(0);
            TaskSetFrame(0x839);
            TaskYieldTrampoline(8);
            sub_0803ccd8(1);
            gCurTask->unk3C--;
            TaskYieldTrampoline(8);
            sub_0803ccd8(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(8);
            sub_0803ccd8(3);
            gCurTask->unk3C--;
            TaskYieldTrampoline(8);
            sub_0803ccd8(4);
            gCurTask->unk3C--;
            TaskYieldTrampoline(8);
            if (gCurTask->unk43 == 1)
                gCurTask->unk28 = -0x800;
            else
                gCurTask->unk28 = 0x800;
            sub_0803ccd8(5);
            TaskSetFrame(0x840);
            TaskYieldTrampoline(4);
            sub_0803ccd8(6);
            TaskSetFrame(0x836);
            TaskYieldTrampoline(4);
            sub_0803ccd8(7);
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
            sub_0803ccd8(8);
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
            sub_0803ccd8(9);
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
            if (gCurTask->unk43 == 1)
                gCurTask->unk28 = 0x400;
            else
                gCurTask->unk28 = -0x400;
            sub_0803ccd8(10);
            TaskSetFrame(0x83A);
            TaskYieldTrampoline(8);
            sub_0803ccd8(11);
            gCurTask->unk3C++;
            TaskYieldTrampoline(8);
            sub_0803ccd8(12);
            gCurTask->unk3C++;
            TaskYieldTrampoline(8);
            sub_0803ccd8(13);
            gCurTask->unk3C++;
            TaskYieldTrampoline(8);
            sub_0803ccd8(14);
            gCurTask->unk3C++;
            TaskYieldTrampoline(8);
            if (gCurTask->unk43 == 1)
                gCurTask->unk28 = 0x800;
            else
                gCurTask->unk28 = -0x800;
            sub_0803ccd8(15);
            TaskSetFrame(0x83F);
            TaskYieldTrampoline(4);
            sub_0803ccd8(16);
            TaskSetFrame(0x83D);
            TaskYieldTrampoline(4);
            sub_0803ccd8(17);
            gCurTask->unk3C--;
            TaskYieldTrampoline(4);
            sub_0803ccd8(18);
            gCurTask->unk3C--;
            TaskYieldTrampoline(4);
            sub_0803ccd8(19);
            gCurTask->unk3C--;
            TaskYieldTrampoline(4);
        }
    }
}

void sub_08035458(void)
{
    sub_0803f870();
    while (sub_08040264() == 0 && sub_0803ff7c() == 0 && sub_08040298() == 0
           && sub_0803ffe0() == 0 && sub_08040084() == 0 && sub_080400c0() == 0
           && sub_08040340() == 0)
    {
        if (gCurTask->unk7A & 1)
        {
            sub_0803f8e8();
            sub_0803e4ec(0);
            sub_0804042c();
            break;
        }
        if (gUnk_03001F30 == 0 && gCurTask->unk88->unk0D != 10
            && gCurTask->unk88->unk06 == 0 && gCurTask->unk58 > 0
            && --gCurTask->unk88->unk14 == 0)
        {
            gCurTask->unk88->unk01 = 8;
            break;
        }
        if (gCurTask->unk88->unk0D != 10 && gUnk_03005550.unk0 != 0)
        {
            sub_0803f8e8();
            if (gCurTask->unk88->unk3E & 7)
                TaskSetEntry(sub_08034f8c, gCurTaskIdx);
        }
        if (gCurTask->unk88->unk0D == 10)
        {
            if ((s16)gCurTask->unk88->unk14 != 0)
                gCurTask->unk88->unk14--;
            if (gCurTask->unk28 != 0
                && (gUnk_03002458[gCurTask->unk88->unk00] & 128))
            {
                sub_0803e050(1);
                gCurTask->unk88->unk01 = 8;
            }
        }
        break;
    }
    sub_08040b40(7, 72);
    if (gCurTask->unk7A & 1)
        sub_0803e4ec(1);
    sub_0803f9c0();
}

void sub_080355d8(void)
{
    s16 *anim;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 8;
    gCurTask->unk15 = 8;
    if (gCurTask->unk88->unk05 != 8)
    {
        gCurTask->unk73 = 0;
        gCurTask->unk88->unk40 &= 0xFFFD;
    }
    gCurTask->unk88->unk6C = 0;
    gCurTask->unk28 = 0;
    anim = gUnk_0873D420[gCurTask->unk88->unk0D];
    switch (gCurTask->unk73)
    {
    case 0:
        gCurTask->unk88->unk6C = gUnk_0873CC74;
        switch (gCurTask->unk88->unk0D)
        {
        case 10:
            sub_080413a4(2);
        case 0:
        default:
            TaskSetFrame(anim[0]);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk28++;
            TaskSetFrame(anim[1]);
            TaskSleepForever();
        case 1:
        case 2:
        case 5:
        case 15:
        case 16:
        case 17:
        case 19:
        case 22:
        case 23:
            TaskSetFrame(anim[0]);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk28++;
            gCurTask->unk46 = anim[1];
            while (1)
            {
                TaskSetFrame(gCurTask->unk46);
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            }
        }
    case 2:
        gCurTask->unk88->unk42 |= 0x100;
        sub_0805afac(gCurTask->unk88->unk00, 4, 0);
        gCurTask->unk88->unk3E = 0;
        TaskSetFrame(anim[2]);
        TaskYieldTrampoline(2);
        gCurTask->unk73 = 1;
    case 1:
        gCurTask->unk88->unk3E = 0;
        sub_080413a4(3);
        TaskSetFrame((s16)(anim[2] + 1));
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk28++;
        break;
    }
    TaskSleepForever();
}

void sub_08035848(void)
{
    if (sub_0803ff7c() == 0 && sub_08040298() == 0 && sub_08040264() == 0
        && sub_0803ffe0() == 0 && sub_08040084() == 0 && sub_080400c0() == 0)
        sub_08040340();
    if (gCurTask->unk88->unk01 != 0)
    {
        gCurTask->unk88->unk42 &= 0xFEFF;
    }
    else
    {
        sub_0803f870();
        sub_08040b40(7, 72);
        switch (gCurTask->unk73)
        {
        case 0:
            if (sub_0803fa44() != 0)
            {
                sub_0803f8e8();
                sub_0803e34c(116, gCurTask->unk88->unk00);
                gCurTask->unk73 = 2;
                TaskSetEntry(sub_080355d8, gCurTaskIdx);
            }
            else
            {
                if (gCurTask->unk88->unk44 != 0)
                    gCurTask->unk88->unk40 |= 2;
                if (gCurTask->unk88->unk40 & 2)
                {
                    gCurTask->unk73 = 1;
                    TaskSetEntry(sub_080355d8, gCurTaskIdx);
                }
                else
                {
                    if (gUnk_03005550.unk0 != 0)
                    {
                        sub_0803f8e8();
                        if (gCurTask->unk88->unk3E & 7)
                            TaskSetEntry(sub_08034f8c, gCurTaskIdx);
                    }
                    if (gCurTask->unk28 != 0)
                        sub_0801a828(gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A, gUnk_0873BEB0);
                }
            }
            break;
        case 1:
            if (gCurTask->unk7A & 1)
            {
                sub_0803f8e8();
                sub_0803e4ec(0);
                sub_0804042c();
                gCurTask->unk88->unk42 &= 0xFEFF;
                break;
            }
        case 2:
            if (gCurTask->unk28 != 0)
            {
                gCurTask->unk88->unk01 = 7;
                gCurTask->unk88->unk42 &= 0xFEFF;
            }
            break;
        }
    }
    sub_0803f9c0();
}

void sub_080359f8(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 14;
    gCurTask->unk15 = 9;
    if (gCurTask->unk88->unk05 != 14)
    {
        gCurTask->unk88->unk3D = 0;
        gCurTask->unk73 = 0;
    }
    gCurTask->unk88->unk64 = (u32)gUnk_0873BD14;
    while (1)
    {
        switch (gCurTask->unk73)
        {
        case 0:
            sub_0803e34c(228, gCurTask->unk88->unk00);
            TaskSetFrame(gUnk_0873D7E4[gCurTask->unk88->unk0D][0]);
            TaskYieldTrampoline(2);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 3; gCurTask->unk6C++)
            {
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            }
            gCurTask->unk73 = 2;
            gCurTask->unk88->unk06 = 2;
            break;
        case 1:
            sub_0803e34c(115, gCurTask->unk88->unk00);
            gCurTask->unk46 = gUnk_0873D7E4[gCurTask->unk88->unk0D][1];
            switch (gCurTask->unk88->unk0D)
            {
            case 0:
            default:
                while (1)
                {
                    TaskSetFrame(gCurTask->unk46);
                    TaskYieldTrampoline(4);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(4);
                    gCurTask->unk3C -= 2;
                    TaskYieldTrampoline(2);
                    if (!(gUnk_03002458[gCurTask->unk88->unk00] & 65))
                        break;
                    if (gUnk_03002458[gCurTask->unk88->unk00] & 1)
                        sub_0803e34c(115, gCurTask->unk88->unk00);
                }
                break;
            case 1:
            case 2:
            case 5:
            case 19:
                while (1)
                {
                    TaskSetFrame(gCurTask->unk46);
                    TaskYieldTrampoline(2);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    TaskSetFrame((s16)(gCurTask->unk46 + 5));
                    TaskYieldTrampoline(2);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    TaskSetFrame((s16)(gCurTask->unk46 + 5));
                    TaskYieldTrampoline(2);
                    if (!(gUnk_03002458[gCurTask->unk88->unk00] & 65))
                        break;
                    if (gUnk_03002458[gCurTask->unk88->unk00] & 1)
                        sub_0803e34c(115, gCurTask->unk88->unk00);
                }
                break;
            }
            gCurTask->unk73 = 2;
            break;
        case 2:
            gCurTask->unk46 = gUnk_0873D7E4[gCurTask->unk88->unk0D][1];
            switch (gCurTask->unk88->unk0D)
            {
            case 0:
            default:
                while (1)
                {
                    TaskSetFrame(gCurTask->unk46);
                    TaskYieldTrampoline(6);
                    TaskSetFrame((s16)(gCurTask->unk46 + 5));
                    TaskYieldTrampoline(3);
                    TaskSetFrame((s16)(gCurTask->unk46 + 1));
                    TaskYieldTrampoline(6);
                    TaskSetFrame((s16)(gCurTask->unk46 + 5));
                    TaskYieldTrampoline(3);
                }
            case 1:
            case 2:
            case 5:
            case 19:
                while (1)
                {
                    TaskSetFrame(gCurTask->unk46);
                    TaskYieldTrampoline(3);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(3);
                    TaskSetFrame((s16)(gCurTask->unk46 + 10));
                    TaskYieldTrampoline(3);
                    TaskSetFrame((s16)(gCurTask->unk46 + 3));
                    TaskYieldTrampoline(3);
                    gCurTask->unk3C--;
                    TaskYieldTrampoline(3);
                    TaskSetFrame((s16)(gCurTask->unk46 + 9));
                    TaskYieldTrampoline(3);
                }
            }
        case 3:
            gCurTask->unk46 = gUnk_0873D7E4[gCurTask->unk88->unk0D][1];
            switch (gCurTask->unk88->unk0D)
            {
            case 0:
            default:
                while (1)
                {
                    TaskSetFrame(gCurTask->unk46);
                    TaskYieldTrampoline(4);
                    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 3; gCurTask->unk6C++)
                    {
                        gCurTask->unk3C++;
                        TaskYieldTrampoline(4);
                    }
                    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2; gCurTask->unk6C++)
                    {
                        gCurTask->unk3C--;
                        TaskYieldTrampoline(4);
                    }
                }
            case 1:
            case 2:
            case 5:
            case 19:
                while (1)
                {
                    TaskSetFrame(gCurTask->unk46);
                    TaskYieldTrampoline(2);
                    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 7; gCurTask->unk6C++)
                    {
                        gCurTask->unk3C++;
                        TaskYieldTrampoline(2);
                    }
                    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 5; gCurTask->unk6C++)
                    {
                        gCurTask->unk3C--;
                        TaskYieldTrampoline(2);
                    }
                }
            }
        case 4:
            if (gCurTask->unk88->unk06 == 2)
            {
                gCurTask->unk88->unk06 = 0;
                gCurTask->unk88->unk64 = (u32)gUnk_0873BD00;
                gCurTask->unk88->unk68 = (u32)gUnk_0873CB1C;
                sub_08053940(gCurTask->unk88->unk00, 0, 0);
                gCurTask->unk28++;
                sub_080413a4(2);
                TaskSetFrame(gUnk_0873D7E4[gCurTask->unk88->unk0D][2]);
                TaskYieldTrampoline(6);
                gCurTask->unk3C--;
                TaskYieldTrampoline(2);
            }
            else
            {
                TaskSetFrame(gUnk_0873D7E4[gCurTask->unk88->unk0D][2]);
                gCurTask->unk3C--;
                TaskYieldTrampoline(2);
            }
            if (sub_08040298() == 0)
            {
                if ((gUnk_03002458[gCurTask->unk88->unk00] & 64) && !(gCurTask->unk7B & 1))
                {
                    gCurTask->unk88->unk64 = (u32)gUnk_0873BD14;
                    gCurTask->unk73 = 0;
                    break;
                }
                gCurTask->unk73 = 6;
            }
            TaskSleepForever();
        case 5:
            gCurTask->unk46 = gUnk_0873D7E4[gCurTask->unk88->unk0D][1];
            switch (gCurTask->unk88->unk0D)
            {
            case 0:
            default:
                while (1)
                {
                    TaskSetFrame(gCurTask->unk46);
                    TaskYieldTrampoline(4);
                    sub_080413a4(9);
                    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 3; gCurTask->unk6C++)
                    {
                        gCurTask->unk3C++;
                        TaskYieldTrampoline(4);
                    }
                    sub_080413a4(10);
                    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 2; gCurTask->unk6C++)
                    {
                        gCurTask->unk3C--;
                        TaskYieldTrampoline(4);
                    }
                }
            case 1:
            case 2:
            case 5:
            case 19:
                while (1)
                {
                    sub_080413a4(9);
                    TaskSetFrame(gCurTask->unk46);
                    TaskYieldTrampoline(2);
                    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 5; gCurTask->unk6C++)
                    {
                        gCurTask->unk3C++;
                        TaskYieldTrampoline(2);
                    }
                    sub_080413a4(10);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    gCurTask->unk3C++;
                    TaskYieldTrampoline(2);
                    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 4; gCurTask->unk6C++)
                    {
                        gCurTask->unk3C--;
                        TaskYieldTrampoline(2);
                    }
                    gCurTask->unk58 = 0;
                    gCurTask->unk3C--;
                    TaskYieldTrampoline(2);
                }
            }
        }
    }
}
