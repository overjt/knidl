#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* menutask_0f180.c (0x0800F180-0x0800FCBB, issue #99).
 *
 * Menu sprite tasks, last part: the link-play connection screen (menu
 * screens 8 and 9) and the menu's background tasks.  Task type #251
 * (Task_LinkPlayPlayerList, body sub_0800f2b4) spawns a #252 (sub_0800f390, body
 * sub_0800f408) and a #253 (sub_0800f5ec, body sub_0800f698) per player
 * and slides them as partners join (sub_0800ffd8); #256 (Task_MenuScreenTitle,
 * body sub_0800f840) is the menu screen's title sprite; #259
 * (sub_0800fa30) and #258 (sub_0800fb94) cycle and cross-fade the
 * background palettes when the menu screen changes. */

extern s8 gMenuScreen;
extern s8 gPrevMenuScreen;
extern s8 gUnk_02007FC8;
/* Plain u8 here (vu8 elsewhere): a volatile byte load expands to a load plus
   two shifts, which lengthens this address's live range in sub_0800f408 enough
   to lose r6 to the hoisted copy of &gCurTask. */
extern u8 gMultiBootStruct[];
extern u16 gUnk_030012B0[];
extern u16 gUnk_03001430[];
extern u16 gUnk_03001612[];
extern u16 gUnk_085563C8[];
extern u16 gUnk_08562FE4[][8];
extern u16 gUnk_08563024[][13];
extern u16 *gUnk_08731CF8[];
extern u16 *gUnk_08731D28[];
extern u16 gUnk_08731D58[];
extern u32 gUnk_08731D70[];
extern s32 gUnk_08731E58[][4];
extern s32 gUnk_08731E98[];
extern u16 gUnk_08731EA8[];
extern s16 gUnk_08731EB0[];
extern s16 gUnk_08731EB8[];
extern s16 gUnk_08731EC0[];
extern s32 gUnk_08731EC8[][4];
extern s32 gUnk_08731F08[][4];
extern s16 gUnk_08731F48[][4];
extern u32 gUnk_08755620[];
extern u32 gUnk_08755688[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 TaskCreateFrom(u32 type, s32 idx);
void TaskMove(void);
void TaskDrawScreen(void);
void TaskSleepForever(void);
void LoadGfxSet(u16 a0);
s32 sub_0800ffd8(void);
u8 TaskIsOnScreenNoCamera(void);
void sub_0800f2b4(void);
void sub_0800f408(void);
void sub_0800f698(void);
void sub_0800f840(void);

void Task_LinkPlayPlayerList(void)
{
    struct Task *t;
    struct Task *s;
    struct Task *u;
    struct Task *x;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk04 = (u32)sub_0800f2b4;
    gCurTask->unk42 = 7;
    t = gCurTask;
    t->unk38 = gUnk_08755688;
    t->unk4C = 0xC40000;
    t->unk50 = 0x340000;
    t->unk6C = 0;
    do {
        gCurTask->unk28 = TaskCreateFrom(252, 32);
        u = gCurTask;
        x = &gTasks[u->unk28];
        x->unk44 = gCurTaskIdx;
        x->unk18 = (s16)u->unk6C;
        gCurTask->unk28 = TaskCreateFrom(253, 32);
        u = gCurTask;
        x = &gTasks[u->unk28];
        x->unk44 = gCurTaskIdx;
        x->unk18 = (s16)u->unk6C;
    } while ((s16)++u->unk6C <= 3);
    s = gCurTask;
    s->unk2C = 0;
    s->unk30 = 1;
    s->unk34 = 0;
    s->unk18 = 0;
    s->unk1C = 0;
    s->unk20 = 0;
    while (gMenuScreen == 8 || gMenuScreen == 9) {
        s32 n = sub_0800ffd8();
        struct Task *v;

        u = gCurTask;
        u->unk24 = n;
        if (u->unk20 == 0 || --u->unk20 == 0) {
            v = gCurTask;
            if (v->unk1C < v->unk24) {
                v->unk18 = 1;
                v->unk1C = v->unk1C + 1;
                v->unk20 = 6;
            } else if (v->unk1C > v->unk24) {
                v->unk18 = -1;
                v->unk1C = v->unk1C - 1;
                v->unk20 = 6;
            } else {
                v->unk18 = 0;
            }
        }
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void sub_0800f2b4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    s32 n;

    t = gCurTask;
    if (t->unk34 == 256) {
        if (++t->unk2C > 3)
            t->unk2C = 0;
        u = gCurTask;
        if (++u->unk30 > 3)
            u->unk30 = 0;
        gCurTask->unk34 = 0;
    }
    v = gCurTask;
    n = v->unk34 + 16;
    v->unk34 = n;
    if (n > 256)
        v->unk34 = 256;
    w = gCurTask;
    BlendColors(gUnk_08562FE4[w->unk2C], gUnk_08562FE4[w->unk30], (u16)w->unk34, 8, gUnk_03001612);
    y = gCurTask;
    BlendColors(gUnk_08563024[y->unk2C], gUnk_08563024[y->unk30], (u16)y->unk34, 13, &gUnk_03001612[16]);
    x = gCurTask;
    if (x->unk20 == 0 && x->unk24 != 0 && gMultiBootStruct[0] == 0 && gUnk_02007FC8 == 0)
        x->unk3C = 17;
    else
        gCurTask->unk3C = 0xFFFF;
}

void sub_0800f390(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk04 = (u32)sub_0800f408;
    gCurTask->unk42 = gCurTask->unk18 * 2 + 9;
    gCurTask->unk38 = gUnk_08755688;
    gCurTask->unk4C = 0x780000;
    gCurTask->unk50 = 0x480000;
    gCurTask->unk2C = 0;
    while (gMenuScreen == 8 || gMenuScreen == 9)
        TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_0800f408(void)
{
    struct Task *t = gCurTask;
    struct Task *p = &gTasks[t->unk44];
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    u32 *tbl;
    s32 k;
    s32 g;

    if (p->unk18 != 0) {
        if (p->unk18 == 1) {
            t->unk28 = p->unk1C;
            if (p->unk1C == t->unk18 && gMultiBootStruct[0] == 0)
                t->unk2C = 24;
        } else {
            t->unk28 = p->unk1C + 1;
        }
        u = gCurTask;
        u->unk54 = gUnk_08731E58[u->unk28][u->unk18] * p->unk18;
        u->unk58 = gUnk_08731E98[u->unk28] * p->unk18;
    } else {
        t->unk54 = 0;
        t->unk58 = 0;
    }
    if (gUnk_02007FC8 == 0) {
        v = gCurTask;
        v->unk3C = gUnk_08731EA8[p->unk1C];
        if (TaskIsOnScreenNoCamera()) {
            w = gCurTask;
            tbl = w->unk38;
            QueueSprite(w->unk42 - 1, tbl[gUnk_08731EB0[p->unk1C]], 0, 0, w->unk48, w->unk4A);
            if (gMultiBootStruct[0] == (gCurTask->unk18 & 0xFF) && sub_0800ffd8() != 0) {
                x = gCurTask;
                if (p->unk1C >= x->unk18)
                    QueueSprite(x->unk42 - 1, tbl[gMultiBootStruct[0] + 13], 0, 0, x->unk48, x->unk4A + 16);
            }
            y = gCurTask;
            if (y->unk2C != 0 && --y->unk2C <= 17) {
                if (y->unk2C <= 3) {
                    k = (4 - y->unk2C) * 64 + 256;
                    y->unk30 = k;
                    g = DrawAffineSprite(tbl[18], k, k, 0);
                    z = gCurTask;
                    QueueSprite(z->unk42 - 1, g, 0, 0, z->unk48, z->unk4A + 28 + z->unk2C);
                } else {
                    QueueSprite(y->unk42 - 1, tbl[18], 0, 0, y->unk48, y->unk4A + 32);
                }
            }
        }
    } else {
        gCurTask->unk3C = 0xFFFF;
    }
}

void sub_0800f5ec(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk04 = (u32)sub_0800f698;
    gCurTask->unk42 = gCurTask->unk18 * 2 + 9;
    t = gCurTask;
    t->unk38 = gUnk_08755688;
    t->unk4C = gUnk_08731EB8[t->unk18] << 16;
    t->unk50 = gUnk_08731EC0[t->unk18] << 16;
    t->unk28 = 1;
    t->unk2C = 1;
    while (gMenuScreen == 8 || gMenuScreen == 9) {
        struct Task *u = gCurTask;

        if (u->unk18 == 0 && --u->unk28 == 0) {
            u->unk28 = 16;
            u->unk2C = -u->unk2C;
            u->unk58 = u->unk2C << 16;
        }
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void sub_0800f698(void)
{
    struct Task *t = gCurTask;
    struct Task *p = &gTasks[t->unk44];
    struct Task *u;
    s16 a;
    s16 b;

    if (t->unk18 == 0) {
        if (p->unk1C == 0 && p->unk18 == 0)
            t->unk3C = 6;
        else
            gCurTask->unk3C = 0xFFFF;
    } else if (p->unk18 != 0) {
        if (p->unk18 == -1)
            t->unk1C = p->unk1C + 1;
        else
            t->unk1C = p->unk1C;
        u = gCurTask;
        u->unk54 = gUnk_08731EC8[u->unk1C][u->unk18] * p->unk18;
        u->unk58 = gUnk_08731F08[u->unk1C][u->unk18] * p->unk18;
        a = gUnk_08731F48[p->unk1C][u->unk18];
        if (a == -1 || (b = gUnk_08731F48[p->unk1C - p->unk18][u->unk18]) == -1)
            u->unk3C = 0xFFFF;
        else if (p->unk20 > 3)
            u->unk3C = b;
        else
            u->unk3C = a;
    } else {
        t->unk3C = gUnk_08731F48[p->unk1C][t->unk18];
        t->unk54 = 0;
        t->unk58 = 0;
    }
    if (gUnk_02007FC8 == 1)
        gCurTask->unk3C = 0xFFFF;
}

void Task_MenuScreenTitle(void)
{
    struct Task *t;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk04 = (u32)sub_0800f840;
    gCurTask->unk38 = gUnk_08755620;
    gCurTask->unk42 = 7;
    t = gCurTask;
    t->unk28 = gMenuScreen;
    LoadGfxSet(gUnk_08731D70[t->unk28]);
    if (gMenuScreen == 7) {
        t = gCurTask;
        t->unk4C = 0x600000;
        t->unk50 = 0x200000;
        t->unk3C = 11;
    } else {
        t = gCurTask;
        t->unk4C = 0x640000;
        t->unk50 = 0xE0000;
        t->unk3C = 10;
    }
    gCurTask->unk2C = 3;
    TaskSleepForever();
}

void sub_0800f840(void)
{
    u32 *tbl = gUnk_08731D70;
    struct Task *t = gCurTask;
    struct Task *u;
    struct Task *v;
    s32 old = t->unk28;

    if (tbl[old] != tbl[gMenuScreen]) {
        if (gMenuScreen == 7 || old == 7) {
            t->unk2C = 0;
            t->unk30 = 8;
            if (old == 7) {
                t->unk4C = 0x600000;
                t->unk50 = 0x200000;
                t->unk3C = 11;
                t->unk54 = 0xFFE00000;
            } else {
                t->unk4C = 0x640000;
                t->unk50 = 0xE0000;
                t->unk3C = 10;
                t->unk54 = 0x200000;
            }
        } else {
            t->unk30 = (old == 11) ? 1 : 7;
            u = gCurTask;
            u->unk4C = 0x640000;
            u->unk50 = 0xE0000;
            u->unk54 = 0;
            if (gUnk_08731D70[gMenuScreen])
                u->unk2C = 2;
            else
                u->unk2C = 3;
        }
        gCurTask->unk28 = gMenuScreen;
    }
    v = gCurTask;
    switch (v->unk2C) {
    case 0:
        if (--v->unk30 == 0) {
            LoadGfxSet(gUnk_08731D70[v->unk28]);
            if (gMenuScreen == 7) {
                u = gCurTask;
                u->unk4C = 0xFFE00000;
                u->unk50 = 0x200000;
                u->unk3C = 11;
                u->unk54 = 0x100000;
            } else {
                u = gCurTask;
                u->unk4C = 0xE40000;
                u->unk50 = 0xE0000;
                u->unk3C = 10;
                u->unk54 = 0xFFF00000;
            }
            u = gCurTask;
            u->unk30 = 8;
            u->unk2C = 1;
        }
        break;
    case 2:
        if (--v->unk30 == 0) {
            if (gMenuScreen == 8 && gPrevMenuScreen == 5)
                LoadGfxSet(44);
            else
                LoadGfxSet(gUnk_08731D70[gCurTask->unk28]);
            if (gMenuScreen == 7)
                gCurTask->unk3C = 11;
            else
                gCurTask->unk3C = 10;
            gCurTask->unk2C = 3;
        }
        break;
    case 1:
        if (--v->unk30 == 0) {
            if (gMenuScreen == 7) {
                v->unk4C = 0x600000;
                v->unk50 = 0x200000;
            } else {
                v->unk4C = 0x640000;
                v->unk50 = 0xE0000;
            }
            gCurTask->unk2C = 3;
            gCurTask->unk54 = 0;
        }
        break;
    }
}

void sub_0800fa30(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *v, *w, *x, *y, *z, *a;
    u16 *p;
    s32 s;

    t->unk2C = 0;
    t->unk30 = 1;
    t->unk34 = 0;
    s = gMenuScreen;
    t->unk18 = s;
    t->unk1C = s;
    t->unk20 = 0;
    t->unk24 = 0;
    for (;;) {
        p = gUnk_08731D28[gMenuScreen];
        if (p != NULL && gUnk_08731D28[gCurTask->unk18] != p) {
            gCurTask->unk1C = gCurTask->unk18;
            gCurTask->unk18 = gMenuScreen;
            gCurTask->unk24 = 0;
            gCurTask->unk20 = 1;
        }
        u = gCurTask;
        if (u->unk34 == 256) {
            if (++u->unk2C > 5)
                u->unk2C = 0;
            v = gCurTask;
            if (++v->unk30 > 5)
                v->unk30 = 0;
            gCurTask->unk34 = 0;
        }
        w = gCurTask;
        if ((w->unk34 += 32) > 256)
            w->unk34 = 256;
        x = gCurTask;
        switch (x->unk20) {
        case 0:
            BlendColors(gUnk_08731D28[x->unk18] + x->unk2C * 16, gUnk_08731D28[x->unk18] + x->unk30 * 16, (u16)x->unk34, 16, gUnk_03001430 - 176);
            break;
        case 1:
            BlendColors(gUnk_08731D28[x->unk1C] + x->unk2C * 16, gUnk_08731D28[x->unk1C] + x->unk30 * 16, (u16)x->unk34, 16, gUnk_03001430);
            y = gCurTask;
            BlendColors(gUnk_08731D28[y->unk18] + y->unk2C * 16, gUnk_08731D28[y->unk18] + y->unk30 * 16, (u16)y->unk34, 16, gUnk_03001430 + 16);
            z = gCurTask;
            z->unk24 += 16;
            BlendColors(gUnk_03001430, gUnk_03001430 + 16, (u16)z->unk24, 16, gUnk_03001430 - 176);
            a = gCurTask;
            if (a->unk24 == 256) {
                a->unk20 = 0;
                a->unk24 = 0;
            }
            break;
        }
        TaskYieldTrampoline(1);
    }
}

void sub_0800fb94(void)
{
    struct Task *t = gCurTask;
    struct Task *u, *w;
    u16 *p;
    s32 s;

    s = gMenuScreen;
    t->unk18 = s;
    t->unk1C = s;
    t->unk20 = 0;
    t->unk24 = 0;
    LoadGfxSet(gUnk_08731D58[s]);
    RequestCopy(2, (u32)gUnk_08731CF8[gCurTask->unk18], (u32)gUnk_030012B0, 32);
    for (;;) {
        p = gUnk_08731CF8[gMenuScreen];
        if (p != NULL && gUnk_08731CF8[gCurTask->unk18] != p) {
            gCurTask->unk1C = gCurTask->unk18;
            gCurTask->unk18 = gMenuScreen;
            gCurTask->unk24 = 0;
            gCurTask->unk20 = 1;
        }
        u = gCurTask;
        if (u->unk20 != 0) {
            u->unk24 += 32;
            switch (u->unk20) {
            case 1:
                BlendColors(gUnk_08731CF8[u->unk1C], gUnk_085563C8, (u16)u->unk24, 16, gUnk_030012B0);
                w = gCurTask;
                if (w->unk24 == 256) {
                    w->unk24 = 0;
                    w->unk20 = 2;
                    LoadGfxSet(gUnk_08731D58[w->unk18]);
                }
                break;
            case 2:
                BlendColors(gUnk_085563C8, gUnk_08731CF8[u->unk18], (u16)u->unk24, 16, gUnk_030012B0);
                w = gCurTask;
                if (w->unk24 == 256) {
                    w->unk24 = 0;
                    w->unk20 = 0;
                }
                break;
            }
        }
        TaskYieldTrampoline(1);
    }
}
