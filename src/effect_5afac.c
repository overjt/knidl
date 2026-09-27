#include "gba/gba.h"
#include "global.h"
#include "task.h"

extern u32 gUnk_02000020[];
extern u32 gUnk_020060CC[];
extern u8 gUnk_02006A14[];
extern u8 gUnk_02007CF0;
extern s32 gUnk_02007D00[];
extern s8 gUnk_02007FB8[];
extern u32 gUnk_02008010[];
extern u32 gUnk_02020000[];
extern u32 gBg0ScrollY;
extern vs32 gBg3ScrollX;
extern vs32 gBg2ScrollX;
extern vs32 gBg3ScrollY;
extern vs32 gBg1ScrollY;
extern u32 gBg0ScrollX;
extern u8 gObjPalette[];
extern u32 gUnk_03001570[];
extern vs32 gBg2ScrollY;
extern u16 gFrameCount;
extern vu16 gDispCnt;
extern vs32 gBg1ScrollX;
extern u8 gUnk_03001F34;
extern struct PlayerState gPlayerStates[];
extern u8 gActivePlayerMask;
extern s16 gSpriteCameraX;
extern u8 gActivePlayerCount;
extern u16 gLocalPlayer;
extern u16 gPlayerCount;
extern s32 gUnk_030023B4;
extern u32 gLatchedPressedKeys[];
extern s32 gUnk_030023D4;
extern s16 gSpriteCameraY;
extern u32 gStageRequest[];
extern s16 gUnk_0300244C;
extern struct Task *gCurTask;
extern struct Task gTasks[];
extern vs16 gTaskSlotTypes[];
extern u32 gObjVram[];
extern u32 gUnk_085B9B6C[];
extern s16 gUnk_0873DBAC[];
extern s16 gUnk_0873DBD4[];
extern u32 gUnk_0873DBE4[];
extern u32 gUnk_0873DC10[];
extern u32 gUnk_0873DC3C[];
extern u8 gUnk_0873DC4C[];
extern u8 gUnk_0873DC66[];
extern u8 gUnk_0873DC80[];
extern u16 gUnk_0873DC9A[];
extern u32 gUnk_0873DCA8[];
extern u32 gUnk_0873DCC0[];
extern u32 gUnk_0873DCC8[];
extern u32 gUnk_0873DCCC[];
extern u32 gUnk_0873DD16[];
extern u32 gUnk_0873DD30[];
extern u32 gUnk_0873DD4C[];
extern u32 gUnk_0873DD5C[];
extern u32 gUnk_0873DD64[];
extern u32 gUnk_0873DD80[];
extern u32 gUnk_0873DDA2[];
extern u32 gUnk_0873DDB4[];
extern u32 gUnk_0873DDBE[];
extern u32 gUnk_0873DDE8[];
extern u32 gUnk_0873DEA0[];
extern u16 gUnk_0873DEA8[];
extern u32 gUnk_0873DEDC[];
extern u32 gUnk_0874C890[];
extern u32 gUnk_0874CCF4[];
extern u32 gUnk_0874CDF8[];
extern u32 gUnk_0874CFEC[];
extern u32 gUnk_08754850[];
extern u32 gUnk_0875488C[];
extern u32 gUnk_087548A0[];

void TaskExitTrampoline(void);
void TaskYieldTrampoline(s32 frames);
void RequestCopy(u32 mode, void *src, void *dst, u32 size);
void QueueSprite(u32 a, s32 b, u32 c, u32 d, u32 e, u32 f);
void ResetFadeAndBlend(void);
void BeginFastFadeInFromWhite(void);
void BeginFastFadeOutToWhite(void);
void ResetTasksAndOam(void);
void sub_080022fc(void);
void sub_08002338(void);
void sub_08002358(void);
void sub_08002378(void);
void RunLinkFrame(void);
void RunLinkFramesUntilFadeDone(void);
void CallTableEntry(u32 a, u32 b, u32 *c);
u32 RandomRange(u32 range);
s32 PlayBgm(s32 songId);
void PlaySfx(s32 id);
s32 TaskCreateFrom(u32 type, s32 idx);
s32 TaskCreateInRange(u32 type, s32 start, s32 end);
void TaskMove(void);
u32 TaskIsOnScreen(void);
void TaskDrawWorld(void);
void TaskSleepForever(void);
void TaskSetEntry(void *fn, u32 i);
void TaskStopX(void);
void TaskSetMotionY(s32 a, s32 b, s32 c);
void TaskStopY(void);
void TaskSetMotion(s32 a, s32 b, s32 c, s32 d, s32 e, s32 f);
void TaskStop(void);
void TaskSetFrame(s32 a);
void TaskSetFrameFlip(s32 a);
s32 IsOnScreen(s16 a, s16 b);
u32 IsWorldPosOnScreen(s16 x, s16 y);
void sub_08008c4c(u32 a);
void sub_08008c64(u16 a);
void sub_08009b2c(u16 a);
void sub_08009e60(s32 a, s32 b);
void sub_0800a008(u32 a, s32 b, s32 c);
void sub_0800a04c(s32 a, u32 b);
void sub_08023fd4(void);
void sub_080258e0(void);
void SetCameraFocus(s32 a, s32 b);
void sub_08026998(void);
void sub_08027178(void);
void sub_08033d0c(void);
void LatchPlayerKeys(void);
void sub_0805b16c(void);
void sub_0805b370(void);
s32 sub_0805b4bc(void);
void sub_0805b514(void);
s32 sub_0805b5b0(void);
void sub_0805b61c(void);
void sub_0805b670(void);
void sub_0805b83c(void);
void sub_0805b8b8(void);
void sub_0805b8f8(void);
void sub_0805b9a4(void);
void sub_0805ba08(void);
void sub_0805bb90(void);
void sub_0805bc1c(void);
void sub_0805bc5c(void);
void sub_0805bca4(void);
void sub_0805bce0(void);
void sub_0805bd34(void);
void sub_0805be48(void);
void sub_0805c114(void);
void sub_0805c150(void);
void sub_0805c584(void);
void sub_0805c814(void);
void sub_0805c990(void);
s32 sub_0805cc54(void);
void sub_0805ceec(void);
void sub_0805d420(void);
void sub_0805d5fc(void);
void TaskStartFrameScript(s32 a0);
void TaskStartFrameScriptId(s32 a0);
void TaskUpdateFrameScript(void);
void TaskAdvanceFrameScript(void);
void sub_0805d994(s32 a0, s32 a1);
void sub_0805da2c(void);
void sub_0805dd4c(void);
void sub_0805deac(void);
void sub_0805e15c(void);
void sub_0805e1bc(void);
void sub_0805e24c(void);
void ActorMove(void);
void sub_08068a8c(u32 a, u8 flag);
void sub_0806d4e4(s32 a, s32 b);

s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2)
{
    s8 kind = a0;
    u8 param = a1;
    s32 base;
    s32 idx;
    struct Task *t;

    if (kind == 0)
        base = 16;
    else if (kind == 1)
        base = 20;
    else if (kind == 2)
        base = 24;
    else if (kind == 3)
        base = 28;
    else
        return -1;
    idx = TaskCreateInRange(7, base, base + 3);
    if (idx == -1)
    {
        if (kind == 0)
            base = 4;
        else if (kind == 1)
            base = 7;
        else if (kind == 2)
            base = 10;
        else if (kind == 3)
            base = 13;
        else
            return -1;
        idx = TaskCreateInRange(7, base, base + 2);
    }
    if (idx != -1)
    {
        t = &gTasks[idx];
        t->unk18 = (param << 24) | (a2 & 0x00FFFFFF);
        t->unk4C = gCurTask->unk4C;
        t->unk48 = gCurTask->unk48;
        t->unk50 = gCurTask->unk50;
        t->unk4A = gCurTask->unk4A;
        t->unk43 = gCurTask->unk43;
        t->unk88 = gCurTask->unk88;
    }
    return idx;
}

s32 CreatePlayerEffectHighSlot(s32 a0, s32 a1, s32 a2)
{
    u8 param = a1;
    s32 idx;
    struct Task *t;

    idx = TaskCreateInRange(7, 32, 62);
    if (idx != -1)
    {
        t = &gTasks[idx];
        t->unk18 = (param << 24) | (a2 & 0x00FFFFFF);
        t->unk4C = gCurTask->unk4C;
        t->unk48 = gCurTask->unk48;
        t->unk50 = gCurTask->unk50;
        t->unk4A = gCurTask->unk4A;
        t->unk43 = gCurTask->unk43;
        t->unk88 = gCurTask->unk88;
        t->unk72 = 10;
    }
    return idx;
}

void sub_0805b110(void)
{
    sub_08008c4c(3);
    sub_0805b16c();
    sub_08009b2c(gLocalPlayer);
    sub_08002358();
    sub_08002378();
    sub_080022fc();
    BeginFastFadeInFromWhite();
    RunLinkFramesUntilFadeDone();
    do
    {
        RunLinkFrame();
        LatchPlayerKeys();
    } while (*(s8 *)0x03002438 == 0);
    sub_08002338();
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
    sub_08026998();
    sub_08027178();
}

void sub_0805b16c(void)
{
    s32 i;
    s8 *p;
    s8 *q;
    s32 z;

    ResetFadeAndBlend();
    ResetTasksAndOam();
    gDispCnt &= 0xE0FF;
    gDispCnt |= 248 << 5;
    gDispCnt |= 128;
    gBg0ScrollX = gBg1ScrollX = gBg2ScrollX = gBg3ScrollX = 0;
    gBg0ScrollY = gBg1ScrollY = gBg2ScrollY = gBg3ScrollY = 0;
    sub_08008c64(0);
    sub_08023fd4();
    for (i = 0; i < gPlayerCount; i++)
    {
        if (gPlayerStates[i].unk0D == 24)
            sub_0800a008(0, -1, i);
        else
            gPlayerStates[i].unk0D = 0;
    }
    gUnk_03001F34 = 1;
    gUnk_02007CF0 = 0;
    *(s8 *)gUnk_02008010 = -1;
    *(s8 *)gStageRequest = 0;
    q = gUnk_02007FB8;
    z = 0;
    p = q + 2;
    do
    {
        *p = z;
        p--;
    } while ((s32)p >= (s32)q);
}

void sub_0805b278(void)
{
    gUnk_030023D4 = 0;
    gCurTask->unk6C = 0;
    while ((s16)gCurTask->unk6C < gPlayerCount)
    {
        if ((s16)gCurTask->unk6C == gCurTask->unk88->unk00)
            gCurTask->unk2C = gUnk_030023D4;
        if (((gActivePlayerMask >> (s16)gCurTask->unk6C) & 1) != 0)
            gUnk_030023D4++;
        gCurTask->unk6C++;
    }
    gCurTask->unk43 = 1;
    gCurTask->unk04 = (u32)sub_0805b4bc;
    gUnk_02007D00[8] |= 1 << gCurTask->unk88->unk00;
    sub_0805b370();
    gCurTask->unk14 = 0;
    CallTableEntry(gCurTask->unk14, 11, gUnk_0873DBE4);
}

void sub_0805b354(void)
{
    CallTableEntry(gCurTask->unk14, 11, gUnk_0873DBE4);
}

void sub_0805b370(void)
{
    gCurTask->unk30 = 0;
    gCurTask->unk34 = 0;
    gUnk_02007D00[7] = 0;
    gUnk_02007D00[9] = 0;
    gCurTask->unk70 = 0xFFFF;
    if (gCurTask->unk2C == 0)
    {
        LZ77UnCompWram((const void *)gUnk_085B9B6C[3], gUnk_02020000);
        RequestCopy(4, gUnk_02020000, gObjVram, ((u16 *)gUnk_085B9B6C)[1] << 5);
        RequestCopy(2, (void *)gUnk_085B9B6C[2], gUnk_03001570, ((u16 *)gUnk_085B9B6C)[0] << 5);
        gCurTask->unk6C = 0;
        do
        {
            gUnk_02007D00[(s16)gCurTask->unk6C] = TaskCreateFrom(85, 32);
            (gTasks + gUnk_02007D00[(s16)gCurTask->unk6C])->unk73 = gCurTask->unk6C;
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 6);
        TaskCreateFrom(90, 32);
    }
    gCurTask->unk4C = gUnk_0873DBAC[gActivePlayerCount * 4 + gCurTask->unk2C] << 16;
    gCurTask->unk50 = 232 << 18;
    TaskCreateFrom(88, 32);
    gCurTask->unk28 = TaskCreateFrom(84, 32);
    (gTasks + gCurTask->unk28)->unk73 = gCurTask->unk88->unk00;
    if (gActivePlayerCount > 1)
    {
        gCurTask->unk46 = TaskCreateFrom(87, 32);
        (gTasks + gCurTask->unk46)->unk73 = gCurTask->unk88->unk00;
    }
}

s32 sub_0805b4bc(void)
{
    CallTableEntry(gCurTask->unk15, 11, gUnk_0873DC10);
}

void sub_0805b4d8(void)
{
    gCurTask->unk15 = 0;
    TaskStartFrameScriptId(0);
    gCurTask->unk60 = 128 << 7;
    TaskYieldTrampoline(27);
    gCurTask->unk14 = 1;
    sub_0805b514();
}

void sub_0805b508(void)
{
    TaskUpdateFrameScript();
}

void sub_0805b514(void)
{
    gCurTask->unk15 = 1;
    TaskStop();
    TaskStartFrameScriptId(1);
    TaskSleepForever();
}

void sub_0805b534(void)
{
    TaskUpdateFrameScript();
    if (sub_0805b5b0() == 0)
    {
        sub_0805c584();
        sub_0805be48();
        gCurTask->unk30++;
        if (gCurTask->unk30 > 35)
        {
            gCurTask->unk14 = 4;
            TaskSetEntry(sub_0805b354, gCurTaskIdx);
        }
    }
    else if (gUnk_030023D4 != 0)
    {
        gCurTask->unk14 = 2;
        TaskSetEntry(sub_0805b354, gCurTaskIdx);
    }
    else
    {
        gCurTask->unk14 = 3;
        TaskSetEntry(sub_0805b354, gCurTaskIdx);
    }
}

s32 sub_0805b5b0(void)
{
    u16 *p = (u16 *)gLatchedPressedKeys;
    s32 n;

    if ((p[gCurTask->unk88->unk00] & 3) != 0)
    {
        sub_0805b61c();
        n = gCurTask->unk30;
        if (n <= 24)
        {
            gCurTask->unk30 = ((24 - n) >> 1) + 24;
            gUnk_030023D4 = 1;
        }
        else
        {
            gUnk_030023D4 = 0;
        }
        gCurTask->unk70 = 0;
        return 1;
    }
    return 0;
}

void sub_0805b61c(void)
{
    if (gCurTask->unk30 > 24)
        gCurTask->unk34 = 24 - ((gCurTask->unk30 - 24) << 1);
    else
        gCurTask->unk34 = gCurTask->unk30;
    gCurTask->unk34++;
}

void sub_0805b644(void)
{
    gCurTask->unk15 = 2;
    TaskYieldTrampoline(4);
    sub_0805b670();
}

void sub_0805b660(void)
{
    TaskUpdateFrameScript();
    sub_0805c584();
}

void sub_0805b670(void)
{
    gCurTask->unk15 = 3;
    TaskSleepForever();
}

void sub_0805b688(void)
{
    TaskUpdateFrameScript();
    sub_0805c584();
    gCurTask->unk30++;
    if (gCurTask->unk30 > 35)
    {
        gCurTask->unk14 = 4;
        TaskSetEntry(sub_0805b354, gCurTaskIdx);
    }
}

void sub_0805b6c0(void)
{
    gCurTask->unk15 = 4;
    sub_0806d4e4(1, 0);
    gCurTask->unk30 = gUnk_0873DC80[gCurTask->unk34];
    sub_0805b83c();
    sub_0805b8b8();
    TaskStartFrameScriptId(3);
    TaskCreateFrom(81, 32);
    gCurTask->unk24 = 0;
    if (gCurTask->unk88->unk00 == gLocalPlayer)
    {
        if (gCurTask->unk30 <= 1)
            PlaySfx(244);
        else
            PlaySfx(227);
    }
    gCurTask->unk58 = 0xFFF80000;
    TaskYieldTrampoline(gUnk_0873DC4C[gCurTask->unk34]);
    gCurTask->unk60 = 128 << 7;
    TaskYieldTrampoline(32);
    if (gUnk_0873DC66[gCurTask->unk34] != 0)
    {
        gCurTask->unk14 = 5;
        sub_0805b8f8();
    }
    else
    {
        gCurTask->unk14 = 6;
        sub_0805b9a4();
    }
}

void sub_0805b788(void)
{
    gCurTask->unk70++;
    TaskUpdateFrameScript();
    gCurTask->unk24++;
    if (IsWorldPosOnScreen(gCurTask->unk48, gCurTask->unk4A) != 0)
        QueueSprite(14, gUnk_0873DC3C[gCurTask->unk24 & 3], 0, 0,
                     gCurTask->unk48 - gSpriteCameraX,
                     (s16)(gCurTask->unk4A - gSpriteCameraY + 16));
    if ((gCurTask->unk24 & 7) == 0)
        TaskCreateFrom(83, 32);
    if (gCurTask->unk24 > 31)
    {
        TaskCreateFrom(82, 32);
        gCurTask->unk24 = 0;
    }
}

void sub_0805b83c(void)
{
    gUnk_02006A14[gCurTask->unk88->unk00]--;
    if (gUnk_02006A14[gCurTask->unk88->unk00] != gCurTask->unk30)
    {
        gUnk_02006A14[gCurTask->unk88->unk00] = 7;
        if (gCurTask->unk30 == 6)
            gUnk_02006A14[gCurTask->unk88->unk00]--;
    }
    if (gUnk_02006A14[gCurTask->unk88->unk00] == 0)
        gUnk_02007D00[9] = 1;
}

void sub_0805b8b8(void)
{
    struct Task *t;

    t = &gTasks[gUnk_02007D00[gCurTask->unk30]];
    t->unk28 = 0;
    t->unk2C |= 1 << gCurTask->unk88->unk00;
}

void sub_0805b8f8(void)
{
    gCurTask->unk15 = 5;
    TaskStop();
    TaskStartFrameScriptId(4);
    gCurTask->unk4A -= 4;
    gCurTask->unk50 = gCurTask->unk4A << 16;
    TaskYieldTrampoline(60);
    gCurTask->unk4A -= 4;
    gCurTask->unk50 = gCurTask->unk4A << 16;
    TaskYieldTrampoline(4);
    gCurTask->unk4A -= 4;
    gCurTask->unk50 = gCurTask->unk4A << 16;
    TaskYieldTrampoline(8);
    gCurTask->unk4A -= 4;
    gCurTask->unk50 = gCurTask->unk4A << 16;
    TaskYieldTrampoline(12);
    gCurTask->unk58 = -163840;
    gCurTask->unk60 = 128 << 8;
    TaskYieldTrampoline(9);
    sub_0805ba08();
}

void sub_0805b998(void)
{
    TaskUpdateFrameScript();
}

void sub_0805b9a4(void)
{
    gCurTask->unk15 = 6;
    TaskStartFrameScriptId(0);
    TaskSleepForever();
}

void sub_0805b9c0(void)
{
    s32 v;
    s16 *tbl;

    TaskUpdateFrameScript();
    v = gCurTask->unk4A;
    tbl = (s16 *)gUnk_0873DBD4;
    if (v > tbl[gCurTask->unk30] - 2)
    {
        gCurTask->unk14 = 7;
        TaskSetEntry(sub_0805b354, gCurTaskIdx);
    }
}

void sub_0805ba08(void)
{
    gCurTask->unk15 = 7;
    TaskStop();
    gCurTask->unk50 = (gUnk_0873DBD4[gCurTask->unk30] - 2) << 16;
    TaskStartFrameScriptId(5);
    TaskYieldTrampoline(2);
    TaskStartFrameScriptId(6);
    TaskYieldTrampoline(32);
    gUnk_030023D4 = 0;
    gUnk_030023B4 = 0;
    if (gPlayerCount > 1)
    {
        gCurTask->unk6C = 0;
        while ((s16)gCurTask->unk6C < gPlayerCount)
        {
            if (((gActivePlayerMask >> (s16)gCurTask->unk6C) & 1) != 0
             && gTasks[(s16)gCurTask->unk6C].unk30 == gCurTask->unk30)
            {
                if ((s16)gCurTask->unk6C == gCurTaskIdx)
                    gUnk_030023B4 = gUnk_030023D4;
                gUnk_030023D4++;
            }
            gCurTask->unk6C++;
        }
        gCurTask->unk24 = ((s16 *)gUnk_0873DBAC)[gUnk_030023D4 * 4 + gUnk_030023B4];
        if (gCurTask->unk24 == gCurTask->unk48)
        {
            gCurTask->unk14 = 8;
            sub_0805bc1c();
        }
        if (gCurTask->unk48 < gCurTask->unk24 + gSpriteCameraX)
            gCurTask->unk43 = 1;
        else
            gCurTask->unk43 = -1;
        gCurTask->unk04 = (u32)sub_0805bb90;
        sub_08033d0c();
    }
    TaskStop();
    TaskStartFrameScriptId(6);
    sub_0805bca4();
    gCurTask->unk14 = 9;
    sub_0805bce0();
}

void sub_0805bb84(void)
{
    TaskUpdateFrameScript();
}

void sub_0805bb90(void)
{
    if (gCurTask->unk43 == 1)
    {
        if (gCurTask->unk48 >= gCurTask->unk24)
        {
            gCurTask->unk48 = gCurTask->unk24;
            gCurTask->unk4C = gCurTask->unk48 << 16;
            gCurTask->unk04 = (u32)sub_0805b4bc;
            gCurTask->unk14 = 8;
            TaskSetEntry(sub_0805b354, gCurTaskIdx);
        }
    }
    else
    {
        if (gCurTask->unk48 <= gCurTask->unk24)
        {
            gCurTask->unk48 = gCurTask->unk24;
            gCurTask->unk4C = gCurTask->unk48 << 16;
            gCurTask->unk04 = (u32)sub_0805b4bc;
            gCurTask->unk14 = 8;
            TaskSetEntry(sub_0805b354, gCurTaskIdx);
        }
    }
}

void sub_0805bc1c(void)
{
    gCurTask->unk15 = 8;
    gCurTask->unk08 = (u32)sub_0805bc5c;
    gCurTask->unk43 = 1;
    TaskStop();
    TaskStartFrameScriptId(6);
    sub_0805bca4();
    TaskSleepForever();
}

void sub_0805bc50(void)
{
    TaskUpdateFrameScript();
}

void sub_0805bc5c(void)
{
    struct Task *t;

    t = &gTasks[gUnk_02007D00[gCurTask->unk30]];
    if (t->unk2C == 0)
    {
        gCurTask->unk14 = 9;
        TaskSetEntry(sub_0805b354, gCurTaskIdx);
    }
}

void sub_0805bca4(void)
{
    struct Task *t;

    t = &gTasks[gUnk_02007D00[gCurTask->unk30]];
    t->unk2C &= ~(1 << gCurTask->unk88->unk00);
}

void sub_0805bce0(void)
{
    gCurTask->unk15 = 9;
    gCurTask->unk08 = 0;
    if (gCurTask->unk30 == 0)
        TaskYieldTrampoline(50);
    else
        TaskYieldTrampoline(30);
    TaskStartFrameScriptId(0);
    sub_0805e15c();
    gCurTask->unk14 = 10;
    sub_0805bd34();
}

void sub_0805bd28(void)
{
    TaskUpdateFrameScript();
}

void sub_0805bd34(void)
{
    gCurTask->unk04 = (u32)sub_0805b4bc;
    gCurTask->unk15 = 10;
    if ((gCurTask->unk3E & (128 << 8)) != 0)
        gCurTask->unk43 = -1;
    else
        gCurTask->unk43 = 1;
    TaskStartFrameScriptId(7);
    if (gCurTask->unk30 == 0)
        TaskYieldTrampoline(30);
    else if (gCurTask->unk30 != 6)
        sub_0800a04c(gUnk_0873DC9A[gCurTask->unk30],
                     gCurTask->unk88->unk00);
    if (gUnk_02006A14[gCurTask->unk88->unk00] == 0)
    {
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(60);
        TaskYieldTrampoline(30);
    }
    gUnk_02007D00[8] &= ~(1 << gCurTask->unk88->unk00);
    while (gUnk_02007D00[8] != 0)
        TaskYieldTrampoline(1);
    TaskYieldTrampoline(10);
    *(s8 *)gStageRequest = 1;
    TaskSleepForever();
}

void sub_0805be3c(void)
{
    TaskUpdateFrameScript();
}

void sub_0805be48(void)
{
    if (((gActivePlayerMask >> gLocalPlayer) & 1) != 0
     && gLocalPlayer == gCurTask->unk88->unk00
     && (gFrameCount & 4) != 0)
        QueueSprite(8, 0x085B9B2C, 0, 0x00009010, 120, 70);
}

void sub_0805beb0(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)sub_0805c150;
    gCurTask->unk04 = (u32)sub_0805c114;
    gCurTask->unk42 = 12;
    gCurTask->unk38 = gUnk_0874C890;
    gCurTask->unk40 = 0;
    TaskStop();
    gCurTask->unk48 = gTasks[gCurTask->unk44].unk48;
    gCurTask->unk4A = gTasks[gCurTask->unk44].unk4A + 32;
    gCurTask->unk4C = gCurTask->unk48 << 16;
    gCurTask->unk50 = gCurTask->unk4A << 16;
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 1;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 2;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 3;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 1;
    TaskYieldTrampoline(1);
    while (1)
    {
        gCurTask->unk48 = gTasks[gCurTask->unk44].unk48;
        gCurTask->unk4A = gTasks[gCurTask->unk44].unk4A + 32;
        gCurTask->unk4C = gCurTask->unk48 << 16;
        gCurTask->unk50 = gCurTask->unk4A << 16;
        gCurTask->unk3C = 6;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 7;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 4;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 5;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 6;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 7;
        TaskYieldTrampoline(1);
        gCurTask->unk48 = gTasks[gCurTask->unk44].unk48;
        gCurTask->unk4A = gTasks[gCurTask->unk44].unk4A + 32;
        gCurTask->unk4C = gCurTask->unk48 << 16;
        gCurTask->unk50 = gCurTask->unk4A << 16;
        gCurTask->unk3C = 8;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 9;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 10;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 11;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 8;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 9;
        TaskYieldTrampoline(1);
    }
}

void sub_0805c0a8(void)
{
    gCurTask->unk04 = 0;
    gCurTask->unk60 = 128 << 7;
    gCurTask->unk68 = 192 << 10;
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C = 10;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 11;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 8;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 9;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 9);
    TaskExitTrampoline();
}

void sub_0805c114(void)
{
    if (gTasks[gCurTask->unk44].unk60 != 0)
        TaskSetEntry(sub_0805c0a8, gCurTaskIdx);
}

void sub_0805c150(void)
{
    TaskDrawWorld();
    {
        s16 *tbl = (s16 *)gUnk_0873DCA8;

        if (tbl[gCurTask->unk3C] != -1
            && IsOnScreen(gCurTask->unk48 - gSpriteCameraX,
                            gCurTask->unk4A - gSpriteCameraY + 48))
        {
            struct Task *q = gCurTask;
            u32 *g = q->unk38;

            QueueSprite(q->unk42, g[tbl[q->unk3C]], q->unk3E, q->unk40,
                         q->unk48 - gSpriteCameraX,
                         (s16)(q->unk4A - gSpriteCameraY + 48));
        }
    }
}

void sub_0805c204(void)
{
    gCurTask->unk00 = (u32)ActorMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 11;
    gCurTask->unk38 = gUnk_0874C890;
    gCurTask->unk40 = 0;
    gCurTask->unk48 = gTasks[gCurTask->unk44].unk48
                         + ((s8 *)gUnk_0873DCC0)[RandomRange(8)];
    gCurTask->unk4A = gTasks[gCurTask->unk44].unk4A
                         + ((s8 *)gUnk_0873DCC8)[RandomRange(4)];
    gCurTask->unk4C = gCurTask->unk48 << 16;
    gCurTask->unk50 = gCurTask->unk4A << 16;
    TaskSetMotionY(gTasks[gCurTask->unk44].unk58 + (128 << 9),
                 128 << 7, 192 << 10);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C = 13;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 14;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 15;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 14;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C = 16;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 17;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 18;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 19;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C = 20;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 21;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 22;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 23;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C = 24;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 25;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 26;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 27;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskExitTrampoline();
}

void sub_0805c410(void)
{
    gCurTask->unk00 = (u32)ActorMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 13;
    gCurTask->unk38 = gUnk_0874C890;
    gCurTask->unk40 = 0;
    gCurTask->unk48 = gTasks[gCurTask->unk44].unk48
                         + ((s8 *)gUnk_0873DCC0)[RandomRange(8)];
    gCurTask->unk4A = gTasks[gCurTask->unk44].unk4A
                         + ((s8 *)gUnk_0873DCC8)[RandomRange(4)];
    gCurTask->unk4C = gCurTask->unk48 << 16;
    gCurTask->unk50 = gCurTask->unk4A << 16;
    TaskSetMotionY(gTasks[gCurTask->unk44].unk58 + (128 << 8),
                 128 << 7, 192 << 10);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C = 20;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 21;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 22;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 23;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    gCurTask->unk6C = 0;
    do
    {
        gCurTask->unk3C = 24;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 25;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 26;
        TaskYieldTrampoline(1);
        gCurTask->unk3C = 27;
        TaskYieldTrampoline(1);
        gCurTask->unk6C++;
    } while ((s16)gCurTask->unk6C <= 1);
    TaskExitTrampoline();
}

void sub_0805c584(void)
{
    struct Task *t = gCurTask;
    s16 *tbl = (s16 *)gUnk_0873DCCC;

    t->unk4A = 1024 + tbl[t->unk30];
    t->unk50 = t->unk4A << 16;
    (gTasks + t->unk28)->unk50 = (tbl[t->unk30] + 1048) << 16;
    if (t->unk30 == 24)
    {
        (gTasks + t->unk28)->unk3C = 2;
        (gTasks + t->unk28)->unk10 = 2;
    }
}

void sub_0805c5fc(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk04 = (u32)sub_0805c990;
    if ((gActivePlayerMask >> gLocalPlayer) & 1)
    {
        TaskStop();
        gCurTask->unk28 = -1;
        gCurTask->unk2C = 0;
        gCurTask->unk48 = 144;
        gCurTask->unk4A = gTasks[gLocalPlayer].unk4A;
        gCurTask->unk4C = gCurTask->unk48 << 16;
        gCurTask->unk50 = gCurTask->unk4A << 16;
        while (gTasks[gLocalPlayer].unk14 <= 3)
        {
            gCurTask->unk4A = gTasks[gLocalPlayer].unk4A;
            gCurTask->unk50 = gCurTask->unk4A << 16;
            TaskYieldTrampoline(1);
        }
        gCurTask->unk4A = gTasks[gLocalPlayer].unk4A;
        gCurTask->unk50 = gCurTask->unk4A << 16;
        TaskYieldTrampoline(1);
        gCurTask->unk58 = 0xFFF78000;
        {
            u8 *t1 = (u8 *)gUnk_0873DD16;

            TaskYieldTrampoline(t1[gTasks[gLocalPlayer].unk34]);
        }
        gCurTask->unk60 = 136 << 7;
        TaskYieldTrampoline(32);
        TaskStop();
        {
            u8 *t2 = (u8 *)gUnk_0873DD30;

            TaskYieldTrampoline(t2[gTasks[gLocalPlayer].unk34]);
        }
        gCurTask->unk60 = 128 << 7;
        while (gCurTask->unk4A
               < gUnk_0873DBD4[gTasks[gLocalPlayer].unk30] - 2)
            TaskYieldTrampoline(1);
        TaskStop();
        TaskSleepForever();
    }
    else
    {
        for (gUnk_030023D4 = 0; gUnk_030023D4 < gPlayerCount; gUnk_030023D4++)
        {
            if ((gActivePlayerMask >> gUnk_030023D4) & 1)
            {
                gCurTask->unk28 = gUnk_030023D4;
                break;
            }
        }
        gCurTask->unk2C = 0;
        sub_0805c814();
    }
}

void sub_0805c814(void)
{
    TaskStop();
    gCurTask->unk48 = 144;
    gCurTask->unk4A = gTasks[gCurTask->unk28].unk4A;
    gCurTask->unk4C = gCurTask->unk48 << 16;
    gCurTask->unk50 = gCurTask->unk4A << 16;
    while (gTasks[gCurTask->unk28].unk14 <= 3)
    {
        gCurTask->unk4A = gTasks[gCurTask->unk28].unk4A;
        gCurTask->unk50 = gCurTask->unk4A << 16;
        TaskYieldTrampoline(1);
    }
    gCurTask->unk4A = gTasks[gCurTask->unk28].unk4A;
    gCurTask->unk50 = gCurTask->unk4A << 16;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFF78000;
    {
        u8 *t1 = (u8 *)gUnk_0873DD16;

        TaskYieldTrampoline(t1[gTasks[gCurTask->unk28].unk34]
                            - (s16)gTasks[gCurTask->unk28].unk70 + 1);
    }
    gCurTask->unk60 = 136 << 7;
    TaskYieldTrampoline(32);
    TaskStop();
    {
        u8 *t2 = (u8 *)gUnk_0873DD30;

        TaskYieldTrampoline(t2[gTasks[gCurTask->unk28].unk34]);
    }
    gCurTask->unk60 = 128 << 7;
    while (gCurTask->unk4A
           < gUnk_0873DBD4[gTasks[gCurTask->unk28].unk30] - 2)
        TaskYieldTrampoline(1);
    TaskStop();
    TaskSleepForever();
}

void sub_0805c990(void)
{
    u16 x;
    u16 y;

    x = gCurTask->unk48;
    y = gCurTask->unk4A + gCurTask->unk2C;
    if ((s16)y > 0x41C)
        y = 0x41C;
    SetCameraFocus((s16)x, (s16)y);
    if (gCurTask->unk2C != 0)
    {
        if (gCurTask->unk2C > 0)
        {
            gCurTask->unk2C -= 4;
            if (gCurTask->unk2C < 0)
                gCurTask->unk2C = 0;
        }
        else
        {
            gCurTask->unk2C += 4;
            if (gCurTask->unk2C > 0)
                gCurTask->unk2C = 0;
        }
    }
    if (gCurTask->unk28 >= 0)
    {
        for (gUnk_030023D4 = 0; gUnk_030023D4 < gPlayerCount; gUnk_030023D4++)
        {
            if ((s16)gTasks[gCurTask->unk28].unk70 < 0)
            {
                if (((gActivePlayerMask >> gUnk_030023D4) & 1)
                    && gCurTask->unk28 != gUnk_030023D4
                    && (s16)gTasks[gUnk_030023D4].unk70 >= 0)
                {
                    gCurTask->unk2C = 0;
                    gCurTask->unk28 = gUnk_030023D4;
                    gCurTask->unk4A = gTasks[gUnk_030023D4].unk4A;
                    gCurTask->unk50 = gCurTask->unk4A << 16;
                    TaskSetEntry(sub_0805c814, gCurTaskIdx);
                }
            }
            else
            {
                if (((gActivePlayerMask >> gUnk_030023D4) & 1)
                    && gCurTask->unk28 != gUnk_030023D4
                    && (s16)gTasks[gUnk_030023D4].unk70 > 0
                    && gTasks[gCurTask->unk28].unk34
                           < gTasks[gUnk_030023D4].unk34)
                {
                    gCurTask->unk2C = gCurTask->unk4A + gCurTask->unk2C
                                         - gTasks[gUnk_030023D4].unk4A;
                    gCurTask->unk28 = gUnk_030023D4;
                    gCurTask->unk4A = gTasks[gUnk_030023D4].unk4A;
                    gCurTask->unk50 = gCurTask->unk4A << 16;
                    TaskSetEntry(sub_0805c814, gCurTaskIdx);
                }
            }
        }
    }
}

void sub_0805cb30(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 8;
    if (gActivePlayerCount == 1)
        gCurTask->unk38 = gUnk_08754850;
    else
        gCurTask->unk38 = (u32 *)gUnk_0873DD4C[gCurTask->unk73];
    gCurTask->unk40 = 0x00009010;
    {
        s16 *tbl = (s16 *)gUnk_0873DBAC;

        gCurTask->unk4C = tbl[(gActivePlayerCount << 2)
            + gTasks[gCurTask->unk44].unk2C] << 16;
    }
    gCurTask->unk50 = 131 << 19;
    while (1)
    {
        gCurTask->unk3C = 0;
        TaskYieldTrampoline(4);
        gCurTask->unk3C = 1;
        TaskYieldTrampoline(4);
    }
}

void sub_0805cbec(void)
{
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 11;
    gCurTask->unk38 = gUnk_0875488C;
    gCurTask->unk04 = (u32)sub_0805cc54;
    gCurTask->unk40 = 0x0000A010;
    if (gLocalPlayer == gCurTask->unk73)
    {
        gCurTask->unk3C = 4;
    }
    else
    {
        u16 *tbl = (u16 *)gUnk_0873DD5C;
        gCurTask->unk3C = tbl[gCurTask->unk73];
    }
    TaskYieldTrampoline(27);
    TaskExitTrampoline();
}

s32 sub_0805cc54(void)
{
    gCurTask->unk48 = (gTasks + gCurTask->unk44)->unk48;
    gCurTask->unk4A = (gTasks + gCurTask->unk44)->unk4A - 24;
}

void sub_0805cca0(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 9;
    gCurTask->unk38 = (u32 *)gUnk_0873DD64[gCurTask->unk73];
    gCurTask->unk4C = 200 << 16;
    {
        s16 *t = (s16 *)gUnk_0873DBD4;

        gCurTask->unk50 = (t[gCurTask->unk73] - 8) << 16;
    }
    gCurTask->unk40 = 0x00008010;
    gCurTask->unk28 = 1;
    gCurTask->unk2C = 0;
    gCurTask->unk3C = 0;
    do
    {
        TaskYieldTrampoline(1);
    } while (gCurTask->unk28 != 0 || gCurTask->unk2C != 0);
    gCurTask->unk46 = TaskCreateFrom(86, 32);
    (gTasks + gCurTask->unk46)->unk74 = gCurTask->unk73;
    (gTasks + gCurTask->unk46)->unk28 = gUnk_02007D00[7];
    gUnk_02007D00[7]++;
    if (gCurTask->unk73 == 0)
    {
        TaskYieldTrampoline(24);
        gCurTask->unk04 = (u32)sub_0805ceec;
        gCurTask->unk28 = 1;
        do
        {
            TaskYieldTrampoline(1);
        } while (gCurTask->unk28 != 0);
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk3C = 1;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 2;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 3;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 4;
            TaskYieldTrampoline(24);
            gCurTask->unk3C = 3;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 2;
            TaskYieldTrampoline(1);
            gCurTask->unk3C = 1;
            TaskYieldTrampoline(1);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 2);
        gCurTask->unk3C = 1;
        TaskYieldTrampoline(3);
        gCurTask->unk3C = 2;
        TaskYieldTrampoline(3);
        gCurTask->unk3C = 3;
        TaskYieldTrampoline(3);
        gCurTask->unk3C = 4;
        TaskSleepForever();
    }
    else
    {
        gCurTask->unk28 = 1;
        do
        {
            TaskYieldTrampoline(1);
        } while (gCurTask->unk28 != 0);
        gCurTask->unk58 = 0xFFF8CD00;
        TaskYieldTrampoline(3);
        gCurTask->unk58 = 0xFFFB3300;
        TaskYieldTrampoline(3);
        gCurTask->unk58 = 0xFFFD9A00;
        TaskYieldTrampoline(3);
        gCurTask->unk58 = 0x00026600;
        TaskYieldTrampoline(3);
        gCurTask->unk58 = 0x0004CD00;
        TaskYieldTrampoline(5);
        TaskStop();
        TaskYieldTrampoline(27);
        gCurTask->unk3C = 1;
        TaskYieldTrampoline(5);
        gCurTask->unk3C = 2;
        TaskYieldTrampoline(5);
        gCurTask->unk3C = 3;
        TaskYieldTrampoline(5);
        gCurTask->unk3C = 4;
    }
    TaskSleepForever();
}

void sub_0805ceec(void)
{
    gCurTask->unk48 = (gTasks + gCurTask->unk46)->unk48 - 4;
    gCurTask->unk4A = (gTasks + gCurTask->unk46)->unk4A - 16;
}

void sub_0805cf3c(void)
{
    gCurTask->unk04 = (u32)sub_0805d420;
    gCurTask->unk4C = 248 << 16;
    gCurTask->unk43 = 255;
    gCurTask->unk2C = 0;
    gCurTask->unk30 = 0;
    gCurTask->unk24 = 0;
    if (gCurTask->unk74 == 0)
        gCurTask->unk73 = 1;
    else
        gCurTask->unk73 = 0;
    sub_0805d994(gCurTask->unk28, gCurTask->unk73);
    gCurTask->unk42 = 8;
    switch (gCurTask->unk73)
    {
    case 1:
        if (((gActivePlayerMask >> gLocalPlayer) & 1)
            && (gTasks + gLocalPlayer)->unk30 != 0)
            gCurTask->unk0C = 0;
        gCurTask->unk4C = 244 << 16;
        gCurTask->unk50 = 196 << 16;
        TaskStartFrameScript((s32)gUnk_0873DDB4);
        gCurTask->unk54 = 0xFFFB3300;
        gCurTask->unk58 = 0xFFFC6600;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0xFFFD9A00;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0xFFFECD00;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0xFFFF6600;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0x00009A00;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0x00013300;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0x00026600;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0x0004CD00;
        TaskYieldTrampoline(5);
        gCurTask->unk58 = 0;
        TaskYieldTrampoline(5);
        TaskStop();
        TaskYieldTrampoline(53);
        TaskStartFrameScript((s32)gUnk_0873DDA2);
        gCurTask->unk54 = 0xFFFD6000;
        TaskYieldTrampoline(60);
        TaskStartFrameScript((s32)gUnk_0873DD80);
        TaskStop();
        (gTasks + gCurTask->unk44)->unk28 = 0;
        gCurTask->unk6C = 0;
        do
        {
            gCurTask->unk58 = 0xFFFECD00;
            TaskYieldTrampoline(3);
            gCurTask->unk58 = 0xFFFF6600;
            TaskYieldTrampoline(3);
            gCurTask->unk58 = 0x00009A00;
            TaskYieldTrampoline(3);
            gCurTask->unk58 = 0x00013300;
            TaskYieldTrampoline(3);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 7);
        gCurTask->unk58 = 0;
        TaskStartFrameScript((s32)gUnk_0873DDA2);
        gCurTask->unk54 = 0x00009A00;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0x00013300;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0x00026600;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0x0004CD00;
        TaskYieldTrampoline(10);
        gCurTask->unk54 = 0x0004CD00;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0x00026600;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0x00013300;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0x00009A00;
        TaskYieldTrampoline(5);
        for (gCurTask->unk6C = 0;
             (s16)gCurTask->unk6C < gPlayerCount;
             gCurTask->unk6C++)
        {
            if (((gActivePlayerMask >> (s16)gCurTask->unk6C) & 1)
                && (gTasks + (s16)gCurTask->unk6C)->unk30 == 0)
            {
                gCurTask->unk2C |= 1 << (s16)gCurTask->unk6C;
                gCurTask->unk30++;
            }
        }
        gCurTask->unk34 = gCurTask->unk30 - 1;
        gCurTask->unk54 = 0xFFFF6600;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0xFFFECD00;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0xFFFD9A00;
        TaskYieldTrampoline(5);
        gCurTask->unk54 = 0xFFFB3300;
        TaskYieldTrampoline(60);
        TaskStop();
        if (gUnk_02007D00[9] != 0)
        {
            gCurTask->unk43 = 1;
            gCurTask->unk54 = 192 << 10;
            TaskYieldTrampoline(36);
            gCurTask->unk54 = 128 << 10;
            TaskYieldTrampoline(8);
            gCurTask->unk54 = 128 << 9;
            TaskYieldTrampoline(8);
            gCurTask->unk54 = 128 << 8;
            TaskYieldTrampoline(8);
            TaskStop();
            gCurTask->unk6C = 0;
            do
            {
                for (gCurTask->unk6E = 0;
                     gCurTask->unk6E < gPlayerCount;
                     gCurTask->unk6E++)
                {
                    if (((gActivePlayerMask >> gCurTask->unk6E) & 1)
                        && ((u8 *)gUnk_02006A14)[gCurTask->unk6E] == 0)
                    {
                        gCurTask->unk46 = TaskCreateFrom(89, 32);
                        (gTasks + gCurTask->unk46)->unk2C
                            = gCurTask->unk6E;
                        (gTasks + gCurTask->unk46)->unk73 = 1;
                    }
                }
                TaskYieldTrampoline(11);
                gCurTask->unk6C++;
            } while ((s16)gCurTask->unk6C <= 29);
        }
        TaskSleepForever();
        break;
    case 0:
        gCurTask->unk4C = 240 << 16;
        {
            s16 *t = (s16 *)gUnk_0873DBD4;

            gCurTask->unk50 = (t[gCurTask->unk74] - 2) << 16;
        }
        TaskStartFrameScript((s32)gUnk_0873DDBE);
        gCurTask->unk54 = 0xFFFE0000;
        TaskYieldTrampoline(23);
        TaskStop();
        TaskStartFrameScript(0);
        gCurTask->unk43 = 1;
        gCurTask->unk6C = 0;
        do
        {
            TaskSetFrame(165);
            TaskYieldTrampoline(4);
            TaskSetFrame(166);
            TaskYieldTrampoline(4);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 3);
        TaskSetFrame(167);
        TaskYieldTrampoline(4);
        (gTasks + gCurTask->unk44)->unk28 = 0;
        TaskSetFrame(165);
        TaskYieldTrampoline(2);
        TaskSetFrame(168);
        TaskYieldTrampoline(6);
        TaskSetFrame(169);
        TaskYieldTrampoline(4);
        TaskSetFrame(170);
        TaskYieldTrampoline(4);
        TaskSetFrame(171);
        TaskSleepForever();
        break;
    }
}

void sub_0805d420(void)
{
    TaskUpdateFrameScript();
    if (gCurTask->unk2C == 0)
        return;
    for (gCurTask->unk6C = gPlayerCount - 1;
         (s16)gCurTask->unk6C >= 0;
         gCurTask->unk6C--)
    {
        if (((gActivePlayerMask >> (s16)gCurTask->unk6C) & 1)
            && ((gCurTask->unk2C >> (s16)gCurTask->unk6C) & 1)
            && gCurTask->unk48 < gUnk_0873DBAC[(gCurTask->unk30 << 2) + gCurTask->unk34] - 6)
        {
            if (gUnk_02006A14[(s16)gCurTask->unk6C] != 0)
            {
                gCurTask->unk46 = TaskCreateFrom(89, 32);
                (gTasks + gCurTask->unk46)->unk2C = (s16)gCurTask->unk6C;
                (gTasks + gCurTask->unk46)->unk30 = gCurTask->unk30;
                (gTasks + gCurTask->unk46)->unk34 = gCurTask->unk34;
                (gTasks + gCurTask->unk46)->unk73 = 0;
            }
            gCurTask->unk2C &= ~(1 << (s16)gCurTask->unk6C);
            gCurTask->unk34--;
        }
    }
    gCurTask->unk24++;
}

void sub_0805d564(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)sub_0805d5fc;
    gCurTask->unk42 = 15;
    gCurTask->unk38 = gUnk_087548A0;
    gCurTask->unk40 = 0x0000B010;
    gCurTask->unk4C = (gTasks + gCurTask->unk44)->unk48 << 16;
    gCurTask->unk50 = (gTasks + gCurTask->unk44)->unk4A << 16;
    gCurTask->unk3C = 0;
    while (1)
    {
        gCurTask->unk6C = 0;
        do
        {
            TaskYieldTrampoline(3);
            gCurTask->unk6C++;
        } while ((s16)gCurTask->unk6C <= 3);
    }
}

void sub_0805d5fc(void)
{
    TaskDrawWorld();
    if (TaskIsOnScreen() != 0)
    {
        u32 *tbl = gUnk_0874CDF8;
        struct Task *t = gCurTask;

        QueueSprite(14, tbl[(s16)t->unk6C], 0, 0,
                     t->unk48 - gSpriteCameraX,
                     (s16)(t->unk4A - gSpriteCameraY));
    }
}

void sub_0805d668(void)
{
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)TaskDrawWorld;
    gCurTask->unk42 = 10;
    gCurTask->unk38 = gUnk_0874CCF4;
    gCurTask->unk40 = 240 << 8;
    gCurTask->unk3C = 4;
    switch (gCurTask->unk73)
    {
    case 0:
        gCurTask->unk4C = (gUnk_0873DBAC[(gCurTask->unk30 << 2)
            + gCurTask->unk34] - 6) << 16;
        gCurTask->unk50 = (gTasks + gCurTask->unk44)->unk4A << 16;
        gCurTask->unk24 = 44 - gTasks[gCurTask->unk44].unk24;
        if (gCurTask->unk24 & 1)
            TaskYieldTrampoline(1);
        gCurTask->unk58 = 0xFFFE0000;
        TaskYieldTrampoline(gCurTask->unk24 >> 1);
        gCurTask->unk54 = 128 << 8;
        gCurTask->unk58 = 0xFFFE0000;
        TaskYieldTrampoline(4);
        gCurTask->unk58 = 0xFFFF0000;
        TaskYieldTrampoline(4);
        gCurTask->unk58 = 0xFFFF8000;
        TaskYieldTrampoline(4);
        TaskStop();
        TaskYieldTrampoline(5);
        gCurTask->unk58 = 128 << 9;
        TaskYieldTrampoline(8);
        gCurTask->unk58 = 128 << 10;
        TaskYieldTrampoline(gCurTask->unk24 >> 1);
        TaskYieldTrampoline(22);
        break;
    case 1:
        gCurTask->unk4C = gTasks[gCurTask->unk44].unk48 << 16;
        gCurTask->unk50 = gTasks[gCurTask->unk44].unk4A << 16;
        gCurTask->unk54 = (gTasks[gCurTask->unk2C].unk48
            - gTasks[gCurTask->unk44].unk48) << 11;
        gCurTask->unk58 = 0xFFFC0000;
        TaskYieldTrampoline(4);
        gCurTask->unk58 = 0xFFFE0000;
        TaskYieldTrampoline(4);
        gCurTask->unk58 = 0xFFFF0000;
        TaskYieldTrampoline(4);
        gCurTask->unk58 = 0xFFFF8000;
        TaskYieldTrampoline(4);
        gCurTask->unk58 = 128 << 8;
        TaskYieldTrampoline(4);
        gCurTask->unk58 = 128 << 9;
        TaskYieldTrampoline(4);
        gCurTask->unk58 = 128 << 10;
        TaskYieldTrampoline(4);
        gCurTask->unk58 = 128 << 11;
        TaskYieldTrampoline(4);
        TaskStopX();
        TaskYieldTrampoline(8);
        break;
    }
    if (gCurTask->unk2C == gLocalPlayer)
        PlaySfx(220);
    sub_08009e60(1, gCurTask->unk2C);
    TaskExitTrampoline();
}

void TaskStartFrameScript(s32 a0)
{
    gCurTask->unk18 = a0;
    gCurTask->unk1C = 0;
    gCurTask->unk20 = 0;
    if (a0 != 0)
        TaskAdvanceFrameScript();
}

void TaskStartFrameScriptId(s32 a0)
{
    u32 i = (u8)a0;

    if (i > 7)
        while (1) {}
    gCurTask->unk18 = *(gUnk_0873DDE8 + i);
    gCurTask->unk1C = 0;
    gCurTask->unk20 = 0;
    TaskAdvanceFrameScript();
}

void TaskUpdateFrameScript(void)
{
    if (gCurTask->unk18 != 0)
    {
        if (--gCurTask->unk20 <= 0)
            TaskAdvanceFrameScript();
    }
}

void TaskAdvanceFrameScript(void)
{
    struct Task *t;
    s32 i;
    s16 *p;
    s32 c;
    s32 j;
    struct Task *t2;
    s32 k;
    s16 *q;
    s32 v;
    struct Task *t3;
    s32 m;
    s16 *r;

top:
    t = gCurTask;
    i = t->unk1C;
    p = (s16 *)t->unk18;
    c = p[i];
    switch (c)
    {
    case -2:
        return;
    case -3:
        t->unk1C = 0;
        goto top;
    case -4:
        j = i + 1;
        t->unk1C = j;
        TaskStartFrameScriptId(*(u8 *)&p[j]);
        return;
    }
    t2 = gCurTask;
    k = t2->unk1C;
    q = (s16 *)t2->unk18;
    v = q[k];
    k++;
    t2->unk1C = k;
    TaskSetFrame(v);
    t3 = gCurTask;
    m = t3->unk1C;
    r = (s16 *)t3->unk18;
    t3->unk20 = r[m];
    m++;
    t3->unk1C = m;
}

void sub_0805d994(s32 a0, s32 a1)
{
    if (a1 == 0)
    {
        if (a0 <= 1)
            gCurTask->unk40 = ((a0 << 3) + 768) | -16368;
        else
            gCurTask->unk40 = ((a0 << 3) + 896) | -16368;
    }
    else
    {
        if (a0 <= 1)
            gCurTask->unk40 = ((a0 << 3) + 768) | -12272;
        else
            gCurTask->unk40 = ((a0 << 3) + 896) | -12272;
    }
    gCurTask->unk00 = (u32)TaskMove;
    gCurTask->unk0C = (u32)sub_0805da2c;
    gCurTask->unk42 = 7;
    gCurTask->unk38 = gUnk_0874CFEC;
}

void sub_0805da2c(void)
{
    struct Task *t;
    struct Task *s;
    struct Task *u;
    struct TaskGfx *g;
    u32 *tbl;
    u16 *p;
    u8 *dst;
    u16 a;
    u16 **pal;

    t = gCurTask;
    if (t->unk38 == NULL)
        return;
    if (t->unk3C == -1)
        return;
    if (TaskIsOnScreen() == 0)
        return;
    s = gCurTask;
    a = s->unk40;
    tbl = s->unk38;
    g = (struct TaskGfx *)tbl[s->unk3C];
    if ((g->unk00 & 1) != 0)
    {
        pal = &g->unk04;
        if (g->unk04 != NULL)
            RequestCopy(2, g->unk04 + 1, gObjPalette + ((a >> 12) << 5), *g->unk04);
        p = pal[1];
        dst = (u8 *)(((a & 0x7FF) << 5) + 0x0600FE00);
        while (*p != 0xFFFF)
        {
            u16 *q = p + 1;
            RequestCopy(4, q, dst, *p);
            p = (u16 *)((u8 *)q + *p);
            dst += 0x400;
        }
    }
    else
    {
        if (*g->unk04 != 0)
            RequestCopy(2, g->unk04 + 1, gObjPalette + ((a >> 12) << 5), *g->unk04);
        p = g->unk08;
        dst = (u8 *)(((a & 0x7FF) << 5) + 0x0600FE00);
        while (*p != 0xFFFF)
        {
            u16 *q = p + 1;
            RequestCopy(4, q, dst, *p);
            p = (u16 *)((u8 *)q + *p);
            dst += 0x400;
        }
    }
    u = gCurTask;
    QueueSprite(u->unk42, g->unk00 & ~1, u->unk3E, 0x800 | u->unk40,
                 u->unk48 - gSpriteCameraX, (s16)(u->unk4A - gSpriteCameraY));
}

void sub_0805dba0(void)
{
    struct Task *t;
    struct Task *u;

    t = gCurTask;
    t->unk04 = 0;
    t->unk08 = 0;
    t->unk00 = (u32)TaskMove;
    t->unk38 = gUnk_0874CFEC;
    TaskStop();
    u = gCurTask;
    u->unk3E &= 0x7FFF;
    u->unk88->unk42 &= 0xFFEF;
    CallTableEntry(u->unk14, 2, gUnk_0873DEA0);
}

void sub_0805dbfc(void)
{
    CallTableEntry(gCurTask->unk14, 2, gUnk_0873DEA0);
}

/* NOTE: needs hdr.c corrected to "extern u16 gUnk_0873DEA8[];" (stride 2, ldrh, symbol-first) */
void sub_0805dc18(void)
{
    struct Task *t;
    struct Task *u;
    struct Task *v;
    struct Task *w;
    struct Task *x;

    t = gCurTask;
    t->unk04 = (u32)sub_0805dd4c;
    t->unk43 = 1;
    u = gCurTask;
    u->unk3E &= 0x7FFF;
    switch ((s8)u->unk88->unk0D)
    {
    case 0:
    case 7:
    case 20:
    case 21:
    case 24:
        v = gCurTask;
        v->unk3E |= 128 << 8;
        break;
    case 1:
    case 2:
    case 5:
    case 19:
        while (1)
        {
            w = gCurTask;
            w->unk3C = gUnk_0873DEA8[(s8)w->unk88->unk0D];
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
        }
    }
    x = gCurTask;
    x->unk3C = gUnk_0873DEA8[(s8)x->unk88->unk0D];
    TaskSleepForever();
}

void sub_0805dd4c(void)
{
    if (gActivePlayerCount == 1)
    {
        if ((s16)gTaskSlotTypes[gCurTask->unk46] == -1)
            sub_0805deac();
    }
}

void sub_0805dd88(void)
{
    gCurTask->unk04 = (u32)sub_0805dd4c;
    gCurTask->unk43 = 1;
    TaskSetFrameFlip(146);
    TaskSleepForever();
}

void sub_0805ddb0(s32 a0)
{
    struct Task *t;

    t = &gTasks[a0];
    t->unk46 = CreatePlayerEffectHighSlot((s8)a0, 19, 0);
    gTasks[t->unk46].unk44 = a0;
    gTasks[t->unk46].unk88 = t->unk88;
    switch ((s8)gPlayerStates[a0].unk0D)
    {
    case 0:
    case 7:
    case 11:
    case 20:
    case 21:
        break;
    default:
        gTasks[CreatePlayerEffect((s8)a0, 17, 0)].unk44 = a0;
        break;
    }
    gPlayerStates[a0].unk0D = 0;
    gPlayerStates[a0].unk04 = 22;
}

void sub_0805deac(void)
{
    s32 i;

    for (i = 0; i < gPlayerCount; i++)
    {
        if (((gActivePlayerMask >> i) & 1) != 0)
        {
            if (gPlayerCount != 1)
            {
                switch ((s8)gPlayerStates[i].unk0D)
                {
                case 0:
                case 7:
                case 11:
                case 20:
                case 21:
                    break;
                default:
                    gTasks[CreatePlayerEffect((s8)i, 17, 0)].unk44 = i;
                    break;
                }
                gPlayerStates[i].unk0D = 0;
                gPlayerStates[i].unk04 = 22;
            }
            TaskSetEntry(sub_0805e15c, i);
        }
    }
}

void sub_0805df9c(void)
{
    if (gPlayerCount != 1)
        gCurTask->unk14 = 0;
    else
        gCurTask->unk14 = gPlayerCount;
    TaskSetEntry(sub_0805dba0, gCurTaskIdx);
    gCurTask->unk88->unk16 = 2;
    TaskStop();
}

void sub_0805dfe8(void)
{
    struct Task *t;
    u16 d;

    t = gCurTask;
    d = t->unk48 - gSpriteCameraX;
    if (t->unk43 == 1)
    {
        if ((s16)d >= t->unk18)
            sub_0805df9c();
    }
    else
    {
        if ((s16)d <= t->unk18)
            sub_0805df9c();
    }
}

void sub_0805e038(s32 a0)
{
    struct PlayerState *ps;
    struct Task *t;
    u16 v;
    s32 w;

    ps = &gPlayerStates[a0];
    t = &gTasks[a0];
    switch (gPlayerCount)
    {
    case 1:
        v = (ps->unk00 + 1) * 104 + 24;
        break;
    case 2:
        v = (ps->unk00 + 1) * 69 + 24;
        break;
    case 3:
        v = (ps->unk00 + 1) * 52 + 24;
        break;
    case 4:
        v = (ps->unk00 + 1) * 41 + 24;
        break;
    }
    w = (s16)v;
    t->unk18 = w;
    if (gUnk_0300244C == 0)
    {
        if (t->unk48 - w <= 0)
            t->unk43 = 1;
        else
            t->unk43 = 255;
    }
    else
    {
        if (t->unk48 - gSpriteCameraX - w <= 0)
            t->unk43 = 1;
        else
            t->unk43 = 255;
    }
}

void sub_0805e110(s32 a0)
{
    struct PlayerState *ps;
    struct Task *t;

    ps = &gPlayerStates[a0];
    t = &gTasks[a0];
    sub_08068a8c(a0, 1);
    sub_0805e038(a0);
    TaskSetEntry(sub_08033d0c, a0);
    ps->unk16 = 1;
    t->unk04 = (u32)sub_0805dfe8;
}

void sub_0805e15c(void)
{
    struct PlayerState *ps;
    struct Task *t;

    ps = gCurTask->unk88;
    ps->unk05 = ps->unk04;
    gCurTask->unk88->unk04 = 22;
    t = gCurTask;
    t->unk04 = 0;
    t->unk08 = 0;
    t->unk88->unk64 = 0;
    t->unk88->unk68 = 0;
    t->unk88->unk6C = 0;
    t->unk64 = 128 << 24;
    t->unk2C = t->unk50;
    t->unk28 = t->unk4C;
    if (((u8 *)gUnk_02000020)[0] == 1)
    {
        sub_0805e1bc();
    }
    else
    {
        sub_0805e24c();
        TaskSleepForever();
    }
}

void sub_0805e1bc(void)
{
    struct Task *t;

    if (*(s8 *)gUnk_02008010 < 0)
        *(s8 *)gUnk_02008010 = RandomRange(7);
    t = gCurTask;
    if (t->unk30 != 0)
    {
        if (gLocalPlayer == t->unk88->unk00)
            PlayBgm(14);
        CallTableEntry(*(s8 *)gUnk_02008010, 14, gUnk_0873DEDC);
    }
    else
    {
        if (gLocalPlayer == t->unk88->unk00)
            PlayBgm(13);
        CallTableEntry(*(s8 *)gUnk_02008010 + 7, 14, gUnk_0873DEDC);
    }
}

void sub_0805e24c(void)
{
    TaskSetFrameFlip(146);
    TaskYieldTrampoline(30);
    if (*(s8 *)gUnk_02008010 < 0)
    {
        *(s8 *)gUnk_02008010 = RandomRange(7) + 7;
        gCurTask->unk34 = 1;
    }
    else
    {
        gCurTask->unk34 = 0;
    }
    if (*(u8 *)gUnk_020060CC == 0)
    {
        PlayBgm(13);
        *(u8 *)gUnk_020060CC = 1;
    }
    CallTableEntry(*(s8 *)gUnk_02008010, 14, gUnk_0873DEDC);
    TaskYieldTrampoline(60);
    if (gCurTask->unk34 != 0)
        sub_080258e0();
}

void sub_0805e2d4(void)
{
    TaskSetMotion(128 << 9, 0, 0x5A5A5A5A, 144 << 10, 0, 0x5A5A5A5A);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 147 << 1;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFD8000;
    gCurTask->unk60 = 128 << 7;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000135;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(4);
    gCurTask->unk3C--;
    TaskYieldTrampoline(4);
    TaskSetMotion(0xFFFF0000, 0, 0x5A5A5A5A, 0xFFFD4000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0xFFFB0000, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFC1000;
    gCurTask->unk60 = 192 << 6;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x25;
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 0x2F;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x2C;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x27;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x21;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x25;
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x2F;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x2C;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x27;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x21;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(1);
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 9, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFF8800;
    gCurTask->unk5C = 192 << 5;
    gCurTask->unk3C = 0x66;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 240 << 7;
    gCurTask->unk5C = 0xFFFFE800;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 7, 0, 0x5A5A5A5A, 242 << 7, 0xFFFFF500, 0x5A5A5A5A);
    gCurTask->unk3C = 0x7B;
    TaskYieldTrampoline(4);
    gCurTask->unk54 = 0;
    TaskYieldTrampoline(15);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->unk3E &= 0x7FFF;
    CreatePlayerEffect(gCurTask->unk88->unk00, 16, 0);
    gCurTask->unk3C = 0x92;
    TaskYieldTrampoline(0x15);
}

void sub_0805e7b4(void)
{
    gCurTask->unk58 = 144 << 10;
    gCurTask->unk60 = 0xFFFF8000;
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(4);
    gCurTask->unk3C = 9;
    TaskYieldTrampoline(4);
    TaskSetMotion(152 << 8, 0, 0x5A5A5A5A, 0xFFFBE000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3C = 4;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(10);
    gCurTask->unk58 = 0xFFF8E000;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFF2000;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 188 << 11;
    gCurTask->unk3E &= 0x7FFF;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 128 << 6;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(12);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(4);
    TaskYieldTrampoline(2);
    gCurTask->unk3E |= 128 << 8;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFC8000;
    gCurTask->unk3C = 97;
    TaskYieldTrampoline(8);
    gCurTask->unk5C = 160 << 8;
    TaskYieldTrampoline(4);
    gCurTask->unk54 = 0xFFFF8000;
    gCurTask->unk5C = 0;
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    gCurTask->unk3E &= 0x7FFF;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 128 << 10;
    gCurTask->unk3C = 97;
    TaskYieldTrampoline(8);
    gCurTask->unk5C = 0xFFFF6000;
    TaskYieldTrampoline(4);
    gCurTask->unk54 = 0xFFFF8000;
    gCurTask->unk5C = 0;
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0xFFFE0000, 128 << 9, 0x5A5A5A5A);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(3);
    TaskStop();
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 192 << 8;
    gCurTask->unk5C = 0xFFFFD000;
    gCurTask->unk3C = 102;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 0xFFFE8000;
    gCurTask->unk5C = 192 << 5;
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 17;
    TaskYieldTrampoline(2);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0xFFFEA000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(10);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0xFFFE6000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(12);
    TaskSetMotion(128 << 7, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(7);
    TaskStop();
    gCurTask->unk3E |= 128 << 8;
    CreatePlayerEffect(gCurTask->unk88->unk00, 16, 0);
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(21);
}

void sub_0805eb2c(s32 a0, s32 a1, s32 a2)
{
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0xFFFD4000, 128 << 8, 0x5A5A5A5A);
    gCurTask->unk3C = 0x133;
    TaskYieldTrampoline(10);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 144 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gCurTask->unk3C = 123;
    TaskYieldTrampoline(8);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(3);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0xFFFD4000, 128 << 8, 0x5A5A5A5A);
    gCurTask->unk3C = 0x133;
    TaskYieldTrampoline(10);
    gCurTask->unk3E |= 128 << 8;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 144 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gCurTask->unk3C = 123;
    TaskYieldTrampoline(8);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(3);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk54 = 0xFFF80000;
    gCurTask->unk5C = 128 << 9;
    gCurTask->unk3C = 100;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0;
    TaskYieldTrampoline(2);
    TaskSetMotion(192 << 12, 0xFFFFD000, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 30;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 180 << 10;
    TaskYieldTrampoline(15);
    gCurTask->unk54 = 128 << 7;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF0000, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 32;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0xFFFE0000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(15);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(4);
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0xFFFE0000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(15);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(4);
    TaskSetMotion(240 << 8, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 32;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->unk3E |= 128 << 8;
    CreatePlayerEffect(gCurTask->unk88->unk00, 16, 0);
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(21);
}

void sub_0805ee90(void)
{
    gCurTask->unk58 = 144 << 9;
    gCurTask->unk60 = 0xFFFFC000;
    gCurTask->unk3C = 150;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0xFFFC8000;
    gCurTask->unk60 = 128 << 7;
    gCurTask->unk3C = 0x133;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 151;
    TaskYieldTrampoline(12);
    gCurTask->unk3C = 0x133;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 144 << 9;
    gCurTask->unk60 = 0xFFFFC000;
    gCurTask->unk3C = 150;
    TaskYieldTrampoline(4);
    gCurTask->unk58 = 0xFFFC8000;
    gCurTask->unk60 = 128 << 7;
    gCurTask->unk3C = 0x133;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 151;
    TaskYieldTrampoline(12);
    gCurTask->unk60 = 192 << 6;
    gCurTask->unk3C = 0x133;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 208 << 8;
    gCurTask->unk60 = 128 << 6;
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 192 << 11;
    gCurTask->unk60 = 0;
    gCurTask->unk3C = 310;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFB2000;
    gCurTask->unk60 = 0xFFFFC000;
    gCurTask->unk3C = 0x133 - 8;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 224 << 8;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 0xFFFDA000;
    gCurTask->unk60 = 128 << 7;
    gCurTask->unk3C = 151 + 161;
    TaskYieldTrampoline(14);
    gCurTask->unk3C = 0x133 - 8;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 192 << 11;
    gCurTask->unk60 = 0;
    gCurTask->unk3C = 310;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFB2000;
    gCurTask->unk60 = 0xFFFF8000;
    gCurTask->unk3C = 0x133 - 8;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 224 << 8;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 0xFFFBC000;
    gCurTask->unk60 = 128 << 7;
    gCurTask->unk3C = 151 + 161;
    TaskYieldTrampoline(14);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 0x139;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(8);
    gCurTask->unk3C = 106;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 128 << 10;
    gCurTask->unk60 = 0xFFFF0000;
    TaskYieldTrampoline(3);
    TaskStopY();
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFEA000;
    gCurTask->unk5C = 128 << 6;
    TaskYieldTrampoline(4);
    gCurTask->unk3E &= 0x7FFF;
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 17;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->unk3E |= 128 << 8;
    CreatePlayerEffect(gCurTask->unk88->unk00, 16, 0);
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(21);
}

void sub_0805f1bc(void)
{
    gCurTask->unk54 = 128 << 9;
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 20;
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 128 << 8;
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 128 << 7;
    gCurTask->unk3C--;
    TaskYieldTrampoline(10);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 128 << 9;
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 128 << 7;
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(10);
    gCurTask->unk54 = 176 << 8;
    gCurTask->unk5C = 0xFFFFF000;
    gCurTask->unk3C = 102;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 17;
    TaskYieldTrampoline(2);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(2);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 110;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 128 << 9;
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 102;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 17;
    TaskYieldTrampoline(1);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFF38000;
    gCurTask->unk5C = 0xFFFE8000;
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 0x10F7;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 160 << 10;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 140 << 13;
    gCurTask->unk3C = 104;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 107;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 200 << 12;
    gCurTask->unk5C = 128 << 8;
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 0x10F7;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFFD8000;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFEE8000;
    gCurTask->unk58 = 128 << 10;
    gCurTask->unk3C = 37;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0;
    gCurTask->unk58 = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk3E |= 128 << 8;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFF38000, 0xFFFE8000, 0x5A5A5A5A, 0xFFFE0000, 0, 0x5A5A5A5A);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 0x10F7;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 160 << 10;
    gCurTask->unk58 = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 140 << 13;
    gCurTask->unk3C = 104;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 107;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 200 << 12;
    gCurTask->unk5C = 128 << 8;
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 0x10F7;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFFD8000;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFEE8000;
    gCurTask->unk58 = 128 << 10;
    gCurTask->unk3C = 37;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0;
    gCurTask->unk58 = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk3E |= 128 << 8;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFF38000, 0xFFFE8000, 0x5A5A5A5A, 0xFFFE0000, 0, 0x5A5A5A5A);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 0x10F7;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFFD8000;
    gCurTask->unk58 = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 140 << 13;
    gCurTask->unk3C = 104;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 37;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 200 << 12;
    gCurTask->unk5C = 128 << 8;
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 0x10F7;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFFD8000;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFEE8000;
    gCurTask->unk58 = 128 << 10;
    gCurTask->unk3C = 37;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0;
    gCurTask->unk58 = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 37;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0xFFFC6000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3C = 31;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFD9000;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 128 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(7);
    TaskStop();
    gCurTask->unk3E |= 128 << 8;
    CreatePlayerEffect(gCurTask->unk88->unk00, 16, 0);
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(21);
}

void sub_0805f778(void)
{
    gCurTask->unk54 = 0xFFFD8000;
    gCurTask->unk5C = 128 << 7;
    gCurTask->unk3C = 20;
    TaskYieldTrampoline(5);
    gCurTask->unk54 = 0xFFFF0000;
    gCurTask->unk5C = 128 << 5;
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(10);
    gCurTask->unk54 = 0xFFFF8000;
    gCurTask->unk5C = 0;
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 160 << 10;
    gCurTask->unk5C = 0xFFFFC000;
    gCurTask->unk3C = 18;
    TaskYieldTrampoline(5);
    gCurTask->unk54 = 128 << 9;
    gCurTask->unk5C = 0;
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 128 << 8;
    gCurTask->unk5C = 0;
    gCurTask->unk3C = 12;
    TaskYieldTrampoline(10);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFFE000, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0xFFFAA000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 37;
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 7, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(6);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 224 << 8, 0xFFFFE000, 0x5A5A5A5A);
    gCurTask->unk3C = 123;
    TaskYieldTrampoline(13);
    gCurTask->unk3E |= 128 << 8;
    TaskStopY();
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->unk3E |= 128 << 8;
    CreatePlayerEffect(gCurTask->unk88->unk00, 16, 0);
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(21);
}

void sub_0805fb88(void)
{
    TaskSetMotion(204 << 9, 0xFFFFE800, 0x5A5A5A5A, 0xFFFE7800, 224 << 6, 0x5A5A5A5A);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 13;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 192 << 6;
    gCurTask->unk5C = 0;
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 0;
    gCurTask->unk60 = 0;
    gCurTask->unk3C++;
    TaskYieldTrampoline(8);
    TaskSetMotion(204 << 9, 0xFFFFE800, 0x5A5A5A5A, 0xFFFE7800, 224 << 6, 0x5A5A5A5A);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 192 << 6;
    gCurTask->unk5C = 0;
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 0;
    gCurTask->unk60 = 0;
    gCurTask->unk3C = 12;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 168 << 9;
    gCurTask->unk5C = 0xFFFFE800;
    gCurTask->unk3C = 102;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 192 << 6;
    gCurTask->unk5C = 0;
    TaskYieldTrampoline(2);
    TaskSetMotion(192 << 6, 0, 0x5A5A5A5A, 0xFFFD6000, 192 << 7, 0x5A5A5A5A);
    TaskSetFrameFlip(17);
    TaskYieldTrampoline(1);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 102;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    TaskSetMotion(192 << 6, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(11);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF2800, 0, 0x5A5A5A5A, 0xFFFB8000, 192 << 6, 0x5A5A5A5A);
    gCurTask->unk3C = 31;
    TaskYieldTrampoline(14);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFFA000;
    gCurTask->unk5C = 0xFFFFF400;
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFFF2800;
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 0xFFFFA000;
    gCurTask->unk3C = 32;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFF7000;
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(7);
    TaskYieldTrampoline(5);
    TaskSetMotion(0xFFFFD000, 0, 0x5A5A5A5A, 224 << 7, 0xFFFFF000, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFF4000;
    gCurTask->unk5C = 192 << 5;
    gCurTask->unk3C = 123;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 0;
    gCurTask->unk5C = 0;
    TaskYieldTrampoline(5);
    TaskStop();
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(3);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(19);
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFE2000;
    gCurTask->unk5C = 240 << 4;
    gCurTask->unk3C = 97;
    TaskYieldTrampoline(19);
    gCurTask->unk54 = 0;
    gCurTask->unk5C = 0;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFE8000;
    gCurTask->unk5C = 144 << 4;
    gCurTask->unk3C = 97;
    TaskYieldTrampoline(11);
    gCurTask->unk54 = 0xFFFFA000;
    gCurTask->unk3C = 123;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0xFFFE8600;
    gCurTask->unk5C = 216 << 6;
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 160 << 11;
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 154 << 1;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 309;
    TaskYieldTrampoline(4);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0xFFFDC000, 192 << 7, 0x5A5A5A5A);
    gCurTask->unk3C = 147 << 1;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 309;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0;
    gCurTask->unk60 = 0;
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 144 << 9;
    gCurTask->unk5C = 192 << 4;
    gCurTask->unk3C = 149 << 1;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 309;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 216 << 8;
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 144 << 8;
    gCurTask->unk3C--;
    TaskYieldTrampoline(4);
    TaskSetMotion(144 << 7, 0, 0x5A5A5A5A, 0xFFFB0000, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 144 << 7;
    gCurTask->unk3C = 102;
    TaskYieldTrampoline(7);
    gCurTask->unk58 = 0;
    gCurTask->unk60 = 0;
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    gCurTask->unk3C++;
    TaskYieldTrampoline(5);
    gCurTask->unk5C = 192 << 3;
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 16;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(2);
    TaskSetMotion(144 << 8, 0xFFFFEE00, 0x5A5A5A5A, 242 << 7, 0xFFFFF500, 0x5A5A5A5A);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 123;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0;
    gCurTask->unk5C = 0;
    TaskYieldTrampoline(11);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->unk3E |= 128 << 8;
    CreatePlayerEffect(gCurTask->unk88->unk00, 16, 0);
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(21);
}

void sub_08060308(void)
{
    TaskSetMotion(0xFFFF8C00, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 15;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0xFFFF4000;
    gCurTask->unk60 = 128 << 6;
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(7);
    gCurTask->unk54 = 0xFFFE8000;
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(7);
    gCurTask->unk3C = 19;
    TaskYieldTrampoline(1);
    TaskSetMotion(0xFFFF8C00, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 20;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0xFFFF4000;
    gCurTask->unk60 = 128 << 6;
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 12;
    TaskYieldTrampoline(7);
    gCurTask->unk54 = 0xFFFE8000;
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(7);
    gCurTask->unk3C = 14;
    TaskYieldTrampoline(1);
    TaskStop();
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(3);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 128 << 8;
    gCurTask->unk3C = 102;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 108;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 128 << 9;
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 192 << 9;
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(3);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 128 << 8;
    gCurTask->unk3C = 148;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0xFFFCA000, 128 << 6, 0x5A5A5A5A);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 172;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 151;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 172;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 151;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 172;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 151;
    TaskYieldTrampoline(5);
    gCurTask->unk3C = 0x00000133;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C = 147 << 1;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(11);
    gCurTask->unk58 = 160 << 12;
    gCurTask->unk60 = 0xFFFF0000;
    gCurTask->unk3C = 155 << 1;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 192 << 10;
    TaskYieldTrampoline(6);
    gCurTask->unk58 = 0xFFFE0000;
    gCurTask->unk60 = 0;
    gCurTask->unk3C = 0x0000012B;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0xFFFD0000;
    gCurTask->unk3C = 156 << 1;
    TaskYieldTrampoline(1);
    TaskStop();
    TaskYieldTrampoline(18);
    gCurTask->unk3C = 0x0000012B;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 160 << 11;
    gCurTask->unk3C = 0x00000137;
    TaskYieldTrampoline(1);
    TaskStop();
    TaskYieldTrampoline(1);
    TaskSetMotion(192 << 8, 0, 0x5A5A5A5A, 0xFFF7C000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3C = 149 << 1;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFE0000;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000135;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk3E |= 128 << 8;
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(212 << 8, 0, 0x5A5A5A5A, 0xFFFD8000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF4000, 0, 0x5A5A5A5A, 0xFFFD5000, 128 << 6, 0x5A5A5A5A);
    gCurTask->unk3C = 31;
    TaskYieldTrampoline(21);
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(3);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 41;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFFF8000;
    gCurTask->unk3C = 102;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFFE8000;
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFFF8000;
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(2);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk54 = 128 << 8;
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(2);
    TaskSetMotion(128 << 8, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 128 << 8;
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(3);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk54 = 0xFFFF8000;
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(3);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk54 = 0xFFFF0000;
    gCurTask->unk3C = 109;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFFF8000;
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 128 << 8;
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 128 << 9;
    gCurTask->unk3C--;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 128 << 8;
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(1);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk54 = 0xFFFF8000;
    gCurTask->unk3C = 111;
    TaskYieldTrampoline(2);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk54 = 0xFFFF0000;
    gCurTask->unk3C = 109;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 128 << 8;
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 128 << 9;
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 128 << 7;
    gCurTask->unk3C = 14;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 128 << 9;
    gCurTask->unk3C = 150;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 128 << 8;
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0;
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0;
    gCurTask->unk58 = 0xFFFE1600;
    gCurTask->unk3C = 151;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 176 << 5;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 172;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 151;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 172;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 151;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 172;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 248 << 6;
    gCurTask->unk3C = 151;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 172;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 128 << 8;
    gCurTask->unk3C = 148;
    TaskYieldTrampoline(1);
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0;
    gCurTask->unk3C++;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0xFFFD0000;
    gCurTask->unk3E |= 128 << 8;
    CreatePlayerEffect(gCurTask->unk88->unk00, 16, 0);
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(1);
    TaskStop();
    TaskYieldTrampoline(20);
}

void sub_08060c2c(void)
{
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk54 = 0xFFFFC000;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF8000, 128 << 3, 0x5A5A5A5A, 0xFFFE0800, 224 << 6, 0x5A5A5A5A);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    gCurTask->unk3C++;
    TaskYieldTrampoline(4);
    gCurTask->unk3C--;
    TaskYieldTrampoline(3);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFFC000;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF4000, 192 << 3, 0x5A5A5A5A, 0xFFFE0800, 224 << 6, 0x5A5A5A5A);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(3);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 109;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 107;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    gCurTask->unk3C--;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 16;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 15;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 128 << 7;
    gCurTask->unk5C = 160 << 3;
    gCurTask->unk3C = 10;
    TaskYieldTrampoline(14);
    gCurTask->unk58 = 0xFFFE0000;
    gCurTask->unk60 = 128 << 9;
    gCurTask->unk3C = 10;
    TaskYieldTrampoline(3);
    TaskSetMotion(128 << 9, 0, 0x5A5A5A5A, 128 << 10, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 38;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0;
    gCurTask->unk60 = 0;
    gCurTask->unk3C = 33;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0xFFFC0000;
    gCurTask->unk3C = 43;
    TaskYieldTrampoline(1);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 43;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 128 << 7;
    gCurTask->unk58 = 128 << 11;
    gCurTask->unk3C = 42;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFE0000;
    gCurTask->unk60 = 128 << 9;
    gCurTask->unk3C = 42;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 0xFFFE0000;
    gCurTask->unk60 = 128 << 9;
    gCurTask->unk3C = 42;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 0xFFFE0000;
    gCurTask->unk60 = 128 << 9;
    gCurTask->unk3C = 42;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 128 << 7;
    gCurTask->unk58 = 0;
    gCurTask->unk60 = 0;
    gCurTask->unk3C = 42;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 0xFFFC0000;
    gCurTask->unk3C = 43;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0;
    gCurTask->unk3C = 43;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 128 << 9;
    gCurTask->unk58 = 0xFFFD8000;
    gCurTask->unk60 = 128 << 7;
    gCurTask->unk3C = 44;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 46;
    TaskYieldTrampoline(4);
    gCurTask->unk3C = 47;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 36;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 49;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 37;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 32;
    TaskYieldTrampoline(3);
    TaskSetMotion(128 << 7, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 43;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFF6000;
    gCurTask->unk58 = 0xFFFD4000;
    gCurTask->unk60 = 128 << 7;
    gCurTask->unk3C = 42;
    TaskYieldTrampoline(6);
    gCurTask->unk3C = 33;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 32;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 41;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 41;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(1);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(14);
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(5);
    gCurTask->unk54 = 0xFFFA0000;
    gCurTask->unk3C = 100;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 100;
    TaskYieldTrampoline(1);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk54 = 192 << 11;
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(20);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk58 = 160 << 9;
    gCurTask->unk60 = 0xFFFFC000;
    gCurTask->unk3C = 123;
    TaskYieldTrampoline(9);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0xFFFE0000;
    gCurTask->unk60 = 128 << 8;
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(7);
    gCurTask->unk58 = 0;
    gCurTask->unk60 = 0;
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(3);
    gCurTask->unk3E |= 128 << 8;
    TaskSetMotion(0xFFFFE000, 0, 0x5A5A5A5A, 160 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gCurTask->unk3C = 123;
    TaskYieldTrampoline(9);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0xFFFE0000;
    gCurTask->unk60 = 128 << 8;
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(7);
    gCurTask->unk58 = 0;
    gCurTask->unk60 = 0;
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(3);
    gCurTask->unk3E &= 0x7FFF;
    TaskSetMotion(128 << 6, 0, 0x5A5A5A5A, 160 << 9, 0xFFFFC000, 0x5A5A5A5A);
    gCurTask->unk3C = 123;
    TaskYieldTrampoline(9);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk5C = 192 << 4;
    gCurTask->unk58 = 0xFFFDF800;
    gCurTask->unk60 = 160 << 6;
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(4);
    gCurTask->unk3C = 31;
    TaskYieldTrampoline(4);
    gCurTask->unk3C = 32;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 33;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 34;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 35;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 36;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 37;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 41;
    TaskYieldTrampoline(3);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0xFFFF2000, 128 << 7, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(6);
    gCurTask->unk3E |= 128 << 8;
    gCurTask->unk58 = 0;
    gCurTask->unk60 = 0;
    gCurTask->unk3C = 15;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 14;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFFC000;
    gCurTask->unk5C = 0xFFFFFB00;
    gCurTask->unk3C = 10;
    TaskYieldTrampoline(14);
    gCurTask->unk5C = 0;
    gCurTask->unk58 = 0xFFFE0000;
    gCurTask->unk60 = 128 << 9;
    gCurTask->unk3C = 10;
    TaskYieldTrampoline(3);
    TaskSetMotion(0xFFFF4000, 0, 0x5A5A5A5A, 128 << 10, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 38;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0;
    gCurTask->unk60 = 0;
    gCurTask->unk3C = 33;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 43;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFFFB800;
    gCurTask->unk3C = 42;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFE0000;
    gCurTask->unk60 = 128 << 9;
    gCurTask->unk3C = 42;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0xFFFE0000;
    gCurTask->unk60 = 136 << 7;
    gCurTask->unk3C = 46;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 36;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 37;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 41;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 40;
    TaskYieldTrampoline(2);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 132 << 8;
    gCurTask->unk60 = 0xFFFFF400;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(19);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    CreatePlayerEffect(gCurTask->unk88->unk00, 16, 0);
    gCurTask->unk3C = 146;
    TaskYieldTrampoline(21);
}

void sub_080613e4(void)
{
    gCurTask->unk3E |= 0x8000;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk3E &= 0x7FFF;
    TaskSetMotion(0xFFFF0000, 0, 0x5A5A5A5A, 0xFFFDC000, 0x4000, 0x5A5A5A5A);
    gCurTask->unk3C = 0x134;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000135;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x126;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000127;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x128;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000129;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x12A;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x0000012B;
    TaskYieldTrampoline(3);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0x60000, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 0x00000137;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0x20000;
    gCurTask->unk60 = 0xFFFF0000;
    gCurTask->unk3C = 0x00000137;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 0x00000137;
    TaskYieldTrampoline(2);
    TaskSetMotion(0x10000, 0, 0x5A5A5A5A, 0xFFF7C000, 0x4000, 0x5A5A5A5A);
    gCurTask->unk3C = 0x0000012B;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFE0000;
    gCurTask->unk3C = 0x0000012B;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x12A;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000129;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x128;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000127;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x126;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000135;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x134;
    TaskYieldTrampoline(3);
    TaskSetMotion(0x4000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0x5800;
    gCurTask->unk5C = 0xFFFFF800;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(5);
    gCurTask->unk3C = 0x10;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 0x66;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 0x67;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 0x6000;
    gCurTask->unk60 = 0xFFFFE000;
    gCurTask->unk3C = 0x68;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 0x69;
    TaskYieldTrampoline(2);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0xFFFE5000, 0x3000, 0x5A5A5A5A);
    gCurTask->unk3C = 0x5A;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x58;
    TaskYieldTrampoline(15);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(6);
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(2);
    TaskSetMotion(0x10000, 0, 0x5A5A5A5A, 0xFFFDC000, 0x4000, 0x5A5A5A5A);
    gCurTask->unk3C = 0x132;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000131;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x130;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0x12000;
    gCurTask->unk3C = 0x0000012F;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x12E;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x0000012D;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x12C;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x0000012B;
    TaskYieldTrampoline(3);
    TaskSetMotion(0x4000, 0, 0x5A5A5A5A, 0x60000, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 0x136;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0x20000;
    gCurTask->unk60 = 0xFFFF0000;
    gCurTask->unk3C = 0x136;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 0x136;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF0000, 0, 0x5A5A5A5A, 0xFFF7C000, 0x4000, 0x5A5A5A5A);
    gCurTask->unk3C = 0x0000012B;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFE0000;
    gCurTask->unk3C = 0x0000012B;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x12C;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x0000012D;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x12E;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x0000012F;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x130;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000131;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x132;
    TaskYieldTrampoline(3);
    gCurTask->unk3E |= 0x8000;
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFFA800;
    gCurTask->unk5C = 0x800;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(5);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 0x6C;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 0x6D;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 0x6E;
    TaskYieldTrampoline(3);
    gCurTask->unk3E |= 0x8000;
    gCurTask->unk58 = 0x6000;
    gCurTask->unk60 = 0xFFFFE800;
    gCurTask->unk3C = 0x6F;
    TaskYieldTrampoline(5);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0xFFFE4800, 0x2800, 0x5A5A5A5A);
    gCurTask->unk3C = 0x92;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x92;
    TaskYieldTrampoline(0x13);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0xFFFDC000;
    gCurTask->unk60 = 0x4000;
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(1);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0xFFFDC000;
    gCurTask->unk60 = 0x4000;
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(1);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk3E |= 0x8000;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0xFFFB8000;
    gCurTask->unk60 = 0x4000;
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(10);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk58 = 0xFFFF7000;
    gCurTask->unk60 = 0x1800;
    gCurTask->unk3C = 0x00000133;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x00000135;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000127;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000129;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x0000012B;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x0000012D;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0;
    gCurTask->unk60 = 0x3000;
    gCurTask->unk3C = 0x0000012F;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000131;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000133;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x134;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000135;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x126;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000127;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x128;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x00000129;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0x60000;
    gCurTask->unk60 = 0;
    gCurTask->unk3C = 0x136;
    TaskYieldTrampoline(1);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 0x136;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0xFFFD3000;
    gCurTask->unk60 = 0x3000;
    gCurTask->unk3C = 0x0000012B;
    TaskYieldTrampoline(9);
    gCurTask->unk3C = 0x00000139;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 0x13A;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 0x0000013B;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 0x13C;
    TaskYieldTrampoline(3);
    gCurTask->unk3C = 0x0000013D;
    TaskYieldTrampoline(8);
    gCurTask->unk58 = 0xFFFC0000;
    gCurTask->unk60 = 0xFFFE8000;
    gCurTask->unk3C = 0x6A;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0x18000;
    gCurTask->unk3C = 0x6A;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0;
    gCurTask->unk60 = 0;
    gCurTask->unk3C = 0x69;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x68;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x67;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x66;
    TaskYieldTrampoline(2);
    gCurTask->unk3E |= 0x8000;
    gCurTask->unk3C = 0x6E;
    TaskYieldTrampoline(2);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 0x6F;
    TaskYieldTrampoline(2);
    gCurTask->unk3E |= 0x8000;
    gCurTask->unk3C = 0x11;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(15);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    gCurTask->unk3E |= 0x8000;
    CreatePlayerEffect(gCurTask->unk88->unk00, 16, 0);
    gCurTask->unk3C = 0x92;
    TaskYieldTrampoline(0x15);
}

void sub_08061cac(void)
{
    gCurTask->unk3E &= 0x7FFF;
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFB0000, 0, 0x5A5A5A5A, 0xFFFD0000, 0x10000, 0x5A5A5A5A);
    gCurTask->unk3C = 0x64;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 0x64;
    TaskYieldTrampoline(4);
    TaskSetMotion(0x80000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0x20000;
    gCurTask->unk5C = 0xFFFFC000;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(7);
    TaskSetMotion(0x4000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(4);
    gCurTask->unk3C = 0x13;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFB0000, 0, 0x5A5A5A5A, 0xFFFD0000, 0x10000, 0x5A5A5A5A);
    gCurTask->unk3C = 0x64;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 0x64;
    TaskYieldTrampoline(4);
    TaskSetMotion(0x80000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0x20000;
    gCurTask->unk5C = 0xFFFFC000;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(7);
    TaskSetMotion(0x4000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(4);
    gCurTask->unk3C = 0x13;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFF0000;
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFA0000, 0, 0x5A5A5A5A, 0xFFFB8000, 0x8000, 0x5A5A5A5A);
    gCurTask->unk3C = 0x64;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFFF0000;
    gCurTask->unk3C = 0x64;
    TaskYieldTrampoline(4);
    gCurTask->unk54 = 0x40000;
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFFF0000;
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    TaskSetMotion(0x8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF0000, 0x1000, 0x5A5A5A5A, 0xFFFE8000, 0x4000, 0x5A5A5A5A);
    gCurTask->unk3C = 0x1E;
    TaskYieldTrampoline(11);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 0x1E;
    TaskYieldTrampoline(8);
    gCurTask->unk54 = 0x4000;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFF0000, 0, 0x5A5A5A5A, 0xFFFDC000, 0x4000, 0x5A5A5A5A);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0x14000;
    gCurTask->unk3C = 0x00000133;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFF8000;
    gCurTask->unk3C = 0x00000133;
    TaskYieldTrampoline(5);
    gCurTask->unk3E |= 0x8000;
    gCurTask->unk58 = 0xFFFEC000;
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0x8000;
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0xFFFFC000;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(1);
    TaskSetMotion(0xFFFFC000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x7B;
    TaskYieldTrampoline(2);
    TaskSetMotion(0x10000, 0, 0x5A5A5A5A, 0xFFFDC000, 0x4000, 0x5A5A5A5A);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk58 = 0x14000;
    gCurTask->unk3C = 0x00000133;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFF8000;
    gCurTask->unk3C = 0x00000133;
    TaskYieldTrampoline(5);
    gCurTask->unk58 = 0xFFFEC000;
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0x8000;
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->unk54 = 0x4000;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(1);
    TaskSetMotion(0x4000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x7B;
    TaskYieldTrampoline(2);
    TaskSetMotion(0, 0, 0x5A5A5A5A, 0xFFFA8000, 0x8000, 0x5A5A5A5A);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x66;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x68;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x6B;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x6D;
    TaskYieldTrampoline(2);
    gCurTask->unk3E |= 0x8000;
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk58 = 0x38000;
    gCurTask->unk3C = 0x00000133;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0x20000;
    gCurTask->unk3C = 0x00000133;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0x10000;
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0x38000;
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk58 = 0x20000;
    gCurTask->unk60 = 0xFFFF0000;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(3);
    gCurTask->unk58 = 0x20000;
    gCurTask->unk60 = 0;
    gCurTask->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFE0000;
    gCurTask->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0x20000;
    gCurTask->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFE0000;
    gCurTask->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0x10000;
    gCurTask->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFF0000;
    gCurTask->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0x10000;
    gCurTask->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0xFFFF0000;
    gCurTask->unk3C = 0x7B;
    TaskYieldTrampoline(1);
    gCurTask->unk58 = 0;
    gCurTask->unk3C = 0x7B;
    TaskYieldTrampoline(10);
    gCurTask->unk3E |= 0x8000;
    gCurTask->unk54 = 0x20000;
    gCurTask->unk5C = 0xFFFFF000;
    gCurTask->unk3C = 0x12;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 0x10000;
    gCurTask->unk3C = 0x11;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 0x10000;
    gCurTask->unk3C = 0x10;
    TaskYieldTrampoline(4);
    gCurTask->unk54 = 0x8000;
    gCurTask->unk3C = 15;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 14;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0x20000;
    gCurTask->unk5C = 0xFFFFF000;
    gCurTask->unk3C = 13;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 0x10000;
    gCurTask->unk3C = 12;
    TaskYieldTrampoline(6);
    gCurTask->unk54 = 0x10000;
    gCurTask->unk3C = 0x15;
    TaskYieldTrampoline(4);
    gCurTask->unk54 = 0x8000;
    gCurTask->unk3C = 0x14;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 0x13;
    TaskYieldTrampoline(2);
    gCurTask->unk3E &= 0x7FFF;
    gCurTask->unk54 = 0x10000;
    gCurTask->unk3C = 0x6D;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x6C;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xC000;
    gCurTask->unk3C = 0x6B;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x6A;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x69;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0x8000;
    gCurTask->unk3C = 0x68;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x67;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x66;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0x4000;
    gCurTask->unk3C = 0x6F;
    TaskYieldTrampoline(2);
    gCurTask->unk3E |= 0x8000;
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 0x11;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 0x10;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 0xFFFFC000;
    gCurTask->unk5C = 0;
    gCurTask->unk3C = 15;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFF8000;
    gCurTask->unk3C = 14;
    TaskYieldTrampoline(1);
    gCurTask->unk3C = 13;
    TaskYieldTrampoline(3);
    gCurTask->unk54 = 0xFFFF0000;
    gCurTask->unk3C = 0x1B;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x1C;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x1D;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x1C;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x1B;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFF8000;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskSetMotion(0xFFFEC000, 0, 0x5A5A5A5A, 0xFFFD4000, 0x4000, 0x5A5A5A5A);
    gCurTask->unk3C = 0x16;
    TaskYieldTrampoline(14);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0x29;
    TaskYieldTrampoline(1);
    TaskSetMotion(0xFFFF8000, 0, 0x5A5A5A5A, 0, 0, 0x5A5A5A5A);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0xFFFFC000;
    gCurTask->unk3C = 0x1E;
    TaskYieldTrampoline(0xF);
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 0x28;
    TaskYieldTrampoline(2);
    gCurTask->unk3C = 0;
    TaskYieldTrampoline(2);
    gCurTask->unk54 = 0x4000;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(5);
    gCurTask->unk54 = 0;
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(14);
    gCurTask->unk3C = 5;
    TaskYieldTrampoline(2);
    TaskStop();
    CreatePlayerEffect(gCurTask->unk88->unk00, 16, 0);
    gCurTask->unk3C = 0x92;
    TaskYieldTrampoline(0x15);
}
