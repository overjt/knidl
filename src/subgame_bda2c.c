/* game_code_and_rodata_080653ec_0806ef5c 0x080BDA2C-0x080BF994
 * (issue #66, module M36 batch 1).
 *
 * RECIPE: agbcc -O2 -mthumb-interwork -fprologue-bugfix
 *   ./tools/fnmatch.sh 0x080BDA2C 0x080BF994 src/subgame_bda2c.c --newpb
 *
 * Head of M36, the sub-game task bank the game-mode flow module (M35)
 * starts as task type #95.  The 41-entry anchor table at 0x08756668 that
 * CallTableEntry dispatches on Task.unk14/unk15 lives here, and every state
 * below is one of its entries.
 *
 * The mode is a four-slot round-robin: Task.unk34 is the slot 0-3 that the
 * turn walks forwards or backwards (Task.unk28 <= 2 selects the direction),
 * Task.unk2C is the speed level 0-6 that indexes the per-level beat length
 * at gUnk_08756570, and Task.unk1C counts the beats until the level steps
 * up (the per-player-count limits are the three parallel byte tables at
 * 0x0875665C/0x0875665F/0x08756662, indexed by the player count in
 * gUnk_02006168).
 *
 *   sub_080bda2c   entry: install the dispatchers and kick BGM 0x82E
 *   sub_080bdab4   the beat loop (states 0..6 in Task.unk28)
 *   sub_080bdd28   "is this a record?" over gUnk_02006A10[]
 *   sub_080bdebc   / sub_080bdf3c / sub_080bdf9c   sprite placement helpers
 *   sub_080be04c   the per-round randomiser (slot + winner)
 *   sub_080be1ec   the vblank-flag wait (goto/do-while shape, lesson 6)
 *   sub_080bf0ac   the button-timing judgement against the 5-byte records
 *                  at 0x087565F4 (thresholds -> Task.unk20 = 2/1/0)
 *   sub_080bf394   the per-turn state advance: RNG over gUnk_087565E0,
 *                  the animation pick from gUnk_087565F4, the SE and the
 *                  eight TaskYield steps that fan the position out.
 */
#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern s8 gUnk_02006168;
extern u8 gUnk_020061DC;
extern s8 gUnk_02006A10[];
extern u8 gUnk_0200AF10;
extern u8 gUnk_0200AFF0;
extern u8 gUnk_0200B044[];
extern u16 gFadeSteps;
extern u8 gObjPalette[];
extern vs32 gBg3ScrollX;
extern vs32 gBg3ScrollY;
extern vu16 gPlayerPressedKeys[];
extern u16 gPrevGameState;
extern u16 gLocalPlayer;
extern u16 gPlayerCount;

extern s8 gUnk_08756560[];
extern s8 gUnk_08756564[];
extern u32 gUnk_08756528[];
extern u8 gUnk_08756570[];
extern u8 gUnk_0875665C[];
extern u8 gUnk_0875665F[];
extern u8 gUnk_08756662[];
extern u32 gUnk_08756668[];
extern u32 gUnk_08756670[];
extern u32 gUnk_08756678[];
extern u32 gUnk_08756680[];
extern u32 gUnk_08756688[];
extern u32 gUnk_08755DC0;
extern u16 gUnk_08756538[];
extern u8 gUnk_087565E0[];
extern u8 *gUnk_08756650[];
extern u8 *gUnk_087565F4[];
extern u32 *gUnk_0875670C[];
extern s16 gUnk_0875672C[];
extern s16 gUnk_08756734[];
extern s8 gUnk_08756740[];
extern s8 gUnk_08756744[];
extern s8 gUnk_08756748[];
extern u32 gUnk_087566A4[];
extern u32 gUnk_087566D8[];

extern void TaskYieldTrampoline(u32 frames);

extern void CallTableEntry(u32 a, u32 b, u32 *c);
extern void BeginFastFadeInFromWhite(void);
extern void BeginFastFadeOutToWhite(void);
extern s32 TaskCreateFrom(u32 type, s32 idx);
extern void TaskSetEntry(void *a, u32 i);
extern void TaskMoveRelativeToBg3(void);
extern void TaskSetFrame(s32 a);
extern void PlaySfx(u32 a);
extern void sub_080060c0(void);
extern void sub_08009eb8(u32 a, u32 b);
extern void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
extern s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, u32 f);
extern s32 DrawAffineSprite(s32 a, s16 b, s16 c, s32 d);
extern void TaskStop(void);
extern u32 RandomRange(u32 range);
extern void PlayBgm(u32 a);
extern void StopBgm(void);
extern void TaskSleepForever(void);
extern void SubGameReplay(s32 a);
extern void SubGameQuit(void);
extern void SubGameCheckEnd(void);

extern void sub_080bda78(void);
extern void sub_080bda98(void);
extern u32 sub_080bdd28(void);
extern u32 sub_080bdd70(void);
extern void sub_080bddb8(void);
extern void sub_080bde0c(void);
extern void sub_080bde78(u32 a);
extern void sub_080be04c(void);
extern void sub_080be1b0(void);
extern void sub_080be1d0(void);
extern void sub_080be4a4(void);
extern void sub_080be550(void);
extern void sub_080be5fc(void);
extern void sub_080be6b4(u32 a);
extern void sub_080be714(u32 a);
extern void sub_080be774(void);
extern void sub_080be850(void);
extern u32 sub_080bf0ac(void);
extern void sub_080bf154(void);
extern void sub_080be8a8(void);
extern void sub_080bdf3c(s32 a, s32 b, u32 c, u32 d);
extern void sub_080be7c0(u32 a);

void sub_080bda2c(void)
{
    gCurTask->unk04 = (u32)sub_080bda78;
    sub_080bddb8();
    sub_080be04c();
    gCurTask->unk28 = -3;
    PlayBgm(0x82E);
    gCurTask->unk14 = 0;
    CallTableEntry(gCurTask->unk14, 2, gUnk_08756668);
    TaskSleepForever();
}

void sub_080bda78(void)
{
    CallTableEntry(gCurTask->unk15, 2, gUnk_08756670);
    SubGameCheckEnd();
}

void sub_080bda98(void)
{
    CallTableEntry(gCurTask->unk14, 2, gUnk_08756668);
}

void sub_080bdab4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;

    gCurTask->unk15 = 0;
    TaskYieldTrampoline(16);
    t = gCurTask;
    if (t->unk28 != -3)
        t->unk28 = -2;
    while (gCurTask->unk28 < 0)
        TaskYieldTrampoline(1);
    while (1) {
        gCurTask->unk28 = gCurTask->unk20;
        gCurTask->unk20 = -1;
        if (gCurTask->unk28 == 6 || gCurTask->unk28 == -1)
            break;
        u = gCurTask;
        if (u->unk28 <= 2)
            u->unk24 = gUnk_08756570[u->unk2C] * (u->unk28 + 2);
        else
            u->unk24 = gUnk_08756570[u->unk2C] * (u->unk28 - 1);
        gCurTask->unk30 = 0;
        while (gCurTask->unk30 != gCurTask->unk24) {
            gCurTask->unk30++;
            TaskYieldTrampoline(1);
        }
        if (gCurTask->unk20 != 6 && gCurTask->unk20 != -1)
            TaskYieldTrampoline(3);
        if (gUnk_020061DC != 0)
            gUnk_020061DC--;
        v = gCurTask;
        v->unk1C++;
        if (v->unk1C == gUnk_0875665F[gUnk_02006168]) {
            v->unk1C = 0;
            if (v->unk2C + 1 != 7)
                v->unk2C++;
        }
        w = gCurTask;
        if (w->unk28 <= 2) {
            w->unk34++;
            if (w->unk34 == 4)
                w->unk34 = 0;
        } else {
            w->unk34--;
            if (w->unk34 < 0)
                w->unk34 = 3;
        }
    }
    gCurTask->unk28 = 6;
    if (sub_080bdd28())
        StopBgm();
    TaskYieldTrampoline(120);
    gCurTask->unk14 = 1;
    TaskSleepForever();
}

void sub_080bdc18(void)
{
    if (gCurTask->unk14 != 0)
        TaskSetEntry(sub_080bda98, gCurTaskIdx);
}

void sub_080bdc40(void)
{
    u8 i;

    gCurTask->unk15 = 1;
    if (sub_080bdd70()) {
        while (gUnk_0200AFF0 != 3) {
            for (i = 0; i < 4; i++) {
                if (gUnk_0200B044[i] == 3) {
                    gUnk_0200B044[i] = gUnk_0200AFF0;
                    break;
                }
            }
            gUnk_0200AFF0++;
        }
        gCurTask->unk18 = 2;
    } else {
        BeginFastFadeOutToWhite();
        while (gFadeSteps != 0)
            TaskYieldTrampoline(1);
        sub_080bde0c();
        BeginFastFadeInFromWhite();
        while (gFadeSteps != 0)
            TaskYieldTrampoline(1);
    }
    gCurTask->unk14 = 0;
    TaskSleepForever();
}

void sub_080bdd00(void)
{
    if (gCurTask->unk14 != 1)
        TaskSetEntry(sub_080bda98, gCurTaskIdx);
}

u32 sub_080bdd28(void)
{
    s32 i = gUnk_02006A10[gCurTask->unk34];

    gUnk_0200B044[i] = gUnk_0200AFF0;
    gUnk_0200AFF0++;
    gUnk_0200AF10 |= 1 << i;
    return sub_080bdd70();
}

u32 sub_080bdd70(void)
{
    s32 i;
    s32 mask = 0;

    for (i = 0; i < gPlayerCount; i++)
        mask += 1 << i;
    if (gUnk_0200AFF0 == 3 || (gUnk_0200AF10 & mask) == mask)
        return 1;
    return 0;
}

void sub_080bddb8(void)
{
    struct Task *t;

    gUnk_020061DC = gUnk_08756662[gUnk_02006168];
    t = gCurTask;
    t->unk28 = -4;
    t->unk20 = 1;
    t->unk30 = 0;
    t->unk2C = gUnk_0875665C[gUnk_02006168];
    t->unk24 = gUnk_08756570[t->unk2C] * (t->unk20 + 2);
}

void sub_080bde0c(void)
{
    struct Task *t;
    struct Task *u;

    gUnk_020061DC = gUnk_08756662[gUnk_02006168];
    t = gCurTask;
    t->unk28 = -4;
    t->unk20 = 1;
    t->unk30 = 0;
    t->unk2C -= 2;
    if (t->unk2C < gUnk_0875665C[gUnk_02006168])
        t->unk2C = gUnk_0875665C[gUnk_02006168];
    u = gCurTask;
    u->unk24 = gUnk_08756570[u->unk2C] * (u->unk20 + 2);
}

void sub_080bde78(u32 a)
{
    s32 i = TaskCreateFrom(0x5F, 32);

    if (i != -1) {
        struct Task *t = &gTasks[i];

        t->unk44 = gCurTaskIdx;
        t->unk73 = 1;
        t->unk18 = a;
    }
}

void sub_080bdebc(s32 a, s32 b, u16 c, s32 d)
{
    s32 i = TaskCreateFrom(0x5F, 32);

    if (i != -1) {
        struct Task *t = &gTasks[i];

        t->unk44 = gCurTaskIdx;
        t->unk73 = 2;
        t->unk18 = (s16)c;
        t->unk1C = d;
        t->unk4C = a + (gCurTask->unk43 << 19);
        t->unk50 = b - 0x40000;
        t->unk48 = a >> 16;
        t->unk4A = b >> 16;
    }
}

void sub_080bdf3c(s32 a, s32 b, u32 c, u32 d)
{
    s32 i = TaskCreateFrom(0x5F, 32);

    if (i != -1) {
        struct Task *t = &gTasks[i];

        t->unk44 = gCurTaskIdx;
        t->unk73 = 3;
        t->unk74 = c;
        t->unk43 = d;
        t->unk4C = a;
        t->unk50 = b;
        t->unk48 = a >> 16;
        t->unk4A = b >> 16;
    }
}

void sub_080bdf9c(u32 a)
{
    s32 i = TaskCreateFrom(0x5F, 32);

    if (i != -1) {
        struct Task *t = &gTasks[i];

        t->unk44 = gCurTaskIdx;
        t->unk73 = 3;
        t->unk74 = a;
        t->unk43 = gCurTask->unk43;
        t->unk4C = gCurTask->unk4C;
        t->unk50 = gCurTask->unk50;
        t->unk48 = gCurTask->unk48;
        /* The ROM reads unk48 twice here; unk4A is never sourced. */
        t->unk4A = gCurTask->unk48;
    }
}

void sub_080be010(void)
{
    s32 i = TaskCreateFrom(0x5F, 32);

    if (i != -1) {
        struct Task *t = &gTasks[i];

        t->unk44 = gCurTaskIdx;
        t->unk73 = 4;
    }
}

void sub_080be04c(void)
{
    u8 arr[4];
    s32 mask;
    s32 i;
    s32 j;
    s32 k;
    s32 r;
    s32 slot;

    mask = 0;
    for (i = 0; i <= 3; i++) {
        do {
            r = RandomRange(4);
        } while ((mask >> r) & 1);
        mask |= 1 << r;
        arr[i] = r;
        if (r == gLocalPlayer)
            slot = i;
    }
    r = RandomRange(gPlayerCount);
    for (i = 0; i <= 3; i++) {
        gUnk_02006A10[i] = arr[(slot + i) & 3];
        if (gUnk_02006A10[i] == r)
            gCurTask->unk34 = i;
        RequestCopy(2, gUnk_08756528[gUnk_02006A10[i]],
                     (u32)(gObjPalette + (i << 5)), 32);
    }
    sub_080bde78(0);
    for (i = 0; i <= 3; i++) {
        for (j = 0; j < 4; j++)
            if (i == gUnk_02006A10[j])
                break;
        k = TaskCreateFrom(0x5F, 32);
        if (k != -1) {
            struct Task *t = &gTasks[k];

            t->unk44 = gCurTaskIdx;
            t->unk73 = 0;
            t->unk18 = i;
            t->unk1C = j;
        }
    }
}

void sub_080be164(void)
{
    gCurTask->unk04 = (u32)sub_080be1b0;
    sub_080be4a4();
    gBg3ScrollX = 0x780000;
    gBg3ScrollY = 0;
    gCurTask->unk14 = 0;
    CallTableEntry(gCurTask->unk14, 2, gUnk_08756678);
    TaskSleepForever();
}

void sub_080be1b0(void)
{
    CallTableEntry(gCurTask->unk15, 2, gUnk_08756680);
    SubGameCheckEnd();
}

void sub_080be1d0(void)
{
    CallTableEntry(gCurTask->unk14, 2, gUnk_08756678);
}

void sub_080be1ec(void)
{
    s32 i;
    s32 x;
    vu16 *p;
    s32 m;

    gCurTask->unk15 = 0;
    TaskYieldTrampoline(60);
    sub_080be5fc();
    switch (gUnk_0200B044[gLocalPlayer]) {
    case 3:
        PlayBgm(29);
        break;
    case 1:
    case 2:
        PlayBgm(28);
        break;
    case 0:
        PlayBgm(23);
        break;
    }
    x = 0;
    for (i = 0; i < gPlayerCount; i++) {
        if (gUnk_0200B044[i] == 3) {
            x = 2;
            break;
        }
        if (gUnk_0200B044[i] == 0)
            x = 1;
    }
    if (x == 0)
        TaskYieldTrampoline(75);
    else if (x == 1)
        TaskYieldTrampoline(168);
    else
        TaskYieldTrampoline(174);
    if (gPrevGameState == 4) {
        gCurTask->unk14 = 1;
    } else {
        sub_080be550();
        TaskYieldTrampoline(60);
        sub_080be774();
        p = gPlayerPressedKeys;
        m = 9;
        goto wait;
        do {
            TaskYieldTrampoline(1);
        wait:
            ;
        } while ((*p & m) == 0);
        SubGameQuit();
    }
    TaskSleepForever();
}

void sub_080be2f0(void)
{
    if (gCurTask->unk14 != 0)
        TaskSetEntry(sub_080be1d0, gCurTaskIdx);
}

void sub_080be318(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 a;
    s32 c;
    u16 b;

    gCurTask->unk15 = 1;
    gCurTask->unk28 = 0;
    while (1) {
        gCurTask->unk2C = 0;
        sub_080be6b4(0);
        while (gCurTask->unk28 == 0) {
            TaskYieldTrampoline(1);
            if (gPlayerPressedKeys[0] & 9) {
                sub_080be7c0(102);
                t = gCurTask;
                if (t->unk2C == 0) {
                    t->unk28 = 1;
                    TaskYieldTrampoline(16);
                } else {
                    SubGameQuit();
                    TaskSleepForever();
                }
            } else if (gPlayerPressedKeys[0] & 0xF0) {
                sub_080be7c0(101);
                u = gCurTask;
                u->unk2C ^= 1;
                if (gPlayerPressedKeys[0] & 0x60)
                    u->unk30 = 1;
                else
                    u->unk30 = 0;
                TaskYieldTrampoline(10);
            }
        }
        gCurTask->unk2C = c = gUnk_02006168;
        sub_080be714(c);
        while (gCurTask->unk28 == 1) {
            TaskYieldTrampoline(1);
            a = gPlayerPressedKeys[0] & 9;
            if (a) {
                sub_080be7c0(102);
                SubGameReplay(gCurTask->unk2C);
                TaskSleepForever();
            } else {
                b = gPlayerPressedKeys[0] & 2;
                if (b) {
                    gCurTask->unk28 = a;
                    sub_080be7c0(215);
                    TaskYieldTrampoline(16);
                } else if (gPlayerPressedKeys[0] & 0x90) {
                    sub_080be7c0(101);
                    v = gCurTask;
                    v->unk2C++;
                    if (v->unk2C == 3)
                        v->unk2C = b;
                    gCurTask->unk30 = b;
                    TaskYieldTrampoline(10);
                } else if (gPlayerPressedKeys[0] & 0x60) {
                    sub_080be7c0(101);
                    w = gCurTask;
                    w->unk2C--;
                    if (w->unk2C < 0)
                        w->unk2C = 2;
                    gCurTask->unk30 = 1;
                    TaskYieldTrampoline(10);
                }
            }
        }
    }
}

void sub_080be4a0(void)
{
}

void sub_080be4a4(void)
{
    struct Task *t;
    s32 i;
    s32 k;

    if (gPlayerCount == 1) {
        k = TaskCreateFrom(0x5F, 32);
        if (k != -1) {
            t = &gTasks[k];
            t->unk44 = gCurTaskIdx;
            t->unk73 = 5;
            t->unk18 = 0;
            t->unk1C = 0;
            t->unk20 = 0;
        }
    } else {
        for (i = 0; i <= 3; i++) {
            k = TaskCreateFrom(0x5F, 32);
            if (k != -1) {
                t = &gTasks[k];
                t->unk44 = gCurTaskIdx;
                t->unk73 = 5;
                t->unk18 = i;
                t->unk1C = 0;
                t->unk20 = gUnk_02006A10[i];
            }
        }
    }
}

void sub_080be550(void)
{
    struct Task *t;
    s32 i;
    s32 k;

    if (gPlayerCount == 1) {
        k = TaskCreateFrom(0x5F, 32);
        if (k != -1) {
            t = &gTasks[k];
            t->unk44 = gCurTaskIdx;
            t->unk73 = 5;
            t->unk18 = 0;
            t->unk1C = 2;
            t->unk20 = 0;
        }
    } else {
        for (i = 0; i <= 3; i++) {
            k = TaskCreateFrom(0x5F, 32);
            if (k != -1) {
                t = &gTasks[k];
                t->unk44 = gCurTaskIdx;
                t->unk73 = 5;
                t->unk18 = i;
                t->unk1C = 2;
                t->unk20 = gUnk_02006A10[i];
            }
        }
    }
}

void sub_080be5fc(void)
{
    struct Task *t;
    s32 i;
    s32 k;

    if (gPrevGameState != 5) {
        if (gPlayerCount == 1) {
            k = TaskCreateFrom(0x5F, 32);
            if (k != -1) {
                t = &gTasks[k];
                t->unk44 = gCurTaskIdx;
                t->unk73 = 5;
                t->unk18 = 0;
                t->unk1C = 1;
                t->unk20 = 0;
            }
        } else {
            for (i = 0; i <= 3; i++) {
                k = TaskCreateFrom(0x5F, 32);
                if (k != -1) {
                    t = &gTasks[k];
                    t->unk44 = gCurTaskIdx;
                    t->unk73 = 5;
                    t->unk18 = i;
                    t->unk1C = 1;
                    t->unk20 = gUnk_02006A10[i];
                }
            }
        }
    }
}

void sub_080be6b4(u32 a)
{
    struct Task *t;
    s32 i;
    s32 k;

    if (gLocalPlayer == 0) {
        for (i = 0; i <= 1; i++) {
            k = TaskCreateFrom(0x5F, 32);
            if (k != -1) {
                t = &gTasks[k];
                t->unk44 = gCurTaskIdx;
                t->unk73 = 6;
                t->unk74 = 0;
                t->unk18 = i;
                t->unk1C = a;
            }
        }
    }
}

void sub_080be714(u32 a)
{
    struct Task *t;
    s32 i;
    s32 k;

    if (gLocalPlayer == 0) {
        for (i = 0; i <= 2; i++) {
            k = TaskCreateFrom(0x5F, 32);
            if (k != -1) {
                t = &gTasks[k];
                t->unk44 = gCurTaskIdx;
                t->unk73 = 6;
                t->unk74 = 1;
                t->unk18 = i;
                t->unk1C = a;
            }
        }
    }
}

void sub_080be774(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++) {
        switch (gUnk_0200B044[i]) {
        case 0:
            break;
        case 3:
            sub_08009eb8(3, i);
            break;
        case 2:
            sub_08009eb8(2, i);
            break;
        case 1:
            sub_08009eb8(1, i);
            break;
        }
    }
}

void sub_080be7c0(u32 a)
{
    if (gLocalPlayer == 0)
        PlaySfx(a);
}

void Task_BombRallyObject(void)
{
    CallTableEntry(gCurTask->unk73, 7, gUnk_08756688);
}

void sub_080be7fc(void)
{
    struct Task *t;

    t = gCurTask;
    t->unk0C = (u32)sub_080060c0;
    t->unk04 = (u32)sub_080be850;
    if (t->unk1C <= 1)
        t->unk42 = 7;
    else
        t->unk42 = 9;
    gCurTask->unk14 = 0;
    CallTableEntry(gCurTask->unk14, 13, gUnk_087566A4);
    TaskSleepForever();
}

void sub_080be850(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;

    t = gCurTask;
    u = &gTasks[t->unk44];
    CallTableEntry(t->unk15, 13, gUnk_087566D8);
    w = gCurTask;
    if (w->unk14 != 9 && u->unk28 == 6) {
        w->unk14 = 9;
        TaskSetEntry(sub_080be8a8, gCurTaskIdx);
    }
}

void sub_080be8a8(void)
{
    CallTableEntry(gCurTask->unk14, 13, gUnk_087566A4);
}

void sub_080be8c4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;

    v = gCurTask;
    v->unk00 = (u32)TaskMoveRelativeToBg3;
    v->unk15 = 0;
    w = gCurTask;
    w->unk4C = gUnk_0875672C[gCurTask->unk1C] << 16;
    w->unk50 = gUnk_08756734[gCurTask->unk1C] << 16;
    w->unk38 = gUnk_0875670C[gCurTask->unk1C];
    if (gCurTask->unk1C == 3) {
        w->unk43 = -1;
        gCurTask->unk40 = 0xA0 << 6;
    } else {
        w->unk43 = 1;
        gCurTask->unk40 = 0;
    }
    TaskSetFrame(0);
    x = gCurTask;
    x->unk20 = 0;
    x->unk24 = 0;
    while (1) {
        TaskYieldTrampoline(1);
        t = gCurTask;
        u = &gTasks[t->unk44];
        if (u->unk34 == t->unk1C) {
            if (u->unk28 == -2 && (gPlayerPressedKeys[t->unk18] & 1)) {
                u->unk28 = -1;
                TaskSetFrame(2);
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
                gCurTask->unk3C++;
                TaskYieldTrampoline(5);
                gCurTask->unk3C++;
                TaskYieldTrampoline(7);
                gCurTask->unk3C++;
                TaskYieldTrampoline(8);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                PlaySfx(254);
                z = gCurTask;
                sub_080bdf3c(z->unk4C + (gUnk_08756560[z->unk1C] << 16) * z->unk43,
                             z->unk50 + (gUnk_08756564[z->unk1C] << 16),
                             z->unk1C, z->unk43);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
                u->unk28 = u->unk20;
                TaskSetFrame(11);
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(15);
                gCurTask->unk3C++;
                TaskYieldTrampoline(4);
                TaskSetFrame(1);
                TaskYieldTrampoline(4);
                TaskSetFrame(15);
                TaskYieldTrampoline(6);
                break;
            }
        } else if (u->unk28 >= 0) {
            break;
        }
    }
    y = gCurTask;
    if (y->unk18 < gPlayerCount)
        y->unk14 = 1;
    else
        y->unk14 = 5;
    TaskSleepForever();
}

void sub_080beae0(void)
{
    if (gCurTask->unk14 != 0)
        TaskSetEntry(sub_080be8a8, gCurTaskIdx);
}

void sub_080beb08(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk15 = 1;
    u = gCurTask;
    if (u->unk1C == 0 || u->unk1C == 2) {
        if (u->unk20 == 0)
            u->unk43 = 1;
        else
            u->unk43 = -1;
        TaskSetFrame(0);
    } else if (u->unk20 == 0) {
        TaskSetFrame(0);
    } else {
        TaskSetFrame(16);
    }
    TaskSleepForever();
}

void sub_080beb54(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    sub_080bf154();
    t = gCurTask;
    if (t->unk24 != t->unk20) {
        t->unk14 = 2;
        TaskSetEntry(sub_080be8a8, gCurTaskIdx);
    } else if (gPlayerPressedKeys[t->unk18] & 1) {
        if (sub_080bf0ac()) {
            u = gCurTask;
            u->unk24 = 0;
            u->unk14 = 3;
            TaskSetEntry(sub_080be8a8, gCurTaskIdx);
        } else {
            v = gCurTask;
            v->unk24 = v->unk20 ^ 1;
            v->unk14 = 2;
            TaskSetEntry(sub_080be8a8, gCurTaskIdx);
        }
    }
}

void sub_080bebd4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;
    struct Task *y;
    struct Task *z;
    s32 n;

    t = gCurTask;
    t->unk15 = 2;
    u = gCurTask;
    if (u->unk20 == 0) {
        u->unk20 = 1;
        if (u->unk1C == 0 || u->unk1C == 2)
            u->unk43 = 1;
        TaskSetFrame(15);
        TaskYieldTrampoline(3);
        TaskSetFrame(1);
        TaskYieldTrampoline(4);
        v = gCurTask;
        v->unk3C--;
        TaskYieldTrampoline(2);
        w = gCurTask;
        if (gCurTask->unk1C == 0 || gCurTask->unk1C == 2) {
            n = w->unk43;
            w->unk43 = -n;
        }
        TaskSetFrame(17);
        TaskYieldTrampoline(2);
    } else {
        u->unk20 = 0;
        if (u->unk1C == 0 || u->unk1C == 2)
            u->unk43 = -1;
        TaskSetFrame(31);
        TaskYieldTrampoline(3);
        TaskSetFrame(17);
        TaskYieldTrampoline(4);
        x = gCurTask;
        x->unk3C--;
        TaskYieldTrampoline(2);
        y = gCurTask;
        if (y->unk1C == 0 || y->unk1C == 2) {
            n = y->unk43;
            y->unk43 = -n;
        }
        TaskSetFrame(1);
        TaskYieldTrampoline(2);
    }
    z = gCurTask;
    z->unk14 = 1;
    TaskSleepForever();
}

void sub_080becc0(void)
{
    if (gPlayerPressedKeys[gCurTask->unk18] & 1)
        gCurTask->unk24 ^= 1;
    if (gCurTask->unk14 != 2)
        TaskSetEntry(sub_080be8a8, gCurTaskIdx);
}

void sub_080bed08(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;
    struct Task *e;
    struct Task *f;
    struct Task *g;
    struct Task *h;
    struct Task *z;
    s32 n;

    t = gCurTask;
    u = &gTasks[t->unk44];
    t->unk15 = 3;
    if (u->unk20 != 6) {
        v = gCurTask;
        n = u->unk24 - u->unk30;
        v->unk34 = (n - (n >> 1)) - ((n - (n >> 1)) >> 1);
        v->unk30 = (n - (n >> 1)) >> 1;
        v->unk2C = (n >> 1) - (n >> 2);
        v->unk28 = n >> 2;
        if (v->unk1C == 0 || v->unk1C == 2) {
            if (v->unk20 == 0)
                v->unk43 = 1;
            else
                v->unk43 = -1;
        }
        if (gCurTask->unk20 == 0)
            TaskSetFrame(2);
        else
            TaskSetFrame(18);

        a = gCurTask;
        if ((a->unk28 >> 1) != 0) {
            a->unk3C++;
            TaskYieldTrampoline(a->unk28 >> 1);
        } else {
            a->unk3C++;
        }
        b = gCurTask;
        b->unk28 -= b->unk28 >> 1;
        if (b->unk28 != 0) {
            b->unk3C++;
            TaskYieldTrampoline(b->unk28);
        } else {
            b->unk3C++;
        }
        c = gCurTask;
        if ((c->unk2C >> 1) != 0) {
            c->unk3C++;
            TaskYieldTrampoline(c->unk2C >> 1);
        } else {
            c->unk3C++;
        }
        d = gCurTask;
        d->unk2C -= d->unk2C >> 1;
        if (d->unk2C != 0) {
            d->unk3C++;
            TaskYieldTrampoline(d->unk2C);
        } else {
            d->unk3C++;
        }
        e = gCurTask;
        if ((e->unk30 >> 1) != 0) {
            e->unk3C++;
            TaskYieldTrampoline(e->unk30 >> 1);
        } else {
            e->unk3C++;
        }
        f = gCurTask;
        f->unk30 -= f->unk30 >> 1;
        if (f->unk30 != 0) {
            f->unk3C++;
            TaskYieldTrampoline(f->unk30);
        } else {
            f->unk3C++;
        }
        g = gCurTask;
        if ((g->unk34 >> 1) != 0) {
            g->unk3C++;
            TaskYieldTrampoline(g->unk34 >> 1);
        } else {
            g->unk3C++;
        }
        h = gCurTask;
        h->unk34 -= h->unk34 >> 1;
        if (h->unk34 != 0) {
            h->unk3C++;
            TaskYieldTrampoline(h->unk34);
        } else {
            h->unk3C++;
        }
        PlaySfx(254);
        z = gCurTask;
        sub_080bdf3c(z->unk4C + (gUnk_08756560[z->unk1C] << 16) * z->unk43,
                     z->unk50 + (gUnk_08756564[z->unk1C] << 16),
                     z->unk1C, z->unk43);
        TaskYieldTrampoline(3);
        w = gCurTask;
        w->unk14 = 4;
    }
    TaskSleepForever();
}

void sub_080bef1c(void)
{
    if (gPlayerPressedKeys[gCurTask->unk18] & 1)
        gCurTask->unk24 ^= 1;
    if (gCurTask->unk14 != 3)
        TaskSetEntry(sub_080be8a8, gCurTaskIdx);
}

void sub_080bef64(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *z;

    t = gCurTask;
    t->unk15 = 4;
    u = gCurTask;
    if (u->unk20 == 0) {
        TaskSetFrame(11);
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(15);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        TaskSetFrame(1);
        TaskYieldTrampoline(4);
        TaskSetFrame(15);
        TaskYieldTrampoline(6);
    } else {
        u->unk20 = 0;
        TaskSetFrame(27);
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(15);
        v = gCurTask;
        if (v->unk1C == 0 || v->unk1C == 2)
            v->unk43 = 1;
        TaskSetFrame(14);
        TaskYieldTrampoline(4);
        TaskSetFrame(1);
        TaskYieldTrampoline(4);
        TaskSetFrame(15);
        TaskYieldTrampoline(6);
    }
    z = gCurTask;
    z->unk14 = 1;
    TaskSleepForever();
}

void sub_080bf048(void)
{
    struct Task *t;
    struct Task *u;

    if (gPlayerPressedKeys[gCurTask->unk18] & 1) {
        if (sub_080bf0ac()) {
            t = gCurTask;
            t->unk14 = 3;
            gCurTask->unk24 = 0;
        } else {
            u = gCurTask;
            u->unk24 ^= 1;
        }
    }
    if (gCurTask->unk14 != 4)
        TaskSetEntry(sub_080be8a8, gCurTaskIdx);
}

u32 sub_080bf0ac(void)
{
    struct Task *t;
    struct Task *u;
    s32 v;
    s32 q1;
    s32 q2;
    s32 q3;
    u8 *p;
    u8 *r;

    t = gCurTask;
    u = &gTasks[t->unk44];
    if ((u->unk34 == ((t->unk1C + 3) & 3) || u->unk28 > 2)
     && (u->unk34 == ((t->unk1C + 1) & 3) || u->unk28 <= 2)) {
        r = gUnk_087565F4[u->unk28];
        v = u->unk2C * 5;
        q1 = r[v + 1];
        q2 = r[v + 2];
        q3 = r[v + 3];
        if (u->unk30 >= r[v] && u->unk30 < q1)
            u->unk20 = 2;
        else if (u->unk30 >= q1 && u->unk30 < q2)
            u->unk20 = 1;
        else if (u->unk30 >= q2 && u->unk30 < q3)
            u->unk20 = 0;
        else
            return 0;
    } else {
        return 0;
    }
    if (gCurTask->unk20 == 1)
        u->unk20 += 3;
    return 1;
}

void sub_080bf154(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    u = &gTasks[t->unk44];
    if (!((u->unk34 == ((t->unk1C + 3) & 3) || u->unk28 > 2)
       && (u->unk34 == ((t->unk1C + 1) & 3) || u->unk28 <= 2))) {
        if (gCurTask->unk20 == 0)
            TaskSetFrame(0);
        else
            TaskSetFrame(16);
    } else {
        if (gCurTask->unk20 == 0)
            TaskSetFrame(2);
        else
            TaskSetFrame(18);
    }
}

void sub_080bf1cc(void)
{
    struct Task *t;
    struct Task *u;
    s32 n;

    t = gCurTask;
    t->unk15 = 5;
    gCurTask->unk20 = 0;
    n = RandomRange(4);
    u = gCurTask;
    u->unk28 = (n + 1) * 20;
    if (u->unk1C == 0 || u->unk1C == 2)
        u->unk43 = 1;
    TaskSetFrame(0);
    TaskSleepForever();
}

void sub_080bf214(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *w;
    s32 n;

    t = gCurTask;
    u = &gTasks[t->unk44];
    if ((u->unk34 == ((t->unk1C + 3) & 3) && u->unk28 <= 2)
     || (u->unk34 == ((t->unk1C + 1) & 3) && u->unk28 > 2)) {
        gCurTask->unk14 = 7;
        TaskSetEntry(sub_080be8a8, gCurTaskIdx);
    } else {
        w = gCurTask;
        w->unk28--;
        if (w->unk28 == 0) {
            if (RandomRange(8) == 0) {
                gCurTask->unk14 = 6;
                TaskSetEntry(sub_080be8a8, gCurTaskIdx);
            } else {
                n = RandomRange(4);
                gCurTask->unk28 = (n + 1) * 20;
            }
        }
    }
}

void sub_080bf2ac(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    s32 n;

    t = gCurTask;
    t->unk15 = 6;
    u = gCurTask;
    if (u->unk1C == 0 || u->unk1C == 2)
        u->unk43 = 1;
    TaskSetFrame(15);
    TaskYieldTrampoline(3);
    TaskSetFrame(1);
    TaskYieldTrampoline(4);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    v = gCurTask;
    if (v->unk1C == 0 || v->unk1C == 2)
        v->unk43 = -1;
    w = gCurTask;
    w->unk20 = 1;
    TaskSetFrame(17);
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskSleepForever();
}

void sub_080bf32c(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    u = &gTasks[t->unk44];
    if ((u->unk34 == ((t->unk1C + 3) & 3) && u->unk28 <= 2)
     || (u->unk34 == ((t->unk1C + 1) & 3) && u->unk28 > 2))
        gCurTask->unk14 = 7;
    if (gCurTask->unk14 != 6)
        TaskSetEntry(sub_080be8a8, gCurTaskIdx);
}

void sub_080bf394(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *a;
    struct Task *b;
    struct Task *c;
    struct Task *d;
    struct Task *e;
    struct Task *f;
    struct Task *g;
    struct Task *h;
    struct Task *z;
    u8 *tbl;
    s32 i;
    u8 q;
    s32 w;
    s32 n;
    u8 k;
    s32 m;
    s32 vv;

    u = &gTasks[gCurTask->unk44];
    tbl = gUnk_087565F4[u->unk28];
    gCurTask->unk15 = 7;
    k = gUnk_08756650[gUnk_02006168][u->unk2C * 3 + gUnk_0200AFF0];
    if (gUnk_020061DC == 0 && RandomRange(k) == 0) {
        u->unk20 = 6;
    } else {
        q = RandomRange(36);
        for (i = 0; i <= 1; i++) {
            if (q < gUnk_087565E0[i + u->unk28 * 3])
                break;
        }
        if (gCurTask->unk20 == 0)
            u->unk20 = i;
        else
            u->unk20 = i + 3;
    }
    w = u->unk20;
    if (w > 2)
        w -= 3;
    q = w;
    t = gCurTask;
    vv = u->unk2C * 5;
    m = q - 2;
    t->unk28 = tbl[vv - m];
    if (t->unk1C == 1 || t->unk1C == 3) {
        if (t->unk20 == 0)
            TaskSetFrame(2);
        else
            TaskSetFrame(18);
        TaskYieldTrampoline(gCurTask->unk28);
    } else {
        if (t->unk20 == 0)
            t->unk43 = 1;
        else
            t->unk43 = -1;
        TaskSetFrame(2);
        TaskYieldTrampoline(gCurTask->unk28);
    }
    if (u->unk20 != 6) {
        a = gCurTask;
        n = u->unk24 - a->unk28;
        a->unk34 = (n - (n >> 1)) - ((n - (n >> 1)) >> 1);
        a->unk30 = (n - (n >> 1)) >> 1;
        a->unk2C = (n >> 1) - (n >> 2);
        a->unk28 = n >> 2;
        if ((a->unk28 >> 1) != 0) {
            a->unk3C++;
            TaskYieldTrampoline(a->unk28 >> 1);
        } else {
            a->unk3C++;
        }
        b = gCurTask;
        b->unk28 -= b->unk28 >> 1;
        if (b->unk28 != 0) {
            b->unk3C++;
            TaskYieldTrampoline(b->unk28);
        } else {
            b->unk3C++;
        }
        c = gCurTask;
        if ((c->unk2C >> 1) != 0) {
            c->unk3C++;
            TaskYieldTrampoline(c->unk2C >> 1);
        } else {
            c->unk3C++;
        }
        d = gCurTask;
        d->unk2C -= d->unk2C >> 1;
        if (d->unk2C != 0) {
            d->unk3C++;
            TaskYieldTrampoline(d->unk2C);
        } else {
            d->unk3C++;
        }
        e = gCurTask;
        if ((e->unk30 >> 1) != 0) {
            e->unk3C++;
            TaskYieldTrampoline(e->unk30 >> 1);
        } else {
            e->unk3C++;
        }
        f = gCurTask;
        f->unk30 -= f->unk30 >> 1;
        if (f->unk30 != 0) {
            f->unk3C++;
            TaskYieldTrampoline(f->unk30);
        } else {
            f->unk3C++;
        }
        g = gCurTask;
        if ((g->unk34 >> 1) != 0) {
            g->unk3C++;
            TaskYieldTrampoline(g->unk34 >> 1);
        } else {
            g->unk3C++;
        }
        h = gCurTask;
        h->unk34 -= h->unk34 >> 1;
        if (h->unk34 != 0) {
            h->unk3C++;
            TaskYieldTrampoline(h->unk34);
        } else {
            h->unk3C++;
        }
        PlaySfx(254);
        z = gCurTask;
        sub_080bdf3c(z->unk4C + (gUnk_08756560[z->unk1C] << 16) * z->unk43,
                     z->unk50 + (gUnk_08756564[z->unk1C] << 16),
                     z->unk1C, z->unk43);
        TaskYieldTrampoline(3);
        gCurTask->unk14 = 8;
    }
    TaskSleepForever();
}

void sub_080bf67c(void)
{
    if (gCurTask->unk14 != 7)
        TaskSetEntry(sub_080be8a8, gCurTaskIdx);
}

void sub_080bf6a4(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;

    t = gCurTask;
    t->unk15 = 8;
    u = gCurTask;
    if (u->unk20 == 0) {
        TaskSetFrame(11);
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(15);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        TaskSetFrame(1);
        TaskYieldTrampoline(4);
        TaskSetFrame(15);
        TaskYieldTrampoline(6);
    } else {
        u->unk20 = 0;
        TaskSetFrame(27);
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(15);
        v = gCurTask;
        if (v->unk1C == 0 || v->unk1C == 2)
            v->unk43 = 1;
        TaskSetFrame(14);
        TaskYieldTrampoline(4);
        TaskSetFrame(1);
        TaskYieldTrampoline(4);
        TaskSetFrame(15);
        TaskYieldTrampoline(6);
    }
    gCurTask->unk14 = 5;
    TaskSleepForever();
}

void sub_080bf788(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    u = &gTasks[t->unk44];
    if ((u->unk34 == ((t->unk1C + 3) & 3) && u->unk28 <= 2)
     || (u->unk34 == ((t->unk1C + 1) & 3) && u->unk28 > 2))
        gCurTask->unk14 = 7;
    if (gCurTask->unk14 != 8)
        TaskSetEntry(sub_080be8a8, gCurTaskIdx);
}

void sub_080bf7f0(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *z;
    s32 x;
    s32 y;
    s32 c;

    t = gCurTask;
    u = &gTasks[t->unk44];
    t->unk15 = 9;
    v = gCurTask;
    if (v->unk1C == u->unk34) {
        v->unk3C = 0xFFFF;
        TaskStop();
        gCurTask->unk6C = 0;
        do {
            x = gUnk_0875672C[gCurTask->unk1C] - (gBg3ScrollX >> 16)
              + gUnk_08756740[gCurTask->unk1C] * (s16)gCurTask->unk6C;
            y = gUnk_08756734[gCurTask->unk1C] - (gBg3ScrollY >> 16)
              + gUnk_08756744[gCurTask->unk1C] * (s16)gCurTask->unk6C;
            c = gUnk_08756538[gCurTask->unk1C] + gUnk_08756748[gCurTask->unk1C] * (s16)gCurTask->unk6C;
            if (c <= 127)
                c = 128;
            if (x >= -63 && x <= 303 && y > -64 && y <= 223)
                QueueSprite(8, DrawAffineSprite(gUnk_08755DC0, c, c,
                                            (((s16)gCurTask->unk6C * 3) << 3) & 0xFF),
                             0, gCurTask->unk1C << 12, x, (s16)y);
            TaskYieldTrampoline(1);
            z = gCurTask;
            z->unk6C++;
        } while ((s16)z->unk6C <= 63);
    }
    TaskSleepForever();
}

void sub_080bf934(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    u = &gTasks[t->unk44];
    if (u->unk28 == -4) {
        if (((gUnk_0200AF10 >> t->unk18) & 1) == 0)
            t->unk14 = 0;
        else
            t->unk14 = 10;
        TaskSetEntry(sub_080be8a8, gCurTaskIdx);
    }
}
