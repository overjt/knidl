#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* mode_c6260.c (0x080C6260-0x080C641F, issue #98).
 *
 * AgbMain state 11 (src/main.c calls EndingMain once and moves to
 * state 12, which runs M38's sub_080c6420).
 * 
 *   EndingMain   copies gLocalPlayer, gLinkIsMaster, gLinkPlayerCount and
 *       gPlayerCount into EWRAM cells, runs the SIO teardown DisconnectLink
 *       (after LinkRequestSync/LinkSyncClock when gPrevGameState is 20, the value
 *       AgbMain's state 20 leaves there), stops the sound (StopAllSound),
 *       plays the two scenes below unless gPrevGameState is 20 or
 *       gUnk_03001F30 is 1, and ends with sub_080b8070(gCurSaveSlot).
 *   sub_080c62f0 / sub_080c6388   one scene each: preset and load a room
 *       (sub_08024610(0, 0) / sub_08024654(632, 248)), palette set 15 / 16
 *       (LoadBgLayout), DISPCNT BG bits 0x1D00 / 0x1C00, spawn the scene's
 *       task and run frames (RunLinkFrame) until it clears gEndingSceneActive,
 *       then tear the level down (sub_08027178).
 *   sub_080c6354 / sub_080c63ec   spawn M38's task type #100 / #101,
 *       retrying every frame until a slot is free, with Task.unk73 = 0. */

extern u16 gUnk_02000028;
extern u16 gLocalPlayer;
extern u16 gUnk_02007D3C;
extern u16 gLinkIsMaster;
extern u16 gUnk_0200616C;
extern u16 gLinkPlayerCount;
extern u16 gUnk_02004C94;
extern u16 gPlayerCount;
extern u16 gPrevGameState;
extern u8 gUnk_03001F30;
extern s32 gCurSaveSlot;
extern u8 gEndingSceneActive;
extern vu16 gDispCnt;

void BeginFastFadeInFromWhite(void);
void BeginFastFadeOutToWhite(void);
void LinkRequestSync(void);
void LinkSyncClock(void);
void DisconnectLink(void);
void RunLinkFrame(void);
void RunLinkFramesUntilFadeDone(void);
void StopAllSound(void);
s32 TaskCreateFrom(u32 type, s32 idx);                         /* spawn a task */
void LoadBgLayout(s32 a0);
void sub_08024610(s32 x, s32 y);
void sub_08024654(s32 x, s32 y);
void sub_08027178(void);
void sub_080b8070(s32 a);
void sub_080c62f0(void);
void sub_080c6354(void);
void sub_080c6388(void);
void sub_080c63ec(void);

void EndingMain(void)
{
    gUnk_02000028 = gLocalPlayer;
    gUnk_02007D3C = gLinkIsMaster;
    gUnk_0200616C = gLinkPlayerCount;
    gUnk_02004C94 = gPlayerCount;
    if (gPrevGameState == 20) {
        LinkRequestSync();
        LinkSyncClock();
    }
    DisconnectLink();
    StopAllSound();
    if (gPrevGameState != 20 && gUnk_03001F30 != 1) {
        sub_080c62f0();
        sub_080c6388();
    }
    StopAllSound();
    sub_080b8070(gCurSaveSlot);
}

void sub_080c62f0(void)
{
    gEndingSceneActive = 1;
    sub_08024610(0, 0);
    LoadBgLayout(15);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1D00;
    sub_080c6354();
    BeginFastFadeInFromWhite();
    RunLinkFramesUntilFadeDone();
    while (gEndingSceneActive != 0)
        RunLinkFrame();
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
    sub_08027178();
}

void sub_080c6354(void)
{
    s32 id;
    struct Task *t;

    while ((id = TaskCreateFrom(100, 32)) == -1)
        RunLinkFrame();
    t = &gTasks[id];
    t->unk73 = 0;
}

void sub_080c6388(void)
{
    gEndingSceneActive = 1;
    sub_08024654(632, 248);
    LoadBgLayout(16);
    gDispCnt &= 0xE0FF;
    gDispCnt |= 0x1C00;
    sub_080c63ec();
    BeginFastFadeInFromWhite();
    RunLinkFramesUntilFadeDone();
    while (gEndingSceneActive != 0)
        RunLinkFrame();
    BeginFastFadeOutToWhite();
    RunLinkFramesUntilFadeDone();
    sub_08027178();
}

void sub_080c63ec(void)
{
    s32 id;
    struct Task *t;

    while ((id = TaskCreateFrom(101, 32)) == -1)
        RunLinkFrame();
    t = &gTasks[id];
    t->unk73 = 0;
}
