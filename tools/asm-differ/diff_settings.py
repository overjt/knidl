# diff_settings.py — asm-differ configuration for
# Kirby: Nightmare in Dream Land (GBA, USA, A7KE).
#
# Run from the repo root:
#   python3 tools/asm-differ/diff.py -mwo <symbol_or_address>
#
# Flags:
#   -m   re-run make before diffing (triggers Docker build)
#   -w   watch mode: auto-refresh on file changes
#   -o   show byte offsets in the diff columns
#
# Thumb vs ARM:
#   Most game code is Thumb.  For ARM-mode functions add the flag
#   --objdump-flags='-Mno-force-thumb' when invoking diff.py, or
#   temporarily set force_thumb=False below and revert when done.

import os
import shutil


def apply(config, args):
    # ── ROM images ─────────────────────────────────────────────────────────
    # asm-differ's "binary" mode compares two flat ROM blobs directly instead
    # of object files.  baseimg = reference; myimg = rebuilt ROM.
    config["baseimg"] = "baserom.gba"
    config["myimg"]   = "knidl.gba"

    # ── Linker map ─────────────────────────────────────────────────────────
    # The GNU ld map file lets diff.py resolve symbol names to addresses.
    config["mapfile"] = "build/knidl.map"

    # ── Architecture ───────────────────────────────────────────────────────
    config["arch"] = "armv4t"

    # ── objdump binary path ────────────────────────────────────────────────
    # arm-none-eabi-objdump lives only inside the Docker image; the wrapper
    # script below routes the call through Docker when running on the host.
    # If INSIDE_DOCKER=1 the binary is on PATH and the wrapper is a no-op.
    if os.environ.get("INSIDE_DOCKER") == "1" or shutil.which("arm-none-eabi-objdump"):
        config["objdump_executable"] = "arm-none-eabi-objdump"
    else:
        # Wrapper script: runs objdump inside the builder container.
        config["objdump_executable"] = "tools/asm-differ/docker-objdump.sh"

    # ── Base address ───────────────────────────────────────────────────────
    # GBA ROM is memory-mapped starting at 0x08000000.
    config["base_address"] = 0x08000000

    # ── Make rebuild support ───────────────────────────────────────────────
    # -m flag: re-run `make` (host-side) before diffing, which rebuilds via
    # Docker and updates knidl.gba.
    config["makeflags"] = []

    # ── Source directories (for context lines) ─────────────────────────────
    config["source_directories"] = ["src", "asm", "include"]

    # ── Thumb mode (default) ───────────────────────────────────────────────
    # Nearly all GBA game code is compiled/assembled as Thumb.  Switch to
    # False temporarily for ARM-mode (.arm) functions.
    config["thumb"] = True
