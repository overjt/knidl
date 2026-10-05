#!/usr/bin/env python3
"""extract_assets.py — decode the ROM's graphics assets into editable files.

The data policy (AGENTS.md, docs/data.md section 1) never commits assets in
any form: they stay `.incbin` slices of the user's own `baserom.gba`.  This
tool is the sanctioned "become editable" path: it extracts them into the
gitignored `assets/` directory at run time (the tmc/mzm model), where a
modder can view and edit them locally.  Nothing under `assets/` is ever
committed, and the ROM build itself keeps reading the original bytes.

What gets extracted is not guessed: every object below was proven by the
pointer census (tools/ptrcensus.py providers, issue #36 phase 2), whose
claims carry the consumer that reads the bytes.  The providers are reused
directly, so the extractor's boundaries and formats are exactly the
census-proven ones:

  families (each a source of objects, with the consumer evidence kept):
    rooms    census_rooms.py   — every RoomDef's maps, palettes and tiles
    frames   census_sprites.py — the sprite frame network (TaskGfx tile
                                 streams, counted palettes, OAM templates)
    sheets   census_sheets.py  — what the frame network leaves in the two
                                 sprite-sheet zones (LZ77 sources, sized
                                 palettes, raw sheet headers, chained OAM)
    pictures gUnk_087319C8 rows: palette + LZ77 tiles + LZ77 map each,
             the map rendered to a PNG (mode_gfx_loaders.c sub_08008d98)
    stage    gUnk_08731F78 GfxHeaders: palette + LZ77 tiles, rendered to
             tile strips (cutscene_main.c sub_080102c0)
    headers  the four raw-tiles sheet headers the code reads
    misclz   gUnk_08731980, TransferNode mode 8 sources, direct LZ77 calls
             and the consumer-sized palette/tile blocks (census_sheets.SIZED)

  output formats (all derived from the ROM, all re-derivable):
    palettes/*.pal       JASC-PAL text; raw BGR555 or byteCount-prefixed
    tiles/*.4bpp         raw 4bpp tile bytes (one file per proven blob)
    tiles/*.chunks.4bpp  a TaskGfx {u16 size; data}.. 0xFFFF stream,
                         chunks concatenated (layout kept in the manifest)
    lz77/*.bin           the decoded content of a BIOS LZ77 stream
    maps/*.bin           decoded picture-screen tilemaps (u16 entries)
    oam/*.json           BuildOam template streams as structured entries
    graphics/*.png       rendered views (picture maps, tile strips) as
                         indexed PNGs whose PLTE is the object's palette
    manifest.json        every file with its ROM range, format, evidence

`--check` re-extracts into a temporary directory and compares the two
trees byte for byte (the extraction is deterministic), then fails closed
on any missing, extra or differing file — the same contract `make
compare` gives the built ROM.  It also fails when the manifest regenerates
differently, which catches split_config/segments drift.  Hand-edited files
therefore make `--check` fail by design: it is a fidelity check, not a
linter.  Re-injecting edited assets into a ROM is out of scope for this
phase and listed in docs/assets.md as the next step.

Usage:
  python3 tools/extract_assets.py                # extract into assets/
  python3 tools/extract_assets.py --check        # verify assets/ vs ROM
  python3 tools/extract_assets.py --verbose
"""

import argparse
import json
import os
import struct
import sys
import tempfile
import zlib

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import census_rooms    # noqa: E402  (provider + LZ77 validation)
import census_sheets   # noqa: E402  (provider + consumer tables)
import census_sprites  # noqa: E402  (provider + format lengths)

ROM_BASE = 0x08000000


def u16(rom, a):
    return struct.unpack_from("<H", rom, a - ROM_BASE)[0]


def u32(rom, a):
    return struct.unpack_from("<I", rom, a - ROM_BASE)[0]


# ---------------------------------------------------------------------------
# BIOS LZ77 (type 0x10), decoded to bytes.  Token rules follow
# census_rooms.lz77 (the validator): a back-reference may overshoot the
# declared size (the BIOS copies it whole); the decoded content is the
# first `size` bytes.  Byte-at-a-time copy so overlapping references
# reproduce the BIOS behaviour.
# ---------------------------------------------------------------------------

def lz77_decode(rom, a):
    """(decoded bytes, end VMA of the stream) or None when invalid."""
    off = a - ROM_BASE
    if off < 0 or off + 4 > len(rom) or rom[off] != 0x10:
        return None
    size = rom[off + 1] | rom[off + 2] << 8 | rom[off + 3] << 16
    if size == 0:
        return None
    out = bytearray()
    p = off + 4
    while len(out) < size:
        if p >= len(rom):
            return None
        flags = rom[p]
        p += 1
        for bit in range(8):
            if len(out) >= size:
                break
            if flags & (0x80 >> bit):
                if p + 1 >= len(rom):
                    return None
                b0, b1 = rom[p], rom[p + 1]
                p += 2
                disp = ((b0 & 0xF) << 8 | b1) + 1
                if disp > len(out):
                    return None
                for _ in range((b0 >> 4) + 3):
                    if len(out) >= size:
                        break
                    out.append(out[len(out) - disp])
            else:
                if p >= len(rom):
                    return None
                out.append(rom[p])
                p += 1
    # the stream ends where the last token's bytes end (overshoot included)
    return bytes(out), ROM_BASE + p


# ---------------------------------------------------------------------------
# Indexed PNG writer (stdlib only): 8-bit colour type 3, filter 0 rows.
# ---------------------------------------------------------------------------

def _png_chunk(tag, data):
    return (struct.pack(">I", len(data)) + tag + data
            + struct.pack(">I", zlib.crc32(tag + data) & 0xFFFFFFFF))


def png_write_indexed(path, width, indices, palette):
    """width px per row; indices row-major; palette [(r, g, b), ...]."""
    height = len(indices) // width
    assert len(indices) == width * height
    assert all(0 <= i < len(palette) for i in indices)
    raw = bytearray()
    for y in range(height):
        raw.append(0)  # filter type 0 (None)
        raw += bytes(indices[y * width:(y + 1) * width])
    ihdr = struct.pack(">IIBBBBB", width, height, 8, 3, 0, 0, 0)
    plte = bytearray()
    for r, g, b in palette:
        plte += bytes((r, g, b))
    with open(path, "wb") as f:
        f.write(b"\x89PNG\r\n\x1a\n")
        f.write(_png_chunk(b"IHDR", ihdr))
        f.write(_png_chunk(b"PLTE", bytes(plte)))
        f.write(_png_chunk(b"IDAT", zlib.compress(bytes(raw), 9)))
        f.write(_png_chunk(b"IEND", b""))


# ---------------------------------------------------------------------------
# Palettes: BGR555 in the ROM, expanded to 8-bit with (v << 3) | (v >> 2).
# ---------------------------------------------------------------------------

def bgr555_to_rgb(word):
    r, g, b = word & 31, (word >> 5) & 31, (word >> 10) & 31
    e = lambda v: (v << 3) | (v >> 2)
    return e(r), e(g), e(b)


def pal_write(path, words):
    lines = ["JASC-PAL", "0100", str(len(words))]
    lines += ["%d %d %d" % bgr555_to_rgb(w) for w in words]
    with open(path, "w", encoding="ascii") as f:
        f.write("\n".join(lines) + "\n")


def pal_words(rom, vma, nbytes):
    """The BGR555 words of an nbytes-long palette blob at vma."""
    return [struct.unpack_from("<H", rom, vma - ROM_BASE + 2 * i)[0]
            for i in range(nbytes // 2)]


# ---------------------------------------------------------------------------
# 4bpp tile rendering
# ---------------------------------------------------------------------------

def unpack_4bpp(blob):
    """One index per pixel, two pixels per byte, low nibble first."""
    out = []
    for byte in blob:
        out.append(byte & 0xF)
        out.append(byte >> 4)
    return out


def tile_pixels(indices, base, hflip, vflip):
    """The 64 pixel indices of one 8x8 tile, read out row-major."""
    rows = [indices[base + y * 8:base + y * 8 + 8] for y in range(8)]
    if hflip:
        rows = [r[::-1] for r in rows]
    if vflip:
        rows = rows[::-1]
    return [px for row in rows for px in row]


def render_strip(tile_blob, tiles_per_row=16):
    """Pixel indices + (w, h) of a raw 4bpp blob as a tile strip."""
    if len(tile_blob) % 32:
        return None
    ntiles = len(tile_blob) // 32
    px = unpack_4bpp(tile_blob)
    rows = (ntiles + tiles_per_row - 1) // tiles_per_row
    width = min(ntiles, tiles_per_row) * 8
    out = [0] * (rows * 8 * width)
    for t in range(ntiles):
        tx, ty = (t % tiles_per_row) * 8, (t // tiles_per_row) * 8
        cell = tile_pixels(px, t * 64, False, False)
        for y in range(8):
            for x in range(8):
                out[(ty + y) * width + tx + x] = cell[y * 8 + x]
    return out, (width, rows * 8)


def render_map(map_words, tile_blob, palette_len):
    """(indices, (w, h)) of a tiled image from u16 map entries
    (tile | hflip<<10 | vflip<<11 | palette bank<<12).  Accepts 32x32-cell
    pages (stacked vertically) or a square map; None when it does not fit.
    A map may reference tiles that already sit in VRAM (fonts, HUD) rather
    than in the local blob; those cells render as colour 0."""
    if len(tile_blob) % 32:
        return None
    if len(map_words) % 1024 == 0:
        side = 32
    elif int(len(map_words) ** 0.5) ** 2 == len(map_words):
        side = int(len(map_words) ** 0.5)
    else:
        return None
    px = unpack_4bpp(tile_blob)
    ntiles = len(px) // 64
    width = side * 8
    pages = len(map_words) // (side * side)
    out = [0] * (pages * side * 8 * width)
    for p in range(pages):
        for e in range(side * side):
            m = map_words[p * side * side + e]
            tx, ty = (e % side) * 8, (e // side) * 8 + p * side * 8
            bank = m >> 12
            if (m & 0x3FF) < ntiles:
                cell = tile_pixels(px, (m & 0x3FF) * 64, m & 0x400, m & 0x800)
            else:
                cell = [0] * 64  # a tile already in VRAM, not in this blob
            for y in range(8):
                for x in range(8):
                    out[(ty + y) * width + tx + x] = min(
                        bank * 16 + cell[y * 8 + x], palette_len - 1)
    return out, (width, pages * side * 8)


# ---------------------------------------------------------------------------
# The extractor
# ---------------------------------------------------------------------------

class Extractor(object):
    def __init__(self, rom, cfg, segs, out_dir, verbose=False):
        self.rom, self.cfg, self.segs, self.verbose = rom, cfg, segs, verbose
        self.out = out_dir
        self.names = {}
        for key in ("data_symbols", "extra_labels"):
            for k, v in cfg.get(key, {}).items():
                self.names[int(k, 16)] = v
        self.sorted_labels = sorted(self.names)
        self.used_names = set()
        self.records = []
        self.claimed = {}
        self.skipped = 0

    def log(self, msg):
        if self.verbose:
            print(msg)

    def name(self, vma):
        base = self.names.get(vma) or "unk_%08x" % vma
        name, n = base, 1
        while name in self.used_names:
            n += 1
            name = "%s_%d" % (base, n)
        self.used_names.add(name)
        return name

    def next_label(self, a):
        import bisect
        i = bisect.bisect_right(self.sorted_labels, a)
        return self.sorted_labels[i] if i < len(self.sorted_labels) else None

    # -- record emitters ----------------------------------------------------

    def _write(self, rel, data):
        path = os.path.join(self.out, rel)
        os.makedirs(os.path.dirname(path), exist_ok=True)
        if not (os.path.exists(path) and open(path, "rb").read() == data):
            with open(path, "wb") as f:
                f.write(data)
        return rel

    def _write_text(self, rel, text):
        self._write(rel, text.encode("ascii"))

    def add_palette(self, vma, end, counted, evidence, family):
        if counted:
            bc = u16(self.rom, vma)
            if 2 + bc != end - vma:
                raise ValueError("palette 0x%08X: byteCount %d does not fit its "
                                 "span" % (vma, bc))
            words = pal_words(self.rom, vma + 2, bc)
        else:
            words = pal_words(self.rom, vma, end - vma)
        rel = "palettes/%s.pal" % self.name(vma)
        lines = ["JASC-PAL", "0100", str(len(words))]
        lines += ["%d %d %d" % bgr555_to_rgb(w) for w in words]
        self._write_text(rel, "\n".join(lines) + "\n")
        self.records.append({
            "file": rel, "family": family, "kind": "palette", "vma": vma,
            "rom_end": end, "format": "pal-counted" if counted else "pal-raw",
            "evidence": evidence})
        return rel

    def add_tiles(self, vma, end, chunks, evidence, family):
        rom = self.rom
        if chunks:
            blob, p, layout = bytearray(), vma, []
            while True:
                n = u16(rom, p)
                if n == 0xFFFF:
                    break
                layout.append({"offset": p - vma, "size": n})
                blob += rom[p + 2 - ROM_BASE:p + 2 + n - ROM_BASE]
                p += 2 + n
            if p + 2 != end:
                raise ValueError("chunk stream 0x%08X: ends at 0x%08X, proven end "
                                 "0x%08X" % (vma, p + 2, end))
            fmt, ext = "tiles-chunks", "chunks.4bpp"
            blob = bytes(blob)
        else:
            blob = rom[vma - ROM_BASE:end - ROM_BASE]
            fmt, ext, layout = "tiles-raw", "4bpp", None
        rel = self._write("tiles/%s.%s" % (self.name(vma), ext), blob)
        rec = {"file": rel, "family": family, "kind": "tiles", "vma": vma,
               "rom_end": end, "format": fmt, "evidence": evidence}
        if layout is not None:
            rec["chunks"] = layout
        self.records.append(rec)
        return blob, rel

    def add_lz77(self, vma, evidence, family, subdir="lz77"):
        x = lz77_decode(self.rom, vma)
        if x is None:
            raise ValueError("LZ77 stream at 0x%08X does not decode" % vma)
        blob, end = x
        rel = self._write("%s/%s.bin" % (subdir, self.name(vma)), blob)
        self.records.append({
            "file": rel, "family": family, "kind": "lz77", "vma": vma,
            "rom_end": end, "decoded_size": len(blob), "evidence": evidence})
        return blob, end, rel

    def add_oam(self, vma, end, evidence, family):
        rom = self.rom
        entries = [{"attr0": "0x%04X" % u16(rom, a),
                    "attr1": "0x%04X" % u16(rom, a + 2),
                    "attr1_flip": "0x%04X" % u16(rom, a + 4),
                    "attr2": "0x%04X" % u16(rom, a + 6)}
                   for a in range(vma, end, 8)]
        obj = {"format": "BuildOam template stream (src/main_build_oam.c): 8-byte "
                         "entries up to and including the one whose attr0 has "
                         "bit 12 set",
               "evidence": evidence, "entries": entries}
        rel = "oam/%s.json" % self.name(vma)
        self._write_text(rel, json.dumps(obj, indent=1) + "\n")
        self.records.append({
            "file": rel, "family": family, "kind": "oam", "vma": vma,
            "rom_end": end, "format": "oam", "evidence": evidence})

    def add_png(self, rel, indices, size, palette, vma, fmt, evidence, family):
        w, h = size
        path = os.path.join(self.out, rel)
        os.makedirs(os.path.dirname(path), exist_ok=True)
        png_write_indexed(path, w, indices, palette)
        self.records.append({
            "file": rel, "family": family, "kind": "png", "vma": vma,
            "rom_end": None, "format": fmt, "width": w, "height": h,
            "evidence": evidence})

    # -- family walkers ------------------------------------------------------

    def _claim(self, vma, family):
        """True when vma is free for this family to extract."""
        if vma in self.claimed:
            self.log("  skip 0x%08X: already extracted by family %s"
                     % (vma, self.claimed[vma]))
            return False
        self.claimed[vma] = family
        return True

    # Census kinds that are structural tables, not extractable graphics:
    # RoomDefs, door records, room object lists, BG animation scripts, sine
    # tables and the value tables.  The functional records among them are C
    # since #167 (docs/data.md 5.2); doors and object lists are level layouts
    # no view decodes yet (docs/assets.md).  The extractor only counts them.
    SKIPPED_KINDS = frozenset(("objects", "doors"))
    SKIPPED_SUFFIXES = ("-values", "-value", "-table", "-cmd")

    def walk_census(self, family, provider):
        """One artifact per coincidence claim of a census provider."""
        got = provider(self.rom, self.cfg, self.segs)
        for s, e, kind, why in sorted(got["coincidence"]):
            if kind in self.SKIPPED_KINDS or kind.endswith(self.SKIPPED_SUFFIXES):
                self.skipped += 1
                continue
            if not self._claim(s, family):
                continue
            if kind == "palette":
                counted = (("byteCount" in why or "byteSize" in why)
                           and 2 + u16(self.rom, s) == e - s)
                self.add_palette(s, e, counted, why, family)
            elif kind == "raw-tiles":
                self.add_tiles(s, e, False, why, family)
            elif kind == "tiles":
                self.add_tiles(s, e, True, why, family)
            elif kind == "lz77":
                self.add_lz77(s, why, family)
            elif kind == "oam":
                self.add_oam(s, e, why, family)
            elif kind in ("raw-map", "bgmap-raw"):
                if kind == "bgmap-raw":
                    # RoomDef.bg3Map: 6-byte header {u16 size, w, h} then the
                    # raw u16 map at +6 (DrawBg3Tile, census_rooms.py)
                    hdr = {"size": u16(self.rom, s), "width": u16(self.rom, s + 2),
                           "height": u16(self.rom, s + 4)}
                    blob = self.rom[s + 6 - ROM_BASE:e - ROM_BASE]
                else:
                    hdr, blob = None, self.rom[s - ROM_BASE:e - ROM_BASE]
                rel = self._write("maps/%s.bin" % self.name(s), blob)
                rec = {"file": rel, "family": family, "kind": "map", "vma": s,
                       "rom_end": e, "format": kind, "evidence": why}
                if hdr is not None:
                    rec["header"] = hdr
                self.records.append(rec)
            elif kind == "bgmap-lz77":
                # RoomDef.bg3Map: 6-byte header {u16 size, w, h}, stream at +8
                # (LoadBg3Map, census_rooms.py)
                hdr = {"size": u16(self.rom, s), "width": u16(self.rom, s + 2),
                       "height": u16(self.rom, s + 4)}
                x = lz77_decode(self.rom, s + 8)
                if x is None or len(x[0]) != hdr["size"]:
                    raise ValueError("bg3 map 0x%08X: the stream at +8 does not "
                                     "decode to its declared size" % s)
                rel = self._write("maps/%s.bin" % self.name(s), x[0])
                self.records.append({
                    "file": rel, "family": family, "kind": "map", "vma": s,
                    "rom_end": x[1], "format": kind, "header": hdr,
                    "stream_vma": s + 8, "evidence": why})
            elif kind in ("huffman", "raw-copy"):
                # a compressed stream this tool does not decode (the Huffman
                # screen loaders) or a plain consumer-copied block: keep the
                # ROM bytes so the tree still covers them
                blob = self.rom[s - ROM_BASE:e - ROM_BASE]
                rel = self._write("misc/%s.bin" % self.name(s), blob)
                self.records.append({
                    "file": rel, "family": family, "kind": kind, "vma": s,
                    "rom_end": e, "format": kind, "evidence": why})
            else:
                raise ValueError("family %s: unhandled census kind %r at 0x%08X"
                                 % (family, kind, s))

    def walk_pictures(self):
        """gUnk_087319C8 rows {palette, LZ77 tiles, LZ77 map}, map to PNG."""
        rom = self.rom
        t = census_sheets.PICTURE_TABLE
        end = self.next_label(t)
        for i, a in enumerate(range(t, end - 11, 12)):
            pal, til, m = u32(rom, a), u32(rom, a + 4), u32(rom, a + 8)
            if not pal or not til:
                continue
            ev = "gUnk_087319C8[%d], sub_08008d98 src/mode_gfx_loaders.c:109-113" % i
            if self._claim(pal, "pictures"):
                self.add_palette(pal, pal + 64, False,
                                 ev + " (BG palette, RequestCopy 64 bytes)",
                                 "pictures")
            til_blob = None
            if self._claim(til, "pictures"):
                til_blob, _, _ = self.add_lz77(til, ev + " (tiles)", "pictures")
            map_words = None
            if m and self._claim(m, "pictures"):
                blob, _, _ = self.add_lz77(m, ev + " (map)", "pictures",
                                           subdir="maps")
                map_words = [struct.unpack_from("<H", blob, j)[0]
                             for j in range(0, len(blob) - 1, 2)]
            if til_blob is not None and map_words is not None:
                palette = [bgr555_to_rgb(w)
                           for w in pal_words(rom, pal, 64)]
                r = render_map(map_words, til_blob, len(palette))
                if r is not None:
                    indices, size = r
                    self.add_png("graphics/picture_%02d.png" % i, indices, size,
                                 palette, pal, "png-map",
                                 ev + " (rendered map)", "pictures")

    def walk_stage_gfx(self):
        """gUnk_08731F78 GfxHeaders: palette + LZ77 tiles, tile-strip PNG."""
        rom = self.rom
        t = census_sheets.STAGE_GFX
        end = self.next_label(t)
        for i, a in enumerate(range(t, end, 4)):
            h = u32(rom, a)
            if not h:
                continue
            ev = ("gUnk_08731F78[%d] -> GfxHeader 0x%08X, sub_080102c0 "
                  "src/cutscene_main.c:88-93" % (i, h))
            banks, tcount = u16(rom, h), u16(rom, h + 2)
            pal, til = u32(rom, h + 8), u32(rom, h + 12)
            palette = None
            if pal and self._claim(pal, "stage"):
                n = banks * 32
                words = pal_words(rom, pal, n)
                rel = "palettes/%s.pal" % self.name(pal)
                lines = ["JASC-PAL", "0100", str(len(words))]
                lines += ["%d %d %d" % bgr555_to_rgb(w) for w in words]
                self._write_text(rel, "\n".join(lines) + "\n")
                self.records.append({
                    "file": rel, "family": "stage", "kind": "palette",
                    "vma": pal, "rom_end": pal + n, "format": "pal-raw",
                    "evidence": ev + " (paletteBankCount %d * 32 bytes)" % banks})
                palette = [bgr555_to_rgb(w) for w in words]
            if til and self._claim(til, "stage"):
                blob, _, _ = self.add_lz77(
                    til, ev + " (tiles, tileCount %d)" % tcount, "stage")
                if palette is not None and len(blob) == tcount * 32:
                    r = render_strip(blob)
                    if r is not None:
                        indices, size = r
                        self.add_png("graphics/stage_%02d.png" % i, indices,
                                     size, palette, pal, "png-strip",
                                     ev + " (rendered)", "stage")

    def walk_raw_headers(self):
        """The four raw-tiles sheet headers the code reads (census 1c)."""
        rom = self.rom
        for h, (why, palsize) in sorted(census_sheets.RAW_HEADERS.items()):
            banks, tcount = u16(rom, h), u16(rom, h + 2)
            pal, til = u32(rom, h + 8), u32(rom, h + 12)
            ev = "sheet header 0x%08X: %s" % (h, why)
            palette = None
            if self._claim(pal, "headers"):
                n = banks * 32 if palsize == "banks" else (palsize or banks * 32)
                words = pal_words(rom, pal, n)
                rel = "palettes/%s.pal" % self.name(pal)
                lines = ["JASC-PAL", "0100", str(len(words))]
                lines += ["%d %d %d" % bgr555_to_rgb(w) for w in words]
                self._write_text(rel, "\n".join(lines) + "\n")
                self.records.append({
                    "file": rel, "family": "headers", "kind": "palette",
                    "vma": pal, "rom_end": pal + n, "format": "pal-raw",
                    "evidence": ev})
                palette = [bgr555_to_rgb(w) for w in words]
            if self._claim(til, "headers"):
                blob = rom[til - ROM_BASE:til - ROM_BASE + tcount * 32]
                rel = self._write("tiles/%s.4bpp" % self.name(til), blob)
                self.records.append({
                    "file": rel, "family": "headers", "kind": "tiles",
                    "vma": til, "rom_end": til + tcount * 32,
                    "format": "tiles-raw",
                    "evidence": ev + " (tileCount %d)" % tcount})
                if palette is not None:
                    r = render_strip(blob)
                    if r is not None:
                        indices, size = r
                        self.add_png("graphics/sheet_%08x.png" % h, indices,
                                     size, palette, pal, "png-strip",
                                     ev + " (rendered)", "headers")

    def walk_misc(self):
        """gUnk_08731980, TransferNode mode 8 sources, direct LZ77, SIZED.

        Unlike the census providers, these tables hold raw pointers whose
        targets were only claimed when they validated, so non-decoding
        entries are skipped here too (the consumers treat them as absent
        or the element is not an LZ77 stream)."""
        rom = self.rom
        end = self.next_label(census_sheets.OBJ_LZ_TABLE)
        for k, a in enumerate(range(census_sheets.OBJ_LZ_TABLE, end, 4)):
            v = u32(rom, a)
            if v and self._claim(v, "misclz"):
                if lz77_decode(rom, v) is None:
                    self.log("  skip 0x%08X: gUnk_08731980 element is no LZ77 "
                             "stream" % v)
                    continue
                self.add_lz77(v, "gUnk_08731980[%d][%d][%d], LZ77UnCompWram "
                              "sub_08008d10 src/mode_gfx_loaders.c:96-100"
                              % (k // 4, k // 2 % 2, k % 2), "misclz")
        end = self.next_label(census_sheets.GFX_SETS)
        for a in range(census_sheets.GFX_SETS, end, 4):
            lst = u32(rom, a)
            if not lst:
                continue
            p = lst
            while u32(rom, p):
                if u32(rom, p) & 0xF == 8 and self._claim(u32(rom, p + 4),
                                                           "misclz"):
                    if lz77_decode(rom, u32(rom, p + 4)) is None:
                        self.log("  skip 0x%08X: TransferNode mode 8 source is "
                                 "no LZ77 stream" % u32(rom, p + 4))
                        continue
                    self.add_lz77(u32(rom, p + 4),
                                  "TransferNode 0x%08X {mode 8} source, "
                                  "LZ77UnCompVram (RequestCopyList "
                                  "src/main_copy_queue.c:203)" % p, "misclz")
                p += 12
        for v, why in sorted(census_sheets.DIRECT_LZ77.items()):
            if self._claim(v, "misclz"):
                self.add_lz77(v, why, "misclz")
        for s, n, kind, why in census_sheets.SIZED:
            if kind == "palette":
                if self._claim(s, "misclz"):
                    self.add_palette(s, s + n, False, why, "misclz")
            elif self._claim(s, "misclz"):
                self.add_tiles(s, s + n, False, why, "misclz")

    # -- driver --------------------------------------------------------------

    def run(self):
        os.makedirs(self.out, exist_ok=True)
        # The rendering families run first so their consumer-paired blobs
        # (picture screens, stage GfxHeaders, raw sheet headers, misc LZ77)
        # claim their inputs; the census families then cover everything
        # proven without duplicating those files.
        self.walk_pictures()
        self.walk_stage_gfx()
        self.walk_raw_headers()
        self.walk_misc()
        self.walk_census("rooms", census_rooms.provide)
        self.walk_census("frames", census_sprites.provide)
        self.walk_census("sheets", census_sheets.provide)
        self.records.sort(key=lambda r: r["file"])
        manifest = {
            "generator": "tools/extract_assets.py",
            "policy": "assets are never committed (AGENTS.md, docs/data.md 1); "
                      "this directory is gitignored by design",
            "records": self.records}
        self._write_text("manifest.json", json.dumps(manifest, indent=1) + "\n")
        return manifest


def tree_files(root):
    out = {}
    for dirpath, _dirs, files in os.walk(root):
        for fn in files:
            path = os.path.join(dirpath, fn)
            rel = os.path.relpath(path, root)
            with open(path, "rb") as f:
                out[rel] = f.read()
    return out


def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--rom", default="baserom.gba")
    ap.add_argument("--config", default="tools/split_config.json")
    ap.add_argument("--segments", default="docs/analysis/segments.txt")
    ap.add_argument("--out", default="assets")
    ap.add_argument("--check", action="store_true",
                    help="re-extract into a temp dir and compare byte for "
                         "byte (CI mode; fails on any drift or edit)")
    ap.add_argument("--verbose", action="store_true")
    args = ap.parse_args()

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

    if args.check:
        if not os.path.exists(os.path.join(args.out, "manifest.json")):
            print("ERROR: %s has no manifest.json; run without --check first"
                  % args.out, file=sys.stderr)
            return 1
        with tempfile.TemporaryDirectory() as tmp:
            ex = Extractor(rom, cfg, segs, tmp, args.verbose)
            ex.run()
            want = tree_files(tmp)
        have = tree_files(args.out)
        bad = []
        for rel in sorted(set(want) | set(have)):
            if rel not in have:
                bad.append("%s: missing" % rel)
            elif rel not in want:
                bad.append("%s: not derived from the ROM (stale or hand-added)"
                           % rel)
            elif want[rel] != have[rel]:
                bad.append("%s: differs from the ROM-derived content" % rel)
        n = len(want)
        if bad:
            print("assets check: FAILED (%d problems)" % len(bad))
            for line in bad[:20]:
                print("  " + line)
            if len(bad) > 20:
                print("  ... and %d more" % (len(bad) - 20))
            return 1
        print("assets check: OK (%d files byte-identical to a fresh extraction "
              "from %s)" % (n, args.rom))
        return 0

    ex = Extractor(rom, cfg, segs, args.out, args.verbose)
    manifest = ex.run()
    by_kind = {}
    for r in manifest["records"]:
        by_kind[r["kind"]] = by_kind.get(r["kind"], 0) + 1
    print("extracted %d artifacts into %s/: %s"
          % (len(manifest["records"]), args.out,
             ", ".join("%s=%d" % kv for kv in sorted(by_kind.items()))))
    print("skipped %d structural claims (RoomDefs, doors, object lists, value "
          "tables: records are C since #167, layouts not decoded yet; "
          "docs/assets.md)" % ex.skipped)
    print("policy: nothing here is committed; --check verifies against the ROM")
    return 0


if __name__ == "__main__":
    sys.exit(main())
