#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* menutask_0daf8.c (0x0800DAF8-0x0800E313, issue #99).
 *
 * Menu sprite tasks, first part: the file-select screen.  Task types #238
 * and #239 (sub_0800daf8, sub_0800db64 with body sub_0800dbdc; three of
 * each, spawned by CreateFileSelectSprites) slide the three save-slot sprites in with
 * sub_0800dc98 and recolour a slot when the cursor moves; #240
 * (Task_FileSelectCursor, body sub_0800dda0) is the cursor; #241 (sub_0800de6c,
 * body sub_0800dfdc) a sprite group that slides with the screen; #242
 * (Task_FileMenuHighlight, body sub_0800e148) the file-menu highlight, which
 * marks the selected entry (MenuUpdateFileMenuPalette) and loads its picture
 * (MenuLoadPicture: LZ77 into 0x02020000, one 2 KiB part to 0x06004200). */

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

extern s8 gFileMenuCursor;
extern s8 gMenuScreen;
extern s8 gMenuCursor;
extern struct SaveSlot gSaveSlots[];
extern u32 gUnk_02020000[];
extern u16 gBgPalette[];
extern u16 gUnk_0300153C[];
extern u16 gUnk_03001668[];
extern s32 gCurSaveSlot;
extern u8 gUnk_08550B9C[];
extern u16 gUnk_08554B60[][4];
extern u16 gUnk_08554D7A[];
extern u16 gUnk_08554D80[];
extern u16 gUnk_08559B68[][10];
extern u16 gUnk_08559B90[];
extern void *const gUnk_08731E34[];
extern u32 gUnk_08755620[];
extern u32 gUnk_08755650[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void TaskMove(void);
void TaskDrawScreen(void);
void LoadGfxSet(u16 a0);
s32 sub_0800bf10(s32 slot, u32 pal);
u8 TaskIsOnScreenNoCamera(void);
void sub_0800dbdc(void);
void sub_0800dc98(void);
void sub_0800dda0(void);
void sub_0800dfdc(void);
void sub_0800e148(void);
void MenuUpdateFileMenuPalette(void);
s32 MenuLoadPicture(s32 id, s32 part);

void sub_0800daf8(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk42 = 8;
    gCurTask->unk38 = gUnk_08755620;
    gCurTask->unk3C = gCurTask->unk1C + 3;
    sub_0800dc98();
    while (gMenuScreen != 1)
        TaskYieldTrampoline(1);
    gCurTask->unk54 = 0x1AE000;
    TaskYieldTrampoline(10);
    TaskExitTrampoline();
}

void sub_0800db64(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk04 = (u32)sub_0800dbdc;
    gCurTask->unk42 = 9;
    gCurTask->unk38 = gUnk_08755620;
    gCurTask->unk3C = gCurTask->unk1C;
    gCurTask->unk28 = -1;
    sub_0800dc98();
    while (gMenuScreen != 1)
        TaskYieldTrampoline(1);
    gCurTask->unk54 = 0x1AE000;
    TaskYieldTrampoline(10);
    TaskExitTrampoline();
}

void sub_0800dbdc(void)
{
    struct Task *t;
    struct SaveSlot *s;
    s32 slot;
    s32 i;

    if (gMenuCursor != gCurTask->unk28) {
        slot = gCurTask->unk1C;
        s = gSaveSlots;
        i = slot * 256;
        if (gSaveSlots[slot].unk12[1] != 0 && gSaveSlots[slot].unk04 != 0x99999999)
            i++;
        sub_0800bf10(slot, (s8)s->unk16[i]);
        gCurTask->unk28 = gMenuCursor;
    }

    if (gSaveSlots[gCurTask->unk1C].unk12[1] != 0 && gSaveSlots[gCurTask->unk1C].unk04 != 0x99999999) {
        t = gCurTask;
        QueueSprite(t->unk42 - 1, gUnk_08755620[t->unk1C + 6], t->unk3E, t->unk40, t->unk48, t->unk4A);
    }
}

void sub_0800dc98(void)
{
    struct Task *t = gCurTask;
    s32 n;

    t->unk4C = 0x1500000;
    n = t->unk1C;
    t->unk50 = ((n + 1) * 5) << 19;
    TaskYieldTrampoline(n * 5);
    gCurTask->unk54 = 0xFFE52000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0;
}

void Task_FileSelectCursor(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk04 = (u32)sub_0800dda0;
    gCurTask->unk42 = 6;
    gCurTask->unk38 = gUnk_08755620;
    gCurTask->unk3C = 9;
    gCurTask->unk28 = -1;
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 1;
    gCurTask->unk34 = 0;
    gCurTask->unk4C = 0x160000;
    gCurTask->unk50 = ((gMenuCursor * 5) << 19) + 0x240000;
    gCurTask->unk54 = 0x30000;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0;
    while (gMenuScreen != 1) {
        gCurTask->unk50 = ((gMenuCursor * 5) << 19) + 0x240000;
        TaskYieldTrampoline(1);
    }
    gCurTask->unk54 = 0xFFFA0000;
    TaskYieldTrampoline(8);
    TaskExitTrampoline();
}

void sub_0800dda0(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *v, *w;
    s32 cur;

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
    w = gCurTask;
    BlendColors(gUnk_08554B60[w->unk2C], gUnk_08554B60[w->unk30], (u16)w->unk34, 4, gUnk_03001668);
    cur = gMenuCursor;
    if (cur != gCurTask->unk28) {
        u8 *p;
        RequestCopy(3, (u32)&gUnk_08550B9C[cur * 192], 0x06013580, 96);
        p = gUnk_08550B9C;
        RequestCopy(3, (u32)&p[gMenuCursor * 192 + 96], 0x06013980, 96);
        gCurTask->unk28 = gMenuCursor;
    }
}

void sub_0800de6c(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *v, *w;
    s8 s;

    t->unk00 = 0;
    t->unk0C = 0;
    t->unk04 = (u32)sub_0800dfdc;
    t->unk3E = 0x2000;
    t->unk28 = 0;
    t->unk18 = 0;
    t->unk1C = 0;
    t->unk20 = 0;
    t->unk6C = 0;
    for (; (s16)gCurTask->unk6C <= 7; gCurTask->unk6C++) {
        u = gCurTask;
        u->unk18 += 0xFFF90000;
        u->unk20 += 0xFFFA0000;
        TaskYieldTrampoline(1);
        s = gMenuScreen;
        if (s == 0 || s == 4 || s == 7 || s == 8) {
            gCurTask->unk28 = 1;
            break;
        }
    }
    for (;;) {
        TaskYieldTrampoline(1);
        s = gMenuScreen;
        if (s == 1) {
            v = gCurTask;
            if (v->unk18 < (s32)0xFFC80000) {
                v->unk3E = 0;
                v->unk18 += 0x33000;
                v->unk1C += 0xFFFD2000;
                if (v->unk18 > (s32)0xFFC80000) {
                    v->unk18 = 0xFFC80000;
                    v->unk1C = 0;
                }
            } else {
                v->unk3E = 0x2000;
            }
        } else if (s == 6) {
            v = gCurTask;
            v->unk3E = 0;
            if (v->unk18 > (s32)0xFFA80000) {
                v->unk18 += 0xFFFCD000;
                v->unk1C += 0x2E000;
                if (v->unk18 < (s32)0xFFA80000) {
                    v->unk18 = 0xFFA80000;
                    v->unk1C = 0x1D0000;
                }
            }
        } else if (s == 0 || s == 4 || s == 7 || s == 8) {
            gCurTask->unk28 = 1;
            break;
        }
    }
    gCurTask->unk6C = 0;
    do {
        w = gCurTask;
        w->unk18 += 0x70000;
        w->unk20 += 0x33000;
        TaskYieldTrampoline(1);
    } while ((s16)++gCurTask->unk6C <= 7);
    TaskExitTrampoline();
}

void sub_0800dfdc(void)
{
    struct Task *t;
    u32 *tbl;

    if (TaskIsOnScreenNoCamera()) {
        tbl = gUnk_08755650;
        QueueSprite(12, tbl[3], gCurTask->unk3E, 0, (gCurTask->unk18 >> 16) + 304, (gCurTask->unk1C >> 16) + 40);
        QueueSprite(12, tbl[2], gCurTask->unk3E, 0, (gCurTask->unk18 >> 16) + 304, (gCurTask->unk1C >> 16) + 40);
        QueueSprite(11, tbl[5], gCurTask->unk3E, 0, (gCurTask->unk18 >> 16) + 304, (gCurTask->unk1C >> 16) + 40);
        QueueSprite(13, tbl[4], gCurTask->unk3E, 0, (gCurTask->unk20 >> 16) + 296, 40);
        t = gCurTask;
        if (t->unk28 == 0) {
            tbl = gUnk_08755620;
            QueueSprite(11, tbl[gCurSaveSlot + 3], t->unk3E, 0, (t->unk18 >> 16) + 234, (t->unk1C >> 16) + 13);
        }
    }
}

void Task_FileMenuHighlight(void)
{
    s8 s;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = 0;
    gCurTask->unk04 = (u32)sub_0800e148;
    gCurTask->unk38 = gUnk_08755650;
    gCurTask->unk28 = -1;
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 1;
    gCurTask->unk34 = 0;
    gCurTask->unk4C = 0x640000;
    for (;;) {
        s = gMenuScreen;
        if (s == 0 || s == 4 || s == 7 || s == 8)
            break;
        gCurTask->unk50 = ((gFileMenuCursor * 3) << 19) + 0x300000;
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void sub_0800e148(void)
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
    if (gMenuScreen == 1) {
        w = gCurTask;
        BlendColors(gUnk_08559B68[w->unk2C], gUnk_08559B68[w->unk30], (u16)w->unk34, 10, gUnk_0300153C);
        LoadGfxSet(23);
        if (gFileMenuCursor != gCurTask->unk28) {
            MenuUpdateFileMenuPalette();
            MenuLoadPicture(0, gFileMenuCursor);
            gCurTask->unk28 = gFileMenuCursor;
        }
    } else {
        BlendColors(gUnk_08559B90, gUnk_08559B90, (u16)gCurTask->unk34, 10, gUnk_0300153C);
        LoadGfxSet(24);
        gCurTask->unk28 = -1;
    }
    if (TaskIsOnScreenNoCamera()) {
        tbl = gUnk_08755650;
        QueueSprite(6, tbl[1], 0, 0, gCurTask->unk48, gCurTask->unk4A);
        QueueSprite(10, tbl[0], 0, 0, gCurTask->unk48, gCurTask->unk4A);
    }
}

void MenuUpdateFileMenuPalette(void)
{
    s32 i;
    u16 *p;

    for (i = 0; i <= 3; i++) {
        p = gBgPalette;
        if (i == gFileMenuCursor)
            RequestCopy(2, (u32)gUnk_08554D7A, (u32)&p[i * 3 + 1], 6);
        else
            RequestCopy(2, (u32)gUnk_08554D80, (u32)&p[i * 3 + 1], 6);
    }
}

s32 MenuLoadPicture(s32 id, s32 part)
{
    LZ77UnCompWram(gUnk_08731E34[id], gUnk_02020000);
    RequestCopy(1, (u32)gUnk_02020000 + (part << 11), 0x06004200, 0x800);
}
