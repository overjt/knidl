#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* camtask_2d38c.c (0x0802D38C-0x0802EAC7, issue #86).
 *
 * The seven bodies of task type #4 (Task_MapEvent dispatches Task.unk14
 * into the anchor table gMapEventVariants), the level's scripted map events:
 * sub_0802d38c waits for camera mode 3 and M07's sub_08027750, raises
 * gUnk_0200D080 until it drops, then by Task.unk18 spawns a type-#4
 * child (sub_0802d478/sub_0802d5b4) or updates two metatiles through
 * sub_080261c0 and M09's sub_08030f78; sub_0802d4bc and sub_0802d5f8
 * update one or three metatiles behind type-#236 effects;
 * sub_0802d6cc and sub_0802d96c/sub_0802da8c fade the room palettes
 * towards another room's (the table gRoomTable, BlendColors into the
 * palette buffer gBgPalette); sub_0802dcb4 and sub_0802e3ac pan the
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

/* The room header gCurRoomDef points at (one entry of the gRoomTable
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

extern u16 gCameraMode;
extern u8 gUnk_0200D080;
extern u16 gUnk_02007D60;
extern s8 gUnk_02007D64;
extern struct RoomDef *gCurRoomDef;
extern u16 gBgPalette[];
extern struct RoomDef **gRoomTable[][8];
extern s8 gStageIndex;
extern s8 gLevelIndex;
extern s8 gRoomIndex;
extern u16 gUnk_02005E10[];
extern s8 gUnk_02006098[];
extern u8 gObjPalette[];
extern u8 gUnk_087328BC[];
extern u16 gUnk_0200001C;
extern s8 gUnk_08732428[][3];
extern s16 gCameraAnchorX;
extern s16 gCameraAnchorY;
extern s16 gRoomBounds[4];
extern s16 gUnk_0200AFE0[4];
extern u16 gUnk_087323C6[][2];
extern u8 gUnk_02007FC4;
extern s8 gUnk_030023B8;
extern s16 gRoomWidth;
extern u16 gBg1MetatileMap[];
extern struct Unk0200A6F0 gUnk_0200A6F0[];
extern u8 gUnk_0200AF08;
extern s16 gUnk_020055D4;
extern u8 gUnk_020055E8;
extern s8 gUnk_087324A6[][3];
extern vs16 gBrightness;

void TaskExitTrampoline(void);
void TaskYieldTrampoline(u32 frames);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void PlaySfx(u32 a);
void TaskFree(s32 id);
void TaskSleepForever(void);
void sub_08025dc4(void);
s32 sub_080261c0(s32 x, s32 y);
s32 sub_0802621c(s32 type);
s32 sub_08026584(void);
s32 sub_08027750(void);
void sub_08029b30(void);
void CameraLeaveScrollLock(void);
s32 sub_080301e8(s32 a, s32 x, s32 y);
void PauseBlockAnims(void);
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

    t = gCurTask;
    t->unk00 = 0;
    t->unk0C = 0;
    if (t->unk24 != 0)
    {
        while (gCameraMode != 3)
            TaskYieldTrampoline(1);
        while (sub_08027750() == 0)
            TaskYieldTrampoline(1);
    }
    gUnk_0200D080 = 1;
    do
        TaskYieldTrampoline(1);
    while (gUnk_0200D080 == 1);
    gUnk_02007D60 |= 0x8000;
    CameraLeaveScrollLock();
    if (gCurTask->unk18 == 1)
    {
        if (gUnk_02007D64 == 4)
            sub_0802d5b4(gCurTask->unk1C, gCurTask->unk20);
        else
            sub_0802d478(gCurTask->unk1C, gCurTask->unk20);
    }
    else if (gCurTask->unk18 == 2)
    {
        PlaySfx(159);
        sub_080261c0((gCurTask->unk1C & 0xFFF0) + 8, (gCurTask->unk20 & 0xFFF0) + 8);
        sub_08030f78(gCurTask->unk1C >> 4, gCurTask->unk20 >> 4);
        sub_08030f78(gCurTask->unk1C >> 4, (gCurTask->unk20 >> 4) - 1);
    }
    TaskExitTrampoline();
}

s32 sub_0802d478(s32 x, s32 y)
{
    s32 id;
    struct Task *t;

    id = sub_0802621c(4);
    if (id != -1)
    {
        t = &gTasks[id];
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
    gCurTask->unk00 = 0;
    gCurTask->unk0C = 0;
    PlaySfx(159);
    sub_080301e8(2, gCurTask->unk48, gCurTask->unk4A - 8);
    if ((gCurTask->unk46 = sub_080301e8(1, gCurTask->unk48, gCurTask->unk4A - 8)) == -1)
        TaskExitTrampoline();
    gTasks[gCurTask->unk46].unk18 = 0;
    while (gTasks[gCurTask->unk46].unk18 == 0)
        TaskYieldTrampoline(1);
    sub_08030f78(gCurTask->unk48 >> 4, gCurTask->unk4A >> 4);
    sub_08030f78(gCurTask->unk48 >> 4, (gCurTask->unk4A - 16) >> 4);
    TaskYieldTrampoline(1);
    TaskFree(gCurTask->unk46);
    TaskExitTrampoline();
}

s32 sub_0802d5b4(s32 x, s32 y)
{
    s32 id;
    struct Task *t;

    id = sub_0802621c(4);
    if (id != -1)
    {
        t = &gTasks[id];
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
    gCurTask->unk00 = 0;
    gCurTask->unk0C = 0;
    sub_080301e8(5, gCurTask->unk48 + 2, gCurTask->unk4A);
    TaskYieldTrampoline(2);
    sub_08030f78(gCurTask->unk48 >> 4, gCurTask->unk4A >> 4);
    TaskYieldTrampoline(4);
    sub_080301e8(5, gCurTask->unk48 + 2, gCurTask->unk4A - 12);
    TaskYieldTrampoline(2);
    sub_08030f78(gCurTask->unk48 >> 4, (gCurTask->unk4A - 16) >> 4);
    TaskYieldTrampoline(4);
    sub_080301e8(5, gCurTask->unk48 + 2, gCurTask->unk4A - 24);
    TaskYieldTrampoline(2);
    sub_08030f78(gCurTask->unk48 >> 4, (gCurTask->unk4A - 32) >> 4);
    TaskYieldTrampoline(4);
    TaskExitTrampoline();
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

    t = gCurTask;
    t->unk00 = 0;
    t->unk0C = 0;
    m = gCurRoomDef;
    p18 = m->unk18;
    t->unk54 = *p18 >> 1;
    p28 = m->unk28;
    t->unk58 = *p28 >> 1;
    t->unk60 = (s32)((u8 *)(gBgPalette + 0x100) - *p28);
    t->unk28 = (s32)(p18 + 1);
    t->unk2C = (s32)(p28 + 1);
    t->unk30 = (s32)(gRoomTable[gLevelIndex][gStageIndex][gRoomIndex + 1]->unk18 + 1);
    t->unk34 = (s32)(gRoomTable[gLevelIndex][gStageIndex][gRoomIndex + 1]->unk28 + 1);
    t->unk6C = 0;
    do
    {
        t1 = gCurTask;
        BlendColors((u16 *)t1->unk28, (u16 *)t1->unk30, (u16)((s16)t1->unk6C * 8), (u16)t1->unk54, gBgPalette + 0x20);
        u1 = gCurTask;
        BlendColors((u16 *)u1->unk2C, (u16 *)u1->unk34, (u16)((s16)u1->unk6C * 8), (u16)u1->unk58, (u16 *)u1->unk60);
        BlendColors(gUnk_02005E10 + 0x80, gUnk_02005E10, (u16)((s16)gCurTask->unk6C * 8), 96, gBgPalette + 0x180);
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 32);
    TaskYieldTrampoline(8);
    gCurTask->unk6E = 0;
    do
    {
        gCurTask->unk6C = 0;
        do
        {
            t2 = gCurTask;
            BlendColors((u16 *)t2->unk30, (u16 *)t2->unk28, (u16)((s16)t2->unk6C * 64), (u16)t2->unk54, gBgPalette + 0x20);
            u2 = gCurTask;
            BlendColors((u16 *)u2->unk34, (u16 *)u2->unk2C, (u16)((s16)u2->unk6C * 64), (u16)u2->unk58, (u16 *)u2->unk60);
            BlendColors(gUnk_02005E10, gUnk_02005E10 + 0x80, (u16)((s16)gCurTask->unk6C * 64), 96, gBgPalette + 0x180);
            TaskYieldTrampoline(1);
        } while ((s16)++gCurTask->unk6C <= 4);
        gCurTask->unk6C = 0;
        do
        {
            t3 = gCurTask;
            BlendColors((u16 *)t3->unk28, (u16 *)t3->unk30, (u16)((s16)t3->unk6C * 64), (u16)t3->unk54, gBgPalette + 0x20);
            u3 = gCurTask;
            BlendColors((u16 *)u3->unk2C, (u16 *)u3->unk34, (u16)((s16)u3->unk6C * 64), (u16)u3->unk58, (u16 *)u3->unk60);
            BlendColors(gUnk_02005E10 + 0x80, gUnk_02005E10, (u16)((s16)gCurTask->unk6C * 64), 96, gBgPalette + 0x180);
            TaskYieldTrampoline(1);
        } while ((s16)++gCurTask->unk6C <= 4);
        TaskYieldTrampoline(4);
    } while (++gCurTask->unk6E <= 1);
    gUnk_02006098[0] = 1;
    TaskExitTrampoline();
}

void sub_0802d96c(void)
{
    struct Task *t;
    struct RoomDef *m;

    t = gCurTask;
    t->unk00 = 0;
    t->unk0C = 0;
    m = gCurRoomDef;
    t->unk64 = *m->unk18 >> 1;
    t->unk68 = *m->unk28 >> 1;
    t->unk60 = (s32)(gObjPalette - *m->unk28);
    t->unk28 = (s32)(gRoomTable[gLevelIndex][gStageIndex][gUnk_087328BC[gUnk_02006098[1]]]->unk18 + 1);
    t->unk2C = (s32)(gRoomTable[gLevelIndex][gStageIndex][gUnk_087328BC[gUnk_02006098[1]]]->unk28 + 1);
    t->unk30 = (s32)(gRoomTable[gLevelIndex][gStageIndex][gUnk_087328BC[gUnk_02006098[2]]]->unk18 + 1);
    t->unk34 = (s32)(gRoomTable[gLevelIndex][gStageIndex][gUnk_087328BC[gUnk_02006098[2]]]->unk28 + 1);
    t->unk04 = (u32)sub_0802da8c;
    if (gUnk_02006098[4] > 0)
        t->unk6C = 0;
    else
        t->unk6C = 0x100;
    TaskSleepForever();
    TaskExitTrampoline();
}

void sub_0802da8c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    if (gUnk_02006098[4] > 0)
    {
        gCurTask->unk6C += 16;
        if ((s16)gCurTask->unk6C > 0x100)
            gCurTask->unk6C = 0x100;
    }
    else
    {
        gCurTask->unk6C -= 16;
        if ((s16)gCurTask->unk6C < 0)
            gCurTask->unk6C = 0;
    }
    t = gCurTask;
    BlendColors((u16 *)t->unk2C, (u16 *)t->unk34, t->unk6C, (u16)t->unk68, (u16 *)t->unk60);
    if (gUnk_02006098[4] > 0 && (s16)(u = gCurTask)->unk6C == 0x100)
    {
        if (gUnk_02006098[3] != -1)
        {
            gUnk_02006098[1] = gUnk_02006098[2];
            gUnk_02006098[2] = gUnk_02006098[3];
            ((struct Unk02006098Bits *)gUnk_02006098)->unk3 = -1;
            u->unk28 = u->unk30;
            u->unk2C = u->unk34;
            u->unk30 = (s32)(gRoomTable[gLevelIndex][gStageIndex][gUnk_087328BC[gUnk_02006098[2]]]->unk18 + 1);
            u->unk34 = (s32)(gRoomTable[gLevelIndex][gStageIndex][gUnk_087328BC[gUnk_02006098[2]]]->unk28 + 1);
            u->unk6C = 0;
            gUnk_02006098[0] = gUnk_02006098[1] | 0x80;
        }
        else
        {
            gUnk_02006098[0] = gUnk_02006098[2];
            TaskFree(gCurTaskIdx);
        }
    }
    else if (gUnk_02006098[4] < 0 && (s16)(v = gCurTask)->unk6C == 0)
    {
        if (gUnk_02006098[3] != -1)
        {
            gUnk_02006098[2] = gUnk_02006098[1];
            gUnk_02006098[1] = gUnk_02006098[3];
            ((struct Unk02006098Bits *)gUnk_02006098)->unk3 = -1;
            v->unk30 = v->unk28;
            v->unk34 = v->unk2C;
            v->unk28 = (s32)(gRoomTable[gLevelIndex][gStageIndex][gUnk_087328BC[gUnk_02006098[1]]]->unk18 + 1);
            v->unk2C = (s32)(gRoomTable[gLevelIndex][gStageIndex][gUnk_087328BC[gUnk_02006098[1]]]->unk28 + 1);
            v->unk6C = 0x100;
            gUnk_02006098[0] = gUnk_02006098[2] | 0x80;
        }
        else
        {
            gUnk_02006098[0] = gUnk_02006098[1];
            TaskFree(gCurTaskIdx);
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

    t1 = gCurTask;
    t1->unk00 = 0;
    t1->unk0C = 0;
    t1->unk1C = gStageIndex * 6 + gUnk_0200001C - 1;
    t1->unk54 = t1->unk28 = gUnk_08732428[t1->unk1C][1];
    if (t1->unk28 > 0)
        t1->unk2C = 4;
    else if (t1->unk28 < 0)
    {
        t1->unk28 = -t1->unk28;
        t1->unk2C = -4;
    }
    t2 = gCurTask;
    t2->unk58 = t2->unk30 = gUnk_08732428[t2->unk1C][2];
    if (t2->unk30 > 0)
        t2->unk34 = 4;
    else if (t2->unk30 < 0)
    {
        t2->unk30 = -t2->unk30;
        t2->unk34 = -4;
    }
    t3 = gCurTask;
    t3->unk48 = gCameraAnchorX;
    t3->unk4A = gCameraAnchorY;
    if (gUnk_08732428[t3->unk1C][0] == 0)
    {
        for (t3->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk28; gCurTask->unk6C++)
        {
            t4 = gCurTask;
            if ((t4->unk54 > 0 && gCameraAnchorX >= gRoomBounds[1]) || (t4->unk54 < 0 && gCameraAnchorX <= gRoomBounds[0]))
                break;
            gCameraAnchorX += t4->unk2C;
            TaskYieldTrampoline(1);
        }
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk30; gCurTask->unk6C++)
        {
            t5 = gCurTask;
            if ((t5->unk58 > 0 && gCameraAnchorY >= gRoomBounds[3]) || (t5->unk58 < 0 && gCameraAnchorY <= gRoomBounds[2]))
                break;
            gCameraAnchorY += t5->unk34;
            TaskYieldTrampoline(1);
        }
    }
    else
    {
        for (t3->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk30; gCurTask->unk6C++)
        {
            t6 = gCurTask;
            if ((t6->unk54 > 0 && gCameraAnchorX == gRoomBounds[1]) || (t6->unk54 < 0 && gCameraAnchorX == gRoomBounds[0]))
                break;
            gCameraAnchorY += t6->unk34;
            TaskYieldTrampoline(1);
        }
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk28; gCurTask->unk6C++)
        {
            t7 = gCurTask;
            if ((t7->unk58 > 0 && gCameraAnchorY == gRoomBounds[3]) || (t7->unk58 < 0 && gCameraAnchorY == gRoomBounds[2]))
                break;
            gCameraAnchorX += t7->unk2C;
            TaskYieldTrampoline(1);
        }
    }
    TaskYieldTrampoline(15);
    sub_080307e8();
    TaskYieldTrampoline(1);
    gCurTask->unk20 = gCurTask->unk1C * 2;
    if (sub_08032288(gUnk_0200AFE0[0], gUnk_0200AFE0[1]))
        sub_08032338();
    if (gUnk_0200AFE0[2] != -1 && sub_08032288(gUnk_0200AFE0[2], gUnk_0200AFE0[3]))
        sub_08032338();
    while (1)
    {
        t8 = gCurTask;
        if (t8->unk24 != 0 && gUnk_02007FC4 == 0)
        {
            t8->unk4C = gUnk_087323C6[gUnk_030023B8][0];
            t8->unk50 = gUnk_087323C6[gUnk_030023B8][1];
            t8->unk5C = gRoomWidth * t8->unk50 + t8->unk4C;
            for (t8->unk6C = 0; (s16)gCurTask->unk6C <= 1; gCurTask->unk6C++)
            {
                for (gCurTask->unk6E = 0; gCurTask->unk6E <= 1; gCurTask->unk6E++)
                {
                    t9 = gCurTask;
                    if (gBg1MetatileMap[t9->unk5C + (s16)t9->unk6C + gRoomWidth * t9->unk6E] != 0)
                        goto skip;
                }
            }
            gUnk_02007FC4++;
            gCurTask->unk24 = 0;
        }
    skip:
        t10 = gCurTask;
        t10->unk20 = 0;
        for (t10->unk6C = 0; (s16)gCurTask->unk6C <= 63; gCurTask->unk6C++)
        {
            if (gUnk_0200A6F0[(s16)gCurTask->unk6C].unk6 != 0x7FFF)
            {
                gCurTask->unk20++;
                break;
            }
        }
        if (gCurTask->unk20 == 0)
            break;
        TaskYieldTrampoline(1);
    }
    PauseBlockAnims();
    RequestCopy(6, 0, 0x06001800, 0x800);
    if (gCurTask->unk24 != 0 && gUnk_02007FC4 == 0)
        gUnk_02007FC4++;
    TaskYieldTrampoline(20);
    sub_08029b30();
    gUnk_0200AF08 = 0;
    if (gUnk_08732428[gCurTask->unk1C][0] == 0)
    {
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk28 && gCameraAnchorX != gCurTask->unk48; gCurTask->unk6C++)
        {
            t11 = gCurTask;
            gCameraAnchorX -= t11->unk2C;
            d1 = t11->unk54;
            if ((d1 > 0 && gCameraAnchorX < t11->unk48) || (d1 < 0 && gCameraAnchorX > t11->unk48))
                gCameraAnchorX = gCurTask->unk48;
            TaskYieldTrampoline(1);
        }
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk30 && gCameraAnchorX != gCurTask->unk4A; gCurTask->unk6C++)
        {
            t12 = gCurTask;
            gCameraAnchorY -= t12->unk34;
            d2 = t12->unk58;
            if ((d2 > 0 && gCameraAnchorY < t12->unk4A) || (d2 < 0 && gCameraAnchorY > t12->unk4A))
                gCameraAnchorY = gCurTask->unk4A;
            TaskYieldTrampoline(1);
        }
    }
    else
    {
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk30 && gCameraAnchorX != gCurTask->unk4A; gCurTask->unk6C++)
        {
            gCameraAnchorY -= gCurTask->unk34;
            d3 = gCurTask->unk58;
            if ((d3 > 0 && gCameraAnchorY < gCurTask->unk4A) || (d3 < 0 && gCameraAnchorY > gCurTask->unk4A))
                gCameraAnchorY = gCurTask->unk4A;
            TaskYieldTrampoline(1);
        }
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk28 && gCameraAnchorX != gCurTask->unk48; gCurTask->unk6C++)
        {
            t13 = gCurTask;
            gCameraAnchorX -= t13->unk2C;
            d4 = t13->unk54;
            if ((d4 > 0 && gCameraAnchorX < t13->unk48) || (d4 < 0 && gCameraAnchorX > t13->unk48))
                gCameraAnchorX = gCurTask->unk48;
            TaskYieldTrampoline(1);
        }
    }
    TaskYieldTrampoline(10);
    if (gUnk_020055D4 == 0x2000)
        gUnk_020055D4 = 0x4000;
    gUnk_020055E8 = 1;
    TaskExitTrampoline();
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

    t1 = gCurTask;
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
    t2 = gCurTask;
    t2->unk58 = t2->unk30 = gUnk_087324A6[t2->unk1C][2];
    if (t2->unk30 > 0)
        t2->unk34 = 4;
    else if (t2->unk30 < 0)
    {
        t2->unk30 = -t2->unk30;
        t2->unk34 = -4;
    }
    t3 = gCurTask;
    t3->unk48 = gCameraAnchorX;
    t3->unk4A = gCameraAnchorY;
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(20);
    if (gUnk_087324A6[gCurTask->unk1C][0] == 0)
    {
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk28; gCurTask->unk6C++)
        {
            t4 = gCurTask;
            if ((t4->unk54 > 0 && gCameraAnchorX >= gRoomBounds[1]) || (t4->unk54 < 0 && gCameraAnchorX <= gRoomBounds[0]))
                break;
            gCameraAnchorX += t4->unk2C;
            TaskYieldTrampoline(1);
        }
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk30; gCurTask->unk6C++)
        {
            t5 = gCurTask;
            if ((t5->unk58 > 0 && gCameraAnchorY >= gRoomBounds[3]) || (t5->unk58 < 0 && gCameraAnchorY <= gRoomBounds[2]))
                break;
            gCameraAnchorY += t5->unk34;
            TaskYieldTrampoline(1);
        }
    }
    else
    {
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk30; gCurTask->unk6C++)
        {
            t6 = gCurTask;
            if ((t6->unk58 > 0 && gCameraAnchorY >= gRoomBounds[3]) || (t6->unk58 < 0 && gCameraAnchorY <= gRoomBounds[2]))
                break;
            gCameraAnchorY += t6->unk34;
            TaskYieldTrampoline(1);
        }
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk28; gCurTask->unk6C++)
        {
            t7 = gCurTask;
            if ((t7->unk54 > 0 && gCameraAnchorX >= gRoomBounds[1]) || (t7->unk54 < 0 && gCameraAnchorX <= gRoomBounds[0]))
                break;
            gCameraAnchorX += t7->unk2C;
            TaskYieldTrampoline(1);
        }
    }
    TaskYieldTrampoline(15);
    sub_080307e8();
    TaskYieldTrampoline(1);
    gCurTask->unk20 = gCurTask->unk1C * 2;
    if (sub_08032288(gUnk_0200AFE0[0], gUnk_0200AFE0[1]))
        sub_08032338();
    if (gUnk_0200AFE0[2] != -1 && sub_08032288(gUnk_0200AFE0[2], gUnk_0200AFE0[3]))
        sub_08032338();
    while (1)
    {
        t8 = gCurTask;
        if (t8->unk24 != 0 && gUnk_02007FC4 == 0)
        {
            t8->unk4C = gUnk_087323C6[gUnk_030023B8][0];
            t8->unk50 = gUnk_087323C6[gUnk_030023B8][1];
            t8->unk5C = gRoomWidth * t8->unk50 + t8->unk4C;
            for (t8->unk6C = 0; (s16)gCurTask->unk6C <= 1; gCurTask->unk6C++)
            {
                for (gCurTask->unk6E = 0; gCurTask->unk6E <= 1; gCurTask->unk6E++)
                {
                    t9 = gCurTask;
                    if (gBg1MetatileMap[t9->unk5C + (s16)t9->unk6C + gRoomWidth * t9->unk6E] != 0)
                        goto skip;
                }
            }
            gUnk_02007FC4++;
            gCurTask->unk24 = 0;
        }
    skip:
        t10 = gCurTask;
        t10->unk20 = 0;
        for (t10->unk6C = 0; (s16)gCurTask->unk6C <= 63; gCurTask->unk6C++)
        {
            if (gUnk_0200A6F0[(s16)gCurTask->unk6C].unk6 != 0x7FFF)
            {
                gCurTask->unk20++;
                break;
            }
        }
        if (gCurTask->unk20 == 0)
            break;
        TaskYieldTrampoline(1);
    }
    PauseBlockAnims();
    RequestCopy(6, 0, 0x06001800, 0x800);
    if (gCurTask->unk24 != 0 && gUnk_02007FC4 == 0)
        gUnk_02007FC4++;
    TaskYieldTrampoline(20);
    gUnk_0200AF08 = 0;
    if (gUnk_087324A6[gCurTask->unk1C][0] == 0)
    {
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk28 && gCameraAnchorX != gCurTask->unk48; gCurTask->unk6C++)
        {
            t13 = gCurTask;
            gCameraAnchorX -= t13->unk2C;
            d1 = t13->unk54;
            if ((d1 > 0 && gCameraAnchorX < t13->unk48) || (d1 < 0 && gCameraAnchorX > t13->unk48))
                gCameraAnchorX = gCurTask->unk48;
            TaskYieldTrampoline(1);
        }
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk30 && gCameraAnchorX != gCurTask->unk4A; gCurTask->unk6C++)
        {
            t14 = gCurTask;
            gCameraAnchorY -= t14->unk34;
            d2 = t14->unk58;
            if ((d2 > 0 && gCameraAnchorY < t14->unk4A) || (d2 < 0 && gCameraAnchorY > t14->unk4A))
                gCameraAnchorY = gCurTask->unk4A;
            TaskYieldTrampoline(1);
        }
    }
    else
    {
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk30 && gCameraAnchorX != gCurTask->unk4A; gCurTask->unk6C++)
        {
            t15 = gCurTask;
            gCameraAnchorY -= t15->unk34;
            d3 = t15->unk58;
            if ((d3 > 0 && gCameraAnchorY < t15->unk4A) || (d3 < 0 && gCameraAnchorY > t15->unk4A))
                gCameraAnchorY = gCurTask->unk4A;
            TaskYieldTrampoline(1);
        }
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < gCurTask->unk28 && gCameraAnchorX != gCurTask->unk48; gCurTask->unk6C++)
        {
            t16 = gCurTask;
            gCameraAnchorX -= t16->unk2C;
            d4 = t16->unk54;
            if ((d4 > 0 && gCameraAnchorX < t16->unk48) || (d4 < 0 && gCameraAnchorX > t16->unk48))
                gCameraAnchorX = gCurTask->unk48;
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
    TaskExitTrampoline();
}
