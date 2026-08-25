#!/usr/bin/env python3
"""ROM-wide function/symbol database generator (issue #22).

Emits one record per identified function plus a caller->callee call graph,
using the census heuristics validated in docs/analysis/rom-map.md section 1
(dual-view disassembly technique, re-implemented here as direct Thumb/ARM
decoding of baserom.gba so the generator is deterministic and fast):

  - BL-target census: every Thumb `bl` pair whose site lies in the code span
    0x080000C0-0x080CFFFF contributes its target as a function entry; the
    ARM `bl` census covers only the arm_code segments (rom-map.md section 3).
  - ROM-pointer census: every word-aligned word in the ROM whose value points
    into the code span (bit 0 set = Thumb entry) contributes a function entry.
  - Prologue plausibility: a candidate must start with `push {.., lr}`, be an
    immediate terminator (bx rN / mov pc, lr / pop {.., pc} / b), or (BL
    targets only) reach a terminator in a short forward sweep.

Outputs (deterministic, sorted; commit the results):
  docs/analysis/symbols.csv   vma,size,isa,evidence,name
  docs/analysis/callgraph.csv caller,callee,kind,site,count

`size` is the entry->next-entry distance capped at 4 KiB (as in rom-map.md
section 3): an upper bound that includes trailing literal pools / padding.

Run inside the knidl-builder image via `make symbols`, or directly:
  python3 tools/symdb.py --rom baserom.gba
"""

import argparse
import bisect
import os
import struct
import sys

ROM_BASE = 0x08000000

# Code span used by the BL/pointer census (rom-map.md section 3: restricted to
# 0x080000C0-0x080CFFFF; rounded up to the 0x080D0000 code|data boundary).
CODE_SPAN_START = 0x080000C0
CODE_SPAN_END = 0x080D0000

# Entry->next-entry size cap (rom-map.md section 3).
MAX_SIZE = 0x1000

# Forward-sweep budget for the prologue plausibility check of BL targets
# (must reach past __divsi3's first unconditional branch at +0x30).
SWEEP_LIMIT = 0x100

# ARM-mode functions. The three ARM zones of rom-map.md section 3
# (crt0+master ISR, task-switch helpers, interworking veneer) are split by
# hand-validated boundaries; entry evidence (bl-target / rom-pointer) is
# still derived automatically from the censuses. Names marked None stay
# sub_XXXXXXXX until they are identified from sibling projects.
ARM_ENTRIES = [
    (0x080000C0, 0x48, "Start"),  # asm/crt0.s (ROM entry via header branch)
    (0x08000108, 0x108, "MasterIsr"),  # asm/crt0.s (copied to 0x03001030)
    (0x08000234, 0x24, None),  # task/context-switch helper 1 (stmfd sp!,{lr})
    (0x08000258, 0x30, None),  # helper 2: save sp/lr/r0, restore task sp
    (0x08000288, 0x20, None),  # helper 3: call task fn via ARM veneer
    (0x080002A8, 0x40, None),  # helper 4: task-done check + hang loop
    (0x080CFDDC, 0x08, None),  # ARM interworking veneer -> 0x08005655
]

# Canonical names, all previously validated in this repo (asm/crt0.s,
# src/agb_sram.c, asm/sdk_libc.s) or canonical SDK/BIOS-syscall names from
# sibling pret projects (katam / pokeemerald; SWI numbering per GBATEK).
KNOWN_SYMBOLS = {
    0x08000310: "AgbInit",  # rom-map.md section 2 seg 5 (crt0 literal 0x08000311)
    0x08007300: "AgbMain",  # rom-map.md section 4 (crt0 literal 0x08007301); the
    # ROM has no __gccmain call in its prologue, so the original source did
    # NOT name it `main` — AgbMain per SDK/pret convention (issue #33).
    # SDK SWI thunks (rom-map.md section 2 seg 8 / GBATEK SWI numbering)
    0x080CFA4C: "DummyFunc",  # bare `bx lr` (m4a-style no-op)
    0x080CFA50: "ArcTan2",  # svc 0x0A
    0x080CFA54: "CpuFastSet",  # svc 0x0C
    0x080CFA58: "CpuSet",  # svc 0x0B
    0x080CFA5C: "Div",  # svc 0x06
    0x080CFA60: "Mod",  # svc 0x06 + mov r0,r1 (returns remainder; syscall.h)
    0x080CFA68: "HuffUnComp",  # svc 0x13 (SDK order; verified insn bytes, #29)
    0x080CFA6C: "LZ77UnCompVram",  # svc 0x12
    0x080CFA70: "LZ77UnCompWram",  # svc 0x11
    0x080CFA74: "MultiBoot",  # r1=1; svc 0x25
    0x080CFA7C: "SoundDriverVSyncOff",  # svc 0x28
    # m4a/mp2k XCMD (extended command 0xCD) handlers, issue #29.  Evidence:
    # the 12-entry Thumb-pointer table at 0x0860A3E8 matches gXcmdTable of
    # katam (src/m4a_tables.c) / pokeemerald one-for-one — ply_xxx fills
    # indices 0 AND 3 in both — and each handler stores its argument byte
    # to exactly the MusicPlayerTrack field its name implies
    # (m4a_internal.h offsets: pseudoEchoVolume 0x1E, pseudoEchoLength
    # 0x1F, instrument.type 0x24, .length 0x26, .pan_sweep 0x27, .wav
    # 0x28, .attack 0x2C, .decay 0x2D, .sustain 0x2E, .release 0x2F;
    # track->cmdPtr 0x40).  None is ever BL-called (dispatched through the
    # table only), and most start with `ldr r0, [r1, #0x40]`, so the
    # strict pointer-candidate prologue filter would reject them — the
    # KNOWN_SYMBOLS bypass in build() accepts curated entries instead.
    # ------------------------------------------------------------------
    # m4a/mp2k sound engine (issue #31).  The engine occupies
    # 0x080CD89C-0x080CFA4B at the tail of game_code_and_rodata, split in
    # two halves exactly like pret sibling projects (pokeemerald/katam):
    #
    #   asm core ("m4a_1.s"): 0x080CD89C-0x080CE51F.  Identified by shape
    #   against pokeemerald m4a_1.s and by the 36-entry
    #   gMPlayJumpTableTemplate at 0x0860A140 (copied verbatim by
    #   MPlayJumpTableCopy, `movs r1, #36` loop), whose slots map commands
    #   0xB1..0xCF one-for-one to the handlers below (slots B6-B9/C6-C7/
    #   C9-CB/CD default to ply_fine, exactly like pokeemerald's template;
    #   MPlayExtender installs ply_memacc/ply_xcmd/ply_endtie at runtime).
    #   gScaleTable (0x0860A1D0), gFreqTable (0x0860A284) and gClockTable
    #   (0x0860A3B4) match pokeemerald's tables BYTE-FOR-BYTE (same engine
    #   revision).  SoundMain checks SOUND_INFO_PTR (0x03007FF0) against
    #   ID_NUMBER 'Smsh' (0x68736D53) and tail-jumps to 0x03007151: the
    #   mixer SoundMainRAM (ROM image 0x080CD930, with an embedded
    #   ARM-mode inner loop) is CpuSet-copied to IWRAM 0x03007150 by
    #   m4aSoundInit (0x400 bytes, literals at 0x080CE5B0/0x080CE5B4).
    0x080CD89C: "umul3232H32",  # adr r2; bx r2 -> ARM umull r2,r3,r0,r1
    0x080CD8AC: "SoundMain",  # ident lock, VCOUNT wrap (0xA0/+0xE4),
    # calls soundInfo->func/intp (0x20/0x24) and CgbSound (0x28)
    0x080CD930: "SoundMainRAM",  # ROM image; envelope loop + ARM mixer;
    # runs from IWRAM 0x03007150 (pointer 0x080CD931 in m4aSoundInit pool)
    0x080CDCD4: "SoundMainBTM",  # 16-word stmia zero-fill (template[35])
    0x080CDCEC: "RealClearChain",  # unlink chan: track 0x2C/prev 0x30/next 0x34
    0x080CDD0C: "ply_fine",  # template[0] (cmd B1); stop flag 0x40 + RealClearChain
    0x080CDD3C: "MPlayJumpTableCopy",  # movs r1,#36; copies 0x0860A140 template
    0x080CDD54: "ld_r3_r2",  # ldrb r3,[r2] + fall into chk_adr_r2 (descriptive)
    0x080CDD56: "chk_adr_r2",  # zeroes r3 unless r2 is a sane sample/ROM adr (descriptive)
    0x080CDD70: "ld_r3_tp_adr_i",  # fetch byte at track->cmdPtr++ (0x40) w/ check
    0x080CDD7C: "ply_goto",  # template[1]: assemble 4-byte LE target -> cmdPtr
    0x080CDD9C: "ply_patt",  # template[2]: push cmdPtr to patternStack (0x44+)
    0x080CDDB8: "ply_pend",  # template[3]: pop patternStack
    0x080CDDCC: "ply_rept",  # template[4]: repeat count via track->repN (0x03)
    0x080CDDFC: "ply_prio",  # template[9]: strb -> track->priority (0x1D)
    0x080CDE08: "ply_tempo",  # template[10]: tempoD 0x1C, tempoU 0x1E, tempoI 0x20
    0x080CDE1C: "ply_keysh",  # template[11]: strb -> track->keyShiftX (0x0A)
    0x080CDE30: "ply_voice",  # template[12]: 12-byte voicegroup entry -> 0x24/0x28/0x2C
    0x080CDE60: "ply_volu",  # template[13]: strb -> track->vol (0x12), flags |= 3
    0x080CDE74: "ply_pan",  # template[14]: -0x40 -> track->pan (0x14), flags |= 3
    0x080CDE88: "ply_bend",  # template[15]: -0x40 -> track->bend (0x0E), flags |= 0xC
    0x080CDE9C: "ply_bendr",  # template[16]: strb -> track->bendRange (0x0F), flags |= 0xC
    0x080CDEB0: "ply_lfodl",  # template[18]: strb -> track->lfoDelay (0x1B)
    0x080CDEBC: "ply_modt",  # template[20]: cmp/strb track->modT (0x18), flags |= 0xF
    0x080CDED4: "ply_tune",  # template[23]: -0x40 -> track->tune (0x0C), flags |= 0xC
    0x080CDEE8: "ply_port",  # template[27] (cmd CC): strb -> REG_SOUND1CNT_L+n (0x04000060)
    0x080CDF00: "m4aSoundVSync",  # (ident-ID)<=1 gate; DMA1/DMA2 FIFO restart
    # (0x040000BC pool; CNT_H 0x0400 then 0xB600)
    0x080CDF4C: "MPlayMain",  # player ident lock; calls player->func/intp
    # (0x38/0x3C); installed into soundInfo->func by MPlayOpen (pool word
    # 0x080CDF4D at 0x080CED10)
    0x080CE1B4: "TrackStop",  # template[31]; CgbOscOff via soundInfo+0x2C for type&7
    0x080CE1F8: "ChnVolSetAsm",  # chan L/R vol from velocity x (128+-pan) >> 14
    0x080CE228: "ply_note",  # gClockTable (0x0860A3B4) gate lookup, chan alloc by
    # prio, TrkVolPitSet + MidiKeyToFreq/MidiKeyToCgbFreq; SoundInit stores
    # soundInfo->plynote = 0x080CE229 (pool 0x080CEA44)
    0x080CE428: "ply_endtie",  # template[29]/extender: match midiKey (0x11), stop 0x40
    0x080CE468: "ClearModM_asm",  # asm-block copy of ClearModM (used by
    # ply_lfos/ply_mod); the C driver has its own static copy at 0x080CF6CC
    0x080CE484: "ld_r3_tp_adr",  # fetch byte at track->cmdPtr++, no check (descriptive)
    0x080CE490: "ply_lfos",  # template[17]: strb -> track->lfoSpeed (0x19), 0 -> ClearModM
    0x080CE4A4: "ply_mod",  # template[19]: strb -> track->mod (0x17), 0 -> ClearModM
    0x080CE4B8: "MidiKeyToFreq",  # clamp key 0xB2; gScaleTable 0x0860A1D0 +
    # gFreqTable 0x0860A284 interpolation via 2x umul3232H32
    #
    #   C driver ("m4a.c", old_agbcc -O1 expected): 0x080CE520-0x080CFA4B.
    #   Function order matches pokeemerald src/m4a.c; each identified by
    #   its literal pool (SOUND_INFO_PTR / ID_NUMBER / gSongTable
    #   0x0860B460 / gMPlayTable 0x0860B430 / IO regs) and field offsets.
    0x080CE520: "MPlayContinue",  # internal body: ident check, status &= ~0x80000000
    0x080CE53C: "MPlayFadeOut",  # internal body: fadeOI/OC (0x24/0x26) = speed, fadeOV = 0x100
    0x080CE55C: "m4aSoundInit",  # called from AgbInit; CpuSet-copies SoundMainRAM
    # to 0x03007150, then SoundInit(gSoundInfo=0x030056D0),
    # MPlayExtender(gCgbChans=0x03006710), m4aSoundMode(0x0095F700),
    # MPlayOpen loop over gMPlayTable (0x0860B430, 4 players),
    # info->memAccArea = gMPlayMemAccArea (0x030068D0)
    0x080CE5D4: "m4aSoundMain",  # bl SoundMain
    0x080CE5E0: "m4aSongNumStart",  # gSongTable 0x0860B460 + gMPlayTable -> MPlayStart
    0x080CE60C: "m4aSongNumStartOrChange",  # dead SDK export (no in-ROM ref)
    0x080CE658: "m4aSongNumStartOrContinue",
    0x080CE6AC: "m4aSongNumStop",  # songHeader match -> m4aMPlayStop
    0x080CE6E0: "m4aSongNumContinue",  # dead SDK export (no in-ROM ref)
    0x080CE714: "m4aMPlayAllStop",  # 4-player loop -> m4aMPlayStop
    0x080CE740: "m4aMPlayContinue",  # public wrapper -> MPlayContinue; dead export
    0x080CE74C: "m4aMPlayAllContinue",  # 4-player loop -> MPlayContinue
    0x080CE778: "m4aMPlayFadeOut",  # public wrapper -> MPlayFadeOut; dead export
    0x080CE788: "m4aMPlayFadeOutTemporarily",  # fadeOV = 0x101; dead export
    0x080CE7A8: "m4aMPlayFadeIn",  # fadeOV = 2, clears pause bit; dead export
    0x080CE7D0: "m4aMPlayImmInit",  # per started track: Clear64byte, bendRange=2,
    # volX=0x40, lfoSpeed=22, tone.type=1
    0x080CE818: "MPlayExtender",  # PSG reg init; installs ply_memacc/ply_lfos/
    # ply_mod/ply_xcmd/ply_endtie/SampleFreqSet slot/TrackStop/FadeOutBody/
    # TrkVolPitSet into gMPlayJumpTable (0x03006680) and CgbSound/CgbOscOff/
    # MidiKeyToCgbFreq into SoundInfo (pools 0x080CE8F4-0x080CE924)
    0x080CE930: "MusicPlayerJumpTableCopy",  # dead SDK export: 4-byte BIOS
    # thunk `swi 0x2A; bx lr` after MPlayExtender's pool, exactly katam's
    # `void MusicPlayerJumpTableCopy(void) { asm("swi 0x2A"); }` (issue #53)
    0x080CE934: "ClearChain",  # calls RealClearChain via jump table
    0x080CE948: "Clear64byte",
    0x080CE95C: "SoundInit",  # DMA1/2 reset, SOUNDCNT_H=0xA90E, BIAS resolution,
    # DMA SAD/DAD to pcmBuffer/FIFO_A+B, SOUND_INFO_PTR=soundInfo, CpuSet
    # clear (0x050003EC), plynote=ply_note, Cgb*=DummyFunc,
    # MPlayJumpTableCopy(0x03006680), SampleFreqSet, ident=ID_NUMBER
    0x080CEA54: "SampleFreqSet",  # gPcmSamplesPerVBlankTable 0x0860A2B4;
    # pcmFreq=(597275*n+5000)/10000; divFreq; TM0CNT setup; VCOUNT 0x9F sync
    0x080CEAF8: "m4aSoundMode",  # masks: FF reverb, F00 maxChans, F000 vol,
    # B00000 BIAS resolution, F0000 freq (-> m4aSoundVSyncOff + SampleFreqSet)
    0x080CEB90: "SoundClear",  # 12-chan clear + cgbChans off via CgbOscOff
    # (_call_via_r1); dead SDK export (no in-ROM ref)
    0x080CEBE4: "m4aSoundVSyncOff",  # (ident-ID)<=1 -> ident+=10, DMA1/2 off,
    # CpuSet-fill pcmBuffer (0x05000318)
    0x080CEC60: "m4aSoundVSyncOn",  # DMA1/2 CNT_H=0xB600, vsync counter=0, ident-=10
    0x080CEC9C: "MPlayOpen",  # trackCount clamp 16; Clear64byte(info); chains
    # soundInfo->func = MPlayMain (pool 0x080CDF4D @0x080CED10)
    0x080CED14: "MPlayStart",
    0x080CEDF8: "m4aMPlayStop",
    0x080CEE38: "FadeOutBody",  # extender slot [32]
    0x080CEF00: "TrkVolPitSet",  # extender slot [33]; called by ply_note
    0x080CEFB4: "MidiKeyToCgbFreq",  # gNoiseTable 0x0860A368, gCgbScaleTable
    # 0x0860A2CC, gCgbFreqTable 0x0860A350 (extender pool 0x080CE924)
    0x080CF05C: "CgbOscOff",  # extender pool 0x080CE920
    0x080CF0AC: "CgbModVol",  # static; bl-called from CgbSound only
    0x080CF114: "CgbSound",  # extender pool 0x080CE91C; PSG channel state machine
    0x080CF588: "m4aMPlayVolumeControl",  # trackBits; track->volX (0x13) = vol>>2
    0x080CF5F0: "m4aMPlayPitchControl",  # keyShiftX (0x0B) = pitch>>8, pitX (0x0D)
    0x080CF664: "m4aMPlayPanpotControl",  # track->panX (0x15); dead SDK export
    0x080CF6CC: "ClearModM",  # C-side static copy (see ClearModM_asm 0x080CE468)
    0x080CF6EC: "m4aMPlayModDepthSet",  # track->mod (0x17), 0 -> ClearModM; dead export
    0x080CF760: "m4aMPlayLFOSpeedSet",  # track->lfoSpeed (0x19), 0 -> ClearModM; dead export
    0x080CF7D4: "ply_memacc",  # 18-op switch over info->memAccArea (0x18);
    # conditional ops tail-call ply_goto via _call_via_r2 (extender slot [8])
    0x080CF92C: "ply_xcmd",  # dispatch via gXcmdTable 0x0860A3E8 (extender slot [28])
    0x080CF94C: "ply_xxx",    # xcmd 0x00/0x03: gMPlayJumpTable dispatch
    0x080CF960: "ply_xwave",  # xcmd 0x01: assemble instrument.wav pointer
    0x080CF9A8: "ply_xtype",  # xcmd 0x02: instrument.type
    0x080CF9BC: "ply_xatta",  # xcmd 0x04: instrument.attack
    0x080CF9D0: "ply_xdeca",  # xcmd 0x05: instrument.decay
    0x080CF9E4: "ply_xsust",  # xcmd 0x06: instrument.sustain
    0x080CF9F8: "ply_xrele",  # xcmd 0x07: instrument.release
    0x080CFA0C: "ply_xiecv",  # xcmd 0x08: pseudoEchoVolume
    0x080CFA18: "ply_xiecl",  # xcmd 0x09: pseudoEchoLength
    0x080CFA24: "ply_xleng",  # xcmd 0x0A: instrument.length
    0x080CFA38: "ply_xswee",  # xcmd 0x0B: instrument.pan_sweep.  Its tail
    # (0x080CFA40-0x080CFA4B) was the former sdk_swi_wrappers
    # gUnk_080cfa40: the old 0x080CFA40 segment boundary cut this handler
    # in half; issue #29 moved it to 0x080CFA4C.
    # SRAM driver (decompiled in src/agb_sram.c, issue #8)
    0x080CFA9C: "ReadSram_Core",
    0x080CFAC0: "ReadSram",
    0x080CFB24: "WriteSram",
    0x080CFB64: "VerifySram_Core",
    0x080CFB94: "VerifySram",
    0x080CFBF8: "WriteSramEx",
    # SDK libc (asm/sdk_libc.s; agbcc libgcc division helpers)
    0x080CFC30: "_call_via_r0",
    0x080CFC34: "_call_via_r1",
    0x080CFC38: "_call_via_r2",
    0x080CFC3C: "_call_via_r3",
    0x080CFC40: "_call_via_r4",
    0x080CFC44: "_call_via_r5",
    0x080CFC48: "_call_via_r6",
    0x080CFC4C: "_call_via_r7",
    0x080CFC6C: "__divsi3",
    0x080CFD00: "_div0",
    0x080CFD04: "__umodsi3",
    # Thumb->ARM task trampolines at the sdk_libc tail (issue #30).  Each
    # decodes as `bx pc; nop` + one raw ARM `b` into the seg-4 task helpers
    # (asm/task_switch_helpers.s): 0xEAFCC119 @0x080CFDC8 -> 0x08000234
    # (switch-to-task), 0xEAFCC120 @0x080CFDD0 -> 0x08000258 (yield-back),
    # 0xEAFCC12A @0x080CFDD8 -> 0x08000288 (dispatcher call).  Names follow
    # the helper semantics documented in docs/analysis/rom-map.md section 6.
    0x080CFDC4: "TaskSwitchTrampoline",
    0x080CFDCC: "TaskYieldTrampoline",
    0x080CFDD4: "TaskDispatchTrampoline",
}

# Curated false positives: candidate addresses whose only evidence is
# coincidental data, rejected before the prologue filter (issue #30).
# 0x080CFCFC is the `pop {pc}` tail of __divsi3's division-by-zero path
# (gcc 2.9 lib1funcs.asm Ldiv0: push {lr}; bl __div0; mov r0, #0; pop {pc})
# — the interior of __divsi3, not a function entry.  Its single
# "rom-pointer" reference (word 0x080CFCFD at 0x086DA494) sits inside the
# m4a_songs data segment surrounded by signed 8-bit PCM sample bytes; the
# `pop {pc}` halfword passes the strict terminator check by accident.
FALSE_POSITIVES = {
    0x080CFCFC,
    # 0x0800315E is literal-pool data inside sub_08003110, not a function
    # (issue #32): the mask word 0xFFFFF7FF at 0x0800315C has a low half that
    # decodes as a `bl`, which fooled the bl-target heuristic.  sub_08003110
    # really runs 0x08003110-0x08003184 and 0x08003164 is a branch target
    # inside it.
    0x0800315E,
}

# Curated Thumb entries with NO in-ROM reference (issue #31): dead m4a SDK
# exports.  The m4a driver was linked as whole objects, so public functions
# this game never calls (and that no ROM word points at) are still present
# in the binary.  Each address was hand-verified against the pokeemerald
# m4a.c function order and body shape (see the KNOWN_SYMBOLS comments);
# they are injected as candidates and carry the "curated" evidence kind.
EXTRA_THUMB_ENTRIES = {
    0x080CE60C,  # m4aSongNumStartOrChange
    0x080CE6E0,  # m4aSongNumContinue
    0x080CE740,  # m4aMPlayContinue (wrapper)
    0x080CE778,  # m4aMPlayFadeOut (wrapper)
    0x080CE788,  # m4aMPlayFadeOutTemporarily
    0x080CE7A8,  # m4aMPlayFadeIn
    0x080CE930,  # MusicPlayerJumpTableCopy (swi 0x2A thunk, issue #53)
    0x080CEB90,  # SoundClear
    0x080CF664,  # m4aMPlayPanpotControl
    0x080CF6EC,  # m4aMPlayModDepthSet
    0x080CF760,  # m4aMPlayLFOSpeedSet

    # game_code_early dead exports (issue #32).  Same whole-object-linking
    # cause as the m4a ones above, but in game code: each sits INSIDE the
    # census size of its predecessor, so without these entries symbols.csv
    # reports one oversized function where the ROM has two.  Every address
    # was confirmed by decompiling the range and byte-matching it.
    0x08001460,  # SetHBlankHandler   (inside sub_080013f8's 0x90)
    0x080014BC,  # SetVCountHandler   (inside sub_08001488's 0x60)
    0x0800151C,  # forced-blank on    (inside sub_08001518's 0x64)
    0x08001560,  # forced-blank off   (inside sub_08001518's 0x64)
    0x08002104,  # fade variant       (inside sub_080020b8's 0x94)
    0x08002220,  # fade variant       (inside sub_080021dc's 0x8C)
    0x08002358,  # debug-code writer  (inside sub_08002348's 0x30)
    0x080030B8,  # scalar colour blend(inside sub_08003014's 0xFC)
    0x080034A0,  # SE stop helper     (inside sub_08003484's 0x4C)
    0x080034B8,  # SE stop helper     (inside sub_08003484's 0x4C)
    0x080036B8,  # BGM fade-in helper (inside sub_08003688's 0xC4)
    0x080037A4,  # SE volume setter   (inside sub_08003770's 0x88)
    0x08005618,  # SetAllTaskSkipMasks(inside sub_080055c4's 0x90)
}

EVIDENCE_KINDS = ("bl-target", "rom-pointer", "prologue-scan", "curated")


def u16(rom, off):
    return struct.unpack_from("<H", rom, off)[0]


def u32(rom, off):
    return struct.unpack_from("<I", rom, off)[0]


def push_lr(hw):
    """Thumb `push {rList, lr}` (0xB500-0xB5FF)."""
    return 0xB500 <= hw <= 0xB5FF


def thumb_terminator(hw):
    """Thumb instructions that unconditionally end a function body:
    bx rN (incl. bx lr / bx pc), mov pc, lr, pop {.., pc}, b (uncond.)."""
    if hw & 0xFF87 == 0x4700:  # bx rN
        return True
    if hw == 0x46F7:  # mov pc, lr
        return True
    if hw & 0xFF00 == 0xBD00:  # pop {.., pc}
        return True
    if hw & 0xF800 == 0xE000:  # b (unconditional; cond fields 0xDxxx excluded)
        return True
    return False


def parse_segments(path):
    """Return [(start, end, kind, name)] from docs/analysis/segments.txt."""
    segs = []
    with open(path) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            parts = line.split()
            segs.append((int(parts[0], 16), int(parts[1], 16), parts[2], parts[3]))
    return segs


def bl_census(rom, arm_ranges):
    """Scan for BL instructions.

    Thumb: sources restricted to the code span (rom-map.md section 3); the
    BL prefix/suffix pair is decoded directly. ARM: only the arm_code
    segments are scanned — decoding the Thumb-dominated rest of the ROM in
    ARM view yields huge amounts of bogus `bl` words (rom-map.md section 3
    reports exactly one ARM bl in the whole ROM, at 0x08000290).

    Returns [(site, target)] with absolute VMAs; targets may lie outside the
    census span and are filtered by the caller.
    """
    edges = []
    end = min(len(rom), CODE_SPAN_END - ROM_BASE) - 4
    for off in range(CODE_SPAN_START - ROM_BASE, end, 2):
        hw1 = u16(rom, off)
        if not 0xF000 <= hw1 <= 0xF7FF:  # BL prefix (11110 offset_high[10:0])
            continue
        hw2 = u16(rom, off + 2)
        # BL suffix: bits [15:11] = 11111 (0xF800-0xFFFF). The offset field is
        # hw1[10:0]:hw2[10:0]:0 (23 bits, sign at bit 22); BLX(2) suffixes
        # (0xE800-0xEFFF) do not exist on ARMv4T and are excluded.
        if not 0xF800 <= hw2 <= 0xFFFF:
            continue
        offset = ((hw1 & 0x7FF) << 12) | ((hw2 & 0x7FF) << 1)
        if offset & 0x400000:  # sign-extend from bit 22
            offset -= 0x800000
        target = ROM_BASE + off + 4 + offset
        edges.append((ROM_BASE + off, target))

    for start, stop in arm_ranges:
        for off in range(start - ROM_BASE, stop - ROM_BASE - 3, 4):
            w = u32(rom, off)
            if (w & 0x0F000000) != 0x0B000000 or (w & 0xF0000000) == 0xF0000000:
                continue
            imm = w & 0xFFFFFF
            if imm & 0x800000:
                imm -= 0x1000000
            edges.append((ROM_BASE + off, ROM_BASE + off + 8 + (imm << 2)))
    return edges


def pointer_census(rom, arm_ranges):
    """Scan every word-aligned word for pointers into the code span.

    Odd pointers are Thumb entry candidates; even pointers are accepted only
    when they land exactly on an ARM zone AND the referencing word itself
    sits inside the code span (ARM entries here are only referenced from
    code literal pools; even words in the far data zones matching ARM
    addresses are coincidences). Returns [(site, target, is_thumb_ptr)].
    """
    refs = []
    for off in range(0, len(rom) - 3, 4):
        w = u32(rom, off)
        if not CODE_SPAN_START <= w < CODE_SPAN_END:
            continue
        if w & 1:
            refs.append((ROM_BASE + off, w & ~1, True))
        elif (
            CODE_SPAN_START <= ROM_BASE + off < CODE_SPAN_END
            and any(start <= w < end for start, end in arm_ranges)
        ):
            refs.append((ROM_BASE + off, w, False))
    return refs


def plausible_thumb_entry(rom, vma, hard_end, strict):
    """Prologue plausibility check for a Thumb candidate at `vma` (a file
    offset relative to ROM_BASE). `hard_end` stops the sweep at the next
    known candidate entry.

    strict (pointer-only candidates): require `push {.., lr}` or an immediate
    terminator (`bx rN` thunk / `bx lr` leaf such as the default IRQ handler
    / `bx pc` veneer) — odd pointers into the rodata interleaved with the
    code are common and only instruction-shaped entries keep precision.
    non-strict (BL targets): the call itself is strong evidence, so any
    body reaching an unconditional terminator within SWEEP_LIMIT passes.
    """
    off = vma - ROM_BASE
    hw0 = u16(rom, off)
    if push_lr(hw0) or thumb_terminator(hw0):
        return True
    if strict:
        return False
    limit = min(off + SWEEP_LIMIT, hard_end, len(rom) - 2)
    off += 2
    while off < limit:
        if thumb_terminator(u16(rom, off)):
            return True
        off += 2
    return False


def build(rom, segments):
    arm_ranges = [(s, e) for s, e, kind, _ in segments if kind == "arm_code"]

    bl_edges = bl_census(rom, arm_ranges)
    ptr_refs = pointer_census(rom, arm_ranges)

    bl_targets = {}
    for site, target in bl_edges:
        bl_targets.setdefault(target, []).append(site)
    # Pointer targets, split by the interworking bit: bit 0 set references
    # Thumb entries, bit 0 clear references ARM entries. A mismatched parity
    # (e.g. a coincidental odd data word equal to an ARM entry address) is
    # ignored for both evidence and call-graph edges.
    ptr_targets = {}
    for site, target, is_thumb_ptr in ptr_refs:
        if not is_thumb_ptr:
            continue
        ptr_targets.setdefault(target, []).append(site)
    ptr_targets_arm = {}
    for site, target, is_thumb_ptr in ptr_refs:
        if is_thumb_ptr:
            continue
        ptr_targets_arm.setdefault(target, []).append(site)

    # Thumb candidates: BL targets union bit0-set pointer targets, restricted
    # to the code span, excluding the ARM zones, validated by prologue shape.
    candidates = {}
    for target in set(bl_targets) | set(ptr_targets) | EXTRA_THUMB_ENTRIES:
        if target in FALSE_POSITIVES:
            continue
        if not CODE_SPAN_START <= target < CODE_SPAN_END:
            continue
        if target & 1 or any(s <= target < e for s, e in arm_ranges):
            continue
        if target - ROM_BASE + 1 >= len(rom):
            continue
        candidates[target] = None
    order = sorted(candidates)
    for i, target in enumerate(order):
        nxt = order[i + 1] if i + 1 < len(order) else CODE_SPAN_END
        strict = target not in bl_targets
        # Curated identifications (KNOWN_SYMBOLS) are accepted directly:
        # the m4a XCMD handlers are table-dispatched only and open with
        # `ldr r0, [r1, #0x40]`, which no generic prologue filter admits.
        if target in KNOWN_SYMBOLS or plausible_thumb_entry(
            rom, target, nxt - ROM_BASE, strict
        ):
            candidates[target] = nxt
    thumb_entries = {t: n for t, n in candidates.items() if n is not None}

    # ARM entries: curated boundaries; evidence merged from the censuses.
    arm_entries = {}
    for vma, size, _ in ARM_ENTRIES:
        arm_entries[vma] = vma + size

    # Boundaries for every accepted entry, used for size computation and for
    # attributing call-graph sites to the containing function. Sizes run to
    # the next ACCEPTED entry (rejected candidates must not truncate sizes).
    all_entries = sorted(list(thumb_entries) + list(arm_entries))
    sizes = {}
    for i, vma in enumerate(all_entries):
        nxt = all_entries[i + 1] if i + 1 < len(all_entries) else CODE_SPAN_END
        if vma in arm_entries:
            end = arm_entries[vma]
        else:
            end = min(nxt, vma + MAX_SIZE)
        sizes[vma] = max(0, min(end, vma + MAX_SIZE) - vma)

    def containing(site):
        i = bisect.bisect_right(all_entries, site) - 1
        if i < 0:
            return None
        vma = all_entries[i]
        return vma if site < vma + sizes[vma] else None

    # ---- symbol records ----------------------------------------------------
    symbols = []
    for vma in all_entries:
        is_arm = vma in arm_entries
        ev = []
        if vma in bl_targets:
            ev.append("bl-target")
        if vma in (ptr_targets_arm if is_arm else ptr_targets):
            ev.append("rom-pointer")
        if not ev:
            ev.append("curated" if vma in EXTRA_THUMB_ENTRIES else "prologue-scan")
        name = KNOWN_SYMBOLS.get(vma)
        if is_arm:
            name = dict((a, n) for a, s, n in ARM_ENTRIES).get(vma, name)
        if name is None:
            name = "sub_%08x" % vma
        isa = "arm" if is_arm else "thumb"
        symbols.append((vma, sizes[vma], isa, "+".join(ev), name))

    # ---- call graph --------------------------------------------------------
    edges = {}

    def add_edge(caller, callee, kind, site):
        key = (caller or 0, callee, kind)
        if key not in edges:
            edges[key] = [site, 0]
        edges[key][1] += 1

    for site, target in bl_edges:
        if target not in sizes:
            continue
        add_edge(containing(site), target, "bl", site)
    for site, target, is_thumb_ptr in ptr_refs:
        if target not in sizes:
            continue
        if is_thumb_ptr == (target in arm_entries):
            continue  # interworking-bit / ISA mismatch: coincidence
        add_edge(containing(site), target, "ptr", site)

    callgraph = [
        (caller, callee, kind, site, count)
        for (caller, callee, kind), (site, count) in sorted(
            edges.items(), key=lambda kv: (kv[0][2], kv[0][0] or 0, kv[0][1])
        )
    ]
    return symbols, callgraph, thumb_entries, arm_entries


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--rom", default="baserom.gba")
    parser.add_argument(
        "--segments", default="docs/analysis/segments.txt",
        help="machine-readable segment table (kinds arm_code etc.)",
    )
    parser.add_argument("--out-dir", default="docs/analysis")
    args = parser.parse_args()

    with open(args.rom, "rb") as f:
        rom = f.read()
    if len(rom) & 3:
        sys.exit("error: ROM size is not word-aligned")

    segments = parse_segments(args.segments)

    symbols, callgraph, thumb_entries, arm_entries = build(rom, segments)

    sym_path = os.path.join(args.out_dir, "symbols.csv")
    with open(sym_path, "w") as f:
        f.write("vma,size,isa,evidence,name\n")
        for vma, size, isa, ev, name in symbols:
            f.write("0x%08X,0x%X,%s,%s,%s\n" % (vma, size, isa, ev, name))

    graph_path = os.path.join(args.out_dir, "callgraph.csv")
    with open(graph_path, "w") as f:
        f.write("caller,callee,kind,site,count\n")
        for caller, callee, kind, site, count in callgraph:
            f.write(
                "0x%08X,0x%08X,%s,0x%08X,%d\n" % (caller, callee, kind, site, count)
            )

    def count_ev(pred):
        return sum(1 for _, _, _, ev, _ in symbols if pred(ev))

    print("wrote %s (%d functions: %d thumb, %d arm)" % (
        sym_path, len(symbols), len(thumb_entries), len(arm_entries)))
    print("  evidence: %d bl-target, %d rom-pointer, %d prologue-scan" % (
        count_ev(lambda e: "bl-target" in e),
        count_ev(lambda e: "bl-target" not in e and "rom-pointer" in e),
        count_ev(lambda e: e == "prologue-scan")))
    print("wrote %s (%d edges: %d bl, %d ptr)" % (
        graph_path, len(callgraph),
        sum(1 for e in callgraph if e[2] == "bl"),
        sum(1 for e in callgraph if e[2] == "ptr")))


if __name__ == "__main__":
    main()
