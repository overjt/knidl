#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* camtask_2d38c.c (0x0802D38C-0x0802EAC7, issue #86).
 *
 * The seven bodies of task type #4 (sub_0802d370 dispatches Task.unk14
 * into the anchor table gUnk_087328A0), the level's scripted map events:
 * sub_0802d38c waits for camera mode 3 and M07's sub_08027750, raises
 * gUnk_0200D080 until it drops, then by Task.unk18 spawns a type-#4
 * child (sub_0802d478/sub_0802d5b4) or updates two metatiles through
 * sub_080261c0 and M09's sub_08030f78; sub_0802d4bc and sub_0802d5f8
 * update one or three metatiles behind type-#236 effects;
 * sub_0802d6cc and sub_0802d96c/sub_0802da8c fade the room palettes
 * towards another room's (the table gUnk_087E1D58, sub_08003014 into the
 * palette buffer gUnk_03001270); sub_0802dcb4 and sub_0802e3ac pan the
 * camera four pixels a frame by the step counts of gUnk_08732428 /
 * gUnk_087324A6 (axis order, x steps, y steps) inside the room bounds,
 * wait, and pan back. */

struct BgMap
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u16 unk4;
    /*0x06*/ u16 unk6[0];
};

/* The room header gUnk_030055EC points at (one entry of the gUnk_087E1D58
   room table): unk18/unk28 are length-prefixed palettes, unk30 the BG map
   streamed into 0x06003000, unk40 the room's BG animation script set. */
struct RoomDef
{
    /*0x00*/ u8 filler00[0x18];
    /*0x18*/ u16 *unk18;
    /*0x1C*/ u8 filler1C[0xC];
    /*0x28*/ u16 *unk28;
    /*0x2C*/ u8 filler2C[4];
    /*0x30*/ struct BgMap *unk30;
    /*0x34*/ u8 filler34[0xC];
    /*0x40*/ u16 unk40;
};

/* gUnk_02006098[3] is cleared through an 8-bit signed bit-field: only a
   bit-field store of -1 gives the ROM's `movs #255; orrs rX, rLoaded; strb`
   (a plain `= -1` or `|= 0xFF` is folded to `movs #255; strb`). */
struct Unk02006098Bits { s8 unk0; s8 unk1; s8 unk2; s32 unk3:8; };

struct Unk0200A6F0
{
    /*0x00*/ u8 filler00[6];
    /*0x06*/ u16 unk6;
    /*0x08*/ u8 filler08[0x18];
};

extern u16 gUnk_030055C0;
extern u8 gUnk_0200D080;
extern u16 gUnk_02007D60;
extern s8 gUnk_02007D64;
extern struct RoomDef *gUnk_030055EC;
extern u16 gUnk_03001270[];
extern struct RoomDef **gUnk_087E1D58[][8];
extern s8 gUnk_030023EC;
extern s8 gUnk_0300238C;
extern s8 gUnk_03002468;
extern u16 gUnk_02005E10[];
extern s8 gUnk_02006098[];
extern u8 gUnk_03001470[];
extern u8 gUnk_087328BC[];
extern u16 gUnk_0200001C;
extern s8 gUnk_08732428[][3];
extern s16 gUnk_03002398;
extern s16 gUnk_03001F00;
extern s16 gUnk_03005628[4];
extern s16 gUnk_0200AFE0[4];
extern u16 gUnk_087323C6[][2];
extern u8 gUnk_02007FC4;
extern s8 gUnk_030023B8;
extern s16 gUnk_03005620;
extern u16 gUnk_02004CA0[];
extern struct Unk0200A6F0 gUnk_0200A6F0[];
extern u8 gUnk_0200AF08;
extern s16 gUnk_020055D4;
extern u8 gUnk_020055E8;
extern s8 gUnk_087324A6[][3];
extern vs16 gUnk_03000FB8;

void TaskDispatchTrampoline(void);
void TaskYieldTrampoline(u32 frames);
void sub_080017e4(u32 mode, u32 src, u32 dst, u32 size);
void sub_08003014(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void sub_080031b8(u32 a);
void sub_08005654(s32 id);
void sub_08006138(void);
void sub_08025dc4(void);
s32 sub_080261c0(s32 x, s32 y);
s32 sub_0802621c(s32 type);
s32 sub_08026584(void);
s32 sub_08027750(void);
void sub_08029b30(void);
void sub_0802d01c(void);
s32 sub_080301e8(s32 a, s32 x, s32 y);
void sub_080307a4(void);
void sub_080307e8(void);
s32 sub_08030f78(u32 x, u32 y);
s32 sub_08032288(s32 x, s32 y);
s32 sub_08032338(void);
s32 sub_0802d478(s32 x, s32 y);
s32 sub_0802d5b4(s32 x, s32 y);
void sub_0802da8c(void);

void sub_0802d38c(void)
{
    struct Task *t;

    t = gUnk_03002490;
    t->unk00 = 0;
    t->unk0C = 0;
    if (t->unk24 != 0)
    {
        while (gUnk_030055C0 != 3)
            TaskYieldTrampoline(1);
        while (sub_08027750() == 0)
            TaskYieldTrampoline(1);
    }
    gUnk_0200D080 = 1;
    do
        TaskYieldTrampoline(1);
    while (gUnk_0200D080 == 1);
    gUnk_02007D60 |= 0x8000;
    sub_0802d01c();
    if (gUnk_03002490->unk18 == 1)
    {
        if (gUnk_02007D64 == 4)
            sub_0802d5b4(gUnk_03002490->unk1C, gUnk_03002490->unk20);
        else
            sub_0802d478(gUnk_03002490->unk1C, gUnk_03002490->unk20);
    }
    else if (gUnk_03002490->unk18 == 2)
    {
        sub_080031b8(159);
        sub_080261c0((gUnk_03002490->unk1C & 0xFFF0) + 8, (gUnk_03002490->unk20 & 0xFFF0) + 8);
        sub_08030f78(gUnk_03002490->unk1C >> 4, gUnk_03002490->unk20 >> 4);
        sub_08030f78(gUnk_03002490->unk1C >> 4, (gUnk_03002490->unk20 >> 4) - 1);
    }
    TaskDispatchTrampoline();
}

s32 sub_0802d478(s32 x, s32 y)
{
    s32 id;
    struct Task *t;

    id = sub_0802621c(4);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk14 = 2;
        t->unk4C = x << 16;
        t->unk50 = y << 16;
        t->unk48 = x;
        t->unk4A = y;
    }
    return id;
}

void sub_0802d4bc(void)
{
    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk0C = 0;
    sub_080031b8(159);
    sub_080301e8(2, gUnk_03002490->unk48, gUnk_03002490->unk4A - 8);
    if ((gUnk_03002490->unk46 = sub_080301e8(1, gUnk_03002490->unk48, gUnk_03002490->unk4A - 8)) == -1)
        TaskDispatchTrampoline();
    gUnk_03002790[gUnk_03002490->unk46].unk18 = 0;
    while (gUnk_03002790[gUnk_03002490->unk46].unk18 == 0)
        TaskYieldTrampoline(1);
    sub_08030f78(gUnk_03002490->unk48 >> 4, gUnk_03002490->unk4A >> 4);
    sub_08030f78(gUnk_03002490->unk48 >> 4, (gUnk_03002490->unk4A - 16) >> 4);
    TaskYieldTrampoline(1);
    sub_08005654(gUnk_03002490->unk46);
    TaskDispatchTrampoline();
}

s32 sub_0802d5b4(s32 x, s32 y)
{
    s32 id;
    struct Task *t;

    id = sub_0802621c(4);
    if (id != -1)
    {
        t = &gUnk_03002790[id];
        t->unk14 = 3;
        t->unk4C = x << 16;
        t->unk50 = y << 16;
        t->unk48 = x;
        t->unk4A = y;
    }
    return id;
}

void sub_0802d5f8(void)
{
    gUnk_03002490->unk00 = 0;
    gUnk_03002490->unk0C = 0;
    sub_080301e8(5, gUnk_03002490->unk48 + 2, gUnk_03002490->unk4A);
    TaskYieldTrampoline(2);
    sub_08030f78(gUnk_03002490->unk48 >> 4, gUnk_03002490->unk4A >> 4);
    TaskYieldTrampoline(4);
    sub_080301e8(5, gUnk_03002490->unk48 + 2, gUnk_03002490->unk4A - 12);
    TaskYieldTrampoline(2);
    sub_08030f78(gUnk_03002490->unk48 >> 4, (gUnk_03002490->unk4A - 16) >> 4);
    TaskYieldTrampoline(4);
    sub_080301e8(5, gUnk_03002490->unk48 + 2, gUnk_03002490->unk4A - 24);
    TaskYieldTrampoline(2);
    sub_08030f78(gUnk_03002490->unk48 >> 4, (gUnk_03002490->unk4A - 32) >> 4);
    TaskYieldTrampoline(4);
    TaskDispatchTrampoline();
}

void sub_0802d6cc(void)
{
    struct Task *t;
    struct Task *t1;
    struct Task *u1;
    struct Task *t2;
    struct Task *u2;
    struct Task *t3;
    struct Task *u3;
    struct RoomDef *m;
    u16 *p18;
    u16 *p28;

    t = gUnk_03002490;
    t->unk00 = 0;
    t->unk0C = 0;
    m = gUnk_030055EC;
    p18 = m->unk18;
    t->unk54 = *p18 >> 1;
    p28 = m->unk28;
    t->unk58 = *p28 >> 1;
    t->unk60 = (s32)((u8 *)(gUnk_03001270 + 0x100) - *p28);
    t->unk28 = (s32)(p18 + 1);
    t->unk2C = (s32)(p28 + 1);
    t->unk30 = (s32)(gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_03002468 + 1]->unk18 + 1);
    t->unk34 = (s32)(gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_03002468 + 1]->unk28 + 1);
    t->unk6C = 0;
    do
    {
        t1 = gUnk_03002490;
        sub_08003014((u16 *)t1->unk28, (u16 *)t1->unk30, (u16)((s16)t1->unk6C * 8), (u16)t1->unk54, gUnk_03001270 + 0x20);
        u1 = gUnk_03002490;
        sub_08003014((u16 *)u1->unk2C, (u16 *)u1->unk34, (u16)((s16)u1->unk6C * 8), (u16)u1->unk58, (u16 *)u1->unk60);
        sub_08003014(gUnk_02005E10 + 0x80, gUnk_02005E10, (u16)((s16)gUnk_03002490->unk6C * 8), 96, gUnk_03001270 + 0x180);
        TaskYieldTrampoline(1);
    } while ((s16)++gUnk_03002490->unk6C <= 32);
    TaskYieldTrampoline(8);
    gUnk_03002490->unk6E = 0;
    do
    {
        gUnk_03002490->unk6C = 0;
        do
        {
            t2 = gUnk_03002490;
            sub_08003014((u16 *)t2->unk30, (u16 *)t2->unk28, (u16)((s16)t2->unk6C * 64), (u16)t2->unk54, gUnk_03001270 + 0x20);
            u2 = gUnk_03002490;
            sub_08003014((u16 *)u2->unk34, (u16 *)u2->unk2C, (u16)((s16)u2->unk6C * 64), (u16)u2->unk58, (u16 *)u2->unk60);
            sub_08003014(gUnk_02005E10, gUnk_02005E10 + 0x80, (u16)((s16)gUnk_03002490->unk6C * 64), 96, gUnk_03001270 + 0x180);
            TaskYieldTrampoline(1);
        } while ((s16)++gUnk_03002490->unk6C <= 4);
        gUnk_03002490->unk6C = 0;
        do
        {
            t3 = gUnk_03002490;
            sub_08003014((u16 *)t3->unk28, (u16 *)t3->unk30, (u16)((s16)t3->unk6C * 64), (u16)t3->unk54, gUnk_03001270 + 0x20);
            u3 = gUnk_03002490;
            sub_08003014((u16 *)u3->unk2C, (u16 *)u3->unk34, (u16)((s16)u3->unk6C * 64), (u16)u3->unk58, (u16 *)u3->unk60);
            sub_08003014(gUnk_02005E10 + 0x80, gUnk_02005E10, (u16)((s16)gUnk_03002490->unk6C * 64), 96, gUnk_03001270 + 0x180);
            TaskYieldTrampoline(1);
        } while ((s16)++gUnk_03002490->unk6C <= 4);
        TaskYieldTrampoline(4);
    } while (++gUnk_03002490->unk6E <= 1);
    gUnk_02006098[0] = 1;
    TaskDispatchTrampoline();
}

void sub_0802d96c(void)
{
    struct Task *t;
    struct RoomDef *m;

    t = gUnk_03002490;
    t->unk00 = 0;
    t->unk0C = 0;
    m = gUnk_030055EC;
    t->unk64 = *m->unk18 >> 1;
    t->unk68 = *m->unk28 >> 1;
    t->unk60 = (s32)(gUnk_03001470 - *m->unk28);
    t->unk28 = (s32)(gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_087328BC[gUnk_02006098[1]]]->unk18 + 1);
    t->unk2C = (s32)(gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_087328BC[gUnk_02006098[1]]]->unk28 + 1);
    t->unk30 = (s32)(gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_087328BC[gUnk_02006098[2]]]->unk18 + 1);
    t->unk34 = (s32)(gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_087328BC[gUnk_02006098[2]]]->unk28 + 1);
    t->unk04 = (u32)sub_0802da8c;
    if (gUnk_02006098[4] > 0)
        t->unk6C = 0;
    else
        t->unk6C = 0x100;
    sub_08006138();
    TaskDispatchTrampoline();
}

void sub_0802da8c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    if (gUnk_02006098[4] > 0)
    {
        gUnk_03002490->unk6C += 16;
        if ((s16)gUnk_03002490->unk6C > 0x100)
            gUnk_03002490->unk6C = 0x100;
    }
    else
    {
        gUnk_03002490->unk6C -= 16;
        if ((s16)gUnk_03002490->unk6C < 0)
            gUnk_03002490->unk6C = 0;
    }
    t = gUnk_03002490;
    sub_08003014((u16 *)t->unk2C, (u16 *)t->unk34, t->unk6C, (u16)t->unk68, (u16 *)t->unk60);
    if (gUnk_02006098[4] > 0 && (s16)(u = gUnk_03002490)->unk6C == 0x100)
    {
        if (gUnk_02006098[3] != -1)
        {
            gUnk_02006098[1] = gUnk_02006098[2];
            gUnk_02006098[2] = gUnk_02006098[3];
            ((struct Unk02006098Bits *)gUnk_02006098)->unk3 = -1;
            u->unk28 = u->unk30;
            u->unk2C = u->unk34;
            u->unk30 = (s32)(gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_087328BC[gUnk_02006098[2]]]->unk18 + 1);
            u->unk34 = (s32)(gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_087328BC[gUnk_02006098[2]]]->unk28 + 1);
            u->unk6C = 0;
            gUnk_02006098[0] = gUnk_02006098[1] | 0x80;
        }
        else
        {
            gUnk_02006098[0] = gUnk_02006098[2];
            sub_08005654(gCurTaskIdx);
        }
    }
    else if (gUnk_02006098[4] < 0 && (s16)(v = gUnk_03002490)->unk6C == 0)
    {
        if (gUnk_02006098[3] != -1)
        {
            gUnk_02006098[2] = gUnk_02006098[1];
            gUnk_02006098[1] = gUnk_02006098[3];
            ((struct Unk02006098Bits *)gUnk_02006098)->unk3 = -1;
            v->unk30 = v->unk28;
            v->unk34 = v->unk2C;
            v->unk28 = (s32)(gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_087328BC[gUnk_02006098[1]]]->unk18 + 1);
            v->unk2C = (s32)(gUnk_087E1D58[gUnk_0300238C][gUnk_030023EC][gUnk_087328BC[gUnk_02006098[1]]]->unk28 + 1);
            v->unk6C = 0x100;
            gUnk_02006098[0] = gUnk_02006098[2] | 0x80;
        }
        else
        {
            gUnk_02006098[0] = gUnk_02006098[1];
            sub_08005654(gCurTaskIdx);
        }
    }
}

void sub_0802dcb4(void)
{
    struct Task *t1;
    struct Task *t2;
    struct Task *t3;
    struct Task *t4;
    struct Task *t5;
    struct Task *t6;
    struct Task *t7;
    struct Task *t8;
    struct Task *t9;
    struct Task *t10;
    struct Task *t11;
    struct Task *t12;
    struct Task *t13;
    s32 d1;
    s32 d2;
    s32 d3;
    s32 d4;

    t1 = gUnk_03002490;
    t1->unk00 = 0;
    t1->unk0C = 0;
    t1->unk1C = gUnk_030023EC * 6 + gUnk_0200001C - 1;
    t1->unk54 = t1->unk28 = gUnk_08732428[t1->unk1C][1];
    if (t1->unk28 > 0)
        t1->unk2C = 4;
    else if (t1->unk28 < 0)
    {
        t1->unk28 = -t1->unk28;
        t1->unk2C = -4;
    }
    t2 = gUnk_03002490;
    t2->unk58 = t2->unk30 = gUnk_08732428[t2->unk1C][2];
    if (t2->unk30 > 0)
        t2->unk34 = 4;
    else if (t2->unk30 < 0)
    {
        t2->unk30 = -t2->unk30;
        t2->unk34 = -4;
    }
    t3 = gUnk_03002490;
    t3->unk48 = gUnk_03002398;
    t3->unk4A = gUnk_03001F00;
    if (gUnk_08732428[t3->unk1C][0] == 0)
    {
        for (t3->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk28; gUnk_03002490->unk6C++)
        {
            t4 = gUnk_03002490;
            if ((t4->unk54 > 0 && gUnk_03002398 >= gUnk_03005628[1]) || (t4->unk54 < 0 && gUnk_03002398 <= gUnk_03005628[0]))
                break;
            gUnk_03002398 += t4->unk2C;
            TaskYieldTrampoline(1);
        }
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk30; gUnk_03002490->unk6C++)
        {
            t5 = gUnk_03002490;
            if ((t5->unk58 > 0 && gUnk_03001F00 >= gUnk_03005628[3]) || (t5->unk58 < 0 && gUnk_03001F00 <= gUnk_03005628[2]))
                break;
            gUnk_03001F00 += t5->unk34;
            TaskYieldTrampoline(1);
        }
    }
    else
    {
        for (t3->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk30; gUnk_03002490->unk6C++)
        {
            t6 = gUnk_03002490;
            if ((t6->unk54 > 0 && gUnk_03002398 == gUnk_03005628[1]) || (t6->unk54 < 0 && gUnk_03002398 == gUnk_03005628[0]))
                break;
            gUnk_03001F00 += t6->unk34;
            TaskYieldTrampoline(1);
        }
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk28; gUnk_03002490->unk6C++)
        {
            t7 = gUnk_03002490;
            if ((t7->unk58 > 0 && gUnk_03001F00 == gUnk_03005628[3]) || (t7->unk58 < 0 && gUnk_03001F00 == gUnk_03005628[2]))
                break;
            gUnk_03002398 += t7->unk2C;
            TaskYieldTrampoline(1);
        }
    }
    TaskYieldTrampoline(15);
    sub_080307e8();
    TaskYieldTrampoline(1);
    gUnk_03002490->unk20 = gUnk_03002490->unk1C * 2;
    if (sub_08032288(gUnk_0200AFE0[0], gUnk_0200AFE0[1]))
        sub_08032338();
    if (gUnk_0200AFE0[2] != -1 && sub_08032288(gUnk_0200AFE0[2], gUnk_0200AFE0[3]))
        sub_08032338();
    while (1)
    {
        t8 = gUnk_03002490;
        if (t8->unk24 != 0 && gUnk_02007FC4 == 0)
        {
            t8->unk4C = gUnk_087323C6[gUnk_030023B8][0];
            t8->unk50 = gUnk_087323C6[gUnk_030023B8][1];
            t8->unk5C = gUnk_03005620 * t8->unk50 + t8->unk4C;
            for (t8->unk6C = 0; (s16)gUnk_03002490->unk6C <= 1; gUnk_03002490->unk6C++)
            {
                for (gUnk_03002490->unk6E = 0; gUnk_03002490->unk6E <= 1; gUnk_03002490->unk6E++)
                {
                    t9 = gUnk_03002490;
                    if (gUnk_02004CA0[t9->unk5C + (s16)t9->unk6C + gUnk_03005620 * t9->unk6E] != 0)
                        goto skip;
                }
            }
            gUnk_02007FC4++;
            gUnk_03002490->unk24 = 0;
        }
    skip:
        t10 = gUnk_03002490;
        t10->unk20 = 0;
        for (t10->unk6C = 0; (s16)gUnk_03002490->unk6C <= 63; gUnk_03002490->unk6C++)
        {
            if (gUnk_0200A6F0[(s16)gUnk_03002490->unk6C].unk6 != 0x7FFF)
            {
                gUnk_03002490->unk20++;
                break;
            }
        }
        if (gUnk_03002490->unk20 == 0)
            break;
        TaskYieldTrampoline(1);
    }
    sub_080307a4();
    sub_080017e4(6, 0, 0x06001800, 0x800);
    if (gUnk_03002490->unk24 != 0 && gUnk_02007FC4 == 0)
        gUnk_02007FC4++;
    TaskYieldTrampoline(20);
    sub_08029b30();
    gUnk_0200AF08 = 0;
    if (gUnk_08732428[gUnk_03002490->unk1C][0] == 0)
    {
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk28 && gUnk_03002398 != gUnk_03002490->unk48; gUnk_03002490->unk6C++)
        {
            t11 = gUnk_03002490;
            gUnk_03002398 -= t11->unk2C;
            d1 = t11->unk54;
            if ((d1 > 0 && gUnk_03002398 < t11->unk48) || (d1 < 0 && gUnk_03002398 > t11->unk48))
                gUnk_03002398 = gUnk_03002490->unk48;
            TaskYieldTrampoline(1);
        }
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk30 && gUnk_03002398 != gUnk_03002490->unk4A; gUnk_03002490->unk6C++)
        {
            t12 = gUnk_03002490;
            gUnk_03001F00 -= t12->unk34;
            d2 = t12->unk58;
            if ((d2 > 0 && gUnk_03001F00 < t12->unk4A) || (d2 < 0 && gUnk_03001F00 > t12->unk4A))
                gUnk_03001F00 = gUnk_03002490->unk4A;
            TaskYieldTrampoline(1);
        }
    }
    else
    {
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk30 && gUnk_03002398 != gUnk_03002490->unk4A; gUnk_03002490->unk6C++)
        {
            gUnk_03001F00 -= gUnk_03002490->unk34;
            d3 = gUnk_03002490->unk58;
            if ((d3 > 0 && gUnk_03001F00 < gUnk_03002490->unk4A) || (d3 < 0 && gUnk_03001F00 > gUnk_03002490->unk4A))
                gUnk_03001F00 = gUnk_03002490->unk4A;
            TaskYieldTrampoline(1);
        }
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk28 && gUnk_03002398 != gUnk_03002490->unk48; gUnk_03002490->unk6C++)
        {
            t13 = gUnk_03002490;
            gUnk_03002398 -= t13->unk2C;
            d4 = t13->unk54;
            if ((d4 > 0 && gUnk_03002398 < t13->unk48) || (d4 < 0 && gUnk_03002398 > t13->unk48))
                gUnk_03002398 = gUnk_03002490->unk48;
            TaskYieldTrampoline(1);
        }
    }
    TaskYieldTrampoline(10);
    if (gUnk_020055D4 == 0x2000)
        gUnk_020055D4 = 0x4000;
    gUnk_020055E8 = 1;
    TaskDispatchTrampoline();
}

void sub_0802e3ac(void)
{
    struct Task *t1;
    struct Task *t2;
    struct Task *t3;
    struct Task *t4;
    struct Task *t5;
    struct Task *t6;
    struct Task *t7;
    struct Task *t8;
    struct Task *t9;
    struct Task *t10;
    struct Task *t12;
    struct Task *t13;
    struct Task *t14;
    struct Task *t15;
    struct Task *t16;
    s32 d1;
    s32 d2;
    s32 d3;
    s32 d4;

    t1 = gUnk_03002490;
    t1->unk00 = 0;
    t1->unk0C = 0;
    t1->unk1C = (u8)gUnk_0200001C;
    t1->unk54 = t1->unk28 = gUnk_087324A6[t1->unk1C][1];
    if (t1->unk28 > 0)
        t1->unk2C = 4;
    else if (t1->unk28 < 0)
    {
        t1->unk28 = -t1->unk28;
        t1->unk2C = -4;
    }
    t2 = gUnk_03002490;
    t2->unk58 = t2->unk30 = gUnk_087324A6[t2->unk1C][2];
    if (t2->unk30 > 0)
        t2->unk34 = 4;
    else if (t2->unk30 < 0)
    {
        t2->unk30 = -t2->unk30;
        t2->unk34 = -4;
    }
    t3 = gUnk_03002490;
    t3->unk48 = gUnk_03002398;
    t3->unk4A = gUnk_03001F00;
    while (gUnk_03000FB8 != 0)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(20);
    if (gUnk_087324A6[gUnk_03002490->unk1C][0] == 0)
    {
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk28; gUnk_03002490->unk6C++)
        {
            t4 = gUnk_03002490;
            if ((t4->unk54 > 0 && gUnk_03002398 >= gUnk_03005628[1]) || (t4->unk54 < 0 && gUnk_03002398 <= gUnk_03005628[0]))
                break;
            gUnk_03002398 += t4->unk2C;
            TaskYieldTrampoline(1);
        }
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk30; gUnk_03002490->unk6C++)
        {
            t5 = gUnk_03002490;
            if ((t5->unk58 > 0 && gUnk_03001F00 >= gUnk_03005628[3]) || (t5->unk58 < 0 && gUnk_03001F00 <= gUnk_03005628[2]))
                break;
            gUnk_03001F00 += t5->unk34;
            TaskYieldTrampoline(1);
        }
    }
    else
    {
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk30; gUnk_03002490->unk6C++)
        {
            t6 = gUnk_03002490;
            if ((t6->unk58 > 0 && gUnk_03001F00 >= gUnk_03005628[3]) || (t6->unk58 < 0 && gUnk_03001F00 <= gUnk_03005628[2]))
                break;
            gUnk_03001F00 += t6->unk34;
            TaskYieldTrampoline(1);
        }
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk28; gUnk_03002490->unk6C++)
        {
            t7 = gUnk_03002490;
            if ((t7->unk54 > 0 && gUnk_03002398 >= gUnk_03005628[1]) || (t7->unk54 < 0 && gUnk_03002398 <= gUnk_03005628[0]))
                break;
            gUnk_03002398 += t7->unk2C;
            TaskYieldTrampoline(1);
        }
    }
    TaskYieldTrampoline(15);
    sub_080307e8();
    TaskYieldTrampoline(1);
    gUnk_03002490->unk20 = gUnk_03002490->unk1C * 2;
    if (sub_08032288(gUnk_0200AFE0[0], gUnk_0200AFE0[1]))
        sub_08032338();
    if (gUnk_0200AFE0[2] != -1 && sub_08032288(gUnk_0200AFE0[2], gUnk_0200AFE0[3]))
        sub_08032338();
    while (1)
    {
        t8 = gUnk_03002490;
        if (t8->unk24 != 0 && gUnk_02007FC4 == 0)
        {
            t8->unk4C = gUnk_087323C6[gUnk_030023B8][0];
            t8->unk50 = gUnk_087323C6[gUnk_030023B8][1];
            t8->unk5C = gUnk_03005620 * t8->unk50 + t8->unk4C;
            for (t8->unk6C = 0; (s16)gUnk_03002490->unk6C <= 1; gUnk_03002490->unk6C++)
            {
                for (gUnk_03002490->unk6E = 0; gUnk_03002490->unk6E <= 1; gUnk_03002490->unk6E++)
                {
                    t9 = gUnk_03002490;
                    if (gUnk_02004CA0[t9->unk5C + (s16)t9->unk6C + gUnk_03005620 * t9->unk6E] != 0)
                        goto skip;
                }
            }
            gUnk_02007FC4++;
            gUnk_03002490->unk24 = 0;
        }
    skip:
        t10 = gUnk_03002490;
        t10->unk20 = 0;
        for (t10->unk6C = 0; (s16)gUnk_03002490->unk6C <= 63; gUnk_03002490->unk6C++)
        {
            if (gUnk_0200A6F0[(s16)gUnk_03002490->unk6C].unk6 != 0x7FFF)
            {
                gUnk_03002490->unk20++;
                break;
            }
        }
        if (gUnk_03002490->unk20 == 0)
            break;
        TaskYieldTrampoline(1);
    }
    sub_080307a4();
    sub_080017e4(6, 0, 0x06001800, 0x800);
    if (gUnk_03002490->unk24 != 0 && gUnk_02007FC4 == 0)
        gUnk_02007FC4++;
    TaskYieldTrampoline(20);
    gUnk_0200AF08 = 0;
    if (gUnk_087324A6[gUnk_03002490->unk1C][0] == 0)
    {
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk28 && gUnk_03002398 != gUnk_03002490->unk48; gUnk_03002490->unk6C++)
        {
            t13 = gUnk_03002490;
            gUnk_03002398 -= t13->unk2C;
            d1 = t13->unk54;
            if ((d1 > 0 && gUnk_03002398 < t13->unk48) || (d1 < 0 && gUnk_03002398 > t13->unk48))
                gUnk_03002398 = gUnk_03002490->unk48;
            TaskYieldTrampoline(1);
        }
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk30 && gUnk_03002398 != gUnk_03002490->unk4A; gUnk_03002490->unk6C++)
        {
            t14 = gUnk_03002490;
            gUnk_03001F00 -= t14->unk34;
            d2 = t14->unk58;
            if ((d2 > 0 && gUnk_03001F00 < t14->unk4A) || (d2 < 0 && gUnk_03001F00 > t14->unk4A))
                gUnk_03001F00 = gUnk_03002490->unk4A;
            TaskYieldTrampoline(1);
        }
    }
    else
    {
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk30 && gUnk_03002398 != gUnk_03002490->unk4A; gUnk_03002490->unk6C++)
        {
            t15 = gUnk_03002490;
            gUnk_03001F00 -= t15->unk34;
            d3 = t15->unk58;
            if ((d3 > 0 && gUnk_03001F00 < t15->unk4A) || (d3 < 0 && gUnk_03001F00 > t15->unk4A))
                gUnk_03001F00 = gUnk_03002490->unk4A;
            TaskYieldTrampoline(1);
        }
        for (gUnk_03002490->unk6C = 0; (s16)gUnk_03002490->unk6C < gUnk_03002490->unk28 && gUnk_03002398 != gUnk_03002490->unk48; gUnk_03002490->unk6C++)
        {
            t16 = gUnk_03002490;
            gUnk_03002398 -= t16->unk2C;
            d4 = t16->unk54;
            if ((d4 > 0 && gUnk_03002398 < t16->unk48) || (d4 < 0 && gUnk_03002398 > t16->unk48))
                gUnk_03002398 = gUnk_03002490->unk48;
            TaskYieldTrampoline(1);
        }
    }
    TaskYieldTrampoline(10);
    if (sub_08026584())
        TaskYieldTrampoline(20);
    TaskYieldTrampoline(10);
    if (gUnk_020055D4 == 0x2000)
        gUnk_020055D4 = 0x4000;
    sub_08025dc4();
    gUnk_020055E8 = 1;
    TaskDispatchTrampoline();
}
