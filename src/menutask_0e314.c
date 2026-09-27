#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* menutask_0e314.c (0x0800E314-0x0800EA0B, issue #99).
 *
 * Menu sprite tasks, middle part.  Task types #245 (sub_0800e314, body
 * sub_0800e390) and #246 (sub_0800e46c, body sub_0800e518) show the
 * pictures and palette pulses of menu screens 2 and 3; #247
 * (sub_0800e5b0, body sub_0800e674) the mode list's picture and row
 * highlight (sub_0800e7b0); #248 (sub_0800e81c, body sub_0800e8c0) the
 * picture of screen 5; and sub_0800e9a4 is the entry of #243, whose
 * body sub_0800ea0c is in menutask_0ea0c.c. */

struct SaveSlot
{
    /*0x00*/ u32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ u32 unk08;
    /*0x0C*/ s32 unk0C;
    /*0x10*/ u16 unk10;
    /*0x12*/ u16 unk12[2];
    /*0x16*/ u8 unk16[2];
    /*0x18*/ u8 unk18[2];
    /*0x1A*/ u8 unk1A[2];
    /*0x1C*/ u8 unk1C[2];
    /*0x1E*/ u8 pad1E[2];
    /*0x20*/ u32 unk20[2];
    /*0x28*/ u8 unk28[8][7];
    /*0x60*/ u16 unk60[4];
    /*0x68*/ u16 unk68[4];
    /*0x70*/ u32 unk70;
    /*0x74*/ u8 filler74[0x8C];
};

extern s8 gModeListExtraRows;
extern s8 gUnk_02004B44;
extern s8 gMenuScreen;
extern u8 gEraseConfirmCount;
extern s8 gMenuChoiceCursor;
extern s8 gMenuCursor;
extern struct SaveSlot gSaveSlots[];
extern u16 gUnk_030012F0[][16];
extern u16 gUnk_0300153C[];
extern u16 gUnk_03001550[];
extern u16 gUnk_03001570[];
extern s32 gCurSaveSlot;
extern u16 gUnk_08559C24[][16];
extern u16 gUnk_08559CE6[];
extern u16 gUnk_08559CEC[];
extern u16 gUnk_0855D2F8[][10];
extern u16 gUnk_0855D320[];
extern u16 gUnk_0855D334[][16];
extern u16 gUnk_08731E4C[];
extern u16 gUnk_08731E52[];
extern u32 gUnk_08755650[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void TaskMove(void);
void TaskDrawScreen(void);
void LoadGfxSet(u16 a0);
s32 MenuLoadPicture(s32 id, s32 part);
void sub_0800ea0c(void);
u8 TaskIsOnScreenNoCamera(void);
void sub_0800e390(void);
void sub_0800e518(void);
void sub_0800e674(void);
void sub_0800e7b0(void);
void sub_0800e8c0(void);

void sub_0800e314(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk04 = (u32)sub_0800e390;
    gCurTask->unk42 = 6;
    gCurTask->unk38 = gUnk_08755650;
    gCurTask->unk3C = 7;
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 1;
    gCurTask->unk34 = 0;
    gCurTask->unk4C = 0xA00000;
    gCurTask->unk50 = 0x300000;
    while (gMenuScreen == 2 || gMenuScreen == 3)
        TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_0800e390(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *v, *w;
    s32 k;

    if (t->unk34 == 256) {
        if (++t->unk2C > 1)
            t->unk2C = 0;
        u = gCurTask;
        if (++u->unk30 > 1)
            u->unk30 = 0;
        gCurTask->unk34 = 0;
    }
    v = gCurTask;
    if ((v->unk34 += 32) > 256)
        v->unk34 = 256;
    if (gMenuScreen == 2) {
        k = gMenuChoiceCursor * 3;
        w = gCurTask;
        BlendColors(gUnk_08559C24[k + w->unk2C], gUnk_08559C24[k + w->unk30], (u16)w->unk34, 16, gUnk_03001570);
        MenuLoadPicture(1, gMenuChoiceCursor);
    } else {
        BlendColors((u16 *)gUnk_08559C24 + (gMenuChoiceCursor * 3 + 2) * 16, (u16 *)gUnk_08559C24 + (gMenuChoiceCursor * 3 + 2) * 16, (u16)gCurTask->unk34, 16, gUnk_03001570);
    }
}

void sub_0800e46c(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk04 = (u32)sub_0800e518;
    gCurTask->unk42 = 5;
    gCurTask->unk38 = gUnk_08755650;
    gCurTask->unk3C = 6;
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 1;
    gCurTask->unk34 = 0;
    if (gSaveSlots[gCurSaveSlot].unk10 & 4) {
        gCurTask->unk4C = 0xB00000;
        gCurTask->unk50 = (gMenuChoiceCursor << 20) + 0x280000;
    } else {
        gCurTask->unk4C = 0xA80000;
        gCurTask->unk50 = 0x300000;
    }
    while (gMenuScreen == 3)
        TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_0800e518(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *v, *w;
    s32 k;

    if (t->unk34 == 256) {
        if (++t->unk2C > 1)
            t->unk2C = 0;
        u = gCurTask;
        if (++u->unk30 > 1)
            u->unk30 = 0;
        gCurTask->unk34 = 0;
    }
    v = gCurTask;
    if ((v->unk34 += 32) > 256)
        v->unk34 = 256;
    k = gUnk_02004B44 * 3;
    w = gCurTask;
    BlendColors(gUnk_08559C24[k + w->unk2C], gUnk_08559C24[k + w->unk30], (u16)w->unk34, 16, gUnk_03001550);
    MenuLoadPicture(2, gUnk_02004B44);
}

void sub_0800e5b0(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = 0;
    gCurTask->unk04 = (u32)sub_0800e674;
    gCurTask->unk38 = gUnk_08755650;
    LoadGfxSet(32);
    sub_0800e7b0();
    if (gMenuScreen == 4)
        MenuLoadPicture(3, gMenuCursor);
    gCurTask->unk28 = gMenuCursor;
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 1;
    gCurTask->unk34 = 0;
    gCurTask->unk4C = 0x680000;
    while (1) {
        gCurTask->unk50 = (gUnk_08731E4C[gModeListExtraRows] + gUnk_08731E52[gModeListExtraRows] * gMenuCursor) << 16;
        if (gMenuScreen == 1 || gMenuScreen == 8)
            break;
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void sub_0800e674(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *v, *w;
    u32 *tbl;

    if (t->unk34 == 256) {
        if (++t->unk2C > 1)
            t->unk2C = 0;
        u = gCurTask;
        if (++u->unk30 > 1)
            u->unk30 = 0;
        gCurTask->unk34 = 0;
    }
    v = gCurTask;
    if ((v->unk34 += 32) > 256)
        v->unk34 = 256;
    if (gMenuScreen == 4) {
        w = gCurTask;
        BlendColors(gUnk_0855D2F8[w->unk2C], gUnk_0855D2F8[w->unk30], (u16)w->unk34, 10, gUnk_0300153C);
        if (gMenuCursor != gCurTask->unk28) {
            sub_0800e7b0();
            if (gMenuScreen == 4)
                MenuLoadPicture(3, gMenuCursor);
            gCurTask->unk28 = gMenuCursor;
        }
    } else {
        BlendColors(gUnk_0855D320, gUnk_0855D320, (u16)gCurTask->unk34, 10, gUnk_0300153C);
        gCurTask->unk28 = -1;
    }
    if (TaskIsOnScreenNoCamera()) {
        tbl = gUnk_08755650;
        QueueSprite(6, tbl[13], 0, 0, gCurTask->unk48, gCurTask->unk4A);
        QueueSprite(10, tbl[12], 0, 0, gCurTask->unk48, gCurTask->unk4A);
    }
}

void sub_0800e7b0(void)
{
    s32 i;
    u16 *p;

    for (i = 0; i < gModeListExtraRows + 3; i++) {
        p = gUnk_030012F0[0];
        if (i == gMenuCursor)
            RequestCopy(2, (u32)gUnk_08559CE6, (u32)&p[i * 3 + 1], 6);
        else
            RequestCopy(2, (u32)gUnk_08559CEC, (u32)&p[i * 3 + 1], 6);
    }
}

void sub_0800e81c(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk04 = (u32)sub_0800e8c0;
    gCurTask->unk42 = 5;
    gCurTask->unk38 = gUnk_08755650;
    gCurTask->unk3C = 6;
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 1;
    gCurTask->unk34 = 0;
    gCurTask->unk4C = 0xA80000;
    gCurTask->unk50 = (gUnk_08731E4C[gModeListExtraRows] + gUnk_08731E52[gModeListExtraRows] * gMenuCursor) << 16;
    while (gMenuScreen == 5)
        TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_0800e8c0(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *v, *w;
    s32 k;

    if (t->unk34 == 256) {
        if (++t->unk2C > 1)
            t->unk2C = 0;
        u = gCurTask;
        if (++u->unk30 > 1)
            u->unk30 = 0;
        gCurTask->unk34 = 0;
    }
    v = gCurTask;
    if ((v->unk34 += 32) > 256)
        v->unk34 = 256;
    if (gMenuScreen == 5) {
        k = gMenuChoiceCursor * 2;
        w = gCurTask;
        BlendColors(gUnk_0855D334[k + w->unk2C], gUnk_0855D334[k + w->unk30], (u16)w->unk34, 16, gUnk_03001550);
        MenuLoadPicture(4, gMenuCursor * 2 + gMenuChoiceCursor);
    } else {
        BlendColors((u16 *)gUnk_0855D334 + (gMenuChoiceCursor + 4) * 16, (u16 *)gUnk_0855D334 + (gMenuChoiceCursor + 4) * 16, (u16)gCurTask->unk34, 16, gUnk_03001550);
    }
}

void sub_0800e9a4(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = 0;
    gCurTask->unk04 = (u32)sub_0800ea0c;
    gCurTask->unk38 = gUnk_08755650;
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 1;
    gCurTask->unk34 = 0;
    gCurTask->unk4C = 0x800000;
    gCurTask->unk50 = 0x680000;
    while (gMenuScreen == 6 && gEraseConfirmCount != 2)
        TaskYieldTrampoline(1);
    TaskExitTrampoline();
}
