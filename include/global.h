#ifndef GUARD_GLOBAL_H
#define GUARD_GLOBAL_H

#include "gba/types.h"
#include "gba/defines.h"

/* Prevent cross-jump optimization in agbcc. */
#define BLOCK_CROSS_JUMP asm("");

/* Inline GAS comment — useful when diffing codegen. */
#define asm_comment(x) asm volatile("@ -- " x " -- ")

/* Convenience: array element count. */
#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))

/* Useful math helpers. */
#define abs(n)    (((n) >= 0) ? (n) : -(n))
#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) >= (b) ? (a) : (b))

#endif // GUARD_GLOBAL_H
