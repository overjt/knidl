#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_45d34.c (0x08045D34-0x0804632F, issue #87).
 *
 * Player action bodies, part 14: action 38 and per-frame handler 35.
 * sub_08045d34 (action 38, mode 13) spends one charge of the ability
 * counter PlayerState.unk0E (the HUD update sub_08009fcc) and plays one
 * of three sequences picked by the charges left (Task.unk73 = unk0E - 1),
 * each with its own song (StopSfx) and sound; meanwhile it freezes
 * the stage (gUnk_03001F34 = 1), switches the DISPCNT shadow
 * gDispCnt to BG0, BG2, BG3 and OBJ unless BG2 is already on, raises
 * PlayerState.unk42 bits 8-10 and repeats the last loop until the effect
 * counter PlayerState.unk16 runs out.  With the last charge spent it
 * drops the ability (sub_0800a130) unless the ability is 7.  Its handler
 * sub_080462f0 requests action 1 or 7 (on the ground or in the air) once
 * it has finished. */

extern vu16 gDispCnt;              /* DISPCNT shadow */
extern u8 gUnk_03001F34;
extern u32 gPlayerDefaultTerrainBox[];

void TaskYieldTrampoline(s32 frames);
s32 PlaySfx(s32 id);
s32 StopSfx(s32 songId);
void TaskSleepForever(void);
void TaskSetFrame(s32 a);
s32 sub_08009fcc(s32 a, s32 b, u32 c);
void sub_0800a130(s32 a, s32 id);
void sub_080261d4(u16 a);
void sub_08027204(u32 a);
void PlayerStopAxes(s32 a0);
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2);
void FreezeOtherTasks(s32 a0);
void PlayerStartOffsetScript(s32 a0);
void PlayerStopAtCeilingAndWall(void);
void PlayerSetMotionXPreset(s32 a0, s32 a1);           /* M11, still asm; M11's own spelling */
void PlayerSetMotionYPreset(s32 a0);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void sub_08045d34(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 13;
    gCurTask->unk15 = 35;
    gCurTask->unk73 = gCurTask->unk88->unk0E - 1;
    gCurTask->unk88->unk42 &= 0xFFEF;
    if (--gCurTask->unk88->unk0E == 0) {
        sub_08009fcc(0, -1, gCurTask->unk88->unk00);
    } else {
        struct PlayerState *p = gCurTask->unk88;
        sub_08009fcc(p->unk0D, p->unk0E, p->unk00);
    }
    gUnk_03001F34 = 1;
    PlayerStopAxes(3);
    FreezeOtherTasks(15);
    if (!(gDispCnt & 0x400)) {
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
    }
    gCurTask->unk88->unk16 = 0;
    CreatePlayerEffect(gCurTask->unk88->unk00, 37, 0);
    gCurTask->unk28 = 0;
    SetPlayerInvulnerability(3, 0, gCurTask->unk88->unk00);
    gCurTask->unk88->unk42 |= 0x700;
    switch (gCurTask->unk73) {
    case 2:
        PlayerSetMotionXPreset(11, 42);
        TaskSetFrame(0x66B);
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(6);
        PlayerSetMotionXPreset(11, 43);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        PlayerStopAxes(1);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C--;
        TaskYieldTrampoline(2);
        TaskSetFrame(0x670);
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        PlaySfx(160);
        gCurTask->unk88->unk16++;
        sub_080261d4(4);
        sub_08027204(2);
        CreatePlayerEffect(gCurTask->unk88->unk00, 37, 1);
        gCurTask->unk6C = 0;
        do {
            PlayerStartOffsetScript(4);
            TaskSetFrame(0x674);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 7);
        while ((s8)gCurTask->unk88->unk16 != 0) {
            PlayerStartOffsetScript(4);
            TaskSetFrame(0x674);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        }
        TaskSetFrame(0x670);
        TaskYieldTrampoline(4);
        break;
    case 1:
        TaskSetFrame(0x676);
        TaskYieldTrampoline(4);
        gCurTask->unk6C = 0;
        do {
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
        } while ((s16)++gCurTask->unk6C <= 5);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        StopSfx(160);
        TaskYieldTrampoline(1);
        PlaySfx(161);
        gCurTask->unk88->unk16++;
        sub_080261d4(4);
        sub_08027204(2);
        CreatePlayerEffect(gCurTask->unk88->unk00, 37, 1);
        gCurTask->unk6C = 0;
        do {
            PlayerStartOffsetScript(4);
            TaskSetFrame(0x67D);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 7);
        while ((s8)gCurTask->unk88->unk16 != 0) {
            PlayerStartOffsetScript(4);
            TaskSetFrame(0x67D);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        }
        TaskSetFrame(0x679);
        TaskYieldTrampoline(4);
        break;
    case 0:
        TaskSetFrame(0x67F);
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(2);
        gCurTask->unk3C++;
        TaskYieldTrampoline(5);
        gCurTask->unk88->unk68 = 0;
        PlayerSetMotionYPreset(35);
        gCurTask->unk3C--;
        TaskYieldTrampoline(4);
        gCurTask->unk3C--;
        TaskYieldTrampoline(4);
        TaskSetFrame(0x684);
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        gCurTask->unk3C++;
        TaskYieldTrampoline(4);
        StopSfx(161);
        TaskYieldTrampoline(1);
        gCurTask->unk88->unk68 = (u32)gPlayerDefaultTerrainBox;
        PlayerStopAxes(2);
        PlaySfx(162);
        gCurTask->unk88->unk16++;
        sub_080261d4(4);
        sub_08027204(2);
        CreatePlayerEffect(gCurTask->unk88->unk00, 37, 1);
        gCurTask->unk6C = 0;
        do {
            PlayerStartOffsetScript(4);
            TaskSetFrame(0x689);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        } while ((s16)++gCurTask->unk6C <= 7);
        while ((s8)gCurTask->unk88->unk16 != 0) {
            PlayerStartOffsetScript(4);
            TaskSetFrame(0x689);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        }
        TaskSetFrame(0x683);
        TaskYieldTrampoline(4);
        break;
    }
    SetPlayerInvulnerability(255, 0, gCurTask->unk88->unk00);
    FreezeOtherTasks(0);
    {
        struct PlayerState *p = gCurTask->unk88;
        if (p->unk0D != 7)
            sub_0800a130(p->unk0D, p->unk00);
        else
            p->unk22 = 2;
    }
    gCurTask->unk28++;
    gUnk_03001F34 = 0;
    gCurTask->unk88->unk42 &= 0xF8FF;
    TaskSleepForever();
}

void sub_080462f0(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 != 0) {
        if (t->unk7A & 1)
            t->unk88->unk01 = 1;
        else
            t->unk88->unk01 = 7;
    }
    PlayerStopAtCeilingAndWall();
}
