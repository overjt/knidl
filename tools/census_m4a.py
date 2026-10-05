"""Pointer-census evidence: the m4a song structure, the multiboot images of
seg 19 and one byte table (issue #36 phase 2 run 2, tools/ptrcensus.py
provider).

provide(rom, cfg, segs) -> {"coincidence": [(start, end, kind, why)],
                            "pointer": [(addr, why)]}

* The m4a song structure (tools/m4a_struct.py, started from the song
  tables the config's "m4a" entry lists, or from gSongTable/gSfxTable with
  their consumers when it has none): every pointer word it proves, at any
  alignment, and as coincidences every other byte it parsed: executed and
  never-executed track events without their pointer operands, the PCM
  bodies of the 129 WaveData (extent = the header's size field, plus the
  one sample byte and zeros to the next 4-aligned object), WaveData
  headers, 4-bit CGB waves, ToneData number fields, song-header counts,
  the song tables' ms/me and prio/chans fields, and padding.  The parse
  must round-trip (every structure re-encoded from its fields equals the
  ROM) or nothing is provided.
* seg 19 from 0x0876B1FC: four complete GBA program images that the game
  only copies to another GBA, whose extents the consumers give as label
  differences: MultiBootInitWithParams(gUnk_0876B1FC, gUnk_0876F690)
  (src/menu_sound_test_link_play.c:137, src/link_setup.c:114) sends [start+0xC0, end)
  as the multiboot program, sub_08007e04 copies gUnk_0876F690 -
  gUnk_0876B1FC bytes of it to 0x02020000 (src/mode_extra_mode_title.c:164-165), and
  sub_08007c5c sends one of [gUnk_0876F690, gUnk_087954C0),
  [gUnk_087954C0, gUnk_087C0A4C), [gUnk_087C0A4C, gRoomTable) to the other
  players with LinkBlockAnnounce (src/mode_extra_mode_title.c:99-111,
  src/link_setup_intr_block.c:265).  Each image starts with its own cartridge header
  (checked here: `b 0xC0` and the Nintendo logo of this ROM).  An image is
  a separately linked program that runs from the receiver's EWRAM; its
  words do not depend on this ROM's layout, and no code of this game reads
  inside it (its only references are the four starts, include/mode.h).
* gUnk_0872EB14, u8 [shape][size][2] sprite half-dimensions read as bytes
  by the affine sprite emitter (src/main_affine_sprite.c:54-55), up to
  gBootSignature (0x0872EB2C).
"""

import os
import struct
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import m4a_struct  # noqa: E402  (tools/m4a_struct.py)

ROM_BASE = 0x08000000

# One kind for every m4a range: ptrcensus.py merges touching ranges and
# reports a merged run under its first range's kind, and the song zone is
# one run (song table, headers, waves, tracks back to back).  The "why"
# of each range still says what it is.
KIND = "m4a song data"

# the multiboot / link images, in address order, and what ends the last one
# (src/mode_extra_mode_title.c:99-111 and :164, src/menu_sound_test_link_play.c:137)
IMAGE_STARTS = [
    (0x0876B1FC, "the single-pak multiboot program: MultiBootInitWithParams("
     "gUnk_0876B1FC, gUnk_0876F690), src/menu_sound_test_link_play.c:137; sub_08007e04 "
     "copies it to 0x02020000, src/mode_extra_mode_title.c:164-165"),
    (0x0876F690, "sub-game image 0: sub_08007c5c sends gUnk_087954C0 - "
     "gUnk_0876F690 bytes to the other players, src/mode_extra_mode_title.c:99-101"),
    (0x087954C0, "sub-game image 1: sub_08007c5c sends gUnk_087C0A4C - "
     "gUnk_087954C0 bytes, src/mode_extra_mode_title.c:103-105"),
    (0x087C0A4C, "sub-game image 2: sub_08007c5c sends gRoomTable - "
     "gUnk_087C0A4C bytes, src/mode_extra_mode_title.c:107-109"),
]
IMAGES_END = 0x087E1D58  # gRoomTable

HALF_DIMS = (0x0872EB14, 0x0872EB2C,
             "gUnk_0872EB14: u8 {w, h} sprite half-dimensions by shape and "
             "size, read as bytes by the affine sprite emitter "
             "(src/main_affine_sprite.c:54-55; include/main.h:128), up to "
             "gBootSignature")


def _data_symbol_addrs(cfg):
    out = set()
    for a in cfg.get("data_symbols", {}):
        try:
            out.add(int(a, 16))
        except ValueError:
            pass
    return out


def _m4a(rom, cfg):
    roots = m4a_struct.load_roots(cfg) or m4a_struct.DEFAULT_ROOTS
    m = m4a_struct.Parser(rom, roots).parse()
    _enc, _ext, bad = m4a_struct.roundtrip(rom, m)
    if bad or m4a_struct.check_overlaps(m):
        return [], []
    ptr_bytes = set()
    pointers = []
    for a, (_t, why) in sorted(m.slots.items()):
        pointers.append((a, why))
        ptr_bytes.update(range(a, a + 4))
    ranges = []

    def add(s, e, kind, why):
        """[s, e) minus the pointer bytes inside it."""
        cur = s
        for a in range(s, e):
            if a in ptr_bytes:
                if a > cur:
                    ranges.append((cur, a, kind, why))
                cur = a + 1
        if e > cur:
            ranges.append((cur, e, kind, why))

    why_ev = ("m4a track bytes, executed: decoded by an abstract run of "
              "MPlayMain's dispatch and the ply_* handlers (asm/m4a_1.s, "
              "src/m4a_ctrl.c) from the song headers' part[] pointers; "
              "commands, waits, notes and their operands (pointer operands "
              "excluded) are numbers")
    why_dead = ("m4a track bytes, never executed: the rest of a track after "
                "its looping GOTO, decoded linearly to its FINE right before "
                "the next object (pointer operands excluded)")
    for evs, why in ((m.events, why_ev), (m.dead, why_dead)):
        for a in sorted(evs):
            add(a, a + evs[a].size, KIND, why)
    whys = {
        "pcm": "PCM sample body: WaveData.size samples (s8) after a WaveData "
               "header that a DirectSound ToneData or XCMD xWAVE points at "
               "(ply_note -> the mixer, asm/m4a_1.s)",
        "pcmtail": "PCM sample tail: the sample byte after WaveData.size, "
                   "then zeros to the next 4-aligned object",
        "wavehdr": "WaveData header {u16 type, status; u32 freq, loopStart, "
                   "size}: numbers (include/gba/m4a_internal.h)",
        "progwave": "CGB wave: 16 bytes of 4-bit samples a ToneData of type "
                    "3 points at (CgbSound copies 4 words to wave RAM, "
                    "src/m4a_cgb.c)",
        "keysplit": "key-split table: u8 sub-voice indices (ply_note)",
        "pad": "zero padding after a track's FINE, up to the next 4-aligned "
               "song header",
        "tone": "ToneData number fields (type, key, length, pan/sweep, "
                "ADSR; a square/noise voice's duty or period): numbers; its "
                "pointer fields are excluded",
        "song": "SongHeader counts {u8 trackCount, blockCount, priority, "
                "reverb} (MPlayStart); its pointer fields are excluded",
        "songtable": "song-table value fields (struct Song u16 ms, me; "
                     "struct SongEntry u8 prio, chans, pad[2]); the header "
                     "pointers are excluded",
    }
    for a, (e, kind, _f) in sorted(m.objects.items()):
        add(a, e, KIND, whys[kind])
    # touching ranges with the same reason become one (events back to back)
    merged = []
    for r in sorted(ranges):
        if merged and merged[-1][1] == r[0] and merged[-1][3] == r[3]:
            merged[-1] = (merged[-1][0], r[1], r[2], r[3])
        else:
            merged.append(r)
    return merged, pointers


def _images(rom, cfg):
    labels = _data_symbol_addrs(cfg)
    logo = rom[4:0xA0]
    out = []
    bounds = [s for s, _w in IMAGE_STARTS] + [IMAGES_END]
    for i, (s, why) in enumerate(IMAGE_STARTS):
        e = bounds[i + 1]
        o = s - ROM_BASE
        if (s not in labels or e not in labels
                or struct.unpack_from("<I", rom, o)[0] != 0xEA00002E
                or rom[o + 4:o + 0xA0] != logo):
            continue  # the consumer's labels or the image header moved
        out.append((s, e, "program image",
                    "a separately linked GBA program the game only copies to "
                    "another GBA (own cartridge header at 0x%08X); %s"
                    % (s, why)))
    return out


def provide(rom, cfg, segs):
    ranges, pointers = _m4a(rom, cfg)
    ranges += _images(rom, cfg)
    s, e, why = HALF_DIMS
    if s in _data_symbol_addrs(cfg):
        ranges.append((s, e, "u8 table", why))
    return {"coincidence": ranges, "pointer": pointers}


if __name__ == "__main__":
    import json
    rom = open(sys.argv[1] if len(sys.argv) > 1 else "baserom.gba", "rb").read()
    cfg = json.load(open(sys.argv[2] if len(sys.argv) > 2
                         else "tools/split_config.json"))
    r = provide(rom, cfg, [])
    print("%d coincidence ranges (%d bytes), %d pointers"
          % (len(r["coincidence"]),
             sum(e - s for s, e, _k, _w in r["coincidence"]),
             len(r["pointer"])))
