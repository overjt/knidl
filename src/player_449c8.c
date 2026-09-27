#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_449c8.c (0x080449C8-0x08044D03, issue #87).
 *
 * Player action bodies, part 11: per-frame handler 30 and action 34
 * with its handler 31.  sub_080449c8 (handler 30) is the
 * per-frame half of M11's action 33 (sub_08044878): in state 1 it counts
 * Task.unk28 down and re-binds the coroutine to state 2 once B is no
 * longer held, and over the animation frames 0x36B-0x372 it blends the
 * player's palettes gUnk_081BE6BC[player] into the OBJ palette buffer
 * (BlendColors, raising PlayerState.unk42 bit 4 while it does).
 * sub_08044b94 (action 34, mode 13) is a linear yield script (animation
 * 0x3E9, effect 28 on the ground, M14's sub_08053a44, sound 144); its
 * handler sub_08044c7c re-enters it on a newly-pressed B and requests
 * action 2 when left or right is held on the ground. */

extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern u8 gUnk_081BE6BC[];              /* per-player palettes, 128 bytes each */
extern u8 gObjPalette[];              /* OBJ palette buffer (M11 spelling) */
extern u16 gLatchedPressedKeys[];             /* newly-pressed keys, latched per player */

void TaskYieldTrampoline(s32 frames);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
s32 sub_0803e55c(void);
void PlayerTurnToHeldDirection(void);
s32 PlayerRequestLocomotion(void);
void sub_08044878(void);
void sub_08053a44(s32 a0, s32 a1, s32 a2);   /* M14, src/plobj_52f6c.c (defined s32 (s8, u8, s32); the result is unused here) */
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void sub_080449c8(void)
{
    struct Task *t = gCurTask;
    u8 *st = &t->unk73;

    switch (*st) {
    case 0:
        break;
    case 1:
        if (t->unk28 == 0) {
            if ((gLatchedHeldKeys[t->unk88->unk00] & 2) == 0) {
                *st = 2;
                TaskSetEntry(sub_08044878, gCurTaskIdx);
            }
        } else {
            t->unk28--;
        }
        switch (gCurTask->unk3C) {
        case 0x36B:
        case 0x36C:
        case 0x36F:
        case 0x370:
            {
                struct Task *u = gCurTask;
                u->unk88->unk42 &= 0xFFEF;
                u->unk70 = 0;
                u->unk6E = 0;
            }
            break;
        case 0x36E:
        case 0x372:
            {
                struct Task *u = gCurTask;
                u->unk6E += 128;
                if (u->unk6E > 256)
                    u->unk6E = 256;
            }
            {
                struct Task *u = gCurTask;
                BlendColors((u16 *)&gUnk_081BE6BC[u->unk88->unk00 * 128],
                             (u16 *)&gUnk_081BE6BC[u->unk88->unk00 * 128 + 32],
                             (u16)u->unk6E, 16,
                             (u16 *)(gObjPalette + ((u->unk40 >> 12) << 5)));
            }
            /* fallthrough */
        case 0x36D:
        case 0x371:
            {
                struct Task *u = gCurTask;
                u->unk70 += 64;
                if ((s16)u->unk70 > 256)
                    u->unk70 = 256;
            }
            {
                struct Task *u = gCurTask;
                BlendColors((u16 *)&gUnk_081BE6BC[u->unk88->unk00 * 128 + 64],
                             (u16 *)&gUnk_081BE6BC[u->unk88->unk00 * 128 + 96],
                             u->unk70, 16,
                             (u16 *)(gObjPalette + (((u->unk40 >> 12) + 1) << 5)));
            }
            gCurTask->unk88->unk42 |= 16;
            break;
        }
        break;
    case 2:
        break;
    case 3:
        PlayerRequestLocomotion();
        break;
    }
    sub_0803e55c();
    {
        struct PlayerState *p = gCurTask->unk88;
        if (p->unk01 != 0)
            p->unk42 &= 0xFFEF;
    }
}

void sub_08044b94(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 31;
    gCurTask->unk28 = 0;
    gCurTask->unk80 = 0;
    TaskSetFrame(0x3E9);
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(10);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    if (gCurTask->unk7A & 1)
        CreatePlayerEffect(gCurTask->unk88->unk00, 28, 3);
    sub_08053a44(gCurTask->unk88->unk00, 5, 0);
    gCurTask->unk28++;
    PlaySfxIfLocalPlayer(144, gCurTask->unk88->unk00);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk28++;
    TaskSleepForever();
}

void sub_08044c7c(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 != 0) {
        if (t->unk28 == 1) {
            if (gLatchedPressedKeys[t->unk88->unk00] & 2) {
                TaskSetEntry(sub_08044b94, gCurTaskIdx);
            } else if (t->unk7A & 1) {
                if (gLatchedHeldKeys[t->unk88->unk00] & 48) {
                    PlayerTurnToHeldDirection();
                    gCurTask->unk88->unk01 = 2;
                }
            }
        } else {
            PlayerRequestLocomotion();
        }
    }
    sub_0803e55c();
}
