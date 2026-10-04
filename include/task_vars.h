#ifndef GUARD_TASK_VARS_H
#define GUARD_TASK_VARS_H

/*
 * Per-family names of struct Task's registers (#155 run 5).
 *
 * unk18-unk34, unk46, unk6C-unk70 and unk74 hold a different thing in every
 * task family: a timer, a counter, a child's slot.  Like pret's
 * `#define tState data[0]` over struct Task's data[16], each family names
 * the registers it uses with object-like macros, and its code writes
 * `gCurTask->fireLionHopCount` for `gCurTask->unk6C`.  A macro changes no
 * token after preprocessing; it is used only on a pointer proven to hold a
 * task of its family, and shared helpers keep the plain unkXX.  Each line
 * gives the member, the type the code reads it as (the sites keep their
 * casts) and the role.  Generated and checked by tools/task_alias.py
 * (docs/header-conventions.md, "Per-family registers").
 */

/* Actor - every actor: a task made by CreateActor, sub_08064a78 or
   sub_08064d9c (src/actor_63698.c), which also bind Task.u8C.actor; the room
   objects of kinds 0-6 come through CreateActor */
#define actorAnimDelay unk28 /* s32: frames until the next animation-script step (ActorStartAnim / ActorTickAnim) */
#define actorSpawnArg unk74 /* u8: the spawn argument (CreateActor's p4, ActorSpawn.spawnArg), set once at creation */

#endif // GUARD_TASK_VARS_H
