#include "gba/gba.h"
#include "global.h"
#include "task.h"

struct M11R8 { u8 unk00; u8 unk01; u8 unk02; u8 unk03; u8 *unk04; };
struct M11Buf { u8 unk00[4]; u8 unk04[4]; };
struct M11R20 { u32 w[5]; };

extern u8 gUnk_02000020;
extern u16 gUnk_02000028;
extern u16 gUnk_02004B50[];
extern struct M11R8 gPlayerHitBoxSets[];
extern u8 gUnk_02005574[];
extern s16 gUnk_02005588[];
extern u8 gUnk_020055C4[];
extern s16 gRoomEntryX;
extern struct M11R20 gPlayerBodyBoxes[];
extern struct M11Buf gUnk_02006A80[];
extern u8 gUnk_02007CF0;
extern u16 gUnk_02007F60[];
extern s16 gRoomEntryY;
extern u16 gUnk_0200AF18[];
extern u16 gPlayerBubbleTimers[];
extern vs16 gCurrentBgm;
extern u16 gPlayerHeldKeys[];
extern u8 gObjPalette[];
extern u32 gUnk_03001490[];
extern vu16 gFrameCount;
extern vu16 gPlayerPressedKeys[];
extern u32 gUnk_03001F10;
extern s32 gUnk_03001F2C;
extern u8 gUnk_03001F30;
extern u8 gUnk_03001F34;
extern u32 gUnk_03002160;
extern struct PlayerState gPlayerStates[];
extern u8 gActivePlayerMask;
extern s32 gUnk_03002344;
extern s16 gSpriteCameraX;
extern u8 gActivePlayerCount;
extern u16 gLocalPlayer;
extern u16 gPlayerCount;
extern u8 gUnk_030023B0;
extern u16 gLatchedPressedKeys[];
extern s16 gSpriteCameraY;
extern s8 gUnk_03002444;
extern u32 gUnk_03002448;
extern s16 gUnk_0300244C;
extern u16 gLatchedHeldKeys[];
extern struct Task *gCurTask;
extern struct Task gTasks[];
extern vs16 gTaskSlotTypes[];
extern u8 gTerrainResult[];
extern u16 gUnk_080DC628[];
extern u32 gUnk_080DC728[];
extern u8 gUnk_080DCA28[];
extern u32 gUnk_080DCC28[];
extern u32 gUnk_080DCC48[];
extern u8 gUnk_081BC050[];
extern u8 gUnk_081BE6BC[];
extern u16 gUnk_08226254[];
extern u16 gUnk_0873AF0C[][2];
extern s8 gUnk_0873AF20[][2];
extern u8 gUnk_0873AF30[][2];
extern u8 gUnk_0873AF3A[][2];
extern u8 gUnk_0873AF42[];
extern u32 gPlayerDefaultTerrainBox[];
extern u8 gUnk_0873D0C4[];
extern s16 gUnk_0873D210[];
extern s16 gUnk_0873D2E0[];
extern u16 gUnk_0873D79E[];
extern u16 gUnk_0873DB44[][2];
extern u32 gUnk_08751990[];
extern u32 gUnk_087519CC[];

void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
s32 QueueSprite(u32 a, u32 b, u32 c, u32 d, u32 e, s16 f);
u32 RandomRange(u32 range);
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 PlaySfx(s32 id);
void StopSfxOnPlayer(s32 player, s32 songId);
void TaskFree(s32 id);
s32 TaskCreateFrom(u32 type, s32 idx);
void TaskSetFrame(s32 a);
s32 IsOnScreen(s16 x, s16 y);
u32 IsWorldPosOnScreen(s16 a, s16 b);
void sub_0800652c(u16 a, s32 b);
void TaskRestoreSkipMask(u32 idx);
s32 sub_08009ee8(s32 a, s32 b);
void sub_08009fcc(s32 a, s32 b, s32 c);
void sub_0800a008(u32 a, s32 b, s32 c);
s32 sub_08022760(struct Task *t);
s32 sub_08024e40(s32 a0, s32 a1);
void sub_080270d0(void);
void PauseRoom(void);
void ResumeRoom(void);
void sub_08033414(void);
s32 sub_0803d010(void);
void sub_0803d7c4(void);
s32 sub_0803d870(void);
void sub_0803db74(void);
void sub_0803e28c(s32 a0);
s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1);
s32 sub_0803e7d8(void);
void sub_0803e8ec(void);
void PlayerStopAtCeilingAndWall(void);
s32 PlayerStopAtWall(void);
s32 PlayerCheckLanding(void);
s32 PlayerHasCrossedWaterSurface(s32 a);
void sub_080409b8(s32 a0);
void sub_08040a44(s16 p0, s16 p1);
void PlayerSetMotionXPreset(s32 a0, s32 a1);
void PlayerSetMotionYPreset(s32 a0);
s32 CreatePlayerEffect(s32 band, s32 id, s32 payload);
s32 CreatePlayerEffectHighSlot(s32 band, s32 id, s32 payload);
void sub_08064eb8(u32 a);

void PlayerPlayBump(void)
{
    u8 v;
    s16 *p;

    v = gCurTask->unk88->unk3E;
    if (v == 0)
        return;
    gCurTask->unk88->unk3E = 0;
    if (gUnk_03001F30 == 0)
        p = &gUnk_0873D210[gCurTask->unk88->unk0D * 4];
    else
        p = gUnk_0873D2E0;
    CreatePlayerEffect(gCurTask->unk88->unk00, 4, 0);
    if (gCurTask->unk88->unk06 == 1) {
        if (v == 2) {
            TaskSetFrame(0x15F);
            TaskYieldTrampoline(2);
        }
    } else {
        switch (v) {
        case 1:
            PlaySfxIfLocalPlayer(107, (u16)gCurTask->unk88->unk00);
            TaskSetFrame(p[0]);
            TaskYieldTrampoline(4);
            break;
        case 2:
            TaskSetFrame(p[3]);
            TaskYieldTrampoline(6);
            break;
        case 3:
        case 5:
            PlaySfxIfLocalPlayer(107, (u16)gCurTask->unk88->unk00);
            TaskSetFrame(p[1]);
            TaskYieldTrampoline(4);
            break;
        case 4:
        case 6:
            PlaySfxIfLocalPlayer(107, (u16)gCurTask->unk88->unk00);
            TaskSetFrame(p[2]);
            TaskYieldTrampoline(4);
            break;
        }
    }
}

void sub_0803ce98(void)
{
    struct PlayerState *ps;
    struct PlayerState *ps2;
    s32 v, c;

    ps = gCurTask->unk88;
    if (ps->unk04 != 0 && ps->unk04 != 6)
        return;
    if (ps->unk35 != 0) {
        ps->unk35--;
    } else {
        if (gUnk_0873AF20[ps->unk34][0] == 0)
            ps->unk34 = 0;
        gCurTask->unk88->unk35 = gUnk_0873AF20[gCurTask->unk88->unk34][0];
        gCurTask->unk88->unk32 = gUnk_0873AF20[gCurTask->unk88->unk34++][1];
    }
    ps2 = gCurTask->unk88;
    if (ps2->unk32 == 0)
        return;
    c = (ps2->unk04 != 0);
    v = gUnk_0873AF0C[ps2->unk33][c];
    if (ps2->unk06 == 1)
        v += 10;
    if (gLocalPlayer != ps2->unk00 || gUnk_03002444 != 0) {
        if (IsWorldPosOnScreen(gCurTask->unk48, gCurTask->unk4A) == 0)
            return;
    }
    if (sub_0803d010() != 0)
        return;
    QueueSprite(gCurTask->unk42, gUnk_08751990[v],
                 (u16)(gCurTask->unk3E & 0x8000),
                 gCurTask->unk40 & 0xF000,
                 gCurTask->unk48 - gSpriteCameraX,
                 gCurTask->unk4A - gSpriteCameraY);
}

/* True while the running task's unk3C (state id) is one of 1511..1515. */
s32 sub_0803d010(void)
{
    if ((u16)(gCurTask->unk3C - 1511) <= 4)
        return 1;
    return 0;
}

void CreatePlayer(s32 a0)
{
    struct Task *t;

    t = &gTasks[TaskCreateFrom(5, 0)];
    t->unk4C = gRoomEntryX << 16;
    t->unk50 = gRoomEntryY << 16;
    t->unk48 = t->unk4C >> 16;
    t->unk4A = t->unk50 >> 16;
    if ((u8)(gUnk_02000020 - 2) <= 1)
        sub_08009fcc(25, -1, a0);
}

/* Reset player record a0 to its start-of-stage state. */
void InitPlayerState(s32 a0)
{
    struct PlayerState *p;

    p = &gPlayerStates[a0];
    p->unk00 = a0;
    p->unk03 = 0;
    p->unk02 = 0;
    p->unk01 = 0;
    p->unk05 = 255;
    p->unk04 = -1;
    p->unk06 = 0;
    p->unk08 = 0;
    p->unk07 = 0;
    p->unk09 = 0;
    p->unk0B = 0;
    p->unk0A = 0;
    p->unk0C = -1;
    p->unk0D = gUnk_02004B50[a0];
    p->unk0E = gUnk_0200AF18[a0];
    p->unk10 = 0;
    p->unk0F = 0;
    p->unk12 = 0;
    p->unk14 = 0;
    p->unk17 = 0;
    p->unk18 = 0;
    p->unk1C = 0;
    p->unk1A = 0;
    p->unk22 = 0;
    p->unk20 = 0;
    p->unk1E = 0;
    p->unk26 = 0;
    p->unk24 = 0;
    p->unk2B = 0;
    p->unk29 = 0;
    p->unk28 = 0;
    p->unk2C = 0xFFFF;
    p->unk2E = 0;
    p->unk30 = 0;
    p->unk31 = 0;
    p->unk36 = 0;
    p->unk37 = 0;
    p->unk38 = 0;
    p->unk3B = 0;
    p->unk3A = 0;
    p->unk3C = 0;
    p->unk48 = 0;
    p->unk3D = 0;
    p->unk3E = 0;
    p->unk3F = 0;
    p->unk40 = 0;
    p->unk42 = 0;
    p->unk44 = 0;
    p->unk45 = 0;
    p->unk50 = 0;
    p->unk46 = 0;
    p->unk47 = 1;
    p->unk50 = 0;
    p->unk4C = 0;
    p->unk4A = 0;
    p->unk4B = 0;
    p->unk49 = 0;
    p->unk4E = -1;
    p->unk58 = 0;
    p->unk54 = 0;
    p->unk60 = 0;
    p->unk5E = 0;
    p->unk64 = 0;
    p->unk68 = 0;
    p->unk6C = 0;
    p->unk70 = gPlayerDefaultTerrainBox;
    gPlayerBubbleTimers[a0] = 45;
}

/* Reset player record a0 to its start-of-stage state. */
void sub_0803d1c4(s32 a0)
{
    struct PlayerState *p;

    p = &gPlayerStates[a0];
    p->unk00 = a0;
    p->unk03 = 0;
    p->unk02 = 0;
    p->unk01 = 0;
    p->unk05 = 255;
    p->unk04 = -1;
    p->unk06 = 0;
    p->unk08 = 0;
    p->unk07 = 0;
    p->unk09 = 0;
    p->unk0B = 0;
    p->unk0A = 0;
    p->unk0C = -1;
    p->unk0D = gUnk_02004B50[a0];
    p->unk0E = gUnk_0200AF18[a0];
    p->unk10 = 0;
    p->unk0F = 0;
    p->unk12 = 0;
    p->unk14 = 0;
    p->unk22 = 0;
    p->unk20 = 0;
    p->unk1E = 0;
    p->unk26 = 0;
    p->unk24 = 0;
    p->unk2B = 0;
    p->unk29 = 0;
    p->unk28 = 0;
    p->unk2C = 0xFFFF;
    p->unk2E = 0;
    p->unk30 = 0;
    p->unk31 = 0;
    p->unk36 = 0;
    p->unk38 = 0;
    p->unk3B = 0;
    p->unk3A = 0;
    p->unk3C = 0;
    p->unk48 = 0;
    p->unk3D = 0;
    p->unk3E = 0;
    p->unk3F = 0;
    p->unk40 = 0;
    p->unk42 = 0;
    p->unk44 = 0;
    p->unk45 = 0;
    p->unk50 = 0;
    p->unk46 = 0;
    p->unk47 = 1;
    p->unk50 = 0;
    p->unk4C = 0;
    p->unk4A = 0;
    p->unk4B = 0;
    p->unk49 = 0;
    p->unk4E = -1;
    p->unk58 = 0;
    p->unk54 = 0;
    p->unk64 = 0;
    p->unk68 = 0;
    p->unk6C = 0;
    p->unk70 = gPlayerDefaultTerrainBox;
    gPlayerBubbleTimers[a0] = 45;
}

/* Reset player record a0 to its start-of-stage state. */
void sub_0803d2d4(s32 a0)
{
    struct PlayerState *p;

    p = &gPlayerStates[a0];
    p->unk00 = a0;
    p->unk03 = 0;
    p->unk02 = 0;
    p->unk01 = 0;
    p->unk05 = 255;
    p->unk04 = -1;
    p->unk0D = gUnk_02004B50[a0];
    p->unk0E = gUnk_0200AF18[a0];
    p->unk10 = 0;
    p->unk0F = 0;
    p->unk12 = 0;
    p->unk14 = 0;
    p->unk17 = 0;
    p->unk18 = 0;
    p->unk1C = 0;
    p->unk1A = 0;
    p->unk22 = 0;
    p->unk20 = 0;
    p->unk1E = 0;
    p->unk26 = 0;
    p->unk24 = 0;
    p->unk2B = 0;
    p->unk29 = 0;
    p->unk28 = 0;
    p->unk2C = 0xFFFF;
    p->unk2E = 0;
    p->unk36 = 0;
    p->unk38 = 0;
    p->unk3B = 0;
    p->unk3A = 0;
    p->unk48 = 0;
    p->unk3D = 0;
    p->unk3E = 0;
    p->unk3F = 0;
    p->unk40 = 0;
    p->unk42 = 0;
    p->unk44 = 0;
    p->unk45 = 0;
    p->unk50 = 0;
    p->unk46 = 0;
    p->unk47 = 1;
    p->unk50 = 0;
    p->unk4C = 0;
    p->unk4A = 0;
    p->unk4B = 0;
    p->unk49 = 0;
    p->unk4E = -1;
    p->unk58 = 0;
    p->unk54 = 0;
    p->unk64 = 0;
    p->unk68 = 0;
    p->unk6C = 0;
    p->unk70 = gPlayerDefaultTerrainBox;
    gPlayerBubbleTimers[a0] = 45;
}

/* Set the camera/scroll target (a0 = dx, a1 = dy, a2 = limit). */
void sub_0803d3d4(s32 a0, s32 a1, s32 a2)
{
    s8 sign;
    s32 ax, ay;

    gUnk_03001F2C = a0;
    gUnk_03002448 = a1;
    gUnk_03002344 = a2;
    if (a2 == 0x80000000)
        return;
    if (a1 != 0) {
        sign = (a1 >= 0);
    } else if (a0 != 0) {
        sign = (a0 >= 0);
    } else {
        if (a2 != 0)
            gUnk_03002344 = a1;
        return;
    }
    if (sign) {
        if (a0 < 0)
            return;
    } else {
        if (a0 > 0)
            return;
    }
    gUnk_03002160 = ax = (a0 < 0) ? -a0 : a0;
    gUnk_03001F10 = ay = (a1 < 0) ? -a1 : a1;
    if (ax != a2) {
        if (ax > a2) {
            if (ax - ay < a2) {
                if (sign)
                    gUnk_03001F2C = a2;
                else
                    gUnk_03001F2C = -a2;
            } else {
                if (a1 != 0) {
                    gUnk_03002448 = -a1;
                    return;
                }
                if (sign)
                    gUnk_03001F2C = a2;
                else
                    gUnk_03001F2C = -a2;
                return;
            }
        } else {
            if (ax + ay <= a2)
                return;
            if (sign)
                gUnk_03001F2C = a2;
            else
                gUnk_03001F2C = -a2;
        }
    }
    gUnk_03002448 = 0;
}

void PlayerMove(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    sub_0803d3d4(t->unk54, t->unk5C, t->unk64);
    t = gCurTask;
    t->unk54 = gUnk_03001F2C;
    t->unk64 = gUnk_03002344;
    t->unk54 += gUnk_03002448;
    sub_0803d3d4(t->unk58, t->unk60, t->unk68);
    t = gCurTask;
    t->unk58 = gUnk_03001F2C;
    t->unk68 = gUnk_03002344;
    t->unk58 += gUnk_03002448;
    if ((t->unk88->unk42 & 0x100) == 0) {
        t->unk4C += t->unk88->unk54;
        if ((t->unk7A & 1) == 0) {
            t->unk50 += t->unk88->unk58;
            if (t->unk88->unk58 < 0)
                t->unk7A = 0;
        }
    }
    u = gCurTask;
    u->unk4C += u->unk54;
    u->unk50 += u->unk58;
    u->unk48 = u->unk4C >> 16;
    u->unk4A = u->unk50 >> 16;
}

/* Upload the frame's graphics for gfx script entry unk3C+a0 and return its
   OAM/anim word (bit 0 is the "extended record" tag). */
s32 PlayerLoadFrameTilesAndPalette(s32 a0)
{
    struct Task *t;
    struct TaskGfx *g;
    u16 **p;
    u16 *s;
    u16 *q;
    u32 dst;
    u32 prio;
    u32 *tbl;

    t = gCurTask;
    prio = t->unk40;
    tbl = t->unk38;
    g = (struct TaskGfx *)tbl[t->unk3C + a0];
    if ((g->unk00 & 1) != 0) {
        p = &g->unk04;
        if ((t->unk88->unk42 & 16) == 0 && g->unk04 != NULL)
            RequestCopy(2, (u32)(g->unk04 + 1),
                         (u32)gObjPalette + ((prio >> 12) << 5), *g->unk04);
        p++;
        s = *p;
        dst = ((prio & 0x7FF) << 5) + 0x06010000;
        if (*s != 0xFFFF) {
            do {
                q = s + 1;
                RequestCopy(3, (u32)q, dst, *s);
                s = (u16 *)((u8 *)q + *s);
                dst += 0x400;
            } while (*s != 0xFFFF);
        }
        p++;
        if ((gCurTask->unk88->unk42 & 16) == 0 && *p != NULL)
            RequestCopy(2, (u32)(*p + 1),
                         (u32)gUnk_03001490 + ((prio >> 12) << 5), **p);
        q = p[1];
        if (q != NULL) {
            s = q;
            dst = ((prio & 0x7FF) << 5) + 0x06010800;
            if (*s != 0xFFFF) {
                do {
                    q = s + 1;
                    RequestCopy(3, (u32)q, dst, *s);
                    s = (u16 *)((u8 *)q + *s);
                    dst += 0x400;
                } while (*s != 0xFFFF);
            }
        }
    } else {
        if ((t->unk88->unk42 & 16) == 0 && *g->unk04 != 0)
            RequestCopy(2, (u32)(g->unk04 + 1),
                         (u32)gObjPalette + ((prio >> 12) << 5), *g->unk04);
        s = g->unk08;
        dst = ((prio & 0x7FF) << 5) + 0x06010000;
        if (*s != 0xFFFF) {
            do {
                q = s + 1;
                RequestCopy(3, (u32)q, dst, *s);
                s = (u16 *)((u8 *)q + *s);
                dst += 0x400;
            } while (*s != 0xFFFF);
        }
    }
    return g->unk00 & ~1;
}

void sub_0803d710(void)
{
    struct Task *t;
    struct TaskGfx *g;
    u32 *tbl;
    u16 **p;
    u16 *w;

    t = gCurTask;
    tbl = t->unk38;
    p = (u16 **)tbl[t->unk3C];
    g = (struct TaskGfx *)p;
    p++;
    w = g->unk04;
    if (((u32)w & 1) != 0) {
        RequestCopy(2, (u32)(p[0] + 1),
                     (u32)gObjPalette + ((t->unk40 & 0xF000) >> 7), *p[0]);
        if (p[2] != NULL)
            RequestCopy(2, (u32)(p[2] + 1),
                         (u32)gObjPalette + 32
                             + ((gCurTask->unk40 & 0xF000) >> 7), *p[2]);
    } else {
        RequestCopy(2, (u32)(w + 1),
                     (u32)gObjPalette + ((t->unk40 & 0xF000) >> 7), *w);
    }
    if (gUnk_03001F30 == 0) {
        if (gPlayerCount > 1)
            sub_0803d7c4();
        sub_0803db74();
    }
}

void sub_0803d7c4(void)
{
    s32 idx;

    if (gCurTask->unk88->unk00 == 0)
        return;
    idx = sub_0803d870();
    if (idx == -1)
        return;
    RequestCopy(2, (idx << 1) + (u32)gUnk_080DC628
                    + (gCurTask->unk88->unk00 << 5),
                 (u32)gObjPalette + ((gCurTask->unk40 >> 12) << 5), 32);
}

void sub_0803d824(void)
{
    s32 idx;

    idx = sub_0803d870();
    if (idx == -1)
        return;
    RequestCopy(2, (idx << 1) + (u32)gUnk_080DC628 + (gUnk_02000028 << 5),
                 (u32)gObjPalette + ((gCurTask->unk40 >> 12) << 5), 32);
}

/* Pick the HUD/status graphics slot for the running task's state, or -1 for
   "nothing to upload". */
s32 sub_0803d870(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *x;
    struct Task *y;
    struct PlayerState *ps;
    struct PlayerState *q;
    u16 v;
    u16 w;

    t = gCurTask;
    v = t->unk3C;
    if ((v >= 273 && v <= 284) || (v >= 389 && v <= 403))
        return 192;
    if (v >= 252 && v <= 272)
        return -1;
    if (v >= 372 && v <= 388)
        return -1;
    if (v >= 4551 && v <= 4554)
        return 64;
    ps = t->unk88;
    if (ps->unk0D == 12 || ps->unk0D == 13) {
        if ((t->unk7B & 1) == 0)
            return -1;
        if ((ps->unk40 & 0x100) != 0)
            return -1;
        if (ps->unk06 == 2)
            return -1;
        if (v >= 173 && v <= 242)
            return 0;
        if (v >= 285 && v <= 293)
            return 0;
        return -1;
    }
    if ((ps->unk42 & 16) == 0) {
        switch (ps->unk0D) {
        case 9:
            return 320;
        case 18:
            return 640;
        case 22:
            return 448;
        case 23:
            return 512;
        case 17:
            u = gCurTask;
            ps = u->unk88;
            if (ps->unk04 == 13)
                goto second;
            if ((u->unk7B & 1) == 0)
                return -1;
            if ((ps->unk40 & 0x100) != 0)
                return -1;
            if (ps->unk06 == 2)
                return -1;
            w = u->unk3C;
            if (w >= 173 && w <= 242)
                return 0;
            if (w >= 285 && w <= 293)
                return 0;
            return -1;
        }
    }
    q = gCurTask->unk88;
    if (q->unk04 == 13) {
    second:
        switch (gCurTask->unk88->unk0D) {
        case 5:
            x = gCurTask;
            w = x->unk3C;
            if (w >= 1487 && w <= 1504)
                return -1;
            break;
        case 19:
            x = gCurTask;
            w = x->unk3C;
            if (w >= 3549 && w <= 3556)
                return -1;
            break;
        case 8:
            x = gCurTask;
            w = x->unk3C;
            if (w >= 1794 && w <= 1810)
                return 256;
            break;
        case 14:
            if ((gCurTask->unk88->unk42 & 16) != 0)
                return -1;
            return 704;
        case 17:
            x = gCurTask;
            w = x->unk3C;
            if (w >= 3144 && w <= 3148)
                return 384;
            return -1;
        case 24:
            y = gCurTask;
            if ((y->unk88->unk42 & 16) != 0)
                return -1;
            w = y->unk3C;
            if (w >= 3976 && w <= 4068)
                return 576;
            break;
        }
    } else {
        if (q->unk0D == 24) {
            if ((q->unk42 & 16) != 0)
                goto ret_m1;
            return 576;
        }
    }
    if ((gCurTask->unk88->unk42 & 16) != 0)
        goto ret_m1;
    return 0;
ret_m1:
    return -1;
}

void sub_0803db74(void)
{
    struct Task *t;
    struct Task *u3;
    struct Task *u;
    struct PlayerState *ps;
    struct PlayerState *ps2;

    t = gCurTask;
    ps = t->unk88;
    if ((u8)(ps->unk37 - 2) <= 1) {
        switch (ps->unk3C++) {
        case 0:
        case 1:
            u = gCurTask;
            if ((u->unk88->unk42 & 16) == 0)
                RequestCopy(2, (u32)gUnk_08226254,
                             (u32)gObjPalette + (((u->unk40 >> 12) + 1) << 5),
                             32);
            break;
        case 2:
            u = gCurTask;
            if ((u->unk88->unk42 & 16) == 0)
                BlendColors(gUnk_08226254, gUnk_08226254 - 16, 128, 16,
                             (u16 *)((u32)gObjPalette
                                     + (((u->unk40 >> 12) + 1) << 5)));
            break;
        case 3:
            u3 = gCurTask;
            if ((u3->unk88->unk42 & 16) == 0)
                BlendColors(gUnk_08226254, gUnk_08226254 - 16, 256, 16,
                             (u16 *)((u32)gObjPalette
                                     + (((u3->unk40 >> 12) + 1) << 5)));
            break;
        case 9:
            gCurTask->unk88->unk3C = 0;
            break;
        }
        return;
    }
    if ((u16)(t->unk3C - 655) <= 218 || (u16)(t->unk3C - 4369) <= 23) {
        if ((ps->unk42 & 16) == 0)
            RequestCopy(2, (u32)gUnk_081BC050,
                         (u32)gObjPalette + (((t->unk40 >> 12) + 1) << 5), 32);
        return;
    }
    if ((u16)(t->unk3C - 874) <= 8) {
        if ((ps->unk42 & 16) == 0)
            RequestCopy(2, (u32)&gUnk_081BE6BC[ps->unk00 * 128],
                         (u32)gObjPalette + ((t->unk40 >> 12) << 5), 32);
    }
    u = gCurTask;
    if ((u16)(u->unk3C - 3821) <= 128 || (u16)(u->unk3C - 4525) <= 5) {
        ps2 = u->unk88;
        if ((ps2->unk42 & 16) == 0) {
            if (gUnk_030023B0 == 0)
                RequestCopy(2, (u32)&gUnk_080DCA28[ps2->unk00 * 32],
                             (u32)gObjPalette + ((u->unk40 >> 12) << 5), 32);
            else
                RequestCopy(2, (u32)&gUnk_080DCA28[gUnk_02000028 * 32],
                             (u32)gObjPalette + ((u->unk40 >> 12) << 5), 32);
        }
    }
}

/* BLOCKED ON fwd.h: byte-exact (520/520, MATCH) as soon as fwd.h line 79
   reads `void sub_08040a44(s16 a0, s16 a1);` instead of
   `void sub_08040a44(u16 a0, u16 a1);`.  With the u16 prototype gcc folds
   `(u16)(s16)x` back to `x`, so the a44 call is fed the RAW x/y, x/y stay
   live past the sign-extends, and the sign-extended values need two extra
   callee-saved registers (26 differing bytes, size exact).  With s16 the
   `(s16)x` CSE reaches the a44 call, x/y die at the sign-extend and the
   allocator coalesces them into r4/r5 exactly as the ROM does.
   NB sub_08040a44's own body zero-extends r0/r1 (0x08040A4C
   `lsls r0,r0,#16; lsrs r5,r0,#16`), so its DEFINITION takes u16 params:
   this call site was compiled without a narrowing prototype in scope.
   Also needs `void sub_08033414(void);` in hdr.c (already present). */
void sub_0803ddc0(void)
{
    s32 speed;
    u16 x;
    u16 y;
    s32 pal;

    if (gCurTask->unk08 != 0 && (gCurTask->unk13 & 8) == 0)
        sub_08033414();
    if ((gLocalPlayer != gCurTask->unk88->unk00
         || gUnk_03002444 != 0)
     && gCurTask->unk88->unk37 != 2
     && IsWorldPosOnScreen(gCurTask->unk48, gCurTask->unk4A) == 0)
    {
        if (gCurTask->unk38 == 0)
            return;
        if (gCurTask->unk3C == -1)
            return;
        sub_0803d710();
        return;
    }
    if (gCurTask->unk38 == 0)
        return;
    if (gCurTask->unk3C == -1)
        return;
    speed = 0;
    if ((gCurTask->unk88->unk37 == 2
      || gCurTask->unk88->unk37 == 3)
     && gCurTask->unk3C > 0xFEC && (gFrameCount & 3) == 0)
        speed = 131;
    if (gCurTask->unk88->unk37 == 2)
    {
        x = gCurTask->unk48;
        y = gCurTask->unk4A;
    }
    else
    {
        x = gCurTask->unk48 - gSpriteCameraX;
        y = gCurTask->unk4A - gSpriteCameraY;
    }
    pal = PlayerLoadFrameTilesAndPalette(speed);
    if (gUnk_03001F30 == 0)
    {
        if (gUnk_030023B0 == 0)
        {
            if (gPlayerCount > 1)
                sub_0803d7c4();
        }
        else
        {
            sub_0803d824();
        }
        sub_0803db74();
    }
    if (IsOnScreen((s16)x, (s16)y) == 0)
        return;
    sub_08040a44((s16)x, (s16)y);
    QueueSprite(gCurTask->unk42, pal, gCurTask->unk3E,
                 2048 | gCurTask->unk40, (s16)x, (s16)y);
    if ((gCurTask->unk88->unk40 & 128) == 0)
        return;
    QueueSprite(gCurTask->unk42, pal, gCurTask->unk3E,
                 2048 | gCurTask->unk40, (s16)x - 48, (s16)y);
    QueueSprite(gCurTask->unk42, pal, gCurTask->unk3E,
                 2048 | gCurTask->unk40, (s16)x + 48, (s16)y);
}

void sub_0803dfc8(void)
{
    if (gCurTask->unk38 == 0)
        return;
    if (gCurTask->unk3C == -1)
        return;
    if (IsWorldPosOnScreen(gCurTask->unk48, gCurTask->unk4A) == 0)
        return;
    QueueSprite(gCurTask->unk42, PlayerLoadFrameTilesAndPalette(0), gCurTask->unk3E,
                 gCurTask->unk40,
                 gCurTask->unk48 - gSpriteCameraX,
                 gCurTask->unk4A - gSpriteCameraY);
}

void PlayerStopAxes(s32 a0)
{
    if (a0 & 1)
    {
        gCurTask->unk64 = 0;
        gCurTask->unk5C = 0;
        gCurTask->unk54 = 0;
    }
    if (a0 & 2)
    {
        gCurTask->unk68 = 0;
        gCurTask->unk60 = 0;
        gCurTask->unk58 = 0;
    }
}

void sub_0803e080(void)
{
    struct Task *t = gCurTask;
    struct PlayerState *p = t->unk88;

    switch ((s8)p->unk22)
    {
    case 1:
        p->unk42 &= ~0x10;
        {
            struct PlayerState *q = t->unk88;

            if ((s16)q->unk12 == 0)
            {
                struct PlayerState *r;

                q->unk22 = 0;
                r = gCurTask->unk88;
                r->unk20 = 0;
                r->unk1E = 0;
                break;
            }
            if ((gFrameCount & 7) > 3)
                break;
            RequestCopy(2, (u32)&gUnk_080DC728[q->unk00 * 8],
                (u32)&gObjPalette[(t->unk40 >> 12) << 5], 32);
        }
        gCurTask->unk88->unk42 |= 16;
        break;
    case 2:
        p->unk42 &= ~0x10;
        {
            struct PlayerState *q = t->unk88;
            u32 src;

            if (q->unk17 != 0)
                break;
            if (q->unk0D != 7 && q->unk0D != 20 && q->unk0D != 21)
            {
                q->unk22 = 0;
                return;
            }
            if ((gFrameCount & 15) == 9)
            {
                src = (u32)gUnk_080DCC48;
            }
            else if ((gFrameCount & 15) == 10 || (gFrameCount & 15) == 11)
            {
                src = (u32)gUnk_080DCC28;
            }
            else
            {
                break;
            }
            RequestCopy(2, src,
                (u32)&gObjPalette[(gCurTask->unk40 >> 12) << 5], 24);
        }
        gCurTask->unk88->unk42 |= 16;
        break;
    }
    sub_0803e8ec();
}

/* CENSUS: this is ONE function, 0x0803E1B8-0x0803E28C (212 bytes).  The
   7-entry jump table at 0x0803E1E0 runs to 0x0803E1F8, so bounds.txt's
   `sub_0803e1f4` is the table's last two words plus this body's case arms -
   0x0803E1F4 carries `rom-pointer` evidence in symbols.csv, not a prologue. */
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2)
{
    struct PlayerState *p = &gPlayerStates[a2];

    switch (a0)
    {
    default:
        sub_0803e28c((s32)p);
        p->unk3F = 0;
        p->unk12 = 0;
        p->unk22 = 0;
        break;
    case 0:
        sub_0803e28c((s32)p);
        p->unk12 = a1;
        break;
    case 1:
        p->unk3F = 1;
        p->unk12 = a1;
        p->unk22 = 1;
        break;
    case 2:
        sub_0803e28c((s32)p);
        p->unk3F = 2;
        p->unk12 = 0x8000;
        p->unk22 = 0;
        break;
    case 3:
        sub_0803e28c((s32)p);
        p->unk3F = 3;
        p->unk12 = 0x8000;
        p->unk22 = 0;
        break;
    case 4:
        p->unk3F = 3;
        p->unk12 = a1;
        break;
    case 5:
        sub_0803e28c((s32)p);
        p->unk17 = 1;
        p->unk18 = 960;
        break;
    case 6:
        p->unk3F = 6;
        p->unk12 = 0x8000;
        p->unk22 = 3;
        break;
    }
}

void sub_0803e28c(s32 a0)
{
    if ((s8)((struct PlayerState *)a0)->unk22 == 1)
    {
        u16 mask = 16;
        struct Task *t = gCurTask;
        struct PlayerState *p;

        if (t->unk88->unk42 & 0x200)
            mask |= 0x200;
        t->unk88->unk42 &= ~mask;
        p = t->unk88;
        /* task.h has no fields at PlayerState+0x1E / +0x20 yet */
        ((u16 *)p)[16] = 0;
        ((u16 *)p)[15] = 0;
    }
}

void sub_0803e2d4(void)
{
    struct PlayerState *p = gCurTask->unk88;

    if ((p->unk42 & 32) == 0)
    {
        /* task.h has no field at PlayerState+0x12 yet */
        if (((s16 *)p)[9] != -32768)
        {
            if (((s16 *)p)[9] != 0)
                ((s16 *)p)[9]--;
            else if (p->unk3F != 0)
            {
                if (p->unk3F == 1 && (p->unk42 & 0x200))
                    p->unk42 &= ~0x200;
                gCurTask->unk88->unk3F = 0;
            }
        }
    }
    sub_0803e7d8();
}

s32 PlaySfxIfLocalPlayer(s32 a0, u16 a1)
{
    u16 songArea = a1;

    if (gLocalPlayer == songArea)
        return PlaySfx(a0);
    return -1;
}

void PlayerStartSfx(s32 a0, u16 a1)
{
    if (gLocalPlayer == a1)
    {
        gCurTask->unk88->unk2E = a0;
        gCurTask->unk88->unk2C = PlaySfx((s16)a0);
    }
}

void PlayerStopSfx(void)
{
    if (gCurTask->unk88->unk2C != -1)
    {
        StopSfxOnPlayer(gCurTask->unk88->unk2C, gCurTask->unk88->unk2E);
        gCurTask->unk88->unk2C = -1;
    }
}

void FreezeOtherTasks(s32 a0)
{
    sub_0800652c((u16)a0, gCurTaskIdx);
    if (a0 != 0)
    {
        TaskRestoreSkipMask(63);
        PauseRoom();
    }
    else
    {
        ResumeRoom();
    }
}

void sub_0803e414(void)
{
    if (gCurTask->unk43 == 1)
        gCurTask->unk3E &= 0x7FFF;
    else
        gCurTask->unk3E |= 0x8000;
}

s32 PlayerFaceHeldDirection(void)
{
    u16 *tbl = gLatchedHeldKeys;
    s32 i = (s8)gCurTask->unk88->unk00;

    if (tbl[i] & 48)
    {
        if (tbl[i] & 16)
        {
            if (gCurTask->unk43 == -1)
            {
                gCurTask->unk43 = 1;
                return 1;
            }
        }
        else
        {
            if (gCurTask->unk43 == 1)
            {
                gCurTask->unk43 = -1;
                return 1;
            }
        }
    }
    return 0;
}

void PlayerSetWaterMotionY(void)
{
    if (gCurTask->unk88->unk06 == 0)
    {
        if ((u32)gCurTask->unk58 > 0xE000)
            gCurTask->unk58 = 0xE000;
        PlayerSetMotionYPreset(13);
    }
    else
    {
        if ((u32)gCurTask->unk58 > 0x18000)
            gCurTask->unk58 = 0x18000;
        PlayerSetMotionYPreset(16);
    }
}

s32 PlayerLand(s32 a0)
{
    if (PlayerCheckLanding())
    {
        if ((gCurTask->unk7B & 1) == 0)
        {
            PlaySfxIfLocalPlayer(107, (u16)gCurTask->unk88->unk00);
            if ((gCurTask->unk58 & 0xFFFF0000) != 0 && a0 != 0)
                CreatePlayerEffect(gCurTask->unk88->unk00, 4, 0);
        }
        PlayerStopAxes(2);
        return 1;
    }
    return 0;
}

s32 sub_0803e55c(void)
{
    PlayerStopAtCeilingAndWall();
    if (PlayerHasCrossedWaterSurface(0) == 0)
    {
        if (gCurTask->unk7A & 1)
        {
            PlayerLand(1);
            PlayerSetMotionXPreset(0, 72);
        }
        else
        {
            PlayerSetMotionYPreset(2);
            PlayerSetMotionXPreset(11, 2);
        }
    }
    else
    {
        gCurTask->unk88->unk01 = 23;
    }
    return gCurTask->unk88->unk01;
}

s32 LoadPlayerBodyBoxRect(s32 playerIdx, u8 *src6)
{
    vu8 *src = (vu8 *)src6;

    if (src[0] == 128)
        return 0;
    {
        struct M11R20 *tbl = (struct M11R20 *)gPlayerBodyBoxes;
        u8 *dst = (u8 *)&tbl[playerIdx];

        dst[0] = src[0];
        dst[1] = src[1];
        dst[2] = src[2];
        dst[3] = src[3];
        dst[4] = src[4];
        dst[5] = src[5];
    }
    return 1;
}

/* NEEDS a byte-level view of the module's 8-byte rows in hdr.c (see report):
       struct M11R8  { u8 unk00; u8 unk01; u8 unk02; u8 unk03; u8 *unk04; };
       struct M11Buf { u8 unk00[4]; u8 unk04[4]; };
       extern struct M11Buf gUnk_02006A80[];
   gPlayerHitBoxSets keeps its `struct M11R8[]` spelling - only M11R8's members
   change, and an 8-byte struct still copies with ldmia/stmia whatever its
   members are.  Only an ARRAY-typed extern with a SCALAR member at +4 puts the
   field offset on the SYMBOL (`adds r2,#4; adds r2,r3,r2`); a `(T *)` cast or a
   pointer local folds it into the access and costs 4 bytes.
   Byte-matched (88 bytes at 0x0803E5F8) with those lines in place. */
s32 LoadPlayerHitBoxSet(s32 a0, s32 a1)
{
    vu8 *src = (vu8 *)a1;
    u8 *q;

    if (src[0] == 128)
        return 0;
    gPlayerHitBoxSets[a0].unk02 = src[0];
    gPlayerHitBoxSets[a0].unk03 = src[1];
    gPlayerHitBoxSets[a0].unk04 = gUnk_02006A80[a0].unk00;
    gUnk_02006A80[a0].unk00[0] = src[2];
    gUnk_02006A80[a0].unk00[1] = src[3];
    gUnk_02006A80[a0].unk00[2] = src[4];
    gUnk_02006A80[a0].unk00[3] = src[5];
    q = gUnk_02006A80[a0].unk04;
    q[0] = 127;
    q[1] = q[2] = q[3] = 0;
    return 1;
}

void PlayerStartOffsetScript(s32 a0)
{
    gCurTask->unk88->filler2A = a0;
    gCurTask->unk88->unk28 = 0;
    gCurTask->unk88->unk29 = 1;
    gCurTask->unk88->unk40 |= 1;
}

void sub_0803e68c(s32 a0)
{
    struct PlayerState *p = &gPlayerStates[a0];
    struct Task *t = &gTasks[a0];
    s32 v;

    if (gLocalPlayer == p->unk00)
        t->unk42 = 6;
    else
        t->unk42 = 7;
    if (t->unk7A != 0)
    {
        t->unk64 = 0;
        if (t->unk43 == 1)
            t->unk5C = 3584;
        else
            t->unk5C = -3584;
        t->unk68 = 0;
        t->unk60 = 0;
        t->unk58 = 0;
    }
    else
    {
        t->unk64 = 0;
        if (t->unk43 == 1)
            t->unk5C = 2048;
        else
            t->unk5C = -2048;
        t->unk60 = 9728;
        t->unk68 = 163840;
    }
    p->unk42 = 64;
    v = 0;
    if (p->unk06 == 1)
    {
        v = 14;
    }
    else if (p->unk04 == 14)
    {
        t->unk73 = 4;
        v = 9;
    }
    else if (p->unk04 == 10)
    {
        if ((s8)p->unk07 != 0 && (s8)p->unk07 == (s8)p->unk08)
        {
            p->unk08 = p->unk07;
            v = 14;
        }
        else
        {
            p->unk08 = 0;
            p->unk07 = 0;
            v = t->unk7A != 0 ? 1 : 7;
        }
    }
    else if (p->unk04 == 13)
    {
        if (t->unk7A != 0)
        {
            if (t->unk54 != 0)
            {
                s32 d = t->unk54;

                if (d < 0)
                    d = -d;
                if ((u32)d > 0x0001CBFF)
                {
                    p->unk3D = 1;
                    v = 3;
                }
                else
                {
                    p->unk3D = 0;
                    v = 2;
                }
            }
            else
            {
                PlayerStopAxes(1);
                v = 1;
            }
        }
        else
        {
            v = 7;
        }
    }
    p->unk01 = v;
}

s32 sub_0803e7d8(void)
{
    struct PlayerState *p = gCurTask->unk88;
    s32 i;

    if (p->unk42 & 32)
        return;
    if ((s16)p->unk18 == 0)
    {
        p->unk17 = 0;
        return;
    }
    p->unk18--;
    if (gPlayerCount == 1)
    {
        if ((s16)gCurTask->unk88->unk18 == 240)
            sub_080270d0();
        return;
    }
    if ((s16)gCurTask->unk88->unk18 != 240)
        return;
    for (i = 0; i < gPlayerCount; i++)
    {
        if (i != gCurTask->unk88->unk00
         && gPlayerStates[i].unk17 != 0
         && (s16)gPlayerStates[i].unk18 > 240)
            return;
    }
    sub_080270d0();
}

void sub_0803e868(void)
{
    s32 i;
    s32 ok;

    if ((s16)gCurTask->unk88->unk18 == 0)
        return;
    ok = 1;
    for (i = 0; i < gPlayerCount; i++)
    {
        if (i != gCurTask->unk88->unk00
         && gPlayerStates[i].unk17 != 0
         && (s16)gPlayerStates[i].unk18 > 240)
            ok = 0;
    }
    if (ok != 0 && gCurrentBgm == 19)
        sub_080270d0();
    gCurTask->unk88->unk17 = 0;
    gCurTask->unk88->unk18 = 0;
    {
        struct PlayerState *r = gCurTask->unk88;

        r->unk1C = 0;
        r->unk1A = 0;
    }
}

void sub_0803e8ec(void)
{
    struct PlayerState *p = gCurTask->unk88;
    s32 needBig;
    s32 needSmall;

    if (p->unk17 == 0)
        return;
    if (p->unk04 == 13)
    {
        if (p->unk0D == 2)
            return;
        if (p->unk0D == 20 && (p->unk42 & 16))
            return;
    }
    gCurTask->unk88->unk42 &= ~0x10;
    {
        struct PlayerState *q = gCurTask->unk88;

        if ((s16)q->unk18 == 0)
        {
            q->unk1C = 0;
            q->unk1A = 0;
            return;
        }
        needBig = 0;
        needSmall = 0;
        if (q->unk0D != 0)
            needBig = q->unk04 != 13;
        if (!(q->unk04 == 13 && q->unk0D == 2))
            needSmall = 1;
    }
    {
        struct Task *t = gCurTask;
        struct PlayerState *q = t->unk88;

        if ((s16)q->unk18 <= 239)
        {
            if (needSmall == 0)
                return;
            if ((gFrameCount & 7) > 3)
                return;
            RequestCopy(2, (u32)&gUnk_080DC728[q->unk00 * 8],
                (u32)&gObjPalette[(t->unk40 >> 12) << 5], 24);
            gCurTask->unk88->unk42 |= 16;
            return;
        }
        if (needSmall != 0 && (gFrameCount & 15) == 0)
            CreatePlayerEffectHighSlot(q->unk00, 4, 0);
    }
    {
        struct PlayerState *r = gCurTask->unk88;

        if ((s16)r->unk1A != 0)
        {
            r->unk1A--;
            gCurTask->unk88->unk42 |= 16;
            return;
        }
        switch ((s16)r->unk1C)
        {
        case 0:
            sub_0803d710();
            if (needSmall)
            {
                RequestCopy(2, (u32)gUnk_080DCC48,
                    (u32)&gObjPalette[(gCurTask->unk40 >> 12) << 5], 24);
                gCurTask->unk88->unk42 |= 16;
            }
            if (needBig)
            {
                RequestCopy(2, (u32)gUnk_080DCC48,
                    (u32)&gObjPalette[((gCurTask->unk40 >> 12) + 1) << 5], 32);
                gCurTask->unk88->unk42 |= 16;
            }
            gCurTask->unk88->unk1A = 1;
            gCurTask->unk88->unk1C++;
            break;
        case 1:
            sub_0803d710();
            if (needSmall)
            {
                RequestCopy(2, (u32)gUnk_080DCC28,
                    (u32)&gObjPalette[(gCurTask->unk40 >> 12) << 5], 24);
                gCurTask->unk88->unk42 |= 16;
            }
            gCurTask->unk88->unk1A = 2;
            gCurTask->unk88->unk1C++;
            break;
        case 2:
            r->unk1A = 4;
            gCurTask->unk88->unk1C = 0;
            break;
        }
    }
}

/* CENSUS: one function, 0x0803EAF8-0x0803F5FC (2820 bytes).  Returns the
   packed pair (hi << 16) | lo; `pop {r1}` at 0x0803F5E8 is lesson 3.94's
   not-void marker.  The body is three near-identical if/else-if chains over
   Task.unk3C (one per PlayerState.unk0D value 1/2/5) whose arms are all
   cross-jumped into one shared set of tails at 0x0803F16C-0x0803F5C2. */
s32 sub_0803eaf8(s32 a0)
{
    struct Task *t;
    u16 lo;
    u16 hi;

    lo = 0;
    hi = 0;
    switch (gPlayerStates[a0].unk0D)
    {
    case 1:
        t = &gTasks[a0];
        if ((u16)t->unk3C >= 434 && (u16)t->unk3C <= 453)
        {
            hi = 0;
            lo = 6;
        }
        else if ((u16)t->unk3C >= 486 && (u16)t->unk3C <= 487)
        {
            if ((t->unk3E & 0x8000) == 0)
                hi = -12;
            else
                hi = 12;
            lo = 4;
        }
        else if ((u16)t->unk3C >= 488 && (u16)t->unk3C <= 509)
        {
            switch (t->unk3C)
            {
            case 490:
            case 496:
                hi = 0;
                lo = 8;
                break;
            case 491:
            case 492:
            case 497:
            case 500:
            case 501:
                hi = 0;
                lo = 16;
                break;
            case 493:
            case 502:
            case 503:
            case 504:
            case 505:
                hi = 0;
                lo = 20;
                break;
            case 494:
            case 506:
            case 507:
            case 508:
                lo = 0x5A5A;
                hi = lo;
                break;
            default:
                lo = 0;
                hi = 0;
                break;
            }
        }
        else if ((u16)t->unk3C >= 510 && (u16)t->unk3C <= 525)
        {
            switch (t->unk3C)
            {
            default:
                hi = 0;
                lo = -4;
                break;
            case 522:
                hi = 0;
                lo = 6;
                break;
            }
        }
        else if ((u16)t->unk3C >= 526 && (u16)t->unk3C <= 577)
        {
            switch (t->unk3C)
            {
            case 536:
            case 537:
            case 538:
            case 549:
            case 550:
            case 551:
            case 562:
            case 563:
            case 564:
            case 575:
            case 576:
            case 577:
                hi = 0;
                lo = 8;
                break;
            default:
                lo = 0;
                hi = 0;
                break;
            }
        }
        else if ((u16)t->unk3C >= 578 && (u16)t->unk3C <= 582)
        {
            if ((u16)t->unk3C == 582)
                lo = 4;
            else
                lo = 8;
            if ((t->unk3E & 0x8000) == 0)
                hi = -8;
            else
                hi = 8;
        }
        else if ((u16)t->unk3C >= 583 && (u16)t->unk3C <= 585)
        {
            switch (t->unk3C)
            {
            default:
                hi = 0;
                break;
            case 584:
                if ((t->unk3E & 0x8000) == 0)
                    hi = 6;
                else
                    hi = -6;
                lo = 0;
                break;
            case 585:
                if ((t->unk3E & 0x8000) == 0)
                    hi = -8;
                else
                    hi = 8;
                lo = 0;
                break;
            }
        }
        else if ((u16)t->unk3C >= 617 && (u16)t->unk3C <= 632)
        {
            switch (t->unk3C)
            {
            case 617:
                hi = -16;
                lo = 4;
                break;
            case 618:
                hi = -16;
                lo = 16;
                break;
            case 619:
                hi = -16;
                lo = 20;
                break;
            case 620:
                hi = -16;
                lo = 24;
                break;
            case 621:
                hi = -8;
                lo = 24;
                break;
            case 623:
                hi = 8;
                lo = 24;
                break;
            case 624:
                hi = 16;
                lo = 24;
                break;
            case 625:
            case 626:
                hi = 16;
                lo = 16;
                break;
            case 627:
                hi = 16;
                lo = 4;
                break;
            case 628:
                hi = 12;
                lo = 0;
                break;
            case 629:
                hi = 6;
                lo = -4;
                break;
            case 631:
                hi = -6;
                lo = -4;
                break;
            case 632:
                hi = -12;
                lo = 0;
                break;
            case 630:
                lo = 0;
                hi = 0;
                break;
            case 622:
                hi = 0;
                lo = 28;
                break;
            }
        }
        else if ((u16)t->unk3C >= 4345 && (u16)t->unk3C <= 4368)
        {
            if ((u16)t->unk3C >= 4345 && (u16)t->unk3C <= 4348)
            {
                hi = -8;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->unk3E & 0x8000))
                    hi = 8;
            }
            else if ((u16)t->unk3C >= 4349 && (u16)t->unk3C <= 4352)
            {
                hi = 8;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->unk3E & 0x8000))
                    hi = -8;
            }
            else if ((u16)t->unk3C >= 4353 && (u16)t->unk3C <= 4356)
            {
                hi = 0;
                lo = 8;
            }
            else if ((u16)t->unk3C >= 4357 && (u16)t->unk3C <= 4360)
            {
                hi = 12;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->unk3E & 0x8000))
                    hi = lo;
            }
            else if ((u16)t->unk3C >= 4361 && (u16)t->unk3C <= 4364)
            {
                hi = -16;
                lo = 0;
                if (gUnk_0300244C != 0 && (t->unk3E & 0x8000))
                    hi = 16;
            }
            else
            {
                hi = 16;
                lo = 0;
                if (gUnk_0300244C != 0 && (t->unk3E & 0x8000))
                    hi = -16;
            }
        }
        break;
    case 2:
        t = &gTasks[a0];
        if ((u16)t->unk3C >= 675 && (u16)t->unk3C <= 694)
        {
            hi = 0;
            lo = 6;
        }
        else if ((u16)t->unk3C >= 727 && (u16)t->unk3C <= 728)
        {
            if ((t->unk3E & 0x8000) == 0)
                hi = -12;
            else
                hi = 12;
            lo = 4;
        }
        else if ((u16)t->unk3C >= 729 && (u16)t->unk3C <= 750)
        {
            switch (t->unk3C)
            {
            case 731:
            case 737:
                hi = 0;
                lo = 8;
                break;
            case 732:
            case 733:
            case 738:
            case 741:
            case 742:
                hi = 0;
                lo = 16;
                break;
            case 734:
            case 743:
            case 744:
            case 745:
            case 746:
                hi = 0;
                lo = 20;
                break;
            case 735:
            case 747:
            case 748:
            case 749:
                lo = 0x5A5A;
                hi = lo;
                break;
            default:
                lo = 0;
                hi = 0;
                break;
            }
        }
        else if ((u16)t->unk3C >= 751 && (u16)t->unk3C <= 766)
        {
            hi = 0;
            lo = -4;
        }
        else if ((u16)t->unk3C >= 767 && (u16)t->unk3C <= 818)
        {
            switch (t->unk3C)
            {
            case 777:
            case 778:
            case 779:
            case 790:
            case 791:
            case 792:
            case 803:
            case 804:
            case 805:
            case 816:
            case 817:
            case 818:
                hi = 0;
                lo = 8;
                break;
            default:
                lo = 0;
                hi = 0;
                break;
            }
        }
        else if ((u16)t->unk3C >= 819 && (u16)t->unk3C <= 823)
        {
            if ((u16)t->unk3C == 823)
                lo = 4;
            else
                lo = 8;
            if ((t->unk3E & 0x8000) == 0)
                hi = -8;
            else
                hi = 8;
        }
        else if ((u16)t->unk3C >= 824 && (u16)t->unk3C <= 826)
        {
            switch (t->unk3C)
            {
            default:
                hi = 0;
                break;
            case 825:
                if ((t->unk3E & 0x8000) == 0)
                    hi = 6;
                else
                    hi = -6;
                lo = 0;
                break;
            case 826:
                if ((t->unk3E & 0x8000) == 0)
                    hi = -8;
                else
                    hi = 8;
                lo = 0;
                break;
            }
        }
        else if ((u16)t->unk3C >= 858 && (u16)t->unk3C <= 873)
        {
            switch (t->unk3C)
            {
            case 858:
                hi = -16;
                lo = 4;
                break;
            case 859:
                hi = -16;
                lo = 16;
                break;
            case 860:
                hi = -16;
                lo = 20;
                break;
            case 861:
                hi = -16;
                lo = 24;
                break;
            case 862:
                hi = -8;
                lo = 24;
                break;
            case 864:
                hi = 8;
                lo = 24;
                break;
            case 865:
                hi = 16;
                lo = 24;
                break;
            case 866:
            case 867:
                hi = 16;
                lo = 16;
                break;
            case 868:
                hi = 16;
                lo = 4;
                break;
            case 869:
                hi = 12;
                lo = 0;
                break;
            case 870:
                hi = 6;
                lo = -4;
                break;
            case 872:
                hi = -6;
                lo = -4;
                break;
            case 873:
                hi = -12;
                lo = 0;
                break;
            case 871:
                lo = 0;
                hi = 0;
                break;
            case 863:
                hi = 0;
                lo = 28;
                break;
            }
        }
        else if ((u16)t->unk3C >= 4369 && (u16)t->unk3C <= 4392)
        {
            if ((u16)t->unk3C >= 4369 && (u16)t->unk3C <= 4372)
            {
                hi = -8;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->unk3E & 0x8000))
                    hi = 8;
            }
            else if ((u16)t->unk3C >= 4373 && (u16)t->unk3C <= 4376)
            {
                hi = 8;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->unk3E & 0x8000))
                    hi = -8;
            }
            else if ((u16)t->unk3C >= 4377 && (u16)t->unk3C <= 4380)
            {
                hi = 0;
                lo = 8;
            }
            else if ((u16)t->unk3C >= 4381 && (u16)t->unk3C <= 4384)
            {
                hi = 12;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->unk3E & 0x8000))
                    hi = lo;
            }
            else if ((u16)t->unk3C >= 4385 && (u16)t->unk3C <= 4388)
            {
                hi = -16;
                lo = 0;
                if (gUnk_0300244C != 0 && (t->unk3E & 0x8000))
                    hi = 16;
            }
            else
            {
                hi = 16;
                lo = 0;
                if (gUnk_0300244C != 0 && (t->unk3E & 0x8000))
                    hi = -16;
            }
        }
        else if ((u16)t->unk3C >= 843 && (u16)t->unk3C <= 853)
        {
            gCurTask->unk40 = (gCurTask->unk40 & ~15) | 12;
        }
        else
        {
            gCurTask->unk40 = (gCurTask->unk40 & ~15) | 8;
        }
        break;
    case 5:
        t = &gTasks[a0];
        if ((u16)t->unk3C >= 1284 && (u16)t->unk3C <= 1303)
        {
            hi = 0;
            lo = 6;
        }
        else if ((u16)t->unk3C >= 1336 && (u16)t->unk3C <= 1337)
        {
            if ((t->unk3E & 0x8000) == 0)
                hi = -12;
            else
                hi = 12;
            lo = 4;
        }
        else if ((u16)t->unk3C >= 1338 && (u16)t->unk3C <= 1359)
        {
            switch (t->unk3C)
            {
            case 1340:
            case 1346:
                hi = 0;
                lo = 8;
                break;
            case 1341:
            case 1342:
            case 1347:
            case 1350:
            case 1351:
                hi = 0;
                lo = 16;
                break;
            case 1343:
            case 1352:
            case 1353:
            case 1354:
            case 1355:
                hi = 0;
                lo = 20;
                break;
            case 1344:
            case 1356:
            case 1357:
            case 1358:
                lo = 0x5A5A;
                hi = lo;
                break;
            default:
                lo = 0;
                hi = 0;
                break;
            }
        }
        else if ((u16)t->unk3C >= 1360 && (u16)t->unk3C <= 1375)
        {
            hi = 0;
            lo = -4;
        }
        else if ((u16)t->unk3C >= 1376 && (u16)t->unk3C <= 1427)
        {
            switch (t->unk3C)
            {
            case 1386:
            case 1387:
            case 1388:
            case 1399:
            case 1400:
            case 1401:
            case 1412:
            case 1413:
            case 1414:
            case 1425:
            case 1426:
            case 1427:
                hi = 0;
                lo = 8;
                break;
            default:
                lo = 0;
                hi = 0;
                break;
            }
        }
        else if ((u16)t->unk3C >= 1428 && (u16)t->unk3C <= 1432)
        {
            if ((u16)t->unk3C == 1432)
                lo = 4;
            else
                lo = 8;
            if ((t->unk3E & 0x8000) == 0)
                hi = -8;
            else
                hi = 8;
        }
        else if ((u16)t->unk3C >= 1433 && (u16)t->unk3C <= 1435)
        {
            switch (t->unk3C)
            {
            default:
                hi = 0;
                break;
            case 1434:
                if ((t->unk3E & 0x8000) == 0)
                    hi = 6;
                else
                    hi = -6;
                lo = 0;
                break;
            case 1435:
                if ((t->unk3E & 0x8000) == 0)
                    hi = -8;
                else
                    hi = 8;
                lo = 0;
                break;
            }
        }
        else if ((u16)t->unk3C >= 1467 && (u16)t->unk3C <= 1482)
        {
            switch (t->unk3C)
            {
            case 1467:
                hi = -16;
                lo = 4;
                break;
            case 1468:
                hi = -16;
                lo = 16;
                break;
            case 1469:
                hi = -16;
                lo = 20;
                break;
            case 1470:
                hi = -16;
                lo = 24;
                break;
            case 1471:
                hi = -8;
                lo = 24;
                break;
            case 1473:
                hi = 8;
                lo = 24;
                break;
            case 1474:
                hi = 16;
                lo = 24;
                break;
            case 1475:
            case 1476:
                hi = 16;
                lo = 16;
                break;
            case 1477:
                hi = 16;
                lo = 4;
                break;
            case 1478:
                hi = 12;
                lo = 0;
                break;
            case 1479:
                hi = 6;
                lo = -4;
                break;
            case 1481:
                hi = -6;
                lo = -4;
                break;
            case 1482:
                hi = -12;
                lo = 0;
                break;
            case 1480:
                lo = 0;
                hi = 0;
                break;
            case 1472:
                hi = 0;
                lo = 28;
                break;
            }
        }
        else if ((u16)t->unk3C >= 4405 && (u16)t->unk3C <= 4428)
        {
            if ((u16)t->unk3C >= 4405 && (u16)t->unk3C <= 4408)
            {
                hi = -8;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->unk3E & 0x8000))
                    hi = 8;
            }
            else if ((u16)t->unk3C >= 4409 && (u16)t->unk3C <= 4412)
            {
                hi = 8;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->unk3E & 0x8000))
                    hi = -8;
            }
            else if ((u16)t->unk3C >= 4413 && (u16)t->unk3C <= 4416)
            {
                hi = 0;
                lo = 8;
            }
            else if ((u16)t->unk3C >= 4417 && (u16)t->unk3C <= 4420)
            {
                hi = 12;
                lo = -12;
                if (gUnk_0300244C != 0 && (t->unk3E & 0x8000))
                    hi = lo;
            }
            else if ((u16)t->unk3C >= 4421 && (u16)t->unk3C <= 4424)
            {
                hi = -16;
                lo = 0;
                if (gUnk_0300244C != 0 && (t->unk3E & 0x8000))
                    hi = 16;
            }
            else
            {
                hi = 16;
                lo = 0;
                if (gUnk_0300244C != 0 && (t->unk3E & 0x8000))
                    hi = -16;
            }
        }
        break;
    }
    if (t->unk3C == -1 || ((u16)t->unk3C >= 4551 && (u16)t->unk3C <= 4554))
    {
        lo = 0x5A5A;
        hi = lo;
    }
    return (hi << 16) | lo;
}

void sub_0803f5fc(s8 a0)
{
    s32 i, n;

    if (a0 == 0)
        n = 4;
    else if (a0 == 1)
        n = 7;
    else if (a0 == 2)
        n = 10;
    else if (a0 == 3)
        n = 13;
    else
        return;
    for (i = n; i < n + 3; i++)
    {
        if ((s16)gTaskSlotTypes[i] != -1)
            TaskFree(i);
    }
    if (a0 == 0)
        n = 16;
    else if (a0 == 1)
        n = 20;
    else if (a0 == 2)
        n = 24;
    else if (a0 == 3)
        n = 28;
    for (i = n; i < n + 4; i++)
    {
        if ((s16)gTaskSlotTypes[i] != -1)
            TaskFree(i);
    }
    for (i = 32; i <= 62; i++)
    {
        if (gTaskSlotTypes[i] == 7 || gTaskSlotTypes[i] == 6)
            TaskFree(i);
    }
}

void sub_0803f6e0(void)
{
    s32 sel[4];
    s32 i, k, v;
    s32 *p;

    gUnk_020055C4[0]++;
    if (gActivePlayerCount == 1)
    {
        gCurTask->unk2C = 0;
        return;
    }
    v = -1;
    p = &sel[3];
    do
    {
        *p = v;
        p--;
    } while ((s32)p >= (s32)sel);
    for (i = 0; i < gActivePlayerCount; i++)
    {
    retry:
        gUnk_03001F2C = RandomRange(gActivePlayerCount);
        for (k = 0; k < gActivePlayerCount; k++)
        {
            if (sel[k] == gUnk_03001F2C)
                goto retry;
        }
        sel[i] = gUnk_03001F2C;
    }
    for (i = 0; i < gPlayerCount; i++)
        gTasks[i].unk2C = -1;
    k = 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if ((gActivePlayerMask >> i) & 1)
            gTasks[i].unk2C = sel[k++];
    }
}

u16 sub_0803f7e0(u16 a0)
{
    s32 k;

    switch (gCurTask->unk88->unk0D)
    {
    default:
        k = 0;
        break;
    case 4:
        k = 1;
        break;
    case 9:
        k = 2;
        break;
    case 10:
        k = 3;
        break;
    case 24:
        k = 4;
        break;
    }
    return gUnk_0873D79E[k * 7 + a0];
}

void sub_0803f834(u16 a0, void *src)
{
    CpuSet(src, gUnk_02007F60, 32);
    gUnk_02007F60[a0 * 2 + 16] = 0xFFFF;
    gUnk_02007F60[a0 * 2 + 17] = -1;
}

void PlayerTurnToHeldDirection(void)
{
    if (PlayerFaceHeldDirection() != 0)
        sub_0803e414();
}

s32 sub_0803f884(void)
{
    if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 48)
    {
        if (((gLatchedHeldKeys[gCurTask->unk88->unk00] & 16) && gCurTask->unk43 == 1)
         || ((gLatchedHeldKeys[gCurTask->unk88->unk00] & 32) && gCurTask->unk43 == -1))
            return 1;
        return 2;
    }
    return 0;
}

void PlayerCheckBump(void)
{
    u32 v;

    if (gCurTask->unk7A & 1)
    {
        if (gCurTask->unk58 > 45823)
        {
            gCurTask->unk88->unk3E = 2;
            return;
        }
    }
    else if (gTerrainResult[1] != 0)
    {
        if (gCurTask->unk58 < 0 && gCurTask->unk58 <= 0xFFFF4D00)
            gCurTask->unk88->unk3E = 1;
        return;
    }
    if (gTerrainResult[0] != 0)
    {
        v = (gCurTask->unk54 < 0) ? -gCurTask->unk54 : gCurTask->unk54;
        if (v > 45823)
        {
            if (gTerrainResult[0] == 1)
            {
                if (gCurTask->unk43 == 1)
                    gCurTask->unk88->unk3E = 3;
                else
                    gCurTask->unk88->unk3E = 4;
            }
            else if (gCurTask->unk43 == -1)
                gCurTask->unk88->unk3E = 5;
            else
                gCurTask->unk88->unk3E = 6;
        }
    }
}

void PlayerStopAtCeilingAndWall(void)
{
    struct Task *t = gCurTask;

    if (t->unk58 < 0 && gTerrainResult[1] != 0)
        t->unk58 = 0;
    PlayerStopAtWall();
}

s32 PlayerStopAtWall(void)
{
    if (gTerrainResult[0] == 0)
        return 0;
    if (((s32 *)gCurTask->unk88)[21] == 0
     || (gTerrainResult[0] == 1 && gCurTask->unk43 == 1)
     || (gTerrainResult[0] == 2 && gCurTask->unk43 == -1))
    {
        PlayerStopAxes(1);
        return 1;
    }
    return 0;
}

s32 PlayerCheckLanding(void)
{
    struct Task *t = gCurTask;

    if ((t->unk7A & 1) && t->unk58 != 0)
    {
        PlayerStopAxes(2);
        return 1;
    }
    return 0;
}

s32 sub_0803fa74(void)
{
    if (gCurTask->unk78 != 0
     && ((gCurTask->unk88->unk42 & 1024)
      || (sub_08022760(gCurTask) == 0
       && (gUnk_02005574[0] == 0 || (gCurTask->unk7A & 1) == 0
           || (gCurTask->unk88->unk48 & 4) == 0)
       && (gTerrainResult[0] != 1 || (gCurTask->unk88->unk48 & 1) == 0)
       && (gTerrainResult[0] != 2 || (gCurTask->unk88->unk48 & 2) == 0))))
        return 0;
    sub_08009ee8(-gUnk_02005588[gCurTask->unk88->unk00], gCurTask->unk88->unk00);
    gCurTask->unk7C = 1;
    gCurTask->unk88->unk01 = 17;
    return gCurTask->unk88->unk01;
}

void sub_0803fb54(void)
{
    u16 v;

    if (gCurTask->unk7B & 1)
    {
        if (gCurTask->unk88->unk40 & 16)
        {
            gCurTask->unk88->unk40 &= ~16;
            gCurTask->unk88->unk3D = 0;
            gCurTask->unk88->unk0F = 0;
        }
        return;
    }
    v = gCurTask->unk88->unk40 & 16;
    if (v == 0)
    {
        if (gCurTask->unk88->unk3D != 0)
            gCurTask->unk88->unk0F = 0;
        else if ((gLatchedHeldKeys[gCurTask->unk88->unk00] & 48) == 0)
        {
            if ((s8)gCurTask->unk88->unk0F <= 16)
                gCurTask->unk88->unk0F++;
        }
        else if (gCurTask->unk88->unk17 != 0)
        {
            gCurTask->unk88->unk3D = 1;
            gCurTask->unk88->unk40 |= 16;
            gCurTask->unk88->unk0F = 10;
        }
        else if ((s8)gCurTask->unk88->unk0F != 0
              && (s8)gCurTask->unk88->unk0F <= 16)
        {
            if (((gLatchedHeldKeys[gCurTask->unk88->unk00] & 16)
                 && gCurTask->unk43 == 1)
             || ((gLatchedHeldKeys[gCurTask->unk88->unk00] & 32)
                 && gCurTask->unk43 == -1))
            {
                gCurTask->unk88->unk3D = 1;
                gCurTask->unk88->unk40 |= 16;
                gCurTask->unk88->unk0F = 10;
            }
        }
        else
            gCurTask->unk88->unk0F = 0;
    }
    else if (gCurTask->unk88->unk04 == 2)
    {
        gCurTask->unk88->unk40 &= ~16;
        gCurTask->unk88->unk0F = 0;
    }
    else if ((s8)gCurTask->unk88->unk0F != 0)
        gCurTask->unk88->unk0F--;
    else
    {
        gCurTask->unk88->unk3D = 0;
        gCurTask->unk88->unk40 &= ~16;
        gCurTask->unk88->unk0F = 0;
    }
}

s32 PlayerHasCrossedWaterSurface(s32 a)
{
    if (gCurTask->unk7B & 128)
    {
        if (a == 0)
        {
            if (gCurTask->unk7B & 1)
                return 1;
        }
        else if ((gCurTask->unk7B & 1) == 0)
            return 1;
    }
    return 0;
}

s32 sub_0803fd20(s32 a0)
{
    if (gTasks[a0].unk7A == 0)
        return 0;
    if (gTasks[a0].unk43 == 1)
        return gUnk_0873AF30[gPlayerStates[a0].unk4B][0];
    return gUnk_0873AF30[gPlayerStates[a0].unk4B][1];
}

s32 sub_0803fd90(void)
{
    if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 16)
    {
        if (gCurTask->unk43 == -1)
            gCurTask->unk88->unk01 = 4;
    }
    else if ((gLatchedHeldKeys[gCurTask->unk88->unk00] & 32) && gCurTask->unk43 == 1)
        gCurTask->unk88->unk01 = 4;
    return gCurTask->unk88->unk01;
}

s32 PlayerCheckJump(void)
{
    if ((gCurTask->unk7A & 1)
     && (gLatchedHeldKeys[gCurTask->unk88->unk00] & 128) == 0
     && (gLatchedPressedKeys[gCurTask->unk88->unk00] & 1)
     && gCurTask->unk88->unk37 != 2)
    {
        if (gCurTask->unk88->unk37 != 3)
            gCurTask->unk88->unk01 = 5;
        else
            gCurTask->unk88->unk01 = 57;
    }
    return gCurTask->unk88->unk01;
}

s32 sub_0803fe68(void)
{
    struct Task *t = gCurTask;

    if ((t->unk7A & 1) == 0)
        t->unk88->unk01 = 7;
    else if (t->unk7B & 1)
    {
        if (t->unk54 != 0)
            t->unk88->unk01 = 25;
        else
            t->unk88->unk01 = 24;
    }
    return gCurTask->unk88->unk01;
}

s32 PlayerCheckDuckOrSwallow(void)
{
    if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 128)
    {
        if (gCurTask->unk88->unk06 == 1 && gUnk_02007CF0 != 1)
        {
            if (((s8 *)gCurTask->unk88)[11] != 0)
                gCurTask->unk88->unk01 = 29;
            else if ((gCurTask->unk7B & 1) == 0)
                gCurTask->unk88->unk01 = 15;
            else
                gCurTask->unk88->unk01 = 26;
            if (gCurTask->unk88->unk31 != 0)
            {
                if (gCurTask->unk88->unk30 <= 2)
                    gCurTask->unk88->unk30++;
                gCurTask->unk88->unk31 = 0;
            }
            else
                gCurTask->unk88->unk30 = 0;
        }
        else if ((gCurTask->unk7B & 1) == 0)
            gCurTask->unk88->unk01 = 10;
    }
    return gCurTask->unk88->unk01;
}

s32 PlayerCheckLadder(void)
{
    if (gCurTask->unk88->unk06 == 0 && gTerrainResult[6] != 0)
    {
        if (gTerrainResult[6] & 1)
        {
            if (gLatchedPressedKeys[gCurTask->unk88->unk00] & 64)
                gCurTask->unk88->unk01 = 12;
        }
        else if (gLatchedPressedKeys[gCurTask->unk88->unk00] & 128)
            gCurTask->unk88->unk01 = 12;
    }
    return gCurTask->unk88->unk01;
}

s32 PlayerCheckFloat(void)
{
    u16 v;

    if (gUnk_03001F30 == 1)
        return 0;
    if (gCurTask->unk88->unk06 != 1)
    {
        v = gLatchedHeldKeys[gCurTask->unk88->unk00] & 64;
        if (v != 0)
        {
            if (gCurTask->unk7A & 1)
            {
                if (++((s8 *)gCurTask->unk88)[16] == 9)
                {
                    ((s8 *)gCurTask->unk88)[16] = 0;
                    gCurTask->unk88->unk01 = 9;
                }
            }
            else if ((gCurTask->unk7B & 1) == 0)
            {
                ((s8 *)gCurTask->unk88)[16] = 0;
                gCurTask->unk88->unk01 = 9;
            }
        }
        else
            ((s8 *)gCurTask->unk88)[16] = v;
    }
    return gCurTask->unk88->unk01;
}

s32 sub_08040084(void)
{
    if (gCurTask->unk88->unk06 == 0
     && (gLatchedPressedKeys[gCurTask->unk88->unk00] & 1))
        gCurTask->unk88->unk01 = 9;
    return gCurTask->unk88->unk01;
}

s32 sub_080400c0(void)
{
    u8 v;

    if (gUnk_03001F30 == 1)
    {
        if ((gLatchedPressedKeys[gCurTask->unk88->unk00] & 2) == 0)
            goto out;
        if (gCurTask->unk7A & 1)
        {
            if (gCurTask->unk88->unk3D != 0)
                gCurTask->unk88->unk01 = 27;
            else if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 64)
                gCurTask->unk88->unk01 = 28;
            else
                gCurTask->unk88->unk01 = 26;
        }
        else
        {
            if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 128)
                gCurTask->unk88->unk01 = 29;
            else if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 64)
                gCurTask->unk88->unk01 = 28;
            else
                gCurTask->unk88->unk01 = 26;
        }
        gCurTask->unk88->unk3D = 0;
        goto out;
    }
    if (gCurTask->unk88->unk0D == 15
     && ((gCurTask->unk7A & 1) || (gCurTask->unk7B & 1)))
        gCurTask->unk88->unk47 = 1;
    if ((gLatchedPressedKeys[gCurTask->unk88->unk00] & 2) == 0)
        goto out;
    gCurTask->unk88->unk3D = 0;
    v = gUnk_0873D0C4[(gCurTask->unk7B & 1) + gCurTask->unk88->unk0D * 2];
    if (gCurTask->unk88->unk06 == 1)
    {
        if ((gCurTask->unk7B & 1) == 0)
            gCurTask->unk88->unk01 = 14;
        else
            gCurTask->unk88->unk01 = 27;
        goto out;
    }
    if (gCurTask->unk88->unk0D == 15 && (gCurTask->unk88->unk42 & 4) == 0
     && (gCurTask->unk7B & 1) == 0)
    {
        if (gCurTask->unk88->unk47 == 0)
            goto out;
        gCurTask->unk88->unk47--;
        if (gCurTask->unk88->unk04 == 5)
            gCurTask->unk88->unk04 = 4;
    }
    if ((gCurTask->unk88->unk42 & 4) == 0 || v == 13)
        gCurTask->unk88->unk01 = v;
out:
    return gCurTask->unk88->unk01;
}

s32 sub_08040264(void)
{
    if (gCurTask->unk58 > 0 && PlayerHasCrossedWaterSurface(0) != 0)
        gCurTask->unk88->unk01 = 23;
    return gCurTask->unk88->unk01;
}

s32 PlayerCheckEnterDoor(void)
{
    if ((gLatchedHeldKeys[gCurTask->unk88->unk00] & 64) && gUnk_02007CF0 == 0
     && gCurTask->unk88->unk4C != 0
     && (gCurTask->unk88->unk42 & 1) == 0
     && sub_08024e40(gCurTask->unk48, gCurTask->unk4A) != 0)
    {
        gUnk_02007CF0 = 1;
        gUnk_03001F34 = 1;
        gCurTask->unk88->unk42 |= 2;
        SetPlayerInvulnerability(3, 0, gCurTask->unk88->unk00);
        gCurTask->unk88->unk01 = 20;
    }
    return gCurTask->unk88->unk01;
}

s32 PlayerCheckDropAbility(void)
{
    if (gUnk_03001F30 == 1)
        return 0;
    if ((gCurTask->unk88->unk42 & 2) == 0
     && gCurTask->unk88->unk37 == 0
     && (gLatchedPressedKeys[gCurTask->unk88->unk00] & 4)
     && gCurTask->unk88->unk0D != 0)
    {
        sub_08064eb8(gCurTask->unk88->unk30);
        PlaySfxIfLocalPlayer(182, (u16)gCurTask->unk88->unk00);
        sub_0800a008(0, -1, gCurTask->unk88->unk00);
        gCurTask->unk88->unk01 = gCurTask->unk88->unk02;
    }
    return gCurTask->unk88->unk01;
}

s32 sub_080403e4(void)
{
    if ((gLatchedPressedKeys[gCurTask->unk88->unk00] & 65) || (gCurTask->unk7A & 1) == 0)
        gCurTask->unk88->unk01 = 23;
    return gCurTask->unk88->unk01;
}

s32 PlayerRequestLocomotion(void)
{
    if ((gCurTask->unk7B & 1) == 0)
    {
        if ((gCurTask->unk7A & 1) == 0)
            gCurTask->unk88->unk01 = 7;
        else if (gCurTask->unk54 == 0)
            gCurTask->unk88->unk01 = 1;
        else if (gCurTask->unk88->unk3D == 0)
            gCurTask->unk88->unk01 = 2;
        else
            gCurTask->unk88->unk01 = 3;
    }
    else if ((gCurTask->unk7A & 1) == 0)
        gCurTask->unk88->unk01 = 23;
    else if (gLatchedHeldKeys[gCurTask->unk88->unk00] & 65)
        gCurTask->unk88->unk01 = 23;
    else if (gCurTask->unk54 == 0)
        gCurTask->unk88->unk01 = 24;
    else
        gCurTask->unk88->unk01 = 25;
    return gCurTask->unk88->unk01;
}

s32 sub_080404e4(void)
{
    if (gUnk_03001F30 == 1)
        return 0;
    if (gCurTask->unk88->unk06 == 2)
        return 1;
    if (gCurTask->unk88->unk06 == 1)
        return 2;
    return 0;
}

/* PlayerState+0x38 is a u16 counter; task.h still spells 0x38-0x3C as
   `u8 filler38[]`, and only a real struct field gives agbcc's `lsls #16; cmp`
   test instead of `lsls #16; lsrs #16; cmp`.  Replace with `p->unk38` once
   task.h names it (see report). */
struct M11Ctr { u16 unk00; };
#define CTR(p) (((struct M11Ctr *)&(p)->unk38)->unk00)

s32 sub_08040514(void)
{
    struct Task *u;
    struct PlayerState *q;
    s32 i;

    if (CTR(gCurTask->unk88) == 0)
        return 0;
    if (--CTR(gCurTask->unk88) == 0)
    {
        gCurTask->unk88->unk3A = 0;
        gCurTask->unk88->unk3B = 0;
        return 0;
    }
    if (gUnk_02007CF0 != 0)
        return 0;
    if (gActivePlayerCount == 1)
        return 0;
    if (gCurTask->unk88->unk04 == 10 || gCurTask->unk88->unk04 == 11
     || gCurTask->unk88->unk04 == 12 || gCurTask->unk88->unk04 == 16
     || gCurTask->unk88->unk04 == 17 || gCurTask->unk88->unk04 == 18
     || gCurTask->unk88->unk04 == 19 || gCurTask->unk88->unk04 == 22
     || gCurTask->unk88->unk04 == 23 || gCurTask->unk88->unk01 == 19
     || gCurTask->unk7C == 1 || gCurTask->unk7C == 2
     || (gCurTask->unk88->unk04 == 13
         && (gCurTask->unk88->unk0D != 24 || gCurTask->unk73 <= 6)))
        return 0;
    for (i = 0; i < gPlayerCount; i++)
    {
        if (i == gCurTask->unk88->unk00)
            continue;
        if ((gCurTask->unk88->unk3B >> i) & 1)
            continue;
        if (gUnk_02005588[i] == 0)
            continue;
        u = &gTasks[i];
        if (u->unk13 != 0)
            continue;
        if (u->unk48 - gCurTask->unk48 >= 0)
        {
            if (u->unk48 - gCurTask->unk48 > 10)
                continue;
        }
        else if (gCurTask->unk48 - u->unk48 > 10)
            continue;
        if (u->unk4A - gCurTask->unk4A >= 0)
        {
            if (u->unk4A - gCurTask->unk4A > 5)
                continue;
        }
        else if (gCurTask->unk4A - u->unk4A > 5)
            continue;
        q = &gPlayerStates[i];
        if (q->unk04 == 10 || q->unk04 == 11 || q->unk04 == 12 || q->unk04 == 16
         || q->unk04 == 17 || q->unk04 == 18 || q->unk04 == 19 || q->unk04 == 22
         || q->unk04 == 23 || q->unk01 == 19 || u->unk7C == 1 || u->unk7C == 2)
            continue;
        if (q->unk04 == 13)
        {
            if (q->unk0D != 24)
                return 0;
            if (u->unk73 <= 6)
                continue;
        }
        gCurTask->unk88->unk01 = 19;
        q->unk01 = 19;
        gCurTask->unk18 = u->unk18 = i;
        u->unk12++;
        u->unk1C = gCurTaskIdx;
        break;
    }
    return gCurTask->unk88->unk01;
}

void sub_08040710(void)
{
    struct Task *t = gCurTask;

    if ((t->unk7B & 1) == 0)
    {
        if (t->unk7A & 1)
            t->unk88->unk01 = 1;
        else
            t->unk88->unk01 = 7;
    }
    else
    {
        if (!(t->unk7A & 1) || (gLatchedHeldKeys[t->unk88->unk00] & 65))
            t->unk88->unk01 = 23;
        else
            t->unk88->unk01 = 24;
    }
}

void LatchPlayerKeys(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        gLatchedHeldKeys[i] = gPlayerHeldKeys[i];
        gLatchedPressedKeys[i] = gPlayerPressedKeys[i];

        if (gPlayerStates[i].unk42 & 64)
        {
            gLatchedPressedKeys[i] = 0;
            gLatchedHeldKeys[i] = 0;
        }
    }
}

void sub_08040808(s32 a0)
{
    s32 t;

    if (gPlayerCount != 1 && a0 == gLocalPlayer)
    {
        t = CreatePlayerEffectHighSlot((s8)a0, 21, 0);
        gTasks[t].unk44 = a0;
        gTasks[t].unk88 = gTasks[a0].unk88;
    }
}

void sub_08040858(s32 a0)
{
    struct PlayerState *p = gPlayerStates + a0;

    p->unk40 |= 32;
    p->unk42 |= 2;
}

void sub_0804087c(s32 a0)
{
    (gPlayerStates + a0)->unk40 |= 64;
}

void sub_08040894(s32 a0, u8 a1)
{
    struct PlayerState *p;

    if (gActivePlayerCount > 1)
    {
        p = gPlayerStates + a0;
        p->unk38 = 300;
        p->unk3A = a1;
        p->unk3B = 0;
    }
    else
    {
        /* `p` is genuinely uninitialized here in the ROM: the else arm stores
           through whatever register the pointer was allocated to. */
        p->unk38 = 0;
        p->unk3B = 0;
        p->unk3A = 0;
    }
}

s32 sub_080408e4(void)
{
    s32 i;
    s32 r = 0;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (gPlayerStates[i].unk17 != 0 && (s16)gPlayerStates[i].unk18 > 240)
        {
            r = 1;
            break;
        }
    }

    return r;
}

void sub_08040934(s32 a0)
{
    s32 i;
    s32 b;

    b = gUnk_0873AF3A[a0][0];
    for (i = 0; i <= 2; i++)
    {
        if (gTaskSlotTypes[i + b] != -1)
            TaskFree(i + b);
    }

    b = gUnk_0873AF3A[a0][1];
    for (i = 0; i <= 3; i++)
    {
        if (gTaskSlotTypes[i + b] != -1)
            TaskFree(i + b);
    }

    sub_080409b8(a0);
}

void sub_080409b8(s32 a0)
{
    s32 n;

    switch (gPlayerStates[a0].unk0D)
    {
    case 1:
    case 2:
        n = CreatePlayerEffect((s8)a0, 15, 0);
        if (n != -1)
        {
            struct Task *s = &gTasks[a0];
            struct Task *d = &gTasks[n];

            d->unk4C = s->unk4C;
            d->unk48 = s->unk48;
            d->unk50 = s->unk50;
            d->unk4A = s->unk4A;
            d->unk43 = s->unk43;
            d->unk88 = s->unk88;
            d->unk44 = a0;
        }
        break;
    }
}

void sub_08040a44(s16 p0, s16 p1)
{
    u16 a0 = p0;
    u16 a1 = p1;
    struct Task *t = gCurTask;
    s32 idx;
    s32 v;

    if ((t->unk7B & 1) == 0)
    {
        idx = gUnk_0873DB44[t->unk88->unk0D][1];
        v = t->unk3C - gUnk_0873DB44[t->unk88->unk0D][0];
    }
    else
    {
        idx = 0;
        switch (t->unk88->unk0D)
        {
        default:
            v = gCurTask->unk3C - 138;
            break;
        case 4:
            v = t->unk3C - 1122;
            break;
        case 9:
            v = t->unk3C - 1923;
            break;
        case 10:
            v = t->unk3C - 2180;
            break;
        }
    }

    if ((u32)v <= 10)
    {
        v = gUnk_0873AF42[v + idx * 11];

        if (v != 255)
            QueueSprite(6, gUnk_087519CC[v], gCurTask->unk3E & 0xA000, 0, (s16)a0, (s16)a1);
    }
}
