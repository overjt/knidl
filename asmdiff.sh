#!/usr/bin/env bash
# asmdiff.sh — raw ARM/Thumb disassembly diff between baserom.gba and knidl.gba.
#
# Usage:  ./asmdiff.sh <start> <length> [--arm]
#   start   ROM address (decimal or hex, e.g. 0x08000000 or 134217728).
#   length  Number of bytes to disassemble (decimal or hex).
#   --arm   Treat the range as ARM code instead of Thumb (default: Thumb).
#
# Examples:
#   ./asmdiff.sh 0x080000C0 0x40          # crt0 stub, Thumb
#   ./asmdiff.sh 0x080000C0 0x40 --arm   # crt0 stub, ARM mode
#
# Runs objdump inside the knidl-builder Docker image so no host toolchain is
# needed.  Set INSIDE_DOCKER=1 to invoke objdump directly (used by the
# Docker entrypoint wrapper below).

set -euo pipefail

IMAGE="knidl-builder"
ROM_BASE="baserom.gba"
MY_ROM="knidl.gba"

# ── argument parsing ────────────────────────────────────────────────────────
if [[ $# -lt 2 ]]; then
    echo "Usage: $0 <start> <length> [--arm]" >&2
    exit 1
fi

START=$(($1))
LENGTH=$(($2))
STOP=$((START + LENGTH))
FORCE_ARM=0
if [[ "${3:-}" == "--arm" ]]; then
    FORCE_ARM=1
fi

# ── objdump flags ────────────────────────────────────────────────────────────
BASE_FLAGS="-D -bbinary -marmv4t"
if [[ $FORCE_ARM -eq 0 ]]; then
    BASE_FLAGS="$BASE_FLAGS -Mforce-thumb"
fi

# GBA ROM is memory-mapped at 0x08000000; adjust VMA only when the caller
# passes a fully-mapped address.
if [[ $START -ge $((0x08000000)) ]]; then
    RANGE_FLAGS="--adjust-vma=0x08000000 --start-address=$START --stop-address=$STOP"
else
    RANGE_FLAGS="--start-address=$START --stop-address=$STOP"
fi

# ── execution ────────────────────────────────────────────────────────────────
if [[ "${INSIDE_DOCKER:-0}" == "1" ]]; then
    # Running inside the builder container; objdump is on PATH.
    OBJDUMP="arm-none-eabi-objdump"
    # Strip the objdump file-format header line ("filename: file format binary")
    # before diffing — it varies by filename but carries no asm content.
    $OBJDUMP $BASE_FLAGS $RANGE_FLAGS "$ROM_BASE" | grep -v 'file format' > /tmp/baserom.dump
    $OBJDUMP $BASE_FLAGS $RANGE_FLAGS "$MY_ROM"   | grep -v 'file format' > /tmp/myrom.dump
    diff -u /tmp/baserom.dump /tmp/myrom.dump
else
    # Host-side: exec inside the Docker image.
    if ! docker image inspect "$IMAGE" &>/dev/null; then
        echo "Docker image '$IMAGE' not found.  Run 'make image' first." >&2
        exit 1
    fi
    if [[ ! -f "$MY_ROM" ]]; then
        echo "'$MY_ROM' not found.  Run 'make' first." >&2
        exit 1
    fi
    docker run --rm \
        -v "$(pwd):/src" \
        -w /src \
        -e INSIDE_DOCKER=1 \
        "$IMAGE" \
        bash asmdiff.sh "$1" "$2" ${3:-}
fi
