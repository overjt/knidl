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
    # --- M38 (issue #100), a lesson 4.40 phantom ---
    # 0x080CD5AE is not a function: it is the second half of sub_080cd330
    # (AgbMain state 12's first call), whose `strh r0, [r1]` at 0x080CD5AC
    # falls into it and whose `ldr`s at 0x080CD580/0x080CD588/0x080CD5A4
    # load pool words behind it (0x080CD5C8-0x080CD5D0).  Its only evidence
    # is the phantom `bl` of the pool word 0xFFFFF000 at 0x080CC5AC (in
    # sub_080cc2e0's pool), whose target is 0x080CC5B0 + 0xFFE.  sub_080cd330
    # really runs 0x080CD330-0x080CD674 (0x344, was 0x27E).
    0x080CD5AE,
    # --- M37 (issue #98), two `b.n` arm tails (lesson 4.95) ---
    # 0x080C501E is `b.n 0x080C5030` followed by the pool word 0xFFFFFF00
    # (0x080C5020) that sub_080c4f60's `ldr` at 0x080C5008 loads; the `subs`
    # at 0x080C501C falls into it and the `beq` at 0x080C5010 lands on
    # 0x080C5024, inside the row, so sub_080c4f60 really runs
    # 0x080C4F60-0x080C51C0.  0x080C51FE is `b.n 0x080C521E` followed by the
    # three pool words (0x080C5200-0x080C5208) of the push-less function
    # 0x080C51D4 (added below), whose `beq`s at 0x080C51F6/0x080C51FC land on
    # 0x080C520C/0x080C521A behind them, so 0x080C51D4 runs to 0x080C523C.
    # Their "rom-pointer" evidence is coincidental words in data: 0x080C501F
    # at 0x08302440, 0x080C51FF at 0x0824C41C and 0x08257CF0.
    0x080C501E,
    0x080C51FE,
    # --- M15 (issue #89), eight lesson 4.40 phantoms ---
    # None of these is a function.  Each one's only evidence is a phantom
    # `bl`: the pool word 0xFFFFF000 (halfwords F000/FFFF) at a 4-aligned
    # address in an earlier M15 function's pool, whose "target" is that
    # address + 4 + 0xFFE.  Each sits right after a body whose census size is
    # not a multiple of 4, and that body's last instruction falls into it.
    # Pool word / real owner (all in task type #7's 49-entry anchor table
    # gUnk_0873B928 or its companions):
    # 0x0805613A <- 0x08055138: the tail of the push-less companion
    #   0x080560FC (added below), which runs 0x080560FC-0x0805614C.
    # 0x080572F6 <- 0x080562F4: entry 30 sub_0805710c, whose `bl
    #   sub_080064dc` at 0x080572F4 returns into it (0x0805710C-0x080573A4).
    # 0x080575B6 <- 0x080565B4: entry 32 sub_08057494; its ten-entry `mov pc`
    #   jump table at 0x080574E0 sends cases 2-9 past it
    #   (0x08057494-0x08057A10).
    # 0x08057C46 <- 0x08056C44: entry 34 sub_08057ad4 (0x08057AD4-0x08057C98).
    # 0x08058E8E <- 0x08057E8C: the companion sub_08058e80, whose `ldr` at
    #   0x08058E82 loads the pool word 0x08058F08 behind it
    #   (0x08058E80-0x08058F10).
    # 0x08058F8E <- 0x08057F8C: entry 41 sub_08058f10, whose `b.n`s at
    #   0x08058F8A/0x08058F8C branch past it (0x08058F10-0x080594E0).
    # 0x0805980E <- 0x0805880C: entry 42 sub_08059570; its seven-entry jump
    #   table at 0x080595A8 sends cases 4-6 past it (0x08059570-0x08059AAC).
    # 0x0805A366 <- 0x08059364 (inside the 0x08058F8E phantom, really
    #   sub_08058f10's pool): entry 45 sub_0805a358, whose `ldr`s at
    #   0x0805A35A-0x0805A362 load pool words behind it (0x0805A358-0x0805A508).
    0x0805613A,
    0x080572F6,
    0x080575B6,
    0x08057C46,
    0x08058E8E,
    0x08058F8E,
    0x0805980E,
    0x0805A366,
    # --- M38 (issue #100), found by M14's census sweep ---
    # 0x080CCAA6 is not a function: it is the middle of the yield script
    # sub_080cc768 (`ldr r1, [r5]` with r4/r5/r7 and the stack frame already
    # set up, no prologue; the `bl TaskYieldTrampoline` at 0x080CCAA2 falls
    # into it).  Its only evidence is the lesson 4.40 phantom `bl`: the pool
    # word 0xFFFFF000 at 0x080CBAA4 decodes as F000/FFFF, whose target is
    # 0x080CBAA8 + 0xFFE.  sub_080cc768 really runs 0x080CC768-0x080CCD10.
    0x080CCAA6,
    # --- M14 (issue #90), a long-jump exit and two `b.n` arm tails ---
    # 0x0804D6C6 is not a function: it is the exit of the player action
    # sub_0804cc7c (entry 55 of the "enter" table gUnk_0873A748),
    # `bl sub_08006138; pop {r4, r5}; pop {r0}; bx r0`, the epilogue that
    # pairs with sub_0804cc7c's `push {r4, r5, lr}`.  The code in front of it
    # (the `strh` at 0x0804D6C4) falls into it, and its only "callers" are
    # the out-of-range `bls` + long `bl` at 0x0804CCD2 (the default of the
    # function's own eleven-way `switch (Task.unk73)`, jump table at
    # 0x0804CCE4) and a `b.n` at 0x0804D1BA, both inside its own row
    # (lessons 4.39/4.96).  sub_0804cc7c really runs 0x0804CC7C-0x0804D6D0.
    0x0804D6C6,
    # 0x0804F6FA and 0x08051008 are `b.n` arm tails (lesson 4.95).
    # 0x0804F6FA is `b.n 0x0804F756` right after sub_0804f614's pool word
    # 0x00000CEA; the `bl TaskYieldTrampoline` at 0x0804F6F6 falls into it
    # and sub_0804f614's six-entry `mov pc` jump table at 0x0804F660 sends
    # cases 4 and 5 to 0x0804F700, inside the row, so sub_0804f614 really
    # runs 0x0804F614-0x0804F76C.  0x08051008 is `b.n 0x08051028` after
    # sub_08050f80's `movs r0, #9` (0x08051006), and that function's jump
    # table at 0x08050FB0 sends cases 4 and 5 to 0x0805100A, so
    # sub_08050f80 really runs 0x08050F80-0x08051124.  Their "rom-pointer"
    # evidence is one coincidental word each, 0x0804F6FB at 0x086B361C and
    # 0x08051009 at 0x08692178, in graphics data.
    0x0804F6FA,
    0x08051008,
    # --- M13 (issue #88), three long-jump targets inside two big actions ---
    # 0x080491EC and 0x080493D2 are not functions: they are code of the
    # player action sub_08047fe8 (entry 29 of the "enter" table
    # gUnk_0873A748), whose 25-entry `mov pc` jump table at 0x080483B8 (a
    # `switch` on the ability PlayerState.unk0D - 1) sends ability 11 to
    # 0x080493D2 and ability 18 to 0x080491FC.  0x080491EC is the
    # `b.n 0x080493D2` that ends the arm in front of it (the
    # `bl TaskYieldTrampoline` at 0x080491E8 falls into it), followed by a
    # pad and three of that arm's pool words (0x03002490, 0xC48, 0xC4D);
    # its "rom-pointer" evidence is three coincidental words 0x080491ED in
    # graphics data (0x082D8480, 0x082D84D0) and in m4a_songs (0x085EF808).
    # 0x080493D2 (`unk88->unk04 = 19;` ... `sub_08006138();` and the
    # `pop {r4-r7}; pop {r0}` epilogue that pairs with sub_08047fe8's
    # `push {r4-r7, lr}`) is also reached by seven long `bl` jumps from the
    # other arms (0x0804842A and six more; a Thumb `b.n` cannot reach it,
    # lesson 4.39), which the census took for calls.  sub_08047fe8 really
    # runs 0x08047FE8-0x08049484 (0x149C; symbols.csv records the census's
    # MAX_SIZE cap, 0x1000, lesson 4.90).
    0x080491EC,
    0x080493D2,
    # 0x0804B8A0 is the loop head of the player action sub_0804b858 (entry
    # 53 of gUnk_0873A748; the pool word at 0x0804C600 that re-binds the
    # coroutine points at it too): sub_0804b858's `ldr r1, [pc]` at
    # 0x0804B85A loads the word at 0x0804B8B8 behind it, its last store at
    # 0x0804B89E falls into it, and its only "caller" is the long `bl` at
    # 0x0804C49E at the bottom of its own row, a jump back to the top of the
    # eight-way `switch (Task.unk73)` (jump table at 0x0804B8C0, whose
    # out-of-range `bhi` also branches back to 0x0804B8A0).
    # sub_0804b858 really runs 0x0804B858-0x0804C4AC (0xC54).
    0x0804B8A0,
    # --- M12 (issue #87) ---
    # 0x08044A72 is not a function: it is the upper halfword of the fourth
    # word of sub_080449c8's own eight-entry `mov pc` jump table
    # (0x08044A64-0x08044A83, the `switch` on Task.unk3C - 0x36B), in the
    # middle of the table, and the code after the table
    # (0x08044A84-0x08044B93) is that switch's arms, the shared tail and the
    # epilogue.  Its only evidence is the lesson 4.40 phantom `bl`: the pool
    # word 0xFFFFF000 at 0x08043A70 (inside M11's sub_0804374c, landed C that
    # never calls it) decodes as the pair F000/FFFF, whose target is
    # 0x08043A74 + 0xFFE.  sub_080449c8 is handler 30 of gUnk_0873A840 and
    # really runs 0x080449C8-0x08044B94 (0x1CC).
    0x08044A72,
    # --- M10 (issue #91), four long-jump targets inside two big actions ---
    # 0x08037F2A, 0x08038F8E and 0x08038FD8 are not functions: they are the
    # loop head, one arm and the shared exit of the 4368-byte player action
    # sub_08037ed8 (entry 16 of the "enter" table gUnk_0873A748).
    # 0x08037F2A is the top of its seven-state `switch (Task.unk73)` loop,
    # reached by the `beq`/`bne` at 0x08037F06/0x08037F24 of the function's
    # own head and by the long `bl` at 0x08038F98 (a Thumb `b.n` cannot reach
    # it, lesson 4.39); its first pool load reads 0x08037F48, which
    # sub_08037ed8's head also loads.  0x08038F8E (`t->unk73 = 6;` and that
    # long `bl` back to the loop head) is reached only by the long `bl` at
    # 0x08038436.  0x08038FD8 is `bl sub_08006138` + the `pop {r3}; mov r8, r3;
    # pop {r4-r6}; pop {r0}` epilogue that pairs with sub_08037ed8's
    # `push {r4-r6, lr}; mov r6, r8; push {r6}`, reached by the long `bl` at
    # 0x08037F38 (the switch's default) and four `b.n`s.
    # sub_08037ed8 really runs 0x08037ED8-0x08038FE8 (0x1110; symbols.csv
    # records the census's MAX_SIZE cap, 0x1000).
    0x08037F2A,
    0x08038F8E,
    0x08038FD8,
    # 0x0803AA14 is the exit tail of sub_08039c24 (entry 21): `t->unk28++;
    # sub_08006138();` + the `pop {r4-r7}; pop {r0}` epilogue, reached by
    # three long `bl`s from inside the body (0x08039FC0, 0x08039FD8,
    # 0x0803A1B6) and by the fall-through from the `bl sub_08049738` at
    # 0x0803AA10.  Six of sub_08039c24's pool loads read the words at
    # 0x0803AA28-0x0803AA3B after it.  sub_08039c24 really runs
    # 0x08039C24-0x0803AA40 (0xE1C).
    0x0803AA14,
    # --- M07 (issue #93) ---
    # 0x0802589E is not a function: it is the shared epilogue of the
    # 2170-byte sub_08025024 (`pop {r3-r5}; mov r8-sl; pop {r4-r7};
    # pop {r1}; bx r1`), which the body reaches by falling through from
    # 0x0802589C and by the long `bl` jump at 0x0802503A (a Thumb `b.n`
    # cannot reach it, lesson 4.39).  Thirteen of sub_08025024's pool loads
    # (0x08025834-0x08025898) read the words at 0x080258AC-0x080258DF right
    # after it.  0x08025898 (`ldr r4, =0x02000030; ldrh r0, [r4]; lsrs r0,
    # #8`, the return-value load that falls into that epilogue) is the
    # target of the second long jump, the `bl` at 0x08025076.
    # sub_08025024 really runs 0x08025024-0x080258E0 (0x8BC).
    0x08025898,
    0x0802589E,
    # --- M06 (issue #84) ---
    # 0x08021B0E is not a function: it is the `bx r1` that completes
    # sub_08021ab4's epilogue (`pop {r4-r6}; pop {r1}; bx r1`), followed by
    # that function's two literal-pool words (0x03005660, 0x087328F0).  The
    # census split it off because the pool words that follow look like data
    # after a terminator; nothing in the ROM references 0x08021B0E.  Same
    # shape as the former sub_080cfcfc (`pop {pc}` tail of __divsi3).
    0x08021B0E,
    # --- M04 (issue #82), two rows the reachability sweep dropped ---
    # Both are the same shape as M05's 0x0801A41A below and share its cause:
    # the pool word 0xFFFFF000 decodes as the `bl` pair F000/FFFF, whose
    # target is (pool address + 4 + 0xFFE).
    # 0x080153A2 has no prologue and keeps using the r4 task pointer and the
    # r5 constant that sub_08015268's `push {r4-r6,lr}` frame set up; the fake
    # `bl` comes from the pool word at 0x080143A0 (inside sub_080142a0's
    # pool).  sub_08015268 really runs 0x08015268-0x08015400 (0x198).
    0x080153A2,
    # 0x0801625A is the `str r0, [r1, #84]` completing the constant
    # materialised by `movs r0, #128` + `lsls r0, r0, #12` at the end of
    # sub_0801607c; the fake `bl` comes from the pool word at 0x08015258
    # (inside sub_08015268's pool).  sub_0801607c really runs
    # 0x0801607C-0x080162A0 (0x224).
    0x0801625A,

    # --- M11 (issue #85), two phantom ROM-POINTERS.  The pattern that
    # identifies them: the "pointer" is a lone word inside a DATA segment,
    # with neighbours that are plainly not addresses.
    0x0803E1F4,  # the sixth WORD of the 7-entry jump table at 0x0803E1E0,
                 # which runs to 0x0803E1FC - the harness's own table detector
                 # (lesson 4.45) flags the whole run as data.  symbols.csv
                 # already tagged it `rom-pointer` rather than a prologue, and
                 # sub_0803e1b8 has exactly one `push {r4,r5,lr}` at
                 # 0x0803E1B8 and one `pop {r4,r5}; pop {r0}` at 0x0803E286,
                 # so it really runs 0x0803E1B8-0x0803E28C.
    0x080401FC,  # opens `b.n +0x54` - a pool-skip branch (lesson 3.6) inside
                 # sub_080400c0, which really runs 0x080400C0-0x08040264
                 # (0x1A4, was 0x13C).  Its two "pointers" sit at 0x0825DA28
                 # in level_graphics_palettes (neighbours 0x11FB10FD,
                 # 0x01FB00FB - palette data) and 0x087D7FD0 in
                 # song_tail_misc_audio (neighbours 0xEEECE9E7, 0xF7F3F2F0 -
                 # audio samples).
    0x08040512,  # a 2-byte "function" that is really the `bx lr` ENDING
                 # sub_080404e4: 0x08040512 decodes as `bx r14` and 0x08040514
                 # opens the next function with a `push`.  Its only "pointer"
                 # is at 0x087083D0 in m4a_songs_2 (neighbours 0x06FEF6F1,
                 # 0x0AFAF800 - song data).

    # --- M11 (issue #85), five rows: the census had split ONE 2820-byte
    # function into five and clipped a second one's shared epilogue.
    #
    # sub_0803eaf8 really runs 0x0803EAF8-0x0803F5FC (0xB04).  Across that
    # whole span there is exactly ONE prologue (`push {r4, r5, lr}` at
    # 0x0803EAF8) and ONE epilogue (`pop {r4, r5}; pop {r1}` at 0x0803F5E6,
    # whose register set matches that push); no other `push` appears anywhere
    # inside it.  The four bogus entries:
    #   0x0803EFEE  a POOL-SKIP BRANCH at the top of a mid-function literal
    #               pool (`b.n loc_0803f334` immediately followed by `.word`,
    #               lesson 3.6).  Its "rom-pointer" evidence is a phantom: the
    #               only ROM word holding 0x0803EFEF is at 0x087D2A94, in the
    #               middle of the song_tail_misc_audio data segment, between
    #               0xEFECF6FF and 0x20180205 - audio samples, not a table.
    0x0803EFEE,
    #   0x0803F41A  and
    #   0x0803F494  long-jump targets: a Thumb `b.n` reaches only +/-2 KiB, so
    #               agbcc spells the jumps inside this 2.8 KiB function as
    #               `bl` (lesson 4.39).  Their four `bl` sites (0x0803EB62,
    #               0x0803EB68, 0x0803EB94, 0x0803EC34) are all real
    #               instructions inside the same function, and both blocks are
    #               prologue-less and end by branching onward rather than
    #               returning.
    0x0803F41A,
    0x0803F494,
    #   0x0803F5C4  the SHARED EPILOGUE itself - prologue-less, opens
    #               `ldrh r2, [r5, #60]` on an r5 established far earlier, and
    #               ends with the function's only `pop` pair.
    0x0803F5C4,
    #
    # And 0x0804139E is the same 4.39 shape one function later: it starts
    # `pop {r4, r5, r6}; pop {r0}`, is reached by a `bl` from 0x08040B5C
    # inside the 2142-byte sub_08040b40, and sits exactly at that function's
    # claimed end.  sub_08040b40 really runs 0x08040B40-0x080413A4 (0x864).
    0x0804139E,

    # --- M05 (issue #81), one row the reachability sweep dropped ---
    # 0x0801A41A has no prologue: it shares sub_0801a3e4's frame (the `push
    # {r4-r7,lr}` + `sub sp, #12` at 0x0801A3E4) and its epilogue at
    # loc_0801a740, and sub_0801a3e4 falls straight into it.  The only "bl
    # edge" to it is the pool word 0xFFFFF000 at 0x08019418 (inside
    # sub_08019000's literal pool) decoding as a bl pair.  sub_0801a3e4 runs
    # 0x0801A3E4-0x0801A76C (0x388).
    0x0801A41A,

    # --- M02 (issue #96), five rows the reachability sweep dropped ---
    # All five are tails of the function in front of them - a pool-skip
    # branch, a bare `bx r0` completing an epilogue, or a switch arm - and all
    # five have no prologue, sit right after a function whose claimed size is
    # not a multiple of 4 (or inside a jump table's arms), and are "evidenced"
    # only by a lone word inside a graphics/data blob (neighbours in brackets).
    0x080091EA,  # pool-skip `b.n` of sub_080091ac, whose `bne` jumps past it
                 # to 0x080091F0 [0x082A20A8: 81F500E6 080091EB 81D500E6].
    0x080091FA,  # the `bx r0` of sub_080091ac's `pop {r4, r5}; pop {r0}`,
                 # then its pool word gUnk_03002150 [0x0832C6C0].
                 # sub_080091ac really runs 0x080091AC-0x08009200 (0x54).
    0x080099E0,  # pool-skip `b.n` of sub_080099c8 (the sub_08009398 twin),
                 # which loads 0x080099E4 [0x085B53AC: EE111D6E 080099E1].
                 # sub_080099c8 runs 0x080099C8-0x080099FC (0x34).
    0x0800A202,  # pool-skip `b.n` between two arms of the 5-way switch of the
                 # hidden 0x0800A19C (table at 0x0800A1D4) [0x08383470].
    0x0800A332,  # the `bx r0` of sub_0800a294's epilogue, then its three pool
                 # words [0x083FD268: BA0EB34A 0800A333 45080044].
                 # sub_0800a294 runs 0x0800A294-0x0800A340 (0xAC).

    # --- M03 (issue #99), five rows the reachability sweep dropped ---
    # Same classes as M02's: two bare `bx rN` completing the previous
    # function's epilogue (each followed by that function's literal pool) and
    # three branches in the middle of a switch.  None has a prologue, all but
    # 0x0800F8FC sit right after a claimed size that is not a multiple of 4,
    # and each is "evidenced" only by a lone word inside a graphics/data blob
    # (neighbours in brackets).
    0x0800BF02,  # the `bx r0` of sub_0800be8c's `pop {r4-r7}; pop {r0}`, then
                 # its three pool words [0x085E4DC4: 002B20DF 0800BF03
                 # 5FF0B604].  sub_0800be8c runs 0x0800BE8C-0x0800BF10 (0x84).
    0x0800E306,  # the `bx r1` of sub_0800e2dc's `pop {r4, r5}; pop {r1}`, then
                 # its three pool words [0x083B7E54 and three other tile maps:
                 # E306E305 0800E307 0AE309E3].  sub_0800e2dc runs
                 # 0x0800E2DC-0x0800E314 (0x38).
    0x0800EE7E,  # `b.n` into the shared `sub_08003014` call of sub_0800ed78's
                 # two draw arms; the `mov r1, r9` before it falls straight in
                 # [0x08564338: 0C000320 0800EE7F 07340310].  sub_0800ed78
                 # runs 0x0800ED78-0x0800EF30 (0x1B8).
    0x0800F8FC,  # the `b.n` default of sub_0800f840's state switch: the `bne`
                 # in front of it lands on 0x0800F8FE, the first case compare
                 # [0x08662C78 and 0x08780D0C: 0406FFFE 0800F8FD 06050405].
                 # sub_0800f840 runs 0x0800F840-0x0800FA30 (0x1F0).
    0x0800FC26,  # pool-skip `b.n` default of sub_0800fb94's three-way switch,
                 # in front of its literal pool [0x083D3034: 081250B1 0800FC27
                 # 2F04FB05].  sub_0800fb94 runs 0x0800FB94-0x0800FCBC (0x128).

    # --- M08 (issue #86), three rows the reachability sweep dropped ---
    # A shared epilogue reached by a `bl` long jump and two tails of the
    # function in front of them; none has a prologue.  The last two sit right
    # after a claimed size that is not a multiple of 4 and are "evidenced"
    # only by a lone word inside a graphics blob (neighbours in brackets).
    0x0802BE70,  # the shared epilogue of the 2116-byte sub_0802b62c
                 # (`pop {r3, r4, r5}; mov r8, r3 ...; pop {r0}; bx r0`),
                 # reached by the `bl` far jump at 0x0802B63E (+0x832 is out of
                 # `b.n` range, lesson 4.39).  sub_0802b62c runs
                 # 0x0802B62C-0x0802BE80 (0x854).
    0x0802F6EA,  # the back-branch `b.n loc_0802f6d4` of sub_0802f6c0's
                 # two-frame yield loop, then its two pool words
                 # [0x0865DC14: E3DBD6D6 0802F6EB 00040709].  sub_0802f6c0 runs
                 # 0x0802F6C0-0x0802F6F4 (0x34).
    0x0802FDE8,  # the `bx r0` of sub_0802fd98's `pop {r4, r5}; pop {r0}`,
                 # then its two pool words [0x086DA624: EF01D2FD 0802FDE9
                 # DCC6ED0F].  sub_0802fd98 runs 0x0802FD98-0x0802FDF4 (0x5C).

    # --- M34 (issue #94), one row the reachability sweep dropped ---
    # 0x080B6B36 is the `b.n loc_080b6c02` that ends sub_080b6b08's first arm,
    # sitting immediately in front of that function's own literal pool: no
    # prologue, not 4-aligned, and the preceding entry's claimed size (0x2E) is
    # not a multiple of 4 - the pair of properties from lesson 4.40.  Its only
    # evidence is one `bl` whose SITE, 0x0803DB34, is a literal-pool word
    # (0xFFFFF078, inside the landed src/stage_3cd60.c range) decoding as a bl
    # pair: 0x0803DB38 + 0x78FFE = 0x080B6B36 exactly.  sub_080b6b08 really
    # runs 0x080B6B08-0x080B6C40 (0x138).
    0x080B6B36,

    # --- M32 (issue #73), one row the reachability sweep dropped ---
    # 0x080B1AFA is the second halfword of the `bl TaskYieldTrampoline` at
    # 0x080B1AF8, inside sub_080b1a1c's second yield loop.  The "bl edge" the
    # symbol DB found is the pool word 0xFFFFF000 at 0x080B0AF8 (inside
    # sub_080b09ac's literal pool) decoding as a bl pair.  sub_080b1a1c runs
    # 0x080B1A1C-0x080B1B2C (0x110).
    0x080B1AFA,

    # --- M28 (issue #74), one row the reachability sweep dropped ---
    # 0x080A02FC is the pool-skip branch INSIDE sub_080A02D4: the ROM falls
    # into its `b.n 0x080A031E` from the guard above and `loc_080A0304` is a
    # branch target of that same function.  The only ROM word holding
    # 0x080A02FD sits at 0x086FDD40, in the middle of a graphics blob
    # (neighbours 0xFD02F5F9, 0x0C07F4F1, 0xFBFF0405 - pixel data), so the
    # rom-pointer evidence is a phantom.  sub_080A02D4 runs
    # 0x080A02D4-0x080A0358 (0x84).
    0x080A02FC,

    # --- M19 (issue #79), three rows the reachability sweep dropped ---
    # 0x080711F6 is the `b.n 0x807132A` pool-skip branch INSIDE sub_080711D0,
    # not an entry: the three pool words it "owns" (0x0807122C-0x08071234) are
    # loaded from before it, and the ROM falls into it from the guard above.
    0x080711F6,
    # 0x08075B2E is a phantom from a literal-pool word: the `bl` edge the
    # symbol DB found is the pool value 0xFFFFF000 at 0x08074B2C decoding as a
    # `bl` pair.  The address is the middle of the eighth arm of the jump
    # table at 0x08075364, inside sub_080752F4.
    0x08075B2E,
    # 0x08076050 is the SHARED EPILOGUE of sub_080752F4 (`bl
    # TaskDispatchTrampoline` + the multi-register pop), reached by five `bl`
    # far jumps and seven `b.n` ones from that one 3456-byte function: a Thumb
    # `b.n` only reaches +/-2 KiB, so agbcc spells the long jumps as `bl` and
    # the bl-target heuristic saw a function.
    0x08076050,
    0x080CFCFC,
    # 0x0800315E is literal-pool data inside sub_08003110, not a function
    # (issue #32): the mask word 0xFFFFF7FF at 0x0800315C has a low half that
    # decodes as a `bl`, which fooled the bl-target heuristic.  sub_08003110
    # really runs 0x08003110-0x08003184 and 0x08003164 is a branch target
    # inside it.
    0x0800315E,
    # 0x08007102 is a `b.n 0x800711C` INSIDE sub_080070e8, not an entry point
    # (issue #32).  The real split of that 0x6C span is sub_080070b8 (0x30) +
    # sub_080070e8 (0x3C); byte totals agree either way, which is why the
    # mis-split survived until the range was decompiled.
    0x08007102,
    # 0x08063DFE is literal-pool data inside sub_08062584, not a function
    # (issue #65): the word 0xFFFFF000 at 0x08062DFC decodes as the `bl`
    # pair F000/FFFF, whose target lands mid-way through sub_08063DF4.  The
    # real function runs 0x08063DF4-0x08063E14 and byte-matches as one body
    # (src/actor_63698.c).
    0x08063DFE,
    # 0x080643A2 is the same artifact one function later (issue #65): the
    # word 0xFFFFF000 at 0x080633A0, inside sub_08062F88's literal pool,
    # decodes as the same bl pair and lands inside sub_08064398.
    0x080643A2,
    # 0x080671F8 is the `bx r0` that ends sub_080671C0 (issue #65).  A word
    # in the compressed-graphics blob at 0x0826ECC8 happens to equal
    # 0x080671F9, which the rom-pointer heuristic read as a Thumb entry.
    0x080671F8,
    # 0x0806F0E2 is the middle of sub_0806efec (issue #64).  Two independent
    # artifacts pointed at it: the words 5A5A5A5A/FFFFF000 at 0x0806E0DC,
    # inside sub_0806df98's literal pool, decode as the bl pair F000/FFFF,
    # and one word of the signed-byte table at 0x086EAE64 happens to equal
    # 0x0806F0E3.  The entry itself has no prologue -- it opens
    # `ldr r1, [r4, #0]` with r4 set by the code above it -- and
    # sub_0806efec really runs 0x0806EFEC-0x0806F174, pool included
    # (src/actor_6ef5c.c).
    0x0806F0E2,
    # 0x0806FC3E is the middle of sub_0806fb0c (issue #64), from the same
    # artifact one function later: the word 0xFFFFF000 at 0x0806EC3C, in
    # sub_0806eba4's literal pool, decodes as the bl pair F000/FFFF.  The
    # entry opens `movs r0, #128` with r1/r4 set by the code above it, and
    # sub_0806fb0c really runs 0x0806FB0C-0x0806FC98 (0x18C).
    #
    # 0xFFFFF000 has now produced four of these across #32/#65/#64: for any
    # bl-target-only symbol S, check whether the word at S - 0x1002 is
    # 0xFFFFF000 before believing it.
    0x0806FC3E,
    # 0x0806FFF8 and 0x08070406 are the `b.n` that skips a mid-function
    # literal pool (issue #64; the shape of lessons 3.6), reached by
    # fall-through from the instruction above.  Their real functions are
    # sub_0806ff7c (0x0806FF7C-0x080700E8, 0x16C) and sub_080703a8
    # (0x080703A8-0x0807042C, 0x84).
    0x0806FFF8,
    0x08070406,
    # 0x080706A8 opens `adds r1, r3, #0` with r3 live from the
    # `ldr r3, [r5, #0]` three halfwords above it, and the next epilogue is
    # at 0x0807072A: it is the middle of sub_08070648
    # (0x08070648-0x0807073C, 0xF4).
    0x080706A8,
    # 0x080C0AFC is the `bx r0` that ends sub_080c0a10 (issue #66): the
    # function's epilogue is `pop {r4-r7}; pop {r0}; bx r0` and the census cut
    # it one halfword short.  A word of the compressed sound-sample blob at
    # 0x087D2CB8 happens to equal 0x080C0AFD, which the rom-pointer heuristic
    # read as a Thumb entry.  sub_080c0a10 really runs
    # 0x080C0A10-0x080C0B18 (0x108), pool included.
    0x080C0AFC,
    # 0x080C1F18 is the `b.n 0x080C1F4C` default arm of the switch inside
    # sub_080c1ebc (issue #66), reached by fall-through from the `beq` above
    # it and branched to from three other arms.  Same artifact class: the word
    # at 0x087D3F04, inside the same sample blob, equals 0x080C1F19.
    # sub_080c1ebc really runs 0x080C1EBC-0x080C1F88 (0xCC).
    0x080C1F18,
    # 0x08090904 is the `bx r0` that ends sub_080908ec (issue #67): the
    # function's epilogue is `pop {r0}; bx r0` and the census cut it one
    # halfword short.  A word of the byte-table blob at 0x086C5964 happens to
    # equal 0x08090905, which the rom-pointer heuristic read as a Thumb entry.
    # sub_080908ec really runs 0x080908EC-0x08090914 (0x28), pool included.
    0x08090904,
    # 0x0808DFE2 and 0x0808F75E are the 0xFFFFF000 artifact twice more inside
    # M24 (issue #70).  Both pool words (at 0x0808CFE0 and 0x0808E75C) split
    # into the halfwords F000/FFFF, which the bl-target heuristic reads as
    # `bl pc+0x1002`.  Neither target has a prologue: 0x0808DFE2 is the body
    # of sub_0808dfc4's infinite loop (it branches back to 0x0808DFDE, above
    # itself), and 0x0808F75E is the instruction right after sub_0808f75c's
    # `push {lr}`.  The real functions are 0x0808DFC4-0x0808E050 (0x8C) and
    # 0x0808F75C-0x0808F7AC (0x50).
    0x0808DFE2,
    0x0808F75E,
    # 0x0808FCEE is the `b.n 0x0808FD08` that ends sub_0808fcd4's first arm
    # (issue #70), not an entry: it has no prologue and the `pop {r0}` that
    # closes the body at 0x0808FD10 pairs with sub_0808fcd4's `push {lr}`.
    # The word 0x0808FCEF at 0x08673250 that the rom-pointer heuristic found
    # sits inside the m4a_songs_2 tone/track data.  sub_0808fcd4 really runs
    # 0x0808FCD4-0x0808FD1C (0x48).
    0x0808FCEE,
    # 0x08087000 and 0x08087006 are jump-table landing pads inside
    # sub_08086f54 (issue #80, M23), not entries: the 9-entry table at
    # 0x08086FA0 that `mov pc, r0` at 0x08086F90 dispatches through has
    # 0x08087002 and 0x08087008 among its targets, so both "functions" are
    # arms of the same `switch`.  The words 0x08087001 / 0x08087007 the
    # rom-pointer heuristic found sit at 0x0827C564 / 0x0827C1AC, inside the
    # compressed-graphics blob (same artifact as 0x080671F8).  sub_08086f54
    # really runs 0x08086F54-0x0808705C (0x108).
    0x08087000,
    0x08087006,
    # Five M21 rows the rom-pointer heuristic invented (issue #71), each a
    # word inside a graphics/data blob that happens to equal a mid-function
    # address; the reachability walk over the module's annotated listing shows
    # the preceding function's code runs straight through all five.
    # 0x0807FBFC is the `b.n 0x0807FC10` that skips sub_0807fbd0's second arm
    # (host really runs 0x0807FBD0-0x0807FC20, 0x50); 0x0807FDF4 is the
    # `bne` fall-through inside sub_0807fdc8 (0x0807FDC8-0x0807FE18, 0x50);
    # 0x080803FC is the `bx r1` of sub_080803cc's epilogue
    # (0x080803CC-0x08080400, 0x34); 0x08080A0A is a literal-pool word inside
    # sub_08080930 (0x08080930-0x08080AA8, 0x178); and 0x08080E32 is the
    # `bl sub_08063e14` inside sub_08080e10 (0x08080E10-0x08080E40, 0x30).
    0x0807FBFC,
    0x0807FDF4,
    0x080803FC,
    0x08080A0A,
    0x08080E32,
}

# Curated Thumb entries with NO in-ROM reference (issue #31): dead m4a SDK
# exports.  The m4a driver was linked as whole objects, so public functions
# this game never calls (and that no ROM word points at) are still present
# in the binary.  Each address was hand-verified against the pokeemerald
# m4a.c function order and body shape (see the KNOWN_SYMBOLS comments);
# they are injected as candidates and carry the "curated" evidence kind.
EXTRA_THUMB_ENTRIES = {
    # --- M38 (issue #100), two push-less entries the census missed ---
    0x080CB6D8,  # entry 9 of the class-4 anchor table at 0x08758294 (the
                 # word 0x080CB6D9 at 0x087582B8): a push-less leaf callback
                 # (`ldr r2, =gUnk_03002490` ... `bx lr` at 0x080CB6FC).
                 # sub_080cb64c (entry 6) closes with its own `b.n` and pool
                 # (0x080CB6D0-0x080CB6D7) in front of it, so it really runs
                 # 0x080CB64C-0x080CB6D8 (0x8C, was 0xC0).
    0x080CD828,  # a push-less leaf (`ldr r2, =0x0201C1A0` ... `bx lr` at
                 # 0x080CD88A) that sub_080cd70c installs: the word
                 # 0x080CD829 at 0x080CD74C is in its pool.  sub_080cd75c
                 # closes with its own epilogue and pool (0x080CD812-
                 # 0x080CD827) in front of it, so it really runs 0x080CD75C-
                 # 0x080CD828 (0xCC, was 0x140).
    # --- M37 (issue #98), two push-less entries the census missed ---
    0x080C4818,  # a push-less leaf callback (`ldr r0, =gUnk_03002490` ...
                 # `bx lr` at 0x080C484E) that sub_080c4860 installs: the
                 # word 0x080C4819 at 0x080C488C is in its pool.  sub_080c4790
                 # closes with its own epilogue and pool (0x080C480C-
                 # 0x080C4817) in front of it, so it really runs 0x080C4790-
                 # 0x080C4818 (0x88, was 0xD0).
    0x080C51D4,  # called by `bl` at 0x080C2426 (sub_080c241c); sub_080c51c0
                 # ends with `bx lr` and its pool word 0x03006928 at
                 # 0x080C51D0, so it really runs 0x080C51C0-0x080C51D4 (0x14,
                 # was 0x3E), and 0x080C51D4 runs to 0x080C523C through the
                 # 0x080C51FE phantom.
    # --- M15 (issue #89), six companions the prologue filter missed ---
    # Push-less leaf callbacks (`ldr r0, =gUnk_03002490` ... `bx lr`) that
    # the anchor-table entries of task type #7 install; each is pointed at
    # (Thumb bit set) by one pool word of the entry body in front of it, and
    # that body closes with its own epilogue and literal pool before the
    # address, so none can be part of it.
    0x08054504,  # word 0x08054505 at 0x080543A4 (sub_08054330's pool);
                 # runs to 0x08054538 (sub_08054330 was 0x208, now 0x1D4).
    0x08055D24,  # word 0x08055D25 at 0x08055C44 (sub_08055b24's pool);
                 # runs to 0x08055D74 (sub_08055b24 was 0x250, now 0x200).
    0x080560FC,  # word 0x080560FD at 0x0805600C (sub_08055d74's pool);
                 # runs to 0x0805614C, through the 0x0805613A phantom
                 # (sub_08055d74 was 0x3C6, now 0x388).
    0x08056300,  # word 0x08056301 at 0x080562E8 (sub_0805614c's pool);
                 # runs to 0x08056320 (sub_0805614c was 0x1D4, now 0x1B4).
    0x08056428,  # word 0x08056429 at 0x08056368 (sub_08056320's pool);
                 # runs to 0x08056448 (sub_08056320 was 0x128, now 0x108).
    0x08056DA8,  # word 0x08056DA9 at 0x08056C88 (sub_08056770's pool);
                 # runs to 0x08056DD4 (sub_08056770 was 0x664, now 0x638).
    # --- M14 (issue #90), one table entry the prologue filter missed ---
    0x0804F5BC,  # entry 17 of enter 49's 26-entry sub-table gUnk_0873B664
                 # (the word 0x0804F5BD at 0x0873B6A8): a `push`-less leaf
                 # that starts `ldr r0, =gUnk_03002490` and ends `bx lr` at
                 # 0x0804F612.  sub_0804f450 (entry 8) closes with its own
                 # epilogue and its pool fills up to 0x0804F5BB, so it
                 # really runs 0x0804F450-0x0804F5BC (0x16C, was 0x1C4).
    # --- M07 (issue #93), four dead exports the reachability sweep found ---
    # Nothing points at or `bl`s any of them, but in each case the PRECEDING
    # function closes with its own complete epilogue and literal pool before
    # the address, so none can be part of it.
    0x08021B70,  # `push {r4, lr}`: the byte-2 twin of sub_08021b2c (same
                 # metatile-map lookup, `ldrb r0, [r1, #2]` instead of #3).
                 # sub_08021b2c closes `pop {r4}; pop {r1}; bx r1` at
                 # 0x08021B6A + pool, so it really runs 0x08021B2C-0x08021B70.
    0x080228C4,  # `push {r4-r6, lr}`: a three-bound clamp of a task's
                 # position against gUnk_030055F8.  sub_08022810 closes at
                 # 0x080228B6 + pool (0x08022810-0x080228C4).
    0x08026900,  # `push {r4, r5, lr}`.  sub_08026834 closes at 0x080268D6
                 # + pool 0x080268D8-0x080268FF (0x08026834-0x08026900).
    0x08026994,  # an empty `bx lr` stub (lesson 4.34) after sub_0802695c's
                 # own `bx lr` at 0x0802697E and its pool
                 # 0x08026980-0x08026993 (0x0802695C-0x08026994).
    # --- M11 (issue #85), four hidden entries the reachability sweep missed ---
    # Two are genuine callees the size heuristic swallowed, two are dead
    # exports.  In all four the PRECEDING function has its own complete
    # `pop {..}; bx rN` epilogue (plus alignment and literal pool) before the
    # address, so the split is unambiguous.
    0x0803F7E0,  # leaf with a `u16` parameter (`lsls r0,r0,#16; lsrs r3,r0,#16`),
                 # ending `bx lr` at 0x0803F82C, with no `push`.  sub_0803f6e0
                 # closes with its full epilogue at 0x0803F7CA and its pool
                 # fills 0x0803F7CC-0x0803F7DF, so it really runs
                 # 0x0803F6E0-0x0803F7E0 (0x100).
    0x08042D40,  # real table entry: the ROM word 0x08042D41 sits at
                 # 0x0873B4E8, inside a GENUINE function-pointer table whose
                 # neighbours are 0x08042981, 0x0803AA41, 0x0804335D,
                 # 0x0803B47D and 0x0803B6ED - not a data blob.  The preceding
                 # function closes `pop; pop; bx r0` with its pool after it.
    0x0803E5C0,  # leaf, `adds r3,r0,#0; adds r2,r1,#0; ldrb r0,[r2,#0]`: two
                 # args, no `push` (it saves nothing).  SEVEN `bl` sites, three
                 # of them OUTSIDE this module (0x080453FA, 0x080454A0,
                 # 0x080454C2 are in M12), which a long intra-function jump
                 # cannot be.  sub_0803e55c really runs 0x0803E55C-0x0803E5C0
                 # (0x64, was 0x9C) and ends `pop {r1}; bx r1` at 0x0803E5B6.
    0x0803FCE4,  # same shape: `adds r3,r0,#0; ldr r0,[pc,#36]; ldr r0,[r0,#0]`
                 # and seven `bl` sites, three from M12.  sub_0803fb54 really
                 # runs 0x0803FB54-0x0803FCE4 (0x190, was 0x1CC).
    0x08040084,  # dead export: nothing points at it and nothing `bl`s it, but
                 # sub_0803ffe0 closes with `pop; pop; bx r1` at 0x0804007E,
                 # so it cannot be part of it.  sub_0803ffe0 runs
                 # 0x0803FFE0-0x08040084 (0xA4, was 0xE0).
    0x080404E4,  # dead export, same evidence: sub_0804042c closes with
                 # `pop; pop; bx r1` at 0x080404DE.  sub_0804042c runs
                 # 0x0804042C-0x080404E4 (0xB8, was 0xE6).

    # --- M16 (issue #83), one hidden entry ---
    0x0805DBFC,  # dead export.  sub_0805dba0 ENDS at 0x0805DBE0 with a
                 # complete `pop {r4}; pop {r0}; bx r0` epilogue followed by
                 # its literal pool (0x0805DBE4-0x0805DBFB); 0x0805DBFC then
                 # opens its own `push {lr}` prologue and dispatches
                 # `sub_08002e98(Task.unk14, ...)`.  No ROM word holds
                 # 0x0805DBFD and no `bl` anywhere in 0x08000000-0x080D0000
                 # targets it, so the census had merged the two into one
                 # 0x78-byte entry.  Real split: sub_0805dba0 0x0805DBA0-
                 # 0x0805DBFC (0x5C) and sub_0805dbfc 0x0805DBFC-0x0805DC18
                 # (0x1C).

    # --- M05 (issue #81), one hidden entry the reachability sweep found ---
    0x0801A76C,  # dead export: binds the running task to a player record
                 # (Task.unk88 = &gUnk_03002170[i], the 116-byte PlayerState)
                 # and sets the OAM priority bits from the player index.  No
                 # ROM word holds 0x0801A76D and nothing `bl`s it; it sits
                 # after sub_0801a3e4's literal pool with its own prologue-less
                 # `bx lr` body and its own pool at 0x0801A7A0.

    # --- M29-M33 (issues #76 #72 #78 #73 #97), seventeen hidden entries the
    # reachability sweep found.  Three kinds: (a) continuation entries stored
    # as `fn+1` pool words into a task field or referenced from a script/anchor
    # table in asset_metadata_index; (b) dead twin tails - a small
    # `sub_08002e98(Task.unk14, N, <table>)` epilogue-twin of the function
    # right before it that nothing in the ROM references; (c) standalone
    # helpers after the owner's epilogue.
    0x080A1DBC,  # continuation: pool word 0x080A1DBD at 0x080A1DB0 is stored
                 # into Task.unk3C by sub_080a1d84 itself.
    0x080A4AA8,  # dead twin tail of sub_080a4a60 (reads Task.unk14, table
                 # 0x087489B8; the live twin uses unk15/0x087489BC).
    0x080A4C20,  # dead twin tail of sub_080a4bdc (table 0x087489D8).
    0x080A4F24,  # dead twin tail of sub_080a4ee0 (table 0x08748A28).
    0x080A5220,  # dead twin tail of sub_080a51dc (table 0x08748A68).
    0x080A9E88,  # continuation: pool word 0x080A9E89 at 0x080A7D90 (inside
                 # sub_080a7d14) is stored into a task field.
    0x080ACC8C,  # anchor-table entry: word 0x080ACC8D at 0x08749C40.
    0x080AE380,  # the y-coordinate window comparator after sub_080ae37c's
                 # 2-byte `bx lr` body; pool word 0x080AE381 at 0x080AD628.
    0x080AF100,  # anchor-table entry: 0x080AF101 x4 at 0x0874AD7C...0x0874ADA0.
    0x080AF178,  # anchor-table entry: 0x080AF179 x4 at 0x0874ADE4...0x0874AEA4.
    0x080AF26C,  # anchor-table entry: 0x080AF26D at 0x0874B514.
    0x080B0570,  # standalone scroll-shadow helper after sub_080b0338's
                 # TaskDispatchTrampoline epilogue; nothing references it.
    0x080B2E3C,  # dead twin tail of sub_080b2e20 (table 0x0874C150).
    0x080B2F78,  # dead twin tail of sub_080b2f38 (blend setup via
                 # sub_080061c0(0x1CD00, 0x5A5A5A5A)).
    0x080B2FB0,  # second dead twin tail in the same range (same body shape).
    0x080B33BC,  # dead twin tail of sub_080b3398 (table 0x0874C258).
    0x080B408C,  # anchor-table entry: word 0x080B408D at 0x0873F620.

    # --- M28 (issue #74), seven hidden entries the reachability sweep found ---
    # Six are real anchor-table targets that -fprologue-bugfix left without a
    # `push`, so the prologue filter could not propose them; the seventh is a
    # dead export nothing in the ROM references.  All are code inside a
    # declared range that no path from that range's entry reaches.
    0x0809D138,  # `movs r0, #0; bx lr` leaf; table word 0x087481B8 points at
                 # it (neighbours 0x0809D0DD twice).  sub_0809d0dc is 0x5C.
    0x0809E7D4,  # sets gUnk_03002490->unk24 from a pool constant and returns;
                 # table word 0x087481C0.  sub_0809e7c8 is 0xC, not 0x20.
    0x0809E820,  # `movs r0, #0; bx lr` leaf; table word 0x087481D4.
                 # sub_0809e7e8 is 0x38, not 0x3C.
    0x0809F618,  # `movs r0, #0; bx lr` leaf; table word 0x087481F0.
                 # sub_0809f588 is 0x90, not 0x94.
    0x0809F7F8,  # sets Task.unk24 = 18 and returns; four table words
                 # (0x08748214/0x0874822C/0x08748238/0x08748250) point at it.
                 # sub_0809f7e4 is 0x14, not 0x24.
    0x0809FFEC,  # dead export: the "if state 6 and unk82 == 4, poke the
                 # object row" leaf with its own pool; nothing references it.
                 # sub_0809fe10 is 0x1DC, not 0x218.
    0x080A0588,  # sets Task.unk54 = 0 and returns; table word 0x08748940
                 # (its neighbour 0x08748948 points at 0x080A0598).
                 # sub_080a0538 is 0x50, not 0x60.

    # --- M19 (issue #79), five hidden entries the reachability sweep found ---
    # 0x08071850: a dead export - the `sub_08002e98(Task.unk14, 26,
    # 0x0873FBC4)` twin of sub_08071830's unk15 dispatch, with its own
    # `push {lr}` prologue and pool; no ROM word references it.
    0x08071850,
    # 0x080743CC: the whole declared body of "sub_080743C8" is a bare `bx lr`
    # plus this function (lesson 4.34); it is the class-3 state 25 entry.
    0x080743CC,
    # 0x080761B4: a dead export sitting after sub_08076074's epilogue and pool
    # with its own `push {r4, r5, r6, lr}`; nothing references it.
    0x080761B4,
    # 0x08078258: same shape after sub_080781FC's epilogue and pool.
    0x08078258,
    # 0x08078B64: a four-byte `movs r0, #0; bx lr` leaf that the
    # -fprologue-bugfix prologue filter cannot propose; the anchor table word
    # at 0x08740E6C points at it.
    0x08078B64,
    0x080CE60C,  # m4aSongNumStartOrChange
    0x080CE6E0,  # m4aSongNumContinue
    0x080CE740,  # m4aMPlayContinue (wrapper)
    0x080CE778,  # m4aMPlayFadeOut (wrapper)
    0x080CE788,  # m4aMPlayFadeOutTemporarily
    0x080CE7A8,  # m4aMPlayFadeIn
    0x080CE930,  # MusicPlayerJumpTableCopy (swi 0x2A thunk, issue #53)
    0x080CEB90,  # SoundClear
    0x08063E74,  # hidden dead export inside M17 (issue #65): a rect/point
                 # containment test nothing in the ROM references, sitting
                 # between sub_08063E2C and sub_08063EB0 (its own
                 # `push {r4, lr}` prologue and `pop {r4}; pop {r1}; bx r1`
                 # epilogue; verified by byte-matching the range).
    0x08067074,  # hidden dead export inside M17 (issue #65): "count the
                 # active players" - nothing in the ROM references it, and
                 # it sits between sub_08067060 and sub_080670AC with its
                 # own `push {r4, r5, lr}` prologue.
    0x0806567C,  # hidden dead export inside M17 (issue #65): the
                 # sub_0806555C draw wrapper without the "kill the task
                 # when off-screen" arm, sitting between sub_08065640 and
                 # sub_080656B4 with its own `push {lr}` prologue.
    0x080641B0,  # hidden dead export inside M17 (issue #65): the polar
                 # velocity helper that fills gUnk_030023B4/D4 from
                 # ArcTan2 + the trig table; nothing in the ROM references
                 # it, and it sits between sub_08064188 and sub_0806421C
                 # with its own `push {r4, r5, r6, lr}` prologue.
    # 0x08093858 is a real function the prologue filter rejected (issue #67):
    # a four-instruction leaf (`gUnk_03002490->unk58 = 0;`) that opens with a
    # pool load and ends `bx lr`, so -fprologue-bugfix left it without a
    # `push`.  The behaviour-table word at 0x087440CC points at it (its
    # neighbours point at sub_080937d0, sub_08090ef1, sub_08090f15), and the
    # byte match of src/enemy_91f9c.c confirms the split: sub_080937d0 runs
    # 0x080937D0-0x08093858 (0x88) and this one 0x08093858-0x08093868.
    0x08093858,
    # Four M24 entries the census missed (issue #70), all found by the
    # reachability walk over the module's annotated listing: code inside a
    # declared range that no path from that range's entry reaches.
    0x0808D388,  # a real function the prologue filter rejected: a leaf that
                 # opens with a pool load and ends `bx lr` (no `push`), so it
                 # looks like the tail of sub_0808d364.  It recomputes
                 # Task.unk2C/unk18 from Task.unk48 and nothing points at it.
                 # sub_0808d364 really runs 0x0808D364-0x0808D388 (0x24).
    0x0808ED0C,  # hidden dead export: a byte-for-byte twin of sub_0808ece0
                 # (same 0x08743390 argument, same sub_0808f3b8 resume), with
                 # its own `push {lr}`; nothing in the ROM references it.
    0x0808FA04,  # hidden dead export: a byte-for-byte twin of sub_0808f9f8
                 # ("call sub_08063ff4, return 0"), with its own `push {lr}`.
    0x0808FE6C,  # hidden dead export: the sub_0808fe28 arm that plays cue
                 # 0x0874363C for Task.unk14, with its own `push {lr}`;
                 # nothing in the ROM references it.
    # 0x08099AD0 is the same shape one module later (issue #68, M27): a leaf
    # that opens with a pool load and ends `bx lr`, so -fprologue-bugfix left
    # it without a `push` and the prologue filter rejected it.  The behaviour
    # table word at 0x087456C8 points at it, and the byte match of
    # src/enemy_988f8.c confirms the split: sub_08099a7c runs
    # 0x08099A7C-0x08099AD0 (0x54) and this one 0x08099AD0-0x08099AEC.
    0x08099AD0,
    # Three more M27 leaves the same filter rejected (issue #68): the pair of
    # empty `bx lr` state handlers at 0x08099FE0/0x08099FE4 (dead: nothing in
    # the ROM points at them, they sit between sub_08099fd0's pool and
    # sub_08099fe8), the timer-decrement leaf at 0x0809B438 (the table word at
    # 0x0874580C points at it; sub_0809b408 is 0x30, not 0x4C), and the dead
    # table re-arm at 0x0809B8AC (a copy of sub_0809b5ec for the 0x08745B04
    # table; sub_0809b868 is 0x44, not 0x60).  All four are byte-matched by
    # src/enemy_99b20.c.
    0x08099FE0,
    0x08099FE4,
    0x0809B438,
    0x0809B8AC,
    # Six more entries the two blind spots of lesson 4.30 hid inside M22
    # (issue #69).  Four are dead exports with their own `push {lr}` prologue
    # sitting immediately after the previous function's literal pool, each a
    # copy of its host's tail dispatch and referenced by nothing in the ROM:
    # 0x080839D0 (host sub_0808398c is 0x44, not 0x60), 0x08083EE8 (host
    # sub_08083eb4 is 0x34, not 0x50), 0x080840D4 (host sub_080840a4 is 0x30,
    # not 0x4C) and 0x0808429C (host sub_0808424c is 0x50, not 0x6C).
    # The other two ARE pointer-referenced and were rejected only because
    # -fprologue-bugfix left them without a `push`: 0x08084A50 opens
    # `ldr r0, [pc, #N]` and the anchor-table word at 0x08741FA0 points at it
    # (host sub_080849dc is 0x74, not 0x98), and 0x08085FEC opens the same way
    # with the table word at 0x08742D40 pointing at it (host sub_08085fa0 is
    # 0x4C, not 0x84).  All six are byte-matched by src/enemy_82e68.c /
    # src/enemy_84d14.c.
    0x080839D0,
    0x08083EE8,
    0x080840D4,
    0x0808429C,
    0x08084A50,
    0x08085FEC,
    # Eight more entries the same two blind spots hid inside M26 (issue #75),
    # all found by a reachability walk over the module's annotated listing:
    # code no path from the declared entry can reach is a hidden function.
    # Four are pointer-referenced and were rejected only because
    # -fprologue-bugfix left them without a `push`: 0x08094144 (table word
    # 0x087441D4; host sub_08094040 is 0x104, not 0x124), 0x080963C0 (table
    # word 0x08744588; host sub_08096320 is 0xA0, not 0xBC), 0x08098738
    # (table word 0x087455E0; host sub_08098728 is 0x10, not 0x20) and
    # 0x080988A4 (table word 0x087455E4; host sub_0809887c is 0x28, not 0x38).
    # The other four are dead exports nothing in the ROM references:
    # 0x08095768 (a `push {lr}` copy of sub_080956e4's tail; host is 0x84, not
    # 0xB0), 0x08096E70 (a `push {r4, r5, r6, lr}` four-byte SE splitter; host
    # sub_08096e24 is 0x4C, not 0x78), 0x08098708 (a `Task.unk2C = 0` leaf;
    # host sub_080986ec is 0x18, not 0x2C) and 0x080988C0 (a bare `bx lr`
    # stub, lesson 4.34; host sub_080988b4 is 0xC, not 0x10).
    0x08094144,
    0x08095768,
    0x080963C0,
    0x08096E70,
    0x08098708,
    0x08098738,
    0x080988A4,
    0x080988C0,
    # Seventeen more entries the same two blind spots hid inside M21
    # (issue #71), found by the same reachability walk.  Seven are
    # pointer-referenced leaves the prologue filter rejected because
    # -fprologue-bugfix left them without a `push` (they open `ldr rN, [pc]`
    # and end `bx lr`): 0x0807FC94 (table word 0x08741BD0; host sub_0807fc70
    # is 0x24, not 0x3C), 0x080803BC (0x08741BEC; host sub_080803a4 is 0x18,
    # not 0x28), 0x08081960 (0x08741BF0; host sub_08081900 is 0x60, not 0x84),
    # 0x08081E40 (0x087415E0; host sub_08081db4 is 0x8C, not 0xB0),
    # 0x08081F08 and 0x08081F18 (0x08741C40 / 0x08741C2C; host sub_08081e64
    # is 0xA4, not 0xD4) and 0x08082458 (0x0874161C; host sub_08082338 is
    # 0x120, not 0x154).
    # The other ten are dead exports with their own `push {lr}` prologue,
    # each a copy of its host's tail dispatch that no ROM word (4-aligned)
    # references: 0x0807FB44 (host sub_0807fb00 is 0x44, not 0x60),
    # 0x08080300 (sub_080802bc 0x44, not 0x60), 0x080807BC (sub_0808076c
    # 0x50, not 0x6C), 0x0808164C (sub_08081614 0x38, not 0x54), 0x080817B8
    # (sub_08081774 0x44, not 0x60), 0x08081D68 (sub_08081d24 0x44, not
    # 0x60), 0x080820EC (sub_080820b8 0x34, not 0x50), 0x080822E4
    # (sub_080822b0 0x34, not 0x50), 0x080824D0 (sub_0808248c 0x44, not
    # 0x60) and 0x08082BFC (sub_08082bb8 0x44, not 0x60).
    0x0807FB44,
    0x0807FC94,
    0x08080300,
    0x080803BC,
    0x080807BC,
    0x0808164C,
    0x080817B8,
    0x08081960,
    0x08081D68,
    0x08081E40,
    0x08081F08,
    0x08081F18,
    0x080820EC,
    0x080822E4,
    0x08082458,
    0x080824D0,
    0x08082BFC,
    # Twenty-four more entries the same two blind spots hid inside M20
    # (issue #77), found by the same reachability walk.  Five are
    # pointer-referenced leaves the prologue filter rejected because
    # -fprologue-bugfix left them without a `push` (they open `ldr rN, [pc]`
    # and end `bx lr`): 0x08079B98 (table word 0x08740798; host sub_08079b40
    # is 0x58, not 0x6C), 0x0807A05C (0x08740EC0; host sub_0807a050 is 0xC,
    # not 0x1C), 0x0807BEFC (0x08740EF8; host sub_0807bed4 is 0x28, not
    # 0x38), 0x0807E9B4 (0x08741BAC; host sub_0807e9b0 is a bare `bx lr`
    # stub of 0x4, not 0x18) and 0x0807EFEC (0x087418BC; host sub_0807ef7c
    # is 0x70, not 0x80).
    # The other nineteen are dead exports with their own `push {lr}`
    # prologue, each a copy of its host's tail dispatch (or of a sibling
    # leaf) that no 4-aligned ROM word references: 0x0807927C (host
    # sub_08079238 is 0x44, not 0x60), 0x080794D0 (sub_080794c0 0x10, not
    # 0x1C), 0x080799A0 (sub_0807995c 0x44, not 0x60), 0x08079DB8
    # (sub_08079d74 0x44, not 0x60), 0x0807A4E4 (sub_0807a4a0 0x44, not
    # 0x60), 0x0807B888 (sub_0807b844 0x44, not 0x60), 0x0807BD60
    # (sub_0807bd20 0x40, not 0x5C), 0x0807C3D8 (sub_0807c394 0x44, not
    # 0x60), 0x0807CADC (sub_0807ca98 0x44, not 0x60), 0x0807CCEC
    # (sub_0807cc9c 0x50, not 0x6C), 0x0807CDEC (sub_0807cd9c 0x50, not
    # 0x6C), 0x0807CF64 (sub_0807cf20 0x44, not 0x60), 0x0807D0D8
    # (sub_0807d094 0x44, not 0x60), 0x0807DAF0 (sub_0807daac 0x44, not
    # 0x60), 0x0807E428 (sub_0807e3e4 0x44, not 0x60), 0x0807E5A0
    # (sub_0807e568 0x38, not 0x54), 0x0807E904 (sub_0807e8b8 0x4C, not
    # 0x68), 0x0807EE44 (sub_0807ee14 0x30, not 0x4C) and 0x0807EF08
    # (sub_0807eec4 0x44, not 0x60).
    0x0807927C,
    0x080794D0,
    0x080799A0,
    0x08079B98,
    0x08079DB8,
    0x0807A05C,
    0x0807A4E4,
    0x0807B888,
    0x0807BD60,
    0x0807BEFC,
    0x0807C3D8,
    0x0807CADC,
    0x0807CCEC,
    0x0807CDEC,
    0x0807CF64,
    0x0807D0D8,
    0x0807DAF0,
    0x0807E428,
    0x0807E5A0,
    0x0807E904,
    0x0807E9B4,
    0x0807EE44,
    0x0807EF08,
    0x0807EFEC,
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
    0x08005A74,  # TaskUpdatePosNoIntegrate (inside sub_080059fc's 0x94)
    0x08005B20,  # TaskUploadGfxAndPal (inside sub_08005acc's 0xF8)
    0x08005E1C,  # dead export        (inside sub_08005d9c's 0x10C)
    0x080063F0,  # IsOnScreen         (inside sub_080063ac's 0xB8)
    0x0800641C,  # IsWorldPosOnScreen (inside sub_080063ac's 0xB8)
    0x08006664,  # TaskSkipMaskSaveOne(inside sub_0800663c's 0xE8)
    0x0800668C,  # LinkColdInit       (inside sub_0800663c's 0xE8)
    0x08006904,  # LinkRestart        (inside sub_08006868's 0xAC)
    0x08007004,  # LinkSendStep       (inside sub_08006e9c's 0x21C; a real
                 #                     bl target from 0x08006D52, not dead)
    0x080070E8,  # LinkEndRound       (bl target from 0x08006D56; the row
                 #                     0x08007102 was a mis-split of it)
    0x080702D8,  # real function the prologue filter dropped (issue #64): a
                 # straight-line body opening `ldr r1, [pc, #84]`, with two
                 # honest `bl` callers at 0x0806FF72 and 0x0807044A.  The
                 # census folded it into sub_0807029c, whose real size is
                 # 0x3C rather than 0x98.
    0x08070454,  # same (issue #64): sub_08070208 stores the pool word
                 # 0x08070455 into Task.unk00, but the pointer scan only
                 # walks data segments, and the entry opens
                 # `ldr r0, [pc, #56]` with no prologue.  It follows
                 # sub_0807042c's `bx r0` + alignment halfword, so
                 # sub_0807042c is 0x24, not 0x6C.
    0x0806B40C,  # dead `bx lr` stub inside what the census read as one
                 # 0x4C-byte sub_0806b3c4 (issue #64).  It is the same
                 # two-byte no-op body as the named stubs sub_0806b330 and
                 # sub_0806b3c0, nothing references it, and the leaf has no
                 # prologue to scan for.  sub_0806b3c4 is really 0x48.
    0x080694E0,  # dead export inside what the census read as one 0x220-byte
                 # sub_080692fc (issue #64).  It has its own
                 # `push {lr}; sub sp, #8` prologue but nothing in the 8 MiB
                 # image references it -- no word pointer, no bl -- so the
                 # prologue scan never got to propose it.  sub_080692fc
                 # really runs 0x080692FC-0x080694E0 (0x1E4).
    0x0806ACF8,  # dead leaf inside what the census read as one 0x50-byte
                 # sub_0806acc8 (issue #64).  Nothing in the ROM references
                 # it -- no word pointer, no bl -- so it survives only
                 # because the game was linked whole-object, and it has no
                 # prologue because -fprologue-bugfix drops the leaf
                 # `push {lr}`.  Body:
                 #   u32 f(void) { return
                 #       gUnk_03002490->unk8C->unk1A != -1; }
    0x080C05F0,  # real leaf the prologue filter dropped (issue #66): a
                 # straight-line body opening `ldr r1, [pc, #28]` and ending
                 # `bx lr`, with its own pool at 0x080C0610, called by `bl`
                 # from 0x080BFE16, 0x080BFF94, 0x080BFFCC and 0x080C000C.
                 # The census folded it into sub_080c0540, whose real size is
                 # 0xB0 rather than 0xDC.
    0x080C1820,  # dead export inside what the census read as one 0x38-byte
                 # sub_080c1804 (issue #66).  It has its own `push {lr}`
                 # prologue and its own literal pool at 0x080C1834, and it is
                 # sub_080c1804 with `Task.unk15` swapped for `Task.unk14`;
                 # nothing in the 8 MiB image references it.  sub_080c1804 is
                 # really 0x1C.
    # Fourteen more entries the two blind spots of lesson 4.30 hid inside M23
    # (issue #80), all found by the reachability walk over the module's
    # annotated listing: code no path from the declared entry can reach.
    # Five ARE pointer-referenced and were rejected only because
    # -fprologue-bugfix left them without a `push`, so they look like the tail
    # of the function above them: 0x08086274 (anchor-table word 0x08742084;
    # host sub_08086170 is 0x104, not 0x15C), 0x08087210 (table word
    # 0x087425B4; host sub_0808713c is 0xD4, not 0x12C), 0x08089530 (table
    # word 0x08742760; host sub_08089460 is 0xD0, not 0xE4), 0x08089C0C
    # (table word 0x08742D9C; host sub_08089bf0 is 0x1C, not 0x40) and
    # 0x0808AA28 (table word 0x08742DCC; host sub_0808a9d8 is 0x50, not 0x90).
    0x08086274,
    0x08087210,
    0x08089530,
    0x08089C0C,
    0x0808AA28,
    # The other nine are dead exports with their own `push {lr}` prologue,
    # each a near-copy of the tail of the function that hosts them and
    # referenced by nothing in the ROM (the hosts' real sizes in brackets):
    # 0x08086128 (sub_080860f8, 0x30), 0x080862FC (sub_080862cc, 0x30),
    # 0x080868B8 (sub_08086824, 0x94), 0x08086A20 (sub_080869f0, 0x30),
    # 0x080870F4 (sub_080870c4, 0x30), 0x08087298 (sub_08087268, 0x30),
    # 0x08087824 (sub_08087790, 0x94), 0x0808798C (sub_0808795c, 0x30) and
    # 0x08088394 (sub_08088360, 0x34).
    0x08086128,
    0x080862FC,
    0x080868B8,
    0x08086A20,
    0x080870F4,
    0x08087298,
    0x08087824,
    0x0808798C,
    0x08088394,

    # --- M02 (issue #96), four hidden entries the reachability sweep found ---
    # -fprologue-bugfix leaves leaves without a `push`, so the strict prologue
    # filter rejected three of them (their callers' `bl`/pointer edges sit
    # after a mis-sized entry); the fourth is a dead export with its own
    # `push {lr}`.  In all four the preceding function closes with its own
    # epilogue (and literal pool) before the address.
    0x080093CC,  # dead export: the `while (gUnk_03001E90)` twin of
                 # sub_08009398's counted key-wait loop; no ROM word or `bl`
                 # references it.  sub_08009398 is 0x34, not 0x64.
    0x0800A178,  # leaf (`bx lr`) called by `bl` from sub_08047fe8
                 # (0x08048230): stores its first argument to 0x0200801C when
                 # the second matches gUnk_03002360 and 0x02006014 == 1.
                 # sub_0800a130 is 0x48, not 0xD2.
    0x0800A19C,  # leaf with a 5-way jump table at 0x0800A1D4, called by
                 # `bl` from sub_0800a0dc (0x0800A124); it runs to 0x0800A21C.
    0x0800AAD0,  # leaf copying/clamping gUnk_03000498 into gUnk_02006068;
                 # its pointer 0x0800AAD1 sits in sub_08009ab8's literal pool
                 # at 0x08009AD8.  sub_0800aaac is 0x24, not 0x5C.
    # --- M03 (issue #99), one hidden entry the reachability sweep found ---
    0x0800FFE8,  # leaf (`bx lr`, no `push`): returns 1 while the current
                 # task's screen position (Task.unk48/unk4A) is inside
                 # x in [-63, 303], y in [-63, 223]; seven `bl loc_0800ffe8`
                 # sites in M03's sprite task bodies.  sub_0800ffd8 ends `bx lr` at
                 # 0x0800FFE2 with its pool word at 0x0800FFE4, so it is 0x10,
                 # not 0x48.
    # --- M35 (issue #95), three hidden entries the reachability sweep found ---
    # Two are anchor-table targets that -fprologue-bugfix left without a
    # `push` (they open with a pool load and end `bx lr`), so the strict
    # prologue filter rejected them; the third is a dead export.  In all three
    # the preceding function closes with its own epilogue and literal pool
    # before the address.
    0x080B9DA8,  # dead export: the `gUnk_03001EB8[i] & 2` twin of
                 # sub_080b9d68's `& 9` player-key scan, with its own
                 # `push {r4, r5, lr}`; no ROM word references it.
                 # sub_080b9d68 is 0x40, not 0x80.
    0x080BB410,  # leaf: `if (t->unk14 != 4 && t->unk18 != 2) t->unk18 = 2;`
                 # anchor-table word 0x08756360 (entry 25 of the 27-entry
                 # table at 0x087562FC).  sub_080bb3e8 is 0x28, not 0x44.
    0x080BD9E8,  # leaf that zeroes gUnk_0200AFF0/gUnk_0200AF10 and fills
                 # gUnk_0200B044[0..3] with 3; table word 0x087562D0 (next to
                 # 0x080BA455 and 0x080C1F9D).  sub_080bd9b0 is 0x38, not 0x5C.
    # --- M08 (issue #86), one hidden entry the reachability sweep found ---
    0x0802D0C4,  # dead export with its own `push {lr}`: calls sub_08028948
                 # and sub_08028b1c, then stores 0 to 0x030055C0 on both arms
                 # of a gUnk_03002444 test (sub_0802d074 stores 4/5 there);
                 # no ROM word or `bl` references it.
                 # sub_0802d074 ends `bx lr` at 0x0802D0BE with its pool word at
                 # 0x0802D0C0, so it is 0x50, not 0x80.
    # --- M09 (issue #92), three dead exports the reachability sweep found ---
    0x0803093C,  # dead export with its own `push {r4, r5, lr}`: the
                 # three-argument twin of sub_080308e8 that passes its r1/r2
                 # through to sub_0803097c instead of the task's position;
                 # no ROM word or `bl` references it.  sub_080308e8 ends
                 # `bx r1` at 0x08030936 with its pool word at 0x08030938, so
                 # it is 0x54, not 0x94.
    0x080337F8,  # empty `bx lr` stubs (lesson 4.34) between the four stubs
    0x08033800,  # the anchor tables 0x0873A834/0x0873A920 point at
                 # (0x080337F4, 0x080337FC, 0x08033804, 0x08033808); no ROM
                 # word references these two, and each stub is `bx lr` + the
                 # 2-byte alignment pad, so sub_080337f4 and sub_080337fc are
                 # 4 bytes each, not 8.
    # --- M10 (issue #91), one hidden leaf the reachability sweep found ---
    0x0803BDD4,  # leaf `gUnk_03002490->unk88->unk01 = 7;` (no `push`),
                 # per-frame handler 17 of gUnk_0873A840 (table word
                 # 0x0873A884); the prologue filter cannot propose it.
                 # sub_0803bd90 ends `pop {r0}; bx r0` at 0x0803BDCC with its
                 # pool word at 0x0803BDD0, so it is 0x44, not 0x58.
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
        # Curated identifications are accepted directly, bypassing the
        # prologue filter.  KNOWN_SYMBOLS: the m4a XCMD handlers are
        # table-dispatched only and open with `ldr r0, [r1, #0x40]`.
        # EXTRA_THUMB_ENTRIES: every one was hand-verified by decompiling
        # its range and byte-matching it, and several are dead exports that
        # open with a pool load rather than a push (e.g. 0x08005A74 starts
        # `ldr r0, [pc, #20]`), which `strict` rejects.  Curation IS the
        # evidence — re-deriving it from the prologue shape defeats the
        # purpose of the list.
        if (target in KNOWN_SYMBOLS or target in EXTRA_THUMB_ENTRIES
                or plausible_thumb_entry(rom, target, nxt - ROM_BASE, strict)):
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
