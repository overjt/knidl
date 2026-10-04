#!/usr/bin/env python3
"""ROM range splitter (issue #23).

Converts verbatim `.incbin` data segments into labeled assembler files that
reassemble byte-for-byte, giving every function in the range a symbol so C
migration can proceed incrementally (the classic pret "split" flow).

Driven by three committed inputs:

  * tools/split_config.json    - which segments to split, plus the names that
                                 are already defined as real labels elsewhere
                                 (asm/crt0.s, src/*.c)
  * docs/analysis/segments.txt - address ranges / kinds (single source of
                                 truth for segment boundaries)
  * docs/analysis/symbols.csv  - function database from issue #22

For every configured segment the tool writes `asm/<name>.s` and removes the
obsolete `data/<name>.s` incbin slice.  linker.ld needs no edit: it already
pins every section by name, and the generated file re-uses the same section
name, so the object simply takes the place of the incbin blob.

Two optional config keys add hand labels on top of the database:

  * "extra_labels": {"0x080CFC50": "_call_via_r8", ...} - emits a real
    `.global <name>` label at that address inside its split segment (for
    functions that never appear in symbols.csv because nothing calls them,
    or for named items inside data segments);
  * "data_symbols": {"0x03004C94": "gTaskBaseSp", ...} - word values that
    are emitted symbolically as `.word <name>` wherever they appear in a
    pool/data word; the definitions (`<name> = 0x...`) are appended to
    `asm/rom_syms.s`.  Used for IWRAM/MMIO cells referenced by literal
    pools of split code.

A third optional key defines plain link-time constants:

  * "abs_symbols": {"gNumMusicPlayers": "0x00000004", ...} - absolute
    symbols (name -> value) appended to `asm/rom_syms.s` WITHOUT any pool
    word renaming (unlike data_symbols, small values like 0 or 4 would
    otherwise rename unrelated pool words everywhere).  Used for the SDK
    idiom of reading a constant as a symbol's address, e.g. the m4a
    driver's (u16)gNumMusicPlayers / (u32)gMaxLines (issue #53).

The tool also regenerates `asm/rom_syms.s`, which defines every database
function that is not otherwise labeled as an *absolute* symbol
(`name = 0xADDR`).  That lets split files reference not-yet-split code
symbolically, e.g. `.word sub_08001518+1` in the IRQ handler table or
`bl __divsi3` from future split code, while the final bytes stay identical.

Emission rules:

  * ranges covered by a database function become code (`.thumb`/`.arm` +
    `.global <db name>` labels; Thumb entries also get `.thumb_func`);
  * every other range becomes labeled data (`gUnk_XXXXXXXX`);
  * literal-pool words (targets of pc-relative `ldr`, decoded from the ROM
    bytes) are emitted as `.word`, symbolically when the value is a known
    ROM function pointer (`name+1` for Thumb entries);
  * branch targets are rewritten to labels (function names, local `.L_`
    labels inside the file, or database names for external targets);
  * a code segment must start and end on its alignment (2 for Thumb, 4
    for ARM or a literal pool): gas aligns instructions and pads a
    section's size, so a boundary inside an instruction would move bytes;
    the tool refuses one (#37 moved the last two, #167 removed the
    `.tail`-section and raw odd-start paths that handled them);
  * config "isa_ranges" switches the ISA inside a function (the ARM
    islands of m4a_1's Thumb functions), and every `add rd, pc, #imm` is
    written `adr rd, <label>`; config "raw_words" keeps a literal word a
    number with a same-line `@ raw: <reason>` (docs/data.md 3.5).

Every generated file is verified before being written: the tool assembles
it, links it at the segment's ROM VMA with a throwaway linker script plus
rom_syms.o, objcopies the section out and compares it against the original
baserom bytes.  If real instructions fail to round-trip, the whole segment
falls back to raw `.short`/`.byte` emission (a verbatim byte copy), so the
output is always byte-identical.  `make compare` remains the final proof.

Data segments (kind `data` in segments.txt) use a different, structure-only
emitter (issue #36, docs/data.md), because the repository commits no ROM
bytes: `data/<name>.s` holds only labels, symbolic `.word`s and
`.incbin "baserom.gba", <offset>, <length>` slices.  Its rules:

  * a real `.global` label at every ROM `data_symbols` value and every
    `extra_labels` address inside the segment (their absolute definitions
    leave asm/rom_syms.s);
  * `.word <fn>+1` (Thumb) / `.word <fn>` (ARM) for every 4-aligned word
    whose value is a symbols.csv function entry, unless the segment is
    marked `"asset": true` (graphics, samples, songs: a 0x08xxxxxx value
    there is as likely pixels as a pointer) or the word is listed in
    `"not_pointers"`;
  * `.word <label>` for every word of a `"pointer_tables"` entry, the
    tables whose layout a decompiled consumer proves (or, with
    `"extent": "next-label"`, whose element type the consumer declares
    and whose span up to the next label is all NULL or pointer values):
    each non-NULL word must resolve to a function, a ROM data label or a
    named RAM cell, and an unresolved one stops the run
    (`--missing-labels` writes the data_symbols entries it would need);
  * `.incbin` for every other run of bytes, whatever its length.

  A data segment's config entry may name its "zone" (#167): the data
  file it is written to.  The pieces of a zone that C runs cut apart
  (tools/carve_data.py) share one `data/<zone>.s`, each piece its own
  `.section .<piece>` named after its segments.txt row, so linker.ld lists
  the pieces and the C runs in order inside one output section
  (tools/ldgroup.py, docs/data.md 5.2).  Without "zone" a segment is its
  own zone.  A generated data file that no zone writes any more is
  removed.

Run inside the knidl-builder image via `make split`, or directly:

  python3 tools/split.py --rom baserom.gba --config tools/split_config.json
"""

import argparse
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

ROM_BASE = 0x08000000

AS = "arm-none-eabi-as"
LD = "arm-none-eabi-ld"
OBJCOPY = "arm-none-eabi-objcopy"
OBJDUMP = "arm-none-eabi-objdump"

DATA_PREFIX = "gUnk_"
LOCAL_PREFIX = ".L_"

CODE_KINDS = ("arm_code", "thumb_code")

# Structure-only data emission (issue #36): the name every generated
# `.incbin` line references.  The build runs from the repository root, where
# the user-supplied ROM lives under exactly this name.
BASEROM_NAME = "baserom.gba"
STRUCTURE_KINDS = ("data",)
# A functional table carved into C (tools/carve_data.py, issue #36 phase 2):
# src/data/<name>.c defines it, so its labels are C symbols, not asm labels
# or rom_syms.s absolutes, and a pointer table inside it emits no words.
C_DATA_KIND = "c_data"


class FallbackNeeded(Exception):
    """A segment could not be emitted at the current detail level."""


LINK_FAILURE = "<link>"


def compute_chunks(start, end, func_vmas, chunk_bytes):
    """Cut points for a chunked segment (issue #25).

    Cuts land on word-aligned function-boundary addresses roughly
    `chunk_bytes` apart, so that no function straddles a chunk and every
    chunk starts and ends on its alignment (gas aligns instructions and
    pads a section's size to its alignment; a chunk with a literal pool is
    word-aligned).  The segment itself must start and end on its alignment
    (SegmentEmitter.emit checks it).
    """
    cuts = [start]
    pos = start
    while pos < end:
        window_end = min(end, pos + chunk_bytes)
        cands = [v for v in func_vmas if pos < v <= window_end and v % 4 == 0]
        if cands:
            nxt = max(cands)
        else:
            later = [v for v in func_vmas if v > pos and v % 4 == 0]
            nxt = later[0] if later and later[0] < end else end
        cuts.append(nxt)
        pos = nxt
    return cuts


def isa_runs(rs, re_, isa, isa_ranges):
    """[(start, end, isa)] covering [rs, re_): `isa` (the function's),
    except inside the config isa_ranges [(start, end, isa)]."""
    runs = []
    cur = rs
    for s, e, risa in sorted(isa_ranges):
        if e <= cur or s >= re_:
            continue
        s, e = max(s, cur), min(e, re_)
        if s > cur:
            runs.append((cur, s, isa))
        runs.append((s, e, risa))
        cur = e
    if cur < re_:
        runs.append((cur, re_, isa))
    return runs


def collect_ref_targets(rom, start, end, funcs, isa_ranges=()):
    """Set of branch/adr target addresses referenced from anywhere in the
    segment's function stream.

    Each function is decoded only up to the next database function start
    (its real extent): decoding onwards to the segment end would re-walk
    every later function once per predecessor (~O(n^2) halfword decodes
    over the 5k-function game region) while only ever yielding false
    positives from misaligned data.  Literal-pool detection stays inside
    the per-chunk prescan; this pass exists to precompute the global
    `loc_XXXXXXXX` labels that chunked files reference across file
    boundaries.
    """
    targets = set()
    bounds = [vma for vma, _n, _i in funcs]
    for i, (vma, _name, isa) in enumerate(funcs):
        stop = bounds[i + 1] if i + 1 < len(bounds) else end
        for rs, re_, risa in isa_runs(vma, stop, isa, isa_ranges):
            off = vma_off(rs)
            stop_off = vma_off(re_)
            while off < stop_off:
                if risa == "thumb":
                    size, info = thumb_decode(rom, off)
                else:
                    size, info = arm_decode(rom, off)
                if info:
                    if "branch" in info:
                        targets.add(info["branch"])
                    elif "pcadd" in info:
                        targets.add(info["pcadd"] & ~1)
                off += size
    return targets


def u16(rom, off):
    return (rom[off] | (rom[off + 1] << 8)) & 0xFFFF


def u32(rom, off):
    return (
        rom[off]
        | (rom[off + 1] << 8)
        | (rom[off + 2] << 16)
        | (rom[off + 3] << 24)
    )


def vma_off(vma):
    return vma - ROM_BASE


# --------------------------------------------------------------------------
# Input parsing
# --------------------------------------------------------------------------


def parse_segments_file(path):
    """{name: (start, end, kind)} from docs/analysis/segments.txt."""
    segs = {}
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split()
            segs[parts[3]] = (int(parts[0], 16), int(parts[1], 16), parts[2])
    return segs


def load_symbol_db(path):
    """{vma: (name, isa)} from docs/analysis/symbols.csv."""
    db = {}
    with open(path) as f:
        header = f.readline()
        if header.strip() != "vma,size,isa,evidence,name":
            sys.exit("error: unexpected symbols.csv header: %r" % header)
        for line in f:
            line = line.strip()
            if not line:
                continue
            vma, _size, isa, _ev, name = line.split(",")
            db[int(vma, 16)] = (name, isa)
    return db


# --------------------------------------------------------------------------
# Instruction decoding (metadata only: branch targets and literal pools).
# Encoding is left to gas; these decoders mirror tools/symdb.py.
# --------------------------------------------------------------------------


def thumb_decode(rom, off):
    """Return (size, info) for the Thumb instruction at file offset `off`.

    info is None or one of {"branch": target} / {"pool": addr}.
    ARMv4T has no 32-bit Thumb instructions, so `bl` is the only 4-byte case.
    """
    hw = u16(rom, off)
    if 0xF000 <= hw <= 0xF7FF:  # BL prefix
        hw2 = u16(rom, off + 2)
        if 0xF800 <= hw2 <= 0xFFFF:
            imm = ((hw & 0x7FF) << 12) | ((hw2 & 0x7FF) << 1)
            if imm & 0x400000:
                imm -= 0x800000
            return 4, {"branch": ROM_BASE + off + 4 + imm}
        return 2, None
    if 0xE000 <= hw <= 0xE7FF:  # b <label>
        imm = (hw & 0x7FF) << 1
        if imm & 0x800:
            imm -= 0x1000
        return 2, {"branch": ROM_BASE + off + 4 + imm}
    if 0xD000 <= hw <= 0xDDFF:  # b<cond> <label> (0xDExx is udf, 0xDFxx svc)
        imm = (hw & 0xFF) << 1
        if imm & 0x100:
            imm -= 0x200
        return 2, {"branch": ROM_BASE + off + 4 + imm}
    if 0x4800 <= hw <= 0x4FFF:  # ldr rd, [pc, #imm8*4]
        base = (ROM_BASE + off + 4) & ~3
        return 2, {"pool": base + (hw & 0xFF) * 4}
    if 0xA000 <= hw <= 0xA7FF:  # add rd, pc, #imm8*4 (adr)
        base = (ROM_BASE + off + 4) & ~3
        return 2, {"pcadd": base + (hw & 0xFF) * 4, "rd": (hw >> 8) & 7}
    return 2, None


def arm_decode(rom, off):
    """Return (size, info) for the ARM instruction at file offset `off`."""
    w = u32(rom, off)
    if (w & 0x0E000000) == 0x0A000000 and (w & 0xF0000000) != 0xF0000000:
        imm = w & 0xFFFFFF
        if imm & 0x800000:
            imm -= 0x1000000
        return 4, {"branch": ROM_BASE + off + 8 + (imm << 2)}
    if (w & 0x0F7F0000) == 0x051F0000:  # ldr rd, [pc, +/-imm12]
        imm = w & 0xFFF
        base = (ROM_BASE + off + 8) & ~3
        target = base + imm if (w & 0x00800000) else base - imm
        return 4, {"pool": target}
    op = w & 0xFFFF0000
    if op in (0xE28F0000, 0xE24F0000):
        # add/sub rd, pc, #imm (adr): an always-executed data-processing
        # immediate with Rn = pc and S clear (opcode 0100 add, 0010 sub);
        # the target keeps bit 0, which marks a Thumb destination for the
        # `bx` that follows (SoundMainRAM's ARM islands return that way)
        rot = ((w >> 8) & 0xF) * 2
        imm = w & 0xFF
        imm = ((imm >> rot) | (imm << (32 - rot))) & 0xFFFFFFFF if rot else imm
        pc = ROM_BASE + off + 8
        target = pc + imm if op == 0xE28F0000 else pc - imm
        return 4, {"pcadd": target, "rd": (w >> 12) & 0xF}
    return 4, None


# --------------------------------------------------------------------------
# objdump disassembly
# --------------------------------------------------------------------------

OBJDUMP_LINE = re.compile(r"^\s*([0-9a-f]+):\t(.*)$")

# gas reports failing lines as "<path>/<name>.s:<lineno>: Error: ..." —
# line numbers index into the exact text we handed the assembler.
ASM_ERROR_LINE = re.compile(r"^.*\.s:(\d+):\s*Error", re.MULTILINE)


def disassemble(rom, start, end, thumb, tmpdir, tag):
    """Run objdump over rom[start:end]; return {addr: (size, text, hex)}.

    text is the re-assemblable instruction with objdump's `@` comments and
    `<symbol>` suffixes stripped; hex is the byte column.  Lines look like

        80002d0:\te0822003 \tadd\tr2, r2, r3

    and are split on the tabs: a whitespace-based bytes regex would absorb
    hex-only mnemonics (`add`, `bcc`, ...) into the byte column.

    The byte column matters: objdump's linear sweep desyncs on data that
    pairs as a fake `bl` and then emits entries whose text decodes bytes
    from a SHIFTED address (lesson 4.10).  Callers must verify hex against
    the ROM bytes at the address they are about to emit for.
    """
    binpath = os.path.join(tmpdir, tag + ".bin")
    with open(binpath, "wb") as f:
        f.write(rom[vma_off(start):vma_off(end)])
    # -marmv4t is essential: with a plain -marm blob objdump prints
    # Thumb-2-only mnemonics (rev, cbnz, it, blx reg, ...) for halfword
    # pairs that ARMv4T reads differently, which arm7tdmi gas then rejects,
    # forcing whole chunks into the raw fallback.  Same choice as
    # asmdiff.sh.
    cmd = [
        OBJDUMP, "-D", "-z", "-b", "binary", "-marmv4t",
        "--adjust-vma=0x%08X" % start,
    ]
    if thumb:
        cmd.append("-Mforce-thumb")
    cmd.append(binpath)
    proc = subprocess.run(cmd, capture_output=True, text=True)
    if proc.returncode != 0:
        sys.exit("error: objdump failed: %s" % proc.stderr)
    out = {}
    for line in proc.stdout.splitlines():
        m = OBJDUMP_LINE.match(line)
        if not m:
            continue
        parts = m.group(2).split("\t", 1)
        if len(parts) != 2:
            continue  # "\t..." elision / "Address ... out of bounds"
        bytecol, text = parts
        tokens = bytecol.split()
        if not tokens or not all(re.fullmatch(r"[0-9a-fA-F]+", t) for t in tokens):
            continue
        addr = int(m.group(1), 16)
        size = sum(len(t) for t in tokens) // 2
        text = text.split("@")[0].strip()
        text = re.sub(r"\s*<[^>]*>", "", text).strip()
        # objdump's byte column shows endian-swapped UNITS (halfwords for
        # Thumb, words for ARM): file bytes 70 47 print as "4770".  Reverse
        # each token so the joined string equals the ROM slice's hex and a
        # shifted/desynced entry can be detected by simple comparison.
        le = "".join(
            bytes.fromhex(t)[::-1].hex() for t in tokens
        )
        out[addr] = (size, text, le)
    return out


# --------------------------------------------------------------------------
# Segment emission
# --------------------------------------------------------------------------


class SegmentEmitter(object):
    def __init__(self, rom, name, start, end, kind, funcs, db, level,
                 extra_labels=None, data_symbols=None, chunk_index=None,
                 num_chunks=1, seg_start=None, seg_end=None,
                 required_labels=None, seg_names=None):
        self.rom = rom
        self.name = name
        self.start = start
        self.end = end
        self.kind = kind
        self.funcs = funcs  # sorted [(vma, name, isa)]
        self.db = db
        self.level = level  # 0 = real instructions, 1 = raw .short
        self.extra_labels = extra_labels or {}  # addr -> name (config)
        self.data_symbols = data_symbols or {}  # word value -> name (config)
        # extra_labels that name data inside a structure-only data segment
        # (the m4a engine tables): pool words equal to one are pointers to
        # that label, like data_symbols (#36 phase 2 run 2)
        self.data_labels = {}
        # Chunked emission (issue #25): several files share one linker
        # section; chunk_index None means the legacy one-file-per-segment
        # layout with file-local .L_ labels.
        self.chunked = chunk_index is not None
        self.chunk_index = chunk_index
        self.num_chunks = num_chunks
        self.seg_start = start if seg_start is None else seg_start
        self.seg_end = end if seg_end is None else seg_end
        # Addresses inside this chunk that must carry a global loc_ label
        # because other chunks branch/adr to them (plus every non-function
        # intra-segment branch target, uniformly).
        self.required_labels = set(required_labels or ())
        # addr -> name for every named address in the whole segment
        # (function entries of all chunks, extra labels, loc_ labels).
        self.seg_names = seg_names or {}
        self.lines = []
        self.pool_addrs = set()
        self.pending = {}  # addr -> [label, ...]
        self.dis_cache = {}  # isa -> {addr: (size, text)}
        self.cursor = start  # emission frontier (labels behind it are placed)
        self.main_end = end
        self._extra_placed = set()
        self._required_placed = set()
        self._placed_labels = set()
        # Addresses whose objdump text gas rejects (undefined-decode-space
        # halfwords printed as later-arch mnemonics); emit_instruction
        # forces them to raw .short bytes.  Persists across emit() calls so
        # the assembly repair loop can grow it incrementally.
        self.forced_raw_addrs = set()
        # [(start, end, isa)] of config "isa_ranges": code inside a function
        # whose ISA is not the function's (the ARM islands of m4a_1's Thumb
        # functions umul3232H32 and SoundMainRAM); decoded, labelled and
        # emitted in that ISA, with `.arm`/`.thumb` at each boundary
        self.isa_ranges = []
        # addr -> reason of config "raw_words": literal words that stay
        # numbers (constants, not addresses), emitted with `@ raw: <reason>`
        self.raw_words = {}
        # Address of every instruction line appended to self.lines, in
        # order (parallel to the file's instruction lines; used by
        # addrs_from_asm_errors).
        self.insn_addrs = []
        self.func_map = dict((vma, name) for vma, name, _isa in funcs)
        self.stats = {}

    def _append_insn(self, text):
        """Append one instruction line and remember its source address."""
        self.insn_addrs.append(self.cursor)
        self.lines.append(text)

    # ---- label bookkeeping -------------------------------------------

    def file_label(self, addr):
        """Best label for an address inside this file (or None)."""
        if addr in self.func_map:
            return self.func_map[addr]
        if addr == self.start and (
            not self.chunked or self.chunk_index == 0
        ):
            # Only the first chunk defines the segment anchor symbol;
            # a second .global definition would be a link error.
            return self.name
        if addr in self.extra_labels:
            return self.extra_labels[addr]
        return None

    def emit_extra_label(self, addr):
        """Emit the config-driven `.global` label at `addr`, if any."""
        name = self.extra_labels.get(addr)
        if name is None or addr in self._extra_placed:
            return
        self._extra_placed.add(addr)
        self.lines.append("\t.global\t%s" % name)
        self.lines.append("%s:" % name)

    def emit_required_at(self, addr):
        """Emit the global cross-file `loc_XXXXXXXX` label at `addr`."""
        if not self.chunked or addr not in self.required_labels:
            return
        if addr in self._required_placed:
            return
        self._required_placed.add(addr)
        name = "loc_%08x" % addr
        self.lines.append("\t.global\t%s" % name)
        self.lines.append("%s:" % name)

    def emit_labels_at(self, addr):
        for label in self.pending.pop(addr, []):
            self.lines.append("%s:" % label)
            self._placed_labels.add(label)

    def label_pending_at(self, addr):
        """True when a label boundary is needed at halfword `addr`."""
        if addr in self.pending:
            return True
        return self.chunked and addr in self.required_labels

    def resolve_target(self, addr):
        """Label for a branch/pool target, or None if not resolvable."""
        label = self.file_label(addr)
        if label is not None:
            return label
        if not self.chunked:
            if self.start <= addr < self.end:
                if self.level == 0:
                    label = LOCAL_PREFIX + "%08x" % addr
                    # Only queue the label if the walk has not emitted the
                    # address yet (a backward branch re-resolves its target
                    # after the label line was already written; a branch to
                    # itself, `b .`, resolves after its own label).
                    if addr >= self.cursor and label not in self._placed_labels:
                        labels = self.pending.setdefault(addr, [])
                        if label not in labels:
                            labels.append(label)
                    return label
                return None
            return self.db.get(addr)
        # Chunked mode: names for the whole segment are precomputed, so
        # targets in other chunks resolve at link time via global labels.
        if addr in self.seg_names:
            return self.seg_names[addr]
        if self.seg_start <= addr < self.seg_end:
            return None
        return self.db.get(addr)

    # ---- metadata pre-pass --------------------------------------------

    def prescan(self):
        """Decode all function ranges to find literal pools and branch
        targets.

        Runs regardless of emission level so that raw mode can still emit
        pool words as annotated `.word`s.  Each function is decoded only up
        to the next database function start (collect_ref_targets mirrors
        this): walking on to the segment end would re-decode every later
        function once per predecessor while only ever producing
        false-positive labels from misaligned data.  Branch targets seed
        the local label set up front in legacy (flat) mode; some are false
        positives (data that decodes as a branch), but labelling a
        halfword boundary is harmless and beats losing real instructions
        for the whole segment.  In chunked mode cross-file names are
        precomputed instead, so no local queueing happens.
        """
        self.branch_targets = []
        for i, (vma, _name, isa) in enumerate(self.funcs):
            stop = (
                self.funcs[i + 1][0]
                if i + 1 < len(self.funcs)
                else self.end
            )
            for rs, re_, risa in self.isa_runs(vma, stop, isa):
                off = vma_off(rs)
                stop_off = vma_off(re_)
                while off < stop_off:
                    if risa == "thumb":
                        size, info = thumb_decode(self.rom, off)
                    else:
                        size, info = arm_decode(self.rom, off)
                    if info:
                        if "pool" in info:
                            target = info["pool"]
                            if self.start <= target <= self.end - 4:
                                self.pool_addrs.add(target)
                        elif "branch" in info:
                            self.branch_targets.append(info["branch"])
                        elif "pcadd" in info:
                            # an adr's target needs a label too (bit 0 is
                            # the Thumb mark of an ARM `adr rd, label+1`)
                            self.branch_targets.append(info["pcadd"] & ~1)
                    off += size

        for target in self.branch_targets:
            if self.chunked:
                break
            if (self.start < target < self.end
                    and target not in self.func_map
                    and target not in self.extra_labels):
                label = LOCAL_PREFIX + "%08x" % target
                labels = self.pending.setdefault(target, [])
                if label not in labels:
                    labels.append(label)

    # ---- word annotation ----------------------------------------------

    def word_line(self, addr):
        """Emit line(s) for the 4 bytes at `addr` (a pool or data word)."""
        v = u32(self.rom, vma_off(addr))
        if addr in self.raw_words:
            # config raw_words: a constant, not an address
            self.stats["data_words"] += 1
            return "\t.word\t0x%08X\t@ raw: %s" % (v, self.raw_words[addr])
        if v in self.data_symbols:
            self.stats["named_words"] += 1
            return "\t.word\t%s" % self.data_symbols[v]
        if v in self.data_labels:
            self.stats["named_words"] += 1
            return "\t.word\t%s" % self.data_labels[v]
        if v & 1:
            target = v & ~1
            if target in self.db:
                self.stats["symbolic_words"] += 1
                return "\t.word\t%s+1" % self.db[target]
            if target in self.data_symbols:
                # Thumb code copied to RAM: SoundMain's `bx` to the mixer's
                # IWRAM copy gSoundMainRAM_Buffer+1
                self.stats["named_words"] += 1
                return "\t.word\t%s+1" % self.data_symbols[target]
        elif v in self.db:
            self.stats["symbolic_words"] += 1
            return "\t.word\t%s" % self.db[v]
        self.stats["data_words"] += 1
        return "\t.word\t0x%08X" % v

    def raw_bytes_lines(self, addr, count):
        """Emit `count` bytes at `addr` as raw .short/.byte lines."""
        off = vma_off(addr)
        emitted = 0
        while emitted < count:
            left = count - emitted
            if left >= 2 and (addr + emitted) % 2 == 0:
                self.lines.append("\t.short\t0x%04X" % u16(self.rom, off + emitted))
                emitted += 2
            else:
                self.lines.append("\t.byte\t0x%02X" % self.rom[off + emitted])
                emitted += 1
        self.stats["raw_instructions"] += count

    # ---- region emitters -----------------------------------------------

    def emit_data_region(self, rs, re):
        label = DATA_PREFIX + "%08x" % rs
        self.lines.append("\t.global\t%s" % label)
        self.lines.append("%s:" % label)
        addr = rs
        while addr < re:
            self.cursor = addr
            self.emit_labels_at(addr)
            self.emit_extra_label(addr)
            self.emit_required_at(addr)
            left = re - addr
            if (
                addr % 4 == 0
                and left >= 4
                and not self.label_pending_at(addr + 2)
            ):
                self.lines.append(self.word_line(addr))
                addr += 4
            elif addr % 2 == 0 and left >= 2:
                self.lines.append(
                    "\t.short\t0x%04X" % u16(self.rom, vma_off(addr))
                )
                addr += 2
            else:
                self.lines.append("\t.byte\t0x%02X" % self.rom[vma_off(addr)])
                addr += 1

    def emit_func_region(self, rs, re, name, isa, tmpdir):
        runs = self.isa_runs(rs, re, isa)
        pretty = self.level == 0
        if pretty:
            if isa == "thumb":
                self.lines.append("\t.thumb_func")
            self.lines.append("\t.global\t%s" % name)
            self.lines.append("%s:" % name)
        else:
            # Raw mode (the last-resort fallback): label only, no mode
            # directives, so the section can stay at alignment 1 when no
            # real instruction is ever emitted.
            self.lines.append("\t.global\t%s" % name)
            self.lines.append("%s:" % name)
        for s, e, risa in runs:
            if pretty:
                # The mode directive comes before the run's labels: gas
                # gives a label the ISA it is defined in, and `.arm` aligns
                # to 4 (isa_ranges start word-aligned, so it adds nothing).
                self.lines.append("\t.thumb" if risa == "thumb" else "\t.arm")
            self.emit_run(s, e, risa, pretty, tmpdir)

    def sweep(self, isa, rs, tmpdir):
        """objdump decode of [rs, main_end) in `isa`, cached by start."""
        key = (isa, rs)
        if key not in self.dis_cache:
            self.dis_cache[key] = disassemble(
                self.rom, rs, self.main_end, isa == "thumb", tmpdir,
                "%s_%s_%08x" % (self.name, isa, rs),
            )
        return self.dis_cache[key]

    def emit_run(self, rs, re, isa, pretty, tmpdir):
        """Emit [rs, re), code of one ISA inside a function region."""
        dis = {}
        if pretty:
            # One objdump sweep per ISA covers the rest of the segment: a
            # big buffer is decoded once, and Thumb-16 decoding is
            # address-stable (every instruction is one halfword; even a
            # false `bl' prefix in data only corrupts its own 4-byte
            # window).  Where the sweep is out of step with this run (it
            # merged the run's first halfword into a 32-bit Thumb-2 pattern
            # of the bytes before it, e.g. the ARM word ahead of a
            # trampoline's `bx pc`), a fresh sweep starts at the address.
            first = self.dis_cache.setdefault(("first", isa), rs)
            dis = self.sweep(isa, first, tmpdir)
        resynced = set()

        addr = rs
        while addr < re:
            self.cursor = addr
            self.emit_labels_at(addr)
            self.emit_extra_label(addr)
            self.emit_required_at(addr)
            left = re - addr
            off = vma_off(addr)

            # Literal-pool words inside the instruction stream.
            if (
                addr in self.pool_addrs
                and left >= 4
                and addr % 4 == 0
                and (addr - self.start) % 4 == 0
                and not self.label_pending_at(addr + 2)
            ):
                self.stats["pool_words"] += 1
                self.lines.append(self.word_line(addr))
                addr += 4
                continue

            if isa == "thumb":
                size, info = thumb_decode(self.rom, off)
            else:
                size, info = arm_decode(self.rom, off)
            size = min(size, left)

            # A 4-byte item (bl pair / ARM instruction) with a branch
            # target landing on its second halfword must be split into raw
            # halfwords so the label has a boundary to sit on.
            if size == 4 and self.label_pending_at(addr + 2):
                self.raw_bytes_lines(addr, 2)
                addr += 2
                continue

            text = ""
            entry = dis.get(addr) if pretty else None
            if pretty and addr not in resynced and (
                entry is None
                or entry[2] != self.rom[off:off + entry[0]].hex()
            ):
                resynced.add(addr)
                dis = dict(dis)
                dis.update(self.sweep(isa, addr, tmpdir))
                entry = dis.get(addr)
            if entry is not None:
                entry_size, text, hexbytes = entry
                want_hex = self.rom[off:off + entry_size].hex()
                if hexbytes != want_hex or text.startswith("(bad)"):
                    # Desynced linear-sweep entry (fake-BL window shifted
                    # objdump's decode onto other bytes, lesson 4.10): the
                    # text does not describe THIS address's bytes.
                    text = ""
                elif entry_size > left:
                    text = ""
                elif entry_size > size and self.label_pending_at(addr + 2):
                    # objdump merged two halfwords into one 4-byte
                    # instruction, but a cross-chunk `loc_` label has to sit
                    # on the second one: emit raw halfwords so the label
                    # gets a boundary (issue #65 - carving a c_code range
                    # out of the middle of a chunked segment re-cuts every
                    # later chunk, which is how this case first appeared).
                    text = ""
            emitted = False
            if text:
                if entry_size <= left:
                    emitted = self.emit_instruction(addr, size, entry_size, text, info)
                    if emitted:
                        addr += entry_size
            if not emitted:
                self.raw_bytes_lines(addr, size)
                addr += size

    def isa_at(self, addr, default):
        """ISA of the code byte at `addr` in a function of ISA `default`."""
        for s, e, isa in self.isa_ranges:
            if s <= addr < e:
                return isa
        return default

    def isa_runs(self, rs, re_, isa):
        return isa_runs(rs, re_, isa, self.isa_ranges)

    def emit_instruction(self, addr, decode_size, entry_size, text, info):
        """Emit one objdump-derived instruction line.

        Returns True if the instruction was emitted (caller advances by
        entry_size), False if the caller should use raw bytes instead.
        """
        if addr in self.forced_raw_addrs:
            # gas rejected this line in an earlier repair round; emit the
            # halfwords verbatim instead.
            return False

        parts = text.split(None, 1)
        mnemonic = parts[0]
        operand = parts[1] if len(parts) > 1 else ""

        if info and "branch" in info:
            if decode_size != entry_size:
                return False  # objdump boundary disagrees with our decoder
            label = self.resolve_target(info["branch"])
            if label is None:
                return False
            self._append_insn(
                "\t%s\t%s\t@ 0x%08X" % (mnemonic, label, info["branch"])
            )
            self.stats["instructions"] += 1
            return True

        if info and "pool" in info:
            self._append_insn("\t%s\t@ 0x%08X" % (text, info["pool"]))
            self.stats["instructions"] += 1
            return True

        if info and "pcadd" in info:
            # `add rd, pc, #imm` (Thumb, ARM) is `adr rd, <label>`: objdump
            # prints the pc-relative form or an absolute address, neither of
            # which names the target; an odd ARM target is a Thumb label + 1
            if decode_size != entry_size:
                return False
            target = info["pcadd"]
            label = self.resolve_target(target & ~1)
            if label is None:
                return False
            self._append_insn(
                "\tadr\tr%d, %s%s\t@ 0x%08X"
                % (info["rd"], label, "+1" if target & 1 else "", target)
            )
            self.stats["instructions"] += 1
            return True

        # Any other operand that still contains a bare 8-hex-digit address
        # (objdump sometimes annotates pc-relative forms symbolically) would
        # not re-assemble position-independently; fall back to raw bytes.
        if re.search(r"(?<![#\w])0?[0-9a-f]{7,8}(?!\w)", operand.replace("0x", "#")):
            return False

        self._append_insn("\t" + text)
        self.stats["instructions"] += 1
        return True

    def addrs_from_asm_errors(self, err):
        """Map gas error line numbers back to instruction addresses."""
        table = dict()
        insn_idx = 0
        for i, line in enumerate(self.lines, start=1):
            s = line.strip()
            if (
                not s
                or s.startswith("@")
                or s.startswith(".")
                or s.endswith(":")
            ):
                continue
            table[i] = (
                self.insn_addrs[insn_idx]
                if insn_idx < len(self.insn_addrs)
                else None
            )
            insn_idx += 1
        addrs = set()
        for m in ASM_ERROR_LINE.finditer(err):
            addr = table.get(int(m.group(1)))
            if addr is not None:
                addrs.add(addr)
        return addrs

    def instruction_line_offsets(self):
        """[(byte_offset, lineno, addr)] for every instruction line.

        Data directives cannot diverge (they carry literal numbers), so the
        post-assembly repair only ever needs to locate instruction lines:
        given a mismatching byte offset in the assembled section, the entry
        whose [offset, offset+size) window contains it identifies both the
        source line and the ROM address to force to raw emission.
        """
        out = []
        off = 0
        ins = 0
        thumb = True
        for i, line in enumerate(self.lines):
            s = line.strip()
            if not s or s.startswith("@"):
                continue
            if s.startswith(".thumb"):
                thumb = True
                continue
            if s.startswith(".arm"):
                thumb = False
                continue
            if s.startswith("."):
                if s.startswith(".word"):
                    off += 4
                elif s.startswith(".short"):
                    off += 2
                elif s.startswith(".byte"):
                    off += 1
                continue
            if s.endswith(":"):
                continue
            if ins < len(self.insn_addrs):
                addr = self.insn_addrs[ins]
                ins += 1
                o = vma_off(addr)
                size, _info = (
                    thumb_decode(self.rom, o)
                    if thumb
                    else arm_decode(self.rom, o)
                )
                out.append((off, i + 1, addr, size))
                off += size
        return out

    def addr_for_offset(self, byte_offset):
        """Instruction address responsible for `byte_offset`, if any."""
        for off, _lineno, addr, size in self.instruction_line_offsets():
            if off <= byte_offset < off + size:
                return addr
        return None

    # ---- top level -----------------------------------------------------

    def emit(self, tmpdir):
        # Reset per-attempt state (an emitter may be re-run at level 1 after
        # a failed level-0 attempt left pending labels behind).  dis_cache
        # and forced_raw_addrs intentionally survive: they are derived only
        # from immutable inputs (ROM bytes / assembler verdicts).
        self.lines = []
        self.insn_addrs = []
        self.pool_addrs = set()
        self.pending = {}
        self.cursor = self.start
        self._extra_placed = set()
        self._required_placed = set()
        self._placed_labels = set()
        self.stats = {
            "instructions": 0,
            "raw_instructions": 0,
            "pool_words": 0,
            "symbolic_words": 0,
            "data_words": 0,
            "named_words": 0,
        }
        self.prescan()

        # gas aligns every instruction (2 for Thumb, 4 for ARM) and pads a
        # section's SIZE up to its alignment at assembly time, and ld aligns
        # the section's start: a code segment must therefore start and end
        # on its alignment, or the bytes would move.  Until #37 two segment
        # boundaries cut an instruction in two (the `b .` at 0x080002E4,
        # SoundDriverVSyncOff's `bx lr` at 0x080CFA7E), and split.py parked
        # odd trailing bytes in a `.name.tail` section and emitted an odd
        # start as raw data; #37 moved both boundaries to instructions and
        # #167 removed those paths (docs/splitting.md).
        align = 1
        for _vma, _name, isa in self.funcs:
            align = max(align, 4 if isa == "arm" else 2)
        for _s, _e, isa in self.isa_ranges:
            align = max(align, 4 if isa == "arm" else 2)
        if self.pool_addrs and align > 1:
            # a literal pool must stay word-aligned wherever ld puts the
            # section (MATCHING=0, the shift test)
            align = 4
        if self.start % align or (self.end - self.start) % align:
            sys.exit(
                "error: %s 0x%08X-0x%08X: a code segment must start and end"
                " on its alignment (%d): move the boundary to an instruction"
                " (docs/splitting.md, \"Segment boundaries\")"
                % (self.name, self.start, self.end, align))
        main_end = self.end
        self.main_end = main_end

        h = []
        h.append("@ Auto-generated by tools/split.py from baserom.gba - DO NOT EDIT.")
        h.append("@ Regenerate with: make split")
        if self.chunked:
            h.append(
                "@ Segment %s, chunk %d/%d: 0x%08X-0x%08X (%s, 0x%X bytes)"
                % (self.name, self.chunk_index + 1, self.num_chunks,
                   self.start, self.end, self.kind, self.end - self.start)
            )
        else:
            h.append(
                "@ Segment %s: 0x%08X-0x%08X (%s, 0x%X bytes)"
                % (self.name, self.start, self.end, self.kind,
                   self.end - self.start)
            )
        # Why the segment stays asm (config "stays_asm"; tools/audit.py
        # requires this line in every asm/ file, docs/audit.md section 2).
        why = STAYS_ASM.get(self.name)
        if why:
            h.extend(wrap_comment("Stays asm: " + why))
        if self.funcs:
            h.append("@ Functions (docs/analysis/symbols.csv):")
            for vma, name, _isa in self.funcs:
                h.append("@   0x%08X %s" % (vma, name))
        h.append("")
        flags = '"ax"' if self.kind in CODE_KINDS else '"a"'
        h.append("\t.section .%s, %s" % (self.name, flags))
        # Only the first chunk of a chunked segment defines the anchor
        # symbol; later chunks get a file-local marker label instead.
        if not self.chunked or self.chunk_index == 0:
            h.append("\t.global\t%s" % self.name)
        if align >= 4 and self.start % 4 == 0:
            h.append("\t.align\t2")
        h.append("\t.syntax\tunified")
        h.append("\t.cpu\tarm7tdmi")
        if not self.chunked or self.chunk_index == 0:
            h.append("%s:" % self.name)
        else:
            h.append("%s_%02d:" % (self.name, self.chunk_index))
        self.lines = list(h)

        regions = []
        cur = self.start
        for i, (vma, name, isa) in enumerate(self.funcs):
            fend = self.funcs[i + 1][0] if i + 1 < len(self.funcs) else main_end
            if vma > cur:
                regions.append(("data", cur, vma))
            regions.append(("func", vma, min(fend, main_end), name, isa))
            cur = min(fend, main_end)
        if cur < main_end:
            regions.append(("data", cur, main_end))

        for region in regions:
            if region[0] == "data":
                self.emit_data_region(region[1], region[2])
            else:
                self.emit_func_region(
                    region[1], region[2], region[3], region[4], tmpdir
                )

        if self.pending:
            raise FallbackNeeded(
                "unplaceable labels: %s"
                % ", ".join("%08x" % a for a in sorted(self.pending))
            )
        unplaced = set(self.extra_labels) - self._extra_placed
        if unplaced:
            raise FallbackNeeded(
                "unplaceable extra labels: %s"
                % ", ".join("%08x" % a for a in sorted(unplaced))
            )
        if self.chunked:
            unplaced_req = self.required_labels - self._required_placed
            if unplaced_req:
                raise FallbackNeeded(
                    "unplaceable loc_ labels: %s"
                    % ", ".join("%08x" % a for a in sorted(unplaced_req))
                )
        return "\n".join(self.lines) + "\n"


# --------------------------------------------------------------------------
# Structure-only data emission (issue #36, docs/data.md)
# --------------------------------------------------------------------------


class ConfigError(Exception):
    """A pointer table or label that the data plan cannot honour."""


def pointer_valued(v):
    """True for a value that can be an address: ROM, EWRAM or IWRAM."""
    return (
        ROM_BASE <= v < 0x0A000000
        or 0x02000000 <= v < 0x02040000
        or 0x03000000 <= v < 0x03008000
    )


def parse_int(text, what):
    try:
        return int(str(text), 0)
    except ValueError:
        raise ConfigError("%s: bad number %r" % (what, text))


class DataPlan(object):
    """Every label and symbolic word of the structure-only data segments.

    Built once for all of them, because a pointer in one segment names a
    label in another (the room table in seg 20 points at RoomDef headers in
    seg 13).  `labels` maps an address to its names (data_symbols first,
    then extra_labels); `words` maps a 4-aligned word address to the
    operand of its `.word` line and `kinds` to "code" or "data"; `why`
    keeps the pointer table that proved a data pointer.
    """

    def __init__(self, rom, segments, db_rows, data_symbols, extra_labels,
                 pointer_tables, not_pointers, c_ranges=(), m4a=None):
        self.rom = rom
        self.c_ranges = list(c_ranges)  # [(start, end)] of c_data segments
        self.segments = segments  # [(name, start, end, asset)]
        self.db_rows = db_rows  # vma -> (name, isa)
        self.data_symbols = data_symbols  # value -> name
        self.extra_labels = extra_labels  # addr -> name
        self.not_pointers = not_pointers  # addr -> reason
        self.labels = {}
        self.words = {}
        self.kinds = {}
        self.why = {}
        self.missing = {}  # target addr -> [(slot addr, why)]
        self.errors = []
        # Slots of "extent": "next-label" tables (docs/data.md 5.1), kept
        # apart so the metrics can count them separately.
        self.heuristic_slots = set()
        # Pointer slots whose bit 0 is a flag, not part of the address
        # (a "targets" "tagged" field): emitted as `label+1` when set.
        self.tagged_slots = set()
        # Slots of tables marked "proof": "format": pointers proven by a
        # format parse that no code reads (docs/data.md 5.3), counted apart
        # from the consumer-proven ones.
        self.format_slots = set()
        # The m4a song structure (config "m4a", tools/m4a_struct.py,
        # docs/data.md 3.4): a label at every song header, track position,
        # voicegroup and wave, and a pointer slot at every pointer field,
        # at ANY alignment (track operands are unaligned; gas and ld handle
        # an unaligned R_ARM_ABS32).  Config names win over the generated
        # ones at the same address.
        self.m4a_labels = {}
        self.m4a_slots = {}
        if m4a:
            import m4a_struct
            try:
                self.m4a_labels, self.m4a_slots = m4a_struct.split_plan(
                    rom, {"m4a": m4a},
                    set(data_symbols) | set(extra_labels))
            except m4a_struct.M4AError as e:
                raise ConfigError("m4a: %s" % e)
        self._build_labels()
        self.slots = self._build_slots(pointer_tables)
        self._build_words()
        self._build_unaligned_words()

    # ---- helpers -------------------------------------------------------

    def segment_of(self, addr):
        for seg in self.segments:
            if seg[1] <= addr < seg[2]:
                return seg
        return None

    def word(self, addr):
        return u32(self.rom, vma_off(addr))

    def function_operand(self, v):
        """`.word` operand for a function-pointer value, or None.

        Thumb pointers carry bit 0 (`name+1`); ARM entries are even.  An
        even value equal to a Thumb entry is a `mov pc` jump-table target
        (lesson 4.38), which an R_ARM_ABS32 against a C-defined Thumb
        function would turn odd, so it is never symbolized.
        """
        if v & 1:
            row = self.db_rows.get(v & ~1)
            if row is not None and row[1] == "thumb":
                return "%s+1" % row[0]
            return None
        row = self.db_rows.get(v)
        if row is not None and row[1] == "arm":
            return row[0]
        return None

    def label_operand(self, v):
        """`.word` operand for a data-pointer value, or None: a label of a
        structure-only segment, else any named cell (RAM, or a ROM table
        still defined by absolute address)."""
        names = self.labels.get(v)
        if names:
            return names[0]
        return self.data_symbols.get(v)

    # ---- planning --------------------------------------------------------

    def _build_labels(self):
        for value, name in sorted(self.data_symbols.items()):
            if self.segment_of(value) is not None:
                self.labels.setdefault(value, []).append(name)
        for addr, name in sorted(self.extra_labels.items()):
            if self.segment_of(addr) is not None:
                names = self.labels.setdefault(addr, [])
                if name not in names:
                    names.append(name)
        for addr, name in sorted(self.m4a_labels.items()):
            if self.segment_of(addr) is not None:
                self.labels.setdefault(addr, []).append(name)

    def _table_slots(self, table, index):
        """[(slot addr, why)] for one "pointer_tables" entry."""
        what = "pointer_tables[%d]" % index
        why = table.get("why")
        if not why:
            raise ConfigError("%s needs a \"why\" (the consumer that proves "
                              "its layout)" % what)
        start = parse_int(table.get("start"), what + ".start")
        stride = parse_int(table.get("stride", 4), what + ".stride")
        offsets = [parse_int(o, what + ".pointers")
                   for o in table.get("pointers", [0])]
        if table.get("extent") == "next-label":
            # The C declares an array of pointers but not its length: the
            # table runs up to the next label (or the segment end), and
            # every word of that span must be NULL or pointer-valued.
            if "count" in table or "end" in table:
                raise ConfigError("%s: \"extent\": \"next-label\" takes no "
                                  "count or end" % what)
            if stride != 4 or offsets != [0]:
                raise ConfigError("%s: a next-label table is a plain array "
                                  "of pointers" % what)
            seg = self.segment_of(start)
            c_seg = [r for r in self.c_ranges if r[0] <= start < r[1]]
            if seg is not None:
                later = [a for a in self.labels if start < a < seg[2]]
                end = min(later) if later else seg[2]
            elif c_seg:
                # a table carved into C: its neighbours are C objects, named
                # by data_symbols (they are no asm labels any more)
                later = [a for a in self.data_symbols if start < a < c_seg[0][1]]
                end = min(later) if later else c_seg[0][1]
            else:
                raise ConfigError("%s: 0x%08X is not inside a data segment"
                                  % (what, start))
            for addr in range(start, end - 3, 4):
                v = self.word(addr)
                if v and not pointer_valued(v):
                    raise ConfigError(
                        "%s: word 0x%08X at 0x%08X is not a pointer, so the "
                        "table is shorter than the span to the next label "
                        "(0x%08X); give it an explicit end or drop it"
                        % (what, v, addr, end)
                    )
            count = (end - start) // 4
        elif "count" in table:
            count = parse_int(table["count"], what + ".count")
        elif "end" in table:
            end = parse_int(table["end"], what + ".end")
            if (end - start) % stride:
                raise ConfigError("%s: 0x%X bytes is not a multiple of the "
                                  "stride 0x%X" % (what, end - start, stride))
            count = (end - start) // stride
        else:
            raise ConfigError("%s needs \"count\" or \"end\"" % what)
        slots = []
        for i in range(count):
            for off in offsets:
                slots.append((start + i * stride + off, why))
        if table.get("extent") == "next-label":
            self.heuristic_slots.update(a for a, _w in slots)
        return slots, table.get("targets")

    def _build_slots(self, pointer_tables):
        slots = {}

        def add(addr, why):
            if addr % 4:
                raise ConfigError("pointer slot 0x%08X (%s) is not 4-aligned"
                                  % (addr, why))
            if any(s <= addr < e for s, e in self.c_ranges):
                # a record carved into C (src/data/): its pointer fields
                # are C initializers, not words of a data file
                return
            seg = self.segment_of(addr)
            if seg is None or addr + 4 > seg[2]:
                raise ConfigError("pointer slot 0x%08X (%s) is not inside a "
                                  "data segment" % (addr, why))
            slots.setdefault(addr, why)
            (self.format_slots if fmt else proven).add(addr)

        fmt = False
        proven = set()  # slots some consumer-proven table claims
        for index, table in enumerate(pointer_tables):
            proof = table.get("proof", "consumer")
            if proof not in ("consumer", "format"):
                raise ConfigError("pointer_tables[%d].proof must be "
                                  "\"consumer\" or \"format\"" % index)
            fmt = proof == "format"
            base, layout = self._table_slots(table, index)
            in_c = [r for r in self.c_ranges
                    if r[0] <= base[0][0] < r[1]] if base else []
            for addr, why in base:
                if in_c:
                    # the table itself is C now (src/data/): its words are
                    # not emitted, but its "targets" still are records here
                    if not in_c[0][0] <= addr < in_c[0][1]:
                        raise ConfigError(
                            "pointer_tables[%d] straddles the C-defined range "
                            "0x%08X-0x%08X" % (index, in_c[0][0], in_c[0][1]))
                    continue
                add(addr, why)
            if not layout:
                continue
            # "targets": every non-NULL word of the table points at one
            # record of this layout, whose own pointer fields are slots too.
            lwhy = layout.get("why")
            if not lwhy:
                raise ConfigError("pointer_tables[%d].targets needs a "
                                  "\"why\"" % index)
            loffs = [parse_int(o, "pointer_tables[%d].targets" % index)
                     for o in layout.get("pointers", [])]
            # "tagged": {"<off>": ["<off>", ...]}: the pointer field at <off>
            # carries a flag in bit 0; when the flag is set the record has
            # the listed extra pointer fields too (a longer record variant).
            tagged = {}
            for toff, extra in sorted(layout.get("tagged", {}).items()):
                what = "pointer_tables[%d].targets.tagged" % index
                tagged[parse_int(toff, what)] = [parse_int(o, what)
                                                 for o in extra]
            for addr, _why in base:
                if addr in self.not_pointers:
                    continue
                target = self.word(addr)
                if target == 0:
                    continue
                for off in loffs:
                    add(target + off, lwhy)
                for toff, extra in sorted(tagged.items()):
                    add(target + toff, lwhy)
                    self.tagged_slots.add(target + toff)
                    if self.word(target + toff) & 1:
                        for off in extra:
                            add(target + off, lwhy)
        fmt = False  # the m4a slots are a verified format parse (3.4)
        for addr, (_target, why) in sorted(self.m4a_slots.items()):
            if addr % 4 == 0:
                add(addr, why)
        # format-only means no consumer-proven table reaches the slot too
        self.format_slots -= proven
        return slots

    def _build_words(self):
        for name, start, end, asset in self.segments:
            addr = (start + 3) & ~3
            while addr + 4 <= end:
                self._plan_word(addr, asset)
                addr += 4

    def _plan_word(self, addr, asset):
        if addr in self.not_pointers:
            return
        inside = [a for a in (addr + 1, addr + 2, addr + 3) if a in self.labels]
        slot_why = self.slots.get(addr)
        v = self.word(addr)
        if slot_why is not None:
            if v == 0:
                return  # NULL stays inside the .incbin
            # a label strictly inside the word (a table the C declares
            # from a mid-word address, gUnk_0875841E) is emitted after the
            # word as `.set name, . - k` (emit_data_segment), so it stays a
            # section-relative symbol
            if addr in self.tagged_slots and v & 1:
                # a flagged data pointer: the label is at v & ~1
                operand = self.label_operand(v & ~1)
                kind = "data"
                if operand is None:
                    self.missing.setdefault(v & ~1, []).append(
                        (addr, slot_why))
                    return
                operand += "+1"
            else:
                operand = self.function_operand(v)
                kind = "code"
            if operand is None:
                operand = self.label_operand(v)
                kind = "data"
            if operand is None:
                self.missing.setdefault(v, []).append((addr, slot_why))
                return
            self.words[addr] = operand
            self.kinds[addr] = kind
            self.why[addr] = slot_why
            return
        if asset or inside:
            return
        operand = self.function_operand(v)
        if operand is not None:
            self.words[addr] = operand
            self.kinds[addr] = "code"

    def _build_unaligned_words(self):
        """The m4a slots that are not 4-aligned (track operands)."""
        for addr, (target, why) in sorted(self.m4a_slots.items()):
            if addr % 4 == 0:
                continue
            seg = self.segment_of(addr)
            if seg is None or addr + 4 > seg[2]:
                raise ConfigError("m4a slot 0x%08X is not inside a data "
                                  "segment" % addr)
            if u32(self.rom, vma_off(addr)) != target:
                raise ConfigError("m4a slot 0x%08X does not hold 0x%08X"
                                  % (addr, target))
            clash = [a for a in range((addr & ~3) - 4, addr + 4)
                     if a in self.words and a < addr + 4 and a + 4 > addr]
            inside = [a for a in (addr + 1, addr + 2, addr + 3)
                      if a in self.labels]
            if clash or inside:
                self.errors.append(
                    "m4a slot 0x%08X (%s) overlaps %s" % (
                        addr, why, "the word at 0x%08X" % clash[0] if clash
                        else "the label at 0x%08X" % inside[0]))
                continue
            operand = self.label_operand(target)
            if operand is None:
                self.missing.setdefault(target, []).append((addr, why))
                continue
            self.words[addr] = operand
            self.kinds[addr] = "data"
            self.why[addr] = why

    def missing_label_entries(self):
        """data_symbols entries that would resolve every missing target."""
        return dict(
            ("0x%08X" % v, DATA_PREFIX + "%08X" % v)
            for v in sorted(self.missing)
        )


GENERATED_HEADER = "@ Auto-generated by tools/split.py from baserom.gba"


def c_data_notes(path):
    """{name: comment} of the c_data rows of segments.txt (which C file
    and section defines them), for the data files' C-run comments."""
    notes = {}
    with open(path) as f:
        for line in f:
            parts = line.split("#", 1)
            p = parts[0].split()
            if len(p) >= 4 and p[2] == C_DATA_KIND:
                note = parts[1].strip() if len(parts) > 1 else ""
                notes[p[3]] = re.sub(r"\s*\(carved by[^)]*\)", "", note)
    return notes


def emit_data_piece(plan, name, start, end):
    """Body lines and stats of one structure-only data piece: its labels,
    symbolic `.word`s and `.incbin` slices of [start, end)."""
    labels = dict(
        (a, n) for a, n in plan.labels.items() if start <= a < end
    )
    words = dict((a, o) for a, o in plan.words.items() if start <= a < end)
    body = []
    stats = {"labels": 0, "code": 0, "data": 0, "heuristic": 0,
             "incbins": 0, "incbin_bytes": 0}

    def incbin(a, length):
        body.append('\t.incbin\t"%s", 0x%X, 0x%X'
                    % (BASEROM_NAME, vma_off(a), length))
        stats["incbins"] += 1
        stats["incbin_bytes"] += length

    cur = start
    for addr in sorted(set(labels) | set(words)):
        if addr < cur:
            # a label inside the pointer word just emitted: a symbol
            # relative to the location counter, `k` bytes back from the
            # word's end (DataPlan._plan_word allows it only in a slot)
            for label in labels.get(addr, []):
                body.append("\t.global\t%s" % label)
                body.append("\t.set\t%s, . - %d" % (label, cur - addr))
                stats["labels"] += 1
            continue
        if addr > cur:
            incbin(cur, addr - cur)
            cur = addr
        for label in labels.get(addr, []):
            body.append("\t.global\t%s" % label)
            body.append("%s:" % label)
            stats["labels"] += 1
        if addr in words:
            body.append("\t.word\t%s" % words[addr])
            stats[plan.kinds[addr]] += 1
            if addr in plan.heuristic_slots:
                stats["heuristic"] += 1
            cur = addr + 4
    if cur < end:
        incbin(cur, end - cur)
    return body, stats


def data_counts_lines(stats):
    h = ["@ %d label(s), %d code pointer(s), %d data pointer(s), %d .incbin"
         " slice(s) (0x%X bytes)."
         % (stats["labels"], stats["code"], stats["data"], stats["incbins"],
            stats["incbin_bytes"])]
    if stats["heuristic"]:
        h.append("@ %d of the pointers are in next-label tables (docs/data.md"
                 " 5.1)." % stats["heuristic"])
    return h


def section_head(name):
    return ['\t.section .%s, "a"' % name, "\t.global\t%s" % name,
            "%s:" % name]


def emit_data_zone(plan, zone, pieces, c_rows=()):
    """Text of `data/<zone>.s` and its stats.

    `pieces` are the zone's data segments [(name, start, end, kind, asset)]
    in address order; each is its own section, named after its
    segments.txt row, so linker.ld can list the C runs between them
    (`c_rows`: [(name, start, end, note)], tools/ldgroup.py, docs/data.md
    5.2).  A zone of one piece is the plain one-segment file.
    """
    total = {"labels": 0, "code": 0, "data": 0, "heuristic": 0,
             "incbins": 0, "incbin_bytes": 0}
    parts = []
    for name, start, end, kind, asset in pieces:
        body, stats = emit_data_piece(plan, name, start, end)
        for k in total:
            total[k] += stats[k]
        parts.append((name, start, end, kind, asset, body, stats))
    asset = any(p[4] for p in pieces)
    h = [
        "@ Auto-generated by tools/split.py from baserom.gba - DO NOT EDIT.",
        "@ Regenerate with: make split",
    ]
    if len(pieces) == 1:
        name, start, end, kind, _a = pieces[0]
        h.append("@ Segment %s: 0x%08X-0x%08X (%s, 0x%X bytes)"
                 % (name, start, end, kind, end - start))
    else:
        lo, hi = pieces[0][1], pieces[-1][2]
        h.append("@ Zone %s: 0x%08X-0x%08X, %d data pieces (0x%X bytes) and"
                 % (zone, lo, hi, len(pieces),
                    sum(e - s for _n, s, e, _k, _a in pieces)))
        h.append("@ %d C run(s) between them.  Each piece is its own section"
                 " and segments.txt" % len(c_rows))
        h.append("@ row; linker.ld lists the pieces and the C runs in address"
                 " order")
        h.append("@ (tools/ldgroup.py, docs/data.md 5.2).")
    h += [
        "@ Structure only (docs/data.md): labels, symbolic pointer words and",
        "@ .incbin slices of the user's baserom.gba; no ROM bytes are committed.",
    ]
    h += data_counts_lines(total)
    if asset:
        h.append("@ Asset segment: its bytes stay extracted from baserom.gba"
                 " forever; only")
        h.append("@ labels and consumer-proven pointer tables are structure.")
    out = h
    if len(pieces) == 1:
        out.append("")
        out += section_head(pieces[0][0]) + parts[0][5]
        return "\n".join(out) + "\n", total, parts
    # every piece and every C run between them, in address order; a C run
    # is a comment only (its bytes are the C file's section)
    items = [(p[1], 1, p) for p in parts] + [(r[1], 0, r) for r in c_rows]
    for _addr, is_piece, it in sorted(items, key=lambda x: (x[0], x[1])):
        out.append("")
        if not is_piece:
            cname, cs, ce, note = it
            out.append("@ 0x%08X-0x%08X: C, row %s%s"
                       % (cs, ce, cname, (" (%s)" % note) if note else ""))
            continue
        name, start, end, kind, _a, body, stats = it
        out.append("@ Piece %s: 0x%08X-0x%08X (%s, 0x%X bytes)"
                   % (name, start, end, kind, end - start))
        out += data_counts_lines(stats)
        out += section_head(name) + body
    return "\n".join(out) + "\n", total, parts


# --------------------------------------------------------------------------
# Verification: assemble every candidate, link the whole group at the real
# ROM VMAs (plus rom_syms.o and --defsym stand-ins for symbols that real
# code defines), and compare each section's bytes against baserom.
# --------------------------------------------------------------------------


def emit_ext_standins(defsyms_with_isa, tmpdir):
    """Build verification stand-in objects for symbols the real build gets
    from compiled C / hand asm (src/agb_sram.c, asm/crt0.s).

    `--defsym` absolutes carry no Thumb/ARM marker, so a `bl` from split
    code into one of them makes ld inject a 16-byte interworking stub into
    the layout (seen at 0x080CFA40, shifting every later section).  A
    labelled zero-size section pinned at the real VMA defines the same
    address WITH the right ISA and contributes no bytes.

    Returns [(section_name, vma, objpath)] for verify_group.
    """
    helpers = []
    for i, name in enumerate(sorted(defsyms_with_isa)):
        vma, isa = defsyms_with_isa[name]
        sec = ".ext_%03d" % i
        spath = os.path.join(tmpdir, "ext_%03d.s" % i)
        mode = "\t.thumb\n" if isa == "thumb" else "\t.arm\n"
        with open(spath, "w") as f:
            f.write(
                "@ Auto-generated verification stand-in for %s.\n"
                "\t.section %s, \"ax\"\n"
                "\t.global\t%s\n"
                "%s"
                "%s:\n" % (name, sec, name, mode, name)
            )
        opath = os.path.join(tmpdir, "ext_%03d.o" % i)
        run_checked([AS, "-mcpu=arm7tdmi", "-o", opath, spath], "as (%s)" % name)
        helpers.append((sec.lstrip("."), vma, opath))
    return helpers


def run_checked(cmd, what):
    proc = subprocess.run(cmd, capture_output=True, text=True)
    if proc.returncode != 0:
        raise RuntimeError(
            "%s failed:\n  %s\n%s" % (what, " ".join(cmd), proc.stderr)
        )
    return proc


def assemble_text(text, opath, incdirs=()):
    """Assemble `text` to `opath`; returns (ok, stderr)."""
    spath = opath[:-2] + ".s"
    with open(spath, "w") as f:
        f.write(text)
    cmd = [AS, "-mcpu=arm7tdmi"]
    for d in incdirs:
        cmd += ["-I", d]
    proc = subprocess.run(
        cmd + ["-o", opath, spath], capture_output=True, text=True
    )
    return proc.returncode == 0, proc.stderr


def verify_group(candidates, syms_obj, rom, tmpdir, helpers=None):
    """candidates: [(section, start, end, objpath)].

    One row per (section, object): several chunked files share one section
    (issue #25) and ld concatenates same-named input sections in
    command-line order, so callers must list an address-ordered segment's
    objects consecutively; one data file holds the sections of all the
    pieces of its zone (#167), and each object is linked once.
    Links all candidate objects together at their ROM VMAs and compares
    each section's bytes (including any alignment padding ld had to
    insert) with baserom.  Linking the whole group is
    essential: split files reference labels that other files define (e.g.
    the IRQ handler table pointing into game_code_early).  Returns the set
    of section names that mismatch.
    """
    sections = {}
    for sec, start, end, _obj in candidates:
        prev = sections.get(sec)
        if prev is None:
            sections[sec] = (start, end)
        elif prev != (start, end):
            sys.exit("error: section %r has conflicting bounds" % sec)
    script = ["SECTIONS\n{"]
    # Stand-in label sections first: zero-size, pinned outside every
    # candidate range, they only give external symbols their ISA marker.
    for sec, vma, _obj in helpers or []:
        script.append(
            "  .%s 0x%08X : { *(.%s) }\n" % (sec, vma, sec)
        )
    for sec, (start, _end) in sorted(sections.items()):
        script.append(
            "  .%s 0x%08X : { *(.%s) }\n" % (sec, start, sec)
        )
    script.append("}\n")
    lpath = os.path.join(tmpdir, "group.ld")
    with open(lpath, "w") as f:
        f.write("".join(script))
    epath = os.path.join(tmpdir, "group.elf")
    cmd = [LD, "-T", lpath, "-o", epath, syms_obj]
    for _sec, _vma, obj in helpers or []:
        cmd.append(obj)
    for _sec, _s, _e, obj in candidates:
        if obj not in cmd:
            cmd.append(obj)
    try:
        run_checked(cmd, "ld (group verify)")
    except RuntimeError as e:
        # A failed link (usually labels from a unit that is still missing,
        # e.g. dropped to raw in an earlier round) is not a byte mismatch:
        # report it specially so the caller can simply retry with the
        # units' current levels instead of demoting healthy sections.
        print("    group verify failed:\n%s" % e)
        return {LINK_FAILURE}
    failing = {}  # section -> first differing absolute address (None if ?)
    for sec, (start, end) in sorted(sections.items()):
        dump = os.path.join(tmpdir, "dump_%s.bin" % sec)
        try:
            run_checked(
                [OBJCOPY, "--dump-section", ".%s=%s" % (sec, dump), epath],
                "objcopy dump (%s)" % sec,
            )
        except RuntimeError as e:
            print("    %s" % e)
            failing[sec] = None
            continue
        if not os.path.exists(dump):
            print("    %s: section missing from verification ELF" % sec)
            failing[sec] = None
            continue
        with open(dump, "rb") as f:
            got = f.read()
        want = rom[vma_off(start):vma_off(end)]
        if got != want:
            first = next(
                (
                    i
                    for i in range(min(len(got), len(want)))
                    if got[i] != want[i]
                ),
                None,
            )
            diff_addr = start + first if first is not None else None
            print(
                "    %s: byte mismatch (got %d bytes, expected %d)%s"
                % (
                    sec,
                    len(got),
                    len(want),
                    "" if first is None else " first at 0x%08X" % diff_addr,
                )
            )
            failing[sec] = diff_addr
    return failing


# segment name -> why it stays asm (config "stays_asm"), filled by main()
STAYS_ASM = {}


def wrap_comment(text, width=76):
    """`text` as `@ ` comment lines of at most `width` characters."""
    out, line = [], "@"
    for word in text.split():
        if len(line) + 1 + len(word) > width and line != "@":
            out.append(line)
            line = "@  "
        line += " " + word
    out.append(line)
    return out


# --------------------------------------------------------------------------
# rom_syms.s
# --------------------------------------------------------------------------


def emit_rom_syms(db, exclude, path, data_symbols, abs_symbols=None):
    lines = [
        "@ Auto-generated by tools/split.py - DO NOT EDIT. Regenerate with: make split",
        "@ Stays asm: no bytes, only absolute symbols for the linker (docs/audit.md",
        "@ section 2).",
        "@ Absolute symbols for every function in docs/analysis/symbols.csv that is",
        "@ not defined as a real label by split asm files or compiled C sources,",
        "@ so generated files can reference not-yet-split code symbolically",
        "@ (e.g. `.word sub_08001518+1' or `bl __divsi3').",
        "",
    ]
    count = 0
    for vma in sorted(db):
        name = db[vma]
        if name in exclude:
            continue
        lines.append("\t.global\t%s" % name)
        lines.append("%s = 0x%08X" % (name, vma))
        count += 1
    if data_symbols:
        lines.append("")
        lines.append("@ Named cells (tools/split_config.json \"data_symbols\") that no split")
        lines.append("@ data segment defines as a real label: RAM and I/O cells, referenced")
        lines.append("@ symbolically from split literal pools and pointer tables (a ROM")
        lines.append("@ address here is a table still waiting for its label, docs/data.md):")
        for value in sorted(data_symbols):
            lines.append("\t.global\t%s" % data_symbols[value])
            lines.append("%s = 0x%08X" % (data_symbols[value], value))
            count += 1
    if abs_symbols:
        lines.append("")
        lines.append("@ Link-time constants (tools/split_config.json \"abs_symbols\"):")
        lines.append("@ absolute symbols whose VALUE is the constant (SDK idiom, e.g.")
        lines.append("@ the m4a driver reads (u16)gNumMusicPlayers).  Unlike")
        lines.append("@ data_symbols these never rename split pool words.")
        for name in sorted(abs_symbols):
            lines.append("\t.global\t%s" % name)
            lines.append("%s = 0x%08X" % (name, abs_symbols[name]))
            count += 1
    with open(path, "w") as f:
        f.write("\n".join(lines) + "\n")
    return count


# --------------------------------------------------------------------------
# Main
# --------------------------------------------------------------------------


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rom", default="baserom.gba")
    parser.add_argument("--config", default="tools/split_config.json")
    parser.add_argument("--segments", default="docs/analysis/segments.txt")
    parser.add_argument("--symbols", default="docs/analysis/symbols.csv")
    parser.add_argument("--asm-dir", default="asm")
    parser.add_argument("--data-dir", default="data")
    parser.add_argument(
        "--keep-tmp", action="store_true",
        help="keep the scratch directory for debugging link failures",
    )
    parser.add_argument(
        "--missing-labels", metavar="JSON",
        help="when a pointer table points at unlabeled addresses, write the "
             "data_symbols entries that would label them to this file",
    )
    args = parser.parse_args()

    with open(args.rom, "rb") as f:
        rom = f.read()
    segdefs = parse_segments_file(args.segments)
    rows = load_symbol_db(args.symbols)
    db = dict((vma, name) for vma, (name, _isa) in rows.items())
    with open(args.config) as f:
        cfg = json.load(f)

    def parse_addr_map(table, key):
        try:
            return dict(
                (int(addr, 16), name) for addr, name in table.items()
            )
        except (AttributeError, ValueError):
            sys.exit(
                "error: %s must map \"0x...\" hex addresses to names"
                % key
            )

    extra_labels = parse_addr_map(cfg.get("extra_labels", {}), "extra_labels")
    data_symbols = parse_addr_map(cfg.get("data_symbols", {}), "data_symbols")
    try:
        abs_symbols = dict(
            (name, int(value, 16))
            for name, value in cfg.get("abs_symbols", {}).items()
        )
    except (AttributeError, ValueError):
        sys.exit("error: abs_symbols must map names to \"0x...\" hex values")
    not_pointers = parse_addr_map(cfg.get("not_pointers", {}), "not_pointers")
    # "isa_ranges": [{"start", "end", "isa", "why"}] code inside a
    # function whose ISA is not the function's own (the ARM islands that
    # follow `adr rN, <label>; bx rN` in m4a_1's Thumb functions): decoded
    # and emitted in that ISA, so their branches and `adr`s are labels and
    # the Thumb decode cannot invent branches out of ARM words (lesson
    # 4.134).  Each range must be word-aligned for "arm" (`.arm` aligns).
    isa_ranges = []
    for i, r in enumerate(cfg.get("isa_ranges", [])):
        try:
            a, b = int(r["start"], 16), int(r["end"], 16)
        except (KeyError, TypeError, ValueError):
            sys.exit("error: isa_ranges[%d] needs hex \"start\" and \"end\"" % i)
        isa = r.get("isa")
        if isa not in ("arm", "thumb"):
            sys.exit("error: isa_ranges[%d] needs \"isa\": \"arm\" or \"thumb\"" % i)
        if not r.get("why"):
            sys.exit("error: isa_ranges[%d] needs a \"why\"" % i)
        if b <= a or a % (4 if isa == "arm" else 2) or b % (4 if isa == "arm" else 2):
            sys.exit("error: isa_ranges[%d] must be a non-empty %s-aligned range"
                     % (i, "word" if isa == "arm" else "halfword"))
        isa_ranges.append((a, b, isa))
    # "raw_words": {"0x<addr>": "<reason>"} literal-pool words of split code
    # that are constants, not addresses: emitted as numbers with a
    # same-line `@ raw: <reason>` (the justification make audit reads)
    raw_words = {}
    for addr, why in cfg.get("raw_words", {}).items():
        try:
            a = int(addr, 16)
        except (TypeError, ValueError):
            sys.exit("error: raw_words keys must be \"0x...\" addresses")
        if a % 4 or not why:
            sys.exit("error: raw_words[%s] needs a word address and a reason"
                     % addr)
        raw_words[a] = why
    pointer_tables = cfg.get("pointer_tables", [])
    if not isinstance(pointer_tables, list):
        sys.exit("error: pointer_tables must be a list of tables")

    tmpdir = tempfile.mkdtemp(prefix="split_")
    try:
        entries = []
        data_entries = []  # structure-only segments (issue #36)
        # "zone": the data file a piece is written to (#167): the pieces of
        # a zone cut around C runs share data/<zone>.s, one section each
        data_zone = {}
        for seg_cfg in cfg["segments"]:
            name = seg_cfg["name"]
            if seg_cfg.get("stays_asm"):
                STAYS_ASM[name] = seg_cfg["stays_asm"]
            if name not in segdefs:
                sys.exit("error: segment %r not in %s" % (name, args.segments))
            start, end, kind = segdefs[name]
            if kind == "c_code":
                sys.exit(
                    "error: segment %r is c_code (owned by compiled C); "
                    "not splittable" % name
                )
            if vma_off(end) > len(rom):
                sys.exit("error: segment %r exceeds ROM size" % name)
            chunk_bytes = seg_cfg.get("chunk_bytes")
            if chunk_bytes is not None:
                try:
                    chunk_bytes = int(str(chunk_bytes), 0)
                except ValueError:
                    sys.exit(
                        "error: segment %r has a bad chunk_bytes value %r"
                        % (name, chunk_bytes)
                    )
                if chunk_bytes <= 0:
                    sys.exit("error: chunk_bytes must be positive")
            asset = bool(seg_cfg.get("asset", False))
            zone = seg_cfg.get("zone", name)
            if kind in STRUCTURE_KINDS:
                if chunk_bytes is not None:
                    sys.exit("error: data segment %r cannot be chunked" % name)
                if not re.fullmatch(r"[a-z][a-z0-9_]*", zone):
                    sys.exit("error: segment %r: zone %r is not snake_case"
                             % (name, zone))
                data_entries.append((name, start, end, kind, asset))
                data_zone[name] = zone
                continue
            if asset:
                sys.exit("error: only data segments can be assets (%r)" % name)
            if "zone" in seg_cfg:
                sys.exit("error: only data segments have a zone (%r)" % name)
            entries.append((name, start, end, kind, chunk_bytes))
        zones = {}  # zone -> [(name, start, end, kind, asset)] by address
        for ent in data_entries:
            zones.setdefault(data_zone[ent[0]], []).append(ent)
        for zone, pieces in zones.items():
            pieces.sort(key=lambda p: p[1])
            row = segdefs.get(zone)
            if row is not None and data_zone.get(zone) != zone:
                sys.exit("error: zone %r is the name of segment %r, which is"
                         " not one of its pieces" % (zone, zone))
        all_ranges = (
            [(s, e) for _n, s, e, _k, _c in entries]
            + [(s, e) for _n, s, e, _k, _a in data_entries]
        )
        data_labels = dict(
            (a, n) for a, n in extra_labels.items()
            if any(s <= a < e for _n, s, e, _k, _a in data_entries)
        )

        # Config sanity: every extra label must live inside one configured
        # segment and must not shadow a database symbol defined elsewhere.
        for addr, label in sorted(extra_labels.items()):
            if not any(s <= addr < e for s, e in all_ranges):
                sys.exit(
                    "error: extra label %s at 0x%08X is outside every "
                    "configured segment" % (label, addr)
                )
            if addr in db and db[addr] != label:
                sys.exit(
                    "error: extra label %s at 0x%08X collides with "
                    "database symbol %s" % (label, addr, db[addr])
                )
        db_names = set(db.values())
        for value, label in sorted(data_symbols.items()):
            if label in db_names:
                sys.exit(
                    "error: data symbol %s collides with a database "
                    "function name" % label
                )

        # Symbols that get real labels: every DB function inside a split
        # range is emitted as a label by its segment file.
        split_ranges = [(s, e) for _n, s, e, _k, _c in entries]
        in_split = set(
            name
            for vma, name in db.items()
            if any(s <= vma < e for s, e in split_ranges)
        )

        # Functional tables defined in C (c_data rows, tools/carve_data.py):
        # their data_symbols are C symbols.  They leave rom_syms.s and get
        # absolute stand-ins for the verification link only.
        c_ranges = sorted(
            (s, e) for _n, (s, e, k) in segdefs.items() if k == C_DATA_KIND
        )
        c_defined = dict(
            (v, n) for v, n in data_symbols.items()
            if any(s <= v < e for s, e in c_ranges)
        )
        for addr, label in sorted(extra_labels.items()):
            if any(s <= addr < e for s, e in c_ranges):
                sys.exit(
                    "error: extra label %s at 0x%08X is inside a c_data "
                    "segment: C defines it, so it belongs in data_symbols"
                    % (label, addr)
                )

        # Structure-only data segments: one plan for all of them, since a
        # pointer in one names a label in another.
        try:
            plan = DataPlan(
                rom,
                [(n, s, e, a) for n, s, e, _k, a in data_entries],
                rows, data_symbols, extra_labels, pointer_tables,
                not_pointers, c_ranges, m4a=cfg.get("m4a"),
            )
        except ConfigError as e:
            sys.exit("error: %s" % e)
        if plan.errors:
            sys.exit("error: data plan:\n  " + "\n  ".join(plan.errors))
        if plan.missing:
            print("error: %d pointer-table target(s) have no label:"
                  % len(plan.missing))
            for v in sorted(plan.missing)[:20]:
                slot, why = plan.missing[v][0]
                print("    0x%08X (word at 0x%08X, %s)" % (v, slot, why))
            if args.missing_labels:
                with open(args.missing_labels, "w") as f:
                    json.dump(plan.missing_label_entries(), f, indent=2,
                              sort_keys=True)
                    f.write("\n")
                print("wrote %s (add these to data_symbols)"
                      % args.missing_labels)
            sys.exit(1)
        labeled_names = set(
            n for names in plan.labels.values() for n in names
        )
        unlabeled_symbols = dict(
            (v, n) for v, n in data_symbols.items()
            if n not in labeled_names and v not in c_defined
        )

        exclude = set(cfg.get("external_defined", [])) | in_split
        syms_path = os.path.join(args.asm_dir, "rom_syms.s")
        count = emit_rom_syms(
            db, exclude, syms_path, unlabeled_symbols, abs_symbols
        )
        print("wrote %s (%d absolute symbols)" % (syms_path, count))
        syms_obj = os.path.join(tmpdir, "rom_syms.o")
        verify_syms = syms_path
        if c_defined:
            verify_syms = os.path.join(tmpdir, "verify_syms.s")
            with open(syms_path) as src, open(verify_syms, "w") as dst:
                dst.write(src.read())
                dst.write("\n@ verification only: data defined by src/data/*.c\n")
                for v in sorted(c_defined):
                    dst.write("\t.global\t%s\n%s = 0x%08X\n"
                              % (c_defined[v], c_defined[v], v))
        run_checked(
            [AS, "-mcpu=arm7tdmi", "-o", syms_obj, verify_syms], "as (rom_syms.s)"
        )

        # Verification stand-ins for symbols that real code defines
        # (asm/crt0.s, src/agb_sram.c): the real build resolves them from
        # those objects; rom_syms.o does not define them by design.  They
        # are emitted as ISA-labelled zero-size sections instead of
        # --defsyms absolutes: absolutes carry no Thumb marker, so `bl`s
        # into them make ld insert 16-byte interworking stubs that shift
        # every later section (and the byte compare fails).
        name_to_row = dict((name, (vma, isa)) for vma, (name, isa) in rows.items())
        ext_defs = {}
        for name in cfg.get("external_defined", []):
            if name in name_to_row:
                ext_defs[name] = name_to_row[name]
        helpers = emit_ext_standins(ext_defs, tmpdir)

        # Structure-only data files never fall back: they hold no
        # instructions, so a verification mismatch is a planning bug.
        rom_dir = os.path.dirname(os.path.abspath(args.rom))
        c_notes = c_data_notes(args.segments)
        data_results = {}  # zone -> (text, objpath, stats, parts)
        for zone, pieces in sorted(zones.items(), key=lambda z: z[1][0][1]):
            lo, hi = pieces[0][1], pieces[-1][2]
            c_rows = [
                (n, s, e, c_notes.get(n, ""))
                for n, (s, e, k) in segdefs.items()
                if k == C_DATA_KIND and lo <= s < hi
            ] if len(pieces) > 1 else []
            try:
                text, dstats, parts = emit_data_zone(plan, zone, pieces,
                                                     c_rows)
            except ConfigError as e:
                sys.exit("error: %s" % e)
            obj = os.path.join(tmpdir, "data_%s.o" % zone)
            ok, err = assemble_text(text, obj, incdirs=(rom_dir,))
            if not ok:
                sys.exit("error: data file %s does not assemble:\n%s"
                         % (zone, err))
            data_results[zone] = (text, obj, dstats, parts)

        # Plan emission units: one output file per unit. Flat segments are
        # a single unit owning section <name>; chunked segments (optional
        # "chunk_bytes" in the config, issue #25) are cut at even function
        # boundaries and every chunk shares the segment's linker section,
        # so linker.ld needs no edit and ld concatenates the chunks in
        # command-line (address) order.
        units = {}  # uid -> unit dict
        order = []
        seg_names_by_segment = {}
        for name, start, end, kind, chunk_bytes in entries:
            funcs_all = [
                (vma, db[vma], rows[vma][1])
                for vma in sorted(db)
                if start <= vma < end
            ]
            func_vmas = [v for v, _n, _i in funcs_all]
            cuts = (
                compute_chunks(start, end, func_vmas, chunk_bytes)
                if chunk_bytes
                else [start, end]
            )
            num = len(cuts) - 1
            width = max(2, len(str(num - 1)))
            print(
                "splitting %-28s 0x%08X-0x%08X (%s)"
                % (name, start, end, kind)
            )
            seg_extra = dict(
                (a, l) for a, l in extra_labels.items() if start <= a < end
            )

            # Segment-wide name table + global loc_ label set. Every
            # non-function branch/adr target inside a chunked segment gets
            # a global loc_XXXXXXXX label defined by its owning chunk so
            # any other chunk can reference it; flat segments keep the
            # legacy file-local .L_ labels instead.
            seg_names = {}
            for vma, fname, _isa in funcs_all:
                seg_names.setdefault(vma, fname)
            for addr, lab in seg_extra.items():
                seg_names.setdefault(addr, lab)
            required_all = set()
            if num > 1:
                targets = collect_ref_targets(rom, start, end, funcs_all,
                                              isa_ranges)
                for target in sorted(targets):
                    if start <= target < end and target not in seg_names:
                        loc = "loc_%08x" % target
                        seg_names[target] = loc
                        required_all.add(target)
                print(
                    "    %d chunks (%d cross-file loc_ labels):"
                    % (num, len(required_all))
                )
                for ci in range(num):
                    print(
                        "      %s_%0*d: 0x%08X-0x%08X"
                        % (name, width, ci, cuts[ci], cuts[ci + 1])
                    )
            seg_names_by_segment[name] = seg_names

            # Layout flag: configured chunk_bytes segments live in their
            # own directory even when they fit a single chunk. Cross-file
            # (chunked) emitter semantics kick in only with 2+ chunks.
            layout = bool(chunk_bytes)
            for ci in range(num):
                cs, ce = cuts[ci], cuts[ci + 1]
                uid = (
                    "%s_%0*d" % (name, width, ci) if layout else name
                )
                rel = (
                    os.path.join(name, "%s.s" % uid)
                    if layout
                    else "%s.s" % name
                )
                units[uid] = {
                    "uid": uid,
                    "section": name,
                    "rel": rel,
                    "chunked": num > 1,
                    "index": ci if num > 1 else None,
                    "num_chunks": num,
                    "seg_start": start,
                    "seg_end": end,
                    "start": cs,
                    "end": ce,
                    "kind": kind,
                    "funcs": [
                        (v, n, i) for v, n, i in funcs_all if cs <= v < ce
                    ],
                    "extra": dict(
                        (a, l) for a, l in seg_extra.items() if cs <= a < ce
                    ),
                    "required": set(
                        a for a in required_all if cs <= a < ce
                    ),
                }
                order.append(uid)

        # Rounds: prefer real instructions (level 0); fall back to raw
        # .short/.byte emission (level 1) only as a last resort.  Two
        # repair mechanisms run before that:
        #
        #   * gas ERRORS (later-arch mnemonics for undefined-decode
        #     halfwords: revsh, yield, FPA junk, ...) are mapped back to
        #     instruction addresses and those are forced to raw bytes;
        #   * gas ALIAS re-encodings assemble cleanly but to the canonical
        #     encoding instead of the original one (`movs r0, r2` printed
        #     for a `lsls r0, r2, #0` halfword re-encodes as adds).  These
        #     surface as byte mismatches in the group verification; the
        #     first differing address is mapped back to its instruction
        #     line and blacklisted, and the unit is regenerated.
        #
        # Objects that pass stay in every subsequent verification link:
        # their real labels may be the only definition of symbols other
        # files reference (they are excluded from rom_syms.s via in_split),
        # so a retry round without them would fail to link.
        levels = dict((uid, 0) for uid in order)
        raw_addrs = dict((uid, set()) for uid in order)
        fail_count = dict((uid, 0) for uid in order)
        results = {}  # uid -> (text, objpath, stats, emitter)
        for attempt in range(40):
            # 1) Fill in any missing units (first round, or surgically
            # deleted / newly demoted ones).
            for uid in order:
                if uid in results:
                    continue
                u = units[uid]
                em = SegmentEmitter(
                    rom, u["section"], u["start"], u["end"], u["kind"],
                    u["funcs"], db, levels[uid],
                    extra_labels=u["extra"], data_symbols=data_symbols,
                    chunk_index=u["index"], num_chunks=u["num_chunks"],
                    seg_start=u["seg_start"], seg_end=u["seg_end"],
                    required_labels=u["required"] if u["chunked"] else None,
                    seg_names=(
                        seg_names_by_segment[u["section"]]
                        if u["chunked"] else None
                    ),
                )
                em.forced_raw_addrs = raw_addrs[uid]
                em.isa_ranges = [
                    (a, b, i) for a, b, i in isa_ranges
                    if a < u["end"] and b > u["start"]]
                em.raw_words = dict(
                    (a, w) for a, w in raw_words.items()
                    if u["start"] <= a < u["end"])
                em.data_labels = data_labels
                text = None
                obj = os.path.join(
                    tmpdir, "%s_%d_%d.o" % (uid, em.level, attempt % 2)
                )
                for _repair in range(16):
                    try:
                        text = em.emit(tmpdir)
                    except FallbackNeeded as e:
                        print("    %s: level %d unusable: %s" % (uid, em.level, e))
                        break
                    ok, err = assemble_text(text, obj)
                    if ok:
                        break
                    bad = em.addrs_from_asm_errors(err)
                    fresh = bad - em.forced_raw_addrs
                    if not fresh:
                        print(
                            "    %s: level %d does not assemble:\n%s"
                            % (uid, em.level, err)
                        )
                        break
                    em.forced_raw_addrs |= fresh
                if text is None:
                    if levels[uid] == 0:
                        levels[uid] = 1
                        continue
                    sys.exit(
                        "error: %s cannot even be emitted as raw bytes" % uid
                    )
                results[uid] = (text, obj, em.stats, em)

            # 2) Verify the whole group.
            candidates = []
            for uid in order:
                u = units[uid]
                _t, obj, _s, _e = results[uid]
                candidates.append(
                    (u["section"], u["seg_start"], u["seg_end"], obj)
                )
            for zone, pieces in zones.items():
                for name, start, end, _kind, _asset in pieces:
                    candidates.append((name, start, end,
                                       data_results[zone][1]))
            failing = verify_group(
                candidates, syms_obj, rom, tmpdir, helpers=helpers
            )
            if not failing:
                break
            if LINK_FAILURE in failing:
                sys.exit(
                    "error: group link failed even with every unit present\n%s"
                    % failing
                )
            bad_data = sorted(set(failing) & set(data_zone))
            if bad_data:
                sys.exit(
                    "error: structure-only data segment(s) do not match "
                    "baserom: %s" % ", ".join(bad_data)
                )

            # 3) Surgical repair: route each section's FIRST differing
            # address to the unit that owns it and force that instruction
            # to raw bytes.  Only the owner regenerates next round.
            for sec in sorted(failing):
                diff_addr = failing[sec]
                owner = None
                for uid in order:
                    u = units[uid]
                    if u["section"] == sec and (
                        diff_addr is None
                        or u["start"] <= diff_addr < u["end"]
                    ):
                        owner = uid
                        if diff_addr is not None:
                            break
                if owner is None:
                    sys.exit("error: no unit owns %r" % sec)
                u = units[owner]
                em = results[owner][3]
                addr = None
                if diff_addr is not None and u["start"] <= diff_addr < u["end"]:
                    addr = em.addr_for_offset(diff_addr - u["start"])
                progressed = False
                if addr is not None and addr not in raw_addrs[owner]:
                    raw_addrs[owner].add(addr)
                    fail_count[owner] = 0
                    progressed = True
                    print(
                        "    %s: alias/decode mismatch at 0x%08X;"
                        " forcing raw bytes there" % (owner, addr)
                    )
                del results[owner]
                if progressed:
                    continue
                fail_count[owner] += 1
                if levels[owner] == 0 and fail_count[owner] >= 2:
                    levels[owner] = 1
                    fail_count[owner] = 0
                    print(
                        "    %s: no surgical fix found; falling back to"
                        " RAW emission" % owner
                    )
        else:
            missing = [uid for uid in order if uid not in results]
            sys.exit(
                "error: split did not converge; unverified files: %s"
                % ", ".join(missing)
            )
        missing = [uid for uid in order if uid not in results]
        if missing:
            sys.exit(
                "error: files could not be emitted byte-identically: %s"
                % ", ".join(missing)
            )

        # A segment that a carve consumed ENTIRELY disappears from the
        # config, so the loop below never visits it and never purges its
        # chunk directory.  The Makefile's asm/*/*.s glob would keep
        # assembling the orphan next to the C module that replaced it
        # ("multiple definition of sub_XXXXXXXX").  Every directory under
        # asm/ belongs to a configured segment; anything else is stale.
        configured = {name for name, _s, _e, _k, _cb in entries}
        if os.path.isdir(args.asm_dir):
            for child in sorted(os.listdir(args.asm_dir)):
                child_path = os.path.join(args.asm_dir, child)
                if os.path.isdir(child_path) and child not in configured:
                    shutil.rmtree(child_path)
                    print("    removed %s (segment no longer configured)"
                          % child_path)

        made_dirs = set()
        for name, start, end, kind, chunk_bytes in entries:
            blob = os.path.join(args.data_dir, "%s.s" % name)
            if os.path.exists(blob):
                os.remove(blob)
                print("    removed %s (incbin slice replaced)" % blob)
            seg_uids = [u for u in order if units[u]["section"] == name]
            raw_any = any(levels[u] != 0 for u in seg_uids)
            total_funcs = sum(len(units[u]["funcs"]) for u in seg_uids)
            note = " [RAW FALLBACK]" if raw_any else ""
            print(
                "    %s -> %d file(s), %d functions%s"
                % (name, len(seg_uids), total_funcs, note)
            )
            for uid in seg_uids:
                text, _obj, stats, _em = results[uid]
                out_path = os.path.join(args.asm_dir, units[uid]["rel"])
                out_dir = os.path.dirname(out_path)
                if out_dir not in made_dirs:
                    made_dirs.add(out_dir)
                    # Regeneration stability: drop stale chunks from an
                    # earlier run with a different chunk layout.  Gate on the
                    # directory layout, not on `chunked`: a segment that
                    # shrinks to a single chunk (e.g. after a carve) keeps its
                    # directory but stops being `chunked`, and the orphaned
                    # tail chunks would still be picked up by the Makefile's
                    # asm/*/*.s glob and assembled alongside the new segment.
                    if out_dir != args.asm_dir and os.path.isdir(out_dir):
                        shutil.rmtree(out_dir)
                    os.makedirs(out_dir, exist_ok=True)
                with open(out_path, "w") as f:
                    f.write(text)
                print(
                    "    wrote %s: %d functions, %d insns, %d words"
                    " (%d symbolic, %d named), %d raw bytes"
                    % (
                        out_path,
                        len(units[uid]["funcs"]),
                        stats["instructions"],
                        stats["pool_words"] + stats["symbolic_words"]
                        + stats["named_words"],
                        stats["symbolic_words"],
                        stats["named_words"],
                        stats["raw_instructions"],
                    )
                )

        # Structure-only data segments live in data/ (pret layout: asm/ is
        # code), one file per zone; a value-list file an older split left
        # in asm/ is removed.
        written = set()
        for zone, pieces in sorted(zones.items(), key=lambda z: z[1][0][1]):
            text, _obj, dstats, _parts = data_results[zone]
            for name, _s, _e, _k, _a in pieces:
                old = os.path.join(args.asm_dir, "%s.s" % name)
                if os.path.exists(old):
                    os.remove(old)
                    print("    removed %s (moved to %s/)"
                          % (old, args.data_dir))
            out_path = os.path.join(args.data_dir, "%s.s" % zone)
            with open(out_path, "w") as f:
                f.write(text)
            written.add(os.path.basename(out_path))
            print(
                "    wrote %s: %s%d labels, %d code + %d data pointers, "
                "%d incbins%s"
                % (out_path,
                   ("%d pieces, " % len(pieces)) if len(pieces) > 1 else "",
                   dstats["labels"], dstats["code"], dstats["data"],
                   dstats["incbins"],
                   " [asset]" if any(p[4] for p in pieces) else "")
            )
        # A generated data file that no zone writes any more (its pieces
        # moved into a zone file, or a carve consumed its segment) would
        # still be assembled by the Makefile's data/*.s glob: remove it.
        if os.path.isdir(args.data_dir):
            for fname in sorted(os.listdir(args.data_dir)):
                path = os.path.join(args.data_dir, fname)
                if not fname.endswith(".s") or fname in written:
                    continue
                with open(path) as f:
                    first = f.readline()
                if first.startswith(GENERATED_HEADER):
                    os.remove(path)
                    print("    removed %s (no zone writes it)" % path)
    finally:
        if args.keep_tmp:
            print("kept scratch dir: %s" % tmpdir)
        else:
            shutil.rmtree(tmpdir, ignore_errors=True)


if __name__ == "__main__":
    main()
