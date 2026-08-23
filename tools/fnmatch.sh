#!/usr/bin/env bash
# fnmatch.sh — byte-match a candidate C file against a ROM range WITHOUT
# touching the real build (issue #28 workflow, generalized).
#
# Usage:  ./tools/fnmatch.sh <start> <end> <file.c> [--old] [--no-werror]
#   start    ROM VMA where the compiled .text must land (e.g. 0x08000310).
#   end      first VMA NOT in the range (exclusive, e.g. 0x080008E8).
#   file.c   candidate source (usually src/<module>.c).
#   --old    compile with old_agbcc -O1 (SDK zone recipe) instead of the
#            default agbcc -O2 -mthumb-interwork (game-code zone).
#   --old2   compile with old_agbcc -O2 (m4a driver zone, issue #53).
#   --no-werror  drop -Werror while iterating (warnings won't abort).
#
# What it does:
#   1. Compiles file.c through the exact Makefile pipeline (cpp | agbcc |
#      append ".align 2,0" | as).
#   2. Auto-generates stand-ins for every undefined symbol:
#        - names found in docs/analysis/symbols.csv -> zero-size .thumb_func
#          (or .arm) labels pinned at their real VMA (lesson 4.18: --defsym
#          absolutes would make ld insert interworking veneers);
#        - gUnk_<hexaddr> names and split_config.json data_symbols ->
#          absolute symbols (RAM cells, only ever referenced in pools).
#   3. Links alone at <start>, extracts the section, byte-compares against
#      the same slice of baserom.gba.
#   4. On mismatch, prints a pool-resolving instruction diff
#      (tools/fnmatch_diff.py) — literal-pool layout shifts are folded away
#      so only real differences show.
#
# Exit status: 0 on MATCH, 1 on mismatch, 2 on usage/setup errors.
# Scratch lives in build/fnmatch/ (regenerable; make clean deletes it).
#
# Runs itself inside the knidl-builder Docker image (same pattern as
# asmdiff.sh); set INSIDE_DOCKER=1 to run the toolchain directly.

set -euo pipefail

IMAGE="knidl-builder"

if [[ $# -lt 3 ]]; then
    sed -n '2,28p' "$0" >&2
    exit 2
fi

START_RAW="$1"; END_RAW="$2"; CFILE="$3"; shift 3
RECIPE="new"; WERROR="-Werror"
for a in "$@"; do
    case "$a" in
        --old) RECIPE="old" ;;
        --old2) RECIPE="old2" ;;
        --no-werror) WERROR="" ;;
        *) echo "unknown flag: $a" >&2; exit 2 ;;
    esac
done

if [[ "${INSIDE_DOCKER:-0}" != "1" ]]; then
    if ! docker image inspect "$IMAGE" &>/dev/null; then
        echo "Docker image '$IMAGE' not found. Run 'make image' first." >&2
        exit 2
    fi
    exec docker run --rm -v "$(pwd):/src" -w /src -e INSIDE_DOCKER=1 \
        "$IMAGE" bash tools/fnmatch.sh "$START_RAW" "$END_RAW" "$CFILE" \
        $( [[ "$RECIPE" == "old" ]] && echo --old ) \
        $( [[ "$RECIPE" == "old2" ]] && echo --old2 ) \
        $( [[ -z "$WERROR" ]] && echo --no-werror )
fi

START=$((START_RAW)); END=$((END_RAW)); SIZE=$((END - START))
if (( SIZE <= 0 )); then echo "end must be > start" >&2; exit 2; fi

D="build/fnmatch/$(basename "$CFILE" .c)_$(printf %08x "$START")"
mkdir -p "$D"

# ── 1. compile exactly like the Makefile ────────────────────────────────────
if [[ "$RECIPE" == "old" ]]; then
    CCLINE=(old_agbcc -O1 -mthumb-interwork)
elif [[ "$RECIPE" == "old2" ]]; then
    CCLINE=(old_agbcc -O2 -mthumb-interwork)
else
    CCLINE=(agbcc -O2 -mthumb-interwork -Wimplicit -Wparentheses $WERROR -fhex-asm)
fi
cpp -P -I include "$CFILE" \
  | "${CCLINE[@]}" -o - - \
  | { cat; printf '.text\n\t.align\t2, 0\n'; } \
  | arm-none-eabi-as -mcpu=arm7tdmi -o "$D/cand.o" -

# ── 2. auto stand-ins + linker script for undefined symbols ─────────────────
arm-none-eabi-nm -u "$D/cand.o" | awk '{print $NF}' | sort -u > "$D/undef.txt"

python3 - "$D" "$START" <<'PYEOF'
import csv, json, re, sys

d, start = sys.argv[1], int(sys.argv[2])
undef = [l.strip() for l in open(d + '/undef.txt') if l.strip()]

db = {}
for row in csv.reader(open('docs/analysis/symbols.csv')):
    if row and row[0].startswith('0x'):
        db[row[4]] = (int(row[0], 16), row[2])
cfg = json.load(open('tools/split_config.json'))
data_syms = {v: int(k, 16) for k, v in cfg.get('data_symbols', {}).items()}
# extra_labels (ROM data labels, only pool-referenced from C) and
# abs_symbols (link-time constants) resolve as absolutes too.
data_syms.update({v: int(k, 16) for k, v in cfg.get('extra_labels', {}).items()})
data_syms.update({k: int(v, 16) for k, v in cfg.get('abs_symbols', {}).items()})

labels, absolutes, unknown = [], [], []
for name in undef:
    m = re.fullmatch(r'gUnk_([0-9A-Fa-f]{8})', name)
    if name in db:
        addr, mode = db[name]
        labels.append((name, addr, mode))
    elif name in data_syms:
        absolutes.append((name, data_syms[name]))
    elif m:
        absolutes.append((name, int(m.group(1), 16)))
    else:
        unknown.append(name)
if unknown:
    sys.stderr.write(
        "fnmatch: unknown externals (not in symbols.csv, not gUnk_<addr>, "
        "not in data_symbols):\n  " + "\n  ".join(unknown) + "\n"
        "Name them in the symbol DB or reference them as gUnk_<address>.\n")
    sys.exit(2)

with open(d + '/standins.s', 'w') as f:
    f.write("@ auto-generated by tools/fnmatch.sh - zero-size call stand-ins\n")
    f.write("\t.syntax unified\n\t.cpu arm7tdmi\n")
    for name, addr, mode in labels:
        f.write("\t.section .st_%s, \"ax\"\n" % name)
        f.write("\t.%s\n" % ("thumb" if mode == "thumb" else "arm"))
        if mode == "thumb":
            f.write("\t.thumb_func\n")
        f.write("\t.global %s\n%s:\n" % (name, name))
    f.write("\n@ RAM cells (pool-referenced only): absolute symbols are fine\n")
    for name, addr in absolutes:
        f.write("\t.global %s\n%s = 0x%08X\n" % (name, name, addr))

with open(d + '/link.ld', 'w') as f:
    f.write("MEMORY { ROM : ORIGIN = 0x08000000, LENGTH = 8M }\nSECTIONS\n{\n")
    f.write("    .cand 0x%08X : { %s/cand.o(.text) } > ROM\n" % (start, d))
    for name, addr, mode in labels:
        f.write("    .st_%s 0x%08X : { *(.st_%s) } > ROM\n" % (name, addr, name))
    f.write("    /DISCARD/ : { *(*) }\n}\n")
PYEOF

arm-none-eabi-as -mcpu=arm7tdmi -o "$D/standins.o" "$D/standins.s"
arm-none-eabi-ld -T "$D/link.ld" -o "$D/cand.elf" "$D/cand.o" "$D/standins.o"
arm-none-eabi-objcopy -O binary --only-section=.cand "$D/cand.elf" "$D/cand.bin"
[[ -s "$D/cand.bin" ]] || { echo "empty candidate section" >&2; exit 2; }

# ── 3. byte compare ─────────────────────────────────────────────────────────
dd if=baserom.gba bs=1 skip=$((START & 0x01FFFFFF)) count="$SIZE" \
   of="$D/rom.bin" status=none

if cmp -s "$D/cand.bin" "$D/rom.bin"; then
    echo "MATCH ($SIZE bytes at $(printf 0x%08X "$START"))"
    exit 0
fi

CS=$(stat -c%s "$D/cand.bin")
NDIFF=$(python3 - "$D" <<'PYEOF'
import sys
d = sys.argv[1]
a = open(d + '/cand.bin', 'rb').read(); b = open(d + '/rom.bin', 'rb').read()
print(sum(1 for x, y in zip(a, b) if x != y) + abs(len(a) - len(b)))
PYEOF
)
echo "MISMATCH: candidate=$CS bytes, target=$SIZE bytes, $NDIFF differing byte(s)"
echo "── pool-resolving diff (rom vs candidate) ──"
python3 tools/fnmatch_diff.py "$D/rom.bin" "$D/cand.bin" "$START" | head -80
exit 1
