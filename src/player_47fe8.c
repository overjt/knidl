#include "gba/gba.h"
#include "global.h"
#include "task.h"
#include "main.h"
#include "hud.h"
#include "room.h"
#include "player.h"
#include "effect.h"
#include "actor.h"

/* player_47fe8.c (0x08047FE8-0x08049737, issue #88).
 *
 * Player action bodies, part 15: action 29 and per-frame handler 26.
 * PlayerActionGetAbility (action 29, mode 19) is the ability get: it freezes the
 * stage (gPauseDisabled = 1), starts the palette fade BeginFade over
 * the rows gUnk_0873B534[player] selects (unless gCreditsDemoSet is set)
 * and, when the swallowed object gives a random ability
 * (PlayerState.abilitySwallowCount > 1), spins the HUD roulette: PlayerState.pendingAbility
 * steps through abilities 1-24 with the delays gUnk_0873B634[] until A
 * or B is pressed.  It then shows the new ability on the HUD
 * (SetPlayerAbilityNoHud, HudShowAbilityAnimated), loads its sprite tiles (M13's
 * LoadAbilityTiles) and plays the ability's own pose - a 25-way switch on
 * PlayerState.ability - 1 whose arms install hit boxes, spawn effects
 * and load extra tiles (PlayerLoadSparkTiles) - before it restores the palette
 * and the mode (the fade back, BeginFade(4, 2, ...)) and unfreezes
 * the stage.  Its handler PlayerActionGetAbilityUpdate hands
 * over to the ability's own follow-up once Task.unk28 is set (action 42
 * for ability 11, 55 for 24, M11's PlayerRequestStandOrFall otherwise) and, while
 * Task.unk2C is set, drives the pose: ability 2 blends its palettes
 * gUnk_081BE6BC[] over the animation frames 0x36B-0x372 (the twin of
 * M12's PlayerActionSparkUpdate), abilities 4, 12 and 14 register their collider
 * and block hit-box rows. */

/* M09's hit-box set (src/block_30804.c); only a pointer is passed here */
struct HitBoxSet;
u32 BeginFade(u16 steps, s16 delta, u16 *mask);   /* delta is signed: the ROM passes -2 as movs/negs */
void BlendColors(u16 *src, u16 *dst, s32 ratio, s32 count, u16 *out);
s32 PlaySfx(s32 id);
void RegisterCollider(u8 a, s16 x, s16 y, void *p);   /* M11's caller spelling */
u16 TaskBreakBlocks(struct HitBoxSet *p, s32 e);

void CreatePlayerObject(s32 a, s32 b, s32 c);      /* M14, src/plobj_52f6c.c (defined s32 (s8, u8, s32); the result is unused here) */

void PlayerActionGetAbility(void)
{
    gCurTask->player->prevMode = gCurTask->player->mode;
    gCurTask->player->mode = 19;
    gCurTask->updateState = PLAYER_ACTION_HANDLER_GET_ABILITY;
    gPauseDisabled = 1;
    gCurTask->player->running = 0;
    PlayerStopAxes(3);
    {
        struct Task *t = gCurTask;
        t->playerActionDone28 = 0;
        t->playerGetAbilityAttackOn = 0;
        t->player->mode = 13;
    }
    gCurTask->player->unk16 = 0xFF;
    {
        struct Task *t = gCurTask;
        t->player->actionFlags |= PLAYER_ACTION_FLAG_GET_ABILITY;
        t->player->statusFlags |= 0x720;
        t->player->statusFlags &= 0xFFEF;
    }
    FreezeOtherTasks(15);
    if (gCreditsDemoSet == 0)
        BeginFade(4, -2, gUnk_0873B534[gCurTask->player->playerIndex]);
    {
        struct PlayerState *p = gCurTask->player;
        if (p->unk37 == 1) {
            FreePlayerEffectsAndObjects(p->playerIndex);
            PlayerStopSfx();
            gCurTask->player->mouthState = 0;
            {
                struct PlayerState *q = gCurTask->player;
                SetPlayerAbilityNoHud((s8)q->pendingAbility, (s8)q->pendingAbilityUses, q->playerIndex);
            }
            gCurTask->player->pendingAbility = ABILITY_NORMAL;
            gCurTask->player->pendingAbilityUses |= 0xFF;
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
                if (q->abilitySwallowCount > 1) {
                    q->abilitySwallowCount = 0;
                    HudShowAbilityAnimated(27, gCurTask->player->playerIndex);
                    TaskYieldTrampoline(6);
                    gCurTask->playerLoopCount = 0;
                    {
                        /* the ROM loads the key cell's address before the table's:
                           a programmer's pointer, hoisted ahead of loop.c's movables */
                        u16 *keys = gLatchedHeldKeys;

                        while (1) {
                            if ((s8)++gCurTask->player->pendingAbility > 24)
                                gCurTask->player->pendingAbility = ABILITY_FIRE;
                            PlaySfx(SE_CURSOR_MOVE);
                            {
                                struct PlayerState *r = gCurTask->player;
                                HudShowAbilityAnimated((s8)r->pendingAbility, r->playerIndex);
                            }
                            {
                                struct Task *t = gCurTask;
                                if (keys[t->player->playerIndex] & 3)
                                    break;
                                if ((t->sleepFrames = gUnk_0873B634[(s16)t->playerLoopCount++]) == 0)
                                    break;
                                TaskYieldTrampoline((s16)t->sleepFrames);
                            }
                        }
                    }
                    sub_0800a178(0x7FFF, gCurTask->player->playerIndex);
                    switch ((s8)gCurTask->player->pendingAbility) {
                    default:
                        gCurTask->player->pendingAbilityUses = 0xFF;
                        break;
                    case ABILITY_MIKE:
                        gCurTask->player->pendingAbilityUses = 3;
                        break;
                    case ABILITY_SLEEP:
                    case ABILITY_CRASH:
                    case ABILITY_LIGHT:
                        gCurTask->player->pendingAbilityUses = 1;
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
                SetPlayerAbilityNoHud((s8)q->pendingAbility, (s8)q->pendingAbilityUses, q->playerIndex);
            }
            {
                struct PlayerState *q = gCurTask->player;
                HudShowAbilityAnimated(q->ability, q->playerIndex);
            }
        }
    }
    gCurTask->player->pendingAbility = ABILITY_NORMAL;
    PlaySfx(108);
    LoadAbilityTiles();
    {
        struct Task *t = gCurTask;
        t->player->statusFlags |= PLAYER_STATUS_NO_ATTACK_SFX;
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
            gCurTask->player->paletteFlashMode = 2;
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
            CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_FIRE_BREATH, 0);
            CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_FIRE_BREATH, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_FIRE_BREATH_FLAMES, 0);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_FIRE_BREATH_FLAMES, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_FIRE_BREATH_FLAMES, 2);
            CreatePlayerEffect(gCurTask->player->playerIndex, 28, 4);
            TaskSetFrame(0x279);
            TaskYieldTrampoline(2);
            gCurTask->playerLoopCount = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->playerLoopCount <= 14);
            TaskSetFrame(0x28D);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            PlayerLoadSparkTiles();
            CreatePlayerEffect(gCurTask->player->playerIndex, 15, 0);
            break;
        case 1:
            gCurTask->playerGetAbilityAttackOn = 1;
            TaskSetFrame(0x36A);
            TaskYieldTrampoline(2);
            gCurTask->variant = 1;
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_SPARK_AURA, 0);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_SPARK_AURA, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_SPARK_AURA, 2);
            for (gCurTask->playerLoopCount = 0; (s16)gCurTask->playerLoopCount <= 1; gCurTask->playerLoopCount++) {
                TaskSetFrame(0x36B);
                TaskYieldTrampoline(2);
                gCurTask->playerLoopCount = 0;
                do {
                    gCurTask->frame++;
                    TaskYieldTrampoline(2);
                } while ((s16)++gCurTask->playerLoopCount <= 6);
            }
            gCurTask->variant = 0xFF;
            PlayerStopSfx();
            TaskSetFrame(0x36A);
            TaskYieldTrampoline(2);
            TaskSetFrame(0x28F);
            TaskYieldTrampoline(1);
            PlayerLoadSparkTiles();
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
            CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_CUTTER_BLADE, 0);
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
            gCurTask->playerLoopCount = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->playerLoopCount <= 2);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_ABILITY_GET_SPARKLE, 0);
            gCurTask->frame++;
            TaskYieldTrampoline(16);
            if (gCurTask->player->actionFlags & PLAYER_ACTION_FLAG_METAKNIGHT_SWORD)
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
            gCurTask->playerLoopCount = 0;
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
            } while ((s16)++gCurTask->playerLoopCount <= 1);
            gCurTask->player->hitBoxSet = 0;
            TaskSetFrame(0x5DF);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            PlayerStopSfx();
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_BURNING_FLAMES, 5);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_BURNING_FLAMES, 6);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_BURNING_FLAMES, 7);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_BURNING_FLAMES, 8);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            PlayerLoadSparkTiles();
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
            CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_LASER_BEAM, 0);
            gCurTask->playerLoopCount = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->playerLoopCount <= 15);
            TaskSetFrame(0x658);
            TaskYieldTrampoline(2);
            break;
        case 7:
            TaskSetFrame(0x701);
            TaskYieldTrampoline(2);
            gCurTask->playerLoopCount = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->playerLoopCount <= 7);
            gCurTask->playerLoopCount = 0;
            do {
                TaskSetFrame(0x70A);
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->playerLoopCount <= 2);
            PlayerStopSfx();
            TaskSetFrame(0x702);
            TaskYieldTrampoline(1);
            gCurTask->playerLoopCount = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->playerLoopCount <= 6);
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
            gCurTask->playerLoopCount = 0;
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
            } while ((s16)++gCurTask->playerLoopCount <= 1);
            break;
        case 9:
            {
                struct PlayerBodyBox *d = gPlayerBodyBoxes;

                d[gCurTask->player->playerIndex] = *(struct PlayerBodyBox *)gUnk_0873C358;
            }
            gPlayerHitBoxSets[gCurTask->player->playerIndex] = *(struct PlayerHitBoxSet *)gUnk_0873CF94;
            TaskSetFrame(0x8DB);
            TaskYieldTrampoline(4);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->playerLoopCount = 0;
            do {
                TaskSetFrame(0x88C);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->playerLoopCount <= 2);
            break;
        case 11:
            {
                struct Task *u = gCurTask;
                u->playerGetAbilityAttackOn = 1;
                gPlayerBodyBoxes[u->player->playerIndex] = *(struct PlayerBodyBox *)gUnk_0873C1B0;
            }
            {
                struct Task *u;
                gPlayerHitBoxSets[(u = gCurTask)->player->playerIndex] = *(struct PlayerHitBoxSet *)gUnk_0873CEEC;
                u->playerAttackStep30 = -1;
            }
            TaskSetFrame(0x96E);
            TaskYieldTrampoline(2);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->playerAttackStep30++;
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->playerAttackStep30++;
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            gCurTask->playerAttackStep30++;
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            CreatePlayerEffect(gCurTask->player->playerIndex, 39, 0);
            CreatePlayerEffect(gCurTask->player->playerIndex, 39, 1);
            gCurTask->playerAttackStep30 = 3;
            TaskSetFrame(0x978);
            TaskYieldTrampoline(2);
            gCurTask->playerAttackStep30 = 4;
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
            gCurTask->playerAttackStep30 = 1;
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
            CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_ICE_BREATH, 0);
            CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_ICE_BREATH, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_ICE_BREATH_CLOUD, 0);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_ICE_BREATH_CLOUD, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_ICE_BREATH_CLOUD, 2);
            CreatePlayerEffect(gCurTask->player->playerIndex, 28, 4);
            TaskSetFrame(0x9FA);
            TaskYieldTrampoline(2);
            gCurTask->playerLoopCount = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->playerLoopCount <= 14);
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
                u->playerGetAbilityAttackOn = 1;
                CreatePlayerEffect(u->player->playerIndex, PLAYER_EFFECT_VARIANT_FREEZE_AURA, 0);
            }
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_FREEZE_AURA, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_FREEZE_AURA, 2);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_FREEZE_AURA, 3);
            gCurTask->playerLoopCount = 0;
            do {
                TaskSetFrame(0xA86);
                TaskYieldTrampoline(1);
                gCurTask->playerLoopCount6E = 0;
                do {
                    gCurTask->frame++;
                    TaskYieldTrampoline(1);
                } while (++gCurTask->playerLoopCount6E <= 6);
            } while ((s16)++gCurTask->playerLoopCount <= 2);
            gCurTask->playerGetAbilityAttackOn = 0;
            PlayerStopSfx();
            TaskSetFrame(0xA8E);
            TaskYieldTrampoline(1);
            break;
        case 15:
            TaskSetFrame(0xBBD);
            TaskYieldTrampoline(6);
            gCurTask->frame++;
            TaskYieldTrampoline(2);
            CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_BEAM_ORB, 0);
            CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_BEAM_ORB, 1);
            CreatePlayerObject(gCurTask->player->playerIndex, PLAYER_OBJECT_VARIANT_BEAM_ORB, 2);
            gCurTask->playerLoopCount = 0;
            do {
                TaskSetFrame(0xBBF);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
            } while ((s16)++gCurTask->playerLoopCount <= 1);
            gCurTask->playerLoopCount = 0;
            do {
                TaskSetFrame(0xBC1);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
            } while ((s16)++gCurTask->playerLoopCount <= 1);
            gCurTask->playerLoopCount = 0;
            do {
                TaskSetFrame(0xBC3);
                TaskYieldTrampoline(2);
                gCurTask->frame++;
                TaskYieldTrampoline(3);
            } while ((s16)++gCurTask->playerLoopCount <= 1);
            TaskSetFrame(0xBBF);
            TaskYieldTrampoline(4);
            break;
        case 16:
            TaskSetFrame(0xC40);
            TaskYieldTrampoline(1);
            gCurTask->playerLoopCount = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->playerLoopCount <= 3);
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
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_STONE_PUFF, 0);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_STONE_PUFF, 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_STONE_PUFF, 2);
            CreatePlayerEffect(gCurTask->player->playerIndex, PLAYER_EFFECT_VARIANT_STONE_PUFF, 3);
            gCurTask->frame++;
            TaskYieldTrampoline(3);
            gCurTask->playerLoopCount = 0;
            do {
                gCurTask->frame++;
                TaskYieldTrampoline(2);
            } while ((s16)++gCurTask->playerLoopCount <= 3);
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
            gCurTask->playerLoopCount = 0;
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
            } while ((s16)++gCurTask->playerLoopCount <= 1);
            CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x500);
            CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x501);
            CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x502);
            CreatePlayerEffect(gCurTask->player->playerIndex, 44, 0x503);
            break;
        case 18:
            TaskSetFrame(0xDC5);
            TaskYieldTrampoline(2);
            gCurTask->playerLoopCount = 0;
            do {
                gCurTask->frame += 2;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->playerLoopCount <= 10);
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
            gCurTask->playerLoopCount = 0;
            do {
                gCurTask->frame -= 2;
                TaskYieldTrampoline(1);
            } while ((s16)++gCurTask->playerLoopCount <= 10);
            TaskSetFrame(0xDE5);
            TaskYieldTrampoline(1);
            break;
        case 10:
            break;
        }
    }
    gCurTask->player->mode = 19;
    if (gCreditsDemoSet == 0) {
        BeginFade(4, 2, gUnk_0873B534[0]);
        TaskYieldTrampoline(4);
        gBrightness = 0;
    }
    FreezeOtherTasks(0);
    {
        struct Task *t = gCurTask;
        struct PlayerState *p;
        t->player->statusFlags &= 0xF85F;
        p = t->player;
        if (p->invulnerability == 1)
            p->invulnerabilityTimer += 6;
        else
            SetPlayerInvulnerability(4, 6, p->playerIndex);
    }
    {
        struct Task *t = gCurTask;
        t->playerActionDone28++;
        t->player->actionFlags &= 0xFEFF;
    }
    gPauseDisabled = 0;
    TaskSleepForever();
}

void PlayerActionGetAbilityUpdate(void)
{
    struct Task *t = gCurTask;

    if (t->playerActionDone28 != 0) {
        struct PlayerState *p = t->player;
        switch (p->ability) {
        case ABILITY_SLEEP:
            p->requestedAction = PLAYER_ACTION_SLEEP;
            break;
        case ABILITY_UFO:
            p->requestedAction = PLAYER_ACTION_UFO;
            break;
        default:
            PlayerRequestStandOrFall();
            break;
        }
    } else if (t->playerGetAbilityAttackOn != 0) {
        switch (t->player->ability) {
        case ABILITY_SPARK:
            switch (t->frame) {
            default:
            case 0x36B:
            case 0x36C:
            case 0x36F:
            case 0x370:
                {
                    struct Task *u = gCurTask;
                    u->player->statusFlags &= 0xFFEF;
                    u->playerNextBankBlendRatio = 0;
                    u->playerBankBlendRatio = 0;
                }
                break;
            case 0x36E:
            case 0x372:
                {
                    struct Task *u = gCurTask;
                    u->playerBankBlendRatio += 128;
                    if (u->playerBankBlendRatio > 256)
                        u->playerBankBlendRatio = 256;
                }
                {
                    struct Task *u = gCurTask;
                    BlendColors((u16 *)&gUnk_081BE6BC[u->player->playerIndex * 128],
                                 (u16 *)&gUnk_081BE6BC[u->player->playerIndex * 128 + 32],
                                 (u16)u->playerBankBlendRatio, 16,
                                 (u16 *)(gObjPalette + ((u->tileWord >> 12) << 5)));
                }
                /* fallthrough */
            case 0x36D:
            case 0x371:
                {
                    struct Task *u = gCurTask;
                    u->playerNextBankBlendRatio += 64;
                    if ((s16)u->playerNextBankBlendRatio > 256)
                        u->playerNextBankBlendRatio = 256;
                }
                {
                    struct Task *u = gCurTask;
                    BlendColors((u16 *)&gUnk_081BE6BC[u->player->playerIndex * 128 + 64],
                                 (u16 *)&gUnk_081BE6BC[u->player->playerIndex * 128 + 96],
                                 u->playerNextBankBlendRatio, 16,
                                 (u16 *)(gObjPalette + (((u->tileWord >> 12) + 1) << 5)));
                }
                gCurTask->player->statusFlags |= PLAYER_STATUS_PALETTE_LOCKED;
                break;
            }
            break;
        case ABILITY_SWORD:
            if (t->playerAttackStep30 != -1) {
                LoadPlayerBodyBoxRect(t->player->playerIndex, (u8 *)gUnk_0873BF64 + t->playerAttackStep30 * 8);
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                             (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
            }
            break;
        case ABILITY_NEEDLE:
            if (t->playerAttackStep30 != -1) {
                LoadPlayerBodyBoxRect(t->player->playerIndex, (u8 *)gUnk_0873C1C4 + t->playerAttackStep30 * 8);
                LoadPlayerHitBoxSet(gCurTask->player->playerIndex,
                             (s32)((u8 *)gUnk_0873CEF4 + gCurTask->playerAttackStep30 * 8));
                RegisterCollider(gCurTaskIdx, gCurTask->pixelX, gCurTask->pixelY,
                             (u8 *)gPlayerBodyBoxes + gCurTask->player->playerIndex * 20);
                TaskBreakBlocks((struct HitBoxSet *)&gPlayerHitBoxSets[gCurTask->player->playerIndex],
                             gCurTask->player->playerIndex);
            }
            break;
        case ABILITY_FREEZE:
            RegisterCollider(gCurTaskIdx, t->pixelX, t->pixelY, gUnk_0873C214);
            TaskBreakBlocks((struct HitBoxSet *)gUnk_0873CF4C, gCurTask->player->playerIndex);
            break;
        }
    }
}
