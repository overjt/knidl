#include "gba/gba.h"
#include "global.h"
#include "task.h"

/* player_47fe8.c (0x08047FE8-0x08049737, issue #88).
 *
 * Player action bodies, part 15: action 29 and per-frame handler 26.
 * sub_08047fe8 (action 29, mode 19) is the ability get: it freezes the
 * stage (gUnk_03001F34 = 1), starts the palette fade BeginFade over
 * the rows gUnk_0873B534[player] selects (unless gUnk_030023B0 is set)
 * and, when the swallowed object gives a random ability
 * (PlayerState.unk0A > 1), spins the HUD roulette: PlayerState.unk0B
 * steps through abilities 1-24 with the delays gUnk_0873B634[] until A
 * or B is pressed.  It then shows the new ability on the HUD
 * (sub_08009fcc, sub_0800a0dc), loads its sprite tiles (M13's
 * LoadAbilityTiles) and plays the ability's own pose - a 25-way switch on
 * PlayerState.unk0D - 1 whose arms install hit boxes, spawn effects
 * and load extra tiles (sub_08049a58) - before it restores the palette
 * and the mode (the fade back, BeginFade(4, 2, ...)) and unfreezes
 * the stage.  Its handler sub_08049484 hands
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
extern u32 gUnk_0873C214[];             /* collider row passed to sub_0801a828 (4th arg) */
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
s32 sub_08009fcc(s32 a, s32 b, u32 c);
void sub_0800a0dc(s32 a, s32 b);
void sub_0800a178(s32 a, s32 id);
void sub_0801a828(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u16 sub_08030848(struct HitBoxSet *p, s32 e);
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

void sub_08047fe8(void)
{
    gCurTask->unk88->unk05 = gCurTask->unk88->unk04;
    gCurTask->unk88->unk04 = 19;
    gCurTask->unk15 = 26;
    gUnk_03001F34 = 1;
    gCurTask->unk88->unk3D = 0;
    PlayerStopAxes(3);
    {
        struct Task *t = gCurTask;
        t->unk28 = 0;
        t->unk2C = 0;
        t->unk88->unk04 = 13;
    }
    gCurTask->unk88->unk16 = 0xFF;
    {
        struct Task *t = gCurTask;
        t->unk88->unk40 |= 0x100;
        t->unk88->unk42 |= 0x720;
        t->unk88->unk42 &= 0xFFEF;
    }
    FreezeOtherTasks(15);
    if (gUnk_030023B0 == 0)
        BeginFade(4, -2, gUnk_0873B534[gCurTask->unk88->unk00]);
    {
        struct PlayerState *p = gCurTask->unk88;
        if (p->unk37 == 1) {
            sub_0803f5fc(p->unk00);
            PlayerStopSfx();
            gCurTask->unk88->unk06 = 0;
            {
                struct PlayerState *q = gCurTask->unk88;
                sub_08009fcc((s8)q->unk0B, (s8)q->unk0C, q->unk00);
            }
            gCurTask->unk88->unk0B = 0;
            gCurTask->unk88->unk0C |= 0xFF;
            {
                struct PlayerState *q = gCurTask->unk88;
                sub_0800a0dc(q->unk0D, q->unk00);
            }
            CreatePlayerEffect(gCurTask->unk88->unk00, 14, 0);
            CreatePlayerEffect(gCurTask->unk88->unk00, 14, 1);
            CreatePlayerEffect(gCurTask->unk88->unk00, 14, 2);
            CreatePlayerEffect(gCurTask->unk88->unk00, 14, 3);
            TaskYieldTrampoline(4);
        } else {
            p->unk37 = 0;
            gCurTask->unk88->unk06 = 0;
            {
                struct PlayerState *q = gCurTask->unk88;
                if (q->unk0A > 1) {
                    q->unk0A = 0;
                    sub_0800a0dc(27, gCurTask->unk88->unk00);
                    TaskYieldTrampoline(6);
                    gCurTask->unk6C = 0;
                    {
                        /* the ROM loads the key cell's address before the table's:
                           a programmer's pointer, hoisted ahead of loop.c's movables */
                        u16 *keys = gLatchedHeldKeys;

                        while (1) {
                            if ((s8)++gCurTask->unk88->unk0B > 24)
                                gCurTask->unk88->unk0B = 1;
                            PlaySfx(101);
                            {
                                struct PlayerState *r = gCurTask->unk88;
                                sub_0800a0dc((s8)r->unk0B, r->unk00);
                            }
                            {
                                struct Task *t = gCurTask;
                                if (keys[t->unk88->unk00] & 3)
                                    break;
                                if ((t->unk10 = gUnk_0873B634[(s16)t->unk6C++]) == 0)
                                    break;
                                TaskYieldTrampoline((s16)t->unk10);
                            }
                        }
                    }
                    sub_0800a178(0x7FFF, gCurTask->unk88->unk00);
                    switch ((s8)gCurTask->unk88->unk0B) {
                    default:
                        gCurTask->unk88->unk0C = 0xFF;
                        break;
                    case 7:
                        gCurTask->unk88->unk0C = 3;
                        break;
                    case 11:
                    case 20:
                    case 21:
                        gCurTask->unk88->unk0C = 1;
                        break;
                    }
                }
            }
            CreatePlayerEffect(gCurTask->unk88->unk00, 14, 0);
            CreatePlayerEffect(gCurTask->unk88->unk00, 14, 1);
            CreatePlayerEffect(gCurTask->unk88->unk00, 14, 2);
            CreatePlayerEffect(gCurTask->unk88->unk00, 14, 3);
            TaskSetFrame(68);
            TaskYieldTrampoline(2);
            PlaySfx(113);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(8);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            {
                struct PlayerState *q = gCurTask->unk88;
                sub_08009fcc((s8)q->unk0B, (s8)q->unk0C, q->unk00);
            }
            {
                struct PlayerState *q = gCurTask->unk88;
                sub_0800a0dc(q->unk0D, q->unk00);
            }
        }
    }
    gCurTask->unk88->unk0B = 0;
    PlaySfx(108);
    LoadAbilityTiles();
    {
        struct Task *t = gCurTask;
        t->unk88->unk42 |= 128;
        switch ((s8)(t->unk88->unk0D - 1)) {
        case 24:
            gCurTask->unk88->unk37 = 3;
            break;
        case 14:
            if (gCurTask->unk43 == 1)
                TaskSetFrameFlip(0xB1A);
            else
                TaskSetFrameNoFlip(0xB1A);
            TaskYieldTrampoline(32);
            break;
        case 21:
            if (gCurTask->unk43 == 1)
                TaskSetFrameFlip(0xE78);
            else
                TaskSetFrameNoFlip(0xE78);
            TaskYieldTrampoline(32);
            break;
        case 22:
            if (gCurTask->unk43 == 1)
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
            gCurTask->unk88->unk22 = 2;
            break;
        case 0:
            TaskSetFrame(0x289);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            TaskSetFrame(0x28C);
            TaskYieldTrampoline(1);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            CreatePlayerObject(gCurTask->unk88->unk00, 4, 0);
            CreatePlayerObject(gCurTask->unk88->unk00, 4, 1);
            CreatePlayerEffect(gCurTask->unk88->unk00, 29, 0);
            CreatePlayerEffect(gCurTask->unk88->unk00, 29, 1);
            CreatePlayerEffect(gCurTask->unk88->unk00, 29, 2);
            CreatePlayerEffect(gCurTask->unk88->unk00, 28, 4);
            TaskSetFrame(0x279);
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 14);
            TaskSetFrame(0x28D);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            sub_08049a58();
            CreatePlayerEffect(gCurTask->unk88->unk00, 15, 0);
            break;
        case 1:
            gCurTask->unk2C = 1;
            TaskSetFrame(0x36A);
            TaskYieldTrampoline(2);
            gCurTask->unk73 = 1;
            CreatePlayerEffect(gCurTask->unk88->unk00, 30, 0);
            CreatePlayerEffect(gCurTask->unk88->unk00, 30, 1);
            CreatePlayerEffect(gCurTask->unk88->unk00, 30, 2);
            for (gCurTask->unk6C = 0; (s16)gCurTask->unk6C <= 1; gCurTask->unk6C++) {
                TaskSetFrame(0x36B);
                TaskYieldTrampoline(2);
                gCurTask->unk6C = 0;
                do {
                    gCurTask->unk3C++;
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
            CreatePlayerEffect(gCurTask->unk88->unk00, 15, 0);
            break;
        case 2:
            TaskSetFrame(0x3E9);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(10);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            {
                struct Task *u = gCurTask;
                if (u->unk7A & 1)
                    CreatePlayerEffect(u->unk88->unk00, 28, 3);
            }
            CreatePlayerObject(gCurTask->unk88->unk00, 5, 0);
            gCurTask->unk3C++;
            TaskYieldTrampoline(4);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            break;
        case 3:
            TaskSetFrame(0x46A);
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 2);
            CreatePlayerEffect(gCurTask->unk88->unk00, 3, 0);
            gCurTask->unk3C++;
            TaskYieldTrampoline(16);
            if (gCurTask->unk88->unk40 & 32)
                gUnk_02007D00[2]++;
            break;
        case 4:
            TaskSetFrame(0x5CB);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk88->unk6C = gUnk_0873CCA4;
            TaskSetFrame(0x5E1);
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0x5D3);
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                TaskSetFrame(0x5D9);
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 1);
            gCurTask->unk88->unk6C = 0;
            TaskSetFrame(0x5DF);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            PlayerStopSfx();
            CreatePlayerEffect(gCurTask->unk88->unk00, 32, 5);
            CreatePlayerEffect(gCurTask->unk88->unk00, 32, 6);
            CreatePlayerEffect(gCurTask->unk88->unk00, 32, 7);
            CreatePlayerEffect(gCurTask->unk88->unk00, 32, 8);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            sub_08049a58();
            CreatePlayerEffect(gCurTask->unk88->unk00, 15, 0);
            break;
        case 5:
            TaskSetFrame(0x658);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            TaskSetFrame(0x65A);
            TaskYieldTrampoline(2);
            CreatePlayerObject(gCurTask->unk88->unk00, 6, 0);
            gCurTask->unk6C = 0;
            do {
                gCurTask->unk3C++;
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
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 7);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0x70A);
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 2);
            PlayerStopSfx();
            TaskSetFrame(0x702);
            TaskYieldTrampoline(1);
            gCurTask->unk6C = 0;
            do {
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 6);
            TaskSetFrame(0x701);
            TaskYieldTrampoline(1);
            break;
        case 8:
            TaskSetFrame(0x7DA);
            TaskYieldTrampoline(4);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0x78B);
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 1);
            break;
        case 9:
            {
                struct M11R20 *d = gPlayerBodyBoxes;

                d[gCurTask->unk88->unk00] = *(struct M11R20 *)gUnk_0873C358;
            }
            gPlayerHitBoxSets[gCurTask->unk88->unk00] = *(struct M11R8 *)gUnk_0873CF94;
            TaskSetFrame(0x8DB);
            TaskYieldTrampoline(4);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0x88C);
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 2);
            break;
        case 11:
            {
                struct Task *u = gCurTask;
                u->unk2C = 1;
                gPlayerBodyBoxes[u->unk88->unk00] = *(struct M11R20 *)gUnk_0873C1B0;
            }
            {
                struct Task *u;
                gPlayerHitBoxSets[(u = gCurTask)->unk88->unk00] = *(struct M11R8 *)gUnk_0873CEEC;
                u->unk30 = -1;
            }
            TaskSetFrame(0x96E);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk30++;
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk30++;
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk30++;
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            CreatePlayerEffect(gCurTask->unk88->unk00, 39, 0);
            CreatePlayerEffect(gCurTask->unk88->unk00, 39, 1);
            gCurTask->unk30 = 3;
            TaskSetFrame(0x978);
            TaskYieldTrampoline(2);
            gCurTask->unk30 = 4;
            TaskSetFrame(0x973);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk30 = 1;
            gCurTask->unk3C = 0x971;
            TaskYieldTrampoline(2);
            gCurTask->unk3C = 0x96E;
            TaskYieldTrampoline(2);
            break;
        case 12:
            TaskSetFrame(0xA0A);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C--;
            TaskYieldTrampoline(2);
            TaskSetFrame(0xA0D);
            TaskYieldTrampoline(1);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            CreatePlayerObject(gCurTask->unk88->unk00, 7, 0);
            CreatePlayerObject(gCurTask->unk88->unk00, 7, 1);
            CreatePlayerEffect(gCurTask->unk88->unk00, 40, 0);
            CreatePlayerEffect(gCurTask->unk88->unk00, 40, 1);
            CreatePlayerEffect(gCurTask->unk88->unk00, 40, 2);
            CreatePlayerEffect(gCurTask->unk88->unk00, 28, 4);
            TaskSetFrame(0x9FA);
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 14);
            TaskSetFrame(0xA0E);
            TaskYieldTrampoline(3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            break;
        case 13:
            TaskSetFrame(0xA8E);
            TaskYieldTrampoline(2);
            {
                struct Task *u = gCurTask;
                u->unk2C = 1;
                CreatePlayerEffect(u->unk88->unk00, 41, 0);
            }
            CreatePlayerEffect(gCurTask->unk88->unk00, 41, 1);
            CreatePlayerEffect(gCurTask->unk88->unk00, 41, 2);
            CreatePlayerEffect(gCurTask->unk88->unk00, 41, 3);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0xA86);
                TaskYieldTrampoline(1);
                gCurTask->unk6E = 0;
                do {
                    gCurTask->unk3C++;
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
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            CreatePlayerObject(gCurTask->unk88->unk00, 8, 0);
            CreatePlayerObject(gCurTask->unk88->unk00, 8, 1);
            CreatePlayerObject(gCurTask->unk88->unk00, 8, 2);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0xBBF);
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
            } while ((s16)++gCurTask->unk6C <= 1);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0xBC1);
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
                TaskYieldTrampoline(3);
            } while ((s16)++gCurTask->unk6C <= 1);
            gCurTask->unk6C = 0;
            do {
                TaskSetFrame(0xBC3);
                TaskYieldTrampoline(2);
                gCurTask->unk3C++;
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
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            TaskSetFrame(0xC48);
            TaskYieldTrampoline(6);
            TaskSetFrame(0xC4D);
            TaskYieldTrampoline(2);
            CreatePlayerEffect(gCurTask->unk88->unk00, 43, 0);
            CreatePlayerEffect(gCurTask->unk88->unk00, 43, 1);
            CreatePlayerEffect(gCurTask->unk88->unk00, 43, 2);
            CreatePlayerEffect(gCurTask->unk88->unk00, 43, 3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(3);
            gCurTask->unk6C = 0;
            do {
                gCurTask->unk3C++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->unk6C <= 3);
            gCurTask->unk3C++;
            TaskYieldTrampoline(1);
            break;
        case 17:
            TaskSetFrame(0xCCE);
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk3C++;
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                gCurTask->unk3C = 0xCDA;
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                gCurTask->unk3C = 0xCD2;
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
                gCurTask->unk3C++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 1);
            CreatePlayerEffect(gCurTask->unk88->unk00, 44, 0x500);
            CreatePlayerEffect(gCurTask->unk88->unk00, 44, 0x501);
            CreatePlayerEffect(gCurTask->unk88->unk00, 44, 0x502);
            CreatePlayerEffect(gCurTask->unk88->unk00, 44, 0x503);
            break;
        case 18:
            TaskSetFrame(0xDC5);
            TaskYieldTrampoline(2);
            gCurTask->unk6C = 0;
            do {
                gCurTask->unk3C += 2;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 10);
            {
                struct Task *u = gCurTask;
                if (u->unk43 == 1) {
                    u->unk3C = 0xDDC;
                    TaskYieldTrampoline(2);
                } else {
                    u->unk3C = 0xDDB;
                    TaskYieldTrampoline(2);
                }
            }
            gCurTask->unk6C = 0;
            do {
                gCurTask->unk3C -= 2;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->unk6C <= 10);
            TaskSetFrame(0xDE5);
            TaskYieldTrampoline(1);
            break;
        case 10:
            break;
        }
    }
    gCurTask->unk88->unk04 = 19;
    if (gUnk_030023B0 == 0) {
        BeginFade(4, 2, gUnk_0873B534[0]);
        TaskYieldTrampoline(4);
        gBrightness = 0;
    }
    FreezeOtherTasks(0);
    {
        struct Task *t = gCurTask;
        struct PlayerState *p;
        t->unk88->unk42 &= 0xF85F;
        p = t->unk88;
        if (p->unk3F == 1)
            p->unk12 += 6;
        else
            SetPlayerInvulnerability(4, 6, p->unk00);
    }
    {
        struct Task *t = gCurTask;
        t->unk28++;
        t->unk88->unk40 &= 0xFEFF;
    }
    gUnk_03001F34 = 0;
    TaskSleepForever();
}

void sub_08049484(void)
{
    struct Task *t = gCurTask;

    if (t->unk28 != 0) {
        struct PlayerState *p = t->unk88;
        switch (p->unk0D) {
        case 11:
            p->unk01 = 42;
            break;
        case 24:
            p->unk01 = 55;
            break;
        default:
            sub_08040710();
            break;
        }
    } else if (t->unk2C != 0) {
        switch (t->unk88->unk0D) {
        case 2:
            switch (t->unk3C) {
            default:
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
        case 4:
            if (t->unk30 != -1) {
                LoadPlayerBodyBoxRect(t->unk88->unk00, (u8 *)gUnk_0873BF64 + t->unk30 * 8);
                sub_0801a828(gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A,
                             (u8 *)gPlayerBodyBoxes + gCurTask->unk88->unk00 * 20);
            }
            break;
        case 12:
            if (t->unk30 != -1) {
                LoadPlayerBodyBoxRect(t->unk88->unk00, (u8 *)gUnk_0873C1C4 + t->unk30 * 8);
                LoadPlayerHitBoxSet(gCurTask->unk88->unk00,
                             (s32)((u8 *)gUnk_0873CEF4 + gCurTask->unk30 * 8));
                sub_0801a828(gCurTaskIdx, gCurTask->unk48, gCurTask->unk4A,
                             (u8 *)gPlayerBodyBoxes + gCurTask->unk88->unk00 * 20);
                sub_08030848((struct HitBoxSet *)&gPlayerHitBoxSets[gCurTask->unk88->unk00],
                             gCurTask->unk88->unk00);
            }
            break;
        case 14:
            sub_0801a828(gCurTaskIdx, t->unk48, t->unk4A, gUnk_0873C214);
            sub_08030848((struct HitBoxSet *)gUnk_0873CF4C, gCurTask->unk88->unk00);
            break;
        }
    }
}
