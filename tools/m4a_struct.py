#!/usr/bin/env python3
"""The m4a (MusicPlayer2000) song structure of the ROM (issue #36).

A deterministic parser of the song data the sound engine reads, started
from the song tables the decompiled code indexes (the config's "m4a"
entry lists them):

  gSongTable (0x0860B460, 579 x struct Song {SongHeader *header; u16 ms,
      me}, include/gba/m4a_internal.h): m4aSongNumStart and its siblings
      (src/m4a_c1.c) pass gSongTable[n].header to MPlayStart;
  gSfxTable (0x0872EB38, 479 x struct SongEntry {SongHeader *header; u8
      prio, chans, pad[2]}, include/sound.h): PlaySfx (src/early_31b8.c)
      takes id 100..578 and passes gSfxTable[id - 100].header to
      MPlayStart; sub_0800d404 (src/menu_0ca10.c) scans the same 479.

What the engine reads, and so what the parser walks.  Everything below is
THIS ROM's engine (asm/m4a_1.s, src/m4a_c1.c, src/m4a_ctrl.c,
src/m4a_cgb.c), not a generic MP2K description:

  SongHeader {u8 trackCount, blockCount, priority, reverb; ToneData *tone;
      u8 *part[trackCount]}: MPlayStart copies tone and part[i] for
      i < trackCount.  The one 0-track header (0x0860C678) is 4 bytes: its
      "tone" word is the first word of the WaveData the voicegroups point
      at, 4 bytes later, and MPlayStart starts no track from it.
  Track streams: MPlayMain (asm/m4a_1.s, 0x080CE008-0x080CE05E) reads a
      byte; below 0x80 it is a data byte of the running status (the last
      command byte >= 0xBD), else it is consumed and, if >= 0xBD, becomes
      the running status.  >= 0xCF: a note (ply_note: gate time
      gClockTable[cmd - 0xCF], then up to three bytes < 0x80, key,
      velocity, extra gate time, each read only if the one before was);
      0xB1-0xCE: gMPlayJumpTable[cmd - 0xB1] (gMPlayJumpTableTemplate at
      0x0860A140, patched by MPlayExtender, src/m4a_c1.c:263-271);
      0x80-0xB0: a wait.  The handlers and their operands:
        B1 ply_fine    -           the track ends
        B2 ply_goto    ptr32       (read with byte loads: any alignment)
        B3 ply_patt    ptr32       a call; at patternLevel 3 it is ply_fine
        B4 ply_pend    -           a return; a no-op at level 0
        B5 ply_rept    u8, ptr32   count 0: a goto; else loop count times
        B6-B8          ply_fine
        B9 ply_memacc  u8 op, u8 addr, u8 data [, ptr32 when 6 <= op <= 17]
                       (src/m4a_ctrl.c: ops 6-17 jump through
                       gMPlayJumpTable[1] = ply_goto or skip 4 bytes)
        BA prio, BB tempo, BC keysh, BD voice, BE vol, BF pan, C0 bend,
        C1 bendr, C2 lfos, C3 lfodl, C4 mod, C5 modt, C8 tune: one byte
        C6, C7, C9-CB  ply_fine
        CC ply_port    u8, u8
        CD ply_xcmd    u8 n -> gXcmdTable[n] (0x0860A3E8, 12 entries):
                       0, 3 ply_xxx (= ply_fine, reads nothing more);
                       1 ply_xwave ptr32 (a WaveData *); 2, 4-11 one byte
        CE ply_endtie  [key < 0x80]
  ToneData (12 bytes; ply_voice copies voicegroup[prog], ply_note plays):
      type & 0x40 (key split): wav = sub-voicegroup, word 2 = u8 table
          indexed by the note's raw key, giving the sub-voice index;
      type & 0x80 (rhythm): wav = sub-voicegroup indexed by the raw key;
      a sub-voice whose type has 0x40 or 0x80 plays nothing;
      type & 7 == 0: DirectSound, wav = WaveData * (MidiKeyToFreq);
      type & 7 == 3: wav = a 16-byte 4-bit wave (CgbSound copies 4 words);
      type & 7 in 1, 2, 4: wav is a number (duty << 6, noise bit << 3).
  WaveData {u16 type, status; u32 freq, loopStart, size; s8 data[size]},
      followed in this ROM by one more sample byte and zeros up to the
      next 4-aligned object.

The tracks are executed abstractly (state: position, pattern-return
stack, running status, current program, current key), so every decoded
byte is one the engine reads; the (voicegroup, program, key) uses give
the extents: a voicegroup is entries 0..(highest program VOICE selects),
extended up to the next proven object when that gap is whole ToneData
entries that all validate; a drum kit is entries min..max key of the
notes played with it; a key-split table min..max key and its
sub-voicegroup min..max index.  After a track's looping GOTO the ROM
often keeps the rest of the source track (never executed): it is
decoded linearly and must end with a FINE right before the next object.

Everything parsed is re-encoded from the parsed fields and compared with
the ROM (`roundtrip()`).  Stdlib only; importable (tools/split.py calls
`split_plan()`) and runnable:

  python3 tools/m4a_struct.py [--rom baserom.gba] [--config
      tools/split_config.json] [--segments docs/analysis/segments.txt]
"""

import argparse
import json
import struct
import sys

ROM_BASE = 0x08000000

CMD_NAMES = {
    0xB1: "FINE", 0xB2: "GOTO", 0xB3: "PATT", 0xB4: "PEND", 0xB5: "REPT",
    0xB9: "MEMACC", 0xBA: "PRIO", 0xBB: "TEMPO", 0xBC: "KEYSH",
    0xBD: "VOICE", 0xBE: "VOL", 0xBF: "PAN", 0xC0: "BEND", 0xC1: "BENDR",
    0xC2: "LFOS", 0xC3: "LFODL", 0xC4: "MOD", 0xC5: "MODT", 0xC8: "TUNE",
    0xCC: "PORT", 0xCD: "XCMD", 0xCE: "EOT",
}
# gMPlayJumpTable entries that are ply_fine after MPlayExtender
FINE_ALIASES = (0xB6, 0xB7, 0xB8, 0xC6, 0xC7, 0xC9, 0xCA, 0xCB)
ONE_BYTE_CMDS = (0xBA, 0xBB, 0xBC, 0xBD, 0xBE, 0xBF, 0xC0, 0xC1, 0xC2,
                 0xC3, 0xC4, 0xC5, 0xC8)
# gXcmdTable: index -> operand bytes (None = ply_xxx, the track ends)
XCMD_OPERANDS = {0: None, 1: "ptr", 2: 1, 3: None, 4: 1, 5: 1, 6: 1, 7: 1,
                 8: 1, 9: 1, 10: 1, 11: 1}
MAX_PATTERN_LEVEL = 3  # ply_patt: patternLevel >= 3 -> ply_fine

TONE_SIZE = 12
WAVE_HEADER = 16
PROG_WAVE_SIZE = 16

# label name prefixes by kind (the parser names every pointer target)
LABEL_PREFIX = {
    "song": "gSong_",
    "track": "gSongTrack_",
    "trackloc": "gSongLoc_",
    "voicegroup": "gVoiceGroup_",
    "subvg": "gVoiceGroup_",
    "keysplit": "gKeySplit_",
    "wave": "gWave_",
    "progwave": "gProgWave_",
}
# when one address is several kinds of target, the first kind here names it
LABEL_ORDER = ("song", "voicegroup", "subvg", "keysplit", "wave",
               "progwave", "track", "trackloc")

# The consumers of the song tables (used when the config has none).
DEFAULT_ROOTS = [
    (0x0860B460, 579, 8,
     "gSongTable[579] (struct Song {SongHeader *header; u16 ms, me}, "
     "include/gba/m4a_internal.h): m4aSongNumStart (src/m4a_c1.c) passes "
     "gSongTable[n].header to MPlayStart"),
    (0x0872EB38, 479, 8,
     "gSfxTable[479] (struct SongEntry {SongHeader *header; u8 prio, "
     "chans}, include/sound.h): PlaySfx (src/early_31b8.c) passes "
     "gSfxTable[id - 100].header, id 100..578, to MPlayStart"),
]


class M4AError(Exception):
    pass


class Event(object):
    """One decoded track event: the bytes [addr, addr + size)."""
    __slots__ = ("addr", "size", "cmd", "explicit", "args", "ptr_at",
                 "ptr", "kind")

    def __init__(self, addr, cmd, explicit, args, ptr_at=None, ptr=None,
                 kind=None):
        self.addr = addr
        self.cmd = cmd            # effective command (running status resolved)
        self.explicit = explicit  # the command byte is in the stream
        self.args = args          # operand bytes before the pointer
        self.ptr_at = ptr_at      # offset of the pointer operand from addr
        self.ptr = ptr
        self.kind = kind
        self.size = ((1 if explicit else 0) + len(args)
                     + (4 if ptr_at is not None else 0))

    def encode(self):
        out = bytearray()
        if self.explicit:
            out.append(self.cmd)
        out.extend(self.args)
        if self.ptr_at is not None:
            if self.ptr_at != len(out):
                raise M4AError("event 0x%08X: pointer offset" % self.addr)
            out.extend(struct.pack("<I", self.ptr))
        return bytes(out)


class Model(object):
    """Everything the parser proved.

    objects: start -> (end, kind, fields) for the song tables, song
        headers, ToneData, key-split spans, WaveData headers and bodies,
        sample tails, programmable waves and padding;
    events / dead: addr -> Event (executed / never-executed track bytes);
    slots: addr -> (target, why), every pointer word, any alignment;
    labels: target addr -> set of kinds."""

    def __init__(self):
        self.objects = {}
        self.events = {}
        self.dead = {}
        self.slots = {}
        self.labels = {}
        self.warnings = []
        self.headers = {}
        self.table_slots = {}
        self.vg_uses = {}
        self.note_uses = set()
        self.vg_extent = {}
        self.sub_extent = {}
        self.ks_extent = {}
        self.tones = {}
        self.waves = {}
        self.prog_waves = set()
        self.sub_bases = set()
        self.rs_after = {}
        self.dead_ptrs = []
        self.unexplained = []
        self.track_states = 0


class Parser(object):
    def __init__(self, rom, song_tables):
        self.rom = rom
        self.song_tables = song_tables  # [(start, count, stride, why)]
        self.m = Model()

    # ---- ROM access ------------------------------------------------------

    def off(self, addr):
        o = addr - ROM_BASE
        if not 0 <= o < len(self.rom):
            raise M4AError("address 0x%08X outside the ROM" % addr)
        return o

    def u8(self, addr):
        return self.rom[self.off(addr)]

    def u32(self, addr):
        if addr + 4 > ROM_BASE + len(self.rom):
            raise M4AError("word 0x%08X outside the ROM" % addr)
        return struct.unpack_from("<I", self.rom, self.off(addr))[0]

    def ptr_ok(self, v):
        return ROM_BASE <= v < ROM_BASE + len(self.rom)

    # ---- bookkeeping -----------------------------------------------------

    def add_object(self, start, end, kind, fields=None):
        old = self.m.objects.get(start)
        if old is not None:
            if old[0] != end or old[1] != kind:
                raise M4AError("object at 0x%08X parsed twice (%s, %s)"
                               % (start, old[1], kind))
            return
        self.m.objects[start] = (end, kind, fields)

    def add_slot(self, addr, target, why, label_kind):
        old = self.m.slots.get(addr)
        if old is not None and old[0] != target:
            raise M4AError("slot 0x%08X has two targets" % addr)
        if old is None:
            self.m.slots[addr] = (target, why)
        if target:
            self.m.labels.setdefault(target, set()).add(label_kind)

    # ---- the walk --------------------------------------------------------

    def parse(self):
        for start, count, stride, why in self.song_tables:
            self.add_object(start, start + count * stride, "songtable",
                            (count, stride))
            for i in range(count):
                slot = start + i * stride
                h = self.u32(slot)
                if h == 0:
                    continue
                if not self.ptr_ok(h):
                    raise M4AError("song table slot 0x%08X = 0x%08X"
                                   % (slot, h))
                self.m.table_slots[slot] = h
                self.add_slot(slot, h, why, "song")
        for h in sorted(set(self.m.table_slots.values())):
            self.parse_header(h)
        self.run_tracks()
        self.parse_voices()
        self.fill_gaps()
        self.check_dead_ptrs()
        return self.m

    def parse_header(self, h):
        if h % 4:
            raise M4AError("song header 0x%08X is not 4-aligned" % h)
        n, blocks, prio, reverb = struct.unpack_from("<BBBB", self.rom,
                                                     self.off(h))
        if n > 16:
            raise M4AError("song header 0x%08X: %d tracks" % (h, n))
        if n == 0:
            self.add_object(h, h + 4, "song", (0, blocks, prio, reverb,
                                               None, ()))
            self.m.headers[h] = (0, None, ())
            return
        tone = self.u32(h + 4)
        if not self.ptr_ok(tone) or tone % 4:
            raise M4AError("song header 0x%08X: tone 0x%08X" % (h, tone))
        why = "SongHeader 0x%08X (MPlayStart, src/m4a_c1.c)" % h
        self.add_slot(h + 4, tone, why + ": tone", "voicegroup")
        parts = []
        for i in range(n):
            p = self.u32(h + 8 + 4 * i)
            if not self.ptr_ok(p):
                raise M4AError("song header 0x%08X: part[%d] 0x%08X"
                               % (h, i, p))
            self.add_slot(h + 8 + 4 * i, p, why + ": part[%d]" % i, "track")
            parts.append(p)
        self.add_object(h, h + 8 + 4 * n, "song",
                        (n, blocks, prio, reverb, tone, tuple(parts)))
        self.m.headers[h] = (n, tone, tuple(parts))
        self.m.vg_uses.setdefault(tone, set())

    # ---- tracks ----------------------------------------------------------

    def decode(self, pc, rs):
        """(Event at pc, running status after it), as MPlayMain does it."""
        b = self.u8(pc)
        if b < 0x80:
            cmd, explicit, q = rs, False, pc
        else:
            cmd, explicit, q = b, True, pc + 1
            if b >= 0xBD:
                rs = b
        if cmd is None or cmd < 0xBD and not explicit:
            raise M4AError("data byte 0x%02X at 0x%08X without a running "
                           "status" % (b, pc))
        if cmd >= 0xCF:
            args = []
            while len(args) < 3:
                v = self.u8(q + len(args))
                if v >= 0x80:
                    break
                args.append(v)
            return Event(pc, cmd, explicit, args, kind="note"), rs
        if cmd <= 0xB0:
            return Event(pc, cmd, explicit, [], kind="wait"), rs
        if cmd == 0xB1 or cmd in FINE_ALIASES:
            return Event(pc, cmd, explicit, [], kind="fine"), rs
        if cmd in (0xB2, 0xB3):
            return Event(pc, cmd, explicit, [], ptr_at=q - pc,
                         ptr=self.u32(q),
                         kind="goto" if cmd == 0xB2 else "patt"), rs
        if cmd == 0xB4:
            return Event(pc, cmd, explicit, [], kind="pend"), rs
        if cmd == 0xB5:
            return Event(pc, cmd, explicit, [self.u8(q)], ptr_at=q + 1 - pc,
                         ptr=self.u32(q + 1), kind="rept"), rs
        if cmd == 0xB9:
            args = [self.u8(q), self.u8(q + 1), self.u8(q + 2)]
            if 6 <= args[0] <= 17:
                return Event(pc, cmd, explicit, args, ptr_at=q + 3 - pc,
                             ptr=self.u32(q + 3), kind="memacc_jump"), rs
            return Event(pc, cmd, explicit, args, kind="cmd"), rs
        if cmd in ONE_BYTE_CMDS:
            kind = "voice" if cmd == 0xBD else "cmd"
            return Event(pc, cmd, explicit, [self.u8(q)], kind=kind), rs
        if cmd == 0xCC:
            return Event(pc, cmd, explicit, [self.u8(q), self.u8(q + 1)],
                         kind="cmd"), rs
        if cmd == 0xCD:
            n = self.u8(q)
            if n not in XCMD_OPERANDS:
                raise M4AError("XCMD %d at 0x%08X is outside gXcmdTable"
                               % (n, pc))
            what = XCMD_OPERANDS[n]
            if what is None:
                return Event(pc, cmd, explicit, [n], kind="fine"), rs
            if what == "ptr":
                return Event(pc, cmd, explicit, [n], ptr_at=q + 1 - pc,
                             ptr=self.u32(q + 1), kind="xwave"), rs
            return Event(pc, cmd, explicit, [n, self.u8(q + 1)],
                         kind="cmd"), rs
        if cmd == 0xCE:
            v = self.u8(q)
            return Event(pc, cmd, explicit, [v] if v < 0x80 else [],
                         kind="eot"), rs
        raise M4AError("command 0x%02X at 0x%08X" % (cmd, pc))

    def record_event(self, ev):
        old = self.m.events.get(ev.addr)
        if old is not None:
            if old.encode() != ev.encode():
                raise M4AError("event at 0x%08X decodes two ways" % ev.addr)
            return
        self.m.events[ev.addr] = ev

    def run_tracks(self):
        """Abstract execution of every track of every song header."""
        seen = set()
        work = []
        for _h, (_n, tone, parts) in sorted(self.m.headers.items()):
            for p in parts:
                # MPlayStart + MPlayMain's Clear64byte: running status 0,
                # no program, key 0, pattern level 0
                work.append((p, (), None, None, 0, tone))
        while work:
            state = work.pop()
            if state in seen:
                continue
            seen.add(state)
            pc, stack, rs, prog, key, vg = state
            ev, rs2 = self.decode(pc, rs)
            self.record_event(ev)
            self.m.rs_after.setdefault(pc, set()).add(rs2)
            nxt = pc + ev.size
            k = ev.kind
            if k in ("note", "eot"):
                if ev.args:
                    key = ev.args[0]
                if k == "note" and prog is not None:
                    self.m.note_uses.add((vg, prog, key))
                work.append((nxt, stack, rs2, prog, key, vg))
            elif k == "voice":
                prog = ev.args[0]
                self.m.vg_uses.setdefault(vg, set()).add(prog)
                work.append((nxt, stack, rs2, prog, key, vg))
            elif k in ("wait", "cmd"):
                work.append((nxt, stack, rs2, prog, key, vg))
            elif k == "fine":
                pass
            elif k == "goto":
                self.branch(ev, "GOTO (ply_goto)")
                work.append((ev.ptr, stack, rs2, prog, key, vg))
            elif k == "patt":
                self.branch(ev, "PATT (ply_patt)")
                if len(stack) < MAX_PATTERN_LEVEL:
                    work.append((ev.ptr, stack + (nxt,), rs2, prog, key, vg))
                else:
                    self.m.warnings.append("PATT at 0x%08X at level 3" % pc)
            elif k == "pend":
                if stack:
                    work.append((stack[-1], stack[:-1], rs2, prog, key, vg))
                else:
                    work.append((nxt, stack, rs2, prog, key, vg))
            elif k == "rept":
                self.branch(ev, "REPT (ply_rept)")
                work.append((ev.ptr, stack, rs2, prog, key, vg))
                if ev.args[0] != 0:
                    work.append((nxt, stack, rs2, prog, key, vg))
            elif k == "memacc_jump":
                self.branch(ev, "MEMACC jump (ply_memacc, src/m4a_ctrl.c)")
                work.append((ev.ptr, stack, rs2, prog, key, vg))
                work.append((nxt, stack, rs2, prog, key, vg))
            elif k == "xwave":
                if not self.ptr_ok(ev.ptr):
                    raise M4AError("XCMD xWAVE at 0x%08X: 0x%08X"
                                   % (pc, ev.ptr))
                self.add_slot(pc + ev.ptr_at, ev.ptr,
                              "m4a track: XCMD xWAVE operand (ply_xwave, "
                              "src/m4a_ctrl.c): a WaveData", "wave")
                self.m.waves.setdefault(ev.ptr, None)
                work.append((nxt, stack, rs2, prog, key, vg))
            else:
                raise M4AError("event kind %s" % k)
        self.m.track_states = len(seen)

    def branch(self, ev, what):
        if not self.ptr_ok(ev.ptr):
            raise M4AError("%s at 0x%08X: 0x%08X" % (what, ev.addr, ev.ptr))
        self.add_slot(ev.addr + ev.ptr_at, ev.ptr,
                      "m4a track: %s operand, a track position" % what,
                      "trackloc")

    # ---- voicegroups -------------------------------------------------------

    def tone_fields(self, a):
        return struct.unpack_from("<BBBBII", self.rom, self.off(a))

    def parse_tone(self, a, why, level):
        f = self.tone_fields(a)
        if a in self.m.tones:
            return f
        self.m.tones[a] = f
        self.add_object(a, a + TONE_SIZE, "tone", f)
        t, w, x = f[0], f[4], f[5]
        if t & 0xC0:
            if level:
                return f  # ply_note: such a sub-voice plays nothing
            if not self.ptr_ok(w):
                raise M4AError("%s: sub-voicegroup 0x%08X" % (why, w))
            self.m.sub_bases.add(w)
            if t & 0x40:
                self.add_slot(a + 4, w, "ToneData %s (key split): "
                              "sub-voicegroup (ply_note)" % why, "subvg")
                if not self.ptr_ok(x):
                    raise M4AError("%s: key-split table 0x%08X" % (why, x))
                self.add_slot(a + 8, x, "ToneData %s (key split): key-split "
                              "table (ply_note)" % why, "keysplit")
            else:
                self.add_slot(a + 4, w, "ToneData %s (rhythm): "
                              "sub-voicegroup (ply_note)" % why, "subvg")
            return f
        ch = t & 7
        if ch == 0:
            if w:
                if not self.ptr_ok(w):
                    raise M4AError("%s: WaveData 0x%08X" % (why, w))
                self.add_slot(a + 4, w, "ToneData %s (DirectSound): WaveData "
                              "(ply_note, MidiKeyToFreq)" % why, "wave")
                self.m.waves.setdefault(w, None)
        elif ch == 3:
            if not self.ptr_ok(w):
                raise M4AError("%s: wave 0x%08X" % (why, w))
            self.add_slot(a + 4, w, "ToneData %s (CGB wave): 16-byte wave "
                          "(CgbSound, src/m4a_cgb.c)" % why, "progwave")
            self.m.prog_waves.add(w)
        elif ch in (1, 2):
            if w > 3:
                raise M4AError("%s: square duty %d" % (why, w))
        elif ch == 4:
            if w > 1:
                raise M4AError("%s: noise period %d" % (why, w))
        else:
            raise M4AError("%s: CGB channel %d" % (why, ch))
        return f

    def tone_valid(self, a, level):
        """The 12 bytes at a are a ToneData the engine can play, whose
        pointer targets are already proven (the layout extension)."""
        t, _k, _l, _p, w, _x = self.tone_fields(a)
        if t & 0xC0:
            return level == 0 and w in self.m.sub_bases
        ch = t & 7
        if ch == 0:
            return w in self.m.waves
        if ch == 3:
            return w in self.m.prog_waves
        if ch in (1, 2):
            return w <= 3
        if ch == 4:
            return w <= 1
        return False

    def parse_voices(self):
        for vg, progs in sorted(self.m.vg_uses.items()):
            n = max(progs) + 1 if progs else 0
            self.m.vg_extent[vg] = [n, n]
            for prog in range(n):
                self.parse_tone(vg + TONE_SIZE * prog,
                                "voicegroup 0x%08X[%d]" % (vg, prog), 0)
        drum_keys = {}
        split_keys = {}
        for vg, prog, key in sorted(self.m.note_uses):
            t, _k, _l, _p, w, x = self.m.tones[vg + TONE_SIZE * prog]
            if t & 0x40:
                split_keys.setdefault((w, x), set()).add(key)
            elif t & 0x80:
                drum_keys.setdefault(w, set()).add(key)
        for (w, x), keys in sorted(split_keys.items()):
            lo, hi = min(keys), max(keys)
            self.m.ks_extent[x] = (lo, hi)
            self.add_object(x + lo, x + hi + 1, "keysplit", x)
            drum_keys.setdefault(w, set()).update(
                self.u8(x + k) for k in range(lo, hi + 1))
        for w, keys in sorted(drum_keys.items()):
            lo, hi = min(keys), max(keys)
            self.m.sub_extent[w] = (lo, hi)
            for k in range(lo, hi + 1):
                self.parse_tone(w + TONE_SIZE * k,
                                "sub-voicegroup 0x%08X[%d]" % (w, k), 1)
        for w in sorted(self.m.waves):
            self.parse_wave(w)
        for p in sorted(self.m.prog_waves):
            if p % 4:
                raise M4AError("wave 0x%08X is not 4-aligned" % p)
            self.add_object(p, p + PROG_WAVE_SIZE, "progwave")
        # layout extension up to the next proven object
        starts = sorted(set(self.m.objects) | set(self.m.labels))
        for vg in sorted(self.m.vg_extent):
            n = self.m.vg_extent[vg][0]
            if n == 0:
                continue
            end = vg + TONE_SIZE * n
            later = [a for a in starts if a >= end]
            if not later:
                continue
            gap = later[0] - end
            if gap == 0 or gap % TONE_SIZE or gap > 4 * TONE_SIZE:
                continue
            extra = gap // TONE_SIZE
            if all(self.tone_valid(end + TONE_SIZE * i, 0)
                   for i in range(extra)):
                for i in range(extra):
                    self.parse_tone(
                        end + TONE_SIZE * i,
                        "voicegroup 0x%08X[%d] (layout: the %d-entry gap to "
                        "the next proven object 0x%08X is valid ToneData)"
                        % (vg, n + i, extra, later[0]), 0)
                self.m.vg_extent[vg][1] = n + extra

    def parse_wave(self, w):
        if w % 4:
            raise M4AError("WaveData 0x%08X is not 4-aligned" % w)
        typ, status, freq, loop, size = struct.unpack_from(
            "<HHIII", self.rom, self.off(w))
        if typ not in (0, 1) or status not in (0, 0x4000, 0xC000):
            raise M4AError("WaveData 0x%08X: type 0x%X status 0x%X"
                           % (w, typ, status))
        if not 0 < size or w + WAVE_HEADER + size > ROM_BASE + len(self.rom):
            raise M4AError("WaveData 0x%08X: size %d" % (w, size))
        if status & 0x4000 and loop > size:
            raise M4AError("WaveData 0x%08X: loop %d > size %d"
                           % (w, loop, size))
        self.m.waves[w] = (typ, status, freq, loop, size)
        self.add_object(w, w + WAVE_HEADER, "wavehdr",
                        (typ, status, freq, loop, size))
        self.add_object(w + WAVE_HEADER, w + WAVE_HEADER + size, "pcm", w)

    # ---- the bytes between proven objects -------------------------------

    def intervals(self):
        return intervals(self.m)

    def fill_gaps(self):
        """Classify each gap between two proven objects: a sample's tail
        (the byte after its counted samples, then zeros to the next
        4-aligned object), zero padding after a track's FINE, or the
        never-executed rest of a track after its looping GOTO (decoded
        linearly; it must end with a FINE, then zeros)."""
        iv = self.intervals()
        end_of = {}
        for s, e, k in iv:
            end_of[e] = (s, k)
        gaps = []
        cur = iv[0][1]
        for s, e, _k in iv[1:]:
            if s > cur:
                gaps.append((cur, s))
            cur = max(cur, e)
        for g0, g1 in gaps:
            before = end_of.get(g0)
            why = "after nothing proven"
            if before is not None:
                bstart, bkind = before
                if bkind == "pcm":
                    if ((g0 + 4) & ~3) == g1 and all(
                            self.u8(a) == 0 for a in range(g0 + 1, g1)):
                        self.add_object(g0, g1, "pcmtail", bstart)
                        continue
                    why = "after a sample"
                elif bkind == "event":
                    ev = self.m.events[bstart]
                    if ev.kind == "fine" and self.zero_pad(g0, g1):
                        continue
                    if ev.kind == "goto":
                        rs = self.m.rs_after.get(bstart, set())
                        rs = sorted(rs)[0] if len(rs) == 1 else None
                        end = self.decode_dead(g0, g1, rs)
                        if end is not None and (
                                end == g1 or self.zero_pad(end, g1)):
                            continue
                    why = "after a track event"
                else:
                    why = "after a %s" % bkind
            self.m.unexplained.append((g0, g1, why))

    def zero_pad(self, g0, g1):
        if g1 % 4 == 0 and g1 - g0 <= 3 and all(
                self.u8(a) == 0 for a in range(g0, g1)):
            self.add_object(g0, g1, "pad")
            return True
        return False

    def decode_dead(self, pc, limit, rs):
        """Linear decode of never-executed track bytes from pc: every event
        must fit below limit and the run must end with a FINE; returns its
        end (and records the events) or None (and records nothing)."""
        evs = []
        while pc < limit:
            try:
                ev, rs = self.decode(pc, rs)
            except M4AError:
                return None
            if pc + ev.size > limit:
                return None
            evs.append(ev)
            pc += ev.size
            if ev.kind == "fine" and ev.cmd == 0xB1:
                for e in evs:
                    self.m.dead[e.addr] = e
                    if e.ptr_at is not None:
                        self.m.dead_ptrs.append(e)
                return pc
        return None

    def check_dead_ptrs(self):
        """A pointer operand in never-executed bytes is kept only if its
        target starts a decoded event (executed or not)."""
        for e in self.m.dead_ptrs:
            if e.ptr not in self.m.events and e.ptr not in self.m.dead:
                raise M4AError("never-executed %s at 0x%08X points at "
                               "0x%08X, no event" % (e.kind, e.addr, e.ptr))
            self.add_slot(e.addr + e.ptr_at, e.ptr,
                          "m4a track, never executed (the rest of the track "
                          "after its looping GOTO, decoded linearly to its "
                          "FINE): %s operand"
                          % CMD_NAMES.get(e.cmd, "0x%02X" % e.cmd),
                          "trackloc")


# --------------------------------------------------------------------------
# Round trip, coverage, names
# --------------------------------------------------------------------------

def intervals(m):
    """[(start, end, kind)] of everything parsed, by address."""
    iv = [(a, a + e.size, "event") for a, e in m.events.items()]
    iv += [(a, a + e.size, "dead") for a, e in m.dead.items()]
    iv += [(a, o[0], o[1]) for a, o in m.objects.items()]
    iv.sort()
    return iv


def reencode(rom, m, start, end, kind, fields):
    """Bytes of one object rebuilt from its parsed fields, or None for the
    asset bytes that only their extent describes (PCM, 4-bit waves)."""
    def word(a):
        return struct.unpack_from("<I", rom, a - ROM_BASE)[0]
    if kind == "songtable":
        count, stride = fields
        out = bytearray()
        for i in range(count):
            slot = start + i * stride
            out += struct.pack("<I", m.table_slots.get(slot, 0))
            out += rom[slot + 4 - ROM_BASE:slot + stride - ROM_BASE]
        return bytes(out)
    if kind == "song":
        n, blocks, prio, reverb, tone, parts = fields
        out = struct.pack("<BBBB", n, blocks, prio, reverb)
        if n:
            out += struct.pack("<I", tone)
            out += b"".join(struct.pack("<I", p) for p in parts)
        return out
    if kind == "tone":
        return struct.pack("<BBBBII", *fields)
    if kind == "wavehdr":
        return struct.pack("<HHIII", *fields)
    if kind == "pad":
        return bytes(end - start)
    if kind == "pcmtail":
        return rom[start - ROM_BASE:start + 1 - ROM_BASE] + bytes(end - start - 1)
    return None


def roundtrip(rom, m):
    """(bytes re-encoded, bytes covered only by extent, mismatches)."""
    enc = 0
    extent = 0
    bad = []
    for a, (e, k, f) in sorted(m.objects.items()):
        b = reencode(rom, m, a, e, k, f)
        if b is None:
            extent += e - a
            continue
        if b != rom[a - ROM_BASE:e - ROM_BASE]:
            bad.append((a, k))
        enc += e - a
    for evs in (m.events, m.dead):
        for a, ev in evs.items():
            if ev.encode() != rom[a - ROM_BASE:a + ev.size - ROM_BASE]:
                bad.append((a, ev.kind))
            enc += ev.size
    return enc, extent, bad


def check_overlaps(m):
    iv = [(a, a + e.size) for a, e in m.events.items()]
    iv += [(a, a + e.size) for a, e in m.dead.items()]
    iv += [(a, o[0]) for a, o in m.objects.items()]
    iv.sort()
    bad = []
    for (s0, e0), (s1, e1) in zip(iv, iv[1:]):
        if s1 < e0:
            bad.append((s0, s1))
    return bad


def label_names(m, taken=()):
    """addr -> generated name for every pointer target not in `taken`
    (addresses the config already names keep their names)."""
    out = {}
    for a, kinds in sorted(m.labels.items()):
        if a in taken:
            continue
        kind = [k for k in LABEL_ORDER if k in kinds][0]
        out[a] = "%s%08X" % (LABEL_PREFIX[kind], a)
    return out


def load_roots(cfg):
    ent = cfg.get("m4a")
    if not ent:
        return []
    out = []
    for t in ent["song_tables"]:
        if not t.get("why"):
            raise M4AError("m4a.song_tables entries need a \"why\"")
        out.append((int(t["start"], 16), int(str(t["count"]), 0),
                    int(str(t.get("stride", 8)), 0), t["why"]))
    return out


def split_plan(rom, cfg, taken=()):
    """What tools/split.py needs: (labels {addr: name}, slots {addr:
    (target, why)}); raises M4AError when the parse or its round trip
    fails, so a wrong ROM or config never reaches the emitter."""
    roots = load_roots(cfg)
    if not roots:
        return {}, {}
    m = Parser(rom, roots).parse()
    _enc, _ext, bad = roundtrip(rom, m)
    if bad:
        raise M4AError("m4a round trip fails at 0x%08X (%s)" % bad[0])
    ov = check_overlaps(m)
    if ov:
        raise M4AError("m4a objects overlap at 0x%08X/0x%08X" % ov[0])
    return label_names(m, taken), dict(m.slots)


def parse_segments(path):
    segs = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            p = line.split()
            segs.append((int(p[0], 16), int(p[1], 16), p[2], p[3]))
    return sorted(segs)


def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("--rom", default="baserom.gba")
    ap.add_argument("--config", default=None,
                    help="split_config.json (its \"m4a\" entry); default: "
                         "the two song tables above")
    ap.add_argument("--segments", default="docs/analysis/segments.txt")
    args = ap.parse_args()
    with open(args.rom, "rb") as f:
        rom = f.read()
    roots = DEFAULT_ROOTS
    if args.config:
        with open(args.config) as f:
            roots = load_roots(json.load(f)) or DEFAULT_ROOTS
    m = Parser(rom, roots).parse()
    enc, ext, bad = roundtrip(rom, m)
    ov = check_overlaps(m)
    print("song headers %d (%d tracks), voicegroups %d, sub-voicegroups %d, "
          "key-split tables %d, ToneData %d, WaveData %d, 4-bit waves %d"
          % (len(m.headers), sum(n for n, _t, _p in m.headers.values()),
             len(m.vg_extent), len(m.sub_extent), len(m.ks_extent),
             len(m.tones), len(m.waves), len(m.prog_waves)))
    print("track events: %d executed (%d abstract states), %d never "
          "executed" % (len(m.events), m.track_states, len(m.dead)))
    print("pointer words: %d (%d unaligned), labels: %d"
          % (len(m.slots), sum(1 for a in m.slots if a % 4), len(m.labels)))
    print("round trip: %d bytes re-encoded from fields, %d bytes by extent "
          "(PCM bodies, 4-bit waves), %d mismatches, %d overlaps"
          % (enc, ext, len(bad), len(ov)))
    for vg, (n, n2) in sorted(m.vg_extent.items()):
        print("  voicegroup 0x%08X: %d entries (highest program %d)%s"
              % (vg, n2, n - 1, " + %d by layout" % (n2 - n) if n2 > n else ""))
    for w, (lo, hi) in sorted(m.sub_extent.items()):
        print("  sub-voicegroup 0x%08X: keys %d-%d" % (w, lo, hi))
    try:
        segs = parse_segments(args.segments)
    except IOError:
        segs = []
    cover = {}
    for a, e, _k in intervals(m):
        for s0, s1, _kind, name in segs:
            lo, hi = max(a, s0), min(e, s1)
            if lo < hi:
                cover[name] = cover.get(name, 0) + hi - lo
    for s0, s1, _kind, name in segs:
        if name in cover:
            print("  %-46s 0x%08X-0x%08X: %7d of %7d bytes parsed"
                  % (name, s0, s1, cover[name], s1 - s0))
    for g0, g1, why in m.unexplained:
        print("  not parsed: 0x%08X-0x%08X (%d bytes, %s)"
              % (g0, g1, g1 - g0, why))
    for w in m.warnings:
        print("warning:", w)
    return 1 if bad or ov else 0


if __name__ == "__main__":
    sys.exit(main())
