/* game_code_and_rodata_080653ec_0806ef5c 0x080BC0CC-0x080BD9E8
 * (issue #95, module M35, file 4 of 5).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BC0CC 0x080BD9E8 src/subgame_bc0cc.c --newpb
 *
 * Task type #94, the duel's sprite objects.  Task_QuickDrawObject dispatches the
 * table 0x087563B0 on Task.unk73, the kind its spawner wrote (ten function
 * pointers; the call passes 12):
 *
 *   0  QuickDrawPlayer  a player: a six-state machine over 0x08756468 (entry
 *                    coroutines) / 0x08756480 (per-frame hooks), started in
 *                    state 0, 1 or 5 by Task.unk74
 *   1  sub_080bcdac  a label / icon sprite (draw callback sub_080bcbfc)
 *   2  QuickDrawTimer  a two-digit counter (QuickDrawTimerCount counts to 99 and
 *                    mirrors the value into the parent's Task.unk20)
 *   3  sub_080bcfa4  a scripted fly-in
 *   4  sub_080bd06c  a four-frame effect at (120, 96)
 *   5  QuickDrawOpponent  the single-player opponent CreateQuickDrawOpponent spawns: a
 *                    five-state machine over 0x087564E4 / 0x087564FC, one
 *                    animation set per level Task.unk18 (0-4) and its
 *                    reaction time from 0x087564B0[unk74 * 5 + unk18]; own
 *                    graphics loader QuickDrawLoadOpponentGraphics, draw callback sub_080bd938
 *   6  sub_080bd110  a three-frame sprite
 *   7  sub_080bd7f0  a sprite drawn by TaskDrawScreen (0x08755B90)
 *   8  sub_080bd8ac  the same graphics as a per-player award: in
 *                    gPrevGameState == 5, sub_080bd828 passes 1000 / 3000 /
 *                    5000 or 10 to AddPlayerScoreNoHud, or 1 to sub_08009eb8
 *   9  sub_080bd9b0  a sprite drawn by sub_080bd938 (0x08755A68)
 *
 * sub_080bc1c4 places a player by the number of linked players and the
 * local id (x from 0x087563D8, then y / facing / animation from x);
 * sub_080bc0ec / sub_080bc168 slide it in and out.  gBrightness is a busy
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
extern u8 gQuickDrawWins[];
extern u8 gUnk_0200B048;
extern u32 gUnk_02020000[];
extern vs16 gBrightness;
extern u32 gUnk_03001570[];
extern u16 gPrevGameState;
extern u16 gLocalPlayer;
extern u16 gPlayerCount;
extern struct Task *gCurTask;
extern struct Task gTasks[];
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
extern u32 gQuickDrawObjectKinds[];
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

void TaskExitTrampoline(void);
void TaskYieldTrampoline(u32 frames);
void LZ77UnCompWram(const void *src, void *dest);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
void CallTableEntry(u32 a, u32 b, u32 *c);
s32 PlaySfx(s32 id);
void TaskFree(s32 id);
s32 TaskCreateFrom(u32 type, s32 idx);
void TaskMove(void);
void TaskDrawScreen(void);
void TaskSleepForever(void);
void TaskSetEntry(void *fn, u32 i);
void TaskStop(void);
void sub_08009eb8(u32 a, u32 b);
void AddPlayerScoreNoHud(u32 a, u32 b);

void sub_080bc1c4(void);
void QuickDrawPlayerEnterState(void);
void QuickDrawPlayerUpdate(void);
void sub_080bcf8c(void);
void QuickDrawOpponentEnterState(void);
void sub_080bd290(void);
void QuickDrawOpponentUpdate(void);

void Task_QuickDrawObject(void)
{
    CallTableEntry(gCurTask->unk73, 12, gQuickDrawObjectKinds);
}

void sub_080bc0ec(void)
{
    struct Task *t;
    struct Task *u;

    if (gPlayerCount <= 2)
    {
        t = gCurTask;
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
    u = gCurTask;
    u->unk4C = u->unk48 << 16;
    u->unk50 = u->unk4A << 16;
}

void sub_080bc168(void)
{
    struct Task *t;

    sub_080bc1c4();
    if (gPlayerCount <= 2)
    {
        switch (gCurTask->unk48)
        {
        case 68:
            gCurTask->unk48 = -44;
            break;
        case 164:
            gCurTask->unk48 = 276;
            break;
        }
        t = gCurTask;
        t->unk24 = 0;
        t->unk4C = t->unk48 << 16;
        t->unk50 = t->unk4A << 16;
    }
}

void sub_080bc1c4(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    s32 n;
    s32 k;

    t->unk3E &= 0x7FFF;
    t->unk40 = t->unk18 << 12;
    n = gPlayerCount - 1;
    k = gLocalPlayer - t->unk18;
    t->unk48 = gUnk_087563D8[n * 8 + k];
    t->unk42 = 11;
    switch (gCurTask->unk48)
    {
    case 68:
        gCurTask->unk4A = 120;
        gCurTask->unk43 = 1;
        gCurTask->unk38 = gUnk_08755AF0;
        gCurTask->unk42 = 8;
        break;
    case 164:
        gCurTask->unk4A = 56;
        gCurTask->unk43 = -1;
        gCurTask->unk38 = gUnk_08755B68;
        gCurTask->unk42 = 11;
        break;
    case 188:
        gCurTask->unk4A = 104;
        gCurTask->unk43 = -1;
        gCurTask->unk38 = gUnk_08755B40;
        gCurTask->unk42 = 9;
        break;
    case 60:
        gCurTask->unk4A = 72;
        gCurTask->unk43 = 1;
        gCurTask->unk38 = gUnk_08755B18;
        gCurTask->unk42 = 10;
        break;
    default:
        gCurTask->unk4A = 0;
        gCurTask->unk43 = 1;
        gCurTask->unk38 = gUnk_08755AF0;
        gCurTask->unk42 = 12;
        break;
    }
    u = gCurTask;
    u->unk4C = u->unk48 << 16;
    u->unk50 = u->unk4A << 16;
    u->unk3C = 0;
    u->unk30 = -1;
    u->unk34 = -1;
    u->unk2C = u->unk48;
}

void sub_080bc30c(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    s32 v = t->unk20;

    if (v == 4)
    {
        t->unk48 = 120;
        t->unk4A = 96;
    }
    else
    {
        s32 n = gPlayerCount - 1;
        s32 k = gLocalPlayer - v;

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
            gCurTask->unk4A = 0;
            break;
        }
    }
    u = gCurTask;
    u->unk4C = u->unk48 << 16;
    u->unk50 = u->unk4A << 16;
    u->unk3C = 1;
}

void sub_080bc3c8(void)
{
    struct Task *t = gCurTask;

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
        TaskStop();
        break;
    }
    gCurTask->unk4C = gCurTask->unk48 << 16;
}

void sub_080bc460(void)
{
    struct Task *t = gCurTask;

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
        gCurTask->unk20 = 0;
        break;
    }
    PlaySfx(256);
}

void sub_080bc4b0(void)
{
    s32 id = TaskCreateFrom(94, 32);

    if (id != -1)
    {
        struct Task *t = &gTasks[id];
        struct Task *p;

        t->unk73 = 1;
        p = gCurTask;
        t->unk18 = p->unk20;
        t->unk1C = 6;
        t->unk20 = -1;
        t->unk24 = -1;
        t->unk48 = p->unk48 + gUnk_08756448[p->unk20] * p->unk43;
        t->unk4A = p->unk4A - gUnk_08756450[p->unk20];
        t->unk34 = 0;
    }
    gCurTask->unk34 = id;
}

void sub_080bc54c(void)
{
    s32 id = TaskCreateFrom(94, 32);

    if (id != -1)
    {
        struct Task *t = &gTasks[id];
        struct Task *p;

        t->unk73 = 6;
        p = gCurTask;
        t->unk18 = p->unk20;
        t->unk48 = p->unk48 + gUnk_08756458[p->unk20];
        t->unk4A = p->unk4A - gUnk_08756460[p->unk20];
    }
    gCurTask->unk30 = id;
}

void sub_080bc5cc(void)
{
    s32 idx = TaskCreateFrom(94, 32);

    if (idx != -1)
    {
        struct Task *t = gCurTask;
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
        n = &gTasks[idx];
        n->unk73 = 1;
        u = gCurTask;
        n->unk18 = u->unk18;
        n->unk1C = 0;
        n->unk20 = 60;
        n->unk24 = -1;
        n->unk48 = x;
        n->unk4A = y;
        if (gLocalPlayer == u->unk18)
        {
            n->unk18 = 4;
            n->unk4A = u->unk4A - 32;
            n->unk48 += 12;
        }
        n->unk34 = 0;
        n->unk42 = gCurTask->unk42 - 8;
    }
    gCurTask->unk1C = 60;
}

void sub_080bc680(void)
{
    s32 idx = TaskCreateFrom(94, 32);

    if (idx != -1)
    {
        struct Task *t = gCurTask;
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
        n = &gTasks[idx];
        n->unk73 = 1;
        n->unk18 = gQuickDrawWins[gCurTask->unk18];
        n->unk1C = 2;
        n->unk20 = 60;
        n->unk24 = -1;
        n->unk48 = x + 4;
        n->unk4A = y;
    }
    gCurTask->unk1C = 60;
}

void sub_080bc70c(void)
{
    if (gCurTaskIdx == 0)
    {
        struct Task *p = &gTasks[gCurTask->unk44];
        p->unk24 = 1;
    }
    gCurTask->unk14 = 1;
}

void QuickDrawSetPlayerState(s32 a0, u16 a1)
{
    struct Task *t = &gTasks[a0];
    struct Task *p = &gTasks[t->unk44];

    if (p->unk18 != 2)
    {
        t->unk14 = a1;
        TaskSetEntry(QuickDrawPlayerEnterState, a0);
        if (t->unk34 != -1)
            TaskFree(t->unk34);
        if (t->unk30 != -1)
            TaskFree(t->unk30);
        t->unk30 = -1;
        t->unk34 = -1;
    }
}

void sub_080bc79c(u16 a0)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
        QuickDrawSetPlayerState(i, a0);
}

u8 sub_080bc7c8(void)
{
    struct Task *t = gCurTask;
    s16 x = t->unk48;
    s16 y = t->unk4A;

    if (x > -64 && x < 304 && y > -64 && y < 224)
        return 1;
    return 0;
}

void sub_080bc800(void)
{
    gCurTask->unk40 = gCurTask->unk18 << 12;
    gCurTask->unk38 = gUnk_08755B18;
    gCurTask->unk42 = 8;
    gCurTask->unk3C = 0;
    gCurTask->unk48 = 120;
    gCurTask->unk4A = 68;
    gCurTask->unk4C = gCurTask->unk48 << 16;
    gCurTask->unk50 = gCurTask->unk4A << 16;
}

void QuickDrawPlayer(void)
{
    struct Task *t = gCurTask;

    t->unk0C = (u32)TaskDrawScreen;
    t->unk04 = (u32)QuickDrawPlayerUpdate;
    t->unk42 = 7;
    t = gCurTask;
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
    CallTableEntry(gCurTask->unk14, 6, gUnk_08756468);
    TaskSleepForever();
}

void QuickDrawPlayerUpdate(void)
{
    CallTableEntry(gCurTask->unk15, 6, gUnk_08756480);
}

void QuickDrawPlayerEnterState(void)
{
    CallTableEntry(gCurTask->unk14, 6, gUnk_08756468);
}

void sub_080bc8e0(void)
{
    gCurTask->unk15 = 0;
    sub_080bc168();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(60);
    sub_080bc5cc();
    TaskYieldTrampoline(gCurTask->unk1C);
    if (gPlayerCount != 1)
    {
        sub_080bc680();
        TaskYieldTrampoline(gCurTask->unk1C);
    }
    sub_080bc70c();
    TaskSleepForever();
}

void sub_080bc948(void)
{
    if (gBrightness == 0 && gCurTask->unk24 == 0)
        sub_080bc0ec();
    if (gCurTask->unk14 != 0)
        TaskSetEntry(QuickDrawPlayerEnterState, gCurTaskIdx);
}

void sub_080bc988(void)
{
    gCurTask->unk15 = 1;
    sub_080bc1c4();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskSleepForever();
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

    gCurTask->unk15 = 2;
    sub_080bc30c();
    saved = gCurTask->unk48;
    x = gCurTask->unk48;
    for (i = 0; i < 2; i++)
    {
        t = gCurTask;
        t->unk48 = x + gUnk_08756498[0] * (u16)t->unk43;
        t->unk4C = t->unk48 << 16;
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->unk48 = x - gUnk_08756498[0] * (u16)t->unk43;
        t->unk4C = t->unk48 << 16;
        TaskYieldTrampoline(1);
    }
    gCurTask->unk48 = saved;
    gCurTask->unk4C = gCurTask->unk48 << 16;
    TaskSleepForever();
}

void sub_080bca68(void)
{
}

void sub_080bca6c(void)
{
    s32 i;

    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk15 = 3;
    sub_080bc3c8();
    while (1)
    {
        gCurTask->unk3C = 2;
        TaskYieldTrampoline(5);
        for (i = 0; i < 7; i++)
        {
            gCurTask->unk3C++;
            TaskYieldTrampoline(5);
        }
    }
}

void sub_080bcaac(void)
{
    if (gCurTask->unk54 != 0 && gCurTask->unk58 != 0)
    {
        u8 r = sub_080bc7c8();
        if (r == 0)
        {
            TaskStop();
            gCurTask->unk00 = r;
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

    gCurTask->unk15 = 4;
    sub_080bc460();
    saved = gCurTask->unk48;
    x = gCurTask->unk48;
    for (i = 0; i < 2; i++)
    {
        t = gCurTask;
        t->unk48 = x + gUnk_08756498[t->unk20] * (u16)t->unk43;
        t->unk4C = t->unk48 << 16;
        TaskYieldTrampoline(1);
        t = gCurTask;
        t->unk48 = x - gUnk_08756498[t->unk20] * (u16)t->unk43;
        t->unk4C = t->unk48 << 16;
        TaskYieldTrampoline(1);
    }
    gCurTask->unk48 = saved;
    gCurTask->unk4C = gCurTask->unk48 << 16;
    sub_080bc4b0();
    sub_080bc54c();
    TaskYieldTrampoline(30);
    p = &gTasks[gCurTask->unk44];
    p->unk75 = 1;
    TaskSleepForever();
}

void sub_080bcbbc(void)
{
}

void sub_080bcbc0(void)
{
    gCurTask->unk15 = 5;
    sub_080bc800();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskSleepForever();
}

void sub_080bcbf8(void)
{
}

void sub_080bcbfc(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    u32 *tbl = t->unk38;

    if (tbl != NULL && t->unk3C != -1 && (u16)(t->unk48 + 63) <= 366
        && t->unk4A > -64 && t->unk4A < 224)
    {
        QueueSprite(t->unk42, tbl[t->unk3C], t->unk3E, t->unk40, t->unk48, t->unk4A);
        u = gCurTask;
        if (u->unk24 != -1)
        {
            tbl = (u32 *)u->unk30;
            QueueSprite(u->unk42, tbl[u->unk24], u->unk3E, u->unk34,
                         u->unk48 + u->unk28, (s16)(u->unk4A + u->unk2C));
        }
    }
}

void sub_080bccbc(void)
{
    struct Task *t;
    u8 *p = &gCurTask->unk42;

    if (*p == 0)
        *p = 4;
    else
        *p += 4;
    switch (gCurTask->unk1C)
    {
    case 0:
        gCurTask->unk38 = gUnk_08755AA4;
        break;
    case 1:
        gCurTask->unk38 = gUnk_08755A78;
        break;
    case 2:
        gCurTask->unk38 = gUnk_08755A5C;
        break;
    case 3:
        gCurTask->unk38 = gUnk_08755A7C;
        break;
    case 4:
        gCurTask->unk38 = gUnk_08755A88;
        break;
    case 5:
        gCurTask->unk38 = gUnk_08755A34;
        break;
    case 6:
        gCurTask->unk38 = gUnk_08755AB8;
        break;
    case 7:
        gCurTask->unk38 = gUnk_08755AC8;
        break;
    }
    t = gCurTask;
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
    gCurTask->unk0C = (u32)sub_080bcbfc;
    sub_080bccbc();
    if (gCurTask->unk20 != -1)
        TaskExitTrampoline();
    else
        TaskSleepForever();
}

void sub_080bcde0(void)
{
    struct Task *t;

    gCurTask->unk12 = 4;
    t = gCurTask;
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

void QuickDrawTimerCount(void)
{
    struct Task *t = gCurTask;
    struct Task *p = &gTasks[t->unk44];

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
        gCurTask->unk24 = q;
        gCurTask->unk3C = r;
    }
}

void sub_080bce74(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    struct Task *v;
    u32 *tbl = t->unk38;

    if (tbl != NULL && t->unk3C != -1 && (u16)(t->unk48 + 63) <= 366
        && t->unk4A > -64 && t->unk4A < 224)
    {
        QueueSprite(t->unk42, tbl[t->unk3C], t->unk3E, t->unk40, t->unk48, t->unk4A);
        u = gCurTask;
        QueueSprite(u->unk42, tbl[u->unk24], u->unk3E, u->unk40,
                     u->unk48 + u->unk28, (s16)(u->unk4A + u->unk2C));
        v = gCurTask;
        QueueSprite(v->unk42 + 1, gUnk_08755AD8[0], v->unk3E, v->unk40,
                     v->unk48, (s16)(v->unk4A + 8));
    }
}

void QuickDrawTimer(void)
{
    struct Task *t = gCurTask;

    t->unk0C = (u32)sub_080bce74;
    t->unk04 = (u32)sub_080bcf8c;
    t->unk42 = 4;
    sub_080bcde0();
    TaskSleepForever();
}

void sub_080bcf8c(void)
{
    if (gCurTask->unk1C != 0)
        QuickDrawTimerCount();
}

void sub_080bcfa4(void)
{
    struct Task *t = gCurTask;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    t->unk0C = (u32)TaskDrawScreen;
    t->unk00 = (u32)TaskMove;
    t->unk42 = 4;
    u = gCurTask;
    u->unk38 = gUnk_08755A7C;
    u->unk40 |= 0x800;
    u->unk48 = 240;
    u->unk4A = 0;
    u->unk4C = u->unk48 << 16;
    u->unk50 = u->unk4A << 16;
    TaskYieldTrampoline(2);
    v = gCurTask;
    v->unk54 = 0xFFD00000;
    v->unk58 = 0x180000;
    v->unk3C = 1;
    TaskYieldTrampoline(5);
    w = gCurTask;
    w->unk48 = 64;
    w->unk4A = -16;
    w->unk4C = w->unk48 << 16;
    w->unk50 = w->unk4A << 16;
    w->unk54 = 0x200000;
    w->unk58 = 0x400000;
    w->unk3C++;
    TaskYieldTrampoline(4);
    TaskExitTrampoline();
}

void sub_080bd06c(void)
{
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk42 = 4;
    gCurTask->unk38 = gUnk_08755ADC;
    gCurTask->unk40 |= 0x800;
    gCurTask->unk48 = 120;
    gCurTask->unk4A = 96;
    gCurTask->unk4C = gCurTask->unk48 << 16;
    gCurTask->unk50 = gCurTask->unk4A << 16;
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = -1;
    TaskYieldTrampoline(60);
    TaskExitTrampoline();
}

void sub_080bd110(void)
{
    struct Task *t;

    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk42 = 4;
    t = gCurTask;
    t->unk38 = gUnk_08755AC8;
    t->unk40 |= 0x800;
    t->unk4C = t->unk48 << 16;
    t->unk50 = t->unk4A << 16;
    t->unk3C = t->unk18;
    t->unk58 = gUnk_087564A0[t->unk18];
    TaskYieldTrampoline(3);
    TaskStop();
    TaskSleepForever();
}

void QuickDrawLoadOpponentGraphics(s32 a0)
{
    struct GfxDesc *d = gUnk_087564D0[a0];

    LZ77UnCompWram(d->unk0C, gUnk_02020000);
    RequestCopy(3, (u32)gUnk_02020000, 0x06013000, d->unk02 << 5);
    RequestCopy(2, d->unk08, (u32)gUnk_03001570, d->unk00 << 5);
}

void QuickDrawSetOpponentState(s32 a0, u16 a1)
{
    struct Task *t = &gTasks[a0];
    struct Task *u = &gTasks[t->unk44];

    if (u->unk18 != 2)
    {
        t->unk14 = a1;
        TaskSetEntry(QuickDrawOpponentEnterState, a0);
    }
}

void sub_080bd210(void)
{
    struct Task *t = gCurTask;

    if (t->unk48 <= 164)
    {
        t->unk48 = 164;
        t->unk24 = 1;
    }
    else
    {
        t->unk48 -= 10;
    }
    gCurTask->unk4C = gCurTask->unk48 << 16;
    gCurTask->unk50 = gCurTask->unk4A << 16;
}

void sub_080bd25c(void)
{
    struct Task *t;

    sub_080bd290();
    t = gCurTask;
    t->unk48 = 276;
    t->unk24 = 0;
    t->unk4C = t->unk48 << 16;
    t->unk50 = t->unk4A << 16;
}

void sub_080bd290(void)
{
    s32 i;

    gCurTask->unk48 = 164;
    gCurTask->unk4A = 72;
    gCurTask->unk4C = gCurTask->unk48 << 16;
    gCurTask->unk50 = gCurTask->unk4A << 16;
    gCurTask->unk43 = -1;
    gCurTask->unk3E &= 0x7FFF;
    switch (gCurTask->unk18)
    {
    case 0:
        gCurTask->unk38 = gUnk_087559E4;
        break;
    case 1:
        gCurTask->unk38 = gUnk_087559F4;
        break;
    case 2:
        gCurTask->unk38 = gUnk_08755A04;
        break;
    case 3:
        gCurTask->unk38 = gUnk_08755A24;
        break;
    case 4:
        gCurTask->unk38 = gUnk_08755A14;
        break;
    }
    gCurTask->unk40 = 0x8180;
    gCurTask->unk3C = 0;
    i = gCurTask->unk74 * 5 + gCurTask->unk18;
    gCurTask->unk1C = gUnk_087564B0[i];
}

void sub_080bd370(void)
{
    s32 idx = TaskCreateFrom(94, 32);

    if (idx != -1)
    {
        struct Task *t = gCurTask;
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
        n = &gTasks[idx];
        n->unk73 = 5;
        n->unk76 = 1;
        n->unk48 = x;
        n->unk4A = y;
    }
    gCurTask->unk20 = 60;
}

void sub_080bd3dc(u8 a0)
{
    if (a0)
    {
        struct Task *t = gCurTask;

        t->unk48 = 120;
        t->unk4A = 96;
        switch (t->unk18)
        {
        case 0:
            PlaySfx(253);
            break;
        case 1:
            PlaySfx(258);
            break;
        case 2:
            PlaySfx(259);
            break;
        case 3:
            PlaySfx(261);
            break;
        case 4:
            PlaySfx(260);
            break;
        }
    }
    else
    {
        gCurTask->unk48 = 148;
        gCurTask->unk4A = 80;
    }
    gCurTask->unk4C = gCurTask->unk48 << 16;
    gCurTask->unk50 = gCurTask->unk4A << 16;
    gCurTask->unk3C = 1;
}

void sub_080bd494(void)
{
    struct Task *t = gCurTask;

    t->unk54 = 0x20000;
    t->unk58 = -0x50000;
    t->unk60 = 0x3000;
    t->unk68 = 0x80000;
    t->unk3C = 2;
}

void QuickDrawOpponent(void)
{
    struct Task *t;

    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk04 = (u32)QuickDrawOpponentUpdate;
    gCurTask->unk42 = 12;
    t = gCurTask;
    t->unk18 = 0;
    if (t->unk76 != 0)
        t->unk14 = 5;
    else
        t->unk14 = 0;
    CallTableEntry(gCurTask->unk14, 6, gUnk_087564E4);
    TaskSleepForever();
}

void QuickDrawOpponentUpdate(void)
{
    CallTableEntry(gCurTask->unk15, 6, gUnk_087564FC);
}

void QuickDrawOpponentEnterState(void)
{
    struct Task *t = gCurTask;

    t->unk00 = 0;
    CallTableEntry(t->unk14, 6, gUnk_087564E4);
}

void sub_080bd544(void)
{
    gCurTask->unk15 = 0;
    sub_080bd25c();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(60);
    sub_080bd370();
    TaskYieldTrampoline(gCurTask->unk20);
    gCurTask->unk14 = 1;
    TaskSleepForever();
}

void sub_080bd594(void)
{
    if (gBrightness == 0 && gCurTask->unk24 == 0)
        sub_080bd210();
    if (gCurTask->unk14 != 0)
        TaskSetEntry(QuickDrawOpponentEnterState, gCurTaskIdx);
}

void sub_080bd5d4(void)
{
    gCurTask->unk15 = 1;
    sub_080bd290();
    while (gBrightness != 0)
        TaskYieldTrampoline(1);
    TaskSleepForever();
}

void sub_080bd60c(void)
{
}

void sub_080bd610(void)
{
    gCurTask->unk15 = 2;
    sub_080bd3dc(1);
    TaskSleepForever();
}

void sub_080bd62c(void)
{
}

void sub_080bd630(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk15 = 3;
    sub_080bd494();
    gUnk_0200B048 = ++gCurTask->unk18;
    TaskSleepForever();
}

void sub_080bd664(void)
{
    if (gCurTask->unk54 != 0 && gCurTask->unk58 != 0)
    {
        u8 r = sub_080bc7c8();
        if (r == 0)
        {
            TaskStop();
            gCurTask->unk00 = r;
        }
    }
}

void sub_080bd694(void)
{
    gCurTask->unk15 = 4;
    sub_080bd3dc(0);
    TaskSleepForever();
}

void sub_080bd6b0(void)
{
}

void sub_080bd6b4(void)
{
    struct Task *t;

    gCurTask->unk04 = 0;
    gCurTask->unk42 = 4;
    switch (gUnk_0200B048)
    {
    case 0:
        gCurTask->unk38 = gUnk_087559E4;
        break;
    case 1:
        gCurTask->unk38 = gUnk_087559F4;
        break;
    case 2:
        gCurTask->unk38 = gUnk_08755A04;
        break;
    case 3:
        gCurTask->unk38 = gUnk_08755A24;
        break;
    case 4:
        gCurTask->unk38 = gUnk_08755A14;
        break;
    }
    t = gCurTask;
    t->unk40 = 0x9180;
    t->unk3C = 3;
    switch (gUnk_0200B048)
    {
    case 0:
        gCurTask->unk48 -= 16;
        gCurTask->unk4A -= 8;
        break;
    case 1:
        gCurTask->unk4A -= 12;
        break;
    case 2:
        gCurTask->unk48 -= 32;
        gCurTask->unk4A -= 28;
        break;
    case 3:
        gCurTask->unk48 -= 20;
        gCurTask->unk4A -= 32;
        break;
    case 4:
        gCurTask->unk48 -= 24;
        gCurTask->unk4A -= 8;
        break;
    }
    gCurTask->unk4C = gCurTask->unk48 << 16;
    gCurTask->unk50 = gCurTask->unk4A << 16;
    TaskYieldTrampoline(60);
    TaskExitTrampoline();
}

void sub_080bd7ec(void)
{
}

void sub_080bd7f0(void)
{
    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk42 = 4;
    gCurTask->unk38 = gUnk_08755B90;
    gCurTask->unk40 |= 0x800;
    TaskSleepForever();
}

void sub_080bd828(void)
{
    if (gPrevGameState == 5)
    {
        switch (gCurTask->unk3C)
        {
        case 5:
            AddPlayerScoreNoHud(10, gCurTask->unk1C);
            break;
        case 2:
            AddPlayerScoreNoHud(1000, gCurTask->unk1C);
            break;
        case 3:
            AddPlayerScoreNoHud(3000, gCurTask->unk1C);
            break;
        case 4:
            AddPlayerScoreNoHud(5000, gCurTask->unk1C);
            break;
        case 6:
            sub_08009eb8(1, gCurTask->unk1C);
            break;
        }
    }
}

void sub_080bd8ac(void)
{
    struct Task *t;

    gCurTask->unk0C = (u32)TaskDrawScreen;
    gCurTask->unk42 = 4;
    t = gCurTask;
    t->unk38 = gUnk_08755B90;
    if (gPlayerCount != 1)
        t->unk3C = gUnk_08756514[t->unk18];
    else
        t->unk3C = gUnk_0875651C[t->unk18];
    if (gCurTask->unk3C == 6 && gLocalPlayer == gCurTask->unk1C)
        PlaySfx(220);
    gCurTask->unk40 |= 0x800;
    sub_080bd828();
    TaskSleepForever();
}

void sub_080bd938(void)
{
    struct Task *t = gCurTask;
    u32 *p = t->unk38;

    if (p != NULL && t->unk3C != -1)
    {
        if (t->unk48 >= -63 && t->unk48 <= 303 && t->unk4A > -64 && t->unk4A < 224)
            QueueSprite(t->unk42, p[t->unk3C], t->unk3E, t->unk40, t->unk48, t->unk4A);
    }
}

void sub_080bd9b0(void)
{
    gCurTask->unk0C = (u32)sub_080bd938;
    gCurTask->unk42 = 4;
    gCurTask->unk38 = gUnk_08755A68;
    gCurTask->unk40 |= 0x800;
    TaskSleepForever();
}
