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

/* ISO C forbids an empty translation unit. */
int gHeaderSmokeGame;
