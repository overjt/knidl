/* game_code_and_rodata_080653ec_0806ef5c 0x080BC0CC-0x080BD9E8
 * (issue #95, module M35, file 4 of 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BC0CC 0x080BD9E8 src/subgame_bc0cc.c --newpb
 *
 * Task type #94, the duel's sprite objects.  sub_080bc0cc dispatches the
 * table 0x087563B0 on Task.unk73, the kind its spawner wrote (ten function
 * pointers; the call passes 12):
 *
 *   0  sub_080bc850  a player: a six-state machine over 0x08756468 (entry
 *                    coroutines) / 0x08756480 (per-frame hooks), started in
 *                    state 0, 1 or 5 by Task.unk74
 *   1  sub_080bcdac  a label / icon sprite (draw callback sub_080bcbfc)
 *   2  sub_080bcf60  a two-digit counter (sub_080bce28 counts to 99 and
 *                    mirrors the value into the parent's Task.unk20)
 *   3  sub_080bcfa4  a scripted fly-in
 *   4  sub_080bd06c  a four-frame effect at (120, 96)
 *   5  sub_080bd4bc  the single-player opponent sub_080bb074 spawns: a
 *                    five-state machine over 0x087564E4 / 0x087564FC, one
 *                    animation set per level Task.unk18 (0-4) and its
 *                    reaction time from 0x087564B0[unk74 * 5 + unk18]; own
 *                    graphics loader sub_080bd188, draw callback sub_080bd938
 *   6  sub_080bd110  a three-frame sprite
 *   7  sub_080bd7f0  a sprite drawn by sub_08005ca0 (0x08755B90)
 *   8  sub_080bd8ac  the same graphics as a per-player award: in
 *                    gUnk_03002150 == 5, sub_080bd828 passes 1000 / 3000 /
 *                    5000 or 10 to sub_0800a0ac, or 1 to sub_08009eb8
 *   9  sub_080bd9b0  a sprite drawn by sub_080bd938 (0x08755A68)
 *
 * sub_080bc1c4 places a player by the number of linked players and the
 * local id (x from 0x087563D8, then y / facing / animation from x);
 * sub_080bc0ec / sub_080bc168 slide it in and out.  gUnk_03000FB8 is a busy
 * counter the scripts wait on until it reaches 0.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

struct GfxDesc
{
    u16 unk00;
    u16 unk02;
    u32 unk04;
    u32 unk08;
    const void *unk0C;
};

extern s32 gCurTaskIdx;
extern u8 gUnk_0200B03C[];
extern u8 gUnk_0200B048;
extern u32 gUnk_02020000[];
extern vs16 gUnk_03000FB8;
extern u32 gUnk_03001570[];
extern u16 gUnk_03002150;
extern u16 gUnk_03002360;
extern u16 gUnk_030023AC;
extern struct Task *gUnk_03002490;
extern struct Task gUnk_03002790[];
extern u32 gUnk_087559E4[];
extern u32 gUnk_087559F4[];
extern u32 gUnk_08755A04[];
extern u32 gUnk_08755A14[];
extern u32 gUnk_08755A24[];
extern u32 gUnk_08755A34[];
extern u32 gUnk_08755A5C[];
extern u32 gUnk_08755A68[];
extern u32 gUnk_08755A78[];
extern u32 gUnk_08755A7C[];
extern u32 gUnk_08755A88[];
extern u32 gUnk_08755AA4[];
extern u32 gUnk_08755AB8[];
extern u32 gUnk_08755AC8[];
extern u32 gUnk_08755AD8[];
extern u32 gUnk_08755ADC[];
extern u32 gUnk_08755AF0[];
extern u32 gUnk_08755B18[];
extern u32 gUnk_08755B40[];
extern u32 gUnk_08755B68[];
extern u32 gUnk_08755B90[];
extern u32 gUnk_087563B0[];
extern u16 gUnk_087563D8[];
extern u16 gUnk_08756410[];
extern s16 gUnk_08756448[];
extern s16 gUnk_08756450[];
extern s16 gUnk_08756458[];
extern s16 gUnk_08756460[];
extern u32 gUnk_08756468[];
extern u32 gUnk_08756480[];
extern u16 gUnk_08756498[];
extern u32 gUnk_087564A0[];
extern s16 gUnk_087564B0[];
extern struct GfxDesc *const gUnk_087564D0[];
extern u32 gUnk_087564E4[];
extern u32 gUnk_087564FC[];
extern u16 gUnk_08756514[];
extern u16 gUnk_0875651C[];

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(u32 frames);
void LZ77UnCompWram(const void *src, void *dest);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
s32 sub_08001a94(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void sub_08002e98(u32 a, u32 b, u32 *c);
s32 sub_080031b8(s32 id);
void sub_08005654(s32 id);
s32 sub_080058e4(u32 type, s32 idx);
void sub_080059d8(void);
void sub_08005ca0(void);
void sub_08006138(void);
void sub_08006148(void *fn, u32 i);
void sub_080062c4(void);
void sub_08009eb8(u32 a, u32 b);
void sub_0800a0ac(u32 a, u32 b);

void sub_080bc1c4(void);
void sub_080bc8c4(void);
void sub_080bc8a8(void);
void sub_080bcf8c(void);
void sub_080bd524(void);
void sub_080bd290(void);
void sub_080bd508(void);

void sub_080bc0cc(void)
{
    sub_08002e98(gUnk_03002490->unk73, 12, gUnk_087563B0);
}

void sub_080bc0ec(void)
{
    struct Task *t;
    struct Task *u;

    if (gUnk_030023AC <= 2)
    {
        t = gUnk_03002490;
        switch (t->unk4A)
        {
        case 120:
            if (t->unk48 > 67)
            {
                t->unk48 = 68;
                t->unk24 = 1;
            }
            else
                t->unk48 += 10;
            break;
        case 56:
            if (t->unk48 <= 164)
            {
                t->unk48 = 164;
                t->unk24 = 1;
            }
            else
                t->unk48 -= 10;
            break;
        }
    }
    u = gUnk_03002490;
    u->unk4C = u->unk48 << 16;
    u->unk50 = u->unk4A << 16;
}

void sub_080bc168(void)
{
    struct Task *t;

    sub_080bc1c4();
    if (gUnk_030023AC <= 2)
    {
        switch (gUnk_03002490->unk48)
        {
        case 68:
            gUnk_03002490->unk48 = -44;
            break;
        case 164:
            gUnk_03002490->unk48 = 276;
            break;
        }
        t = gUnk_03002490;
        t->unk24 = 0;
        t->unk4C = t->unk48 << 16;
        t->unk50 = t->unk4A << 16;
    }
}

void sub_080bc1c4(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;
    s32 n;
    s32 k;

    t->unk3E &= 0x7FFF;
    t->unk40 = t->unk18 << 12;
    n = gUnk_030023AC - 1;
    k = gUnk_03002360 - t->unk18;
    t->unk48 = gUnk_087563D8[n * 8 + k];
    t->unk42 = 11;
    switch (gUnk_03002490->unk48)
    {
    case 68:
        gUnk_03002490->unk4A = 120;
        gUnk_03002490->unk43 = 1;
        gUnk_03002490->unk38 = gUnk_08755AF0;
        gUnk_03002490->unk42 = 8;
        break;
    case 164:
        gUnk_03002490->unk4A = 56;
        gUnk_03002490->unk43 = -1;
        gUnk_03002490->unk38 = gUnk_08755B68;
        gUnk_03002490->unk42 = 11;
        break;
    case 188:
        gUnk_03002490->unk4A = 104;
        gUnk_03002490->unk43 = -1;
        gUnk_03002490->unk38 = gUnk_08755B40;
        gUnk_03002490->unk42 = 9;
        break;
    case 60:
        gUnk_03002490->unk4A = 72;
        gUnk_03002490->unk43 = 1;
        gUnk_03002490->unk38 = gUnk_08755B18;
        gUnk_03002490->unk42 = 10;
        break;
    default:
        gUnk_03002490->unk4A = 0;
        gUnk_03002490->unk43 = 1;
        gUnk_03002490->unk38 = gUnk_08755AF0;
        gUnk_03002490->unk42 = 12;
        break;
    }
    u = gUnk_03002490;
    u->unk4C = u->unk48 << 16;
    u->unk50 = u->unk4A << 16;
    u->unk3C = 0;
    u->unk30 = -1;
    u->unk34 = -1;
    u->unk2C = u->unk48;
}

void sub_080bc30c(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;
    s32 v = t->unk20;

    if (v == 4)
    {
        t->unk48 = 120;
        t->unk4A = 96;
    }
    else
    {
        s32 n = gUnk_030023AC - 1;
        s32 k = gUnk_03002360 - v;

        t->unk48 = gUnk_08756410[n * 8 + k];
        switch (t->unk48)
        {
        case 120:
            t->unk4A = 112;
            break;
        case 136:
            t->unk4A = 80;
            break;
        case 144:
            t->unk4A = 104;
            break;
        case 112:
            t->unk4A = 96;
            break;
        default:
            gUnk_03002490->unk4A = 0;
            break;
        }
    }
    u = gUnk_03002490;
    u->unk4C = u->unk48 << 16;
    u->unk50 = u->unk4A << 16;
    u->unk3C = 1;
}

void sub_080bc3c8(void)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk2C)
    {
    case 68:
        t->unk54 = -0x20000;
        t->unk58 = -0x50000;
        t->unk60 = 0x4000;
        t->unk68 = 0x50000;
        break;
    case 164:
        t->unk54 = 0x20000;
        t->unk58 = -0x50000;
        t->unk60 = 0x3000;
        t->unk68 = 0x80000;
        break;
    case 188:
        t->unk54 = 0x40000;
        t->unk58 = -0x50000;
        t->unk60 = 0x3000;
        t->unk68 = 0x50000;
        break;
    case 60:
        t->unk54 = -0x40000;
        t->unk58 = -0x50000;
        t->unk60 = 0x3000;
        t->unk68 = 0x50000;
        break;
    default:
        sub_080062c4();
        break;
    }
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
}

void sub_080bc460(void)
{
    struct Task *t = gUnk_03002490;

    switch (t->unk2C)
    {
    case 68:
        t->unk20 = 0;
        break;
    case 164:
        t->unk20 = 3;
        break;
    case 188:
        t->unk20 = 1;
        break;
    case 60:
        t->unk20 = 2;
        break;
    default:
        gUnk_03002490->unk20 = 0;
        break;
    }
    sub_080031b8(256);
}

void sub_080bc4b0(void)
{
    s32 id = sub_080058e4(94, 32);

    if (id != -1)
    {
        struct Task *t = &gUnk_03002790[id];
        struct Task *p;

        t->unk73 = 1;
        p = gUnk_03002490;
        t->unk18 = p->unk20;
        t->unk1C = 6;
        t->unk20 = -1;
        t->unk24 = -1;
        t->unk48 = p->unk48 + gUnk_08756448[p->unk20] * p->unk43;
        t->unk4A = p->unk4A - gUnk_08756450[p->unk20];
        t->unk34 = 0;
    }
    gUnk_03002490->unk34 = id;
}

void sub_080bc54c(void)
{
    s32 id = sub_080058e4(94, 32);

    if (id != -1)
    {
        struct Task *t = &gUnk_03002790[id];
        struct Task *p;

        t->unk73 = 6;
        p = gUnk_03002490;
        t->unk18 = p->unk20;
        t->unk48 = p->unk48 + gUnk_08756458[p->unk20];
        t->unk4A = p->unk4A - gUnk_08756460[p->unk20];
    }
    gUnk_03002490->unk30 = id;
}

void sub_080bc5cc(void)
{
    s32 idx = sub_080058e4(94, 32);

    if (idx != -1)
    {
        struct Task *t = gUnk_03002490;
        s16 x = t->unk48 - 24;
        s16 y = t->unk4A - 32;
        struct Task *n;
        struct Task *u;

        /* The ROM keeps a dead `ldrsh` of t->unk48 here (same artifact as
           sub_080bb874): a switch whose arms all reduce to no-ops.  Two
           labels reproduce the allocation; one or four do not. */
        switch (t->unk48)
        {
        case 120:
            x = x;
            break;
        case 56:
            x = x;
            break;
        }
        n = &gUnk_03002790[idx];
        n->unk73 = 1;
        u = gUnk_03002490;
        n->unk18 = u->unk18;
        n->unk1C = 0;
        n->unk20 = 60;
        n->unk24 = -1;
        n->unk48 = x;
        n->unk4A = y;
        if (gUnk_03002360 == u->unk18)
        {
            n->unk18 = 4;
            n->unk4A = u->unk4A - 32;
            n->unk48 += 12;
        }
        n->unk34 = 0;
        n->unk42 = gUnk_03002490->unk42 - 8;
    }
    gUnk_03002490->unk1C = 60;
}

void sub_080bc680(void)
{
    s32 idx = sub_080058e4(94, 32);

    if (idx != -1)
    {
        struct Task *t = gUnk_03002490;
        s16 x = t->unk48 - 24;
        s16 y = t->unk4A - 32;
        struct Task *n;

        /* Dead `ldrsh` of t->unk48 in the ROM: a switch whose arms are all
           no-ops (same artifact as sub_080bb874). */
        switch (t->unk48)
        {
        case 120:
            x = x;
            break;
        case 56:
            x = x;
            break;
        case 104:
            x = x;
            break;
        case 72:
            x = x;
            break;
        }
        n = &gUnk_03002790[idx];
        n->unk73 = 1;
        n->unk18 = gUnk_0200B03C[gUnk_03002490->unk18];
        n->unk1C = 2;
        n->unk20 = 60;
        n->unk24 = -1;
        n->unk48 = x + 4;
        n->unk4A = y;
    }
    gUnk_03002490->unk1C = 60;
}

void sub_080bc70c(void)
{
    if (gCurTaskIdx == 0)
    {
        struct Task *p = &gUnk_03002790[gUnk_03002490->unk44];
        p->unk24 = 1;
    }
    gUnk_03002490->unk14 = 1;
}

void sub_080bc740(s32 a0, u16 a1)
{
    struct Task *t = &gUnk_03002790[a0];
    struct Task *p = &gUnk_03002790[t->unk44];

    if (p->unk18 != 2)
    {
        t->unk14 = a1;
        sub_08006148(sub_080bc8c4, a0);
        if (t->unk34 != -1)
            sub_08005654(t->unk34);
        if (t->unk30 != -1)
            sub_08005654(t->unk30);
        t->unk30 = -1;
        t->unk34 = -1;
    }
}

void sub_080bc79c(u16 a0)
{
    s32 i;

    for (i = 0; i < gUnk_030023AC; i++)
        sub_080bc740(i, a0);
}

u8 sub_080bc7c8(void)
{
    struct Task *t = gUnk_03002490;
    s16 x = t->unk48;
    s16 y = t->unk4A;

    if (x > -64 && x < 304 && y > -64 && y < 224)
        return 1;
    return 0;
}

void sub_080bc800(void)
{
    gUnk_03002490->unk40 = gUnk_03002490->unk18 << 12;
    gUnk_03002490->unk38 = gUnk_08755B18;
    gUnk_03002490->unk42 = 8;
    gUnk_03002490->unk3C = 0;
    gUnk_03002490->unk48 = 120;
    gUnk_03002490->unk4A = 68;
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
}

void sub_080bc850(void)
{
    struct Task *t = gUnk_03002490;

    t->unk0C = (u32)sub_08005ca0;
    t->unk04 = (u32)sub_080bc8a8;
    t->unk42 = 7;
    t = gUnk_03002490;
    switch (t->unk74)
    {
    case 0:
        t->unk14 = 0;
        break;
    case 1:
        t->unk14 = 1;
        break;
    case 2:
        t->unk14 = 5;
        break;
    }
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_08756468);
    sub_08006138();
}

void sub_080bc8a8(void)
{
    sub_08002e98(gUnk_03002490->unk15, 6, gUnk_08756480);
}

void sub_080bc8c4(void)
{
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_08756468);
}

void sub_080bc8e0(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080bc168();
    while (gUnk_03000FB8 != 0)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(60);
    sub_080bc5cc();
    TaskYieldTrampoline(gUnk_03002490->unk1C);
    if (gUnk_030023AC != 1)
    {
        sub_080bc680();
        TaskYieldTrampoline(gUnk_03002490->unk1C);
    }
    sub_080bc70c();
    sub_08006138();
}

void sub_080bc948(void)
{
    if (gUnk_03000FB8 == 0 && gUnk_03002490->unk24 == 0)
        sub_080bc0ec();
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_080bc8c4, gCurTaskIdx);
}

void sub_080bc988(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080bc1c4();
    while (gUnk_03000FB8 != 0)
        TaskYieldTrampoline(1);
    sub_08006138();
}

void sub_080bc9c0(void)
{
}

void sub_080bc9c4(void)
{
    struct Task *t;
    u16 saved;
    s16 x;
    s32 i;

    gUnk_03002490->unk15 = 2;
    sub_080bc30c();
    saved = gUnk_03002490->unk48;
    x = gUnk_03002490->unk48;
    for (i = 0; i < 2; i++)
    {
        t = gUnk_03002490;
        t->unk48 = x + gUnk_08756498[0] * (u16)t->unk43;
        t->unk4C = t->unk48 << 16;
        TaskYieldTrampoline(1);
        t = gUnk_03002490;
        t->unk48 = x - gUnk_08756498[0] * (u16)t->unk43;
        t->unk4C = t->unk48 << 16;
        TaskYieldTrampoline(1);
    }
    gUnk_03002490->unk48 = saved;
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    sub_08006138();
}

void sub_080bca68(void)
{
}

void sub_080bca6c(void)
{
    s32 i;

    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk15 = 3;
    sub_080bc3c8();
    while (1)
    {
        gUnk_03002490->unk3C = 2;
        TaskYieldTrampoline(5);
        for (i = 0; i < 7; i++)
        {
            gUnk_03002490->unk3C++;
            TaskYieldTrampoline(5);
        }
    }
}

void sub_080bcaac(void)
{
    if (gUnk_03002490->unk54 != 0 && gUnk_03002490->unk58 != 0)
    {
        u8 r = sub_080bc7c8();
        if (r == 0)
        {
            sub_080062c4();
            gUnk_03002490->unk00 = r;
        }
    }
}

void sub_080bcadc(void)
{
    struct Task *t;
    struct Task *p;
    u16 saved;
    s16 x;
    s32 i;

    gUnk_03002490->unk15 = 4;
    sub_080bc460();
    saved = gUnk_03002490->unk48;
    x = gUnk_03002490->unk48;
    for (i = 0; i < 2; i++)
    {
        t = gUnk_03002490;
        t->unk48 = x + gUnk_08756498[t->unk20] * (u16)t->unk43;
        t->unk4C = t->unk48 << 16;
        TaskYieldTrampoline(1);
        t = gUnk_03002490;
        t->unk48 = x - gUnk_08756498[t->unk20] * (u16)t->unk43;
        t->unk4C = t->unk48 << 16;
        TaskYieldTrampoline(1);
    }
    gUnk_03002490->unk48 = saved;
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    sub_080bc4b0();
    sub_080bc54c();
    TaskYieldTrampoline(30);
    p = &gUnk_03002790[gUnk_03002490->unk44];
    p->unk75 = 1;
    sub_08006138();
}

void sub_080bcbbc(void)
{
}

void sub_080bcbc0(void)
{
    gUnk_03002490->unk15 = 5;
    sub_080bc800();
    while (gUnk_03000FB8 != 0)
        TaskYieldTrampoline(1);
    sub_08006138();
}

void sub_080bcbf8(void)
{
}

void sub_080bcbfc(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;
    u32 *tbl = t->unk38;

    if (tbl != NULL && t->unk3C != -1 && (u16)(t->unk48 + 63) <= 366
        && t->unk4A > -64 && t->unk4A < 224)
    {
        sub_08001a94(t->unk42, tbl[t->unk3C], t->unk3E, t->unk40, t->unk48, t->unk4A);
        u = gUnk_03002490;
        if (u->unk24 != -1)
        {
            tbl = (u32 *)u->unk30;
            sub_08001a94(u->unk42, tbl[u->unk24], u->unk3E, u->unk34,
                         u->unk48 + u->unk28, (s16)(u->unk4A + u->unk2C));
        }
    }
}

void sub_080bccbc(void)
{
    struct Task *t;
    u8 *p = &gUnk_03002490->unk42;

    if (*p == 0)
        *p = 4;
    else
        *p += 4;
    switch (gUnk_03002490->unk1C)
    {
    case 0:
        gUnk_03002490->unk38 = gUnk_08755AA4;
        break;
    case 1:
        gUnk_03002490->unk38 = gUnk_08755A78;
        break;
    case 2:
        gUnk_03002490->unk38 = gUnk_08755A5C;
        break;
    case 3:
        gUnk_03002490->unk38 = gUnk_08755A7C;
        break;
    case 4:
        gUnk_03002490->unk38 = gUnk_08755A88;
        break;
    case 5:
        gUnk_03002490->unk38 = gUnk_08755A34;
        break;
    case 6:
        gUnk_03002490->unk38 = gUnk_08755AB8;
        break;
    case 7:
        gUnk_03002490->unk38 = gUnk_08755AC8;
        break;
    }
    t = gUnk_03002490;
    t->unk40 |= 0x800;
    t->unk34 = t->unk40;
    if (t->unk20 != -1)
    {
        t->unk3C = t->unk18;
        TaskYieldTrampoline(t->unk20);
    }
    else
    {
        t->unk3C = t->unk18;
    }
}

void sub_080bcdac(void)
{
    gUnk_03002490->unk0C = (u32)sub_080bcbfc;
    sub_080bccbc();
    if (gUnk_03002490->unk20 != -1)
        TaskDispatchTrampoline();
    else
        sub_08006138();
}

void sub_080bcde0(void)
{
    struct Task *t;

    gUnk_03002490->unk12 = 4;
    t = gUnk_03002490;
    t->unk38 = gUnk_08755A34;
    t->unk3C = 0;
    t->unk24 = 0;
    t->unk48 = 206;
    t->unk4A = 132;
    t->unk28 = -8;
    t->unk2C = 0;
    t->unk1C = 0;
    t->unk18 = 0;
    t->unk40 |= 0x800;
}

void sub_080bce28(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *p = &gUnk_03002790[t->unk44];

    if (t->unk18 <= 98)
    {
        s32 r;
        s32 q;

        t->unk18++;
        p->unk20 = t->unk18;
        r = t->unk18;
        q = 0;
        while (r > 9)
        {
            r -= 10;
            q++;
        }
        gUnk_03002490->unk24 = q;
        gUnk_03002490->unk3C = r;
    }
}

void sub_080bce74(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;
    struct Task *v;
    u32 *tbl = t->unk38;

    if (tbl != NULL && t->unk3C != -1 && (u16)(t->unk48 + 63) <= 366
        && t->unk4A > -64 && t->unk4A < 224)
    {
        sub_08001a94(t->unk42, tbl[t->unk3C], t->unk3E, t->unk40, t->unk48, t->unk4A);
        u = gUnk_03002490;
        sub_08001a94(u->unk42, tbl[u->unk24], u->unk3E, u->unk40,
                     u->unk48 + u->unk28, (s16)(u->unk4A + u->unk2C));
        v = gUnk_03002490;
        sub_08001a94(v->unk42 + 1, gUnk_08755AD8[0], v->unk3E, v->unk40,
                     v->unk48, (s16)(v->unk4A + 8));
    }
}

void sub_080bcf60(void)
{
    struct Task *t = gUnk_03002490;

    t->unk0C = (u32)sub_080bce74;
    t->unk04 = (u32)sub_080bcf8c;
    t->unk42 = 4;
    sub_080bcde0();
    sub_08006138();
}

void sub_080bcf8c(void)
{
    if (gUnk_03002490->unk1C != 0)
        sub_080bce28();
}

void sub_080bcfa4(void)
{
    struct Task *t = gUnk_03002490;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t->unk0C = (u32)sub_08005ca0;
    t->unk00 = (u32)sub_080059d8;
    t->unk42 = 4;
    u = gUnk_03002490;
    u->unk38 = gUnk_08755A7C;
    u->unk40 |= 0x800;
    u->unk48 = 240;
    u->unk4A = 0;
    u->unk4C = u->unk48 << 16;
    u->unk50 = u->unk4A << 16;
    TaskYieldTrampoline(2);
    v = gUnk_03002490;
    v->unk54 = 0xFFD00000;
    v->unk58 = 0x180000;
    v->unk3C = 1;
    TaskYieldTrampoline(5);
    w = gUnk_03002490;
    w->unk48 = 64;
    w->unk4A = -16;
    w->unk4C = w->unk48 << 16;
    w->unk50 = w->unk4A << 16;
    w->unk54 = 0x200000;
    w->unk58 = 0x400000;
    w->unk3C++;
    TaskYieldTrampoline(4);
    TaskDispatchTrampoline();
}

void sub_080bd06c(void)
{
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk42 = 4;
    gUnk_03002490->unk38 = gUnk_08755ADC;
    gUnk_03002490->unk40 |= 0x800;
    gUnk_03002490->unk48 = 120;
    gUnk_03002490->unk4A = 96;
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
    gUnk_03002490->unk3C = 0;
    TaskYieldTrampoline(3);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(1);
    gUnk_03002490->unk3C++;
    TaskYieldTrampoline(2);
    gUnk_03002490->unk3C = -1;
    TaskYieldTrampoline(60);
    TaskDispatchTrampoline();
}

void sub_080bd110(void)
{
    struct Task *t;

    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk42 = 4;
    t = gUnk_03002490;
    t->unk38 = gUnk_08755AC8;
    t->unk40 |= 0x800;
    t->unk4C = t->unk48 << 16;
    t->unk50 = t->unk4A << 16;
    t->unk3C = t->unk18;
    t->unk58 = gUnk_087564A0[t->unk18];
    TaskYieldTrampoline(3);
    sub_080062c4();
    sub_08006138();
}

void sub_080bd188(s32 a0)
{
    struct GfxDesc *d = gUnk_087564D0[a0];

    LZ77UnCompWram(d->unk0C, gUnk_02020000);
    sub_080017e4(3, (u32)gUnk_02020000, 0x06013000, d->unk02 << 5);
    sub_080017e4(2, d->unk08, (u32)gUnk_03001570, d->unk00 << 5);
}

void sub_080bd1d0(s32 a0, u16 a1)
{
    struct Task *t = &gUnk_03002790[a0];
    struct Task *u = &gUnk_03002790[t->unk44];

    if (u->unk18 != 2)
    {
        t->unk14 = a1;
        sub_08006148(sub_080bd524, a0);
    }
}

void sub_080bd210(void)
{
    struct Task *t = gUnk_03002490;

    if (t->unk48 <= 164)
    {
        t->unk48 = 164;
        t->unk24 = 1;
    }
    else
    {
        t->unk48 -= 10;
    }
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
}

void sub_080bd25c(void)
{
    struct Task *t;

    sub_080bd290();
    t = gUnk_03002490;
    t->unk48 = 276;
    t->unk24 = 0;
    t->unk4C = t->unk48 << 16;
    t->unk50 = t->unk4A << 16;
}

void sub_080bd290(void)
{
    s32 i;

    gUnk_03002490->unk48 = 164;
    gUnk_03002490->unk4A = 72;
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
    gUnk_03002490->unk43 = -1;
    gUnk_03002490->unk3E &= 0x7FFF;
    switch (gUnk_03002490->unk18)
    {
    case 0:
        gUnk_03002490->unk38 = gUnk_087559E4;
        break;
    case 1:
        gUnk_03002490->unk38 = gUnk_087559F4;
        break;
    case 2:
        gUnk_03002490->unk38 = gUnk_08755A04;
        break;
    case 3:
        gUnk_03002490->unk38 = gUnk_08755A24;
        break;
    case 4:
        gUnk_03002490->unk38 = gUnk_08755A14;
        break;
    }
    gUnk_03002490->unk40 = 0x8180;
    gUnk_03002490->unk3C = 0;
    i = gUnk_03002490->unk74 * 5 + gUnk_03002490->unk18;
    gUnk_03002490->unk1C = gUnk_087564B0[i];
}

void sub_080bd370(void)
{
    s32 idx = sub_080058e4(94, 32);

    if (idx != -1)
    {
        struct Task *t = gUnk_03002490;
        s16 x = t->unk48 - 24;
        s16 y = t->unk4A - 32;
        struct Task *n;

        /* Dead `ldrsh` of t->unk48 in the ROM: a switch whose arms are all
           no-ops (same artifact as sub_080bb874/sub_080bc5cc/sub_080bc680).
           Three labels, the middle one touching y, reproduce the allocation
           (x r3, y r4, idx r5); two or four labels, or x-only arms, swap
           y and idx. */
        switch (t->unk48)
        {
        case 120:
            x = x;
            break;
        case 56:
            y = y;
            break;
        case 104:
            x = x;
            break;
        }
        n = &gUnk_03002790[idx];
        n->unk73 = 5;
        n->unk76 = 1;
        n->unk48 = x;
        n->unk4A = y;
    }
    gUnk_03002490->unk20 = 60;
}

void sub_080bd3dc(u8 a0)
{
    if (a0)
    {
        struct Task *t = gUnk_03002490;

        t->unk48 = 120;
        t->unk4A = 96;
        switch (t->unk18)
        {
        case 0:
            sub_080031b8(253);
            break;
        case 1:
            sub_080031b8(258);
            break;
        case 2:
            sub_080031b8(259);
            break;
        case 3:
            sub_080031b8(261);
            break;
        case 4:
            sub_080031b8(260);
            break;
        }
    }
    else
    {
        gUnk_03002490->unk48 = 148;
        gUnk_03002490->unk4A = 80;
    }
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
    gUnk_03002490->unk3C = 1;
}

void sub_080bd494(void)
{
    struct Task *t = gUnk_03002490;

    t->unk54 = 0x20000;
    t->unk58 = -0x50000;
    t->unk60 = 0x3000;
    t->unk68 = 0x80000;
    t->unk3C = 2;
}

void sub_080bd4bc(void)
{
    struct Task *t;

    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk04 = (u32)sub_080bd508;
    gUnk_03002490->unk42 = 12;
    t = gUnk_03002490;
    t->unk18 = 0;
    if (t->unk76 != 0)
        t->unk14 = 5;
    else
        t->unk14 = 0;
    sub_08002e98(gUnk_03002490->unk14, 6, gUnk_087564E4);
    sub_08006138();
}

void sub_080bd508(void)
{
    sub_08002e98(gUnk_03002490->unk15, 6, gUnk_087564FC);
}

void sub_080bd524(void)
{
    struct Task *t = gUnk_03002490;

    t->unk00 = 0;
    sub_08002e98(t->unk14, 6, gUnk_087564E4);
}

void sub_080bd544(void)
{
    gUnk_03002490->unk15 = 0;
    sub_080bd25c();
    while (gUnk_03000FB8 != 0)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(60);
    sub_080bd370();
    TaskYieldTrampoline(gUnk_03002490->unk20);
    gUnk_03002490->unk14 = 1;
    sub_08006138();
}

void sub_080bd594(void)
{
    if (gUnk_03000FB8 == 0 && gUnk_03002490->unk24 == 0)
        sub_080bd210();
    if (gUnk_03002490->unk14 != 0)
        sub_08006148(sub_080bd524, gCurTaskIdx);
}

void sub_080bd5d4(void)
{
    gUnk_03002490->unk15 = 1;
    sub_080bd290();
    while (gUnk_03000FB8 != 0)
        TaskYieldTrampoline(1);
    sub_08006138();
}

void sub_080bd60c(void)
{
}

void sub_080bd610(void)
{
    gUnk_03002490->unk15 = 2;
    sub_080bd3dc(1);
    sub_08006138();
}

void sub_080bd62c(void)
{
}

void sub_080bd630(void)
{
    gUnk_03002490->unk00 = (u32)sub_080059d8;
    gUnk_03002490->unk15 = 3;
    sub_080bd494();
    gUnk_0200B048 = ++gUnk_03002490->unk18;
    sub_08006138();
}

void sub_080bd664(void)
{
    if (gUnk_03002490->unk54 != 0 && gUnk_03002490->unk58 != 0)
    {
        u8 r = sub_080bc7c8();
        if (r == 0)
        {
            sub_080062c4();
            gUnk_03002490->unk00 = r;
        }
    }
}

void sub_080bd694(void)
{
    gUnk_03002490->unk15 = 4;
    sub_080bd3dc(0);
    sub_08006138();
}

void sub_080bd6b0(void)
{
}

void sub_080bd6b4(void)
{
    struct Task *t;

    gUnk_03002490->unk04 = 0;
    gUnk_03002490->unk42 = 4;
    switch (gUnk_0200B048)
    {
    case 0:
        gUnk_03002490->unk38 = gUnk_087559E4;
        break;
    case 1:
        gUnk_03002490->unk38 = gUnk_087559F4;
        break;
    case 2:
        gUnk_03002490->unk38 = gUnk_08755A04;
        break;
    case 3:
        gUnk_03002490->unk38 = gUnk_08755A24;
        break;
    case 4:
        gUnk_03002490->unk38 = gUnk_08755A14;
        break;
    }
    t = gUnk_03002490;
    t->unk40 = 0x9180;
    t->unk3C = 3;
    switch (gUnk_0200B048)
    {
    case 0:
        gUnk_03002490->unk48 -= 16;
        gUnk_03002490->unk4A -= 8;
        break;
    case 1:
        gUnk_03002490->unk4A -= 12;
        break;
    case 2:
        gUnk_03002490->unk48 -= 32;
        gUnk_03002490->unk4A -= 28;
        break;
    case 3:
        gUnk_03002490->unk48 -= 20;
        gUnk_03002490->unk4A -= 32;
        break;
    case 4:
        gUnk_03002490->unk48 -= 24;
        gUnk_03002490->unk4A -= 8;
        break;
    }
    gUnk_03002490->unk4C = gUnk_03002490->unk48 << 16;
    gUnk_03002490->unk50 = gUnk_03002490->unk4A << 16;
    TaskYieldTrampoline(60);
    TaskDispatchTrampoline();
}

void sub_080bd7ec(void)
{
}

void sub_080bd7f0(void)
{
    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk42 = 4;
    gUnk_03002490->unk38 = gUnk_08755B90;
    gUnk_03002490->unk40 |= 0x800;
    sub_08006138();
}

void sub_080bd828(void)
{
    if (gUnk_03002150 == 5)
    {
        switch (gUnk_03002490->unk3C)
        {
        case 5:
            sub_0800a0ac(10, gUnk_03002490->unk1C);
            break;
        case 2:
            sub_0800a0ac(1000, gUnk_03002490->unk1C);
            break;
        case 3:
            sub_0800a0ac(3000, gUnk_03002490->unk1C);
            break;
        case 4:
            sub_0800a0ac(5000, gUnk_03002490->unk1C);
            break;
        case 6:
            sub_08009eb8(1, gUnk_03002490->unk1C);
            break;
        }
    }
}

void sub_080bd8ac(void)
{
    struct Task *t;

    gUnk_03002490->unk0C = (u32)sub_08005ca0;
    gUnk_03002490->unk42 = 4;
    t = gUnk_03002490;
    t->unk38 = gUnk_08755B90;
    if (gUnk_030023AC != 1)
        t->unk3C = gUnk_08756514[t->unk18];
    else
        t->unk3C = gUnk_0875651C[t->unk18];
    if (gUnk_03002490->unk3C == 6 && gUnk_03002360 == gUnk_03002490->unk1C)
        sub_080031b8(220);
    gUnk_03002490->unk40 |= 0x800;
    sub_080bd828();
    sub_08006138();
}

void sub_080bd938(void)
{
    struct Task *t = gUnk_03002490;
    u32 *p = t->unk38;

    if (p != NULL && t->unk3C != -1)
    {
        if (t->unk48 >= -63 && t->unk48 <= 303 && t->unk4A > -64 && t->unk4A < 224)
            sub_08001a94(t->unk42, p[t->unk3C], t->unk3E, t->unk40, t->unk48, t->unk4A);
    }
}

void sub_080bd9b0(void)
{
    gUnk_03002490->unk0C = (u32)sub_080bd938;
    gUnk_03002490->unk42 = 4;
    gUnk_03002490->unk38 = gUnk_08755A68;
    gUnk_03002490->unk40 |= 0x800;
    sub_08006138();
}
