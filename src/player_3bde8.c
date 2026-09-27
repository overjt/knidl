#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_3bde8.c (0x0803BDE8-0x0803CD5F, issue #91).
 *
 * Player action body, part 10: action 19 and three helpers M09 calls.
 * sub_0803bde8 (action 19, mode 23; its per-frame handler 18 is
 * sub_0803c990) hands something over to the partner player in
 * Task.unk18: the two face each other, play the rows of gUnk_0873DA62 by
 * ability, and PlayerState.unk3A says what passes - 1 refills the
 * partner's health gPlayerHealth[] up to gMaxHealth step by step
 * through sub_080b4204 (src/hud_b2fe8.c), 2 gives one or two steps, 3
 * copies PlayerState.unk17/unk18.  sub_0803c9b4 (from M09's
 * sub_08033414, the twin of M04's sub_080109c8) steps and draws the
 * three spark records gUnk_02007E90[player][]; sub_0803cbd8 (M09's
 * sub_0803332c) steps the knock-back script
 * gUnk_0873A994[PlayerState.filler2A][PlayerState.unk28] into the 8.8
 * offsets PlayerState.unk24/unk26; sub_0803ccd8 (M09's PlayerActionFall)
 * applies step n of the 8.8 motion table gUnk_0873AEBC. */

/* gUnk_02007E90[4][3]: M04's per-player spark records (src/player_10358.c) */
struct M04Spark
{
    /*0x00*/ s32 unk00;
    /*0x04*/ s32 unk04;
    /*0x08*/ s32 unk08;
    /*0x0C*/ u8 unk0C;
    /*0x0D*/ u8 unk0D;
    /*0x0E*/ u16 unk0E;
};

/* one step of a player's knock-back script: {dx, dy, flags} with
   flags & 15 = frames to hold, & 64 = mirror dx with the facing,
   & 128 = sound; a zero flags byte ends the script */
struct Unk0873A994
{
    /*0x00*/ u16 unk0;
    /*0x02*/ u16 unk2;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 filler5[3];
};

extern u16 gUnk_0873DA62[][2];
extern s16 gUnk_0873DACA[][2];
extern struct PlayerState gPlayerStates[];
extern s16 gPlayerHealth[];             /* health per player (M02's HUD) */
extern s16 gMaxHealth;
extern u16 gLocalPlayer;
extern u8 gExtraMode;
extern struct M04Spark gUnk_02007E90[][3];
extern s16 gSpriteCameraY;
extern s16 gSpriteCameraX;
extern s16 gUnk_0873A924[][16];
extern u32 gUnk_0873A964[];
extern struct Unk0873A994 *gUnk_0873A994[];
extern u16 gUnk_0873AEBC[][2];

void TaskYieldTrampoline(s32 frames);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
u32 RandomRange(u32 range);
s32 PlaySfx(s32 id);
void TaskSetSkipMask(u8 val, s32 idx);
void TaskSleepForever(void);
void TaskSetMotionY(s32 a, s32 b, s32 c);
void TaskSetFrame(s32 a);
u32 IsWorldPosOnScreen(s16 a, s16 b);        /* u8 in early_5d9c.c; u32 as in player_109c8.c (u8 costs 78 bytes) */
void PlayerStopAxes(s32 a0);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void FreezeOtherTasks(s32 a0);
void sub_08040710(void);
s32 sub_080b4204(u32 a);

/* Action 19 enter (mode 23, per-frame handler 18), aimed at the player
   Task.unk18 names.  With itself as the target it only plays the ability's
   animation (gUnk_0873DA62[ability][1], or gUnk_0873DACA[k][1] when
   Task.unk7B bit 0 is set) and ends; otherwise it turns to face the target
   task gTasks[unk18], plays gUnk_0873DA62[ability][0] (or
   gUnk_0873DACA[k][0]) and, by PlayerState.unk3A, raises the target's
   health gPlayerHealth[] through sub_080b4204 while it is below the maximum
   gMaxHealth (1: until full, 2: at most 1 or 2 steps by gExtraMode)
   or copies its own unk17/unk18 to the target (3); then it restores both
   tasks' Task.layer/unk43 (saved on the stack), clears PlayerState.unk42
   bit 8 on both players and sets the target's bit in PlayerState.unk3B. */
void sub_0803bde8(void)
{
    struct Task *t;
    struct Task *u;
    struct PlayerState *q;
    s16 n;
    u16 a43;
    u16 b43;
    u8 a42;
    u8 b42;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 23;
    gCurTask->unk15 = 18;
    if (gCurTask->unk88->unk06 == 2)
        gCurTask->unk88->unk06 = 0;
    PlaySfxIfLocalPlayer(292, gCurTask->unk88->unk00);
    if (gCurTask->unk18 == gCurTask->unk88->unk00)
    {
        gCurTask->layer = 5;
        TaskSetSkipMask(14, gCurTaskIdx);
        t = gCurTask;
        t->unk88->unk42 |= 0x100;
        t->unk88->unk42 &= 0xFFEF;
        if (!(t->unk7B & 1))
        {
            t->unk46 = gUnk_0873DA62[t->unk88->unk0D][1];
            switch (t->unk88->unk0D)
            {
            case 0:
                if (gCurTask->unk88->unk06 == 1)
                    gCurTask->unk46 = 324;
            default:
                TaskSetFrame(gCurTask->unk46);
            case 24:
                TaskSleepForever();
            case 1:
            case 2:
            case 5:
            case 15:
            case 19:
                while (1)
                {
                    TaskSetFrame(gCurTask->unk46);
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                }
            }
        }
        else
        {
            switch (gCurTask->unk88->unk0D)
            {
            default:
                if (gCurTask->unk88->unk06 == 1)
                    TaskSetFrame(326);
                else
                    TaskSetFrame(gUnk_0873DACA[0][1]);
                break;
            case 4:
                TaskSetFrame(gUnk_0873DACA[1][1]);
                break;
            case 9:
                TaskSetFrame(gUnk_0873DACA[2][1]);
                break;
            case 10:
                TaskSetFrame(gUnk_0873DACA[3][1]);
                break;
            case 24:
                break;
            }
            TaskSleepForever();
        }
    }
    q = &gPlayerStates[gCurTask->unk18];
    u = &gTasks[gCurTask->unk18];
    FreezeOtherTasks(15);
    TaskSetSkipMask(12, gCurTaskIdx);
    TaskSetSkipMask(0, gCurTask->unk18);
    a43 = gCurTask->facing;
    a42 = gCurTask->layer;
    PlayerStopAxes(3);
    t = gCurTask;
    t->unk88->unk42 |= 0x100;
    t->unk88->unk42 &= 0xFFEF;
    if (u->unk48 - t->unk48 > 0)
        t->facing = 1;
    else
        t->facing = -1;
    gCurTask->layer = 4;
    b43 = u->facing;
    b42 = u->layer;
    u->unk54 = u->unk5C = u->unk64 = 0;
    u->unk58 = u->unk60 = u->unk68 = 0;
    if ((u->facing = -gCurTask->facing) == 1)
        u->unk3E &= 0x7FFF;
    else
        u->unk3E |= 0x8000;
    t = gCurTask;
    if (!(t->unk7B & 1))
    {
        t->unk46 = gUnk_0873DA62[t->unk88->unk0D][0];
        switch (t->unk88->unk0D)
        {
        default:
        case 0:
            if (gCurTask->unk88->unk06 == 1)
            {
                TaskSetFrame(325);
                TaskYieldTrampoline(2);
            }
            else
            {
                TaskSetFrame(gCurTask->unk46);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
            }
            switch (gCurTask->unk88->unk3A)
            {
            case 1:
                if (gPlayerHealth[gCurTask->unk18] < gMaxHealth)
                {
                    do
                    {
                        if (gLocalPlayer == gCurTask->unk88->unk00 || gLocalPlayer == gCurTask->unk18)
                            PlaySfx(221);
                        gCurTask->unk28 = sub_080b4204(gCurTask->unk18);
                        TaskYieldTrampoline(8);
                    } while (gCurTask->unk28 == 0);
                }
                TaskYieldTrampoline(16);
                break;
            case 2:
                if (gPlayerHealth[gCurTask->unk18] < gMaxHealth)
                {
                    if (gExtraMode == 0)
                        n = 2;
                    else
                        n = 1;
                    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < n; gCurTask->unk6C++)
                    {
                        if (gLocalPlayer == gCurTask->unk88->unk00 || gLocalPlayer == gCurTask->unk18)
                            PlaySfx(221);
                        gCurTask->unk28 = sub_080b4204(gCurTask->unk18);
                        TaskYieldTrampoline(8);
                        if (gCurTask->unk28 != 0)
                            break;
                    }
                }
                TaskYieldTrampoline(16);
                break;
            case 3:
                if ((s8)q->unk22 != 0)
                    q->unk1E = q->unk20 = 0;
                q->unk17 = gCurTask->unk88->unk17;
                q->unk18 = gCurTask->unk88->unk18;
                TaskYieldTrampoline(32);
                break;
            }
            if (gCurTask->unk88->unk06 == 1)
            {
                TaskSetFrame(319);
                TaskYieldTrampoline(2);
            }
            else
            {
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
                gCurTask->frame--;
                TaskYieldTrampoline(2);
            }
            break;
        case 1:
        case 2:
        case 5:
        case 19:
        case 22:
        case 23:
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            switch (gCurTask->unk88->unk3A)
            {
            case 1:
                if (gPlayerHealth[gCurTask->unk18] < gMaxHealth)
                {
                    do
                    {
                        if (gLocalPlayer == gCurTask->unk88->unk00 || gLocalPlayer == gCurTask->unk18)
                            PlaySfx(221);
                        gCurTask->unk28 = sub_080b4204(gCurTask->unk18);
                        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 1; gCurTask->unk6C++)
                        {
                            TaskSetFrame((s16)(gCurTask->unk46 + 3));
                            TaskYieldTrampoline(2);
                            TaskSetFrame((s16)(gCurTask->unk46 + 15));
                            TaskYieldTrampoline(2);
                        }
                    } while (gCurTask->unk28 == 0);
                }
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 3; gCurTask->unk6C++)
                {
                    TaskSetFrame((s16)(gCurTask->unk46 + 3));
                    TaskYieldTrampoline(2);
                    TaskSetFrame((s16)(gCurTask->unk46 + 15));
                    TaskYieldTrampoline(2);
                }
                break;
            case 2:
                if (gPlayerHealth[gCurTask->unk18] < gMaxHealth)
                {
                    if (gExtraMode == 0)
                        n = 2;
                    else
                        n = 1;
                    for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < n; gCurTask->unk6C++)
                    {
                        if (gLocalPlayer == gCurTask->unk88->unk00 || gLocalPlayer == gCurTask->unk18)
                            PlaySfx(221);
                        gCurTask->unk28 = sub_080b4204(gCurTask->unk18);
                        for (gCurTask->unk6E = 0; gCurTask->unk6E <= 1; gCurTask->unk6E++)
                        {
                            TaskSetFrame((s16)(gCurTask->unk46 + 3));
                            TaskYieldTrampoline(2);
                            TaskSetFrame((s16)(gCurTask->unk46 + 15));
                            TaskYieldTrampoline(2);
                        }
                        if (gCurTask->unk28 != 0)
                            break;
                    }
                }
                for (gCurTask->unk6E = 0; gCurTask->unk6E <= 3; gCurTask->unk6E++)
                {
                    TaskSetFrame((s16)(gCurTask->unk46 + 3));
                    TaskYieldTrampoline(2);
                    TaskSetFrame((s16)(gCurTask->unk46 + 15));
                    TaskYieldTrampoline(2);
                }
                break;
            case 3:
                if ((s8)q->unk22 != 0)
                    q->unk1E = q->unk20 = 0;
                q->unk17 = gCurTask->unk88->unk17;
                q->unk18 = gCurTask->unk88->unk18;
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 7; gCurTask->unk6C++)
                {
                    TaskSetFrame((s16)(gCurTask->unk46 + 3));
                    TaskYieldTrampoline(2);
                    TaskSetFrame((s16)(gCurTask->unk46 + 15));
                    TaskYieldTrampoline(2);
                }
                break;
            }
            TaskSetFrame((s16)(gCurTask->unk46 + 3));
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            break;
        }
    }
    else
    {
        switch (t->unk88->unk0D)
        {
        default:
            gCurTask->unk46 = gUnk_0873DACA[0][0];
            break;
        case 4:
            t->unk46 = gUnk_0873DACA[1][0];
            break;
        case 9:
            t->unk46 = gUnk_0873DACA[2][0];
            break;
        case 10:
            t->unk46 = gUnk_0873DACA[3][0];
            break;
        case 24:
            t->unk46 = gUnk_0873DACA[4][0];
            break;
        }
        TaskSetFrame(gCurTask->unk46);
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        TaskYieldTrampoline(2);
        gCurTask->frame++;
        switch (gCurTask->unk88->unk3A)
        {
        case 1:
            if (gPlayerHealth[gCurTask->unk18] < gMaxHealth)
            {
                do
                {
                    if (gLocalPlayer == gCurTask->unk88->unk00 || gLocalPlayer == gCurTask->unk18)
                        PlaySfx(221);
                    gCurTask->unk28 = sub_080b4204(gCurTask->unk18);
                    TaskYieldTrampoline(8);
                } while (gCurTask->unk28 == 0);
            }
            TaskYieldTrampoline(16);
            break;
        case 2:
            if (gPlayerHealth[gCurTask->unk18] < gMaxHealth)
            {
                if (gExtraMode == 0)
                    n = 2;
                else
                    n = 1;
                for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C < n; gCurTask->unk6C++)
                {
                    if (gLocalPlayer == gCurTask->unk88->unk00 || gLocalPlayer == gCurTask->unk18)
                        PlaySfx(221);
                    gCurTask->unk28 = sub_080b4204(gCurTask->unk18);
                    TaskYieldTrampoline(8);
                    if (gCurTask->unk28 != 0)
                        break;
                }
            }
            TaskYieldTrampoline(16);
            break;
        case 3:
            if ((s8)q->unk22 != 0)
                q->unk1E = q->unk20 = 0;
            q->unk17 = gCurTask->unk88->unk17;
            q->unk18 = gCurTask->unk88->unk18;
            TaskYieldTrampoline(32);
            break;
        }
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
        gCurTask->frame--;
        TaskYieldTrampoline(2);
    }
    gCurTask->facing = a43;
    gCurTask->layer = a42;
    gCurTask->unk88->unk42 &= 0xFEFF;
    u->facing = b43;
    u->layer = b42;
    gPlayerStates[gCurTask->unk18].unk42 &= 0xFEFF;
    u->taskClass--;
    FreezeOtherTasks(0);
    TaskSetSkipMask(0, gCurTaskIdx);
    gCurTask->unk88->unk3B |= 1 << gCurTask->unk18;
    TaskSleepForever();
}

void sub_0803c990(void)
{
    struct PlayerState *p = gCurTask->unk88;

    if (p->unk0D != 24)
        sub_08040710();
    else
        p->unk01 = 55;
}

/* M09's sub_08033414: step and draw the player's three sparks (twin of M04's
   sub_080109c8, src/player_109c8.c).  a != 0 freezes the frame counter and
   draws frame `a` with no palette offset; Task.skipMask & 1 skips the motion. */
void sub_0803c9b4(s32 a)
{
    struct M04Spark *p;
    s32 i;
    s32 n;
    s32 t;
    s32 u;
    s32 d;
    s32 k;
    s32 x;
    s32 y;

    for (i = 0; i <= 2; i++)
    {
        p = &gUnk_02007E90[gCurTask->unk88->unk00][i];
        if (gCurTask->skipMask & 1)
        {
            k = 0;
            if (a == 0)
                k = gCurTask->unk40 + 0x180C;
            d = p->unk0D;
            if (a == 0 && d == 0)
                d = (gCurTask->facing == 1) ? 2 : 3;
        }
        else
        {
            if (p->unk00 == 0)
            {
                p->unk08 = 0;
                n = RandomRange(16);
                if (gCurTask->facing == 1)
                    p->unk00 = gUnk_0873A924[0][n] << 16;
                else
                    p->unk00 = -(gUnk_0873A924[0][n] << 16);
                p->unk04 = gUnk_0873A924[1][n] << 16;
            }
            if (abs(p->unk00) <= 0xF0000)
            {
                p->unk00 = 0;
                p->unk0C = 1;
                p->unk0D = 0;
                continue;
            }
            if (p->unk00 > 0)
                p->unk08 -= 0x6000;
            else
                p->unk08 += 0x6000;
            /* The two volatile reads are the twin's placeholders: the ROM
               re-reads unk00 here and unk04 below, and a plain read is
               folded into the earlier one by gcse's PRE. */
            p->unk00 = *(volatile s32 *)&p->unk00 + p->unk08;
            /* the shift count reuses n: its own local takes r1 (sh/u swap) */
            n = (abs(p->unk00) >> 20) + 1;
            u = *(volatile s32 *)&p->unk04;
            t = (abs(u) & 0xFFFF0000) >> n;
            if (p->unk04 > 0)
                t = -t;
            p->unk04 += t;
            d = a;
            k = 0;
            if (a == 0)
            {
                if (--p->unk0C == 0)
                {
                    if (p->unk0D == 0)
                    {
                        if (gCurTask->facing == 1)
                            p->unk0D = 2;
                        else
                            p->unk0D = 3;
                    }
                    else if (p->unk0D <= 9)
                        p->unk0D += 2;
                    p->unk0C = 1;
                }
                d = p->unk0D;
                k = gCurTask->unk40 + 0x180C;
            }
        }
        x = gCurTask->unk48 + ((s16 *)&p->unk00)[1];
        y = gCurTask->unk4A + ((s16 *)&p->unk04)[1] + 4;
        if (IsWorldPosOnScreen(x, y))
        {
            x -= gSpriteCameraX;
            y -= gSpriteCameraY;
            QueueSprite(gCurTask->layer, gUnk_0873A964[d], 0, k, x, y);
        }
    }
}

void sub_0803cbd8(void)
{
    struct Unk0873A994 *e;
    if ((s8)--gCurTask->unk88->unk29 > 0)
    {
        gCurTask->unk88->unk29--;
        return;
    }
    e = gUnk_0873A994[(s8)gCurTask->unk88->filler2A];
    e += (s8)gCurTask->unk88->unk28;
    if (e->unk4 != 0)
    {
        if (e->unk4 & 128)
            TaskSetSkipMask(3, gCurTaskIdx);
        if (e->unk4 & 64)
        {
            if (gCurTask->facing == 1)
                gCurTask->unk88->unk24 = e->unk0;
            else
                gCurTask->unk88->unk24 = -e->unk0;
        }
        else
        {
            gCurTask->unk88->unk24 = e->unk0;
        }
        gCurTask->unk88->unk26 = e->unk2;
        gCurTask->unk88->unk29 = e->unk4 & 15;
        gCurTask->unk88->unk28++;
    }
    else
    {
        gCurTask->unk88->unk24 = gCurTask->unk88->unk26 = 0;
        gCurTask->unk88->unk40 &= 0xFFFE;
        TaskSetSkipMask(0, gCurTaskIdx);
    }
}

void sub_0803ccd8(s32 a)
{
    u16 *e = gUnk_0873AEBC[a];
    s32 x = e[1] << 8;
    s32 y;
    struct Task *t;

    if (e[1] & 0x8000)
        x |= 0xFF000000;
    y = e[1] << 8;
    if (e[1] & 0x8000)
        y |= 0xFF000000;
    TaskSetMotionY(x, 0, y);
    t = gCurTask;
    if (t->facing == 1)
    {
        s32 v = e[0] << 8;
        if (e[0] & 0x8000)
            v |= 0xFF000000;
        t->unk2C = v;
    }
    else
    {
        t->unk2C = -(e[0] & 0x8000 ? (e[0] << 8) | 0xFF000000 : e[0] << 8);
    }
}
