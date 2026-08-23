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
  * odd segment boundaries are handled explicitly: gas silently aligns
    Thumb instructions to 2, so a segment with an odd start (e.g.
    sdk_reset_helper at 0x080CFA7F) or an odd trailing byte emits those
    bytes as raw `.byte`/`.short` (`.short`/`.word` themselves are emitted
    unaligned without padding, which was verified empirically).

Every generated file is verified before being written: the tool assembles
it, links it at the segment's ROM VMA with a throwaway linker script plus
rom_syms.o, objcopies the section out and compares it against the original
baserom bytes.  If real instructions fail to round-trip, the whole segment
falls back to raw `.short`/`.byte` emission (a verbatim byte copy), so the
output is always byte-identical.  `make compare` remains the final proof.

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


class FallbackNeeded(Exception):
    """A segment could not be emitted at the current detail level."""


LINK_FAILURE = "<link>"


def compute_chunks(start, end, func_vmas, chunk_bytes):
    """Cut points for a chunked segment (issue #25).

    Cuts land on EVEN function-boundary addresses roughly `chunk_bytes`
    apart so that no function straddles a chunk and every chunk begins at
    an even ROM address: gas aligns Thumb instructions to 2 within a
    section and ld aligns each input section to its own sh_addralign at
    absolute addresses, so a chunk whose first byte is odd can never
    contain real instructions.  An odd segment start therefore produces a
    tiny leading data-only chunk up to the nearest even function boundary.
    """
    cuts = [start]
    pos = start
    while pos < end:
        window_end = min(end, pos + chunk_bytes)
        cands = [v for v in func_vmas if pos < v <= window_end and v % 2 == 0]
        if cands:
            nxt = min(cands) if pos % 2 else max(cands)
        else:
            later = [v for v in func_vmas if v > pos and v % 2 == 0]
            nxt = later[0] if later and later[0] < end else end
        cuts.append(nxt)
        pos = nxt
    return cuts


def collect_ref_targets(rom, start, end, funcs):
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
        off = vma_off(vma)
        stop_off = vma_off(stop)
        while off < stop_off:
            if isa == "thumb":
                size, info = thumb_decode(rom, off)
            else:
                size, info = arm_decode(rom, off)
            if info:
                if "branch" in info or "pcadd" in info:
                    key = "branch" if "branch" in info else "pcadd"
                    targets.add(info[key])
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
        return 2, {"pcadd": base + (hw & 0xFF) * 4}
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
        OBJDUMP, "-D", "-b", "binary", "-marmv4t",
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
        self._noted_raw = False
        self._extra_placed = set()
        self._required_placed = set()
        # Addresses whose objdump text gas rejects (undefined-decode-space
        # halfwords printed as later-arch mnemonics); emit_instruction
        # forces them to raw .short bytes.  Persists across emit() calls so
        # the assembly repair loop can grow it incrementally.
        self.forced_raw_addrs = set()
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
                    # after the label line was already written).
                    if addr >= self.cursor:
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
            off = vma_off(vma)
            stop_off = vma_off(stop)
            while off < stop_off:
                if isa == "thumb":
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
                off += size

        for target in self.branch_targets:
            if self.chunked:
                break
            if self.start < target < self.end and target not in self.func_map:
                label = LOCAL_PREFIX + "%08x" % target
                labels = self.pending.setdefault(target, [])
                if label not in labels:
                    labels.append(label)

    # ---- word annotation ----------------------------------------------

    def word_line(self, addr):
        """Emit line(s) for the 4 bytes at `addr` (a pool or data word)."""
        v = u32(self.rom, vma_off(addr))
        if v in self.data_symbols:
            self.stats["named_words"] += 1
            return "\t.word\t%s" % self.data_symbols[v]
        if v & 1:
            target = v & ~1
            if target in self.db:
                self.stats["symbolic_words"] += 1
                return "\t.word\t%s+1" % self.db[target]
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
        pretty = (
            self.level == 0
            and (rs - self.start) % (4 if isa == "arm" else 2) == 0
        )
        if pretty:
            if isa == "thumb":
                self.lines.append("\t.thumb_func")
            self.lines.append("\t.global\t%s" % name)
            self.lines.append("%s:" % name)
            self.lines.append("\t.thumb" if isa == "thumb" else "\t.arm")
        else:
            # Raw mode (or an odd section offset, which gas would pad):
            # label only, no mode directives, so the section can stay at
            # alignment 1 when no real instruction is ever emitted.
            self.lines.append("\t.global\t%s" % name)
            self.lines.append("%s:" % name)
            if self.level == 0 and not self._noted_raw:
                self._noted_raw = True
                print(
                    "    note: %s at 0x%08X (and any other function at an "
                    "odd section offset) emitted as raw data" % (name, rs)
                )

        dis = {}
        if pretty:
            # One objdump sweep per ISA covers the rest of the segment.
            # Thumb-16 decoding is address-stable (every instruction is one
            # halfword; even a false `bl' prefix in data only corrupts its
            # own 4-byte window), and a big buffer avoids objdump's `...'
            # elision of trailing zero instructions that small per-function
            # buffers trigger.
            if isa not in self.dis_cache:
                self.dis_cache[isa] = disassemble(
                    self.rom, rs, self.main_end, isa == "thumb", tmpdir,
                    "%s_%s" % (self.name, isa),
                )
            dis = self.dis_cache[isa]

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
            emitted = False
            if text:
                if entry_size <= left:
                    emitted = self.emit_instruction(addr, size, entry_size, text, info)
                    if emitted:
                        addr += entry_size
            if not emitted:
                self.raw_bytes_lines(addr, size)
                addr += size

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
            # `adr`-style operands print as absolute addresses, which gas
            # would re-relativize against the wrong origin; rewrite them.
            m = re.match(r"^(adr(?:\.w)?\s+r\d+,\s*)(.*)$", text)
            if m:
                label = self.resolve_target(info["pcadd"])
                if label is None:
                    return False
                self._append_insn(
                    "\t%s%s\t@ 0x%08X" % (m.group(1), label, info["pcadd"])
                )
                self.stats["instructions"] += 1
                return True
            return False

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
        self._noted_raw = False
        self._extra_placed = set()
        self._required_placed = set()
        self.stats = {
            "instructions": 0,
            "raw_instructions": 0,
            "pool_words": 0,
            "symbolic_words": 0,
            "data_words": 0,
            "named_words": 0,
        }
        self.prescan()

        # gas pads a section's SIZE up to its alignment (2 for Thumb, 4 for
        # ARM) at assembly time.  A code segment with a size that is not a
        # multiple of that alignment would therefore gain padding bytes, so
        # the trailing odd bytes are parked in a separate alignment-1
        # section ".name.tail" that the linker script appends right after
        # the main section (e.g. the `bx lr' of SoundDriverVSyncOff split
        # across the sdk_swi_wrappers/sdk_reset_helper boundary).
        align = 1
        for _vma, _name, isa in self.funcs:
            align = max(align, 4 if isa == "arm" else 2)
        tail_len = (self.end - self.start) % align if align > 1 else 0
        main_end = self.end - tail_len
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
        if self.funcs:
            h.append("@ Functions (docs/analysis/symbols.csv):")
            for vma, name, _isa in self.funcs:
                h.append("@   0x%08X %s" % (vma, name))
        if tail_len:
            h.append(
                "@ Trailing %d byte(s) at 0x%08X live in .%s.tail: gas would"
                " otherwise pad" % (tail_len, main_end, self.name)
            )
            h.append("@ the main section to its instruction alignment and"
                     " shift every later segment.")
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

        if tail_len:
            self.emit_labels_at(main_end)
            self.emit_extra_label(main_end)
            self.emit_required_at(main_end)
            self.lines.append("")
            self.lines.append(
                "@ Odd trailing byte(s) split out to keep the main section's"
                " size a multiple of its alignment (see header note)."
            )
            self.lines.append("\t.section .%s.tail, \"a\"" % self.name)
            tail_label = DATA_PREFIX + "%08x" % main_end
            self.lines.append("\t.global\t%s" % tail_label)
            self.lines.append("%s:" % tail_label)
            addr = main_end
            while addr < self.end:
                left = self.end - addr
                if left >= 2 and addr % 2 == 0:
                    self.lines.append(
                        "\t.short\t0x%04X" % u16(self.rom, vma_off(addr))
                    )
                    addr += 2
                else:
                    self.lines.append(
                        "\t.byte\t0x%02X" % self.rom[vma_off(addr)]
                    )
                    addr += 1

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


def assemble_text(text, opath):
    """Assemble `text` to `opath`; returns (ok, stderr)."""
    spath = opath[:-2] + ".s"
    with open(spath, "w") as f:
        f.write(text)
    proc = subprocess.run(
        [AS, "-mcpu=arm7tdmi", "-o", opath, spath], capture_output=True, text=True
    )
    return proc.returncode == 0, proc.stderr


def verify_group(candidates, syms_obj, rom, tmpdir, helpers=None):
    """candidates: [(section, start, end, objpath)].

    One row PER OBJECT: several chunked files share one section (issue #25)
    and ld concatenates same-named input sections in command-line order, so
    callers must list an address-ordered segment's objects consecutively.
    Links all candidate objects together at their ROM VMAs and compares
    each section's bytes (main + optional .tail, including any alignment
    padding ld had to insert) with baserom.  Linking the whole group is
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
            "  .%s 0x%08X : { *(.%s) *(.%s.tail) }\n" % (sec, start, sec, sec)
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
        # ld folds the optional .name.tail input sections into the .name
        # output section (the patterns are listed in order), so one dump of
        # .name covers the whole segment.
        dump = os.path.join(tmpdir, "dump_%s.bin" % sec)
        try:
            run_checked(
                [OBJCOPY, "--dump-section", ".%s=%s" % (sec, dump), epath],
                "objcopy dump (%s)" % sec,
            )
        except RuntimeError as e:
            print("    %s" % e)
            failing.add(sec)
            continue
        if not os.path.exists(dump):
            print("    %s: section missing from verification ELF" % sec)
            failing.add(sec)
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


# --------------------------------------------------------------------------
# rom_syms.s
# --------------------------------------------------------------------------


def emit_rom_syms(db, exclude, path, data_symbols, abs_symbols=None):
    lines = [
        "@ Auto-generated by tools/split.py - DO NOT EDIT. Regenerate with: make split",
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
        lines.append("@ Named non-ROM cells (tools/split_config.json \"data_symbols\"),")
        lines.append("@ referenced symbolically from split literal pools / data words:")
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

    tmpdir = tempfile.mkdtemp(prefix="split_")
    try:
        entries = []
        for seg_cfg in cfg["segments"]:
            name = seg_cfg["name"]
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
            entries.append((name, start, end, kind, chunk_bytes))

        # Config sanity: every extra label must live inside one configured
        # segment and must not shadow a database symbol defined elsewhere.
        for addr, label in sorted(extra_labels.items()):
            if not any(s <= addr < e for _n, s, e, _k, _c in entries):
                sys.exit(
                    "error: extra label %s at 0x%08X is outside every "
                    "configured segment" % (label, addr)
                )
            if addr in db and db[addr] != label:
                sys.exit(
                    "error: extra label %s at 0x%08X collides with "
                    "database symbol %s" % (label, addr, db[addr])
                )
        for value, label in sorted(data_symbols.items()):
            if value in db.values():
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

        exclude = set(cfg.get("external_defined", [])) | in_split
        syms_path = os.path.join(args.asm_dir, "rom_syms.s")
        count = emit_rom_syms(db, exclude, syms_path, data_symbols, abs_symbols)
        print("wrote %s (%d absolute symbols)" % (syms_path, count))
        syms_obj = os.path.join(tmpdir, "rom_syms.o")
        run_checked(
            [AS, "-mcpu=arm7tdmi", "-o", syms_obj, syms_path], "as (rom_syms.s)"
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
                targets = collect_ref_targets(rom, start, end, funcs_all)
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
                    # earlier run with a different chunk layout.
                    if units[uid]["chunked"] and os.path.isdir(out_dir):
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
    finally:
        if args.keep_tmp:
            print("kept scratch dir: %s" % tmpdir)
        else:
            shutil.rmtree(tmpdir, ignore_errors=True)


if __name__ == "__main__":
    main()
