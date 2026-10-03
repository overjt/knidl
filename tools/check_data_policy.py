#!/usr/bin/env python3
"""Mechanical check for the data policy (AGENTS.md, docs/data.md, issue #36).

The repository is public.  Assets (graphics, audio, text, level maps) are
never committed in any form; functional tables may become C once a consumer
proves their layout; everything else is committed as STRUCTURE (labels,
symbolic pointers, .incbin offsets).  This check fails on what can be
detected mechanically:

  1. every `data` segment of docs/analysis/segments.txt (except the
     hand-written cartridge header) is the section of exactly one data/*.s
     file (data/<segment>.s, or the file of its zone, which holds the
     pieces of a zone cut around C runs, #167), and no asm/ file opens a
     data segment's section;
  2. every data/*.s line is one of: a comment, a blank line, `.section`,
     `.global`, a label, `.incbin "baserom.gba", <offset>, <length>`,
     `.word <symbol>` / `.word <symbol>+<n>` (a symbol, never a number) or
     an alignment directive (`.align` / `.balign` / `.p2align`);
  3. `.incbin` anywhere under asm/ and data/ names only baserom.gba;
  4. a brace initializer in src/ or include/ with more than MAX_C_NUMBERS
     numeric literals carries a `data-policy: functional <why>` comment in
     the three lines above it.  A functional table proven by its consumer
     may be C; a long number list without that note is presumed to be an
     asset dump;
  5. no line under asm/, data/, src/ or include/ is a hex dump (more than
     MAX_HEX_BYTES byte-sized hex tokens in a row);
  6. no committed file is binary (holds a NUL byte), unless it is empty;
  7. no committed file has an asset format's extension (ASSET_EXTS).
     Assets are extracted at build time, never committed.

Whether a table is functional data or an asset is a judgement this check
cannot make; reviewers make it with docs/data.md section 1.

Code segments' literal pools and raw `.short` instruction halfwords are
code, not data, and are out of this check's scope (docs/data.md).

Needs no baserom: CI runs it on every push, next to the split-regeneration
check.  Run it with `make check-data`, or directly:

  python3 tools/check_data_policy.py
"""

import os
import re
import subprocess
import sys

MAX_C_NUMBERS = 256
FUNCTIONAL_NOTE = "data-policy: functional"
ASSET_EXTS = (
    ".png", ".bmp", ".gif", ".pal", ".gbapal", ".jasc", ".1bpp", ".2bpp",
    ".4bpp", ".8bpp", ".lz", ".lz77", ".rl", ".bin", ".mid", ".midi",
    ".aif", ".aiff", ".wav", ".pcm", ".sf2", ".ogg", ".mp3",
)
MAX_HEX_BYTES = 16
NOT_DATA_FILES = ("rom_header",)  # asm/rom_header.s, built from source

SYMBOL = r"[A-Za-z_.$][A-Za-z0-9_.$]*"
NUMBER = r"(?:0x[0-9A-Fa-f]+|\d+)"
DATA_LINE_RES = [
    re.compile(r"^$"),
    re.compile(r"^@.*$"),
    re.compile(r'^\.section\s+\.[A-Za-z0-9_]+\s*,\s*"a"$'),
    re.compile(r"^\.global\s+%s$" % SYMBOL),
    re.compile(r"^%s:$" % SYMBOL),
    re.compile(r'^\.incbin\s+"baserom\.gba"\s*,\s*%s\s*,\s*%s$'
               % (NUMBER, NUMBER)),
    re.compile(r"^\.word\s+%s(?:\s*\+\s*%s)?$" % (SYMBOL, NUMBER)),
    # a label inside the pointer word before it (docs/data.md 3.2)
    re.compile(r"^\.set\s+%s\s*,\s*\.\s*-\s*[123]$" % SYMBOL),
    re.compile(r"^\.(?:align|balign|p2align)\s+%s(?:\s*,\s*%s)*$"
               % (NUMBER, NUMBER)),
]
SECTION_RE = re.compile(r"^\s*\.section\s+\.([A-Za-z0-9_]+)")
INCBIN_RE = re.compile(r'^\s*\.incbin\s+"([^"]*)"')
HEX_BYTE_RUN_RE = re.compile(
    r"(?:(?:\b0x)?\b[0-9A-Fa-f]{2}\b[ ,]+){%d,}" % (MAX_HEX_BYTES + 1)
)
C_NUMBER_RE = re.compile(r"(?<![\w.])-?(?:0[xX][0-9A-Fa-f]+|\d+)[uUlL]*(?![\w.])")


def parse_segments(path):
    segs = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            p = line.split()
            segs.append((p[2], p[3]))
    return segs


def tracked_files():
    """Committed files (git ls-files), or a walk of the tree without the
    build products and ignored inputs when git is unavailable."""
    try:
        out = subprocess.run(
            ["git", "-c", "safe.directory=*", "ls-files", "-z"],
            capture_output=True, check=True,
        ).stdout
        return [p for p in out.decode().split("\0") if p]
    except (OSError, subprocess.CalledProcessError):
        skip_dirs = {".git", "build", "pending", "__pycache__"}
        paths = []
        for root, dirs, files in os.walk("."):
            dirs[:] = sorted(d for d in dirs if d not in skip_dirs)
            for name in sorted(files):
                if name.endswith((".gba", ".elf", ".map", ".o", ".pyc")):
                    continue
                paths.append(os.path.relpath(os.path.join(root, name)))
        return paths


def strip_c_comments(text):
    # keep the newlines, so offsets in the result map to the right lines
    text = re.sub(r"/\*.*?\*/", lambda m: re.sub(r"[^\n]", " ", m.group(0)),
                  text, flags=re.S)
    return re.sub(r"//[^\n]*", "", text)


def c_initializers(text):
    """Yield (offset, body) for every `= { ... }` initializer."""
    for m in re.finditer(r"=\s*\{", text):
        depth = 0
        i = m.end() - 1
        while i < len(text):
            if text[i] == "{":
                depth += 1
            elif text[i] == "}":
                depth -= 1
                if depth == 0:
                    yield m.start(), text[m.end():i]
                    break
            i += 1


def main():
    errors = []
    segs = parse_segments("docs/analysis/segments.txt")
    data_names = set(n for k, n in segs if k == "data") - set(NOT_DATA_FILES)

    # 1. data segments live in data/, nowhere else: each is the section
    # of exactly one data file (its zone's).
    opened = {}
    for fname in sorted(os.listdir("data")):
        if not fname.endswith(".s"):
            continue
        with open(os.path.join("data", fname)) as f:
            for line in f:
                m = SECTION_RE.match(line)
                if m:
                    opened.setdefault(m.group(1), []).append(fname)
    for name in sorted(data_names):
        files = opened.get(name, [])
        if len(files) != 1:
            errors.append("data segment %s is opened by %d data/*.s files"
                          " (%s), not one" % (name, len(files),
                                              ", ".join(files) or "none"))
    for root in ("asm",):
        for dirpath, _dirs, files in os.walk(root):
            for fname in sorted(files):
                if not fname.endswith(".s"):
                    continue
                path = os.path.join(dirpath, fname)
                with open(path) as f:
                    for line in f:
                        m = SECTION_RE.match(line)
                        if m and m.group(1) in data_names:
                            errors.append(
                                "%s: data segment .%s belongs in data/"
                                % (path, m.group(1))
                            )

    # 2. data/*.s grammar; 3. incbin only of baserom.gba.
    for dirpath in ("data", "asm"):
        for root, _dirs, files in os.walk(dirpath):
            for fname in sorted(files):
                if not fname.endswith((".s", ".inc")):
                    continue
                path = os.path.join(root, fname)
                with open(path) as f:
                    lines = f.read().splitlines()
                for lineno, raw in enumerate(lines, 1):
                    m = INCBIN_RE.match(raw)
                    if m and m.group(1) != "baserom.gba":
                        errors.append("%s:%d: .incbin of %r (only baserom.gba"
                                      " may be extracted)"
                                      % (path, lineno, m.group(1)))
                    if dirpath != "data":
                        continue
                    line = raw.strip()
                    if not any(r.match(line) for r in DATA_LINE_RES):
                        errors.append("%s:%d: not structure: %s"
                                      % (path, lineno, line[:72]))

    # 4. long numeric C initializers need a functional note; 5. no hex dumps.
    for root in ("src", "include", "asm", "data"):
        for dirpath, _dirs, files in os.walk(root):
            for fname in sorted(files):
                path = os.path.join(dirpath, fname)
                if not fname.endswith((".c", ".h", ".s", ".inc")):
                    continue
                with open(path, errors="replace") as f:
                    text = f.read()
                if fname.endswith((".c", ".h")):
                    clean = strip_c_comments(text)
                    raw_lines = text.splitlines()
                    for off, body in c_initializers(clean):
                        n = len(C_NUMBER_RE.findall(body))
                        if n > MAX_C_NUMBERS:
                            lineno = clean.count("\n", 0, off) + 1
                            above = raw_lines[max(0, lineno - 4):lineno]
                            if any(FUNCTIONAL_NOTE in l for l in above):
                                continue
                            errors.append(
                                "%s:%d: initializer with %d numeric literals"
                                " and no '%s <why>' note (assets are never"
                                " committed; docs/data.md section 1)"
                                % (path, lineno, n, FUNCTIONAL_NOTE)
                            )
                for lineno, line in enumerate(text.splitlines(), 1):
                    if HEX_BYTE_RUN_RE.search(line + " "):
                        errors.append("%s:%d: hex dump: %s"
                                      % (path, lineno, line.strip()[:72]))

    # 6. no committed binary files; 7. no asset-format files.
    for path in tracked_files():
        if path.lower().endswith(ASSET_EXTS):
            errors.append("%s: asset-format file committed (assets are"
                          " extracted at build time)" % path)
            continue
        try:
            with open(path, "rb") as f:
                blob = f.read()
        except OSError:
            continue
        if blob and b"\0" in blob:
            errors.append("%s: binary file committed" % path)

    if errors:
        for e in errors[:200]:
            print("error: " + e)
        if len(errors) > 200:
            print("error: ... and %d more" % (len(errors) - 200))
        print("data policy check FAILED (%d problem(s)); see docs/data.md"
              % len(errors))
        sys.exit(1)
    print("data policy check passed: data/ holds structure only, no assets"
          " committed")


if __name__ == "__main__":
    main()
