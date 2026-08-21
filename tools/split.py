#!/usr/bin/env python3
"""ROM range splitter (issue #23).

Converts verbatim `.incbin` data segments into labeled assembler files that
reassemble byte-for-byte, giving every function in the range a symbol so C
migration can proceed incrementally (the classic pret "split" flow).

Driven by three committed inputs:

  * tools/split_config.json    - which segments to split, plus the names that
                                 are already defined as real labels elsewhere
                                 (crt0.s, src/*.c, data/sdk_libc.s)
  * docs/analysis/segments.txt - address ranges / kinds (single source of
                                 truth for segment boundaries)
  * docs/analysis/symbols.csv  - function database from issue #22

For every configured segment the tool writes `asm/<name>.s` and removes the
obsolete `data/<name>.s` incbin slice.  linker.ld needs no edit: it already
pins every section by name, and the generated file re-uses the same section
name, so the object simply takes the place of the incbin blob.

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
    if 0xD000 <= hw <= 0xDEFF:  # b<cond> <label> (0xDFxx is svc)
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


def disassemble(rom, start, end, thumb, tmpdir, tag):
    """Run objdump over rom[start:end]; return {addr: (size, text)}.

    text is the re-assemblable instruction with objdump's `@` comments and
    `<symbol>` suffixes stripped.  Lines look like

        80002d0:\te0822003 \tadd\tr2, r2, r3

    and are split on the tabs: a whitespace-based bytes regex would absorb
    hex-only mnemonics (`add`, `bcc`, ...) into the byte column.
    """
    binpath = os.path.join(tmpdir, tag + ".bin")
    with open(binpath, "wb") as f:
        f.write(rom[vma_off(start):vma_off(end)])
    cmd = [OBJDUMP, "-D", "-b", "binary", "-marm", "--adjust-vma=0x%08x" % start]
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
        out[addr] = (size, text)
    return out


# --------------------------------------------------------------------------
# Segment emission
# --------------------------------------------------------------------------


class SegmentEmitter(object):
    def __init__(self, rom, name, start, end, kind, funcs, db, level):
        self.rom = rom
        self.name = name
        self.start = start
        self.end = end
        self.kind = kind
        self.funcs = funcs  # sorted [(vma, name, isa)]
        self.db = db
        self.level = level  # 0 = real instructions, 1 = raw .short
        self.lines = []
        self.pool_addrs = set()
        self.pending = {}  # addr -> [label, ...]
        self.dis_cache = {}  # isa -> {addr: (size, text)}
        self.cursor = start  # emission frontier (labels behind it are placed)
        self.main_end = end
        self._noted_raw = False
        self.func_map = dict((vma, name) for vma, name, _isa in funcs)
        self.stats = {}

    # ---- label bookkeeping -------------------------------------------

    def file_label(self, addr):
        """Best label for an address inside this file (or None)."""
        if addr in self.func_map:
            return self.func_map[addr]
        if addr == self.start:
            return self.name
        return None

    def resolve_target(self, addr):
        """Label for a branch/pool target, or None if not resolvable."""
        label = self.file_label(addr)
        if label is not None:
            return label
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

    def emit_labels_at(self, addr):
        for label in self.pending.pop(addr, []):
            self.lines.append("%s:" % label)

    # ---- metadata pre-pass --------------------------------------------

    def prescan(self):
        """Decode all function ranges to find literal pools and branch
        targets.

        Runs regardless of emission level so that raw mode can still emit
        pool words as annotated `.word`s.  Branch targets seed the local
        label set up front: some are false positives (data that decodes as
        a branch), but labelling a halfword boundary is harmless and beats
        losing real instructions for the whole segment.
        """
        self.branch_targets = []
        for vma, _name, isa in self.funcs:
            stop = self.end
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
            if self.start < target < self.end and target not in self.func_map:
                label = LOCAL_PREFIX + "%08x" % target
                labels = self.pending.setdefault(target, [])
                if label not in labels:
                    labels.append(label)

    # ---- word annotation ----------------------------------------------

    def word_line(self, addr):
        """Emit line(s) for the 4 bytes at `addr` (a pool or data word)."""
        v = u32(self.rom, vma_off(addr))
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
            left = re - addr
            if (
                addr % 4 == 0
                and left >= 4
                and (addr + 2) not in self.pending
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
            left = re - addr
            off = vma_off(addr)

            # Literal-pool words inside the instruction stream.
            if (
                addr in self.pool_addrs
                and left >= 4
                and addr % 4 == 0
                and (addr - self.start) % 4 == 0
                and (addr + 2) not in self.pending
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
            if size == 4 and (addr + 2) in self.pending:
                self.raw_bytes_lines(addr, 2)
                addr += 2
                continue

            text = dis.get(addr, (0, ""))[1] if pretty else ""
            emitted = False
            if pretty and text and not text.startswith("(bad)"):
                entry_size = dis[addr][0]
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
        parts = text.split(None, 1)
        mnemonic = parts[0]
        operand = parts[1] if len(parts) > 1 else ""

        if info and "branch" in info:
            if decode_size != entry_size:
                return False  # objdump boundary disagrees with our decoder
            label = self.resolve_target(info["branch"])
            if label is None:
                return False
            self.lines.append(
                "\t%s\t%s\t@ 0x%08X" % (mnemonic, label, info["branch"])
            )
            self.stats["instructions"] += 1
            return True

        if info and "pool" in info:
            self.lines.append("\t%s\t@ 0x%08X" % (text, info["pool"]))
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
                self.lines.append(
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

        self.lines.append("\t" + text)
        self.stats["instructions"] += 1
        return True

    # ---- top level -----------------------------------------------------

    def emit(self, tmpdir):
        # Reset per-attempt state (an emitter may be re-run at level 1 after
        # a failed level-0 attempt left pending labels behind).
        self.lines = []
        self.pool_addrs = set()
        self.pending = {}
        self.dis_cache = {}
        self.cursor = self.start
        self._noted_raw = False
        self.stats = {
            "instructions": 0,
            "raw_instructions": 0,
            "pool_words": 0,
            "symbolic_words": 0,
            "data_words": 0,
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
        h.append(
            "@ Segment %s: 0x%08X-0x%08X (%s, 0x%X bytes)"
            % (self.name, self.start, self.end, self.kind, self.end - self.start)
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
        h.append("\t.global\t%s" % self.name)
        if align >= 4 and self.start % 4 == 0:
            h.append("\t.align\t2")
        h.append("\t.syntax\tunified")
        h.append("\t.cpu\tarm7tdmi")
        h.append("%s:" % self.name)
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
        return "\n".join(self.lines) + "\n"


# --------------------------------------------------------------------------
# Verification: assemble every candidate, link the whole group at the real
# ROM VMAs (plus rom_syms.o and --defsym stand-ins for symbols that real
# code defines), and compare each section's bytes against baserom.
# --------------------------------------------------------------------------


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


def verify_group(candidates, syms_obj, defsyms, rom, tmpdir):
    """candidates: [(name, start, end, has_tail, objpath)].

    Links all candidate objects together at their ROM VMAs and compares each
    segment's bytes (main section + optional .tail) with baserom.  Linking
    the whole group is essential: a split segment may reference labels that
    another split segment defines (e.g. the IRQ table pointing into
    game_code_early).  Returns the set of segment names that mismatch.
    """
    script = ["SECTIONS\n{"]
    for name, start, _end, _tail, _obj in candidates:
        script.append(
            "  .%s 0x%08X : { *(.%s) *(.%s.tail) }\n" % (name, start, name, name)
        )
    script.append("}\n")
    lpath = os.path.join(tmpdir, "group.ld")
    with open(lpath, "w") as f:
        f.write("".join(script))
    epath = os.path.join(tmpdir, "group.elf")
    cmd = [LD, "-T", lpath, "-o", epath, syms_obj]
    for name, _s, _e, _t, obj in candidates:
        cmd.append(obj)
    for name, vma in defsyms.items():
        cmd.append("--defsym=%s=0x%08X" % (name, vma))
    try:
        run_checked(cmd, "ld (group verify)")
    except RuntimeError as e:
        print("    group verify failed:\n%s" % e)
        return set(name for name, _s, _e, _t, _o in candidates)
    failing = set()
    for name, start, end, _has_tail, _obj in candidates:
        # ld folds the optional .name.tail input section into the .name
        # output section (the patterns are listed in order), so one dump of
        # .name covers the whole segment.
        dump = os.path.join(tmpdir, "dump_%s.bin" % name)
        try:
            run_checked(
                [OBJCOPY, "--dump-section", ".%s=%s" % (name, dump), epath],
                "objcopy dump (%s)" % name,
            )
        except RuntimeError as e:
            print("    %s" % e)
            failing.add(name)
            continue
        if not os.path.exists(dump):
            print("    %s: section missing from verification ELF" % name)
            failing.add(name)
            continue
        with open(dump, "rb") as f:
            got = f.read()
        want = rom[vma_off(start):vma_off(end)]
        if got != want:
            print(
                "    %s: byte mismatch (got %d bytes, expected %d)"
                % (name, len(got), len(want))
            )
            failing.add(name)
    return failing


# --------------------------------------------------------------------------
# rom_syms.s
# --------------------------------------------------------------------------


def emit_rom_syms(db, exclude, path):
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
    args = parser.parse_args()

    with open(args.rom, "rb") as f:
        rom = f.read()
    segdefs = parse_segments_file(args.segments)
    rows = load_symbol_db(args.symbols)
    db = dict((vma, name) for vma, (name, _isa) in rows.items())
    with open(args.config) as f:
        cfg = json.load(f)

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
            entries.append((name, start, end, kind))

        # Symbols that get real labels: every DB function inside a split
        # range is emitted as a label by its segment file.
        split_ranges = [(s, e) for _n, s, e, _k in entries]
        in_split = set(
            name
            for vma, name in db.items()
            if any(s <= vma < e for s, e in split_ranges)
        )

        exclude = set(cfg.get("external_defined", [])) | in_split
        syms_path = os.path.join(args.asm_dir, "rom_syms.s")
        count = emit_rom_syms(db, exclude, syms_path)
        print("wrote %s (%d absolute symbols)" % (syms_path, count))
        syms_obj = os.path.join(tmpdir, "rom_syms.o")
        run_checked(
            [AS, "-mcpu=arm7tdmi", "-o", syms_obj, syms_path], "as (rom_syms.s)"
        )

        # Verification stand-ins for symbols that real code defines
        # (asm/crt0.s, src/*.c): the real build resolves them from those
        # objects; rom_syms.o does not define them by design.
        vma_by_name = dict((name, vma) for vma, name in db.items())
        defsyms = {}
        for name in cfg.get("external_defined", []):
            if name in vma_by_name:
                defsyms[name] = vma_by_name[name]

        emitters = {}
        for name, start, end, kind in entries:
            funcs = [
                (vma, db[vma], rows[vma][1])
                for vma in sorted(db)
                if start <= vma < end
            ]
            print(
                "splitting %-28s 0x%08X-0x%08X (%s)"
                % (name, start, end, kind)
            )
            em = SegmentEmitter(rom, name, start, end, kind, funcs, db, 0)
            emitters[name] = (em, funcs)

        # Two rounds: prefer real instructions (level 0); any segment that
        # fails to assemble or to match byte-for-byte falls back to raw
        # .short/.byte emission (level 1), which copies the bytes verbatim.
        levels = dict((name, 0) for name, _s, _e, _k in entries)
        results = {}  # name -> (text, objpath, stats)
        for attempt in (0, 1):
            candidates = []
            for name, start, end, _kind in entries:
                if name in results:
                    continue
                em, funcs = emitters[name]
                em.level = levels[name]
                try:
                    text = em.emit(tmpdir)
                except FallbackNeeded as e:
                    print("    %s: level %d unusable: %s" % (name, em.level, e))
                    levels[name] = 1
                    continue
                obj = os.path.join(tmpdir, "%s_%d.o" % (name, em.level))
                ok, err = assemble_text(text, obj)
                if not ok:
                    print(
                        "    %s: level %d does not assemble:\n%s"
                        % (name, em.level, err)
                    )
                    levels[name] = 1
                    continue
                has_tail = ".%s.tail" % name in text
                candidates.append((name, start, end, has_tail, obj))
                results[name] = (text, obj, em.stats, funcs)
            failing = verify_group(
                candidates, syms_obj, defsyms, rom, tmpdir
            )
            for cand in candidates:
                if cand[0] in failing:
                    print("    %s: level %d failed byte verification"
                          % (cand[0], levels[cand[0]]))
                    del results[cand[0]]
                    levels[cand[0]] = 1
        missing = [n for n, _s, _e, _k in entries if n not in results]
        if missing:
            sys.exit(
                "error: segments could not be emitted byte-identically: %s"
                % ", ".join(missing)
            )

        for name, start, end, kind in entries:
            text, _obj, stats, funcs = results[name]
            out_path = os.path.join(args.asm_dir, "%s.s" % name)
            with open(out_path, "w") as f:
                f.write(text)
            blob = os.path.join(args.data_dir, "%s.s" % name)
            if os.path.exists(blob):
                os.remove(blob)
                print("    removed %s (incbin slice replaced)" % blob)
            note = " [RAW FALLBACK]" if levels[name] != 0 else ""
            print(
                "    wrote %s: %d functions, %d insns, %d words (%d symbolic),"
                " %d raw bytes%s"
                % (
                    out_path,
                    len(funcs),
                    stats["instructions"],
                    stats["pool_words"] + stats["symbolic_words"],
                    stats["symbolic_words"],
                    stats["raw_instructions"],
                    note,
                )
            )
    finally:
        shutil.rmtree(tmpdir, ignore_errors=True)


if __name__ == "__main__":
    main()
