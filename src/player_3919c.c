#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_3919c.c (0x0803919C-0x08039C23, issue #91).
 *
 * Player action bodies, part 7: actions 17 and 20.  sub_0803919c (action
 * 17) is the player's death: it installs sub_080396a4 as the task's
 * per-frame callback Task.unk04 (state 1 falls until the player is below
 * the screen, state 2 waits PlayerState.unk14 frames, state 4 leaves for
 * the results screen), counts the players whose health gPlayerHealth[] is
 * not 0, plays the lost-life or game-over music, loops the fall animation
 * until the callback reaches state 3, and when gUnk_0300234C drops to 0
 * raises M02's stage request gStageRequest = 6; Task.unk73 = 4 or 5 then
 * says whether the player has lives left (gPlayerLives[]).
 * sub_080397f8 (action 20) enters a door: it stops the player, plays the
 * landing or crouch animation picked by M11's sub_080404e4, calls M07's
 * door code sub_08025024 and plays the ability's door animation
 * (gUnk_0873D632[ability][0]). */

struct CamPos { u16 x, y; };

extern u8 gActivePlayerCount;
extern u8 gActivePlayerMask;
extern u16 gUnk_0873D9FA[][2];
extern u16 gPlayerCount;               /* number of players */
extern s16 gPlayerHealth[];             /* health per player (M02's HUD) */
extern u16 gLocalPlayer;
extern vu16 gDispCnt;              /* DISPCNT shadow */
extern vs16 gTaskSlotTypes[];
extern u8 gUnk_02007CF0;
extern u8 gUnk_0300234C;
extern s8 gStageRequest;                /* stage request (M02) */
extern s16 gPlayerLives[];
extern u8 gUnk_03001F34;
extern u16 gGameState;
extern struct CamPos gPlayerCameraPos[4];
extern s16 gSpriteCameraY;
extern s8 gUnk_03002444;
extern s16 gUnk_0873D7E4[][3];
extern u16 gUnk_0873D632[][7];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
s32 PlayBgm(s32 songId);
s32 PlaySfx(s32 id);
void StopAllSound(void);
s32 StopOtherSfx(s32 songId);
void StopAllSfx(void);
void TaskSetSkipMask(u8 val, s32 idx);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
void TaskSetFrame(s32 a);
s32 SetPlayerAbility(s32 a, s32 b, u32 c);
s32 sub_08025024(void);
void RequestScreenShake(u16 a);
s32 sub_080264b0(void);
void PauseRoom(void);
void SetRoomUpdateFlags(u32 a);
void sub_08027548(void);
s32 sub_080276ac(s32 a);
s32 sub_080276cc(s32 i);
void PlayerStopAxes(s32 a0);
void sub_0803e080(void);
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
void FreezeOtherTasks(s32 a0);
void sub_0803e868(void);
u16 sub_0803f7e0(u16 a0);
s32 sub_080404e4(void);
void sub_08040934(s32 a0);
void PlayerSetMotionYPreset(s32 a0);
void CreatePlayerObject(s32 a, s32 b, s32 c);      /* M14, src/plobj_52f6c.c (defined s32 (s8, u8, s32); the result is unused here) */
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);
s32 CreatePlayerEffectHighSlot(s32 a0, s32 a1, s32 a2);
void sub_080b9118(void);
void sub_080b9610(void);
void sub_080396a4(void);

void sub_0803919c(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *t2;
    u16 *anim;
    u16 n;
    u16 i;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 18;
    gCurTask->unk04 = (u32)sub_080396a4;
    gCurTask->unk08 = 0;
    gCurTask->unk43 = 1;
    t = gCurTask;
    t->unk3E &= 0x7FFF;
    gActivePlayerCount--;
    gActivePlayerMask &= ~(1 << t->unk88->unk00);
    if (t->unk82 == 0x200)
        sub_08027548();
    u = gCurTask;
    u->unk88->unk40 |= 8;
    u->unk88->unk42 |= 0x100;
    u->unk73 = 0;
    gCurTask->unk88->unk42 &= 0xFFEF;
    SetPlayerInvulnerability(255, 0, gCurTask->unk88->unk00);
    PlayerStopAxes(3);
    gCurTask->unk88->unk06 = 0;
    SetPlayerAbility(0, -1, gCurTask->unk88->unk00);
    sub_080276ac(gCurTask->unk88->unk00);
    gCurTask->unk88->unk16 = 255;
    anim = gUnk_0873D9FA[gCurTask->unk88->unk0D];
    n = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if (gPlayerHealth[i] != 0)
            n++;
    }
    if (n == 0)
    {
        StopAllSound();
        StopAllSfx();
        gCurTask->unk42 = 4;
        gCurTask->unk88->unk17 = n;
        gCurTask->unk88->unk18 = n;
        gCurTask->unk88->unk1A = gCurTask->unk88->unk1C = n;
    }
    else
    {
        sub_0803e868();
    }
    if (gLocalPlayer == gCurTask->unk88->unk00)
    {
        gCurTask->unk3C = anim[0];
        if (gPlayerCount == 1)
        {
            StopAllSfx();
            StopAllSound();
            FreezeOtherTasks(15);
            SetRoomUpdateFlags(2);
            if (!(gDispCnt & 0x400))
            {
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1D00;
            }
        }
        else if (gActivePlayerMask == 0)
        {
            for (i = 4; i <= 63; i++)
            {
                if (gTaskSlotTypes[i] != -1 && i != 63)
                    TaskSetSkipMask(15, i);
            }
            StopAllSfx();
            PauseRoom();
            SetRoomUpdateFlags(2);
            if (!(gDispCnt & 0x400))
            {
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1D00;
            }
        }
        else
        {
            sub_08040934(gCurTask->unk88->unk00);
        }
        if (gCurTask->unk88->unk37 != 2 || gActivePlayerMask == 0)
            RequestScreenShake(4);
    }
    else
    {
        gCurTask->unk3C = anim[0];
        if (gActivePlayerMask == 0)
        {
            for (i = 4; i <= 63; i++)
            {
                if (gTaskSlotTypes[i] != -1 && i != 63)
                    TaskSetSkipMask(15, i);
            }
            StopAllSfx();
            PauseRoom();
            SetRoomUpdateFlags(2);
            RequestScreenShake(4);
            if (!(gDispCnt & 0x400))
            {
                gDispCnt &= 0xE0FF;
                gDispCnt |= 0x1D00;
            }
        }
        else
        {
            sub_08040934(gCurTask->unk88->unk00);
        }
    }
    TaskYieldTrampoline(1);
    if (gActivePlayerMask == 0)
        PlaySfx(158);
    else
        PlaySfxIfLocalPlayer(158, gCurTask->unk88->unk00);
    TaskYieldTrampoline(59);
    gCurTask->unk73 = 1;
    gCurTask->unk88->unk22 = 1;
    gCurTask->unk88->unk12 = 0x8000;
    for (i = 0; i <= 3; i++)
        CreatePlayerEffect(gCurTask->unk88->unk00, 12, i);
    if (n == 0)
        PlayBgm(3);
    else if (gUnk_02007CF0 != 1)
        PlaySfx(270);
    PlayerSetMotionYPreset(32);
    gCurTask->unk46 = anim[1];
    do
    {
        struct Task *w = gCurTask;
        struct Task *x;

        if (w->unk0C != 0)
            CreatePlayerEffectHighSlot(w->unk88->unk00, 5, 0);
        gCurTask->unk3C = gCurTask->unk46;
        TaskYieldTrampoline(1);
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 6; gCurTask->unk6C++)
        {
            gCurTask->unk3C--;
            TaskYieldTrampoline(1);
        }
        x = gCurTask;
        if (x->unk0C != 0)
            CreatePlayerEffectHighSlot(x->unk88->unk00, 5, 0);
        for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 7; gCurTask->unk6C++)
        {
            gCurTask->unk3C--;
            TaskYieldTrampoline(1);
        }
    } while (gCurTask->unk73 != 3);
    gUnk_0300234C--;
    t2 = gCurTask;
    t2->unk00 = 0;
    t2->unk0C = 0;
    t2->unk08 = 0;
    if (gUnk_0300234C == 0)
    {
        gStageRequest = 6;
        TaskExitTrampoline();
    }
    sub_080276cc(gCurTask->unk88->unk00);
    if (gPlayerLives[gCurTask->unk88->unk00] == 0)
        gCurTask->unk73 = 4;
    else
        gCurTask->unk73 = 5;
    gUnk_03001F34 = 0;
    TaskSleepForever();
}

void sub_080396a4(void)
{
    switch (gCurTask->unk73)
    {
    case 4:
    {
        struct Task *t = gCurTask;

        t->unk04 = 0;
        t->unk12 = 4;
        if (gGameState != 20)
            TaskSetEntry(sub_080b9610, gCurTaskIdx);
        else
            sub_080b9118();
        break;
    }
    case 1:
    {
        struct Task *t = gCurTask;

        if (t->unk58 > 0)
        {
            struct PlayerState *p;
            u16 y;

            t->unk68 = 0x40000;
            p = t->unk88;
            if (p->unk37 != 2)
                y = t->unk4A - (u16)(gPlayerCameraPos[p->unk00].y - 80);
            else
                y = t->unk4A;
            if ((s16)y > 184)
            {
                struct Task *u;
                struct PlayerState *q;

                gCurTask->unk88->unk14 = 90;
                gCurTask->unk73 = 2;
                PlayerStopAxes(2);
                u = gCurTask;
                q = u->unk88;
                if (q->unk37 != 2 && u->unk4A - gSpriteCameraY <= 183)
                    CreatePlayerEffect(q->unk00, 18, 0);
                gCurTask->unk0C = 0;
            }
        }
        if (!(gCurTask->unk88->unk42 & 32))
            sub_0803e080();
        break;
    }
    case 2:
    {
        struct Task *t = gCurTask;

        if (--t->unk88->unk14 == 0)
            t->unk73 = 3;
        break;
    }
    case 0:
    case 3:
    case 5:
        break;
    }
}

void sub_080397f8(void)
{
    struct Task *t;
    s32 n;
    s32 i;

    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 19;
    gCurTask->unk04 = 0;
    gCurTask->unk08 = 0;
    gCurTask->unk88->unk3D = 0;
    PlayerStopAxes(3);
    gCurTask->unk88->unk42 |= 0x100;
    RequestScreenShake(0);
    if (gUnk_03002444 == 0)
    {
        FreezeOtherTasks(15);
        if (!(gDispCnt & 0x400))
        {
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x1D00;
        }
        n = 0;
        if (gPlayerCount > 1 && gUnk_0300234C > 1)
        {
            for (i = 0; i < gPlayerCount; i++)
            {
                if (gPlayerHealth[i] == 0)
                {
                    n++;
                    TaskSetSkipMask(0, i);
                }
            }
        }
        if (n != 0)
            StopOtherSfx(158);
        else
            StopAllSfx();
    }
    switch (sub_080404e4())
    {
    case 1:
        gCurTask->unk88->unk06 = 0;
        CreatePlayerObject(gCurTask->unk88->unk00, 0, 0);
        TaskSetFrame(gUnk_0873D7E4[gCurTask->unk88->unk0D][2]);
        TaskYieldTrampoline(6);
        gCurTask->unk3C--;
        TaskYieldTrampoline(2);
        break;
    case 2:
        gCurTask->unk88->unk06 = 0;
        if (!(gCurTask->unk7B & 1))
        {
            TaskSetFrame(79);
            TaskYieldTrampoline(1);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            if ((s8)gCurTask->unk88->unk07 > 1)
                CreatePlayerObject(gCurTask->unk88->unk00, 2, 0);
            else
                CreatePlayerObject(gCurTask->unk88->unk00, 1, 0);
            if (gCurTask->unk7A & 1)
            {
                CreatePlayerEffect(gCurTask->unk88->unk00, 2, 0);
                CreatePlayerEffect(gCurTask->unk88->unk00, 2, 1);
            }
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            gCurTask->unk3C++;
    default:
            TaskYieldTrampoline(1);
            break;
        }
        TaskSetFrame(224);
        TaskYieldTrampoline(2);
        gCurTask->unk3C--;
        TaskYieldTrampoline(2);
        if ((s8)gCurTask->unk88->unk07 > 1)
            CreatePlayerObject(gCurTask->unk88->unk00, 2, 0);
        else
            CreatePlayerObject(gCurTask->unk88->unk00, 1, 0);
        gCurTask->unk3C--;
        TaskYieldTrampoline(2);
        break;
    }
    sub_08025024();
    if (gUnk_03002444 != 0)
        sub_080264b0();
    if (gUnk_03002444 == 0)
        PlaySfx(181);
    t = gCurTask;
    if (!(t->unk7B & 1))
    {
        t->unk46 = gUnk_0873D632[t->unk88->unk0D][0];
        switch (t->unk88->unk0D)
        {
        case 0:
        default:
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            break;
        case 1:
        case 2:
        case 5:
        case 19:
        case 22:
        case 23:
            TaskSetFrame(gCurTask->unk46);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            while (1)
            {
                TaskSetFrame((s16)(gCurTask->unk46 + 3));
                TaskYieldTrampoline(2);
                TaskSetFrame((s16)(gCurTask->unk46 + 15));
                TaskYieldTrampoline(2);
            }
        }
    }
    else
    {
        gCurTask->unk46 = sub_0803f7e0(0);
        TaskSetFrame(gCurTask->unk46);
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
        TaskYieldTrampoline(3);
        gCurTask->unk3C++;
    }
    TaskSleepForever();
}
