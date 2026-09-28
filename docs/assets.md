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
```

Both run on the host with plain Python 3 (no toolchain image) and require
`baserom.gba`.

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
again over your edits → use your edited assets with the future re-injection
tooling (below); run `make assets-check` only to prove a pristine tree.

CI runs `assets-check` on every push where a `baserom.gba` is available,
right after `make compare`.

## Not extracted (yet)

- **Structural level data** — RoomDef headers, door records, room object
  lists (enemy placements), BG animation scripts and the value tables.
  [data.md](data.md) §7 plans these as decompiled C rather than extracted
  blobs, so the extractor skips them (2,505 claims and counting) and leaves
  them to that path.  A future `assets/rooms/*.json` view for level editors
  would build on the same manifest.
- **Audio** — the m4a song tree (`tools/m4a_struct.py` already parses it:
  330 song headers, 701 tracks, 129 samples).  Songs first need a decoded
  representation (a midi-like JSON) before extraction makes sense.
- **Rendered sprite sheets** — per-frame PNGs from the TaskGfx records
  (8,947 OAM streams + 4,468 tile blobs are extracted; the per-frame
  composition with OAM offsets is a natural follow-up).
- **Re-injection** — rebuilding a modded ROM from edited assets.  The
  manifest already records every ROM range and framing detail (chunk
  layouts, BG3 headers, stream VMAs) needed to write the inverse tool;
  `make MATCHING=0` plus the proven-movable ROM (data.md §8) is the
  building ground.
