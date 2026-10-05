#!/usr/bin/env python3
"""rebuild_assets.py — rebuild a modded ROM from an edited assets/ tree.

The inverse of tools/extract_assets.py (docs/assets.md): `make assets`
decodes the ROM's census-proven graphics objects into the gitignored
assets/ directory, a modder edits them, and this tool splices the edits
back into a copy of baserom.gba, producing a playable knidl-mod.gba.
Nothing under assets/ and no ROM is ever committed (AGENTS.md,
docs/data.md section 1), and the tool never re-extracts — running it
cannot overwrite your edits.

How it finds edits: the extractor is re-run (imported, not copied) into a
temporary directory and every manifest record's file is compared with the
ROM-derived original.  Records whose bytes are equal are *unchanged*:
their ROM ranges are never touched, so an unedited tree rebuilds a ROM
byte-identical to baserom.gba (`--check` proves it and prints the SHA-1
comparison).  Edited records are re-encoded into their ROM format:

    pal-raw /            JASC-PAL colours quantized back to BGR555
    pal-counted          (8 -> 5 bits; >> 3 round-trips the extractor's
                         (v << 3) | (v >> 2) expansion exactly); the
                         counted form re-writes the u16 byte count
    tiles-raw            the .4bpp bytes verbatim
    tiles-chunks         re-framed with the manifest's chunk layout: the
                         chunk count and the 0xFFFF terminator are kept,
                         per-chunk sizes may change — a blob that grew or
                         shrank resizes the last chunk, because chunks
                         land 0x400 apart in VRAM (TaskLoadFrameTiles,
                         src/task_frame_tiles.c), so earlier chunks keep their
                         pages
    lz77                 re-compressed with a BIOS LZ77 (type 0x10)
                         writer; every stream is decoded back and
                         compared before it is spliced, and its size may
                         differ from Nintendo's original
    raw-map /            map bytes verbatim / behind the 6-byte
    bgmap-raw            {size, width, height} header; the dimensions are
                         fixed by the RoomDef, so the file length must
                         not change
    bgmap-lz77           the same header (the u16 flag word at +6 is
                         preserved from the ROM) and the re-compressed
                         stream at the recorded stream address
    oam                  JSON entries packed back to little-endian
                         halfwords; attr0 bit 12 (BuildOam's "last" flag,
                         src/main_build_oam.c) must be set on exactly the
                         final entry

graphics/*.png are rendered views and misc/*.bin (huffman, raw-copy) are
undecoded copies: an edit to either fails with a list of the offending
files and what to edit instead.

Splice policy: knidl-mod.gba starts as the bytes of baserom.gba and each
re-encoded object is written in place over its slot [vma, rom_end).  A
re-encoded object must fit its slot — smaller is fine and zero-padded to
the slot end (every padded object is reported); growing past it fails
with a per-object report, because growth needs the MATCHING=0 rebuild
path on the proven-movable ROM (docs/data.md section 8), which is out of
scope here.  The output's GBA header checksum is fixed with
tools/gbafix.py.

Usage:
  make assets-mod            # python3 tools/rebuild_assets.py
  make assets-mod-check      # pristine round-trip (SHA-1 vs baserom.gba)
  make assets-selftest       # encode every object from the ROM and verify
  python3 tools/rebuild_assets.py --rom baserom.gba --out knidl-mod.gba
  python3 tools/rebuild_assets.py --verbose
"""

import argparse
import hashlib
import json
import os
import struct
import subprocess
import sys
import tempfile

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import extract_assets   # noqa: E402  (the extractor this tool inverts)

ROM_BASE = extract_assets.ROM_BASE

# Rendered views: an edit cannot be re-injected by design (docs/assets.md).
VIEW_ONLY = ("png-map", "png-strip")
# ROM copies this tool has no decoder for; only the formats above are
# editable in this phase.
NOT_REENCODABLE = ("huffman", "raw-copy")

GROWTH_NOTE = ("growing an object past its original slot needs the "
               "MATCHING=0 rebuild path on the proven-movable ROM "
               "(docs/data.md section 8), which is out of scope for this "
               "tool")


class ObjectError(Exception):
    """One object cannot be re-encoded; str() goes into the report."""


def hx(v):
    return "0x%08X" % v


# ---------------------------------------------------------------------------
# BIOS LZ77 (type 0x10) compressor — the writer for the format
# extract_assets.lz77_decode reads.  Greedy longest-match LZSS over the
# 4 KiB window with one step of lazy matching; every stream it produces
# is verified by decoding it back before it is spliced.
# ---------------------------------------------------------------------------

_LZ_MIN, _LZ_MAX, _LZ_WIN = 3, 18, 4096
_LZ_MAX_CHAIN = 128      # hash-chain candidates per position (speed cap)


def lz77_encode(data):
    n = len(data)
    if n == 0:
        raise ObjectError("the decoded blob is empty; a BIOS LZ77 stream "
                          "declares a nonzero size")
    if n > 0xFFFFFF:
        raise ObjectError("the decoded blob (%d bytes) is over the BIOS "
                          "LZ77 u24 size" % n)
    heads = {}            # 3-byte prefix -> ascending positions
    ins = 0               # next position to index

    def insert_upto(p):
        nonlocal ins
        top = min(p, n - _LZ_MIN + 1)
        while ins < top:
            key = bytes(data[ins:ins + _LZ_MIN])
            chain = heads.get(key)
            if chain is None:
                heads[key] = [ins]
            else:
                chain.append(ins)
            ins += 1

    def longest_at(i):
        insert_upto(i)
        if i + _LZ_MIN > n:
            return 0, 0
        chain = heads.get(bytes(data[i:i + _LZ_MIN]))
        if not chain:
            return 0, 0
        maxlen = min(_LZ_MAX, n - i)
        lo = i - _LZ_WIN
        best_len, best_disp = 0, 0
        tries = 0
        for j in reversed(chain):
            if j <= lo or tries >= _LZ_MAX_CHAIN:
                break
            tries += 1
            length = _LZ_MIN
            while length < maxlen and data[j + length] == data[i + length]:
                length += 1
            if length > best_len:
                best_len, best_disp = length, i - j
                if best_len >= maxlen:
                    break
        return best_len, best_disp

    tokens = []           # (False, byte) literal | (True, len, disp) copy
    i = 0
    while i < n:
        length, disp = longest_at(i)
        if length >= _LZ_MIN:
            # lazy matching: emit a literal when the match at i+1 is longer
            if length < _LZ_MAX and i + 1 < n:
                nlen, _ = longest_at(i + 1)
                if nlen > length:
                    tokens.append((False, data[i]))
                    i += 1
                    continue
            tokens.append((True, length, disp))
            i += length
        else:
            tokens.append((False, data[i]))
            i += 1
    insert_upto(n)

    out = bytearray((0x10, n & 0xFF, n >> 8 & 0xFF, n >> 16 & 0xFF))
    for k in range(0, len(tokens), 8):
        group = tokens[k:k + 8]
        flags = 0
        body = bytearray()
        for bit, t in enumerate(group):
            if t[0]:
                flags |= 0x80 >> bit
                body.append((t[1] - _LZ_MIN) << 4 | (t[2] - 1) >> 8)
                body.append((t[2] - 1) & 0xFF)
            else:
                body.append(t[1])
        out.append(flags)
        out += body
    return bytes(out)


def lz77_encode_verified(data):
    """lz77_encode + decode the stream back through the extractor."""
    enc = lz77_encode(data)
    x = extract_assets.lz77_decode(enc, ROM_BASE)
    if x is None or x[0] != data:
        raise ObjectError("internal error: the re-compressed stream does "
                          "not decode back to the file's bytes")
    return enc


# ---------------------------------------------------------------------------
# File-format encoders
# ---------------------------------------------------------------------------

def pal_parse(text):
    """JASC-PAL text -> BGR555 halfwords (channels quantized 8 -> 5 bits)."""
    lines = text.split("\n")
    if lines and lines[-1] == "":
        lines.pop()
    if len(lines) < 3 or lines[0] != "JASC-PAL" or lines[1] != "0100":
        raise ObjectError("not a JASC-PAL file (expected the header "
                          "'JASC-PAL' / '0100' / <count>)")
    try:
        count = int(lines[2])
    except ValueError:
        raise ObjectError("count line %r is not a number" % lines[2])
    if len(lines) != 3 + count:
        raise ObjectError("declares %d colours but the file has %d colour "
                          "lines" % (count, len(lines) - 3))
    words = []
    for line in lines[3:]:
        parts = line.split()
        if len(parts) != 3:
            raise ObjectError("colour line %r is not 'R G B'" % line)
        try:
            r, g, b = (int(p) for p in parts)
        except ValueError:
            raise ObjectError("colour line %r is not three integers" % line)
        if not all(0 <= v <= 255 for v in (r, g, b)):
            raise ObjectError("colour line %r is outside 0..255" % line)
        words.append(r >> 3 | (g >> 3) << 5 | (b >> 3) << 10)
    return words


def oam_parse(text):
    """BuildOam template JSON -> the 8-byte little-endian entries."""
    try:
        obj = json.loads(text)
    except ValueError as e:
        raise ObjectError("invalid JSON (%s)" % e)
    entries = obj.get("entries") if isinstance(obj, dict) else None
    if not isinstance(entries, list) or not entries:
        raise ObjectError("the file has no \"entries\" list")
    rows = []
    for k, e in enumerate(entries):
        if not isinstance(e, dict):
            raise ObjectError("entry %d is not an object" % k)
        vals = []
        for key in ("attr0", "attr1", "attr1_flip", "attr2"):
            if key not in e:
                raise ObjectError("entry %d has no %s" % (k, key))
            v = e[key]
            if isinstance(v, str):
                try:
                    v = int(v, 16)
                except ValueError:
                    raise ObjectError("entry %d: %s %r is not a hex value"
                                      % (k, key, v))
            elif isinstance(v, bool) or not isinstance(v, int):
                raise ObjectError("entry %d: %s is not a number" % (k, key))
            if not 0 <= v <= 0xFFFF:
                raise ObjectError("entry %d: %s 0x%X is outside a u16"
                                  % (k, key, v))
            vals.append(v)
        rows.append(vals)
    for k, vals in enumerate(rows):
        # BuildOam stops at the first entry with attr0 bit 12 set, so the
        # flag must sit on exactly the final entry (src/main_build_oam.c).
        if bool(vals[0] & 0x1000) != (k == len(rows) - 1):
            raise ObjectError(
                "entry %d %s attr0 bit 12 (BuildOam's \"last\" flag); it "
                "must be set on exactly the final entry"
                % (k, "sets" if vals[0] & 0x1000 else "does not set"))
    return b"".join(struct.pack("<4H", *vals) for vals in rows)


def chunks_encode(blob, rec):
    """Re-frame the concatenated blob with the recorded chunk layout.

    The chunk count and the 0xFFFF terminator are kept.  A blob whose
    length changed resizes the last chunk (chunks are copied to VRAM
    pages 0x400 apart, so earlier chunks keep their pages)."""
    layout = rec.get("chunks") or []
    if not layout:
        if blob:
            raise ObjectError("the original stream has no chunks, so there "
                              "is nowhere to place %d bytes" % len(blob))
        return struct.pack("<H", 0xFFFF)
    head = [c["size"] for c in layout[:-1]]
    skeleton = sum(head)
    if len(blob) < skeleton:
        raise ObjectError("the edited blob (%d bytes) is shorter than the "
                          "%d bytes held by the first %d chunks; the chunk "
                          "count is fixed" % (len(blob), skeleton, len(head)))
    last = len(blob) - skeleton
    if last > 0xFFFF:
        raise ObjectError("the last chunk would be %d bytes, over the u16 "
                          "chunk size" % last)
    out = bytearray()
    off = 0
    for s in head + [last]:
        out += struct.pack("<H", s) + blob[off:off + s]
        off += s
    out += struct.pack("<H", 0xFFFF)
    return bytes(out)


def decode_text(data, rel):
    try:
        return data.decode("ascii")
    except UnicodeDecodeError as e:
        raise ObjectError("not an ASCII text file (%s)" % e)


# ---------------------------------------------------------------------------
# Re-encode one manifest record
# ---------------------------------------------------------------------------

def reencode(rec, data, rom):
    """The payload for the record's slot [vma, rom_end).

    Raises ObjectError when the edit cannot be expressed in the record's
    ROM format.  The payload may be shorter than the slot (the splicer
    zero-pads); it may never be longer (the caller reports that)."""
    fmt = rec.get("format") or rec["kind"]     # lz77 records carry no format
    if fmt == "pal-raw":
        words = pal_parse(decode_text(data, rec["file"]))
        return struct.pack("<%dH" % len(words), *words)
    if fmt == "pal-counted":
        words = pal_parse(decode_text(data, rec["file"]))
        if len(words) * 2 > 0xFFFF:
            raise ObjectError("%d colours are over the u16 byte count"
                              % len(words))
        return (struct.pack("<H", len(words) * 2)
                + struct.pack("<%dH" % len(words), *words))
    if fmt == "tiles-raw":
        return bytes(data)
    if fmt == "tiles-chunks":
        return chunks_encode(data, rec)
    if fmt == "lz77":
        return lz77_encode_verified(data)
    if fmt == "raw-map":
        slot = rec["rom_end"] - rec["vma"]
        if len(data) != slot:
            raise ObjectError("the RoomDef fixes this map at %d bytes; the "
                              "edited file has %d (a size change needs the "
                              "RoomDef/C path, docs/data.md section 7)"
                              % (slot, len(data)))
        return bytes(data)
    if fmt == "bgmap-raw":
        h = rec["header"]
        need = h["width"] * h["height"] * 2
        if len(data) != need:
            raise ObjectError("the map's %dx%d dimensions (fixed by the "
                              "RoomDef) need exactly %d bytes; the edited "
                              "file has %d"
                              % (h["width"], h["height"], need, len(data)))
        return struct.pack("<3H", len(data), h["width"], h["height"]) + data
    if fmt == "bgmap-lz77":
        h = rec["header"]
        if len(data) > 0xFFFF:
            raise ObjectError("the decoded map (%d bytes) is over the u16 "
                              "size field" % len(data))
        # struct BgMap {size, width, height, flag}: the flag word is not
        # part of the extracted .bin, so it is preserved from the ROM
        flag = struct.unpack_from("<H", rom, rec["vma"] + 6 - ROM_BASE)[0]
        stream = lz77_encode_verified(data)
        return (struct.pack("<4H", len(data), h["width"], h["height"], flag)
                + stream)
    if fmt == "oam":
        return oam_parse(decode_text(data, rec["file"]))
    if fmt in VIEW_ONLY:
        raise ObjectError("a rendered view; edit the underlying palettes/, "
                          "tiles/ or maps/ file instead (docs/assets.md)")
    if fmt in NOT_REENCODABLE:
        raise ObjectError("format %r has no decoder in this phase; only "
                          "the editable formats of docs/assets.md can be "
                          "re-injected" % fmt)
    raise ObjectError("unhandled format %r" % fmt)


# ---------------------------------------------------------------------------
# The rebuilder
# ---------------------------------------------------------------------------

def load_inputs(args):
    with open(args.rom, "rb") as f:
        rom = f.read()
    with open(args.config) as f:
        cfg = json.load(f)
    segs = []
    with open(args.segments) as f:
        for line in f:
            p = line.split("#")[0].split()
            if len(p) >= 4:
                segs.append((int(p[0], 16), int(p[1], 16), p[2], p[3]))
    return rom, cfg, segs


def fresh_tree(rom, cfg, segs, verbose):
    """The ROM-derived assets tree, via the extractor itself (imported,
    never copied): its bytes are the pristine original of every record."""
    with tempfile.TemporaryDirectory() as tmp:
        ex = extract_assets.Extractor(rom, cfg, segs, tmp, verbose)
        ex.run()
        return extract_assets.tree_files(tmp)


def gbafix(path):
    tool = os.path.join(os.path.dirname(os.path.abspath(__file__)),
                        "gbafix.py")
    try:
        subprocess.run([sys.executable, tool, path], check=True)
    except subprocess.CalledProcessError as e:
        raise SystemExit("ERROR: tools/gbafix.py failed on %s (%s)"
                         % (path, e))


def compare_tree(disk, fresh, rom_name):
    """(structural problems, edited files' relative paths)."""
    problems, edited = [], []
    if disk.get("manifest.json") != fresh.get("manifest.json"):
        problems.append("manifest.json: does not match a fresh extraction "
                        "from %s (the tree is stale; re-run `make assets`, "
                        "which overwrites edits)" % rom_name)
    for rel in sorted(set(fresh) | set(disk)):
        if rel == "manifest.json":
            continue
        if rel not in disk:
            problems.append("%s: missing from the assets tree" % rel)
        elif rel not in fresh:
            problems.append("%s: not derived from the ROM (stale or "
                            "hand-added)" % rel)
        elif disk[rel] != fresh[rel]:
            edited.append(rel)
    return problems, edited


def print_problems(lines, limit=20):
    for line in lines[:limit]:
        print("  " + line)
    if len(lines) > limit:
        print("  ... and %d more" % (len(lines) - limit))


def splice(rom, records, payloads):
    """(ROM bytes with every payload written in place, notes)."""
    out = bytearray(rom)
    notes = []
    writes = sorted((rec["vma"], rec["rom_end"], payloads[rec["file"]], rec)
                    for rec in records if rec["file"] in payloads)
    prev_end, prev_rel = 0xC0, None     # the cartridge header is never touched
    for vma, end, payload, rec in writes:
        if vma < prev_end:
            raise SystemExit("ERROR: internal: %s overlaps the previous "
                             "object %s" % (rec["file"], prev_rel))
        off = vma - ROM_BASE
        if off < 0xC0 or end - ROM_BASE > len(rom):
            raise SystemExit("ERROR: internal: %s has a slot outside the "
                             "ROM" % rec["file"])
        out[off:off + len(payload)] = payload
        pad = (end - vma) - len(payload)
        if pad:
            out[off + len(payload):off + (end - vma)] = b"\x00" * pad
            notes.append("%s: zero-padded %d bytes to the slot end "
                         "(%d -> %d bytes)"
                         % (rec["file"], pad, len(payload), end - vma))
        prev_end, prev_rel = end, rec["file"]
    return bytes(out), notes


def fmt_of(rec):
    return rec.get("format") or rec["kind"]


def run_build(args, rom, cfg, segs):
    if not os.path.exists(os.path.join(args.assets, "manifest.json")):
        print("ERROR: %s has no manifest.json; run `make assets` first"
              % args.assets, file=sys.stderr)
        return 1
    fresh = fresh_tree(rom, cfg, segs, args.verbose)
    disk = extract_assets.tree_files(args.assets)
    problems, edited = compare_tree(disk, fresh, args.rom)
    if problems:
        print("assets-mod: FAILED (%d structural problems)" % len(problems))
        print_problems(problems)
        return 1
    manifest = json.loads(fresh["manifest.json"])
    records = manifest["records"]
    by_file = {r["file"]: r for r in records}

    failures, payloads = [], {}
    for rel in edited:
        rec = by_file[rel]
        try:
            payload = reencode(rec, disk[rel], rom)
        except ObjectError as e:
            failures.append("%s: %s" % (rel, e))
            continue
        slot = rec["rom_end"] - rec["vma"]
        if len(payload) > slot:
            failures.append("%s: re-encodes to %d bytes, over the %d-byte "
                            "slot at %s (+%d bytes); %s"
                            % (rel, len(payload), slot, hx(rec["vma"]),
                               len(payload) - slot, GROWTH_NOTE))
            continue
        payloads[rel] = payload
        if args.verbose:
            print("  edit %-44s %-13s %s..%s  %d -> %d bytes"
                  % (rel, fmt_of(rec), hx(rec["vma"]), hx(rec["rom_end"]),
                     slot, len(payload)))
    if failures:
        print("assets-mod: FAILED (%d objects cannot be re-injected)"
              % len(failures))
        print_problems(failures)
        return 1

    out, splice_notes = splice(rom, records, payloads)
    with open(args.out, "wb") as f:
        f.write(out)
    gbafix(args.out)
    sha_base = hashlib.sha1(rom).hexdigest()
    sha_out = hashlib.sha1(open(args.out, "rb").read()).hexdigest()
    for note in splice_notes:
        print("note: " + note)
    if edited:
        by_fmt = {}
        for rel in edited:
            if rel in payloads:
                by_fmt[fmt_of(by_file[rel])] = \
                    by_fmt.get(fmt_of(by_file[rel]), 0) + 1
        print("rebuilt %s: %d objects unchanged, %d re-encoded and spliced "
              "(%s)" % (args.out, len(records) - len(payloads),
                        len(payloads),
                        ", ".join("%s=%d" % kv for kv in sorted(by_fmt.items()))))
    else:
        print("rebuilt %s: no edits found — all %d objects unchanged, the "
              "output is byte-identical to %s" % (args.out, len(records),
                                                  args.rom))
    print("  sha1 %s: %s" % (args.rom, sha_base))
    print("  sha1 %s: %s" % (args.out, sha_out))
    print("policy: nothing under %s/ is committed; knidl-mod.gba is "
          "gitignored by design" % args.assets)
    return 0


def run_check(args, rom, cfg, segs):
    if not os.path.exists(os.path.join(args.assets, "manifest.json")):
        print("ERROR: %s has no manifest.json; run `make assets` first"
              % args.assets, file=sys.stderr)
        return 1
    fresh = fresh_tree(rom, cfg, segs, args.verbose)
    disk = extract_assets.tree_files(args.assets)
    problems, edited = compare_tree(disk, fresh, args.rom)
    if problems or edited:
        n = len(problems) + len(edited)
        print("assets-mod-check: FAILED (the tree is not pristine: %d "
              "problem(s); an unedited tree is required)" % n)
        print_problems(problems + ["%s: edited" % rel for rel in edited])
        return 1
    manifest = json.loads(fresh["manifest.json"])
    records = manifest["records"]
    out, _ = splice(rom, records, {})      # no edits -> no writes at all
    sha_base = hashlib.sha1(rom).hexdigest()
    with tempfile.TemporaryDirectory() as tmp:
        path = os.path.join(tmp, "pristine-rebuild.gba")
        with open(path, "wb") as f:
            f.write(out)
        gbafix(path)
        sha_out = hashlib.sha1(open(path, "rb").read()).hexdigest()
    ok = sha_base == sha_out
    print("assets-mod-check: %s (pristine round-trip over %d objects)"
          % ("OK" if ok else "FAILED", len(records)))
    print("  sha1 %s: %s" % (args.rom, sha_base))
    print("  sha1 rebuilt ROM: %s" % sha_out)
    return 0 if ok else 1


def run_self_test(args, rom, cfg, segs):
    """Encode every re-encodable object from the ROM and verify it.

    For the verbatim formats the re-encoded pristine file must reproduce
    the ROM slot byte for byte; for the LZ77 forms the stream must decode
    back (through extract_assets.lz77_decode) to exactly the file's
    bytes.  This exercises every encoder on the whole corpus without
    needing an assets/ tree."""
    fresh = fresh_tree(rom, cfg, segs, args.verbose)
    manifest = json.loads(fresh["manifest.json"])
    records = manifest["records"]
    failures, byte_exact, round_trip, skipped = [], 0, 0, 0
    for rec in records:
        fmt = fmt_of(rec)
        if fmt in VIEW_ONLY or fmt in NOT_REENCODABLE:
            skipped += 1
            continue
        data = fresh[rec["file"]]
        try:
            payload = reencode(rec, data, rom)
        except ObjectError as e:
            failures.append("%s: %s" % (rec["file"], e))
            continue
        off = rec["vma"] - ROM_BASE
        slot = rom[off:off + (rec["rom_end"] - rec["vma"])]
        if fmt == "lz77":
            round_trip += 1
        elif fmt == "bgmap-lz77":
            if payload[:8] != slot[:8]:
                failures.append("%s: the rebuilt BgMap header differs from "
                                "the ROM's" % rec["file"])
                continue
            round_trip += 1
        else:
            if payload != slot:
                failures.append("%s: re-encoding the pristine file does "
                                "not reproduce the ROM bytes" % rec["file"])
                continue
            byte_exact += 1
    if failures:
        print("assets self-test: FAILED (%d objects)" % len(failures))
        print_problems(failures)
        return 1
    print("assets self-test: OK (%d objects: %d re-encode byte-exact, %d "
          "LZ77 streams decode back; %d views/undecoded skipped)"
          % (len(records), byte_exact, round_trip, skipped))
    return 0


def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--rom", default="baserom.gba")
    ap.add_argument("--config", default="tools/split_config.json")
    ap.add_argument("--segments", default="docs/analysis/segments.txt")
    ap.add_argument("--assets", default="assets")
    ap.add_argument("--out", default="knidl-mod.gba")
    ap.add_argument("--check", action="store_true",
                    help="pristine round-trip: an unedited assets/ tree "
                         "must rebuild a ROM byte-identical to the "
                         "baserom (prints the SHA-1 comparison)")
    ap.add_argument("--self-test", action="store_true",
                    help="encode every object from the ROM and verify it "
                         "(no assets/ tree needed)")
    ap.add_argument("--verbose", action="store_true")
    args = ap.parse_args()

    rom, cfg, segs = load_inputs(args)
    if args.self_test:
        return run_self_test(args, rom, cfg, segs)
    if args.check:
        return run_check(args, rom, cfg, segs)
    return run_build(args, rom, cfg, segs)


if __name__ == "__main__":
    sys.exit(main())
