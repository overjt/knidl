#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "menu.h"

/* menutask_0ea0c.c (0x0800EA0C-0x0800F17F, issue #99).
 *
 * Menu sprite and palette tasks, second part.  sub_0800ea0c is the
 * per-frame body of task type #243 (two sprites plus a palette
 * cross-fade); task type #244 (sub_0800eae4, body sub_0800ec08) slides a
 * sprite pair in, bobs it and slides it out; task types #254
 * (Task_SoundTestCursors, body sub_0800ecb8) and #255 (Task_SoundTestPulse) animate the
 * sound-test screen (menu screen 7: the cursor sprites and the palette
 * pulse of the selected column, which stays lit while its song plays);
 * task types #249 (sub_0800ef30) and #250 (sub_0800f084) cycle the
 * palettes of the link-play screen (menu screen 8) through BlendColors
 * blends. */

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void TaskMove(void);
u8 TaskIsOnScreenNoCamera(void);
void SetBlend(s32 a, s32 b, s32 c, s32 d);
void sub_0800ec08(void);
void sub_0800ecb8(void);

void sub_0800ea0c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *x;
    s32 n;
    s32 k;

    if (TaskIsOnScreenNoCamera()) {
        struct Task *s = gCurTask;
        u32 *tbl = s->frameTable;

        QueueSprite(3, tbl[8], 0, 0, s->pixelX, s->pixelY);
        QueueSprite(2, tbl[9], 0, 0, gCurTask->pixelX, gCurTask->pixelY);
    }
    t = gCurTask;
    if (t->unk34 == 256) {
        if (++t->unk2C > 1)
            t->unk2C = 0;
        u = gCurTask;
        if (++u->unk30 > 1)
            u->unk30 = 0;
        gCurTask->unk34 = 0;
    }
    v = gCurTask;
    n = v->unk34 + 32;
    v->unk34 = n;
    if (n > 256)
        v->unk34 = 256;
    k = gMenuChoiceCursor * 2;
    x = gCurTask;
    BlendColors(gUnk_08559BA4[k + x->unk2C], gUnk_08559BA4[k + x->unk30], (u16)x->unk34, 16, gUnk_030015D0);
}

void sub_0800eae4(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = (u32)TaskMove;
    t->drawCallback = 0;
    t->updateCallback = (u32)sub_0800ec08;
    t->frameTable = gUnk_08755650;
    t->unk28 = 0;
    t->unk2C = 0;
    for (t->unk6C = 0; (s16)gCurTask->unk6C <= 9; gCurTask->unk6C++) {
        struct Task *u = gCurTask;

        u->unk28 += ((s16)u->unk6C + 1) * 9 << 15;
        TaskYieldTrampoline(1);
    }
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 1; gCurTask->unk6C++) {
        for (gCurTask->unk6E = 0; gCurTask->unk6E <= 1; gCurTask->unk6E++) {
            gCurTask->unk2C += 0x60000;
            TaskYieldTrampoline(1);
        }
        for (gCurTask->unk6E = 0; gCurTask->unk6E <= 1; gCurTask->unk6E++) {
            gCurTask->unk2C -= 0x60000;
            TaskYieldTrampoline(1);
        }
    }
    TaskYieldTrampoline(16);
    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 9; gCurTask->unk6C++) {
        struct Task *u = gCurTask;

        u->unk28 -= ((s16)u->unk6C + 1) * 9 << 15;
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void sub_0800ec08(void)
{
    if (TaskIsOnScreenNoCamera()) {
        struct Task *t = gCurTask;
        u32 *tbl = t->frameTable;

        QueueSprite(1, tbl[10], 0, 0, (t->unk28 >> 16) - 127, (t->unk2C >> 16) + 144);
        QueueSprite(1, tbl[11], 0, 0, 0x16F - (gCurTask->unk28 >> 16), (gCurTask->unk2C >> 16) + 144);
    }
}

void Task_SoundTestCursors(void)
{
    struct Task *t = gCurTask;

    t->moveCallback = 0;
    t->drawCallback = 0;
    t->updateCallback = (u32)sub_0800ecb8;
    t->unk2C = 0;
    t->unk30 = 1;
    t->unk34 = 0;
    while (gMenuScreen == 7)
        TaskYieldTrampoline(1);
    TaskExitTrampoline();
}

void sub_0800ecb8(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    s32 v;

    if (TaskIsOnScreenNoCamera()) {
        u32 *tbl = gUnk_087556D4;

        QueueSprite(9, tbl[0], 0, 0, gMenuCursor * 52 + 94, 136);
        QueueSprite(8, tbl[1], 0, 0, 94, 136);
        QueueSprite(8, tbl[2], 0, 0, 146, 136);
    }
    t = gCurTask;
    if (t->unk34 == 256) {
        t->unk2C ^= 1;
        t->unk30 ^= 1;
        t->unk34 = 0;
    }
    u = gCurTask;
    v = u->unk34 + 32;
    u->unk34 = v;
    if (v > 256)
        u->unk34 = 256;
    w = gCurTask;
    BlendColors(gUnk_08564F38[w->unk2C], gUnk_08564F38[w->unk30], (u16)w->unk34, 5, gUnk_030015F4);
}

void Task_SoundTestPulse(void)
{
    struct Task *t = gCurTask;

    t->unk28 = 0;
    t->unk2C = 0;
    t->unk30 = 1;
    t->unk34 = 0;
    t->unk18 = 0;
    while (gMenuScreen == 7) {
        struct Task *u;
        struct Task *v;
        struct Task *w;
        s32 n;

        u = gCurTask;
        if (u->unk34 == 256) {
            if (++u->unk2C > 5) {
                u->unk2C = 0;
                u->unk28 = 4;
                u->unk18 = 0;
            }
            v = gCurTask;
            if (++v->unk30 > 5)
                v->unk30 = 0;
            gCurTask->unk34 = 0;
        }
        v = gCurTask;
        if (v->unk28 != 0) {
            v->unk28--;
        } else {
            n = v->unk34 + 128;
            v->unk34 = n;
            if (n > 256)
                v->unk34 = 256;
        }
        if (gMenuCursor == 0) {
            if ((s32)gMPlayTable[gSongTable[gSoundTestSelection[gMenuCursor]].ms].info->status >= 0) {
                w = gCurTask;
                BlendColors(gUnk_085634D8[w->unk2C], gUnk_085634D8[w->unk30], (u16)w->unk34, 16, gUnk_03001310[gMenuCursor]);
            } else {
                BlendColors(gUnk_085634D8[5], gUnk_085634D8[5], (u16)gCurTask->unk34, 16, gUnk_03001310[gMenuCursor]);
            }
        } else {
            if (gPressedKeys & 1) {
                struct Task *x = gCurTask;

                x->unk18 = 1;
                x->unk28 = 0;
                x->unk2C = 0;
                x->unk30 = 1;
                x->unk34 = 0;
            }
            w = gCurTask;
            if (w->unk18 != 0)
                BlendColors(gUnk_085634D8[w->unk2C], gUnk_085634D8[w->unk30], (u16)w->unk34, 16, gUnk_03001310[gMenuCursor]);
            else
                BlendColors(gUnk_085634D8[5], gUnk_085634D8[5], (u16)w->unk34, 16, gUnk_03001310[gMenuCursor]);
        }
        RequestCopy(2, (u32)&gUnk_085634D8[0][(gMenuCursor + 6) * 16], (u32)gUnk_03001310[(s8)(gMenuCursor ^ 1)], 32);
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void sub_0800ef30(void)
{
    struct Task *t = gCurTask;

    t->unk28 = 0;
    t->unk2C = 0;
    t->unk30 = 1;
    t->unk34 = 0;
    t->unk18 = 0;
    SetBlend(68, 8, 0, 16);
    while (gMenuScreen == 8 || gMenuScreen == 9) {
        struct Task *u;
        struct Task *v;
        s32 n;

        {
            struct Task *s = gCurTask;

            if (s->unk18 <= 19) {
                s->unk18++;
                gBldAlphaEva = s->unk18 >> 2;
                gBldAlphaEvb = 16 - gBldAlphaEva;
            }
        }
        u = gCurTask;
        if (u->unk34 == 256) {
            if (++u->unk2C > 15) {
                u->unk2C = 0;
                if (++u->unk28 > 3)
                    u->unk28 = 0;
            }
            v = gCurTask;
            if (++v->unk30 > 15)
                v->unk30 = 0;
            gCurTask->unk34 = 0;
        }
        v = gCurTask;
        n = v->unk34 + 64;
        v->unk34 = n;
        if (n > 256)
            v->unk34 = 256;
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 3; gCurTask->unk6C++) {
            struct Task *w = gCurTask;
            s32 i = (s16)w->unk6C;

            if (i == w->unk28)
                BlendColors(gUnk_08560DBC[w->unk2C], gUnk_08560DBC[w->unk30], (u16)w->unk34, 16, gUnk_030012F0[i]);
            else
                RequestCopy(2, (u32)gUnk_08560F9C, (u32)gUnk_030012F0[i], 32);
        }
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}

void sub_0800f084(void)
{
    struct Task *t = gCurTask;

    t->unk28 = gPrevMenuScreen;
    t->unk2C = 0;
    t->unk30 = 1;
    t->unk34 = 0;
    while (gMenuScreen == 8 || gMenuScreen == 9) {
        struct Task *u;
        struct Task *v;
        struct Task *w;
        s32 n;

        u = gCurTask;
        if (u->unk34 == 256) {
            if (++u->unk2C > 6)
                u->unk2C = 0;
            v = gCurTask;
            if (++v->unk30 > 6)
                v->unk30 = 0;
            gCurTask->unk34 = 0;
        }
        v = gCurTask;
        n = v->unk34 + 16;
        v->unk34 = n;
        if (n > 256)
            v->unk34 = 256;
        w = gCurTask;
        if (w->unk28 == 3)
            BlendColors(gUnk_08561224[w->unk2C], gUnk_08561224[w->unk30], (u16)w->unk34, 10, gUnk_03001372);
        else
            BlendColors(gUnk_0856342C[w->unk2C], gUnk_0856342C[w->unk30], (u16)w->unk34, 10, gUnk_03001372);
        TaskYieldTrampoline(1);
    }
    TaskExitTrampoline();
}
