/* game_code_and_rodata_080653ec_0806ef5c 0x080B9D0C-0x080BA774
 * (issue #95, module M35, file 1 of 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080B9D0C 0x080BA774 src/subgame_b9d0c.c --newpb
 *
 * The sub-game framework: the code AgbMain enters for its sub-game state
 * (sub_080ba354) and that every sub-game shares.  gUnk_02007FCC selects the
 * sub-game (0 = the reaction duel in this module, 1 = the four-slot
 * bomb-pass game of M36, 2 = the game whose body is in M37) and indexes the
 * per-game tables 0x087562A8 (graphics set), 0x087562C0 (BGM), 0x087562CC
 * (init hook) and 0x087562D8 (task body).  gUnk_02007D2C is the sub-game's
 * phase (0/1 running, 2-4 finished; the per-game body dispatches on it).
 *
 *   sub_080ba354   entry from AgbMain: link handshake, RNG warm-up, spawn
 *                  the task type #93 controller, run both screens
 *   sub_080b9f34   one screen: load, fade in, wait for the phase to leave
 *                  0/1, optional link resync (sub_080ba150), fade out, stop
 *                  DMA0 and hand the task over to sub_080ba404
 *   sub_080ba150   the SIO handshake: 0x7755 / 0xAA00 / 0xAA01 / 0xAA02
 *                  exchanged through gUnk_03005274 and the send/receive
 *                  buffers gUnk_03004D90 / gUnk_03004D50 until every
 *                  linked player reports 0xAA02
 *   sub_080ba404   task type #93: kill every other task, then run the
 *                  per-game body from 0x087562D8
 *   sub_080ba454.. the reaction duel's set-up and the helpers its round
 *                  controller (next file) calls.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern s32 gCurTaskIdx;
extern u8 gUnk_02004B5C;
extern s16 gUnk_020055EC;
extern s8 gUnk_02006168;
extern u8 gUnk_02006184;
extern u8 gUnk_02007D2C;
extern u8 gUnk_02007FCC;
extern u8 gUnk_0200B03C[];
extern u8 gUnk_0200B048;
extern u8 gUnk_0200B07C[];
extern u32 gUnk_0200EBA0;
extern u32 gUnk_0200EC48;
extern u32 gUnk_03000010;
extern u32 gUnk_0300003C;
extern vu16 gUnk_03000048;
extern vs32 gUnk_03000B78;
extern vs32 gUnk_03000F8C;
extern u32 gUnk_03000FA4;
extern vs32 gUnk_03000FA8;
extern vu16 gUnk_03000FAC;
extern vs32 gUnk_03000FC0;
extern u32 gUnk_0300117C;
extern vu8 gUnk_0300118C;
extern u16 gUnk_03001270[];
extern vu16 gUnk_03001E90;
extern vs32 gUnk_03001E94;    /* vs32 here (vu32 elsewhere): see sub_080b9f34 */
extern vu16 gUnk_03001EB8[];
extern vu16 gUnk_03001ED8;
extern vs32 gUnk_03001EE0;
extern vu16 gUnk_03001EEC;
extern vu16 gUnk_03001F38;
extern u16 gUnk_03002150;
extern u16 gUnk_03002360;
extern u16 gUnk_030023AC;
extern u16 gUnk_030023D8;
extern vu16 gUnk_0300243C;
extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];
extern vs16 gUnk_03004CA0[];
extern u32 gUnk_03004D24;
extern u16 gUnk_03004D50[];
extern u32 gUnk_03004D70;
extern u32 gUnk_03004D7C;
extern u16 gUnk_03004D88[];
extern u16 gUnk_03004D90[];
extern u16 gUnk_03005274;     /* SIO handshake word; see sub_080ba150 */
extern u32 gUnk_087562A8[][2];
extern u16 gUnk_087562C0[];
extern s32 (*const gUnk_087562CC[])(void);
extern void *gUnk_087562D8[];
extern u16 gUnk_087562E4[];
extern u32 gUnk_087562E8[];
extern s16 gUnk_087562F0[];
extern s16 gUnk_087562F6[];

void TaskYieldTrampoline(u32 frames);
void sub_08000de4(void);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_08002028(void);
void sub_0800214c(void);
void sub_080021dc(void);
void sub_080022ac(void);
void sub_080022d0(void);
void sub_080022e4(void);
void sub_080022fc(void);
void sub_08002338(void);
void sub_08002358(void);
void sub_08002378(void);
void sub_08002d18(void);
void sub_08002e98(u32 a, u32 b, u32 *c);
u32 sub_08002ec0(void);
u32 sub_08002ee8(u32 range);
s32 sub_080031b8(s32 id);
void sub_080034d0(void);
void sub_080055b0(u8 val, s32 idx);
void sub_080055c4(u16 val, s32 idx);
void sub_08005654(s32 id);
s32 sub_080058e4(u32 type, s32 idx);
void sub_08006138(void);
void sub_08006148(void *fn, u32 i);
void sub_08006868(void);
void sub_08006914(void *cmd, void *send, void *recv);
u32 sub_080071dc(void);
u32 sub_080072e0(void);
void sub_08008b8c(void);
void sub_08008c4c(u32 a);
void sub_08008c64(u16 a);
void sub_08008d10(u32 a, u32 b);
void sub_080c1f88(void);
void sub_080c59d8(s32 a, s32 b);

void sub_080ba134(void);
void sub_080ba118(void);
void sub_080ba150(void);
void sub_080ba404(void);
void sub_080ba42c(void);

void sub_080b9d0c(s32 a0)
{
    gUnk_02006168 = a0;
    gUnk_03002490->unk18 = 3;
}

void sub_080b9d24(void)
{
    if (gUnk_03002150 == 4)
        gUnk_03004D24 = 0;
    gUnk_03002490->unk18 = 4;
}

s32 sub_080b9d48(void)
{
    return gUnk_087562CC[gUnk_02007FCC]();
}

u8 sub_080b9d68(void)
{
    s32 found = 0;
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        if (gUnk_03001EB8[i] & 9)
        {
            found = 1;
            break;
        }
    }
    return found;
}

u8 sub_080b9da8(void)
{
    s32 found = 0;
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        if (gUnk_03001EB8[i] & 2)
        {
            found = 1;
            break;
        }
    }
    return found;
}

void sub_080b9de8(void)
{
    s32 i;

    sub_080055b0(7, gUnk_020055EC);
    gUnk_03001ED8 |= 0x200;
    gUnk_0300118C = 0xFD;
    for (i = 0; i <= 4; i++)
    {
        gUnk_03001EEC = i;
        sub_080ba134();
    }
    while (1)
        sub_080ba134();
}

void sub_080b9e30(void)
{
    if (gUnk_02007D2C <= 1)
    {
        s32 v = gUnk_03002490->unk18;
        if (v != 0)
            gUnk_02007D2C = v;
    }
}

void sub_080b9e50(s32 a0)
{
    s32 m = gUnk_02007FCC;

    sub_08008c4c(gUnk_087562A8[m][a0]);
    m = m * 2 + a0;
    if (gUnk_087562C0[m] != 0)
        sub_08008c64(gUnk_087562C0[m]);
    sub_08008d10(gUnk_02007FCC, a0);
    gUnk_02007D2C = a0;
}

void sub_080b9ea0(s32 a0)
{
    switch (gUnk_02007FCC)
    {
    case 0:
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1400;
        break;
    case 1:
        gUnk_03001ED8 &= 0xE0FF;
        gUnk_03001ED8 |= 0x1800;
        break;
    case 2:
        if (a0 != 0)
        {
            gUnk_03001ED8 &= 0xE0FF;
            gUnk_03001ED8 |= 0x1C00;
        }
        else
        {
            gUnk_03001ED8 &= 0xE0FF;
            gUnk_03001ED8 |= 0x1F00;
        }
        break;
    }
}

void sub_080b9f34(s32 a0)
{
    s32 i;

    sub_08002028();
    sub_080b9e50(a0);
    if (a0 != 0 || gUnk_02007FCC != 2)
    {
        gUnk_0300117C = gUnk_03001EE0 = gUnk_03000F8C = gUnk_03000B78 = 0;
        /* Every link of a volatile chain is re-read after its store, but
           only while neighbouring links have the same type: a signedness
           change wraps the inner assignment in a conversion that fold()
           turns into `(y = 0, (T)0)`, dropping the re-read.  The ROM
           re-reads gUnk_03000FA8, so gUnk_03001E94 is vs32 in this file. */
        gUnk_03000010 = gUnk_03000FC0 = gUnk_03001E94 = gUnk_03000FA8 = 0;
    }
    else
    {
        sub_080c59d8(gUnk_02006168, 1);
        sub_080c1f88();
    }
    sub_08002358();
    sub_08002378();
    sub_080ba134();
    sub_080022fc();
    sub_080b9ea0(a0);
    if (gUnk_02007FCC != 2)
    {
        sub_0800214c();
        while (gUnk_03001E90 != 0)
            sub_080ba118();
        goto wait;
        /* The ROM places this call between the two arms: a labelled
           block reached from the phase test below (lesson 4.67). */
    de8:
        sub_080b9de8();
        goto tail;
    }
    else
    {
        gUnk_0300118C = 0xBF;
        gUnk_03001ED8 &= 0xFF7F;
        for (i = 16; i >= 0; i--)
        {
            gUnk_03001EEC = i;
            sub_080ba118();
        }
    }
wait:
    gUnk_03000048 = 0;
    /* goto loop, not do/while: the ROM re-loads the cell's address every
       iteration, and a loop note would hoist it (lesson 3.21). */
loop:
    sub_080ba118();
    if (gUnk_02007D2C <= 1)
        goto loop;
    if (gUnk_02007D2C == 4 && gUnk_03002150 == 4)
    {
        if (gUnk_0200EC48 == 2)
            goto de8;
        sub_080ba150();
    }
tail:
    sub_08002338();
    if (gUnk_02007FCC != 2)
    {
        sub_080021dc();
        while (gUnk_03001E90 != 0)
            sub_080ba118();
    }
    else
    {
        gUnk_0300118C = 0xBF;
        for (i = 0; i <= 16; i++)
        {
            gUnk_03001EEC = i;
            sub_080ba118();
        }
        sub_08002028();
        gUnk_03001ED8 |= 0x80;
        sub_08000de4();
    }
    gUnk_03000048 = 0;
    gUnk_0300003C = gUnk_03000FA4 = 0;
    REG_DMA0CNT_L = REG_DMA0CNT_H = 0;
    sub_08006148(sub_080ba404, gUnk_020055EC);
}

void sub_080ba118(void)
{
    sub_08002d18();
    if (gUnk_02007FCC == 2)
        sub_080c1f88();
}

void sub_080ba134(void)
{
    sub_080022e4();
    if (gUnk_02007FCC == 2)
        sub_080c1f88();
}

void sub_080ba150(void)
{
    s32 n;
    u32 stall;
    s32 timer;
    s32 i;
    u32 old;

    if (gUnk_03001F38 != 0)
        gUnk_03005274 = 0x7755;
    else
        gUnk_03005274 = 0x9900;
    if (gUnk_0300243C <= 1)
        return;
    n = 0;
    stall = 0;
    timer = 0;     /* nothing ever sets it non-zero, but the ROM keeps it */
    for (;;)
    {
        /* gUnk_03005274 is NOT volatile here: a vu16 switch operand costs a
           register copy the ROM does not have, and the cell is re-read every
           iteration anyway because the loop calls out. */
        switch (gUnk_03005274)
        {
        case 0x7755:
            gUnk_03004D90[0] = 0x7755;
            gUnk_03005274 = 0xAA00;
            break;
        case 0xAA00:
            timer = 0;
            gUnk_03004D90[0] = 0xAA00;
            gUnk_03005274 = 0x9900;
            break;
        case 0xAA01:
            gUnk_03004D90[0] = 0xAA01;
            gUnk_03005274 = 0x9900;
            break;
        case 0xAA02:
            gUnk_03004D90[0] = 0xAA02;
            break;
        }
        old = gUnk_03004D7C;
        sub_080022e4();
        sub_08006914(gUnk_03004D88, gUnk_03004D90, gUnk_03004D50);
        if (sub_080072e0() != 0)
            sub_08008b8c();
        if (old == gUnk_03004D7C && ++stall > 30)
            sub_08008b8c();
        for (i = 0; i <= 3; i++)
        {
            switch (gUnk_03004D50[i])
            {
            case 0x7755:    /* empty, but it roots the tree at 0xAA00 (3.42) */
                break;
            case 0xAA00:
                gUnk_03005274 = 0xAA01;
                break;
            case 0xAA01:
                if (gUnk_03001F38 != 0 && ++n >= gUnk_0300243C)
                    gUnk_03005274 = 0xAA02;
                break;
            case 0xAA02:
                goto done;
            }
        }
        if (timer != 0)
        {
            if (n == gUnk_0300243C)
                timer = 0;
            else if (--timer == 0)
            {
                gUnk_03005274 = 0xAA00;
                n = 0;
            }
        }
        if (gUnk_02007FCC == 2)
            sub_080c1f88();
    }
done:
    if (gUnk_03001F38 != 0)
        gUnk_0200EBA0 = 0;
    for (i = 4; i >= 0; i--)
        sub_080ba134();
    sub_08006868();
    gUnk_03004D70 = 0;
}

s32 sub_080ba31c(void)
{
    s32 i;

    for (i = 0; i <= 62; i++)
    {
        if (gUnk_03004CA0[i] != -1 && i != gCurTaskIdx)
            sub_08005654(i);
    }
}

void sub_080ba354(void)
{
    s32 i;

    sub_080022ac();
    if (gUnk_0200EC48 == 2 && sub_080071dc() != 0)
        return;
    sub_080022d0();
    sub_080022d0();
    if (gUnk_03002360 == 0)
    {
        for (i = 0; i < (gUnk_03000FAC & 0xFF); i++)
            sub_08002ec0();
    }
    sub_08002358();
    sub_08002378();
    gUnk_020055EC = sub_080058e4(93, 63);
    sub_080b9d48();
    sub_080b9f34(0);
    sub_080b9f34(1);
    sub_080022ac();
    if (gUnk_02007D2C != 3)
    {
        gUnk_030023D8 = gUnk_03002150;
        gUnk_03002150 = gUnk_02007FCC + 14;
    }
}

void sub_080ba404(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_080ba42c;
    t->unk18 = 0;
    sub_080ba31c();
    sub_08006138();
}

void sub_080ba42c(void)
{
    sub_08006148(gUnk_087562D8[gUnk_02007FCC], gCurTaskIdx);
}

void sub_080ba454(void)
{
    struct Task *t = &gUnk_03002790[gUnk_020055EC];
    s32 i;

    for (i = 0; i <= 3; i++)
    {
        gUnk_0200B03C[i] = 0;
        gUnk_0200B07C[i] = i;
    }
    gUnk_02004B5C = 0xFF;
    gUnk_02006184 = 99;
    gUnk_0200B048 = 0;
    t->unk34 = 3;
    sub_080017e4(2, (u32)gUnk_087562E4, (u32)gUnk_03001270, 2);
}

void sub_080ba4e0(void)
{
    gUnk_03002490->unk04 = 0;
    sub_08002e98(gUnk_02007D2C, 2, gUnk_087562E8);
    sub_08006138();
}

void sub_080ba50c(void)
{
    struct Task *t;

    sub_080055c4(31, gCurTaskIdx);
    if (gUnk_03004CA0[62] != -1)
        sub_08005654(62);
    gUnk_03001ED8 &= 0xE0FF;
    gUnk_03001ED8 |= 0x1000;
    t = &gUnk_03002790[gUnk_03002490->unk28];
    t->unk1C = 0;
}

void sub_080ba578(void)
{
    s32 idx = sub_080058e4(94, 32);

    if (idx != -1)
    {
        struct Task *t = &gUnk_03002790[idx];

        t->unk44 = gCurTaskIdx;
        t->unk73 = 2;
        gUnk_03002490->unk28 = idx;
    }
}

void sub_080ba5bc(s32 a0)
{
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
    {
        s32 idx = sub_080058e4(94, 0);

        if (idx != -1)
        {
            struct Task *t = &gUnk_03002790[idx];

            t->unk44 = gCurTaskIdx;
            t->unk18 = idx;
            t->unk73 = 0;
            t->unk74 = a0;
            t->unk28 = 0;
        }
    }
}

void sub_080ba61c(void)
{
    struct Task *t;

    sub_080ba5bc(0);
    sub_080ba578();
    t = gUnk_03002490;
    t->unk24 = 0;
    t->unk75 = 0;
}

void sub_080ba63c(void)
{
    s32 idx = sub_080058e4(94, 62);

    if (idx != -1)
    {
        struct Task *t = &gUnk_03002790[idx];

        t->unk73 = 1;
        t->unk18 = 0;
        t->unk1C = 3;
        t->unk20 = 16;
        t->unk24 = -1;
        t->unk48 = 120;
        t->unk4A = 88;
        t->unk34 = 0;
    }
}

void sub_080ba688(void)
{
    struct Task *t;

    sub_080031b8(234);
    sub_080034d0();
    t = &gUnk_03002790[gUnk_03002490->unk28];
    t->unk1C = 1;
}

void sub_080ba6b4(void)
{
    struct Task *t = gUnk_03002490;

    t->unk70 = 0;
    t->unk6E = 0;
    TaskYieldTrampoline(sub_08002ee8(gUnk_087562F6[gUnk_02006168]) + gUnk_087562F0[gUnk_02006168]);
    sub_080ba63c();
}

s32 sub_080ba708(void)
{
    s32 mask = 0;
    s32 i;
    s32 n;

    for (i = 0, n = 0; i < gUnk_030023AC; i++)
    {
        if ((gUnk_03001EB8[i] & 1) && !((gUnk_03002490->unk6E >> i) & 1))
        {
            n++;
            mask |= 1 << i;
        }
    }
    gUnk_03002490->unk2C = mask;
    return n;
}
