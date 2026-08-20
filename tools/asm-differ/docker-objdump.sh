#!/usr/bin/env bash
# docker-objdump.sh — shim that runs arm-none-eabi-objdump inside the
# knidl-builder Docker image.
#
# asm-differ sets config["objdump_executable"] to this script when
# arm-none-eabi-objdump is not on the host PATH.  All arguments passed to
# this script are forwarded verbatim to the real objdump inside the container.
#
# The current directory is mounted at /src so that file arguments (e.g.
# baserom.gba, knidl.gba) resolve correctly.

set -euo pipefail

IMAGE="knidl-builder"

if ! docker image inspect "$IMAGE" &>/dev/null; then
    echo "Docker image '$IMAGE' not found.  Run 'make image' first." >&2
    exit 1
fi

exec docker run --rm \
    -v "$(pwd):/src" \
    -w /src \
    "$IMAGE" \
    arm-none-eabi-objdump "$@"
