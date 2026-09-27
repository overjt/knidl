#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_32688.c (0x08032688-0x080337F3, issue #92).
 *
 * Task type #5 (class 1), the player task, and its callbacks.
 * Task_Player is the body: it binds the task to its player record
 * (Task.unk88 = &gPlayerStates[gCurTaskIdx]), kills it when the player
 * has no lives and no health left, installs the callbacks (Task.unk00 =
 * M11's PlayerMove, unk04 = PlayerUpdate, unk08 = sub_0803332c, unk0C =
 * M11's sub_0803ddc0), sets up the ability (PlayerState.unk0D) and the
 * stage entry mode (gUnk_02000020, gUnk_020069F0), and starts the first
 * action.  The actions are two tables of void (*)(void) dispatched
 * through CallTableEntry(index, count, table), entry 0 NULL: the "enter"
 * coroutine of action PlayerState.unk02 from gPlayerActions[62] (M11's
 * gUnk_0873B42C[30] when gUnk_03001F30 != 0) and the "per-frame" handler
 * Task.unk15 from gPlayerActionHandlers[57] (M11's gUnk_0873B4A4[27]).  A handler requests
 * the next action in PlayerState.unk01; PlayerStartRequestedAction is the coroutine
 * that switches to it (unk03 = previous, unk02 = new, unk01 = 0).
 * PlayerUpdate (Task.unk04) runs every frame: the attack hit-boxes
 * (TaskBreakBlocks on PlayerState.unk6C), the collision registry, the
 * per-frame handler and the damage and star-block reactions;
 * sub_0803332c (Task.unk08) runs the 10-frame timer PlayerState.unk2B;
 * sub_08033414 (called by M11's sub_0803ddc0) turns the frame's hit
 * event Task.unk7C and the status bits PlayerState.unk40 into an action
 * request, re-binds the task to PlayerStartRequestedAction when one is pending and
 * adds the 8.8 offsets PlayerState.unk24/unk26 to the 16.16 position. */

struct M11R8 { u8 unk00; u8 unk01; u8 unk02; u8 unk03; u8 *unk04; };

struct M11R20 { u32 w[5]; };

/* A hit-box set: unk0 & 0x8000 = mirror with the task's facing, unk0 & 0xFFF
   = the attack id passed to CanBreakBlock; unk2/unk3 = (x, y) offset of the
   set; unk4 = the boxes, {y0, y1, x0, x1} each (x mirrored as -x1..-x0),
   terminated by y0 == 127. */
struct HitBoxSet
{
    /*0x00*/ u16 unk0;
    /*0x02*/ s8 unk2;
    /*0x03*/ s8 unk3;
    /*0x04*/ s8 (*unk4)[4];
};

/* gTerrainResult: M06's collision result block (src/terrain_1bcac.c spells it
   the same way except unk8, which M09 reads with ldrsh). */
struct Unk03005550
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ s16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
};

struct Unk03005530
{
    /*0x00*/ u8 unk0;
    /*0x01*/ u8 unk1;
    /*0x02*/ u8 unk2;
    /*0x03*/ u8 unk3;
    /*0x04*/ u8 unk4;
    /*0x05*/ u8 unk5;
    /*0x06*/ u8 unk6;
    /*0x07*/ u8 unk7;
    /*0x08*/ u16 unk8;
    /*0x0A*/ u8 unkA;
    /*0x0B*/ u8 unkB;
    /*0x0C*/ u8 unkC;
    /*0x0D*/ u8 unkD;
    /*0x0E*/ u8 unkE;
    /*0x0F*/ u8 unkF;
    /*0x10*/ u8 unk10;
};

struct Unk02005E00
{
    /*0x00*/ s32 unk00;
    /*0x04*/ u8 unk04[4];
    /*0x08*/ u8 unk08[4];
};

extern struct PlayerState gPlayerStates[];
extern s16 gPlayerLives[];
extern s16 gPlayerHealth[];
extern u8 gUnk_030023B0;
extern u16 gGameState;
extern u32 gUnk_0874CFEC[];
extern u16 gPlayerCount;
extern u16 gLocalPlayer;
extern u8 gUnk_03001F30;
extern u32 gPlayerDefaultBodyBox[];               /* stored to PlayerState.unk64 as (u32)gPlayerDefaultBodyBox */
extern u32 gUnk_0873CA54[];
extern u32 gPlayerDefaultTerrainBox[];
extern struct M11R20 gPlayerBodyBoxes[];
extern u32 gUnk_0873C358[];
extern struct M11R8 gPlayerHitBoxSets[];
extern u32 gUnk_0873CF94[];
extern u8 gUnk_02000020;
extern u8 gUnk_020069F0;
extern u8 gUnk_020061E0;
extern u8 gUnk_03001F34;
extern void (*gPlayerActions[])(void);
extern void (*gUnk_0873B42C[])(void);
extern u8 gUnk_03005568;
extern u8 gUnk_02005574[];
extern struct Unk03005550 gTerrainResult;
extern u16 gUnk_03005544;
extern struct Unk03005530 gTerrainProbeResult;
extern s16 gSpriteCameraX;
extern s16 gSpriteCameraY;
extern void (*gPlayerActionHandlers[])(void);
extern void (*gUnk_0873B4A4[])(void);
extern s16 gUnk_0300244C;
extern u16 gPlayerBubbleTimers[];
extern u32 gUnk_0873C36C[];
extern u32 gUnk_0873CF9C[];
extern struct Unk02005E00 gUnk_02005E00;
extern u16 gPlayerHeldKeys[];
extern u8 gUnk_02007CF0;
extern u16 gPlayerAbilities[];
extern u16 gSavedPlayerAbilities[];
extern u16 gUnk_02007FA8[];
extern u16 gUnk_0200AF18[];

void CallTableEntry(u32 idx, u32 count, void (**fns)(void));
u32 RandomRange(u32 range);
s32 PlayBgm(s32 songId);
void TaskSetSkipMask(u8 val, s32 idx);
void TaskSleepForever(void);
void TaskSetEntry(void *a, u32 i);
s32 AddPlayerHealth(s32 a, u32 b);
s32 SetPlayerAbilityNoHud(s32 a, s32 b, u32 c);
s32 SetPlayerAbility(s32 a, s32 b, u32 c);
u32 RegisterCollider(u8 idx, s16 x, s16 y, u8 *p);   /* M09's callers pass ldrsh values unextended (LESSONS 8) */
void sub_0801baa4(u32 a);
void sub_08021c74(s8 *box, s32 id);
void TaskInitWaterFlags(void);
s32 sub_080260b0(void);
void SetCameraFocus(s32 x, s32 y);
u16 TaskBreakBlocks(struct HitBoxSet *p, s32 e);
void sub_0803c9b4(s32 a);                     /* M10: mov r8, r0 on entry, void epilogue */
void sub_0803cbd8(void);                      /* M10: no argument read, void epilogue */
void sub_0803ce98(void);
void PlayerMove(void);
void sub_0803ddc0(void);
void sub_0803e080(void);
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2);
void PlayerUpdateInvulnerability(void);
void PlayerStopSfx(void);
s32 LoadPlayerBodyBoxRect(s32 playerIdx, u8 *src6);
s32 LoadPlayerHitBoxSet(s32 a0, s32 a1);
void PlayerStartOffsetScript(s32 a0);
s32 sub_0803fa74(void);
void sub_0803fb54(void);
s32 sub_08040514(void);
void sub_08040808(s32 a0);
void sub_08040894(s32 a0, u8 a1);
void LoadAbilityTiles(void);
void sub_08049a58(void);
void sub_0804fe68(void);
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);
s32 CreatePlayerEffectHighSlot(s32 a0, s32 a1, s32 a2);
void sub_0805b278(void);
void sub_0806ee30(void);
void sub_08071cc0(int x, int y, int c);
void sub_08076318(void);
void sub_080b8ebc(void);
void sub_080b9118(void);
void sub_080b9610(void);
void PlayerUpdate(void);
void sub_0803332c(void);

void Task_Player(void)
{
    struct Task *t;

    gCurTask->unk44 = gCurTaskIdx;
    gCurTask->unk88 = &gPlayerStates[gCurTaskIdx];
    if (gPlayerLives[gCurTask->unk88->unk00] == 0 && gPlayerHealth[gCurTask->unk88->unk00] == 0)
    {
        gCurTask->unk04 = 0;
        gCurTask->unk12 = 4;
        if (gUnk_030023B0 == 0)
        {
            if (gGameState != 20)
                sub_080b9610();
            else
                sub_080b9118();
            TaskSleepForever();
        }
        TaskSleepForever();
    }
    else
    {
        sub_080b8ebc();
    }
    gCurTask->unk43 = 1;
    t = gCurTask;
    t->unk00 = (u32)PlayerMove;
    t->unk0C = (u32)sub_0803ddc0;
    t->unk04 = (u32)PlayerUpdate;
    t->unk08 = (u32)sub_0803332c;
    t->unk38 = gUnk_0874CFEC;
    if (gPlayerCount > 1 && gLocalPlayer == t->unk88->unk00)
        t->unk42 = 6;
    else
        t->unk42 = 7;
    gCurTask->unk40 = (gCurTask->unk88->unk00 << 13) | (gCurTask->unk88->unk00 << 7);
    gCurTask->unk76 = 0;
    gCurTask->unk88->unk01 = 0;
    if (gUnk_03001F30 == 0)
        gCurTask->unk88->unk64 = (u32)gPlayerDefaultBodyBox;
    else
        gCurTask->unk88->unk64 = (u32)gUnk_0873CA54;
    gCurTask->unk88->unk68 = (u32)gPlayerDefaultTerrainBox;
    gCurTask->unk88->unk6C = 0;
    gCurTask->unk88->unk5E = gCurTask->unk4C >> 16;
    gCurTask->unk88->unk60 = gCurTask->unk50 >> 16;
    gCurTask->unk78 = gPlayerHealth[gCurTask->unk88->unk00];
    if (gCurTask->unk88->unk0D != 0)
    {
        LoadAbilityTiles();
        switch (gCurTask->unk88->unk0D)
        {
        case 1:
        case 2:
            CreatePlayerEffect(gCurTask->unk88->unk00, 15, 0);
            sub_08049a58();
            break;
        case 10:
            {
                struct M11R20 *d = gPlayerBodyBoxes;

                d[gCurTask->unk88->unk00] = *(struct M11R20 *)gUnk_0873C358;
            }
            gPlayerHitBoxSets[gCurTask->unk88->unk00] = *(struct M11R8 *)gUnk_0873CF94;
            break;
        case 7:
        case 20:
        case 21:
            gCurTask->unk88->unk22 = 2;
            break;
        case 24:
            if (gGameState != 5)
                break;
        case 11:
            SetPlayerAbility(0, -1, gCurTask->unk88->unk00);
            break;
        case 25:
            if (gUnk_02000020 != 2 && gUnk_02000020 != 3)
                SetPlayerAbilityNoHud(0, -1, gCurTask->unk88->unk00);
            break;
        }
    }
    gCurTask->unk88->unk36 = 0;
    switch (gUnk_02000020)
    {
    case 0:
        break;
    case 1:
        gCurTask->unk04 = 0;
        gCurTask->unk08 = 0;
        gCurTask->unk88->unk64 = 0;
        gCurTask->unk88->unk68 = 0;
        gCurTask->unk88->unk6C = 0;
        sub_0805b278();
        TaskSleepForever();
    case 2:
        gCurTask->unk88->unk37 = 2;
        SetPlayerAbility(25, -1, gCurTask->unk88->unk00);
        gCurTask->unk88->unk06 = 3;
        sub_0804fe68();
        TaskSleepForever();
    case 3:
        gCurTask->unk88->unk37 = 3;
        SetPlayerAbilityNoHud(25, -1, gCurTask->unk88->unk00);
    }
    switch (gUnk_020069F0)
    {
    case 2:
        gCurTask->unk88->unk64 = 0;
        gCurTask->unk88->unk68 = 0;
        gCurTask->unk88->unk6C = 0;
        if (gCurTask->unk88->unk00 == 0 || gUnk_020061E0 == 0)
            sub_08071cc0(gCurTask->unk4C, gCurTask->unk50, sub_080260b0());
        sub_0806ee30();
        TaskSleepForever();
    case 3:
        gCurTask->unk88->unk64 = 0;
        gCurTask->unk88->unk68 = 0;
        gCurTask->unk88->unk6C = 0;
        gUnk_03001F34 = 1;
        gCurTask->unk14 = 4;
        sub_08076318();
        TaskSleepForever();
    case 1:
        gUnk_03001F34 = 1;
        gCurTask->unk88->unk02 = 21;
        break;
    case 0:
    default:
        TaskInitWaterFlags();
        sub_08021c74((s8 *)gPlayerDefaultTerrainBox, gCurTaskIdx);
        gCurTask->unk88->unk5C = gCurTask->unk7B;
        if (!(gCurTask->unk7B & 1))
        {
            if (gCurTask->unk7A & 1)
                gCurTask->unk88->unk02 = 1;
            else
                gCurTask->unk88->unk02 = 7;
        }
        else
        {
            if (gCurTask->unk7A & 1)
                gCurTask->unk88->unk02 = 24;
            else
                gCurTask->unk88->unk02 = 23;
        }
        if (gCurTask->unk88->unk0D == 24)
            gCurTask->unk88->unk02 = 55;
        gCurTask->unk88->unk04 = 21;
        sub_08040808(gCurTask->unk88->unk00);
        break;
    }
    if (gUnk_03001F30 == 0)
    {
        struct PlayerState *p = gCurTask->unk88;
        gCurTask->unk14 = p->unk02;
        CallTableEntry(p->unk02, 62, gPlayerActions);
    }
    else
    {
        struct PlayerState *p = gCurTask->unk88;
        gCurTask->unk14 = p->unk02;
        CallTableEntry(p->unk02, 30, gUnk_0873B42C);
    }
}

void PlayerStartRequestedAction(void)
{
    if (gPlayerCount > 1)
    {
        if (gLocalPlayer == gCurTask->unk88->unk00)
            gCurTask->unk42 = 6;
        else
            gCurTask->unk42 = 7;
    }
    gCurTask->unk88->unk03 = gCurTask->unk88->unk02;
    gCurTask->unk88->unk02 = gCurTask->unk88->unk01;
    gCurTask->unk88->unk01 = 0;
    if (gCurTask->unk88->unk06 == 1)
    {
        gCurTask->unk88->unk64 = (u32)gPlayerDefaultBodyBox;
        gCurTask->unk88->unk68 = (u32)gPlayerDefaultTerrainBox;
        gCurTask->unk88->unk6C = 0;
    }
    else if (gCurTask->unk88->unk37 != 2)
    {
        if (gUnk_03001F30 == 0)
            gCurTask->unk88->unk64 = (u32)gPlayerDefaultBodyBox;
        else
            gCurTask->unk88->unk64 = (u32)gUnk_0873CA54;
        gCurTask->unk88->unk68 = (u32)gPlayerDefaultTerrainBox;
        gCurTask->unk88->unk6C = 0;
    }
    gCurTask->unk88->unk24 = gCurTask->unk88->unk26 = 0;
    if (gCurTask->unk88->unk3F == 3 && (s16)gCurTask->unk88->unk12 == -0x8000)
        SetPlayerInvulnerability(255, 0, gCurTask->unk88->unk00);
    if (gUnk_03001F30 == 0)
    {
        if (gCurTask->unk88->unk03 == 28 && gCurTask->unk88->unk0D != 0)
            LoadAbilityTiles();
        if (gCurTask->unk88->unk0D != 0)
            gCurTask->unk88->unk36 = 1;
    }
    if (gUnk_03001F30 == 0)
    {
        struct PlayerState *p = gCurTask->unk88;
        gCurTask->unk14 = p->unk02;
        CallTableEntry(p->unk02, 62, gPlayerActions);
    }
    else
    {
        struct PlayerState *p = gCurTask->unk88;
        gCurTask->unk14 = p->unk02;
        CallTableEntry(p->unk02, 30, gUnk_0873B42C);
    }
}

void PlayerUpdate(void)
{
    s32 x;
    s32 y;
    s32 r;
    struct PlayerState *p;

    if ((gCurTask->unk88->unk40 & 1) && (gCurTask->unk13 & 1))
        goto post;
    if (gCurTask->unk88->unk0D == 10 && gCurTask->unk88->unk04 == 5)
    {
        gCurTask->unk2C += gCurTask->unk28;
        gCurTask->unk4C += gCurTask->unk2C;
        gCurTask->unk48 = gCurTask->unk4C >> 16;
    }
    if (gCurTask->unk88->unk6C != 0)
    {
        gCurTask->unk88->unk44 = TaskBreakBlocks(gCurTask->unk88->unk6C, gCurTask->unk88->unk00);
        if (gCurTask->unk88->unk44 != 0)
            gCurTask->unk76 |= 1;
    }
    else
    {
        gCurTask->unk88->unk44 = 0;
    }
    gCurTask->unk88->unk5C = gCurTask->unk7B;
    gCurTask->unk88->unk4E = 0xFFFF;
    if (gCurTask->unk88->unk68 != 0)
    {
        sub_0801baa4(gCurTask->unk88->unk68);
        gCurTask->unk88->unk48 = gUnk_03005568;
        if (gUnk_02005574[0] == 0 && (gUnk_03005568 & 4) && gTerrainResult.unk0 != 0)
            gCurTask->unk88->unk4E = gUnk_03005544;
        gCurTask->unk88->unk70 = (u32 *)gCurTask->unk88->unk68;
        if (gTerrainResult.unkC != 0 && !(gCurTask->unk88->unk42 & 0x200)
         && gCurTask->unk88->unk3F != 1 && gCurTask->unk88->unk17 == 0)
        {
            r = AddPlayerHealth(-8, gCurTask->unk88->unk00);
            if (r != 0)
            {
                gCurTask->unk82 = gTerrainResult.unkC | 0x80;
                gCurTask->unk7C = 2;
            }
            else
            {
                gCurTask->unk7C = 1;
                gCurTask->unk82 = 0;
                goto post;
            }
        }
    }
    else
    {
        gTerrainResult.unk0 = gTerrainResult.unk1 = gTerrainResult.unk2 = 0;
        gTerrainResult.unk3 = gTerrainResult.unk4 = gTerrainResult.unk5 = 0;
        gTerrainResult.unk8 = gTerrainResult.unkB = gTerrainResult.unkC = 0;
        gTerrainResult.unkA = 0;
        gTerrainProbeResult.unkE = 0;
    }
    gCurTask->unk88->unk4A = gTerrainResult.unk0;
    gCurTask->unk88->unk4B = gTerrainResult.unk4;
    gCurTask->unk88->unk49 = gTerrainProbeResult.unkE;
    gCurTask->unk88->unk4C = gTerrainResult.unkA;
    if (sub_0803fa74() != 0)
        goto tail;
    p = gCurTask->unk88;
    if (p->unk64 != 0)
    {
        x = gCurTask->unk48;
        y = gCurTask->unk4A;
        if (p->unk37 == 2)
        {
            x += gSpriteCameraX;
            y += gSpriteCameraY;
        }
        RegisterCollider(gCurTaskIdx, x, y, (u8 *)p->unk64);
    }
    if (gUnk_03001F30 == 0)
        CallTableEntry(gCurTask->unk15, 57, gPlayerActionHandlers);
    else
        CallTableEntry(gCurTask->unk15, 27, gUnk_0873B4A4);
    PlayerUpdateInvulnerability();
post:
    gCurTask->unk88->unk45 = 0;
    sub_0803fb54();
    if (!(gCurTask->unk88->unk42 & 32))
        sub_0803e080();
    if ((gUnk_03001F30 == 1 || gUnk_0300244C != 0)
     && (gCurTask->unk88->unk40 & 1) && (gCurTask->unk13 & 1))
        goto check;
    if (gCurTask->unk58 >= 0)
    {
        if (gCurTask->unk7B & 0x80)
            CreatePlayerEffect(gCurTask->unk88->unk00, 9, gTerrainResult.unk8);
    }
    else if (gCurTask->unk88->unk06 == 2)
    {
        if (gCurTask->unk7B & 0x80)
            CreatePlayerEffect(gCurTask->unk88->unk00, 10, gTerrainResult.unk8);
    }
    else if ((gCurTask->unk88->unk5C & 1) && !(gCurTask->unk7B & 1))
    {
        CreatePlayerEffect(gCurTask->unk88->unk00, 10, gTerrainResult.unk8);
    }
    if ((gCurTask->unk7B & 65) == 1)
    {
        if (--gPlayerBubbleTimers[gCurTask->unk88->unk00] == 0)
        {
            gPlayerBubbleTimers[gCurTask->unk88->unk00] = RandomRange(90) + 120;
            CreatePlayerEffectHighSlot(gCurTask->unk88->unk00, 11, 0);
        }
    }
    else
    {
        gPlayerBubbleTimers[gCurTask->unk88->unk00] = 60;
    }
check:
    if (gUnk_03001F30 == 0)
    {
        if (gCurTask->unk88->unk0D == 10)
        {
            x = gCurTask->unk3C - 0x808;
            if (x >= 0 && LoadPlayerBodyBoxRect(gCurTask->unk88->unk00, (u8 *)gUnk_0873C36C + x * 8) != 0)
            {
                if (gCurTask->unk3C <= 0x8D1)
                {
                    struct M11R20 *d = gPlayerBodyBoxes;
                    ((u8 *)&d[gCurTask->unk88->unk00])[12] = 2;
                }
                else
                {
                    struct M11R20 *d = gPlayerBodyBoxes;
                    ((u8 *)&d[gCurTask->unk88->unk00])[12] = 5;
                }
                RegisterCollider(gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A,
                             (u8 *)gPlayerBodyBoxes + gCurTask->unk88->unk00 * 20);
            }
            x = gCurTask->unk3C - 0x8D2;
            if (x >= 0 && LoadPlayerHitBoxSet(gCurTask->unk88->unk00, (s32)((u8 *)gUnk_0873CF9C + x * 8)) != 0)
                TaskBreakBlocks((struct HitBoxSet *)&gPlayerHitBoxSets[gCurTask->unk88->unk00], gCurTask->unk88->unk00);
        }
        if (gUnk_02005E00.unk04[gCurTaskIdx] & 1)
        {
            if ((gPlayerHeldKeys[gCurTask->unk88->unk00] & 0x300) == 0x300)
                gUnk_02005E00.unk04[gCurTaskIdx] = (gUnk_02005E00.unk04[gCurTaskIdx] & 0xF0) | 2;
        }
    }
    else if (gCurTask->unk88->unk10 != 0)
    {
        gCurTask->unk88->unk10--;
    }
tail:
    if (gCurTask->unk88->unk37 != 2 && gLocalPlayer == gCurTask->unk88->unk00)
        SetCameraFocus(gCurTask->unk4C >> 16, gCurTask->unk50 >> 16);
    if (gCurTask->unk88->unk68 != 0 && !(gCurTask->unk13 & 2))
    {
        gCurTask->unk88->unk5E = gCurTask->unk48;
        gCurTask->unk88->unk60 = gCurTask->unk4A;
    }
}

void sub_0803332c(void)
{
    struct Task *t;
    struct PlayerState *p;

    t = gCurTask;
    if (t->unk88->unk01 == 0)
    {
        if (t->unk76 & 1)
        {
            t->unk76 &= 0xFFFE;
            if ((s8)t->unk88->unk2B == 0)
            {
                PlayerStartOffsetScript(0);
                gCurTask->unk88->unk2B = 10;
            }
        }
        p = gCurTask->unk88;
        if (p->unk40 & 1)
        {
            sub_0803cbd8();
            p = gCurTask->unk88;
            if (!(p->unk40 & 1) && gUnk_03001F30 == 0 && p->unk04 == 7 && p->unk44 == 0)
            {
                TaskSetEntry(PlayerStartRequestedAction, gCurTaskIdx);
                gCurTask->unk88->unk01 = 18;
            }
        }
        else if ((s8)p->unk2B != 0)
        {
            p->unk2B--;
        }
    }
    if (gCurTask->unk88->unk01 == 0)
        sub_08040514();
}

void sub_08033414(void)
{
    struct Task *t;
    struct PlayerState *p;

    if (gCurTask->unk88->unk01 > 31 && gUnk_02007CF0 == 1)
        gCurTask->unk88->unk01 = 0;
    switch (gCurTask->unk7C)
    {
    default:
        if (gUnk_03001F30 == 0 && (gCurTask->unk88->unk40 & 32))
        {
            if ((s16)gPlayerAbilities[gCurTask->unk88->unk00] != 0)
            {
                gSavedPlayerAbilities[gCurTask->unk88->unk00] = gPlayerAbilities[gCurTask->unk88->unk00];
                gUnk_02007FA8[gCurTask->unk88->unk00] = gUnk_0200AF18[gCurTask->unk88->unk00];
            }
            else
            {
                gSavedPlayerAbilities[gCurTask->unk88->unk00] = 4;
                gUnk_02007FA8[gCurTask->unk88->unk00] = 0xFFFF;
            }
            gCurTask->unk88->unk0B = 4;
            gCurTask->unk88->unk0C = 255;
            gCurTask->unk88->unk37 = 1;
            gCurTask->unk88->unk01 = 29;
        }
        else if (gCurTask->unk88->unk40 & 64)
        {
            SetPlayerInvulnerability(5, 0, gCurTask->unk88->unk00);
            gCurTask->unk88->unk40 &= 0xFFBF;
            PlayBgm(19);
            sub_08040894(gCurTask->unk88->unk00, 3);
        }
        break;
    case 1:
        gCurTask->unk88->unk01 = 17;
        gCurTask->unk88->unk22 = 0;
        gCurTask->unk88->unk1E = gCurTask->unk88->unk20 = 0;
        break;
    case 2:
        if (gCurTask->unk88->unk37 != 2)
        {
            gCurTask->unk88->unk01 = 16;
            gCurTask->unk88->unk16 = 255;
        }
        else
        {
            gCurTask->unk88->unk01 = 58;
            gCurTask->unk73 = 3;
        }
        gCurTask->unk88->unk22 = 0;
        gCurTask->unk88->unk1E = gCurTask->unk88->unk20 = 0;
        break;
    }
    gCurTask->unk7C = 0;
    if (gCurTask->unk88->unk01 != 0)
    {
        if (gCurTask->unk88->unk40 & 1)
        {
            gCurTask->unk88->unk28 = 0;
            gCurTask->unk88->unk29 = 1;
            gCurTask->unk88->unk2B = 0;
            gCurTask->unk88->unk24 = gCurTask->unk88->unk26 = 0;
            TaskSetSkipMask(0, gCurTaskIdx);
        }
        if (gCurTask->unk88->unk2C != -1)
            PlayerStopSfx();
        gCurTask->unk76 = 0;
        gCurTask->unk88->unk40 = 0;
        gCurTask->unk88->unk50 = 0;
        gCurTask->unk80 = 0;
    }
    else if (gUnk_03001F30 == 0)
    {
        if (gCurTask->unk76 & 2)
        {
            gCurTask->unk88->unk40 |= 2;
            gCurTask->unk73 = 1;
            gCurTask->unk88->unk01 = 8;
            gCurTask->unk76 &= 0xFFFD;
        }
        if (!(gCurTask->unk88->unk40 & 128))
            sub_0803ce98();
    }
    if (gCurTask->unk88->unk01 != 0)
        TaskSetEntry(PlayerStartRequestedAction, gCurTaskIdx);
    if (gUnk_03001F30 == 0)
    {
        if (gCurTask->unk88->unk40 & 4)
        {
            if (gCurTask->unk88->unk0D == 0)
                sub_0803c9b4(0);
            else
                sub_0803c9b4(1);
        }
        if ((s8)gCurTask->unk88->unk36 != 0)
        {
            switch (gCurTask->unk88->unk03)
            {
            case 32:
            case 33:
                CreatePlayerEffect(gCurTask->unk88->unk00, 15, 0);
                sub_08049a58();
                break;
            }
            gCurTask->unk88->unk36 = 0;
        }
    }
    t = gCurTask;
    if (t->unk88->unk37 == 2)
    {
        if (t->unk4A + gSpriteCameraY > 900)
            t->unk50 = (t->unk4A = 900 - gSpriteCameraY) << 16;
    }
    t = gCurTask;
    t->unk48 = (t->unk4C + ((s16)t->unk88->unk24 & 0x8000 ? ((s16)t->unk88->unk24 << 8) | 0xFF000000 : (s16)t->unk88->unk24 << 8)) >> 16;
    t = gCurTask;
    t->unk4A = (t->unk50 + ((s16)t->unk88->unk26 & 0x8000 ? ((s16)t->unk88->unk26 << 8) | 0xFF000000 : (s16)t->unk88->unk26 << 8)) >> 16;
}
