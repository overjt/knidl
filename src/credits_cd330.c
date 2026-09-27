#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* credits_cd330.c (0x080CD330-0x080CD89B, issue #100).
 *
 * AgbMain state 12, part 1: the staff credits (skipped in link play and
 * after AgbMain state 20).  The credits text scrolls up BG0 over a run of
 * recorded demos of the game, one per scene.
 *   CreditsMain   the sequence: per scene of gUnk_087583CC[n] (n by
 *       gExtraMode), load the recorded input (gUnk_0200EC50,
 *       InputRecorderStart) and the room (CreditsLoadScene), fade in, play it for
 *       gUnk_0875841E[n][scene] frames and fade out; then fade the music and
 *       the screen and put back the player's score the demos overwrote.
 *   CreditsLoadScene   load a scene's room and reset the per-scene state (a twin
 *       of M02's sub_0800b788).
 *   CreditsInitText / CreditsStreamText / CreditsScrollText   the text layer: load it,
 *       stream the 14 compressed pages gUnk_087583B4[] into the two BG0 map
 *       halves, and scroll BG0 from the per-frame callback gVBlankEndCallback. */

extern vu16 gDispCnt;          /* DISPCNT shadow */
extern vs32 gBg0ScrollY;          /* BG0 16.16 scroll shadows ... */
extern vs32 gBg0ScrollX;
extern u16 gLocalPlayer;           /* this player's index */
extern u8 gExtraMode;
extern u16 gUnk_02000028;
extern s32 gPlayerScores[];         /* score per player */
extern vu16 gPlayerPressedKeys[];        /* keys pressed per player */
extern vu8 gBldCntTarget1;
extern vu8 gBldCntTarget2;
extern vu8 gBldAlphaEva;
extern vu8 gBldAlphaEvb;
extern u16 gBldY;
extern vu16 gSfxDisabled;
extern s32 gUnk_0201C1A4;           /* credits: the score saved over the demos */
extern u8 gUnk_030023B0;
extern u8 gUnk_0201C1B0;            /* credits: current demo scene */
extern u32 gUnk_087583CC[][8];      /* credits: per variant, the scenes' recorded demos, 0-terminated */
extern u16 gUnk_0875841E[][7];      /* credits: per variant, the scenes' lengths in frames */
extern vu16 gBgPalette[];
extern s16 gInputRecorderMode;
extern u32 gUnk_0200EC50;
extern u16 gUnk_02008008[];
extern u16 gUnk_02007FA8[];
extern u16 gUnk_08758334[];
extern u16 gUnk_08758374[];
extern vu16 gFadeStep;
extern vs16 gBrightness;
extern vu16 gFadeSteps;
extern u32 gVBlankEndCallback;
extern u8 gUnk_020061E0;
extern u16 gNextActorSerial;
extern u8 gUnk_02006178;
extern u8 gUnk_02007CF0;
extern s8 gUnk_02007FB8[];
extern vu16 gPlayerHeldKeys[];
extern u8 gUnk_03001F34;
extern u16 gLatchedPressedKeys[];
extern u16 gLatchedHeldKeys[];
extern s32 gUnk_0201C1A0;           /* credits: BG0 vertical scroll, 16.16 */
extern s32 gUnk_0201C1AC;           /* credits: BG0 horizontal scroll, 16.16 */
extern u8 gUnk_0201C1A8;
extern u8 gUnk_0201C19C;
extern s32 gUnk_0201C1B4;           /* credits: scroll since the last page copy, 1/16 pixel */
extern u32 *gUnk_087583B4[];        /* credits: the 14 compressed text pages */
extern u16 gHudTilemap[];

u32 BeginFade(u16 steps, s16 delta, u16 *mask);   /* delta passed as movs/negs (-2); early_08e8.c defines it u16, src/effect_5a358.c and src/player_47fe8.c spell it s16 too (lesson 3.428) */
void RequestCopy(u32 mode, u32 src, u32 dst, u32 size);
void ResetBgScroll(void);
void ResetTasksAndOam(void);
void RunLinkFrame(void);                                     /* run one frame */
void RunFramesNoTasks(s32 count);
s32 PlayBgm(s32 songId);
void StopAllSound(void);
void FadeOutBgm(s32 speed);
void SetBgmVolume(u16 volume);
void LoadBgLayout(s32 a0);                                   /* load palette set */
void LoadGfxSet(u16 a0);                                   /* load screen graphics */
void ResetScoresAndMaxHealth(void);
void ResetPlayerRecords(void);
void ClearColliderLists(void);
void sub_0802497c(void);
void LatchPlayerKeys(void);
void sub_08066144(void);
void InputRecorderStart(void);
void InputRecorderUpdate(void);
void CreditsLoadScene(void);
void CreditsInitText(void);
void CreditsStreamText(void);
void CreditsScrollText(void);

/* AgbMain state 12, part 1: the staff credits.  The credits text scrolls up
   BG0 (CreditsInitText, CreditsStreamText) over a run of recorded demos, one per
   scene of gUnk_087583CC[n] (n = 2 when gExtraMode is 1, else 1), each
   faded in, played for its length gUnk_0875841E[n][scene] and faded out;
   then the music and the screen fade out and the player's score, which the
   demos overwrite, is put back. */
void CreditsMain(void)
{
    u32 *scenes;
    u16 *frames;
    u16 n;

    gDispCnt |= 0x80;
    gBldCntTarget1 = gBldCntTarget2 = gBldAlphaEva = gBldAlphaEvb = gBldY = 0;
    ResetTasksAndOam();
    LoadBgLayout(17);
    gSfxDisabled = 1;
    gUnk_0201C1A4 = gPlayerScores[gUnk_02000028];
    gLocalPlayer = 0;
    if (gExtraMode == 1)
        gUnk_030023B0 = 2;
    else
        gUnk_030023B0 = 1;
    gUnk_0201C1B0 = 0;
    scenes = gUnk_087583CC[gUnk_030023B0];
    frames = gUnk_0875841E[gUnk_030023B0];
    CreditsInitText();
    ResetScoresAndMaxHealth();
    ResetPlayerRecords();
    LoadGfxSet(0);
    gBgPalette[0] = 0;
    gInputRecorderMode = 3;
    gDispCnt &= ~0x80;
    PlayBgm(20);
    for (;;) {
        gUnk_0200EC50 = scenes[gUnk_0201C1B0];
        InputRecorderStart();
        gUnk_02008008[gLocalPlayer] = 0;
        gUnk_02007FA8[gLocalPlayer] = 0xFFFF;
        CreditsLoadScene();
        gDispCnt &= 0xE0FF;
        gDispCnt |= 0x1D00;
        if (gUnk_0201C1B0 == 0)
            BeginFade(15, -2, gUnk_08758334);
        else
            BeginFade(15, 2, gUnk_08758334);
        n = 15;
        while (n-- != 0) {
            RunLinkFrame();
            if ((s16)gFadeStep < 0) {
                if (gBrightness < -5) {
                    gFadeSteps = 0;
                    gBrightness = -5;
                }
            } else if (gBrightness > -5) {
                gFadeSteps = 0;
                gBrightness = -5;
            }
            CreditsStreamText();
            InputRecorderUpdate();
        }
        gBrightness = -5;
        n = frames[gUnk_0201C1B0];
        while (--n != 0) {
            ClearColliderLists();
            RunLinkFrame();
            CreditsStreamText();
            InputRecorderUpdate();
            LatchPlayerKeys();
        }
        if (scenes[++gUnk_0201C1B0] != 0) {
            BeginFade(15, -2, gUnk_08758334);
            n = 15;
            while (n-- != 0) {
                RunLinkFrame();
                CreditsStreamText();
                InputRecorderUpdate();
            }
            gDispCnt &= 0xE0FF;
            gDispCnt |= 0x100;
            ResetTasksAndOam();
            RunLinkFrame();
            continue;
        }
        break;
    }
    FadeOutBgm(8);
    gBgPalette[0] = 0xFFFF;
    BeginFade(30, 2, gUnk_08758374);
    n = 30;
    while (n-- != 0) {
        RunLinkFrame();
        InputRecorderUpdate();
        LatchPlayerKeys();
    }
    gDispCnt |= 0x80;
    gVBlankEndCallback = 0;
    gSfxDisabled = 0;
    StopAllSound();
    ResetTasksAndOam();
    RunFramesNoTasks(2);
    SetBgmVolume(255);
    gPlayerScores[gUnk_02000028] = gUnk_0201C1A4;
}

/* Load the staff credits' next demo scene: the room, palette set 17, and
   the per-scene state M02's stage loaders reset (sub_0800b788's twin). */
void CreditsLoadScene(void)
{
    s32 i;
    s8 *b;
    s8 *p;
    s8 zero;

    ResetBgScroll();
    ClearColliderLists();
    LoadBgLayout(17);
    sub_0802497c();
    b = gUnk_02007FB8;
    zero = 0;
    p = b + 2;
    do {
        *p = zero;
        p--;
    } while ((s32)p >= (s32)b);
    gUnk_020061E0 = 0;
    sub_08066144();
    gNextActorSerial = 0;
    gUnk_02006178 = 0;
    gUnk_02007CF0 = 0;
    gUnk_03001F34 = 1;
    for (i = 0; i < 4; i++)
        gPlayerHeldKeys[i] = gPlayerPressedKeys[i] = gLatchedHeldKeys[i] = gLatchedPressedKeys[i] = 0;
}

/* Start the staff credits' text layer: load its screen, reset the BG0
   scroll and the page counters and install the scroll callback. */
void CreditsInitText(void)
{
    LoadGfxSet(72);
    gUnk_0201C1A0 = 0x400000;
    gUnk_0201C1AC = 0x40000;
    gVBlankEndCallback = (u32)CreditsScrollText;
    gUnk_0201C1A8 = 0;
    gUnk_0201C19C = 0;
    gUnk_0201C1B4 = gUnk_0201C1A0 >> 12;
}

/* Stream the staff credits' text into BG0: stage the next page (a blank one
   after the 14 pages) in gHudTilemap, and each time another 256 pixels
   have scrolled by copy it into one of the two BG0 map halves (0x06001000
   for odd pages, 0x06001800 for even ones). */
void CreditsStreamText(void)
{
    u16 zero;

    if (gUnk_0201C19C == 0) {
        if (gUnk_0201C1A8 < 14) {
            LZ77UnCompWram(gUnk_087583B4[gUnk_0201C1A8], gHudTilemap);
        } else {
            zero = 0;
            CpuSet(&zero, gHudTilemap, 0x01000400);
        }
        gUnk_0201C19C = 1;
        gUnk_0201C1A8++;
    }
    if ((gUnk_0201C1B4 & 0x1000) && gUnk_0201C19C != 0) {
        if (gUnk_0201C1A8 & 1)
            RequestCopy(1, (u32)gHudTilemap, 0x06001000, 0x800);
        else
            RequestCopy(1, (u32)gHudTilemap, 0x06001800, 0x800);
        gUnk_0201C19C = 0;
        gUnk_0201C1B4 = 0;
    }
}

/* The staff credits' per-frame callback (installed in gVBlankEndCallback by
   CreditsInitText): scroll BG0 up by half a pixel a frame until the text has
   gone by, then park it through the BG0 scroll shadows. */
void CreditsScrollText(void)
{
    if (gUnk_0201C1A0 < 0x0F580000) {
        gUnk_0201C1A0 += 0x8000;
        gUnk_0201C1B4 += 8;
        REG_BG0VOFS = gUnk_0201C1A0 >> 16;
        REG_BG0HOFS = gUnk_0201C1AC >> 16;
    } else {
        gBg0ScrollY = 0x0F580000;
        gBg0ScrollX = 0x40000;
        REG_BG0VOFS = gBg0ScrollY >> 16;
        REG_BG0HOFS = gBg0ScrollX >> 16;
    }
}
