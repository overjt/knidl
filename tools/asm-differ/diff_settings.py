# diff_settings.py — asm-differ configuration for
# Kirby: Nightmare in Dream Land (GBA, USA, A7KE).
#
# Run from the repo root (docs/diffing.md):
#   cp -R build expected/                      # once, from a matching build
#   python3 tools/asm-differ/diff.py -o <C function>
#   python3 tools/asm-differ/diff.py <file offset> <end offset>   # binary mode
#
# Flags:
#   -o   diff the function's object against expected/build/... (by name)
#   -m   re-run make before diffing (triggers Docker build)
#   -w   watch mode: auto-refresh on file changes
#
# Thumb vs ARM:
#   Most game code is Thumb, so objdump runs with -Mforce-thumb.  For an
#   ARM-mode function (crt0, the task switcher) pass --arm.

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
    # asm-differ knows "arm32" (big-endian) and "armel"; the GBA is
    # little-endian ARMv4T.  ("armv4t" is not an asm-differ arch name: it
    # made every invocation stop with "Unknown architecture".)
    config["arch"] = "armel"
    config["objdump_flags"] = ["-marmv4t"] + ([] if getattr(args, "arm", False)
                                              else ["-Mforce-thumb"])

    # ── objdump binary path ────────────────────────────────────────────────
    # arm-none-eabi-objdump lives only inside the Docker image; the wrapper
    # script below routes the call through Docker when running on the host.
    # If INSIDE_DOCKER=1 the binary is on PATH and the wrapper is a no-op.
    if os.environ.get("INSIDE_DOCKER") == "1" or shutil.which("arm-none-eabi-objdump"):
        config["objdump_executable"] = "arm-none-eabi-objdump"
    else:
        # Wrapper script: runs objdump inside the builder container.
        config["objdump_executable"] = "tools/asm-differ/docker-objdump.sh"

    # ── Binary mode ────────────────────────────────────────────────────────
    # Without -o, asm-differ compares the two ROM images by FILE OFFSET
    # (VMA - 0x08000000, e.g. `diff.py 0x7300 0x75B8`).  It cannot look a
    # symbol up for binary mode: its GNU map parser wants overlay "load
    # address" lines, which a flat GBA link does not have.  Diff C
    # functions by name with -o against an expected/ copy of a matching
    # build/ (docs/diffing.md).

    # ── Make rebuild support ───────────────────────────────────────────────
    # -m flag: re-run `make` (host-side) before diffing, which rebuilds via
    # Docker and updates knidl.gba.
    config["makeflags"] = []

    # ── Source directories (for context lines) ─────────────────────────────
    config["source_directories"] = ["src", "asm", "include"]


def add_custom_arguments(parser):
    parser.add_argument("--arm", action="store_true",
                        help="disassemble as ARM instead of Thumb (crt0, the task switcher)")
