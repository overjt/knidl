#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_47fe8.c (0x08047FE8-0x08049737, issue #88).
 *
 * Player action bodies, part 15: action 29 and per-frame handler 26.
 * PlayerActionGetAbility (action 29, mode 19) is the ability get: it freezes the
 * stage (gUnk_03001F34 = 1), starts the palette fade BeginFade over
 * the rows gUnk_0873B534[player] selects (unless gUnk_030023B0 is set)
 * and, when the swallowed object gives a random ability
 * (PlayerState.unk0A > 1), spins the HUD roulette: PlayerState.pendingAbility
 * steps through abilities 1-24 with the delays gUnk_0873B634[] until A
 * or B is pressed.  It then shows the new ability on the HUD
 * (SetPlayerAbilityNoHud, HudShowAbilityAnimated), loads its sprite tiles (M13's
 * LoadAbilityTiles) and plays the ability's own pose - a 25-way switch on
 * PlayerState.ability - 1 whose arms install hit boxes, spawn effects
 * and load extra tiles (sub_08049a58) - before it restores the palette
 * and the mode (the fade back, BeginFade(4, 2, ...)) and unfreezes
 * the stage.  Its handler PlayerActionGetAbilityUpdate hands
 * over to the ability's own follow-up once Task.unk28 is set (action 42
 * for ability 11, 55 for 24, M11's sub_08040710 otherwise) and, while
 * Task.unk2C is set, drives the pose: ability 2 blends its palettes
 * gUnk_081BE6BC[] over the animation frames 0x36B-0x372 (the twin of
 * M12's sub_080449c8), abilities 4, 12 and 14 register their collider
 * and block hit-box rows. */

/* M11's per-player records (src/stage_43654.c spells them the same way) */
struct M11R8 { u8 unk00; u8 unk01; u8 unk02; u8 unk03; u8 *unk04; };

struct M11R20 { u32 w[5]; };

extern u8 gUnk_03001F34;
extern u8 gUnk_030023B0;
extern u16 gUnk_0873B534[][32];
extern u16 gLatchedHeldKeys[];             /* held keys, latched per player (M11) */
extern u8 gUnk_0873B634[];
extern s32 gUnk_02007D00[];
extern u32 gUnk_0873CCA4[];
extern struct M11R20 gPlayerBodyBoxes[];
extern u32 gUnk_0873C358[];
extern struct M11R8 gPlayerHitBoxSets[];
extern u32 gUnk_0873CF94[];
extern u32 gUnk_0873C1B0[];
extern u32 gUnk_0873CEEC[];
extern vs16 gBrightness;
extern u8 gUnk_081BE6BC[];              /* per-player palettes, 128 bytes each */
extern u8 gObjPalette[];              /* OBJ palette buffer (M11 spelling) */
extern u32 gUnk_0873BF64[];
extern u32 gUnk_0873C1C4[];
extern u32 gUnk_0873CEF4[];
extern u32 gUnk_0873C214[];             /* collider row passed to RegisterCollider (4th arg) */
extern u32 gUnk_0873CF4C[];             /* hit-box set, passed as (struct HitBoxSet *) */

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
void TaskYieldTrampoline(s32 frames);
u32 BeginFade(u16 steps, s16 delta, u16 *mask);   /* delta is signed: the ROM passes -2 as movs/negs */
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 PlaySfx(s32 id);
void TaskSleepForever(void);
void TaskSetFrame(s32 a);
void TaskSetFrameNoFlip(s32 a);
void TaskSetFrameFlip(s32 a);
s32 SetPlayerAbilityNoHud(s32 a, s32 b, u32 c);
void HudShowAbilityAnimated(s32 a, s32 b);
void sub_0800a178(s32 a, s32 id);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u16 TaskBreakBlocks(struct HitBoxSet *p, s32 e);
void PlayerStopAxes(s32 a0);
void SetPlayerInvulnerability(s32 a0, s32 a1, s32 a2);
void PlayerStopSfx(void);
void FreezeOtherTasks(s32 a0);
s32 LoadPlayerBodyBoxRect(s32 playerIdx, u8 *src6);
s32 LoadPlayerHitBoxSet(s32 a0, s32 a1);
void sub_0803f5fc(s8 a0);
void sub_08040710(void);

void LoadAbilityTiles(void);
void sub_08049a58(void);
void CreatePlayerObject(s32 a, s32 b, s32 c);      /* M14, src/plobj_52f6c.c (defined s32 (s8, u8, s32); the result is unused here) */
s32 CreatePlayerEffect(s32 a0, s32 a1, s32 a2);

void PlayerActionGetAbility(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 19;
    gCurTask->updateState = 26;
    gUnk_03001F34 = 1;
    gCurTask->player->running = 0;
    PlayerStopAxes(3);
    {
        struct Task *t = gCurTask;
        t->unk28 = 0;
        t->unk2C = 0;
        t->player->mode = 13;
    }
    gCurTask->player->unk16 = 0xFF;
    {
        struct Task *t = gCurTask;
        t->player->unk40 |= 0x100;
        t->player->unk42 |= 0x720;
        t->player->unk42 &= 0xFFEF;
    }
    FreezeOtherTasks(15);
    if (gUnk_030023B0 == 0)
        BeginFade(4, -2, gUnk_0873B534[gCurTask->player->playerIndex]);
    {
        struct PlayerState *p = gCurTask->player;
        if (p->unk37 == 1) {
            sub_0803f5fc(p->playerIndex);
            PlayerStopSfx();
            gCurTask->player->mouthState = 0;
            {
                struct PlayerState *q = gCurTask->player;
                SetPlayerAbilityNoHud((s8)q->pendingAbility, (s8)q->unk0C, q->playerIndex);
            }
            gCurTask->player->pendingAbility = 0;
            gCurTask->player->unk0C |= 0xFF;
            {
                struct PlayerState *q = gCurTask->player;
                HudShowAbilityAnimated(q->ability, q->playerIndex);
            }
            CreatePlayerEffect(gCurTask->player->playerIndex, 14, 0);
            CreatePlayerEffect(gCurTask->player->playerIndex, 14, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 14, 2);
            CreatePlayerEffect(gCurTask->player->playerIndex, 14, 3);
            TaskYieldTrampoline(4);
        } else {
            p->unk37 = 0;
            gCurTask->player->mouthState = 0;
            {
                struct PlayerState *q = gCurTask->player;
                if (q->unk0A > 1) {
                    q->unk0A = 0;
                    HudShowAbilityAnimated(27, gCurTask->player->playerIndex);
                    TaskYieldTrampoline(6);
                    gCurTask->unk6C = 0;
                    {
                        /* the ROM loads the key cell's address before the table's:
                           a programmer's pointer, hoisted ahead of loop.c's movables */
                        u16 *keys = gLatchedHeldKeys;

                        while (1) {
                            if ((s8)++gCurTask->player->pendingAbility > 24)
                                gCurTask->player->pendingAbility = 1;
                            PlaySfx(101);
                            {
                                struct PlayerState *r = gCurTask->player;
                                HudShowAbilityAnimated((s8)r->pendingAbility, r->playerIndex);
                            }
                            {
                                struct Task *t = gCurTask;
                                if (keys[t->player->playerIndex] & 3)
                                    break;
                                if ((t->sleepFrames = gUnk_0873B634[(s16)t->unk6C++]) == 0)
                                    break;
                                TaskYieldTrampoline((s16)t->sleepFrames);
                            }
                        }
                    }
                    sub_0800a178(0x7FFF, gCurTask->player->playerIndex);
                    switch ((s8)gCurTask->player->pendingAbility) {
                    default:
                        gCurTask->player->unk0C = 0xFF;
                        break;
                    case 7:
                        gCurTask->player->unk0C = 3;
                        break;
                    case 11:
                    case 20:
                    case 21:
                        gCurTask->player->unk0C = 1;
                        break;
                    }
                }
            }
            CreatePlayerEffect(gCurTask->player->playerIndex, 14, 0);
            CreatePlayerEffect(gCurTask->player->playerIndex, 14, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 14, 2);
            CreatePlayerEffect(gCurTask->player->playerIndex, 14, 3);
            TaskSetFrame(68);
            TaskYieldTrampoline(2);
            PlaySfx(113);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(8);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            {
                struct PlayerState *q = gCurTask->player;
                SetPlayerAbilityNoHud((s8)q->pendingAbility, (s8)q->unk0C, q->playerIndex);
            }
            {
                struct PlayerState *q = gCurTask->player;
                HudShowAbilityAnimated(q->ability, q->playerIndex);
            }
        }
    }
    gCurTask->player->pendingAbility = 0;
    PlaySfx(108);
    LoadAbilityTiles();
    {
        struct Task *t = gCurTask;
        t->player->unk42 |= 128;
        switch ((s8)(t->player->ability - 1)) {
        case 24:
            gCurTask->player->unk37 = 3;
            break;
        case 14:
            if (gCurTask->facing == 1)
                TaskSetFrameFlip(0xB1A);
            else
                TaskSetFrameNoFlip(0xB1A);
            TaskYieldTrampoline(32);
            break;
        case 21:
            if (gCurTask->facing == 1)
                TaskSetFrameFlip(0xE78);
            else
                TaskSetFrameNoFlip(0xE78);
            TaskYieldTrampoline(32);
            break;
        case 22:
            if (gCurTask->facing == 1)
                TaskSetFrameFlip(0xF5D);
            else
                TaskSetFrameNoFlip(0xF5D);
            TaskYieldTrampoline(32);
            break;
        case 23:
        default:
            TaskSetFrame(146);
            TaskYieldTrampoline(32);
            break;
        case 6:
        case 19:
        case 20:
            TaskSetFrame(146);
            TaskYieldTrampoline(32);
            gCurTask->player->unk22 = 2;
            break;
        case 0:
            TaskSetFrame(0x289);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            TaskSetFrame(0x28C);
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            CreatePlayerObject(gCurTask->player->playerIndex, 4, 0);
            CreatePlayerObject(gCurTask->player->playerIndex, 4, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 29, 0);
            CreatePlayerEffect(gCurTask->player->playerIndex, 29, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 29, 2);
            CreatePlayerEffect(gCurTask->player->playerIndex, 28, 4);
            TaskSetFrame(0x279);
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 14);
            TaskSetFrame(0x28D);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            sub_08049a58();
            CreatePlayerEffect(gCurTask->player->playerIndex, 15, 0);
            break;
        case 1:
            gCurTask->unk2C = 1;
            TaskSetFrame(0x36A);
            TaskYieldTrampoline(2);
            gCurTask->unk73 = 1;
            CreatePlayerEffect(gCurTask->player->playerIndex, 30, 0);
            CreatePlayerEffect(gCurTask->player->playerIndex, 30, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 30, 2);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 1; gCurTask->unk6C++) {
                TaskSetFrame(0x36B);
                TaskYieldTrampoline(2);
                gCurTask->unk6C = 0;
                do {
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                } while ((s16)++gCurTask->unk6C <= 6);
            }
            gCurTask->unk73 = 0xFF;
            PlayerStopSfx();
            TaskSetFrame(0x36A);
            TaskYieldTrampoline(2);
            TaskSetFrame(0x28F);
            TaskYieldTrampoline(1);
            sub_08049a58();
            CreatePlayerEffect(gCurTask->player->playerIndex, 15, 0);
            break;
        case 2:
            TaskSetFrame(0x3E9);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(10);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            {
                struct Task *u = gCurTask;
                if (u->onGround & 1)
                    CreatePlayerEffect(u->player->playerIndex, 28, 3);
            }
            CreatePlayerObject(gCurTask->player->playerIndex, 5, 0);
            gCurTask->frame++;
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            break;
        case 3:
            TaskSetFrame(0x46A);
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 2);
            CreatePlayerEffect(gCurTask->player->playerIndex, 3, 0);
            gCurTask->frame++;
            TaskYieldTrampoline(16);
            if (gCurTask->player->unk40 & 32)
                gUnk_02007D00[2]++;
            break;
        case 4:
            TaskSetFrame(0x5CB);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->player->hitBoxSet = gUnk_0873CCA4;
            TaskSetFrame(0x5E1);
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0x5D3);
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                TaskSetFrame(0x5D9);
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 1);
            gCurTask->player->hitBoxSet = 0;
            TaskSetFrame(0x5DF);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            PlayerStopSfx();
            CreatePlayerEffect(gCurTask->player->playerIndex, 32, 5);
            CreatePlayerEffect(gCurTask->player->playerIndex, 32, 6);
            CreatePlayerEffect(gCurTask->player->playerIndex, 32, 7);
            CreatePlayerEffect(gCurTask->player->playerIndex, 32, 8);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            sub_08049a58();
            CreatePlayerEffect(gCurTask->player->playerIndex, 15, 0);
            break;
        case 5:
            TaskSetFrame(0x658);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            TaskSetFrame(0x65A);
            TaskYieldTrampoline(2);
            CreatePlayerObject(gCurTask->player->playerIndex, 6, 0);
            gCurTask->unk6C = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 15);
            TaskSetFrame(0x658);
            TaskYieldTrampoline(2);
            break;
        case 7:
            TaskSetFrame(0x701);
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 7);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0x70A);
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 2);
            PlayerStopSfx();
            TaskSetFrame(0x702);
            TaskYieldTrampoline(1);
            gCurTask->unk6C = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 6);
            TaskSetFrame(0x701);
            TaskYieldTrampoline(1);
            break;
        case 8:
            TaskSetFrame(0x7DA);
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0x78B);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 1);
            break;
        case 9:
            {
                struct M11R20 *d = gPlayerBodyBoxes;

                d[gCurTask->player->playerIndex] = *(struct M11R20 *)gUnk_0873C358;
            }
            gPlayerHitBoxSets[gCurTask->player->playerIndex] = *(struct M11R8 *)gUnk_0873CF94;
            TaskSetFrame(0x8DB);
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0x88C);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 2);
            break;
        case 11:
            {
                struct Task *u = gCurTask;
                u->unk2C = 1;
                gPlayerBodyBoxes[u->player->playerIndex] = *(struct M11R20 *)gUnk_0873C1B0;
            }
            {
                struct Task *u;
                gPlayerHitBoxSets[(u = gCurTask)->player->playerIndex] = *(struct M11R8 *)gUnk_0873CEEC;
                u->unk30 = -1;
            }
            TaskSetFrame(0x96E);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk30++;
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk30++;
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk30++;
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            CreatePlayerEffect(gCurTask->player->playerIndex, 39, 0);
            CreatePlayerEffect(gCurTask->player->playerIndex, 39, 1);
            gCurTask->unk30 = 3;
            TaskSetFrame(0x978);
            TaskYieldTrampoline(2);
            gCurTask->unk30 = 4;
            TaskSetFrame(0x973);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk30 = 1;
            gCurTask->frame = 0x971;
            TaskYieldTrampoline(2);
            gCurTask->frame = 0x96E;
            TaskYieldTrampoline(2);
            break;
        case 12:
            TaskSetFrame(0xA0A);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame--;
            TaskYieldTrampoline(2);
            TaskSetFrame(0xA0D);
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            CreatePlayerObject(gCurTask->player->playerIndex, 7, 0);
            CreatePlayerObject(gCurTask->player->playerIndex, 7, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 40, 0);
            CreatePlayerEffect(gCurTask->player->playerIndex, 40, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 40, 2);
            CreatePlayerEffect(gCurTask->player->playerIndex, 28, 4);
            TaskSetFrame(0x9FA);
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 14);
            TaskSetFrame(0xA0E);
            TaskYieldTrampoline(3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            break;
        case 13:
            TaskSetFrame(0xA8E);
            TaskYieldTrampoline(2);
            {
                struct Task *u = gCurTask;
                u->unk2C = 1;
                CreatePlayerEffect(u->player->playerIndex, 41, 0);
            }
            CreatePlayerEffect(gCurTask->player->playerIndex, 41, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 41, 2);
            CreatePlayerEffect(gCurTask->player->playerIndex, 41, 3);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0xA86);
                TaskYieldTrampoline(1);
                gCurTask->unk6E = 0;
                do {
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                } while (++gCurTask->unk6E <= 6);
            } while ((s16)++gCurTask->unk6C <= 2);
            gCurTask->unk2C = 0;
            PlayerStopSfx();
            TaskSetFrame(0xA8E);
            TaskYieldTrampoline(1);
            break;
        case 15:
            TaskSetFrame(0xBBD);
            TaskYieldTrampoline(6);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            CreatePlayerObject(gCurTask->player->playerIndex, 8, 0);
            CreatePlayerObject(gCurTask->player->playerIndex, 8, 1);
            CreatePlayerObject(gCurTask->player->playerIndex, 8, 2);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0xBBF);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
            } while ((s16)++gCurTask->unk6C <= 1);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0xBC1);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
            } while ((s16)++gCurTask->unk6C <= 1);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0xBC3);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
            } while ((s16)++gCurTask->unk6C <= 1);
            TaskSetFrame(0xBBF);
            TaskYieldTrampoline(4);
            break;
        case 16:
            TaskSetFrame(0xC40);
            TaskYieldTrampoline(1);
            gCurTask->unk6C = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 3);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            TaskSetFrame(0xC48);
            TaskYieldTrampoline(6);
            TaskSetFrame(0xC4D);
            TaskYieldTrampoline(2);
            CreatePlayerEffect(gCurTask->player->playerIndex, 43, 0);
            CreatePlayerEffect(gCurTask->player->playerIndex, 43, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 43, 2);
            CreatePlayerEffect(gCurTask->player->playerIndex, 43, 3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->unk6C = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 3);
            gCurTask->frame++;
            TaskYieldTrampoline(1);
            break;
        case 17:
            TaskSetFrame(0xCCE);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                gCurTask->frame = 0xCDA;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame = 0xCD2;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x500);
            CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x501);
            CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x502);
            CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x503);
            break;
        case 18:
            TaskSetFrame(0xDC5);
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                gCurTask->frame += 2;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 10);
            {
                struct Task *u = gCurTask;
                if (u->facing == 1) {
                    u->frame = 0xDDC;
                    TaskYieldTrampoline(2);
                } else {
                    u->frame = 0xDDB;
                    TaskYieldTrampoline(2);
                }
            }
            gCurTask->unk6C = 0;
            do {
                gCurTask->frame -= 2;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 10);
            TaskSetFrame(0xDE5);
            TaskYieldTrampoline(1);
            break;
        case 10:
            break;
        }
    }
    gCurTask->player->mode = 19;
    if (gUnk_030023B0 == 0) {
        BeginFade(4, 2, gUnk_0873B534[0]);
        TaskYieldTrampoline(4);
        gBrightness = 0;
    }
    FreezeOtherTasks(0);
    {
        struct Task *t = gCurTask;
        struct PlayerState *p;
        t->player->unk42 &= 0xF85F;
        p = t->player;
        if (p->invulnerability == 1)
            p->invulnerabilityTimer += 6;
        else
            SetPlayerInvulnerability(4, 6, p->playerIndex);
    }
    {
        struct Task *t = gCurTask;
        t->unk28++;
        t->player->unk40 &= 0xFEFF;
    }
    gUnk_03001F34 = 0;
    TaskSleepForever();
}

void PlayerActionGetAbilityUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 != 0) {
        struct PlayerState *p = t->player;
        switch (p->ability) {
        case 11:
            p->requestedAction = 42;
            break;
        case 24:
            p->requestedAction = 55;
            break;
        default:
            sub_08040710();
            break;
        }
    } else if (t->unk2C != 0) {
        switch (t->player->ability) {
        case 2:
            switch (t->frame) {
            default:
            case 0x36B:
            case 0x36C:
            case 0x36F:
            case 0x370:
                {
                    struct Task *u = gCurTask;
                    u->player->unk42 &= 0xFFEF;
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
                    BlendColors((u16 *)&gUnk_081BE6BC[u->player->playerIndex * 128],
                                 (u16 *)&gUnk_081BE6BC[u->player->playerIndex * 128 + 32],
                                 (u16)u->unk6E, 16,
                                 (u16 *)(gObjPalette + ((u->tileWord >> 12) << 5)));
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
                    BlendColors((u16 *)&gUnk_081BE6BC[u->player->playerIndex * 128 + 64],
                                 (u16 *)&gUnk_081BE6BC[u->player->playerIndex * 128 + 96],
                                 u->unk70, 16,
                                 (u16 *)(gObjPalette + (((u->tileWord >> 12) + 1) << 5)));
                }
                gCurTask->player->unk42 |= 16;
                break;
            }
            break;
        case 4:
            if (t->unk30 != -1) {
                LoadPlayerBodyBoxRect(t->player->playerIndex, (u8 *)gUnk_0873BF64 + t->unk30 * 8);
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                             (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
            }
            break;
        case 12:
            if (t->unk30 != -1) {
                LoadPlayerBodyBoxRect(t->player->playerIndex, (u8 *)gUnk_0873C1C4 + t->unk30 * 8);
                LoadPlayerHitBoxSet(gCurTask->player->playerIndex,
                             (s32)((u8 *)gUnk_0873CEF4 + gCurTask->unk30 * 8));
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                             (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
                TaskBreakBlocks((struct HitBoxSet *)&gPlayerHitBoxSets[gCurTask->player->playerIndex],
                             gCurTask->player->playerIndex);
            }
            break;
        case 14:
            RegisterCollider(gCurTaskIdx, t->pixelX, t->pixelY, gUnk_0873C214);
            TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CF4C, gCurTask->player->playerIndex);
            break;
        }
    }
}
