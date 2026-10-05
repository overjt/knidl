# Asset extraction

This is the policy-sanctioned way to make the ROM's assets editable.  The
data policy (`AGENTS.md`, [data.md](data.md) §1) never commits assets in any
form — no PNGs, no MIDI, no hex dumps — because the repository is public;
they stay `.incbin "baserom.gba", <offset>, <length>` slices read from the
user's own dump.  `tools/extract_assets.py` extracts them from your
`baserom.gba` into the **gitignored `assets/` directory** on your machine
(the tmc asset-manifest / mzm extractor model), where you can view and edit
them.  The ROM build never reads this directory.

```sh
make assets        # extract into assets/   (~15,600 files, a few seconds)
make assets-check  # verify assets/ is still byte-identical to the ROM
make assets-mod    # re-inject your edits: rebuild knidl-mod.gba (below)
```

All of them run on the host with plain Python 3 (no toolchain image) and
require `baserom.gba`.

## What is extracted, and why it is trustworthy

Nothing here is pattern-guessed.  Every object's address, size and format
was proven by the pointer census (issue #36 phase 2, `tools/ptrcensus.py`
and its providers), and the extractor reuses those providers directly, so
the boundaries are exactly the census-proven ones and every manifest entry
cites the consumer that reads the bytes (`src/` file and line).

| family | source of objects | contents |
| --- | --- | --- |
| `pictures` | `gUnk_087319C8[]` rows (sub_08008d98) | the 8 full-screen pictures: palette, tiles, tilemap **and a rendered PNG** |
| `stage` | `gUnk_08731F78[]` GfxHeaders (sub_080102c0) | stage graphics: palette + LZ77 tiles, rendered as tile strips |
| `headers` | the 4 raw-tiles sheet headers the code reads | palette + tiles, rendered as tile strips |
| `misclz` | `gUnk_08731980`, TransferNode mode 8, direct LZ77 calls, `census_sheets.SIZED` | decoded LZ77 blobs and consumer-sized palettes/tile blocks |
| `rooms` | `census_rooms.py` | every RoomDef's metatile maps, BG maps (raw, LZ77 and BG3-header forms) and palettes |
| `frames` | `census_sprites.py` | the sprite frame network: TaskGfx tile chunk streams, counted palettes, OAM template streams |
| `sheets` | `census_sheets.py` | everything else proven in the two sprite-sheet zones |

## Output formats

```
assets/
  manifest.json          every file -> ROM range, format, consumer evidence
  palettes/*.pal         JASC-PAL text (us-gba style); BGR555 expanded to RGB
  tiles/*.4bpp           raw 4bpp tile blobs
  tiles/*.chunks.4bpp    TaskGfx {u16 size; data}.. 0xFFFF streams,
                         chunks concatenated (chunk layout in the manifest)
  lz77/*.bin             decoded BIOS LZ77 content
  maps/*.bin             room and picture tilemaps (u16 entries; BG3 headers
                         recorded in the manifest)
  oam/*.json             BuildOam template streams, one entry per object
  graphics/*.png         rendered views: the 8 pictures and the tile strips,
                         indexed PNGs whose PLTE is the object's palette
  misc/*.bin             Huffman streams (undecoded) and raw-copy blocks
```

Files are named after their `split_config.json` label (`gUnk_...` until the
naming runs reach them), with `_2`-style suffixes on collisions and
`unk_<address>` when no label exists.  Re-running `make assets` only
rewrites files whose content changed.

## The check

`make assets-check` re-extracts into a temporary directory and compares the
two trees byte for byte (the extraction is deterministic), then fails on
any missing, extra or differing file.  It also fails when `manifest.json`
regenerates differently, which catches `split_config.json` /
`segments.txt` drift the same way `make compare` catches a moved section.

This is a **fidelity** check, not a linter: a file you edited will (correctly)
fail it.  The intended modding loop is edit → `make assets` is *not* run
again over your edits → re-inject them with `make assets-mod` (below); run
`make assets-check` only to prove a pristine tree.

CI runs `assets-check`, `assets-mod-check` and `assets-selftest` on every
push where a `baserom.gba` is available, right after `make compare`.

## Re-injection: rebuild a modded ROM from edited assets

`tools/rebuild_assets.py` — the inverse of the extractor — rebuilds a
playable modded ROM from `baserom.gba` plus your edited `assets/` tree:

```sh
make assets-mod        # write knidl-mod.gba (gitignored; never re-extracts,
                       # so it cannot overwrite your edits)
make assets-mod-check  # pristine round-trip: an unedited tree must rebuild
                       # a ROM byte-identical to baserom.gba (SHA-1 printed)
make assets-selftest   # encode every object straight from the ROM and verify
                       # (no assets/ tree needed)
```

All three run on the host with plain Python 3 and require `baserom.gba`;
`assets-mod`/`assets-mod-check` also need a populated `assets/`
(`make assets` first).

**How it works.**  The tool re-runs the extractor (imported, not copied)
into a temporary directory and compares every manifest record's file with
the ROM-derived original.  Records whose bytes are equal are *unchanged*:
their ROM ranges are never touched, so an unedited tree rebuilds a
byte-identical ROM.  Edited records are re-encoded into their ROM format
and written in place over their slot `[vma, rom_end)`:

| format | re-encoding |
| --- | --- |
| `pal-raw` / `pal-counted` | JASC-PAL colours quantized back to BGR555 (8→5 bits; `>> 3` round-trips the extractor's `(v << 3) \| (v >> 2)` expansion exactly); the counted form re-writes the `u16` byte count |
| `tiles-raw` | the `.4bpp` bytes verbatim |
| `tiles-chunks` | re-framed with the manifest's chunk layout: chunk count and the `0xFFFF` terminator kept, per-chunk sizes may change — a blob that grew or shrank resizes the *last* chunk, because chunks are copied to VRAM pages `0x400` apart (`TaskLoadFrameTiles`, `src/task_frame_tiles.c`), so earlier chunks keep their pages |
| `lz77` | re-compressed with a BIOS LZ77 (type 0x10) writer; every stream is decoded back through `extract_assets.lz77_decode` and compared before it is spliced, and its size may differ from Nintendo's original |
| `raw-map` / `bgmap-raw` | map bytes verbatim / behind the 6-byte `{size, width, height}` header; the dimensions are fixed by the RoomDef, so the file length must not change |
| `bgmap-lz77` | the same header (the `u16` flag word at +6 is preserved from the ROM) and the re-compressed stream at the recorded `stream_vma` |
| `oam` | JSON entries packed back to little-endian halfwords; attr0 bit 12 (BuildOam's "last" flag, `src/main_build_oam.c`) must be set on exactly the final entry |

`graphics/*.png` are rendered **views**: an edit fails with the list of
view-only files and the underlying `palettes/`, `tiles/` or `maps/` file to
edit instead.  `misc/*.bin` (`huffman`, `raw-copy`) are undecoded copies and
fail the same way — no encoder exists for them in this phase.

**Splice policy.**  `knidl-mod.gba` starts as the bytes of `baserom.gba`
and each re-encoded object is written in place over its slot.  A
re-encoded object must *fit* its slot: smaller is fine and zero-padded to
the slot end (every padded object is reported); growing past it fails with
a per-object report, because growth needs the `MATCHING=0` rebuild path on
the proven-movable ROM ([data.md](data.md) §8), which this tool
deliberately does not attempt.  The output's GBA header checksum is fixed
with `tools/gbafix.py`.

`make assets-selftest` is the encoders' own regression check: for every
verbatim format the re-encoded pristine file must reproduce the ROM slot
byte for byte, and for the LZ77 forms the stream must decode back to
exactly the file's bytes (currently 15,620 objects: 14,386 byte-exact,
1,091 LZ77 round-trips, 143 views/undecoded skipped).

## Not extracted (yet)

- **Structural level data** — the extractor skips these claims (2,505).
  The functional records among them are typed C since #167 (the RoomDef
  headers, the BG animation scripts and the handler tables, `src/data/`,
  [data.md](data.md) §5.2), so they are edited as C, not extracted.  The
  door geometry and the room object lists (enemy placements) are level
  layouts, assets by policy; a future `assets/rooms/*.json` view for level
  editors would decode them on the same manifest.
- **Audio** — the m4a song tree (`tools/m4a_struct.py` already parses it:
  330 song headers, 701 tracks, 129 samples).  Songs first need a decoded
  representation (a midi-like JSON) before extraction makes sense.
- **Rendered sprite sheets** — per-frame PNGs from the TaskGfx records
  (8,947 OAM streams + 4,468 tile blobs are extracted; the per-frame
  composition with OAM offsets is a natural follow-up).
- **ROM growth** — re-injection writes each edited object back into its
  original slot, so an object that grew past its slot fails; moving
  objects and growing the ROM needs the `MATCHING=0` rebuild path on the
  proven-movable layout (data.md §8), the natural next step on top of
  `tools/rebuild_assets.py`.
