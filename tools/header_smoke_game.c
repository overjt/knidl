/*
 * tools/header_smoke_game.c - compile-only check of the subsystem headers
 * (issue #36 phase 2).  Never linked into the ROM; built by
 * `make check-headers` with agbcc.
 *
 * Every game header is included into ONE translation unit, so a symbol
 * declared twice with two types, a struct defined twice, or a header that
 * needs a definition it does not include fails here.  The per-file proof
 * that a header changes no byte is `make compare`.
 */

#include "gba/gba.h"
#include "global.h"
#include "main.h"
#include "task.h"
#include "link.h"
#include "sound.h"
#include "mode.h"
#include "hud.h"
#include "menu.h"
#include "cutscene.h"
#include "collision.h"
#include "room.h"
#include "camera.h"
#include "player.h"
#include "effect.h"
#include "actor.h"
#include "enemy.h"
#include "save.h"
#include "subgame.h"
#include "ending.h"

/* Layout checks for struct Task's views (#155 run 3, docs/header-conventions.md
 * "Views"): a union is padded to 4 bytes by agbcc unless it is packed
 * (lesson 3.522), which would move every later field, so the offsets are
 * checked here as well as by `make compare`.  A false condition gives the
 * array a negative size and fails the build. */
#define SMOKE_OFFSET(type, field) ((unsigned long)&((type *)0)->field)
#define SMOKE_ASSERT(name, cond) typedef char name[(cond) ? 1 : -1]
SMOKE_ASSERT(smokeTaskU80, SMOKE_OFFSET(struct Task, u80) == 0x80);
SMOKE_ASSERT(smokeTaskU80Size, sizeof(((struct Task *)0)->u80) == 1);
SMOKE_ASSERT(smokeTaskUnk81, SMOKE_OFFSET(struct Task, unk81) == 0x81);
SMOKE_ASSERT(smokeTaskHitEffect, SMOKE_OFFSET(struct Task, hitEffect) == 0x82);
SMOKE_ASSERT(smokeTaskU8C, SMOKE_OFFSET(struct Task, u8C) == 0x8C);
SMOKE_ASSERT(smokeTaskSize, sizeof(struct Task) == 0x90);

/* ISO C forbids an empty translation unit. */
int gHeaderSmokeGame;
