#!/bin/bash
set -euo pipefail
INPUT="$1"
OUTPUT="$3"
cpp -P "$INPUT" \
  | old_agbcc -O1 -mthumb-interwork \
  | { cat; printf ".text\n\t.align\t2, 0\n"; } \
  | arm-none-eabi-as -mcpu=arm7tdmi -o "$OUTPUT"
