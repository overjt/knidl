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
    (0x08000234, 0x24, "TaskSwitch"),  # task/context-switch helper 1 (stmfd sp!,{lr})
    (0x08000258, 0x30, "TaskYield"),  # helper 2: save sp/lr/r0, restore task sp
    (0x08000288, 0x20, "TaskExit"),  # helper 3: call task fn via ARM veneer
    (0x080002A8, 0x40, None),  # helper 4: task-done check + hang loop
    (0x080CFDDC, 0x08, None),  # ARM interworking veneer -> 0x08005655
    # The ARM halves of the three task trampolines (asm/sdk_libc.s): after
    # `bx pc; nop` each is one ARM `b` back to its helper above.  Decoded as
    # Thumb they were a stmia plus a raw 0xEAFC halfword, a PC-relative
    # branch the linker never saw, so any insertion between the helpers and
    # the trampolines broke the task switch (the boot test, docs/data.md 8.4).
    (0x080CFDC8, 0x04, None),  # b TaskSwitch
    (0x080CFDD0, 0x04, None),  # b TaskYield
    (0x080CFDD8, 0x04, None),  # b TaskExit
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
    0x080CFA80: "SoftReset",  # IME=0, BIOS reset flag 0x03007FFA=0, sp=0x03007F00;
    # svc 1 (RegisterRamReset) + svc 0 (SoftReset); katam libagbsyscall.s.
    # Two BL callers; the prologue sweep finds no `bx`/`pop {pc}` (it
    # ends in svc 0), so it is curated (#37; an extra label before)
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
    #
    #   C driver ("m4a.c", old_agbcc -O2): 0x080CE4B8-0x080CFA4B.  It opens
    #   with pokeemerald m4a.c's MidiKeyToFreq and UnusedDummyFunc, which
    #   the asm core's segment held until src/m4a_c1.c took them over.
    0x080CE4B8: "MidiKeyToFreq",  # clamp key 0xB2; gScaleTable 0x0860A1D0 +
    # gFreqTable 0x0860A284 interpolation via 2x umul3232H32
    0x080CE51C: "UnusedDummyFunc",  # empty `bx lr`, dead (pokeemerald m4a.c)
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
    0x080CFDD4: "TaskExitTrampoline",
    # ---- names given with tools/rename.py (issue #155 onwards) ----
    # Each name's evidence is its row in docs/analysis/renames.csv;
    # kept sorted by address, tools/rename.py inserts here.
    0x080008E8: "BeginFade",
    0x08000934: "UpdateFade",
    0x08000DE4: "EndFrame",
    0x080010CC: "VBlankIntr",
    0x080011AC: "CopyOamAndPalette",
    0x080011DC: "ReadKeys",
    0x08001280: "FlushDisplayRegs",
    0x080013F8: "ProcessCopyQueue",
    0x08001460: "SetHBlankIntr",
    0x08001488: "ClearHBlankIntr",
    0x080014BC: "SetVCountIntr",
    0x080014E8: "ClearVCountIntr",
    0x08001518: "IntrDummy",
    0x0800151C: "EnableForcedBlank",
    0x08001560: "DisableForcedBlank",
    0x0800157C: "RequestCopyList",
    0x080017E4: "RequestCopy",
    0x08001A0C: "ResetOamShadow",
    0x08001A34: "ResetSpriteQueue",
    0x08001A84: "RunBuildOamInIwram",
    0x08001A94: "QueueSprite",
    0x08001B08: "BuildOam",
    0x08001CC8: "DrawAffineSprite",
    0x08001FD0: "ResetBgScroll",
    0x08002028: "ResetFadeAndBlend",
    0x080020B8: "BeginFadeInFromWhite",
    0x08002104: "BeginFadeInFromBlack",
    0x0800214C: "BeginFastFadeInFromWhite",
    0x08002198: "BeginFadeOutToWhite",
    0x080021DC: "BeginFastFadeOutToWhite",
    0x08002220: "BeginFadeOutToBlack",
    0x08002268: "CheckWarmBoot",
    0x080022A0: "ClearWarmBoot",
    0x080022AC: "ResetTasksAndOam",
    0x080022BC: "ResetPlayTime",
    0x080022D0: "RunFrameNoTasks",
    0x080022E4: "RunFrame",
    0x080022FC: "LinkStartKeyExchange",
    0x08002338: "LinkStopKeyExchange",
    0x08002348: "LinkStartRecordExchange",
    0x08002358: "LinkRequestSync",
    0x08002378: "LinkSyncRandom",
    0x08002668: "LinkSyncClock",
    0x0800293C: "DisconnectLink",
    0x08002B04: "FillSendCmd",
    0x08002B8C: "UpdatePlayerKeys",
    0x08002D18: "RunLinkFrame",
    0x08002D54: "RunFrames",
    0x08002D74: "RunFramesNoTasks",
    0x08002D94: "RunLinkFrames",
    0x08002DB4: "RunFramesUntilFadeDone",
    0x08002DE0: "RunFramesNoTasksUntilFadeDone",
    0x08002E0C: "RunLinkFramesUntilFadeDone",
    0x08002E38: "ApplyBgLayout",
    0x08002E98: "CallTableEntry",
    0x08002EAC: "SeedRandom",
    0x08002EC0: "Random",
    0x08002EE8: "RandomRange",
    0x08002F14: "IntToDigits",
    0x08003014: "BlendColors",
    0x080030B8: "BlendColor",
    0x08003110: "PlayBgm",
    0x08003184: "ResetBgmPlayer",
    0x08003194: "GetCurrentBgm",
    0x080031B8: "PlaySfx",
    0x08003484: "StopAllSound",
    0x080034A0: "PauseAllSound",
    0x080034B8: "ResumeAllSound",
    0x080034D0: "StopBgm",
    0x080034F0: "StopSfxOnPlayer",
    0x08003564: "StopSfx",
    0x080035F4: "StopOtherSfx",
    0x08003688: "StopAllSfx",
    0x080036B8: "PlayBgmFadeIn",
    0x0800374C: "FadeOutBgm",
    0x08003770: "SetBgmVolume",
    0x080037A4: "SetSfxVolume",
    0x080037F8: "FadeInSfx",
    0x0800381C: "FadeOutSfx",
    0x08003840: "DisableSoundDriver",
    0x08003864: "EnableSoundDriver",
    0x08003888: "LinkSetupInit",
    0x08003964: "LinkSetupStop",
    0x08003A00: "MultiBootSetParams",
    0x08003A34: "MultiBootInitWithParams",
    0x08003A98: "LinkSetupRequestStart",
    0x08003AB8: "LinkSetupDetect",
    0x08003BD8: "LinkSetupMultiCart",
    0x08003DC4: "LinkSetupMultiBoot",
    0x08004000: "LinkSetupMain",
    0x08004068: "LinkSetupIntr",
    0x08004308: "LinkBroadcastWordStep",
    0x08004390: "LinkBlockAnnounce",
    0x08004400: "LinkBlockHandshakeStep",
    0x080044B8: "LinkBlockStart",
    0x080045E8: "LinkBlockParentIntr",
    0x0800469C: "LinkBlockChildIntr",
    0x08004714: "IsLinkBlockDone",
    0x08004734: "LinkBlockMain",
    0x08004968: "MultiBootInit",
    0x08004984: "MultiBootMain",
    0x08004D6C: "MultiBootSend",
    0x08004DB4: "MultiBootStartProbe",
    0x08004DD8: "MultiBootStartMaster",
    0x08004E9C: "MultiBootCheckComplete",
    0x08004EAC: "MultiBootHandShake",
    0x08004F98: "MultiBootWaitCycles",
    0x08004FB0: "MultiBootWaitSendDone",
    0x08004FEC: "InitTasks",
    0x08005228: "RunTasks",
    0x080055B0: "TaskSetSkipMask",
    0x080055C4: "TaskSetOthersSkipMask",
    0x08005618: "TaskSetAllSkipMask",
    0x08005654: "TaskFree",
    0x0800579C: "TaskCreate",
    0x080058E4: "TaskCreateFrom",
    0x08005904: "TaskCreateInRange",
    0x08005954: "TaskClampVelocity",
    0x080059A0: "TaskIntegrateMotion",
    0x080059D8: "TaskMove",
    0x080059FC: "TaskMoveRelativeToParent",
    0x08005A74: "TaskUpdatePixelPos",
    0x08005A90: "TaskMoveRelativeToBg3",
    0x08005ACC: "TaskIsOnScreen",
    0x08005B20: "TaskLoadFrameTilesAndPalette",
    0x08005BC4: "TaskLoadFrameTiles",
    0x08005C4C: "TaskIsInView",
    0x08005CA0: "TaskDrawScreen",
    0x08005D18: "TaskDrawScreenOrFree",
    0x08005D9C: "TaskDrawWorld",
    0x08005E1C: "TaskDrawWorldOrFree",
    0x08005EA8: "TaskDrawWorldInView",
    0x08005F30: "TaskDrawWorldInViewOrFree",
    0x08005FC8: "TaskDrawWorldLoadTiles",
    0x08006040: "TaskDrawWorldTilesLoaded",
    0x08006138: "TaskSleepForever",
    0x08006148: "TaskSetEntry",
    0x0800617C: "TaskSetFrameByFacing",
    0x080061A8: "TaskSetMotionX",
    0x080061C0: "TaskSetMotionXFacing",
    0x08006214: "TaskStopX",
    0x0800622C: "TaskSetMotionY",
    0x08006244: "TaskStopY",
    0x0800625C: "TaskSetMotion",
    0x080062C4: "TaskStop",
    0x080062E0: "TaskStopSlot",
    0x08006304: "TaskUpdateFlip",
    0x08006338: "TaskSetFrame",
    0x0800634C: "TaskSetFrameNoFlip",
    0x08006364: "TaskSetFrameFlip",
    0x08006384: "TaskSetPosXFacing",
    0x080063AC: "TaskStepForward",
    0x080063F0: "IsOnScreen",
    0x0800641C: "IsWorldPosOnScreen",
    0x08006464: "IsInView",
    0x080064AC: "RandomSpread",
    0x080064DC: "RandomSpreadFacing",
    0x0800652C: "TaskFreezeOrThawOthers",
    0x0800663C: "TaskRestoreSkipMask",
    0x08006664: "TaskSaveSkipMask",
    0x0800668C: "InitLinkDriver",
    0x08006724: "EnableSerial",
    0x08006868: "DisableSerial",
    0x08006904: "ResetSerial",
    0x08006914: "LinkMain1",
    0x08006A48: "CheckMasterOrSlave",
    0x08006A70: "InitTimer",
    0x08006AC8: "EnqueueSendCmd",
    0x08006BB4: "DequeueRecvCmds",
    0x08006CD4: "LinkVSync",
    0x08006D18: "Timer3Intr",
    0x08006D28: "SerialCB",
    0x08006E8C: "StartTransfer",
    0x08006E9C: "DoRecv",
    0x08007004: "DoSend",
    0x080070B8: "StopTimer",
    0x080070E8: "SendRecvDone",
    0x08007124: "ResetSendBuffer",
    0x08007174: "ResetRecvBuffer",
    0x080071DC: "ConnectLink",
    0x080072E0: "IsLinkError",
    0x080075B8: "CheckPauseButton",
    0x08007624: "HubMain",
    0x0800783C: "BigSwitchViewMain",
    0x0800791C: "StageMain",
    0x08007B68: "SendLinkBlockAndVerify",
    0x08007C5C: "ExtraModeTitleSendModeData",
    0x08007D4C: "ExtraModeTitleLinkErrorScreen",
    0x08007E04: "ExtraModeTitleLoadMultiBootImage",
    0x08007F9C: "ExtraModeTitleMain",
    0x080082D0: "CreateExtraModeTitleSprites",
    0x08008348: "Task_ExtraModeTitleSprite",
    0x08008394: "ExtraModeTitleSpriteUpdate",
    0x080083B0: "ExtraModeTitleTransferIcon",
    0x08008460: "ExtraModeTitleTransferIconUpdate",
    0x080084DC: "ExtraModeTitleLevelBar",
    0x08008558: "ExtraModeTitleLevelBarUpdate",
    0x08008664: "PauseScreen",
    0x080089E0: "PauseScreenLoadChoicePalette",
    0x08008A00: "BossEnduranceMain",
    0x08008B8C: "LinkErrorScreen",
    0x08008C4C: "LoadBgLayout",
    0x08008C64: "LoadGfxSet",
    0x08008E1C: "ExtraModeTitleLoadPicture",
    0x08008EBC: "HudClearAbilityPicture",
    0x08008ED4: "HudLoadAbilityPicture",
    0x08008F10: "LoadMuseumAbilitySignGfx",
    0x08008FC4: "PauseScreenLoadGraphics",
    0x080091AC: "BootLogoMain",
    0x08009200: "PlayBootLogo",
    0x08009398: "BootLogoWait",
    0x080093FC: "Task_BootLogo",
    0x08009418: "Task_TitlePalette",
    0x080095C0: "Task_TitleSprites",
    0x080096E0: "TitleMain",
    0x0800973C: "TitleScreen",
    0x080098A8: "IntroStory",
    0x080099C8: "IntroStoryWait",
    0x080099FC: "Task_IntroStoryPicture",
    0x08009AA0: "HudShowScore",
    0x08009AB8: "HudShowClock",
    0x08009ADC: "HudReset",
    0x08009B2C: "HudInit",
    0x08009CB0: "HudRedraw",
    0x08009E14: "HudDeactivateAbilityPanel",
    0x08009E20: "HudActivateAbilityPanel",
    0x08009E60: "AddPlayerLives",
    0x08009EB8: "AddPlayerLivesNoHud",
    0x08009EE8: "AddPlayerHealth",
    0x08009FCC: "SetPlayerAbilityNoHud",
    0x0800A008: "SetPlayerAbility",
    0x0800A04C: "AddPlayerScore",
    0x0800A0AC: "AddPlayerScoreNoHud",
    0x0800A0DC: "HudShowAbilityAnimated",
    0x0800A130: "HudShowAbility",
    0x0800A19C: "HudOpenAbilityPanel",
    0x0800A21C: "HudCloseAbilityPanel",
    0x0800A280: "HudShowHpBar",
    0x0800A294: "HudStartHpBar",
    0x0800A340: "HudStartTaskHpBar",
    0x0800A42C: "HudAnimateTaskHpBar",
    0x0800A4C0: "HudSetTaskHpBar",
    0x0800A554: "HudRemoveHpBar",
    0x0800A698: "HudStopClock",
    0x0800A778: "HudUpdateAbilityPanel",
    0x0800A854: "HudUpdateHpBars",
    0x0800A9A0: "HudAnimateHpBar",
    0x0800AA18: "HudStartHpBarFill",
    0x0800AA74: "HudSetHpBar",
    0x0800AA94: "HudResetHpBar",
    0x0800AAAC: "HudStopHpBarAnim",
    0x0800AAD0: "HudUpdateClock",
    0x0800AB08: "HudRedrawClock",
    0x0800AB64: "HudDrawPlayerIcon",
    0x0800ABC0: "HudDrawLives",
    0x0800AC38: "HudDrawHealth",
    0x0800ACBC: "HudDrawHealthChange",
    0x0800AD68: "HudDrawScore",
    0x0800AF40: "HudDrawClock",
    0x0800B0A4: "HudDrawAbilityPanel",
    0x0800B0FC: "HudDrawHpBarFrame",
    0x0800B130: "HudDrawHpBar",
    0x0800B190: "HudDrawHpBarChange",
    0x0800B318: "HudDrawTiles",
    0x0800B34C: "HudClearTiles",
    0x0800B37C: "HudClearWholeTilemap",
    0x0800B3A8: "HudClearTilemap",
    0x0800B3F8: "HudFlushTilemap",
    0x0800B428: "HudLoadGfx",
    0x0800B44C: "ResetGameSession",
    0x0800B4A8: "ResetScoresAndMaxHealth",
    0x0800B514: "ResetPlayerRecords",
    0x0800B5DC: "HubResetPlayers",
    0x0800B628: "ResetTasksAndPlayers",
    0x0800B648: "StageInit",
    0x0800B788: "HubInit",
    0x0800B87C: "BigSwitchViewInit",
    0x0800B920: "MainMenuMain",
    0x0800BCF0: "MenuDrawSaveSlots",
    0x0800BDA4: "MenuLoadSaveSlotLabel",
    0x0800BE8C: "MenuLoadSaveSlotPicture",
    0x0800BF10: "MenuLoadSaveSlotPalette",
    0x0800BF6C: "MenuLoadSaveSlotPercent",
    0x0800C09C: "MenuFileSelect",
    0x0800C20C: "MenuSetupFileMenu",
    0x0800C34C: "MenuFileMenuSelect",
    0x0800C558: "MenuNormalExtraSelect",
    0x0800C610: "MenuPlayerCountSelect",
    0x0800C8A0: "MenuEraseSelect",
    0x0800CA10: "MenuEnterModeList",
    0x0800CC30: "MenuDrawModeList",
    0x0800CD60: "MenuModeListSelect",
    0x0800CFF4: "MenuEnterLinkPlay",
    0x0800D0F4: "MenuModePlayerCountSelect",
    0x0800D280: "MenuEnterSoundTest",
    0x0800D310: "SoundTestHighlightCursor",
    0x0800D35C: "SoundTestDrawNumber",
    0x0800D404: "SoundTestPlaySfx",
    0x0800D450: "MenuSoundTest",
    0x0800D85C: "MenuLinkPlay",
    0x0800DA18: "MenuSetLinkSessionMode",
    0x0800DA9C: "CreateFileSelectSprites",
    0x0800DAF8: "Task_FileSelectSlotLabel",
    0x0800DB64: "Task_FileSelectSlot",
    0x0800DBDC: "FileSelectSlotUpdate",
    0x0800DC98: "FileSelectSlotSlideIn",
    0x0800DCD8: "Task_FileSelectCursor",
    0x0800DDA0: "FileSelectCursorUpdate",
    0x0800DE6C: "Task_FileMenuSlot",
    0x0800DFDC: "FileMenuSlotUpdate",
    0x0800E0C0: "Task_FileMenuHighlight",
    0x0800E148: "FileMenuHighlightUpdate",
    0x0800E28C: "MenuUpdateFileMenuPalette",
    0x0800E2DC: "MenuLoadPicture",
    0x0800E314: "Task_NormalExtraPanel",
    0x0800E390: "NormalExtraPanelUpdate",
    0x0800E46C: "Task_PlayerCountPanel",
    0x0800E518: "PlayerCountPanelUpdate",
    0x0800E5B0: "Task_ModeListCursor",
    0x0800E674: "ModeListCursorUpdate",
    0x0800E7B0: "ModeListHighlightRow",
    0x0800E81C: "Task_ModePlayerCountPanel",
    0x0800E8C0: "ModePlayerCountPanelUpdate",
    0x0800E9A4: "Task_EraseConfirmDialog",
    0x0800EA0C: "EraseConfirmDialogUpdate",
    0x0800EAE4: "Task_EraseFileWipe",
    0x0800EC70: "Task_SoundTestCursors",
    0x0800ED78: "Task_SoundTestPulse",
    0x0800EF30: "Task_LinkPlayPalettePulse",
    0x0800F084: "Task_LinkPlayColorCycle",
    0x0800F180: "Task_LinkPlayPlayerList",
    0x0800F2B4: "LinkPlayPlayerListUpdate",
    0x0800F390: "Task_LinkPlayConsole",
    0x0800F408: "LinkPlayConsoleUpdate",
    0x0800F5EC: "Task_LinkPlayCable",
    0x0800F698: "LinkPlayCableUpdate",
    0x0800F7B0: "Task_MenuScreenTitle",
    0x0800F840: "MenuScreenTitleUpdate",
    0x0800FA30: "Task_MenuBgPaletteCycle",
    0x0800FB94: "Task_MenuBackground",
    0x0800FCBC: "BgScrollInit",
    0x0800FCFC: "BgScrollStart",
    0x0800FD24: "BgScrollStartX",
    0x0800FDB8: "BgScrollStartY",
    0x0800FE54: "BgScrollStartBg3Slide",
    0x0800FE94: "BgScrollFinish",
    0x0800FF00: "Task_BgScroll",
    0x0800FFE8: "TaskIsOnScreenNoCamera",
    0x08010020: "SetBlend",
    0x08010048: "SetWindow",
    0x080100AC: "CutsceneMain",
    0x080102C0: "CutsceneLoadGraphics",
    0x08010358: "CreateCutsceneActor",
    0x080103F0: "Task_CutsceneDirector",
    0x08010480: "CutsceneCheckSkip",
    0x080104F0: "Task_CutsceneActor",
    0x0801050C: "CutsceneDuelStart",
    0x08010528: "CutsceneDuelKirby",
    0x08010834: "CutsceneActorScript1",
    0x08010BAC: "CutsceneActorScript2",
    0x08010CB4: "CutsceneActorScript3",
    0x08010EB8: "CutsceneDuelBladeKnight",
    0x08011154: "CutsceneBeachStart",
    0x080111A8: "CutsceneBeachKirby",
    0x080113CC: "CutsceneBeachChair",
    0x08011458: "CutsceneBeachCandyDream",
    0x080115A0: "CutsceneBeachDonutDream",
    0x0801170C: "CutsceneBeachMeatDream",
    0x08011880: "CutsceneBeachThoughtBubble",
    0x080119B4: "CutsceneBeachWaddleDoo",
    0x08011B60: "CutsceneBeachSunglasses",
    0x08011BFC: "CutsceneBeachQuestionMarks",
    0x08011FF8: "CutsceneBombStart",
    0x0801201C: "CutsceneBombKirby",
    0x08012628: "CutsceneBombPoppyBrosSr",
    0x080129F8: "CutsceneBombHeldBomb",
    0x08012B94: "CutsceneBombThrownBomb",
    0x08012DF8: "CutsceneBombActorScript18",
    0x08012E6C: "CutsceneBombHeldBombFuse",
    0x08013058: "CutsceneBombThrownBombFuse",
    0x08013348: "CutsceneBombActorScript21",
    0x08013430: "CutsceneBalloonsStart",
    0x0801347C: "CutsceneBalloonsKirby",
    0x08013AF8: "CutsceneBalloonsLooseBalloon",
    0x08013BE4: "CutsceneBalloonsYellowBalloon",
    0x08013E38: "CutsceneBalloonsGreenBalloon",
    0x0801408C: "CutsceneBalloonsLastBalloon",
    0x08014184: "CutsceneBalloonsActorScript27",
    0x080142A0: "CutsceneBalloonsActorScript28",
    0x080143B4: "CutsceneBalloonsActorScript29",
    0x0801448C: "CutsceneTomatoStart",
    0x080144B0: "CutsceneTomatoKirby",
    0x08014994: "CutsceneTomatoMaximTomato",
    0x08014C08: "CutsceneTomatoActorScript32",
    0x08014D08: "CutsceneTomatoExclamation",
    0x08014E6C: "CutsceneShipStart",
    0x08014EA0: "CutsceneShipKirby",
    0x08015268: "CutsceneShipSpyglass",
    0x08015400: "CutsceneShipPirateHat",
    0x08015524: "CutsceneShipShark",
    0x08015680: "CutsceneShipWaves",
    0x08015704: "CutsceneSingingStart",
    0x08015758: "CutsceneSingingKirby",
    0x08015F18: "CutsceneSingingActorScript40",
    0x0801607C: "CutsceneSingingRainbowBar",
    0x080162A0: "CutsceneSingingActorScript42",
    0x08016514: "CutsceneSingingBeamedNotes",
    0x080167DC: "CutsceneSingingDottedNote",
    0x08016AC4: "CutsceneSingingActorScript45",
    0x08016DD4: "CutsceneSingingQuarterNote",
    0x080170E4: "CutsceneSingingActorScript47",
    0x080173F4: "CutsceneSingingThoughtBubble",
    0x0801757C: "CutsceneSingingTrebleClef",
    0x0801761C: "CutsceneFountainStart",
    0x08017668: "CutsceneFountainKirby",
    0x08018464: "CutsceneFountainKirbyUpdate",
    0x08018498: "CutsceneFountainKingDedede",
    0x08018B84: "CutsceneFountainKingDededeUpdate",
    0x08018BB8: "CutsceneFountainActorScript53",
    0x08018D7C: "CutsceneFountainActorScript54",
    0x08018E14: "CutsceneFountainActorScript52",
    0x08019000: "CutsceneFountainNightmarePowerOrb",
    0x08019590: "CutsceneFountainNightmarePowerOrbUpdate",
    0x080195EC: "CutsceneFountainStarRod",
    0x08019B30: "CutsceneFountainJet",
    0x08019C44: "CutsceneFountainActorScript59",
    0x08019D30: "CutsceneFountainActorScript60",
    0x08019E48: "CutsceneFountainActorScript61",
    0x08019EEC: "CutsceneFountainActorScript62",
    0x0801A07C: "CutsceneFountainActorScript56",
    0x0801A1EC: "CutsceneFountainKirbyDraw",
    0x0801A7B4: "ClearColliderLists",
    0x0801A828: "RegisterCollider",
    0x0801A8C8: "HitTestPlayerColliders",
    0x0801AF14: "HitTestColliderClass10",
    0x0801B24C: "HitTestColliderClass20",
    0x0801B7DC: "PlaceAttackBox",
    0x0801B8E4: "CalcHitDamageAndDirection",
    0x0801B9E4: "HitRecordHitter",
    0x0801BAA4: "PlayerProbeTerrain",
    0x0801BCAC: "TerrainCollideBox",
    0x0801BDE0: "TerrainCollideBoxInCameraBounds",
    0x0801BF1C: "TerrainCollideBoxWalls",
    0x0801C030: "TerrainCollideBoxCeilingAndFloor",
    0x0801C12C: "TerrainCollideBoxFloor",
    0x0801C230: "TerrainCollideBoxAlongVelocity",
    0x0801C30C: "TerrainCollidePointPushOut",
    0x0801C3A4: "TerrainCollidePointStop",
    0x0801C444: "TerrainCollideBoxTileEdge",
    0x0801C51C: "TerrainProbeBegin",
    0x0801C5C8: "TerrainProbeEnd",
    0x0801C690: "TerrainProbeWallRightOnGround",
    0x0801C7CC: "TerrainProbeWallLeftOnGround",
    0x0801C930: "TerrainProbeFloorInCameraBounds",
    0x0801D394: "TerrainProbeFloor",
    0x0801D9C8: "TerrainProbeWallRightInAir",
    0x0801DC88: "TerrainProbeWallLeftInAir",
    0x0801DEE8: "TerrainProbeCeiling",
    0x0801E178: "TerrainProbeLandingInCameraBounds",
    0x0801ECD0: "TerrainProbeLanding",
    0x0801F800: "TerrainProbeCeilingNoSlopeLink",
    0x0801FC48: "TerrainProbeFloorNoSlopeLink",
    0x0801FE2C: "TerrainProbeLandingNoSlopeLink",
    0x0801FF84: "TerrainProbeAlongVelocity",
    0x0802069C: "TerrainProbePointStop",
    0x080207A0: "TerrainProbePointPushOut",
    0x08020B38: "TerrainProbeTileEdge",
    0x08021130: "TerrainProbeWaterAndDrift",
    0x0802136C: "TerrainProbeWater",
    0x080214E0: "TerrainProbeWaterAtPoint",
    0x08021564: "TerrainProbeDamage",
    0x08021634: "TerrainQueryPixel",
    0x080216D8: "TerrainQueryPixelAndBelow",
    0x080217DC: "TerrainQueryPixelAndSides",
    0x080218F8: "TerrainQueryDamage",
    0x08021970: "GetTileFloorSnap",
    0x08021990: "GetTilePushDown",
    0x080219B0: "GetTilePushUp",
    0x080219D0: "GetTilePushRight",
    0x080219F0: "GetTilePushLeft",
    0x08021A10: "TerrainLoadFloorAttributes",
    0x08021A40: "GetShapeAtPixelIgnoringOneWay",
    0x08021AB4: "GetTileShapeAtPixel",
    0x08021B18: "GetCollisionTileAtPixel",
    0x08021B2C: "GetCollisionTile",
    0x08021BB4: "GetCollisionTileAtOffset",
    0x08021C4C: "IsWaterAtPixel",
    0x080222B0: "TerrainInitOneWayFloor",
    0x080224B0: "TaskInitWaterFlags",
    0x080224F8: "TaskInitWaterFlagsSlot",
    0x08022624: "IsFullBlockAtPixel",
    0x08022650: "TerrainClampBoxToPlayerBounds",
    0x08022760: "IsTaskBelowPlayerBounds",
    0x08022788: "IsAtPlayerBoundsTop",
    0x080227A4: "ClampTaskToRoom",
    0x08022810: "TerrainClampBoxToCameraBounds",
    0x0802294C: "IsTaskBelowRoom",
    0x0802296C: "ResetLevelStateAtHub",
    0x08022C3C: "ResetLevelStateForContinue",
    0x08022F50: "BossEnduranceSetStart",
    0x08022F9C: "ClearRoomBgmStarted",
    0x08022FA8: "LoadRoom",
    0x080235EC: "CreateRoomTask",
    0x08023618: "Task_Room",
    0x08023634: "RoomTaskStageInit",
    0x080236D4: "RoomTaskDraw",
    0x080236E4: "RoomTaskUpdateCamera",
    0x08023748: "RoomTaskLateUpdateBg2",
    0x080237A4: "RoomTaskLateUpdateLooping",
    0x080237FC: "RoomTaskLateUpdateBg2Bg3",
    0x0802385C: "RoomTaskLateUpdateBg23HBlank",
    0x080238A4: "RoomTaskLateUpdateBg23RowsHBlank",
    0x080238EC: "RoomTaskLateUpdateBg3AutoScroll",
    0x08023948: "LoadHubRoom",
    0x08023CA0: "LoadBigSwitchViewRoom",
    0x08023E34: "RoomTaskHubInit",
    0x08023E78: "RoomTaskBigSwitchViewInit",
    0x08023EA0: "RoomTaskHubUpdateCamera",
    0x08023EFC: "RoomTaskBigSwitchViewUpdateCamera",
    0x08023F18: "RoomTaskHubLateUpdateBg23",
    0x08023F5C: "RoomTaskHubLateUpdateBg123",
    0x08023FA0: "RoomTaskBigSwitchViewLateUpdate",
    0x08023FD4: "LoadGoalGameRoom",
    0x080242D0: "RoomTaskGoalGameInit",
    0x08024300: "LoadCutsceneRoom",
    0x08024540: "RoomTaskCutsceneInit",
    0x0802457C: "RoomTaskSnapCameraToFocus",
    0x08024598: "RoomTaskLateUpdateBg2NoObjects",
    0x080245D0: "RoomTaskLateUpdateBg2Bg3NoObjects",
    0x08024610: "LoadEndingEpilogueRoom",
    0x08024654: "LoadEndingStarRodReturnRoom",
    0x08024698: "LoadEndingRoom",
    0x0802497C: "LoadCreditsRoom",
    0x08024DA4: "RoomTaskCreditsInit",
    0x08024E40: "FindDoorAt",
    0x08025024: "EnterDoor",
    0x080258E0: "ExitClearedStage",
    0x08025A30: "ExitKingDededeStage",
    0x08025ACC: "ExitToNextRoom",
    0x08025B0C: "ExitToNextRoomOnWarpStar",
    0x08025B5C: "ExitToEnding",
    0x08025BC8: "PressBigSwitch",
    0x08025DC4: "ReturnFromBigSwitchView",
    0x08025F00: "ExitOnWarpStar",
    0x0802610C: "ExitByCannon",
    0x080261C0: "CreateBlockBreakEffect",
    0x080261D4: "RequestScreenShake",
    0x0802621C: "TaskCreateHighSlot",
    0x08026264: "SetCameraFocus",
    0x08026278: "SetCameraFocusOrAnchor",
    0x080262DC: "EndMidBossFight",
    0x08026308: "WrapLoopingRoom",
    0x080264B0: "CreateEntryDoorOpening",
    0x0802651C: "CloseDoorOpening",
    0x0802653C: "CreateEntryDoorStageClearFlag",
    0x0802672C: "CreateStageUnlockPan",
    0x08026834: "CreateBigSwitchUnlockPan",
    0x08026900: "ClampCameraFocusToRoom",
    0x0802695C: "UnlockNextLevel",
    0x08026998: "SaveAndSetContinuePoint",
    0x08026A0C: "WhispyWoodsCheckScrollLock",
    0x08026A80: "KrackoCheckScrollLock",
    0x08026AEC: "KingDededeCheckScrollLock",
    0x08026CA4: "UpdateDoors",
    0x08026EEC: "DrawDoors",
    0x08027128: "StopRoomAndApplyExitFlags",
    0x08027178: "StopRoom",
    0x08027198: "FreeRoomAndDoorObjects",
    0x080271EC: "PauseRoom",
    0x08027204: "SetRoomUpdateFlags",
    0x08027210: "ResumeRoom",
    0x08027228: "PauseSaveBgPalette",
    0x08027240: "PauseRestoreRoomGraphics",
    0x080272DC: "ReturnToHubStageDoor",
    0x080273A0: "ReturnToRestartPoint",
    0x080276AC: "HoldPlayerCamera",
    0x080276CC: "ReleaseDeadPlayerView",
    0x08027750: "AreInactivePlayerCamerasParked",
    0x08027798: "CameraStartHoldAnchorAt",
    0x080277F0: "CameraStartFollowFocusAt",
    0x08027850: "CameraStartFollowingPlayer",
    0x08027908: "CameraStartPlayersAtAnchor",
    0x08027A30: "ArePlayerCamerasDoneGliding",
    0x08027A60: "CameraResumeFollowFocus",
    0x08027E28: "InitRoomBgLayout",
    0x08028280: "InitEndingRoomBgLayout",
    0x08028304: "StartRoomBlockAnims",
    0x08028320: "SpawnDoorObjects",
    0x0802885C: "CalcBg3Parallax",
    0x08028948: "CalcRoomBounds",
    0x08028990: "CameraResetBoundsToGroup",
    0x08028B1C: "CameraResetBounds",
    0x08028B8C: "CameraResetRoomView",
    0x08028E3C: "CalcRoomAndCameraBounds",
    0x08028E4C: "SetRoomEntryPoint",
    0x080290AC: "CameraSetFocusToLocalPlayer",
    0x080290DC: "CameraInitPos",
    0x08029110: "PlayRoomBgm",
    0x08029194: "PlayHubRoomBgm",
    0x080291D0: "LoadBg2Gfx",
    0x08029204: "LoadBg3Gfx",
    0x0802923C: "ClearBg2Bg3Maps",
    0x08029270: "SelectBg3MapShape",
    0x080292B0: "LoadBg3Map",
    0x080292D0: "SpawnRoomObjectsInView",
    0x08029318: "InitDoors",
    0x080293D8: "CameraUpdatePos",
    0x08029444: "CameraUpdatePosNoParallax",
    0x08029474: "CameraUpdatePosBg3AutoScrollX",
    0x080294D0: "StreamBg2Map",
    0x080295B8: "StreamBg3Map",
    0x080296A0: "StreamBg2MapLooping",
    0x08029708: "StreamBg123Maps",
    0x080297DC: "StreamBg23Maps",
    0x080298C4: "StreamBg23Rows",
    0x08029930: "CameraWriteScrollParallax",
    0x080299E8: "CameraWriteScrollHBlank",
    0x08029A4C: "CameraWriteScrollBg23",
    0x08029AB4: "CameraWriteScrollBg123",
    0x08029BB8: "SpawnRoomObjectsScrolledIn",
    0x08029C74: "UpdatePlayerGroupCenter",
    0x08029E24: "SetCameraBoundsToGroup",
    0x08029EF4: "SetCameraBoundsToGroupInScrollLock",
    0x0802A190: "UpdatePlayerCameras",
    0x0802A260: "UpdatePlayerCamerasInScrollLock",
    0x0802A340: "SetPlayerGroupCenterFromTasks",
    0x0802A42C: "SetPlayerBoundsFromCamera",
    0x0802A484: "SetPlayerBoundsFromCameraInScrollLock",
    0x0802A4EC: "SetViewRectToPlayers",
    0x0802A568: "SetViewRectToPlayersInScrollLock",
    0x0802A63C: "LockPlayersPastScrollLine",
    0x0802A82C: "SpawnRoomObjectsInRect",
    0x0802A9CC: "DrawBg2View",
    0x0802AA4C: "DrawBg2Row",
    0x0802AA9C: "DrawBg2Column",
    0x0802AAE8: "DrawBg2EdgeColumn",
    0x0802AB30: "DrawBg3View",
    0x0802ABA8: "DrawBg3Row",
    0x0802ABEC: "DrawBg3Column",
    0x0802AC30: "DrawBg123View",
    0x0802ACBC: "DrawBg123Row",
    0x0802AD1C: "DrawBg123Column",
    0x0802AD78: "DrawBg23View",
    0x0802AE00: "DrawBg23Row",
    0x0802AE58: "DrawBg23Column",
    0x0802AEAC: "DrawBg23FullRows",
    0x0802AF1C: "DrawBg23FullRow",
    0x0802AF6C: "DrawBg1Tile",
    0x0802AFC8: "DrawBg2Tile",
    0x0802B030: "DrawBg3Tile",
    0x0802B074: "DrawBg2ViewLooping",
    0x0802B168: "DrawBg2EdgeTile",
    0x0802B25C: "RestoreMapColumn",
    0x0802B29C: "RestoreMapCell",
    0x0802B2F0: "SetBlockAnimClipRect",
    0x0802B368: "SetBg1BlockAnimClipRect",
    0x0802B3E4: "SetBlockAnimClipRectWithEdges",
    0x0802B460: "SetBg23ScreenSize",
    0x0802B49C: "SetBg3ScreenSize",
    0x0802B4BC: "CameraFollowFocus",
    0x0802B62C: "CameraSlideToScrollLock",
    0x0802BE80: "CameraFollowScrollLocked",
    0x0802BFF4: "CameraSlideFromScrollLock",
    0x0802C42C: "CameraHoldAnchor",
    0x0802C550: "HubCameraFollowFocus",
    0x0802C680: "HubCameraFollowFocusPlayer",
    0x0802C7F4: "HubCameraGlideToPlayers",
    0x0802CAB0: "CameraSnapBoundsToAnchor",
    0x0802CBB8: "CameraSnapPlayersToAnchor",
    0x0802CC90: "CameraSnapToFocus",
    0x0802CD24: "StopScreenShake",
    0x0802CD38: "UpdateScreenShake",
    0x0802CDA0: "StartScrollLock",
    0x0802D01C: "CameraLeaveScrollLock",
    0x0802D074: "CameraStartHoldAnchor",
    0x0802D0F4: "LoadRoomBgAnims",
    0x0802D188: "UpdateBgAnims",
    0x0802D25C: "BgAnimCopyTiles",
    0x0802D278: "BgAnimStartPaletteFade",
    0x0802D294: "BgAnimStepPaletteFade",
    0x0802D2F0: "SetCollisionTile",
    0x0802D32C: "BgAnimStop",
    0x0802D344: "CreateMapEvent",
    0x0802D370: "Task_MapEvent",
    0x0802D38C: "MapEventMidBossFight",
    0x0802D478: "CreateMapEventBreakTwoBlocks",
    0x0802D4BC: "MapEventBreakTwoBlocks",
    0x0802D5B4: "CreateMapEventBreakThreeBlocks",
    0x0802D5F8: "MapEventBreakThreeBlocks",
    0x0802DCB4: "MapEventStageUnlockPan",
    0x0802E3AC: "MapEventBigSwitchUnlockPan",
    0x0802EAC8: "CreateArenaDoorSign",
    0x0802EB28: "Task_ArenaDoorSign",
    0x0802EBA4: "ArenaDoorSignUpdate",
    0x0802EC1C: "CreateBossDoorSign",
    0x0802EC7C: "Task_BossDoorSign",
    0x0802ED20: "BossDoorSignUpdate",
    0x0802ED94: "CreateDoorOpening",
    0x0802EDE4: "Task_DoorOpening",
    0x0802EE88: "DoorOpeningUpdate",
    0x0802EF90: "CreateStageClearFlag",
    0x0802EFF8: "Task_StageClearFlag",
    0x0802F05C: "CreateQuickDrawDoorSign",
    0x0802F0CC: "Task_QuickDrawDoorSign",
    0x0802F110: "QuickDrawDoorSignAnimate",
    0x0802F1DC: "QuickDrawDoorSignShowStill",
    0x0802F1FC: "CreateBombRallyDoorSign",
    0x0802F26C: "Task_BombRallyDoorSign",
    0x0802F2B0: "BombRallyDoorSignAnimate",
    0x0802F2FC: "BombRallyDoorSignShowStill",
    0x0802F31C: "CreateAirGrindDoorSign",
    0x0802F38C: "Task_AirGrindDoorSign",
    0x0802F3D0: "AirGrindDoorSignAnimate",
    0x0802F400: "AirGrindDoorSignShowStill",
    0x0802F420: "CreateMuseumDoorSign",
    0x0802F480: "Task_MuseumDoorSign",
    0x0802F4C8: "CreateStageDoorSign",
    0x0802F53C: "CreateClearedStageDoorSign",
    0x0802F5B4: "CreateCompletedStageDoorSign",
    0x0802F62C: "Task_StageDoorSign",
    0x0802F684: "StageDoorSignBlinkSignAndDoor",
    0x0802F6C0: "StageDoorSignBlinkDoor",
    0x0802F6F4: "StageDoorSignShowStill",
    0x0802F7DC: "CreateWarpStarStationDoorSign",
    0x0802F84C: "Task_WarpStarStationDoorSign",
    0x0802F8C8: "WarpStarStationDoorSignUpdate",
    0x0802FA3C: "CreateWarpStarStationDoorSparkle",
    0x0802FAA8: "Task_WarpStarStationDoorSparkle",
    0x0802FDF4: "CreateLevelDoorSign",
    0x0802FE64: "Task_LevelDoorSign",
    0x0802FEA4: "DoorObjectDraw",
    0x0802FF70: "SubGameDoorSignDrawUsed",
    0x0802FFE8: "CreateWarpStarStationNumber",
    0x08030034: "Task_WarpStarStationNumber",
    0x08030074: "CreateWarpStarStationLevelSign",
    0x080300C0: "Task_WarpStarStationLevelSign",
    0x08030100: "CreateMuseumAbilitySigns",
    0x08030140: "CreateMuseumAbilitySign",
    0x080301A4: "Task_MuseumAbilitySign",
    0x080301E8: "CreateStageEffect",
    0x08030238: "Task_StageEffect",
    0x080306B4: "QueueWorldSprite",
    0x08030724: "ResetBlockAnims",
    0x08030758: "ResumeBlockAnims",
    0x080307A4: "PauseBlockAnims",
    0x080307B0: "StartBlockAnims",
    0x080307CC: "StartBlockAnimsWithEdges",
    0x080307E8: "StartBg1BlockAnims",
    0x08030804: "TaskBreakBlocksAt",
    0x08030848: "TaskBreakBlocks",
    0x08030898: "TaskBreakFirstBlock",
    0x080308E8: "TaskBreakBlocksNoPlayer",
    0x0803093C: "TaskBreakBlocksAtNoPlayer",
    0x0803097C: "BreakBlocksInHitBoxes",
    0x08030B14: "BreakFirstBlockInHitBox",
    0x08030DB8: "TaskBreakTopBlockRow",
    0x08030E00: "BreakTopBlockRow",
    0x08030F1C: "IsUnbrokenBlockAt",
    0x08030F78: "BreakBlockAt",
    0x0803111C: "CanBreakBlock",
    0x08031374: "BreakBlockAtCursor",
    0x080318B4: "UpdateBlockAnims",
    0x08031994: "FreeBlockAnimAndBlock",
    0x080319B0: "FreeBlockAnim",
    0x080319D0: "BlockAnimWriteAndDrawColumn",
    0x08031AB8: "BlockAnimWriteColumn",
    0x08031B58: "BlockAnimDrawColumn",
    0x08031C38: "BlockAnimWriteMetatile",
    0x08031C7C: "BlockAnimDrawTiles",
    0x08031D04: "BlockAnimBreakNeighbors",
    0x08031DE4: "UpdateBlockAnimsWithEdges",
    0x08031EBC: "BlockAnimWriteMetatileWrapped",
    0x08031F3C: "BlockAnimDrawWithEdges",
    0x08032288: "CanBreakBg1Block",
    0x08032338: "BreakBg1BlockAtCursor",
    0x08032428: "UpdateBg1BlockAnims",
    0x080324E4: "FreeBg1BlockAnimAndBlock",
    0x08032500: "Bg1BlockAnimWriteMetatile",
    0x08032520: "Bg1BlockAnimDrawTiles",
    0x080325B8: "Bg1BlockAnimBreakNeighbors",
    0x08032688: "Task_Player",
    0x08032BD0: "PlayerStartRequestedAction",
    0x08032D48: "PlayerUpdate",
    0x0803380C: "PlayerActionStand",
    0x08033A2C: "PlayerActionStandUpdate",
    0x08033D0C: "PlayerActionWalk",
    0x08034278: "PlayerActionWalkUpdate",
    0x080343C0: "PlayerActionRun",
    0x0803469C: "PlayerActionRunUpdate",
    0x08034874: "PlayerActionSkid",
    0x080349B4: "PlayerActionSkidUpdate",
    0x08034A88: "PlayerActionJump",
    0x08034BEC: "PlayerActionJumpUpdate",
    0x08034D34: "PlayerActionReleaseJump",
    0x08034E60: "PlayerActionReleaseJumpUpdate",
    0x08034F8C: "PlayerActionFall",
    0x08035458: "PlayerActionFallUpdate",
    0x080355D8: "PlayerActionHighFall",
    0x08035848: "PlayerActionHighFallUpdate",
    0x080359F8: "PlayerActionFloat",
    0x08036280: "PlayerActionFloatUpdate",
    0x080366C4: "PlayerActionDuck",
    0x08036888: "PlayerActionDuckUpdate",
    0x080369B0: "PlayerActionSlide",
    0x08036B9C: "PlayerActionSlideUpdate",
    0x08036C94: "PlayerActionLadder",
    0x080371F0: "PlayerActionLadderUpdate",
    0x080375E0: "PlayerActionInhale",
    0x08037914: "PlayerActionInhaleUpdate",
    0x08037BD4: "PlayerActionSpit",
    0x08037CC8: "PlayerActionSpitUpdate",
    0x08037D64: "PlayerActionSwallow",
    0x08037E28: "PlayerActionSwallowUpdate",
    0x08037ED8: "PlayerActionHurt",
    0x08038FE8: "PlayerActionHurtUpdate",
    0x0803919C: "PlayerActionDie",
    0x080397F8: "PlayerActionEnterDoor",
    0x08039C24: "PlayerActionExitDoor",
    0x0803AA40: "PlayerActionExitDoorUpdate",
    0x0803AA64: "PlayerActionSwim",
    0x0803AFCC: "PlayerActionSwimUpdate",
    0x0803B3C4: "PlayerActionStandInWater",
    0x0803B47C: "PlayerActionStandInWaterUpdate",
    0x0803B4F8: "PlayerActionWalkInWater",
    0x0803B6EC: "PlayerActionWalkInWaterUpdate",
    0x0803B768: "PlayerActionSwallowInWater",
    0x0803B828: "PlayerActionSwallowInWaterUpdate",
    0x0803B87C: "PlayerActionSpitInWater",
    0x0803B914: "PlayerActionSpitInWaterUpdate",
    0x0803B9A0: "PlayerActionWaterShot",
    0x0803BBF0: "PlayerActionWaterShotUpdate",
    0x0803BD90: "PlayerActionRecoil",
    0x0803BDD4: "PlayerActionRecoilUpdate",
    0x0803BDE8: "PlayerActionShareItem",
    0x0803C990: "PlayerActionShareItemUpdate",
    0x0803CD60: "PlayerPlayBump",
    0x0803CE98: "PlayerUpdateBlink",
    0x0803D034: "CreatePlayer",
    0x0803D0A0: "InitPlayerState",
    0x0803D1C4: "InitPlayerStateKeepInvincibility",
    0x0803D2D4: "InitPlayerStateKeepMouth",
    0x0803D3D4: "PlayerAccelerateAxis",
    0x0803D494: "PlayerMove",
    0x0803D55C: "PlayerLoadFrameTilesAndPalette",
    0x0803D710: "PlayerLoadFramePalette",
    0x0803D7C4: "PlayerLoadPlayerPalette",
    0x0803D824: "PlayerLoadEndingPlayerPalette",
    0x0803D870: "PlayerGetPlayerPaletteOffset",
    0x0803DFC8: "PlayerDrawWorldLoadTilesAndPalette",
    0x0803E050: "PlayerStopAxes",
    0x0803E080: "PlayerUpdatePaletteFlash",
    0x0803E1B8: "SetPlayerInvulnerability",
    0x0803E2D4: "PlayerUpdateInvulnerability",
    0x0803E34C: "PlaySfxIfLocalPlayer",
    0x0803E374: "PlayerStartSfx",
    0x0803E3AC: "PlayerStopSfx",
    0x0803E3E4: "FreezeOtherTasks",
    0x0803E414: "PlayerUpdateFlip",
    0x0803E448: "PlayerFaceHeldDirection",
    0x0803E4A8: "PlayerSetWaterMotionY",
    0x0803E4EC: "PlayerLand",
    0x0803E5C0: "LoadPlayerBodyBoxRect",
    0x0803E5F8: "LoadPlayerHitBoxSet",
    0x0803E650: "PlayerStartOffsetScript",
    0x0803E7D8: "PlayerUpdateInvincibility",
    0x0803E868: "PlayerEndInvincibility",
    0x0803E8EC: "PlayerUpdateInvincibleFlash",
    0x0803F5FC: "FreePlayerEffectsAndObjects",
    0x0803F6E0: "ShufflePlayerOrder",
    0x0803F7E0: "PlayerGetWaterDoorAnim",
    0x0803F870: "PlayerTurnToHeldDirection",
    0x0803F884: "PlayerGetHeldDirection",
    0x0803F8E8: "PlayerCheckBump",
    0x0803F9C0: "PlayerStopAtCeilingAndWall",
    0x0803F9E8: "PlayerStopAtWall",
    0x0803FA44: "PlayerCheckLanding",
    0x0803FA74: "PlayerCheckDie",
    0x0803FCE4: "PlayerHasCrossedWaterSurface",
    0x0803FD20: "PlayerGetFacingSlope",
    0x0803FD90: "PlayerCheckSkid",
    0x0803FDF4: "PlayerCheckJump",
    0x0803FE68: "PlayerCheckFallOrWater",
    0x0803FEC4: "PlayerCheckDuckOrSwallow",
    0x0803FF7C: "PlayerCheckLadder",
    0x0803FFE0: "PlayerCheckFloat",
    0x08040084: "PlayerCheckAirFloat",
    0x080400C0: "PlayerCheckBButton",
    0x08040264: "PlayerCheckEnterWater",
    0x08040298: "PlayerCheckEnterDoor",
    0x08040340: "PlayerCheckDropAbility",
    0x080403E4: "PlayerCheckStartSwim",
    0x0804042C: "PlayerRequestLocomotion",
    0x08040514: "PlayerCheckShareItem",
    0x08040710: "PlayerRequestStandOrFall",
    0x08040788: "LatchPlayerKeys",
    0x08040808: "CreateLocalPlayerArrow",
    0x0804087C: "PlayerGiveInvincibleCandy",
    0x08040894: "PlayerStartItemShare",
    0x08040B40: "PlayerSetMotionXPreset",
    0x080413A4: "PlayerSetMotionYPreset",
    0x08041438: "MetaKnightActionStand",
    0x080414E8: "MetaKnightActionWalk",
    0x080415C8: "MetaKnightActionWalkUpdate",
    0x080416A0: "MetaKnightActionRun",
    0x08041778: "MetaKnightActionRunUpdate",
    0x080418DC: "MetaKnightActionSkid",
    0x08041940: "MetaKnightActionJump",
    0x08041A2C: "MetaKnightActionJumpUpdate",
    0x08041B8C: "MetaKnightActionReleaseJump",
    0x08041BF0: "MetaKnightActionFall",
    0x08041C30: "MetaKnightActionFallUpdate",
    0x08041D14: "MetaKnightActionFloat",
    0x08041DC8: "MetaKnightActionFloatUpdate",
    0x08041E8C: "MetaKnightActionDuck",
    0x08041F10: "MetaKnightActionSlide",
    0x08042050: "MetaKnightActionSlideUpdate",
    0x08042128: "MetaKnightActionLadder",
    0x08042328: "MetaKnightActionLadderUpdate",
    0x08042580: "MetaKnightActionHurt",
    0x08042980: "MetaKnightActionHurtUpdate",
    0x080429FC: "MetaKnightActionDie",
    0x08042CFC: "MetaKnightActionRecoil",
    0x08042D40: "MetaKnightActionRecoilUpdate",
    0x08042D54: "MetaKnightActionEnterDoor",
    0x08042E98: "MetaKnightActionExitDoor",
    0x08043014: "MetaKnightActionSwim",
    0x0804335C: "MetaKnightActionSwimUpdate",
    0x08043654: "MetaKnightActionStandInWater",
    0x080436AC: "MetaKnightActionWalkInWater",
    0x0804374C: "MetaKnightActionSlash",
    0x08043A88: "MetaKnightActionSlashUpdate",
    0x08043B80: "MetaKnightActionDashSlash",
    0x08043E28: "MetaKnightActionDashSlashUpdate",
    0x08043FA8: "MetaKnightActionUpwardSlash",
    0x080441CC: "MetaKnightActionUpwardSlashUpdate",
    0x08044288: "MetaKnightActionDownThrust",
    0x08044470: "MetaKnightActionDownThrustUpdate",
    0x0804462C: "PlayerActionFire",
    0x08044800: "PlayerActionFireUpdate",
    0x08044878: "PlayerActionSpark",
    0x080449C8: "PlayerActionSparkUpdate",
    0x08044B94: "PlayerActionCutter",
    0x08044C7C: "PlayerActionCutterUpdate",
    0x08044D04: "PlayerActionSword",
    0x08045398: "PlayerActionSwordUpdate",
    0x080455C8: "PlayerActionBurning",
    0x08045A50: "PlayerActionBurningUpdate",
    0x08045C40: "PlayerActionLaser",
    0x08045D18: "PlayerActionLaserUpdate",
    0x08045D34: "PlayerActionMike",
    0x080462F0: "PlayerActionMikeUpdate",
    0x08046330: "PlayerActionWheel",
    0x0804676C: "PlayerActionWheelUpdate",
    0x08046C00: "PlayerActionHammer",
    0x08047270: "PlayerActionHammerUpdate",
    0x080474E8: "PlayerActionParasol",
    0x080477CC: "PlayerActionParasolUpdate",
    0x08047844: "PlayerActionSleep",
    0x08047BD8: "PlayerActionSleepUpdate",
    0x08047C30: "PlayerActionNeedle",
    0x08047E74: "PlayerActionNeedleUpdate",
    0x08047FE8: "PlayerActionGetAbility",
    0x08049484: "PlayerActionGetAbilityUpdate",
    0x08049738: "LoadAbilityTiles",
    0x08049A58: "PlayerLoadSparkTiles",
    0x08049B48: "PlayerActionIce",
    0x08049D1C: "PlayerActionIceUpdate",
    0x08049D94: "PlayerActionFreeze",
    0x08049EDC: "PlayerActionFreezeUpdate",
    0x08049F98: "PlayerActionHiJump",
    0x0804A258: "PlayerActionHiJumpUpdate",
    0x0804A54C: "PlayerActionBeam",
    0x0804A6A0: "PlayerActionBeamUpdate",
    0x0804A6BC: "PlayerActionStone",
    0x0804A970: "PlayerActionStoneUpdate",
    0x0804AB70: "PlayerActionTornado",
    0x0804ADA8: "PlayerActionTornadoUpdate",
    0x0804AF54: "PlayerActionCrash",
    0x0804B474: "PlayerActionCrashUpdate",
    0x0804B5B4: "PlayerActionLight",
    0x0804B818: "PlayerActionLightUpdate",
    0x0804B858: "PlayerActionBackdropHold",
    0x0804C4AC: "PlayerActionBackdropHoldUpdate",
    0x0804C64C: "PlayerActionThrowHold",
    0x0804CA84: "PlayerActionThrowHoldUpdate",
    0x0804CC7C: "PlayerActionUFO",
    0x0804D6D0: "PlayerActionUFOUpdate",
    0x0804DC08: "PlayerActionBackdrop",
    0x0804DF00: "PlayerActionBackdropUpdate",
    0x0804E0E0: "PlayerActionThrow",
    0x0804E3A0: "PlayerActionThrowUpdate",
    0x0804E5A4: "PlayerActionBall",
    0x0804E600: "PlayerActionBallEnterVariant",
    0x0804E640: "PlayerActionBallUpdate",
    0x0804E78C: "PlayerBallTransform",
    0x0804E8F4: "PlayerBallTransformUpdate",
    0x0804E97C: "PlayerBallStand",
    0x0804EA7C: "PlayerBallStandUpdate",
    0x0804EB28: "PlayerBallRoll",
    0x0804EB60: "PlayerBallRollUpdate",
    0x0804ECA4: "PlayerBallSkid",
    0x0804ECEC: "PlayerBallSkidUpdate",
    0x0804EE08: "PlayerBallJump",
    0x0804EF00: "PlayerBallJumpUpdate",
    0x0804EFEC: "PlayerBallBounce",
    0x0804F124: "PlayerBallBounceUpdate",
    0x0804F22C: "PlayerBallFall",
    0x0804F258: "PlayerBallFallUpdate",
    0x0804F30C: "PlayerBallLand",
    0x0804F3E4: "PlayerBallLandUpdate",
    0x0804F450: "PlayerBallRevert",
    0x0804F5BC: "PlayerBallRevertUpdate",
    0x0804F614: "PlayerBallPlayBump",
    0x0804F76C: "PlayerBallGetRollDelay",
    0x0804F79C: "PlayerBallStepRoll",
    0x0804F7F8: "PlayerBallCheckVariant",
    0x0804F8EC: "PlayerBallCheckLanding",
    0x0804F948: "PlayerActionStarRod",
    0x0804FAB0: "PlayerActionStarRodUpdate",
    0x0804FBA4: "PlayerActionStarRodJump",
    0x0804FC98: "PlayerActionStarRodJumpUpdate",
    0x0804FE68: "PlayerActionStarRodFlight",
    0x0804FEE8: "PlayerActionStarRodFlightEnterVariant",
    0x0804FF1C: "PlayerActionStarRodFlightUpdate",
    0x0804FFDC: "PlayerStarRodFlightIntro",
    0x080502F0: "PlayerStarRodFlightIntroUpdate",
    0x08050340: "PlayerStarRodFlightFly",
    0x0805035C: "PlayerStarRodFlightFlyUpdate",
    0x08050418: "PlayerStarRodFlightShoot",
    0x080504D4: "PlayerStarRodFlightShootUpdate",
    0x08050508: "PlayerStarRodFlightHurt",
    0x08050630: "PlayerStarRodFlightHurtUpdate",
    0x08050664: "PlayerStarRodFlightCheckShoot",
    0x080506DC: "PlayerStarRodFlightSteer",
    0x080507BC: "Task_PlayerObject",
    0x08050814: "PlayerObjectVanish",
    0x0805091C: "PlayerObjectLaserBeamVanish",
    0x080509EC: "PlayerObjectAirPuff",
    0x08050C48: "PlayerObjectAirPuffUpdate",
    0x08050D00: "PlayerObjectSpitStar",
    0x08050E84: "PlayerObjectSpitStarUpdate",
    0x08051124: "PlayerObjectSpitMultiStar",
    0x080512F8: "PlayerObjectSpitMultiStarUpdate",
    0x080514F8: "PlayerObjectWaterShot",
    0x0805176C: "PlayerObjectWaterShotUpdate",
    0x0805181C: "PlayerObjectFireBreath",
    0x08051B0C: "PlayerObjectFireBreathUpdate",
    0x08051C1C: "PlayerObjectCutterBlade",
    0x08051D84: "PlayerObjectCutterBladeUpdate",
    0x08051F4C: "PlayerObjectLaserBeam",
    0x080520DC: "PlayerObjectLaserBeamUpdate",
    0x0805239C: "PlayerObjectIceBreath",
    0x0805268C: "PlayerObjectIceBreathUpdate",
    0x080527A4: "PlayerObjectBeamOrb",
    0x08052B08: "PlayerObjectBeamOrbUpdate",
    0x08052B88: "PlayerObjectLightOrb",
    0x08052F6C: "PlayerObjectUFOShot",
    0x08053380: "PlayerObjectUFOShotUpdate",
    0x080534D0: "PlayerObjectUFOShotLateUpdate",
    0x080535B0: "PlayerObjectStarRodShot",
    0x080536DC: "PlayerObjectStarRodShotUpdate",
    0x080537DC: "PlayerObjectStarRodFlightShot",
    0x080538CC: "PlayerObjectStarRodFlightShotUpdate",
    0x08053940: "CreatePlayerObject",
    0x08053A44: "CreatePlayerObjectLowSlot",
    0x08053AF4: "Task_PlayerEffect",
    0x08053B40: "PlayerEffectInhaleAir",
    0x08053BE0: "PlayerEffectInhaleAirUpdate",
    0x08053C1C: "PlayerEffectInhaleAirDraw",
    0x08053C48: "PlayerEffectCatchDust",
    0x08053D08: "PlayerEffectSpitDust",
    0x08053DB8: "PlayerEffectAbilityGetSparkle",
    0x08053E38: "PlayerEffectImpactStar",
    0x08053F70: "PlayerEffectDeathStar",
    0x080540D0: "PlayerEffectSkidDust",
    0x08054298: "PlayerEffectSkidDustUpdate",
    0x08054330: "PlayerEffectRunDust",
    0x08054504: "PlayerEffectRunDustUpdate",
    0x08054538: "PlayerEffectSlideDust",
    0x08054838: "PlayerEffectSlideDustUpdate",
    0x0805485C: "PlayerEffectSplash",
    0x080548F0: "PlayerEffectLeaveWaterSplash",
    0x080549A4: "PlayerEffectBubble",
    0x08054A80: "PlayerEffectDeathStarRing",
    0x08054B98: "PlayerEffectDeathStarRingLateUpdate",
    0x08054D94: "PlayerEffectMetaKnightDeathBlast",
    0x0805569C: "PlayerEffectDanceStarBurst",
    0x08055A40: "PlayerEffectLocalPlayerArrow",
    0x08055ABC: "PlayerEffectLocalPlayerArrowUpdate",
    0x08055B24: "PlayerEffectHurtFlames",
    0x08055D24: "PlayerEffectHurtFlamesUpdate",
    0x08055D74: "PlayerEffectHurtSparks",
    0x080560FC: "PlayerEffectHurtSparksUpdate",
    0x08056448: "PlayerEffectHurtBurst",
    0x08056DD4: "PlayerEffectFireBreathFlames",
    0x0805707C: "PlayerEffectFireBreathFlamesUpdate",
    0x0805710C: "PlayerEffectSparkAura",
    0x080573A4: "PlayerEffectSparkAuraUpdate",
    0x08057430: "PlayerEffectSwordSparkle",
    0x08057494: "PlayerEffectBurningFlames",
    0x08057A10: "PlayerEffectBurningFlamesUpdate",
    0x08057A48: "PlayerEffectUFOLaserTrail",
    0x08057CE0: "PlayerEffectHammerDust",
    0x08057E90: "PlayerEffectParasolSparkle",
    0x08057F90: "PlayerEffectMikeAttack",
    0x08058410: "PlayerEffectMikeAttackUpdate",
    0x08058460: "PlayerEffectSleepBubble",
    0x080586FC: "PlayerEffectSleepBubbleUpdate",
    0x08058810: "PlayerEffectIceBreathCloud",
    0x08058E80: "PlayerEffectIceBreathCloudUpdate",
    0x08058F10: "PlayerEffectFreezeAura",
    0x080594E0: "PlayerEffectFreezeAuraUpdate",
    0x08059C28: "PlayerEffectStonePuff",
    0x0805A358: "PlayerEffectTornadoDust",
    0x0805A508: "PlayerEffectTornadoDustUpdate",
    0x0805A52C: "PlayerEffectCrashBlast",
    0x0805AB04: "PlayerEffectCrashBlastUpdate",
    0x0805AC50: "PlayerEffectCrashBlastDraw",
    0x0805AE94: "PlayerEffectUFOChargeSparkle",
    0x0805AF44: "PlayerEffectUFOChargeSparkleUpdate",
    0x0805AFAC: "CreatePlayerEffect",
    0x0805B088: "CreatePlayerEffectHighSlot",
    0x0805B110: "GoalGameMain",
    0x0805B16C: "GoalGameInit",
    0x0805B278: "PlayerGoalGameInit",
    0x0805B354: "PlayerGoalGameEnterState",
    0x0805B4BC: "PlayerGoalGameUpdate",
    0x0805B4D8: "PlayerGoalGameState0",
    0x0805B508: "PlayerGoalGameState0Update",
    0x0805B514: "PlayerGoalGameWaitForPress",
    0x0805B534: "PlayerGoalGameWaitForPressUpdate",
    0x0805B5B0: "PlayerGoalGameCheckPress",
    0x0805B61C: "PlayerGoalGameSetLaunchPower",
    0x0805B644: "PlayerGoalGameState2",
    0x0805B660: "PlayerGoalGameState2Update",
    0x0805B670: "PlayerGoalGameWaitForLaunch",
    0x0805B688: "PlayerGoalGameWaitForLaunchUpdate",
    0x0805B6C0: "PlayerGoalGameLaunch",
    0x0805B788: "PlayerGoalGameLaunchUpdate",
    0x0805B8B8: "PlayerGoalGameAddToLayerSign",
    0x0805B998: "PlayerGoalGameState5Update",
    0x0805B9A4: "PlayerGoalGameFall",
    0x0805B9C0: "PlayerGoalGameFallUpdate",
    0x0805BA08: "PlayerGoalGameLand",
    0x0805BB84: "PlayerGoalGameLandUpdate",
    0x0805BB90: "PlayerGoalGameWalkToSpotUpdate",
    0x0805BC1C: "PlayerGoalGameWait",
    0x0805BC50: "PlayerGoalGameWaitUpdate",
    0x0805BC5C: "PlayerGoalGameWaitLateUpdate",
    0x0805BCA4: "PlayerGoalGameRemoveFromLayerSign",
    0x0805BCE0: "PlayerGoalGameDance",
    0x0805BD28: "PlayerGoalGameDanceUpdate",
    0x0805BD34: "PlayerGoalGameFinish",
    0x0805BE3C: "PlayerGoalGameFinishUpdate",
    0x0805BE48: "PlayerGoalGameDrawPressPrompt",
    0x0805BEB0: "Task_GoalGameLaunchStars",
    0x0805C0A8: "GoalGameLaunchStarsFall",
    0x0805C114: "GoalGameLaunchStarsUpdate",
    0x0805C150: "GoalGameLaunchStarsDraw",
    0x0805C204: "Task_GoalGameBigTrailStar",
    0x0805C410: "Task_GoalGameSmallTrailStar",
    0x0805C584: "PlayerGoalGameRideSpring",
    0x0805C5FC: "Task_GoalGameCamera",
    0x0805C814: "GoalGameCameraFollowPlayer",
    0x0805C990: "GoalGameCameraUpdate",
    0x0805CB30: "Task_GoalGameSpring",
    0x0805CBEC: "Task_GoalGamePlayerMarker",
    0x0805CC54: "GoalGamePlayerMarkerFollowParent",
    0x0805CCA0: "Task_GoalGameSign",
    0x0805CF3C: "Task_GoalGameHelperKirby",
    0x0805D420: "GoalGameHelperKirbyUpdate",
    0x0805D668: "Task_GoalGameOneUp",
    0x0805D8A4: "TaskStartFrameScript",
    0x0805D8C4: "TaskStartFrameScriptId",
    0x0805D8F4: "TaskUpdateFrameScript",
    0x0805D918: "TaskAdvanceFrameScript",
    0x0805D994: "GoalGameHelperKirbyInitSprite",
    0x0805DA2C: "GoalGameHelperKirbyDraw",
    0x0805DEAC: "StartAllPlayersDance",
    0x0805DFE8: "PlayerWalkToDanceSpotUpdate",
    0x0805E038: "PlayerSetDanceSpot",
    0x0805E110: "PlayerWalkToDanceSpot",
    0x0805E15C: "PlayerDance",
    0x0805E1BC: "PlayerDanceInGoalGame",
    0x0805E24C: "PlayerDanceAfterStageClear",
    0x0805E7B4: "PlayerDance1",
    0x0805EB2C: "PlayerDance2",
    0x0805EE90: "PlayerDance3",
    0x0805F1BC: "PlayerDance4",
    0x0805F778: "PlayerDance5",
    0x0805FB88: "PlayerDance7",
    0x08060308: "PlayerDance8",
    0x08060C2C: "PlayerDance9",
    0x080613E4: "PlayerDance10",
    0x08061CAC: "PlayerDance11",
    0x08062584: "PlayerDance12",
    0x08062F88: "PlayerDance13",
    0x08063698: "TaskCreatePausedInScreenAttack",
    0x080636E4: "ActorInitSlot",
    0x08063704: "ActorBindDefSlot",
    0x080637AC: "ActorResetHealthSlot",
    0x080637CC: "ActorResetHealth",
    0x080637E4: "ActorInitFromDefSlot",
    0x08063908: "ActorLoadDef",
    0x0806391C: "ActorLoadDefSlot",
    0x0806395C: "ActorSetState",
    0x08063974: "ActorSetStateSlot",
    0x08063990: "ActorSetTerrainHandlers",
    0x080639A4: "ActorSetHitReactions",
    0x080639B4: "ActorSetAttackBox",
    0x080639C8: "ActorSetAttackBoxSlot",
    0x080639E0: "ActorSetTerrainBox",
    0x080639F0: "ActorSetAux",
    0x08063A00: "ActorSetExtraAttackBox",
    0x08063A14: "ActorSetExtraAttackBoxSlot",
    0x08063A2C: "TaskFindNearestPlayerObject",
    0x08063A9C: "TaskFindNearestPlayerSlot",
    0x08063B38: "TaskFindNearestPlayer",
    0x08063BD4: "GetDistSq",
    0x08063BF4: "GetTaskDistSq",
    0x08063C60: "TaskGetDistSqTo",
    0x08063C74: "TaskGetNearestPlayerDistSq",
    0x08063C94: "GetTaskDx",
    0x08063CBC: "TaskGetDxTo",
    0x08063CD0: "TaskGetNearestPlayerDx",
    0x08063CF0: "GetTaskDy",
    0x08063D18: "TaskGetDyTo",
    0x08063D2C: "TaskGetNearestPlayerDy",
    0x08063D4C: "TaskGetPosSlot",
    0x08063D7C: "TaskGetNearestPlayerPos",
    0x08063DAC: "TaskGetFacingToward",
    0x08063DDC: "TaskFaceToward",
    0x08063DF4: "TaskGetFacingTowardNearestPlayer",
    0x08063E14: "TaskFaceNearestPlayer",
    0x08063E2C: "TaskIsInRect",
    0x08063E74: "IsPointInRect",
    0x08063EB0: "TaskIsInRectSlot",
    0x08063F00: "TaskIsNearestPlayerInRect",
    0x08063F24: "ActorDestroySlot",
    0x08063FE0: "ActorDestroy",
    0x08063FF4: "TaskTurnAroundAndReverseX",
    0x08064038: "TaskTurnAround",
    0x0806406C: "TaskToggleFacingAndReverseX",
    0x080640A8: "ActorStartAnimNoFlip",
    0x080640C8: "ActorStopAnim",
    0x080640DC: "ActorStartAnim",
    0x080640FC: "ActorStepAnim",
    0x0806415C: "ActorTickAnimFacingNearestPlayer",
    0x08064188: "ActorTickAnim",
    0x0806421C: "AngleToVector",
    0x0806425C: "GetPointAngle",
    0x080642B0: "GetTaskAngle",
    0x080642FC: "TaskGetAngleTo",
    0x08064314: "TaskGetAngleToNearestPlayer",
    0x0806433C: "TaskGetYDirBitTo",
    0x08064358: "TaskGetYDirBitToNearestPlayer",
    0x0806437C: "TaskGetXDirBitTo",
    0x08064398: "TaskGetXDirBitToNearestPlayer",
    0x080643BC: "TaskGetCompassDirTo",
    0x0806443C: "TaskGetCompassDirToNearestPlayer",
    0x08064460: "TaskAccelerateAxisPlus",
    0x080644C0: "TaskAccelerateAxisMinus",
    0x0806453C: "TaskDecelerateAxis",
    0x080645A4: "TaskAccelerateTowardNearestPlayer",
    0x08064680: "TaskAccelerateInDir",
    0x08064758: "TaskFindNearestPlayerInScreenXBand",
    0x080647FC: "TaskFindNearestPlayerInScreenYBand",
    0x080648A0: "TaskGetScreenPosSlot",
    0x0806493C: "TaskGetNearestPlayerScreenPos",
    0x08064970: "TaskGetScreenPos",
    0x08064984: "TaskIsNearestPlayerWithinX",
    0x080649B4: "ActorAwardScore",
    0x08064A38: "TaskGetParentFacing",
    0x08064A60: "TaskFaceLikeParent",
    0x08064A78: "CreateChildActor",
    0x08064B28: "CreateActorFromDescHere",
    0x08064B5C: "CreateActorFromDescAtOffsetFacing",
    0x08064BA8: "CreateActorFromDesc",
    0x08064BCC: "TaskStepSpinFrameFacing",
    0x08064C1C: "CreateChildTask",
    0x08064CDC: "CreateChildTaskAtOffsetFacing",
    0x08064D34: "CreateChildTaskHere",
    0x08064D6C: "CreateChildTaskAt",
    0x08064D9C: "CreateItemOrObject",
    0x08064E5C: "CreateItemHere",
    0x08064E90: "CreateItemAt",
    0x08064EB8: "CreateAbilityStar",
    0x08064F28: "CreateActor",
    0x08064FC4: "CreateActorByKind",
    0x0806505C: "CreateChildActorOfSameType",
    0x08065100: "CreateBlockStar",
    0x08065160: "ActorIsInView",
    0x080651B4: "ActorDrawWorldInView",
    0x0806523C: "ActorDrawWorldInViewOrDestroy",
    0x080652C8: "ActorDrawWorldNearView",
    0x08065350: "ActorDrawWorldNearViewOrDestroy",
    0x080653EC: "ActorDrawStreamedFrameNearViewOrDestroy",
    0x08065438: "ActorDrawStreamedFrameNearView",
    0x08065470: "ActorDrawStreamedFrame",
    0x0806555C: "ActorDrawSpriteAndExtra",
    0x08065640: "ActorDrawWorldInViewOrDestroyWithExtra",
    0x0806567C: "ActorDrawSpriteAndExtraInView",
    0x080656B4: "ActorMove",
    0x0806572C: "TaskMoveRelativeToView",
    0x08065760: "SetPaletteAnimSource",
    0x080657A4: "ResetBgPaletteBlend",
    0x080657CC: "EndBgPaletteBlend",
    0x080657F8: "StartBgPaletteBlend",
    0x08065848: "AcquirePaletteAnim",
    0x080658B8: "Task_PaletteAnim",
    0x080658D8: "PaletteAnimVariant0",
    0x080659B4: "PaletteAnimCycle",
    0x08065A68: "PaletteAnimFlashColor",
    0x08065B14: "PaletteAnimVariant3",
    0x08065C14: "PaletteAnimBgBlend",
    0x08065CE0: "ActorSelectPaletteVariant",
    0x08065D44: "LoadActorPaletteVariant",
    0x08065DBC: "LoadEnemyPaletteVariant",
    0x08065E1C: "ActorPlaySfx",
    0x08065E6C: "LockAllActorPalettes",
    0x08065ED0: "ClearActorPaletteOverrides",
    0x08065F2C: "TaskHasSameSerial",
    0x08065F5C: "ActorComputeHealth",
    0x08065F74: "ActorComputeHealthSlot",
    0x08066088: "ActorInitBossGfx",
    0x0806619C: "BossStartHitStun",
    0x0806621C: "BossEndHitStun",
    0x0806627C: "BossHitStunShake",
    0x080662D8: "BossHitStunLateUpdate",
    0x08066338: "AreAllPlayersOnGround",
    0x08066394: "GetLivingActivePlayerHealth",
    0x080663F4: "ActorFlashPalette",
    0x08066468: "ActorClearPaletteOverride",
    0x08066480: "ActorFlashHeaderPalette",
    0x080664CC: "ActorLoadHeaderPalette",
    0x080664E0: "ActorIntroPoseUntilHpBarFull",
    0x08066564: "ActorResetAttackBox",
    0x080665A0: "ActorShowHpBar",
    0x080665FC: "ActorGetGfxTileWord",
    0x0806660C: "ActorGetGfxTileWordPalOffset",
    0x08066630: "ActorGetTileWordPalOffset",
    0x08066658: "ActorStartIntroPose",
    0x080666A4: "ActorEndIntroPose",
    0x080666CC: "ActorIntroPoseUntilMidBossFight",
    0x080666F8: "ActorIntroPoseUntilScrollLocked",
    0x08066718: "BossCheckScrollLock",
    0x08066754: "ActorIntroPoseUpdate",
    0x08066798: "ArenaDropMaximTomato",
    0x080667C0: "MidBossStartDefeat",
    0x0806684C: "EndMidBossFightWithReward",
    0x0806685C: "ActorLoadPalette",
    0x080668C8: "BossDefeatSweep",
    0x08066A0C: "ActorIsInViewMargin",
    0x08066A6C: "ActorIsInNearView",
    0x08066A80: "ActorIsInFarView",
    0x08066A94: "ActorSetDefaultPalette",
    0x08066AE0: "MidBossResetHealth",
    0x08066B34: "ActorStartCarryingParasol",
    0x08066B70: "TaskBounceParasolDriftOffWall",
    0x08066BA8: "TaskStartParasolDrift",
    0x08066BDC: "TaskStepParasolDrift",
    0x08066C08: "ActorDropParasol",
    0x08066C3C: "ActorDropParasolOnLanding",
    0x08066C74: "ActorDrawWorldInViewOrDestroyWithParasol",
    0x08066DCC: "ActorGetParasolOffset",
    0x08066E88: "CreateDroppedParasol",
    0x08066F50: "CreateNextRoomWarpStar",
    0x08066F78: "LoadStarRodPieceGfx",
    0x08066FC0: "CreateStarRodPiece",
    0x0806704C: "CreateRoomStarRodPiece",
    0x08067060: "IsMidBossDroppingIn",
    0x08067074: "CountActivePlayers",
    0x080670AC: "FreezeStage",
    0x080670D4: "ThawStage",
    0x080670F0: "LoadBackdropColor",
    0x08067108: "DisablePause",
    0x08067114: "EnablePause",
    0x08067120: "CreateInhalableStar",
    0x08067170: "InhalableStarInitVariant",
    0x080671C0: "Task_InhalableStar",
    0x08067214: "InhalableStarUpdate",
    0x08067258: "InhalableStarState0",
    0x08067378: "InhalableStarState0Update",
    0x0806737C: "HeldPlayerInit",
    0x080673EC: "HeldPlayerEnterState",
    0x08067408: "HeldPlayerUpdate",
    0x08067470: "HeldPlayerSwallow",
    0x080674A8: "HeldPlayerSwallowUpdate",
    0x080674D8: "HeldPlayerSpitFlight",
    0x080674F4: "HeldPlayerSpitFlightUpdate",
    0x080674F8: "HeldPlayerSpitBounceOff",
    0x08067520: "HeldPlayerSpitBounceOffUpdate",
    0x08067550: "HeldPlayerBackdropHeld",
    0x080675D8: "HeldPlayerBackdropHeldUpdate",
    0x080675E4: "HeldPlayerFollowCaptorPose",
    0x080676C0: "HeldPlayerBackdropBounceOff",
    0x08067908: "HeldPlayerBackdropBounceOffUpdate",
    0x08067950: "HeldPlayerState5",
    0x08067A48: "HeldPlayerState5Update",
    0x08067B24: "HeldPlayerState6",
    0x08067D30: "HeldPlayerState6Update",
    0x08067D78: "HeldPlayerThrowHeld",
    0x08067DB0: "HeldPlayerThrowHeldUpdate",
    0x08067EA4: "HeldPlayerThrowFlightForward",
    0x08068028: "HeldPlayerThrowFlightForwardUpdate",
    0x080680AC: "HeldPlayerThrowFlightBackward",
    0x08068224: "HeldPlayerThrowFlightBackwardUpdate",
    0x080682A8: "HeldPlayerThrowBounceOff",
    0x08068460: "HeldPlayerThrowBounceOffUpdate",
    0x080684A4: "HeldPlayerAddAbilityFrameOffset",
    0x080685EC: "HoldPlayer",
    0x0806865C: "DropHeldPlayer",
    0x08068690: "HeldPlayerStartSwallow",
    0x08068760: "HeldPlayerPullTowardCaptor",
    0x080687A0: "HeldPlayerIsNearCaptor",
    0x080687E0: "HeldPlayerEnterMouth",
    0x080687FC: "HeldPlayerSwallowAdvanceFrame",
    0x08068828: "HeldPlayerStartSpitFlight",
    0x08068840: "HeldPlayerStartSpitBounceOff",
    0x08068920: "SetHeldPlayerState",
    0x0806896C: "HeldPlayerStepTumble",
    0x080689C8: "ReleaseHeldPlayer",
    0x08068A2C: "HeldPlayerDamage",
    0x08068A8C: "PlayerSuspendControl",
    0x08068B88: "PlayerResumeControl",
    0x08068CB4: "ActorTestColliders",
    0x08068CF8: "ActorCheckHitsWithBox",
    0x08068E04: "ActorCheckHits",
    0x08068F68: "ActorCheckHitsWithExtraBox",
    0x0806914C: "ActorCheckPlayerHitsWithBox",
    0x08069234: "ActorStoreHit",
    0x080692FC: "ActorCollideTerrain",
    0x0806951C: "ActorCollideTerrainAlongVelocity",
    0x0806956C: "ActorCollideTerrainCeilingAndFloor",
    0x080695BC: "ActorCollideTerrainWalls",
    0x08069604: "ActorCollideTerrainPointPushOut",
    0x08069660: "ActorCollideTerrainPointStop",
    0x080696A0: "ActorCollideTerrainInCameraBounds",
    0x08069888: "ActorCollideTerrainFloor",
    0x080699A8: "ActorStepBackFromSlope",
    0x08069A64: "ActorGetTerrainBox",
    0x08069AC4: "ActorInitTerrainFlagsSlot",
    0x08069AE4: "ActorReactToHitKind",
    0x08069B44: "ActorReactToHit",
    0x08069B84: "ActorReactToHitOrTerrainDamage",
    0x08069BBC: "PickupReactToHit",
    0x08069C48: "ActorHitKindWithTerrainDamage",
    0x08069C8C: "ActorPlayHitSfx",
    0x08069D78: "ActorPlayDefaultHitSfx",
    0x08069DC4: "ActorStartHitStun",
    0x08069E48: "ActorEndHitStun",
    0x08069EA0: "ActorReactToDamage",
    0x08069F0C: "ActorHitStunShake",
    0x08069F70: "ActorHitStunBlink",
    0x08069FB0: "ActorHitStunLateUpdate",
    0x08069FC8: "ActorPlayBurstDefeatAnim",
    0x0806A008: "ActorFaceHitter",
    0x0806A03C: "TaskGetHitAngle",
    0x0806A0CC: "ActorPlayRandomDefeatSfx",
    0x0806A0F0: "ActorStartDrown",
    0x0806A158: "PickupCollect",
    0x0806A25C: "ActorReactToDefeat",
    0x0806A344: "ActorDie",
    0x0806A3AC: "ActorDefeatByEffect",
    0x0806A3DC: "ActorDefeatKnockAway",
    0x0806A488: "ActorDefeatBlinkAndBurst",
    0x0806A500: "ActorDefeatPlain",
    0x0806A524: "ActorDefeatPlainUpdate",
    0x0806A530: "ActorDefeatBurning",
    0x0806A55C: "ActorDefeatBurningUpdate",
    0x0806A568: "ActorDefeatShocked",
    0x0806A594: "ActorDefeatShockedUpdate",
    0x0806A5A0: "ActorDefeatFrozenBlink",
    0x0806A638: "ActorFreezeIntoIceBlock",
    0x0806A6A0: "ActorDefeatFrozenCheckFreezerAbility",
    0x0806A6E0: "ActorDefeatFrozenCheckPush",
    0x0806A7A0: "ActorDefeatFrozen",
    0x0806A7F4: "ActorDefeatFrozenUpdate",
    0x0806A8D8: "ActorDefeatFrozenEnterState",
    0x0806A8F4: "ActorDefeatFrozenShake",
    0x0806A958: "ActorDefeatFrozenShakeUpdate",
    0x0806A980: "ActorDefeatFrozenSlide",
    0x0806A9D4: "ActorDefeatFrozenSlideUpdate",
    0x0806A9D8: "ActorDefeatFrozenBurst",
    0x0806AA10: "ActorDefeatExplodeByEffect",
    0x0806AA40: "ActorDefeatExplode",
    0x0806AA80: "ActorExplodeDefeatBurning",
    0x0806AA8C: "ActorExplodeDefeatShocked",
    0x0806AA98: "ActorExplodeDefeatFrozen",
    0x0806AAA4: "ExplosionScreenFlash",
    0x0806AB34: "ActorDefeat2",
    0x0806ABA4: "ActorDefeat2Update",
    0x0806ABEC: "ActorDefeat3",
    0x0806AC48: "ActorDefeat3Update",
    0x0806AC6C: "ActorDefeat4",
    0x0806ACC4: "ActorDefeat4Update",
    0x0806ACC8: "ActorDefeatAbilityStar",
    0x0806ACF8: "ActorHasExtraFrame",
    0x0806AD18: "ActorShakeVertically",
    0x0806ADB0: "MidBossDefeatScreenFlash",
    0x0806AE94: "MidBossDefeatFlash",
    0x0806AEC0: "ActorDefeatMidBoss",
    0x0806AF78: "BossDefeatScreenFlash",
    0x0806B05C: "BossDefeatFlash",
    0x0806B070: "BossRunDefeatHook",
    0x0806B098: "BossDefeatExplode",
    0x0806B0F0: "BossDefeatStopBgm",
    0x0806B12C: "ActorDefeatBoss",
    0x0806B178: "ActorDefeatBurstByEffect",
    0x0806B1A8: "ActorBurstDefeatPlain",
    0x0806B1C4: "ActorBurstDefeatBurning",
    0x0806B1F4: "ActorBurstDefeatShocked",
    0x0806B224: "ActorBurstDefeatFrozen",
    0x0806B230: "ActorDefeat9",
    0x0806B24C: "ActorDrownLand",
    0x0806B26C: "ActorDrownInit",
    0x0806B2AC: "ActorDrownUpdate",
    0x0806B2E4: "ActorDrownEnterState",
    0x0806B300: "ActorDrownSink",
    0x0806B330: "ActorDrownSinkUpdate",
    0x0806B334: "ActorDrownWait",
    0x0806B368: "ActorDrownWaitUpdate",
    0x0806B390: "ActorDrownBurst",
    0x0806B3C0: "ActorDrownBurstUpdate",
    0x0806B3C4: "ActorDefeatPickup",
    0x0806B410: "ActorAttachedThrowHeldFollowCarrier",
    0x0806B670: "ActorAttachedBackdropHeldFollowCarrier",
    0x0806B848: "ActorAttachedHeldAddPlayerOffset",
    0x0806B878: "ActorAttachedRestorePalette",
    0x0806B8BC: "ActorAttachedDie",
    0x0806B938: "ActorAttachedDieUnlessInhaling",
    0x0806B9DC: "ActorAttachedBindCarrier",
    0x0806BA34: "ActorAttachedReloadPalette",
    0x0806BA9C: "TaskSetPosRelativeToParent",
    0x0806BAEC: "TaskIsParentWithinX",
    0x0806BB7C: "ActorAttachToHitter",
    0x0806BBE8: "ActorAttachedPullTowardCarrier",
    0x0806BC28: "ActorAttachedPullInStep",
    0x0806BC54: "ActorInitCarryOffset",
    0x0806BCDC: "ActorAttachedSwallowStep",
    0x0806BD10: "ActorAttachedEnterMouth",
    0x0806BE4C: "ActorAttachedReleaseCarrierSlot",
    0x0806BE84: "ActorAttachedCheckScreenAttack",
    0x0806BEFC: "ActorAttachedEnterState",
    0x0806BF38: "ActorAttachedRunState",
    0x0806BF54: "ActorAttachedSwallow",
    0x0806BFD8: "ActorAttachedSwallowLateUpdate",
    0x0806C05C: "ActorAttachedBackdropHeld",
    0x0806C0C0: "ActorAttachedBackdropHeldUpdate",
    0x0806C148: "ActorAttachedBackdropHeldLateUpdate",
    0x0806C158: "ActorAttachedBackdropFlight",
    0x0806C1D0: "ActorAttachedBackdropFlightUpdate",
    0x0806C2A4: "ActorAttachedBackdropBounceOff",
    0x0806C30C: "ActorAttachedBackdropBounceOffUpdate",
    0x0806C324: "ActorAttachedPullIn",
    0x0806C384: "ActorAttachedPullInLateUpdate",
    0x0806C3C4: "ActorAttachedThrowHeld",
    0x0806C418: "ActorAttachedThrowHeldUpdate",
    0x0806C490: "ActorAttachedThrowHeldLateUpdate",
    0x0806C4A0: "ActorAttachedThrowFlight",
    0x0806C5D4: "ActorAttachedThrowFlightUpdate",
    0x0806C770: "ActorAttachedThrowFlightLateUpdate",
    0x0806C930: "ActorAttachedThrowBounceOff",
    0x0806C9E8: "ActorAttachedThrowBounceOffUpdate",
    0x0806CA00: "Task_ActorSplash",
    0x0806CAA0: "CreateStarFlash",
    0x0806CB10: "Task_StarFlash",
    0x0806CB64: "Task_StarFlashOnParent",
    0x0806CBD4: "StarFlashFollowParent",
    0x0806CC90: "CreateDustTrail",
    0x0806CD30: "CreateDustPuff",
    0x0806CD40: "Task_DustTrail",
    0x0806CEB8: "Task_DustPuff",
    0x0806CF70: "Task_BackwardDustPuff",
    0x0806CFFC: "CreateLandingDust",
    0x0806D08C: "CreateLandingDustFacing",
    0x0806D148: "Task_LandingDust",
    0x0806D1E8: "CreateDustBurst",
    0x0806D22C: "Task_DustBurst",
    0x0806D49C: "DustBurstCheckParent",
    0x0806D4E4: "CreateBurstEffect",
    0x0806D554: "Task_StarScatter",
    0x0806D564: "Task_RayBurst",
    0x0806D574: "Task_SmallBlast",
    0x0806D584: "BurstStickToParent",
    0x0806D5A4: "Task_StarScatterOnParent",
    0x0806D5B8: "Task_RayBurstOnParent",
    0x0806D5CC: "Task_SmallBlastOnParent",
    0x0806D5E0: "BurstStickToParentUpdate",
    0x0806D65C: "PlayRayBurstAnim",
    0x0806D6E4: "PlayStarScatterAnim",
    0x0806D730: "PlayExplosionAnim",
    0x0806D77C: "PlaySmallBlastAnim",
    0x0806D7EC: "Task_ImpactStar",
    0x0806D928: "CreateStarRing",
    0x0806D95C: "Task_RingStar",
    0x0806D9D4: "RingStarUpdate",
    0x0806DA04: "Task_BossScreenFlash",
    0x0806DA20: "Task_ExplosionScreenFlash",
    0x0806DA3C: "CreateCannonSmoke",
    0x0806DA74: "CannonSmokeInit",
    0x0806DAEC: "Task_CannonSmoke",
    0x0806DCA0: "CannonFuseSparkDraw",
    0x0806DD90: "Task_CannonFuseSpark",
    0x0806DE18: "CannonFuseSparkCheckParent",
    0x0806DE60: "Task_TrailFlash",
    0x0806DEDC: "ActorUpdateAttachedEffect",
    0x0806DF28: "ActorAttachEffect",
    0x0806DF98: "Task_HitFrost",
    0x0806E0F0: "HitFrostCheckParent",
    0x0806E138: "Task_HitFlames",
    0x0806E210: "HitFlamesCheckParent",
    0x0806E258: "Task_HitSparks",
    0x0806E3DC: "HitSparksCheckParent",
    0x0806E424: "Task_WarpStarSparkle",
    0x0806E5F0: "WarpStarSparkleCheckParent",
    0x0806E638: "Task_AbilityReleaseFlash",
    0x0806E6B0: "AbilityReleaseFlashCheckParent",
    0x0806E6F8: "CreateDashFlame",
    0x0806E73C: "Task_DashFlame",
    0x0806E7C0: "DashFlameCheckParent",
    0x0806E808: "CreateDashFireTrail",
    0x0806E84C: "Task_DashFireTrail",
    0x0806E96C: "DashFireTrailCheckParent",
    0x0806E9B4: "CreateLandingImpact",
    0x0806E9FC: "Task_LandingImpact",
    0x0806EA70: "LandingImpactCenter",
    0x0806EB04: "LandingImpactLeft",
    0x0806EBA4: "LandingImpactRight",
    0x0806EC40: "Task_IceBlock",
    0x0806EC88: "IceBlockUpdate",
    0x0806ED28: "IceBlockBlink",
    0x0806ED9C: "PlaySmokeRingAnim",
    0x0806EE1C: "Task_SmokeRing",
    0x0806EE30: "PlayerWarpStarRideInit",
    0x0806EF1C: "PlayerWarpStarRideUpdate",
    0x0806EF38: "PlayerWarpStarRideEnterState",
    0x0806EF5C: "PlayerWarpStarRideState2",
    0x0806EFE8: "PlayerWarpStarRideState2Update",
    0x0806EFEC: "PlayerWarpStarRideState1",
    0x0806F174: "PlayerWarpStarRideState1Update",
    0x0806F1E0: "PlayerWarpStarRideState3",
    0x0806F36C: "PlayerWarpStarRideState3Update",
    0x0806F3D8: "PlayerWarpStarRideState4",
    0x0806F5C4: "PlayerWarpStarRideState4Update",
    0x0806F638: "PlayerWarpStarRideState5",
    0x0806FAAC: "PlayerWarpStarRideState5Update",
    0x0806FB0C: "PlayerWarpStarRideState6",
    0x0806FC98: "PlayerWarpStarRideState6Update",
    0x0806FD04: "PlayerWarpStarRideState7",
    0x0806FF24: "PlayerWarpStarRideState7Update",
    0x0806FF7C: "PlayerWarpStarRideDraw",
    0x080700E8: "PlayerSetWarpStarRideFrame",
    0x0807029C: "PlayerStartTumble",
    0x08070334: "PlayerStepTumble",
    0x08070454: "TaskCopyParentPixelPos",
    0x08070498: "PlayerBoardWarpStar",
    0x08070614: "PlayerEndRideLanding",
    0x08070648: "MetaKnightWarpStarRideInit",
    0x0807073C: "MetaKnightWarpStarRideUpdate",
    0x08070758: "MetaKnightWarpStarRideEnterState",
    0x0807077C: "MetaKnightWarpStarRideState2",
    0x08070798: "MetaKnightWarpStarRideState2Update",
    0x0807079C: "MetaKnightWarpStarRideState1",
    0x080708EC: "MetaKnightWarpStarRideState1Update",
    0x08070930: "MetaKnightWarpStarRideState3",
    0x08070A84: "MetaKnightWarpStarRideState3Update",
    0x08070AC8: "MetaKnightWarpStarRideState4",
    0x08070C0C: "MetaKnightWarpStarRideState4Update",
    0x08070C54: "MetaKnightWarpStarRideState5",
    0x08070D48: "MetaKnightWarpStarRideState5Update",
    0x08070D90: "MetaKnightWarpStarRideState6",
    0x08070E7C: "MetaKnightWarpStarRideState6Update",
    0x08071030: "Task_WarpStar",
    0x08071098: "WarpStarUpdate",
    0x080710E0: "WarpStarEnterState",
    0x080710FC: "WarpStarAnimateTiles",
    0x080711D0: "WarpStarBoard",
    0x08071360: "WarpStarCopyTilesToRider",
    0x080713F8: "CreateWarpStar",
    0x08071418: "WarpStarState0",
    0x0807156C: "WarpStarState0Update",
    0x0807160C: "WarpStarState1",
    0x08071640: "WarpStarState1Update",
    0x08071694: "WarpStarVanish",
    0x08071778: "WarpStarStartFlight",
    0x08071830: "WarpStarFlightUpdate",
    0x08071850: "WarpStarFlightEnterState",
    0x0807186C: "WarpStarSetTrail",
    0x08071898: "WarpStarStopTrail",
    0x080718C0: "WarpStarEmitTrailStars",
    0x080719A0: "WarpStarDrawFlight",
    0x08071BB0: "WarpStarSetRiderState",
    0x08071C38: "WarpStarSetMetaKnightRiderState",
    0x08071CC0: "CreateFlyingWarpStar",
    0x08071F54: "WarpStarFlight5",
    0x080720DC: "WarpStarFlight5Update",
    0x080720E8: "WarpStarFlight6",
    0x0807237C: "WarpStarFlight6Update",
    0x08072388: "WarpStarFlight8",
    0x08072678: "WarpStarFlight8Update",
    0x08072684: "WarpStarFlight10",
    0x080728A4: "WarpStarFlight10Update",
    0x080728B0: "WarpStarFlight11",
    0x08072AF4: "WarpStarFlight11Update",
    0x08072B00: "WarpStarFlight12",
    0x08072D80: "WarpStarFlight12Update",
    0x08072D8C: "WarpStarFlight13",
    0x080731C4: "WarpStarFlight13Update",
    0x080731D0: "WarpStarFlight14",
    0x0807328C: "WarpStarFlight14Update",
    0x08073298: "WarpStarFlight15",
    0x08073578: "WarpStarFlight15Update",
    0x08073584: "WarpStarFlight17",
    0x080737F8: "WarpStarFlight17Update",
    0x08073804: "WarpStarFlight18",
    0x0807395C: "WarpStarFlight18Update",
    0x08073968: "WarpStarFlight19",
    0x080739BC: "WarpStarFlight19Update",
    0x08073A54: "WarpStarFlight20",
    0x08073CD4: "WarpStarFlight20Update",
    0x08073CE0: "WarpStarFlight21",
    0x08073E00: "WarpStarFlight21Update",
    0x08073E0C: "WarpStarFlight22",
    0x08073E80: "WarpStarFlight22Update",
    0x08073F18: "WarpStarFlight23",
    0x0807409C: "WarpStarFlight23Update",
    0x080740BC: "WarpStarFlight24",
    0x080743C8: "WarpStarFlight24Update",
    0x0807447C: "WarpStarFlight4Update",
    0x0807450C: "Task_WarpStarCamera",
    0x08074568: "WarpStarCameraUpdate",
    0x08074588: "WarpStarCameraFollowPlayer",
    0x08074638: "WarpStarCameraPath5",
    0x080746C0: "WarpStarCameraPath6",
    0x0807470C: "WarpStarCameraPath8",
    0x08074784: "WarpStarCameraPath10",
    0x08074794: "WarpStarCameraPath11",
    0x080747DC: "WarpStarCameraPath12",
    0x080747EC: "WarpStarCameraPath13",
    0x08074880: "WarpStarCameraPath14",
    0x080748A8: "WarpStarCameraPath15",
    0x08074904: "WarpStarCameraPath17",
    0x08074974: "WarpStarCameraPath18",
    0x08074988: "WarpStarCameraPath20",
    0x08074AB8: "WarpStarCameraPath21",
    0x08074AC8: "WarpStarCameraPath23",
    0x08074B60: "WarpStarCameraPath24",
    0x08074BB0: "CreateWarpStarTrailStar",
    0x08074C0C: "Task_WarpStarTrailStar",
    0x08075000: "Task_NightmarePowerOrbEscape",
    0x08075290: "CreateNightmarePowerOrbEscapeStars",
    0x080752F4: "Task_NightmarePowerOrbEscapeStar",
    0x08076074: "NightmarePowerOrbEscapeStarDraw",
    0x08076318: "PlayerCannonInit",
    0x0807637C: "PlayerCannonUpdate",
    0x080763C4: "PlayerCannonEnterState",
    0x08076C88: "PlayerCannonState0",
    0x08076CAC: "PlayerCannonState0Update",
    0x08076CD4: "PlayerCannonState1",
    0x08076D00: "PlayerCannonState1Update",
    0x08076D0C: "PlayerCannonState2",
    0x08076D38: "PlayerCannonState2Update",
    0x08076D44: "PlayerCannonState3",
    0x08076D70: "PlayerCannonState3Update",
    0x08076DAC: "PlayerCannonState4",
    0x08076DDC: "PlayerCannonState4Update",
    0x08076E30: "PlayerCannonState5",
    0x08076E48: "PlayerCannonState5Update",
    0x08076E88: "PlayerEnterCannon",
    0x08076EC8: "PlayerLeaveCannon",
    0x08076F04: "PlayerEndCannonLaunch",
    0x08076F50: "PlayerJumpOutOfCannon",
    0x08076FE4: "CannonLaunchPlayers",
    0x0807705C: "Task_Cannon",
    0x080770A0: "CannonPickLaunchState",
    0x080770F0: "CannonLoadPlayer",
    0x080771D8: "CannonFire",
    0x08077224: "CannonInit",
    0x08077254: "CannonUpdate",
    0x08077270: "CannonEnterState",
    0x0807728C: "CannonWait",
    0x080772B0: "CannonWaitUpdate",
    0x08077308: "CannonState1",
    0x080773A8: "CannonState1Update",
    0x080773D0: "CannonState2",
    0x08077564: "CannonState2Update",
    0x08077568: "CannonState3",
    0x08077718: "CannonState3Update",
    0x0807771C: "Task_CannonFuse",
    0x0807775C: "CannonFuseInitBurn",
    0x0807777C: "CannonFuseGetPieceFrame",
    0x080777BC: "CannonFuseEnterPiece",
    0x08077830: "CannonFuseBurnStep",
    0x08077898: "CannonFuseStepCell",
    0x080779DC: "CannonFuseMoveSpark",
    0x08077A48: "CreateCannonFuseSpark",
    0x08077A64: "CannonFuseInit",
    0x08077AA8: "CannonFuseUpdate",
    0x08077AC4: "CannonFuseEnterState",
    0x08077AE0: "CannonFuseWait",
    0x08077B24: "CannonFuseWaitUpdate",
    0x08077B60: "CannonFuseBurn",
    0x08077BA8: "CannonFuseBurnUpdate",
    0x08077BF0: "CannonFuseState2",
    0x08077C2C: "CannonFuseState2Update",
    0x08077C64: "Task_BigSwitch",
    0x08077CA4: "BigSwitchHitterCanPress",
    0x08077CF4: "BigSwitchStartPress",
    0x08077D38: "BigSwitchStartRefill",
    0x08077D54: "BigSwitchRefillHealth",
    0x08077DD8: "BigSwitchInit",
    0x08077E08: "BigSwitchUpdate",
    0x08077E30: "BigSwitchEnterState",
    0x08077E4C: "BigSwitchWait",
    0x08077E74: "BigSwitchWaitUpdate",
    0x08077E9C: "BigSwitchState1",
    0x08077ECC: "BigSwitchState1Update",
    0x08077ED0: "BigSwitchRefill",
    0x08077F08: "BigSwitchRefillUpdate",
    0x08077F0C: "Task_Stake",
    0x08077F4C: "StakeInit",
    0x08077F7C: "StakeUpdate",
    0x08077F98: "StakeState0",
    0x08077FF4: "StakeState0Update",
    0x08077FF8: "RoomParticlesDrawFixed",
    0x0807802C: "RoomParticlesDrawBelowLine",
    0x080780C4: "RoomParticlesDrawRepeated",
    0x0807811C: "RoomParticleStepX",
    0x0807817C: "RoomParticleInit",
    0x080781FC: "RoomParticleDrawFixed",
    0x080782B4: "RoomParticleDrawScrolled",
    0x0807831C: "RoomParticleDrawRepeated",
    0x080783E0: "RoomParticleIsOnScreen",
    0x0807840C: "RoomParticleStepY",
    0x08078598: "Task_RoomParticles",
    0x08078670: "RoomParticlesVariant0",
    0x080786E8: "RoomParticlesVariant1",
    0x0807876C: "RoomParticlesVariant2",
    0x080787F0: "RoomParticlesVariant3",
    0x08078874: "RoomParticlesVariant4",
    0x0807893C: "Task_WaddleDee",
    0x08078984: "ParasolWaddleDeeReactToDefeat",
    0x080789AC: "WaddleDeeStartFall",
    0x08078A48: "WaddleDeeLand",
    0x08078B08: "WaddleDeeEnterWater",
    0x08078B38: "WaddleDeeHitWall",
    0x08078B64: "WaddleDeeHitCeiling",
    0x08078BE0: "WaddleDeeWalkInit",
    0x08078C14: "WaddleDeeWalkUpdate",
    0x08078C64: "WaddleDeeWalkEnterState",
    0x08078C80: "WaddleDeeWalk",
    0x08078CB8: "WaddleDeeWalkState0Update",
    0x08078CBC: "WaddleDeeWalkFall",
    0x08078CE4: "WaddleDeeWalkFallUpdate",
    0x08078CE8: "WaddleDeePaceInit",
    0x08078D1C: "WaddleDeePaceUpdate",
    0x08078D6C: "WaddleDeePaceEnterState",
    0x08078D88: "WaddleDeePaceWalk",
    0x08078DD4: "WaddleDeePaceWalkUpdate",
    0x08078E10: "WaddleDeePaceFall",
    0x08078E38: "WaddleDeePaceFallUpdate",
    0x08078E3C: "WaddleDeeJumpInit",
    0x08078E80: "WaddleDeeJumpEnterState",
    0x08078E9C: "WaddleDeeJumpUpdate",
    0x08078EEC: "WaddleDeeJumpWalk",
    0x08078F24: "WaddleDeeJumpWalkUpdate",
    0x08078F8C: "WaddleDeeJump",
    0x0807906C: "WaddleDeeJumpState1Update",
    0x080790C0: "WaddleDeeJumpFall",
    0x080790E8: "WaddleDeeJumpFallUpdate",
    0x080790EC: "ParasolWaddleDeeWalkInit",
    0x08079128: "ParasolWaddleDeeWalkUpdate",
    0x08079178: "ParasolWaddleDeeWalkEnterState",
    0x08079194: "ParasolWaddleDeeWalk",
    0x080791BC: "ParasolWaddleDeeWalkState0Update",
    0x080791C0: "ParasolWaddleDeeWalkState1",
    0x0807921C: "ParasolWaddleDeeWalkState1Update",
    0x08079238: "WaddleDeeIdleInit",
    0x0807927C: "WaddleDeeIdleEnterState",
    0x08079298: "WaddleDeeIdleUpdate",
    0x080792DC: "WaddleDeeIdle",
    0x080792F4: "WaddleDeeIdleState0Update",
    0x080792F8: "ParasolWaddleDeeStandInit",
    0x0807933C: "ParasolWaddleDeeStandUpdate",
    0x0807938C: "ParasolWaddleDeeStandEnterState",
    0x080793A8: "ParasolWaddleDeeStandState0",
    0x080793C4: "ParasolWaddleDeeStandState0Update",
    0x080793D0: "ParasolWaddleDeeStandState1",
    0x08079424: "ParasolWaddleDeeStandState1Update",
    0x08079440: "Task_Pengy",
    0x08079480: "PengyStartFall",
    0x080794A0: "PengyLand",
    0x080794C0: "PengyEnterWater",
    0x080794DC: "PengyInit",
    0x0807950C: "PengyUpdate",
    0x0807955C: "PengyEnterState",
    0x08079578: "PengyPickStartState",
    0x080795B4: "PengyWait",
    0x080795D8: "PengyWaitUpdate",
    0x0807964C: "PengyStartShoot",
    0x0807966C: "PengyStartWalk",
    0x0807968C: "PengyCheckWalk",
    0x080796D8: "PengyCheckShoot",
    0x08079710: "PengyWalk",
    0x080797B4: "PengyWalkUpdate",
    0x080797EC: "PengyShoot",
    0x080798B8: "PengyShootUpdate",
    0x080798E8: "PengyFall",
    0x08079914: "PengyFallUpdate",
    0x08079918: "PengyIdleInit",
    0x0807995C: "PengyIdleUpdate",
    0x080799A0: "PengyIdleEnterState",
    0x080799BC: "PengyIdle",
    0x080799DC: "PengyIdleState0Update",
    0x080799E0: "Task_Bomber",
    0x08079A20: "BomberStartFall",
    0x08079A40: "BomberLand",
    0x08079A60: "BomberEnterWater",
    0x08079A90: "BomberBounceOffWall",
    0x08079A9C: "BomberInit",
    0x08079AD4: "BomberUpdate",
    0x08079B24: "BomberEnterState",
    0x08079B40: "BomberWalk",
    0x08079B98: "BomberWalkUpdate",
    0x08079BAC: "BomberState1",
    0x08079BE0: "BomberState1Update",
    0x08079BE4: "BomberState2",
    0x08079C84: "BomberState2Update",
    0x08079CAC: "BomberExplode",
    0x08079D2C: "BomberExplodeUpdate",
    0x08079D30: "BomberIdleInit",
    0x08079D74: "BomberIdleUpdate",
    0x08079DB8: "BomberIdleEnterState",
    0x08079DD4: "BomberIdle",
    0x08079E20: "BomberIdleState0Update",
    0x08079E24: "Task_Sparky",
    0x08079E70: "SparkyTeardown",
    0x08079E8C: "SparkySetJumpMotion",
    0x08079F54: "SparkyPickNextState",
    0x08079FA8: "SparkyPickStartState",
    0x08079FD0: "SparkyStartFall",
    0x0807A008: "SparkyLand",
    0x0807A040: "SparkyEnterWater",
    0x0807A050: "SparkyBounceOffWall",
    0x0807A05C: "SparkyHitCeiling",
    0x0807A06C: "SparkyJumpInit",
    0x0807A09C: "SparkyJumpUpdate",
    0x0807A10C: "SparkyJumpEnterState",
    0x0807A128: "SparkyJump",
    0x0807A1BC: "SparkyJumpState2Update",
    0x0807A1C0: "SparkyJumpDischarge",
    0x0807A380: "SparkyJumpDischargeUpdate",
    0x0807A3BC: "SparkyJumpLand",
    0x0807A408: "SparkyJumpLandUpdate",
    0x0807A424: "SparkyJumpState3",
    0x0807A458: "SparkyJumpState3Update",
    0x0807A45C: "SparkyIdleInit",
    0x0807A4A0: "SparkyIdleUpdate",
    0x0807A4E4: "SparkyIdleEnterState",
    0x0807A500: "SparkyIdle",
    0x0807A574: "SparkyIdleState0Update",
    0x0807A578: "SparkyStandInit",
    0x0807A5A8: "SparkyStandUpdate",
    0x0807A618: "SparkyStandEnterState",
    0x0807A634: "SparkyStandDischarge",
    0x0807A7F4: "SparkyStandDischargeUpdate",
    0x0807A830: "SparkyStandWait",
    0x0807A87C: "SparkyStandWaitUpdate",
    0x0807A898: "Task_Scarfy",
    0x0807A8D4: "ScarfyPickStartState",
    0x0807AA0C: "ScarfyCheckTransform",
    0x0807AAB8: "ScarfyInit",
    0x0807AAE4: "ScarfyUpdate",
    0x0807AB38: "ScarfyEnterState",
    0x0807AB54: "ScarfyHover",
    0x0807AB70: "ScarfyHoverUpdate",
    0x0807AB8C: "ScarfyHide",
    0x0807ABB4: "ScarfyHideUpdate",
    0x0807ABDC: "ScarfyState1",
    0x0807AC08: "ScarfyState1Update",
    0x0807AC58: "ScarfyTransform",
    0x0807ADCC: "ScarfyTransformUpdate",
    0x0807AE1C: "ScarfyChase",
    0x0807AE7C: "ScarfyChaseUpdate",
    0x0807AECC: "ScarfyExplode",
    0x0807AF3C: "ScarfyExplodeUpdate",
    0x0807AF58: "Task_SwordKnight",
    0x0807AF98: "Task_BladeKnight",
    0x0807AFD8: "SwordAndBladeKnightStartFall",
    0x0807B010: "SwordAndBladeKnightLand",
    0x0807B048: "SwordAndBladeKnightEnterWater",
    0x0807B070: "SwordAndBladeKnightHitWall",
    0x0807B088: "SwordAndBladeKnightStopAtSlope",
    0x0807B0B4: "SwordAndBladeKnightStepLunge",
    0x0807B0F0: "SwordAndBladeKnightStartLunge",
    0x0807B16C: "SwordAndBladeKnightEndSlashUp",
    0x0807B1A8: "SwordAndBladeKnightEndSlashDown",
    0x0807B1E4: "CreateSwordAndBladeKnightSlash",
    0x0807B294: "SwordAndBladeKnightPickSlash",
    0x0807B300: "SwordAndBladeKnightWalkInit",
    0x0807B32C: "SwordAndBladeKnightWalkUpdate",
    0x0807B3DC: "SwordAndBladeKnightWalkEnterState",
    0x0807B3F8: "SwordAndBladeKnightWalk",
    0x0807B430: "SwordAndBladeKnightWalkState0Update",
    0x0807B49C: "SwordAndBladeKnightWalkState1",
    0x0807B4C8: "SwordAndBladeKnightWalkState1Update",
    0x0807B4F0: "SwordAndBladeKnightWalkSlashUp",
    0x0807B558: "SwordAndBladeKnightWalkSlashUpUpdate",
    0x0807B584: "SwordAndBladeKnightWalkState2",
    0x0807B5B0: "SwordAndBladeKnightWalkState2Update",
    0x0807B5D8: "SwordAndBladeKnightWalkSlashDown",
    0x0807B640: "SwordAndBladeKnightWalkSlashDownUpdate",
    0x0807B66C: "SwordAndBladeKnightWalkWalkBack",
    0x0807B6B4: "SwordAndBladeKnightWalkWalkBackUpdate",
    0x0807B6E8: "SwordAndBladeKnightWalkState6",
    0x0807B7A8: "SwordAndBladeKnightWalkState6Update",
    0x0807B7D0: "SwordAndBladeKnightWalkFall",
    0x0807B7FC: "SwordAndBladeKnightWalkFallUpdate",
    0x0807B800: "SwordAndBladeKnightIdleInit",
    0x0807B844: "SwordAndBladeKnightIdleUpdate",
    0x0807B888: "SwordAndBladeKnightIdleEnterState",
    0x0807B8A4: "SwordAndBladeKnightIdle",
    0x0807B8D0: "SwordAndBladeKnightIdleState0Update",
    0x0807B8EC: "SwordAndBladeKnightStandInit",
    0x0807B918: "SwordAndBladeKnightStandUpdate",
    0x0807B9C8: "SwordAndBladeKnightStandEnterState",
    0x0807B9EC: "SwordAndBladeKnightStandState0",
    0x0807BA18: "SwordAndBladeKnightStandState0Update",
    0x0807BA7C: "SwordAndBladeKnightStandState1",
    0x0807BAA8: "SwordAndBladeKnightStandState1Update",
    0x0807BAD0: "SwordAndBladeKnightStandSlashUp",
    0x0807BB38: "SwordAndBladeKnightStandSlashUpUpdate",
    0x0807BB60: "SwordAndBladeKnightStandState2",
    0x0807BB8C: "SwordAndBladeKnightStandState2Update",
    0x0807BBB4: "SwordAndBladeKnightStandSlashDown",
    0x0807BC1C: "SwordAndBladeKnightStandSlashDownUpdate",
    0x0807BC44: "SwordAndBladeKnightStandState5",
    0x0807BC78: "SwordAndBladeKnightStandState5Update",
    0x0807BCAC: "Task_BlockStar",
    0x0807BCEC: "BlockStarInit",
    0x0807BD14: "BlockStarUpdate",
    0x0807BD20: "Task_Needlous",
    0x0807BD7C: "NeedlousPickStartState",
    0x0807BDB8: "NeedlousStartFall",
    0x0807BE08: "NeedlousLand",
    0x0807BE9C: "NeedlousEnterWater",
    0x0807BEAC: "NeedlousHitWall",
    0x0807BEFC: "NeedlousHitCeiling",
    0x0807C080: "NeedlousInit",
    0x0807C0AC: "NeedlousUpdate",
    0x0807C0FC: "NeedlousEnterState",
    0x0807C118: "NeedlousWalk",
    0x0807C138: "NeedlousWalkUpdate",
    0x0807C1D0: "NeedlousFall",
    0x0807C1F4: "NeedlousFallUpdate",
    0x0807C210: "NeedlousState2",
    0x0807C22C: "NeedlousState2Update",
    0x0807C248: "NeedlousState3",
    0x0807C264: "NeedlousState3Update",
    0x0807C280: "NeedlousState4",
    0x0807C29C: "NeedlousState4Update",
    0x0807C2B8: "NeedlousDash",
    0x0807C2E0: "NeedlousDashUpdate",
    0x0807C350: "NeedlousIdleInit",
    0x0807C394: "NeedlousIdleUpdate",
    0x0807C3D8: "NeedlousIdleEnterState",
    0x0807C3F4: "NeedlousIdle",
    0x0807C440: "NeedlousIdleState0Update",
    0x0807C444: "Task_UFO",
    0x0807C484: "UFOTeardown",
    0x0807C4A0: "UFOStartPaletteAnim",
    0x0807C508: "UFOPickNextPoint",
    0x0807C530: "UFOSetTarget",
    0x0807C5AC: "UFOIsAtTarget",
    0x0807C5E4: "UFOSetFlightVelocity",
    0x0807C618: "CreateUFOLaser",
    0x0807C684: "UFOInit",
    0x0807C6B0: "UFOUpdate",
    0x0807C6F4: "UFOEnterState",
    0x0807C710: "UFOZigzag",
    0x0807C7D8: "UFOZigzagUpdate",
    0x0807C80C: "UFOPickMove",
    0x0807C828: "UFOPickMoveUpdate",
    0x0807C8B0: "UFOFlyToTarget",
    0x0807C8D0: "UFOFlyToTargetUpdate",
    0x0807C9B4: "UFOShoot",
    0x0807CA18: "UFOShootUpdate",
    0x0807CA50: "UFOIdleInit",
    0x0807CA98: "UFOIdleUpdate",
    0x0807CADC: "UFOIdleEnterState",
    0x0807CAF8: "UFOIdle",
    0x0807CBB0: "UFOIdleState0Update",
    0x0807CBB4: "Task_Parasol",
    0x0807CC68: "ParasolRiseInit",
    0x0807CC9C: "ParasolRiseUpdate",
    0x0807CCEC: "ParasolRiseEnterState",
    0x0807CD08: "ParasolRise",
    0x0807CD60: "ParasolRiseState0Update",
    0x0807CD64: "ParasolChaseInit",
    0x0807CD9C: "ParasolChaseUpdate",
    0x0807CDEC: "ParasolChaseEnterState",
    0x0807CE08: "ParasolChase",
    0x0807CE68: "ParasolChaseState0Update",
    0x0807CEDC: "ParasolIdleInit",
    0x0807CF20: "ParasolIdleUpdate",
    0x0807CF64: "ParasolIdleEnterState",
    0x0807CF80: "ParasolIdle",
    0x0807CFCC: "ParasolIdleState0Update",
    0x0807CFD0: "ParasolVariant3",
    0x0807D008: "Task_PengyIceBreath",
    0x0807D060: "PengyIceBreathInit",
    0x0807D094: "PengyIceBreathUpdate",
    0x0807D0D8: "PengyIceBreathEnterState",
    0x0807D0F4: "PengyIceBreathState0",
    0x0807D178: "PengyIceBreathState0Update",
    0x0807D17C: "Task_UFOLaser",
    0x0807D1BC: "UFOLaserReactToDamage",
    0x0807D1DC: "UFOLaserReactToDefeat",
    0x0807D1FC: "UFOLaserInit",
    0x0807D230: "UFOLaserUpdate",
    0x0807D29C: "UFOLaserEnterState",
    0x0807D2B8: "UFOLaserState0",
    0x0807D2EC: "UFOLaserState0Update",
    0x0807D320: "UFOLaserVanish",
    0x0807D388: "Task_SwordAndBladeKnightSlash",
    0x0807D3B0: "SwordAndBladeKnightSlashUpdate",
    0x0807D490: "Task_PengyIceBreathPuff",
    0x0807D510: "Task_PengyIceBreathSparkle",
    0x0807D684: "Task_Rocky",
    0x0807D6C4: "RockyWalkInit",
    0x0807D6FC: "RockyWalkEnterState",
    0x0807D718: "RockyWalkUpdate",
    0x0807D748: "RockyWalk",
    0x0807D82C: "RockyWalkState0Update",
    0x0807D8F8: "RockyWalkState1",
    0x0807D918: "RockyWalkState1Update",
    0x0807D9AC: "RockyWalkState2",
    0x0807DA08: "RockyWalkState2Update",
    0x0807DA30: "RockyWalkState3",
    0x0807DA5C: "RockyWalkState3Update",
    0x0807DA84: "RockyWalkState4",
    0x0807DAA8: "RockyWalkState4Update",
    0x0807DAAC: "RockyIdleInit",
    0x0807DAF0: "RockyIdleEnterState",
    0x0807DB0C: "RockyIdleUpdate",
    0x0807DB30: "RockyIdle",
    0x0807DB44: "RockyIdleState0Update",
    0x0807DB48: "RockyStandInit",
    0x0807DB80: "RockyStandEnterState",
    0x0807DB9C: "RockyStandUpdate",
    0x0807DBCC: "RockyStandState0",
    0x0807DC78: "RockyStandState0Update",
    0x0807DCA0: "RockyStandState1",
    0x0807DCC0: "RockyStandState1Update",
    0x0807DCE8: "RockyStandState2",
    0x0807DD0C: "RockyStandState2Update",
    0x0807DD70: "RockyLand",
    0x0807DDDC: "RockyStartFall",
    0x0807DE30: "RockyHitCeiling",
    0x0807DE64: "RockyHitWall",
    0x0807DE88: "RockyEnterWater",
    0x0807DE98: "Task_SirKibble",
    0x0807DEE4: "SirKibbleStandInit",
    0x0807DF14: "SirKibbleStandEnterState",
    0x0807DF30: "SirKibbleStandUpdate",
    0x0807DF60: "SirKibbleWait",
    0x0807DF8C: "SirKibbleWaitUpdate",
    0x0807E014: "SirKibbleWalkInit",
    0x0807E044: "SirKibbleWalkEnterState",
    0x0807E060: "SirKibbleWalkUpdate",
    0x0807E090: "SirKibbleWalk",
    0x0807E100: "SirKibbleWalkState0Update",
    0x0807E188: "SirKibbleShoot",
    0x0807E244: "SirKibbleShootUpdate",
    0x0807E290: "SirKibbleJump",
    0x0807E3B0: "SirKibbleJumpUpdate",
    0x0807E3E4: "SirKibbleIdleInit",
    0x0807E428: "SirKibbleIdleEnterState",
    0x0807E444: "SirKibbleIdleUpdate",
    0x0807E468: "SirKibbleIdle",
    0x0807E480: "SirKibbleIdleState0Update",
    0x0807E4FC: "SirKibbleStartFall",
    0x0807E514: "SirKibbleLand",
    0x0807E520: "SirKibbleEnterWater",
    0x0807E530: "Task_Cappy",
    0x0807E568: "CappyCappedInit",
    0x0807E5A0: "CappyCappedEnterState",
    0x0807E5BC: "CappyCappedUpdate",
    0x0807E640: "CappyCappedHop",
    0x0807E6D0: "CappyCappedHopUpdate",
    0x0807E6D4: "CappyCaplessInit",
    0x0807E714: "CappyCaplessEnterState",
    0x0807E730: "CappyCaplessUpdate",
    0x0807E768: "CappyCaplessHop",
    0x0807E810: "CappyCaplessHopUpdate",
    0x0807E814: "CappyCaplessJump",
    0x0807E884: "CappyCaplessJumpUpdate",
    0x0807E8B8: "CappyStandInit",
    0x0807E904: "CappyStandEnterState",
    0x0807E920: "CappyStandUpdate",
    0x0807E950: "CappyStandHop",
    0x0807E9B0: "CappyStandHopUpdate",
    0x0807E9B4: "CappyBounceOffWall",
    0x0807E9C8: "CappyEnterWater",
    0x0807E9D8: "Task_Gordo",
    0x0807EA30: "GordoBobInit",
    0x0807EA60: "GordoBobUpdate",
    0x0807EA84: "GordoBob",
    0x0807EAD4: "GordoBobState0Update",
    0x0807EAF0: "GordoBounceVerticalInit",
    0x0807EB20: "GordoBounceVerticalUpdate",
    0x0807EB60: "GordoBounceVertical",
    0x0807EBC4: "GordoBounceVerticalState0Update",
    0x0807EBE0: "GordoBounceHorizontalInit",
    0x0807EC10: "GordoBounceHorizontalUpdate",
    0x0807EC4C: "GordoBounceHorizontal",
    0x0807ECB0: "GordoBounceHorizontalState0Update",
    0x0807ECCC: "GordoSweepInit",
    0x0807ECFC: "GordoSweepUpdate",
    0x0807ED20: "GordoSweep",
    0x0807ED98: "GordoSweepState0Update",
    0x0807EDB4: "Task_CoolSpook",
    0x0807EE14: "CoolSpookFlyInit",
    0x0807EE44: "CoolSpookFlyEnterState",
    0x0807EE60: "CoolSpookFlyUpdate",
    0x0807EE84: "CoolSpookFly",
    0x0807EEA8: "CoolSpookFlyState0Update",
    0x0807EEC4: "CoolSpookBobInit",
    0x0807EF08: "CoolSpookBobEnterState",
    0x0807EF24: "CoolSpookBobUpdate",
    0x0807EF48: "CoolSpookBob",
    0x0807EF60: "CoolSpookBobState0Update",
    0x0807EFEC: "CoolSpookTeardown",
    0x0807EFFC: "Task_Kabu",
    0x0807F044: "KabuJumpInit",
    0x0807F078: "KabuJumpEnterState",
    0x0807F094: "KabuJumpUpdate",
    0x0807F0C4: "KabuJumpSpin",
    0x0807F1F0: "KabuJumpSpinUpdate",
    0x0807F218: "KabuJump",
    0x0807F2B4: "KabuJumpState1Update",
    0x0807F2EC: "KabuJumpFall",
    0x0807F348: "KabuJumpFallUpdate",
    0x0807F380: "KabuTeleportInit",
    0x0807F3B8: "KabuTeleportEnterState",
    0x0807F3D4: "KabuTeleportUpdate",
    0x0807F42C: "KabuTeleportSpin",
    0x0807F488: "KabuTeleportSpinUpdate",
    0x0807F4DC: "KabuTeleport",
    0x0807F634: "KabuTeleportState1Update",
    0x0807F78C: "KabuTeleportState2",
    0x0807F888: "KabuTeleportState2Update",
    0x0807F88C: "KabuSlideInit",
    0x0807F8BC: "KabuSlideEnterState",
    0x0807F8D8: "KabuSlideUpdate",
    0x0807F920: "KabuSlide",
    0x0807F9A0: "KabuSlideState0Update",
    0x0807FA98: "KabuSlideState1",
    0x0807FAFC: "KabuSlideState1Update",
    0x0807FB00: "KabuIdleInit",
    0x0807FB44: "KabuIdleEnterState",
    0x0807FB60: "KabuIdleUpdate",
    0x0807FB84: "KabuIdle",
    0x0807FBCC: "KabuIdleState0Update",
    0x0807FBD0: "KabuHitWall",
    0x0807FC20: "KabuStartFall",
    0x0807FC70: "KabuLand",
    0x0807FC94: "KabuHitCeiling",
    0x0807FCAC: "KabuEnterWater",
    0x0807FCBC: "Task_Twister",
    0x0807FD34: "TwisterInit",
    0x0807FD64: "TwisterEnterState",
    0x0807FD80: "TwisterUpdate",
    0x0807FDC8: "TwisterState0",
    0x0807FE18: "TwisterState0Update",
    0x0807FFA0: "TwisterState1",
    0x0808003C: "TwisterState1Update",
    0x080801CC: "TwisterState2",
    0x08080278: "TwisterState2Update",
    0x080802BC: "TwisterIdleInit",
    0x08080300: "TwisterIdleEnterState",
    0x0808031C: "TwisterIdleUpdate",
    0x08080340: "TwisterIdle",
    0x08080358: "TwisterIdleState0Update",
    0x08080374: "TwisterTickSound",
    0x08080398: "TwisterLand",
    0x080803A4: "TwisterStartFall",
    0x080803BC: "TwisterHitCeiling",
    0x080803CC: "TwisterEnterWater",
    0x08080400: "Task_HotHead",
    0x0808044C: "HotHeadWalkInit",
    0x080804A4: "HotHeadWalkEnterState",
    0x080804C0: "HotHeadWalkUpdate",
    0x080804F0: "HotHeadWalk",
    0x0808051C: "HotHeadWalkState0Update",
    0x08080570: "HotHeadWalkShoot",
    0x080806E8: "HotHeadWalkShootUpdate",
    0x08080740: "HotHeadWalkFall",
    0x08080768: "HotHeadWalkFallUpdate",
    0x0808076C: "HotHeadIdleInit",
    0x080807BC: "HotHeadIdleEnterState",
    0x080807D8: "HotHeadIdleUpdate",
    0x080807FC: "HotHeadIdle",
    0x08080814: "HotHeadIdleState0Update",
    0x08080818: "HotHeadStandInit",
    0x08080870: "HotHeadStandEnterState",
    0x0808088C: "HotHeadStandUpdate",
    0x080808BC: "HotHeadStandWait",
    0x080808DC: "HotHeadStandWaitUpdate",
    0x08080930: "HotHeadStandShoot",
    0x08080AA8: "HotHeadStandShootUpdate",
    0x08080B00: "HotHeadStandFall",
    0x08080B28: "HotHeadStandFallUpdate",
    0x08080B70: "HotHeadStartFall",
    0x08080BCC: "HotHeadLand",
    0x08080C2C: "HotHeadBounceOffWall",
    0x08080C38: "HotHeadEnterWater",
    0x08080C48: "Task_HotHeadFlame",
    0x08080D58: "HotHeadFlameCheckParent",
    0x08080DD0: "Task_Starman",
    0x08080E10: "StarmanAmbushInit",
    0x08080E40: "StarmanAmbushEnterState",
    0x08080E5C: "StarmanAmbushUpdate",
    0x08080EA4: "StarmanAmbushWalk",
    0x08080EDC: "StarmanAmbushWalkUpdate",
    0x08081084: "StarmanAmbushHide",
    0x080810C4: "StarmanAmbushHideUpdate",
    0x08081140: "StarmanAmbushDive",
    0x0808124C: "StarmanAmbushDiveUpdate",
    0x08081274: "StarmanAmbushFlyAway",
    0x080812EC: "StarmanAmbushFlyAwayUpdate",
    0x08081408: "StarmanJumpInit",
    0x08081440: "StarmanJumpEnterState",
    0x0808145C: "StarmanJumpUpdate",
    0x0808148C: "StarmanJumpWalk",
    0x080814B4: "StarmanJumpWalkUpdate",
    0x08081508: "StarmanJump",
    0x08081560: "StarmanJumpState1Update",
    0x0808159C: "StarmanJumpFall",
    0x080815DC: "StarmanJumpFallUpdate",
    0x08081614: "StarmanFlyInit",
    0x0808164C: "StarmanFlyEnterState",
    0x08081668: "StarmanFlyUpdate",
    0x0808168C: "StarmanFly",
    0x080816E8: "StarmanFlyState0Update",
    0x08081774: "StarmanIdleInit",
    0x080817B8: "StarmanIdleEnterState",
    0x080817D4: "StarmanIdleUpdate",
    0x080817F8: "StarmanIdle",
    0x08081810: "StarmanIdleState0Update",
    0x080818A8: "StarmanHitCeiling",
    0x08081900: "StarmanStartFall",
    0x08081960: "StarmanLand",
    0x08081984: "StarmanHitWall",
    0x080819A4: "StarmanEnterWater",
    0x080819B4: "Task_PoppyBrosJr",
    0x080819F4: "PoppyBrosJrInit",
    0x08081A58: "PoppyBrosJrEnterState",
    0x08081A74: "PoppyBrosJrUpdate",
    0x08081AAC: "PoppyBrosJrHop",
    0x08081B5C: "PoppyBrosJrHopUpdate",
    0x08081BC4: "PoppyBrosJrWalk",
    0x08081C3C: "PoppyBrosJrWalkUpdate",
    0x08081C78: "PoppyBrosJrJump",
    0x08081CE0: "PoppyBrosJrJumpUpdate",
    0x08081D24: "PoppyBrosJrStandInit",
    0x08081D68: "PoppyBrosJrStandEnterState",
    0x08081D84: "PoppyBrosJrStandUpdate",
    0x08081DB4: "PoppyBrosJrStandHop",
    0x08081E40: "PoppyBrosJrStandHopUpdate",
    0x08081F08: "PoppyBrosJrHitCeiling",
    0x08081F18: "PoppyBrosJrStartFall",
    0x08081F38: "PoppyBrosJrHitWall",
    0x08081F50: "PoppyBrosJrEnterWater",
    0x08081F60: "Task_PoppyBrosJrOnApple",
    0x08082008: "Task_PoppyBrosJrOnMaximTomato",
    0x080820B8: "PoppyBrosJrRideInit",
    0x080820EC: "PoppyBrosJrRideEnterState",
    0x08082108: "PoppyBrosJrRideUpdate",
    0x08082270: "PoppyBrosJrRide",
    0x080822A4: "PoppyBrosJrRideState0Update",
    0x080822B0: "PoppyBrosJrDroppedObjectInit",
    0x080822E4: "PoppyBrosJrDroppedObjectEnterState",
    0x08082300: "PoppyBrosJrDroppedObjectUpdate",
    0x08082338: "PoppyBrosJrDroppedObjectState0",
    0x08082458: "PoppyBrosJrDroppedObjectState0Update",
    0x0808248C: "PoppyBrosJrRideIdleInit",
    0x080824D0: "PoppyBrosJrRideIdleEnterState",
    0x080824EC: "PoppyBrosJrRideIdleUpdate",
    0x0808253C: "PoppyBrosJrRideIdle",
    0x08082554: "PoppyBrosJrRiderHop",
    0x080825EC: "PoppyBrosJrRiderHopUpdate",
    0x08082678: "PoppyBrosJrRiderStartFall",
    0x080826A0: "PoppyBrosJrRiderLand",
    0x080826BC: "PoppyBrosJrRiderBounceOffWall",
    0x080826C8: "PoppyBrosJrRiderEnterWater",
    0x080826D8: "Task_Wheelie",
    0x08082718: "WheelieInit",
    0x08082750: "WheelieEnterState",
    0x0808276C: "WheelieUpdate",
    0x0808279C: "WheelieState0",
    0x08082818: "WheelieState0Update",
    0x08082844: "WheelieState1",
    0x0808287C: "WheelieState1Update",
    0x080828A8: "WheelieSkid",
    0x08082908: "WheelieSkidUpdate",
    0x08082980: "WheelieWait",
    0x080829D4: "WheelieWaitUpdate",
    0x08082A08: "WheelieBounceOffWall",
    0x08082AEC: "WheelieBounceOffWallUpdate",
    0x08082B14: "WheelieFall",
    0x08082B48: "WheelieFallUpdate",
    0x08082BB8: "WheelieIdleInit",
    0x08082BFC: "WheelieIdleEnterState",
    0x08082C18: "WheelieIdleUpdate",
    0x08082C3C: "WheelieIdle",
    0x08082C58: "WheelieIdleState0Update",
    0x08082C5C: "WheelieCheckSkid",
    0x08082D14: "WheelieStartFall",
    0x08082D4C: "WheelieHitWall",
    0x08082DD4: "WheelieEnterWater",
    0x08082DE4: "Task_Flamer",
    0x08082E68: "FlamerInit",
    0x08082E98: "FlamerEnterState",
    0x08082EB4: "FlamerUpdate",
    0x08082F04: "FlamerState0",
    0x08082FB4: "FlamerState0Update",
    0x08082FDC: "FlamerCrawl",
    0x08083020: "FlamerCrawlUpdate",
    0x080832D0: "FlamerFall",
    0x0808330C: "FlamerFallUpdate",
    0x08083370: "FlamerState3",
    0x08083400: "FlamerState3Update",
    0x08083428: "FlamerState4",
    0x08083488: "FlamerState4Update",
    0x08083614: "FlamerState5",
    0x0808379C: "FlamerState5Update",
    0x080837D0: "FlamerState6",
    0x080838BC: "FlamerState6Update",
    0x0808398C: "FlamerIdleInit",
    0x080839D0: "FlamerIdleEnterState",
    0x080839EC: "FlamerIdleUpdate",
    0x08083A10: "FlamerIdle",
    0x08083A44: "FlamerIdleState0Update",
    0x08083A48: "FlamerGetSurfaceSlopeInDir",
    0x08083AD4: "FlamerGetSurfaceSlopeOnSide",
    0x08083CB8: "FlamerGetSurfaceSlopeAt",
    0x08083D28: "FlamerSetCrawlVelocity",
    0x08083E5C: "FlamerEnterWater",
    0x08083E6C: "Task_SirKibbleCutter",
    0x08083EB4: "SirKibbleCutterInit",
    0x08083EE8: "SirKibbleCutterEnterState",
    0x08083F04: "SirKibbleCutterUpdate",
    0x08083F48: "SirKibbleCutterState0",
    0x08083FBC: "SirKibbleCutterState0Update",
    0x08084050: "Task_HotHeadFire",
    0x080840A4: "HotHeadFireBreathInit",
    0x080840D4: "HotHeadFireBreathEnterState",
    0x080840F0: "HotHeadFireBreathUpdate",
    0x08084114: "HotHeadFireBreath",
    0x0808424C: "HotHeadFireBallInit",
    0x0808429C: "HotHeadFireBallEnterState",
    0x080842B8: "HotHeadFireBallUpdate",
    0x08084308: "HotHeadFireBall",
    0x080843FC: "Task_FlamerFlame",
    0x08084484: "Task_Noddy",
    0x080844C4: "NoddyInit",
    0x080844F8: "NoddyEnterState",
    0x0808451C: "NoddyUpdate",
    0x080845B4: "NoddyWalk",
    0x080846C4: "NoddyWalkUpdate",
    0x080846F4: "NoddyState5",
    0x080847F8: "NoddyState5Update",
    0x080847FC: "NoddyFallAsleep",
    0x08084854: "NoddyFallAsleepUpdate",
    0x0808487C: "NoddySleep",
    0x080848E4: "NoddySleepUpdate",
    0x08084960: "NoddyWakeUp",
    0x080849B4: "NoddyWakeUpUpdate",
    0x080849DC: "NoddySleepFall",
    0x08084A50: "NoddySleepFallUpdate",
    0x08084A74: "NoddyAsleep",
    0x08084AE8: "NoddyAsleepUpdate",
    0x08084AFC: "Task_NoddyBubble",
    0x08084B7C: "NoddyBubbleUpdate",
    0x08084BC0: "NoddyLand",
    0x08084C0C: "NoddyStartFall",
    0x08084C5C: "NoddyEnterWater",
    0x08084C84: "NoddyTurnAtSlope",
    0x08084CB8: "NoddyBounceOffWall",
    0x08084CD4: "Task_Chilly",
    0x08084D14: "ChillyInit",
    0x08084D6C: "ChillyEnterState",
    0x08084D90: "ChillyUpdate",
    0x08084DC0: "ChillyState0",
    0x08084E74: "ChillyState0Update",
    0x08084E9C: "ChillyWait",
    0x08084F50: "ChillyWaitUpdate",
    0x08084F78: "ChillySlide",
    0x08085158: "ChillySlideUpdate",
    0x08085180: "ChillyState3",
    0x08085274: "ChillyState3Update",
    0x0808529C: "ChillyFall",
    0x080852C8: "ChillyFallUpdate",
    0x080852CC: "ChillyIdle",
    0x0808537C: "ChillyIdleUpdate",
    0x08085390: "ChillyLand",
    0x080853C8: "ChillyStartFall",
    0x08085404: "ChillyEnterWater",
    0x0808542C: "ChillyHitWall",
    0x08085450: "Task_WaddleDoo",
    0x08085498: "WaddleDooWalkInit",
    0x080854D0: "WaddleDooWalkEnterState",
    0x08085500: "WaddleDooWalkUpdate",
    0x08085530: "WaddleDooWalk",
    0x08085608: "WaddleDooWalkState0Update",
    0x08085660: "WaddleDooWalkJump",
    0x080856DC: "WaddleDooWalkJumpUpdate",
    0x080856E0: "WaddleDooWalkShoot",
    0x0808582C: "WaddleDooWalkShootUpdate",
    0x08085858: "ParasolWaddleDooInit",
    0x0808589C: "ParasolWaddleDooEnterState",
    0x080858CC: "ParasolWaddleDooUpdate",
    0x080858FC: "ParasolWaddleDooWalk",
    0x08085998: "ParasolWaddleDooWalkUpdate",
    0x08085A04: "ParasolWaddleDooJump",
    0x08085A80: "ParasolWaddleDooJumpUpdate",
    0x08085A84: "ParasolWaddleDooShoot",
    0x08085BB8: "ParasolWaddleDooShootUpdate",
    0x08085BE4: "ParasolWaddleDooDrift",
    0x08085C10: "ParasolWaddleDooDriftUpdate",
    0x08085C2C: "WaddleDooIdle",
    0x08085CC4: "WaddleDooIdleUpdate",
    0x08085CD8: "WaddleDooShoot",
    0x08085E60: "WaddleDooShootUpdate",
    0x08085E74: "WaddleDooLand",
    0x08085EF0: "WaddleDooStartFall",
    0x08085FA0: "WaddleDooEnterWater",
    0x08085FEC: "WaddleDooHitCeiling",
    0x08086024: "WaddleDooHitWall",
    0x0808606C: "ParasolWaddleDooReactToDefeat",
    0x08086090: "Task_BrontoBurt",
    0x080860D8: "BrontoBurtEnterVariant",
    0x080860F8: "BrontoBurtWaveInit",
    0x08086128: "BrontoBurtWaveEnterState",
    0x0808614C: "BrontoBurtWaveUpdate",
    0x08086170: "BrontoBurtWave",
    0x08086274: "BrontoBurtWaveState0Update",
    0x080862CC: "BrontoBurtWeaveInit",
    0x080862FC: "BrontoBurtWeaveEnterState",
    0x08086320: "BrontoBurtWeaveUpdate",
    0x08086344: "BrontoBurtWeave",
    0x080864EC: "BrontoBurtSwoopInit",
    0x0808659C: "BrontoBurtSwoopUpdate",
    0x080865C0: "BrontoBurtSwoop",
    0x080867B8: "BrontoBurtSwoopState0Update",
    0x08086824: "BrontoBurtDiagonalInit",
    0x080868B8: "BrontoBurtDiagonalEnterState",
    0x080868DC: "BrontoBurtDiagonalUpdate",
    0x08086984: "BrontoBurtDiagonal",
    0x080869B8: "BrontoBurtDiagonalState0Update",
    0x080869F0: "BrontoBurtChaseInit",
    0x08086A20: "BrontoBurtChaseEnterState",
    0x08086A44: "BrontoBurtChaseUpdate",
    0x08086A68: "BrontoBurtChase",
    0x08086B68: "BrontoBurtChaseState0Update",
    0x08086BC0: "BrontoBurtTakeOffInit",
    0x08086BF0: "BrontoBurtTakeOffEnterState",
    0x08086C14: "BrontoBurtTakeOffUpdate",
    0x08086C5C: "BrontoBurtTakeOffWait",
    0x08086CCC: "BrontoBurtTakeOffWaitUpdate",
    0x08086D18: "BrontoBurtTakeOff",
    0x08086DA4: "BrontoBurtTakeOffState1Update",
    0x08086DF0: "BrontoBurtTakeOffState2",
    0x08086EC8: "BrontoBurtTakeOffState2Update",
    0x08086EFC: "BrontoBurtIdle",
    0x08086F40: "BrontoBurtIdleUpdate",
    0x08086F54: "TaskReflectFlightAngle",
    0x0808705C: "Task_Twizzy",
    0x080870A4: "TwizzyEnterVariant",
    0x080870C4: "TwizzyWaveInit",
    0x080870F4: "TwizzyWaveEnterState",
    0x08087118: "TwizzyWaveUpdate",
    0x0808713C: "TwizzyWave",
    0x08087210: "TwizzyWaveState0Update",
    0x08087268: "TwizzyWeaveInit",
    0x08087298: "TwizzyWeaveEnterState",
    0x080872BC: "TwizzyWeaveUpdate",
    0x080872E0: "TwizzyWeave",
    0x08087458: "TwizzySwoopInit",
    0x08087508: "TwizzySwoopUpdate",
    0x0808752C: "TwizzySwoop",
    0x08087724: "TwizzySwoopState0Update",
    0x08087790: "TwizzyDiagonalInit",
    0x08087824: "TwizzyDiagonalEnterState",
    0x08087848: "TwizzyDiagonalUpdate",
    0x080878F0: "TwizzyDiagonal",
    0x08087924: "TwizzyDiagonalState0Update",
    0x0808795C: "TwizzyChaseInit",
    0x0808798C: "TwizzyChaseEnterState",
    0x080879B0: "TwizzyChaseUpdate",
    0x080879D4: "TwizzyChase",
    0x08087A98: "TwizzyChaseState0Update",
    0x08087AF0: "TwizzyTakeOffInit",
    0x08087B20: "TwizzyTakeOffEnterState",
    0x08087B44: "TwizzyTakeOffUpdate",
    0x08087B8C: "TwizzyTakeOffWait",
    0x08087BFC: "TwizzyTakeOffWaitUpdate",
    0x08087C48: "TwizzyTakeOff",
    0x08087CD4: "TwizzyTakeOffState1Update",
    0x08087D20: "TwizzyTakeOffState2",
    0x08087DF8: "TwizzyTakeOffState2Update",
    0x08087E2C: "TwizzyVariant6",
    0x08087FCC: "TwizzyVariant7",
    0x08088360: "TwizzyHoverInit",
    0x08088394: "TwizzyHoverEnterState",
    0x080883B8: "TwizzyHoverUpdate",
    0x080883DC: "TwizzyHover",
    0x08088478: "TwizzyHoverState0Update",
    0x08088498: "TwizzyIdle",
    0x080884D0: "TwizzyIdleUpdate",
    0x080884E4: "TwizzyLand",
    0x08088540: "TwizzyStartFall",
    0x08088590: "TwizzyHitWall",
    0x080885C0: "TwizzyHitCeiling",
    0x08088610: "Task_Squishy",
    0x08088658: "SquishyWalkInit",
    0x080886B4: "SquishyWalkEnterState",
    0x080886D8: "SquishyWalkUpdate",
    0x08088708: "SquishyWalk",
    0x080887A0: "SquishyWalkState0Update",
    0x0808880C: "SquishyWalkState1",
    0x080888A0: "SquishyWalkState1Update",
    0x080888C8: "SquishyWalkFall",
    0x08088920: "SquishyWalkFallUpdate",
    0x08088948: "SquishyWalkState3",
    0x080889C8: "SquishyWalkState3Update",
    0x080889CC: "SquishyWalkState4",
    0x08088A3C: "SquishyWalkState4Update",
    0x08088A64: "SquishyVariant1",
    0x08088CE8: "SquishyVariant2",
    0x08088F4C: "SquishyIdle",
    0x08088FAC: "SquishyIdleUpdate",
    0x08088FC0: "SquishyLand",
    0x08089024: "SquishyStartFall",
    0x08089064: "SquishyEnterWater",
    0x080890D4: "SquishyLeaveWater",
    0x08089120: "SquishyBounceOffWall",
    0x0808913C: "SquishyHitCeiling",
    0x08089180: "Task_Bubbles",
    0x080891C0: "BubblesInit",
    0x080891F8: "BubblesEnterState",
    0x0808921C: "BubblesUpdate",
    0x0808924C: "BubblesJump",
    0x08089330: "BubblesJumpUpdate",
    0x08089334: "BubblesBounceOffWall",
    0x0808945C: "BubblesBounceOffWallUpdate",
    0x08089460: "BubblesBounceOffCeiling",
    0x08089544: "BubblesLand",
    0x080895C4: "BubblesLandUpdate",
    0x080895EC: "BubblesFall",
    0x0808967C: "BubblesFallUpdate",
    0x08089680: "BubblesIdle",
    0x080896DC: "BubblesIdleUpdate",
    0x0808972C: "BubblesStartFall",
    0x0808976C: "BubblesEnterWater",
    0x0808978C: "BubblesHitWall",
    0x080897D0: "BubblesHitCeiling",
    0x08089808: "BubblesSetRollFrame",
    0x08089848: "Task_Glunk",
    0x08089888: "GlunkInit",
    0x080898B8: "GlunkEnterState",
    0x080898DC: "GlunkUpdate",
    0x0808990C: "GlunkWait",
    0x080899D4: "GlunkWaitUpdate",
    0x080899FC: "GlunkShoot",
    0x08089AAC: "GlunkShootUpdate",
    0x08089AD4: "Task_GlunkShotSpray",
    0x08089B44: "GlunkIdle",
    0x08089BDC: "GlunkIdleUpdate",
    0x08089BF0: "GlunkLand",
    0x08089C0C: "GlunkStartFall",
    0x08089C30: "GlunkEnterWater",
    0x08089C58: "Task_Slippy",
    0x08089C98: "SlippyInit",
    0x08089D20: "SlippyEnterState",
    0x08089D44: "SlippyUpdate",
    0x08089DA8: "SlippyState0",
    0x08089EA0: "SlippyState0Update",
    0x08089EC8: "SlippyState1",
    0x0808A020: "SlippyState1Update",
    0x0808A048: "SlippyJump",
    0x0808A0A8: "SlippyJumpUpdate",
    0x0808A0AC: "SlippyHighJump",
    0x0808A10C: "SlippyHighJumpUpdate",
    0x0808A110: "SlippyState4",
    0x0808A1D0: "SlippyState4Update",
    0x0808A204: "SlippyState5",
    0x0808A270: "SlippyState5Update",
    0x0808A298: "SlippyState6",
    0x0808A36C: "SlippyState6Update",
    0x0808A3C4: "SlippyState7",
    0x0808A478: "SlippyState7Update",
    0x0808A4D0: "SlippyState8",
    0x0808A5B8: "SlippyState8Update",
    0x0808A610: "SlippyState9",
    0x0808A710: "SlippyState9Update",
    0x0808A768: "SlippyFall",
    0x0808A7A4: "SlippyFallUpdate",
    0x0808A7A8: "SlippyIdle",
    0x0808A7E0: "SlippyIdleUpdate",
    0x0808A7F4: "SlippyPickMove",
    0x0808A84C: "PickWeightedRandomIndex",
    0x0808A880: "SlippySetSwimAngleToPlayer",
    0x0808A8D4: "SlippyLand",
    0x0808A964: "SlippyStartFall",
    0x0808A9A8: "SlippyEnterWater",
    0x0808A9D8: "SlippyHitWall",
    0x0808AA28: "SlippyHitCeiling",
    0x0808AA68: "Task_Blipper",
    0x0808AAD8: "BlipperChaseInit",
    0x0808AB14: "BlipperChaseEnterState",
    0x0808AB38: "BlipperChaseUpdate",
    0x0808AB70: "BlipperChase",
    0x0808ADEC: "BlipperWaveInit",
    0x0808AE24: "BlipperWaveEnterState",
    0x0808AE48: "BlipperWaveUpdate",
    0x0808AE80: "BlipperWave",
    0x0808B1D0: "BlipperLeapInit",
    0x0808B210: "BlipperLeapEnterState",
    0x0808B234: "BlipperLeapUpdate",
    0x0808B368: "BlipperLeap",
    0x0808B99C: "CreateBlipperDroplet",
    0x0808B9D0: "Task_BlipperDroplet",
    0x0808BB24: "BlipperIdle",
    0x0808BB5C: "BlipperIdleUpdate",
    0x0808BB70: "BlipperLand",
    0x0808BC18: "BlipperStartFall",
    0x0808BC60: "BlipperHitWall",
    0x0808BD04: "BlipperHitCeiling",
    0x0808BDB4: "Task_Gip",
    0x0808BDF4: "GipInit",
    0x0808BE3C: "GipEnterState",
    0x0808BE58: "GipUpdate",
    0x0808BEA0: "GipWalk",
    0x0808BF1C: "GipWalkUpdate",
    0x0808BFC4: "GipWait",
    0x0808C004: "GipWaitUpdate",
    0x0808C02C: "GipClimbUp",
    0x0808C0FC: "GipClimbUpFromFloor",
    0x0808C1D0: "GipClimbUpUpdate",
    0x0808C260: "GipClimbDown",
    0x0808C32C: "GipClimbDownFromLedge",
    0x0808C3E8: "GipClimbDownUpdate",
    0x0808C478: "GipClimbOverTop",
    0x0808C4BC: "GipClimbOverTopUpdate",
    0x0808C4E4: "GipLetGo",
    0x0808C538: "GipLetGoUpdate",
    0x0808C53C: "GipJump",
    0x0808C5E8: "GipJumpUpdate",
    0x0808C610: "GipShoot",
    0x0808C684: "GipShootUpdate",
    0x0808C6AC: "GipIdle",
    0x0808C708: "GipIdleUpdate",
    0x0808C71C: "GipLand",
    0x0808C74C: "GipStartFall",
    0x0808C77C: "GipEnterWater",
    0x0808C79C: "GipHitWall",
    0x0808C7EC: "GipHitCeiling",
    0x0808C82C: "GipTrySnapToWall",
    0x0808C8BC: "GipPickNextState",
    0x0808C934: "CreateGipStar",
    0x0808C980: "GipSetLeapVelocity",
    0x0808CA00: "GipGetWallDistRight",
    0x0808CAB8: "GipGetWallDistLeft",
    0x0808CB70: "Task_ChillyFreeze",
    0x0808CC14: "ChillyFreezeUpdate",
    0x0808CCE8: "Task_ChillyFreezeSparkle",
    0x0808CFEC: "ChillyFreezeSparkleCheckParent",
    0x0808D014: "Task_WaddleDooBeam",
    0x0808D100: "WaddleDooBeamUpdate",
    0x0808D130: "WaddleDooBeamHitTerrain",
    0x0808D148: "Task_GlunkShot",
    0x0808D1D0: "GlunkShotUpdate",
    0x0808D200: "GlunkShotHitCeiling",
    0x0808D218: "Task_GipStar",
    0x0808D2A8: "GipStarUpdate",
    0x0808D2B8: "BroomHatterStartFall",
    0x0808D304: "BroomHatterLand",
    0x0808D354: "BroomHatterEnterWater",
    0x0808D364: "BroomHatterStopAtSlope",
    0x0808D3E4: "BroomHatterPickNextState",
    0x0808D4E8: "Task_BroomHatter",
    0x0808D558: "BroomHatterVariant0",
    0x0808DA00: "BroomHatterVariant1",
    0x0808DF58: "BroomHatterIdleInit",
    0x0808DF9C: "BroomHatterIdleUpdate",
    0x0808DFC4: "BroomHatterIdle",
    0x0808E050: "BroomHatterIdleState0Update",
    0x0808E054: "LaserBallTeardown",
    0x0808E070: "LaserBallSetTargetX",
    0x0808E174: "LaserBallSetMoveDir",
    0x0808E254: "CreateLaserBallLaser",
    0x0808E2B4: "LaserBallCheckShoot",
    0x0808E33C: "LaserBallReaim",
    0x0808E36C: "LaserBallAccelerateInMoveDir",
    0x0808E3A8: "Task_LaserBall",
    0x0808E404: "LaserBallInit",
    0x0808E440: "LaserBallUpdate",
    0x0808E464: "LaserBallEnterState",
    0x0808E480: "LaserBallApproach",
    0x0808E510: "LaserBallApproachUpdate",
    0x0808E54C: "LaserBallHover",
    0x0808E5CC: "LaserBallHoverUpdate",
    0x0808E610: "LaserBallShoot",
    0x0808E704: "LaserBallShootUpdate",
    0x0808E730: "LaserBallRetreat",
    0x0808E800: "LaserBallRetreatUpdate",
    0x0808E804: "LaserBallIdleInit",
    0x0808E848: "LaserBallIdleUpdate",
    0x0808E870: "LaserBallIdle",
    0x0808E8A0: "LaserBallIdleState0Update",
    0x0808E8A4: "CoconutLand",
    0x0808E8C4: "CoconutEnterWater",
    0x0808E8D4: "Task_Coconut",
    0x0808E914: "CoconutInit",
    0x0808E964: "CoconutUpdate",
    0x0808E994: "CoconutEnterState",
    0x0808E9B0: "CoconutWait",
    0x0808E9D4: "CoconutWaitUpdate",
    0x0808EA00: "CoconutFall",
    0x0808EB10: "CoconutExplode",
    0x0808EB24: "CoconutIdleInit",
    0x0808EB64: "CoconutIdleUpdate",
    0x0808EB94: "CoconutIdle",
    0x0808EBDC: "CoconutIdleState0Update",
    0x0808EBE0: "ShotzoStartFall",
    0x0808EC34: "ShotzoLand",
    0x0808EC90: "ShotzoHitWall",
    0x0808ECB4: "ShotzoEnterWater",
    0x0808ECE0: "ParasolShotzoReactToDefeat",
    0x0808ED38: "ShotzoTargetNearestPlayer",
    0x0808EE60: "ShotzoStepBarrel",
    0x0808EE9C: "ShotzoTestBarrelOnTarget",
    0x0808EEC4: "ShotzoSetRecoilVelocity",
    0x0808EF88: "ShotzoInitBarrel",
    0x0808EFDC: "CreateShotzoCannonball",
    0x0808F058: "CreateShotzoFixedCannonball",
    0x0808F0D0: "ShotzoAimBarrel",
    0x0808F1B4: "ShotzoCheckShoot",
    0x0808F224: "Task_Shotzo",
    0x0808F26C: "ShotzoAimInit",
    0x0808F2A0: "ShotzoAimUpdate",
    0x0808F2C4: "ShotzoFixedInit",
    0x0808F2FC: "ShotzoFixedUpdate",
    0x0808F320: "ParasolShotzoInit",
    0x0808F35C: "ParasolShotzoUpdate",
    0x0808F380: "ShotzoAimEnterState",
    0x0808F39C: "ShotzoFixedEnterState",
    0x0808F3B8: "ParasolShotzoEnterState",
    0x0808F3D4: "ShotzoAim",
    0x0808F400: "ShotzoAimState0Update",
    0x0808F41C: "ShotzoAimShoot",
    0x0808F4B4: "ShotzoAimShootUpdate",
    0x0808F4F8: "ShotzoAimFall",
    0x0808F51C: "ShotzoAimFallUpdate",
    0x0808F528: "ShotzoFixedState0",
    0x0808F578: "ShotzoFixedState0Update",
    0x0808F5CC: "ShotzoFixedShoot",
    0x0808F678: "ShotzoFixedShootUpdate",
    0x0808F6C0: "ShotzoFixedFall",
    0x0808F71C: "ShotzoFixedFallUpdate",
    0x0808F728: "ParasolShotzoAim",
    0x0808F75C: "ParasolShotzoAimUpdate",
    0x0808F7AC: "ParasolShotzoShoot",
    0x0808F844: "ParasolShotzoShootUpdate",
    0x0808F888: "ParasolShotzoState2",
    0x0808F8DC: "ParasolShotzoState2Update",
    0x0808F8E8: "ShotzoIdleInit",
    0x0808F930: "ShotzoIdleUpdate",
    0x0808F954: "ShotzoIdle",
    0x0808F974: "ShotzoIdleState0Update",
    0x0808F978: "ConerStartFall",
    0x0808F9B8: "ConerLand",
    0x0808F9D8: "ConerEnterWater",
    0x0808F9F8: "ConerBounceOffWall",
    0x0808FA10: "Task_Coner",
    0x0808FA50: "ConerInit",
    0x0808FA84: "ConerUpdate",
    0x0808FA98: "ConerEnterState",
    0x0808FAB4: "ConerWalk",
    0x0808FB50: "ConerState1",
    0x0808FB80: "ConerState2",
    0x0808FBAC: "ConerIdleInit",
    0x0808FBF0: "ConerIdleUpdate",
    0x0808FC00: "ConerIdle",
    0x0808FC40: "Task_LaserBallLaser",
    0x0808FC90: "LaserBallLaserInit",
    0x0808FCD4: "LaserBallLaserUpdate",
    0x0808FD1C: "LaserBallLaserEnterState",
    0x0808FD38: "LaserBallLaserState1",
    0x0808FD5C: "LaserBallLaserState1Update",
    0x0808FD60: "LaserBallLaserState0",
    0x0808FDB4: "LaserBallLaserState0Update",
    0x0808FDB8: "Task_ShotzoCannonball",
    0x0808FDF8: "ShotzoCannonballInit",
    0x0808FE28: "ShotzoCannonballUpdate",
    0x0808FE6C: "ShotzoCannonballEnterState",
    0x0808FE88: "ShotzoCannonballState0",
    0x0808FFE0: "ShotzoCannonballState0Update",
    0x0809000C: "Task_Bonkers",
    0x08090090: "BonkersInit",
    0x080900D8: "BonkersEnterState",
    0x080900F4: "BonkersUpdate",
    0x080901E0: "BonkersIntro",
    0x08090270: "BonkersIntroUpdate",
    0x08090298: "BonkersWalk",
    0x080903C8: "BonkersWalkUpdate",
    0x080903F0: "BonkersJump",
    0x080904AC: "BonkersJumpUpdate",
    0x080904D4: "BonkersHop",
    0x080905B0: "BonkersHopUpdate",
    0x080905D8: "BonkersDash",
    0x08090724: "BonkersDashUpdate",
    0x0809074C: "BonkersThrow",
    0x080908EC: "BonkersThrowUpdate",
    0x08090914: "BonkersSlam",
    0x080909AC: "BonkersSlamUpdate",
    0x080909D4: "BonkersJumpSlam",
    0x08090B18: "BonkersJumpSlamUpdate",
    0x08090B40: "BonkersTripleSlam",
    0x08090BF0: "BonkersTripleSlamUpdate",
    0x08090C18: "BonkersBounceOffWall",
    0x08090CA8: "BonkersBounceOffWallUpdate",
    0x08090CD0: "BonkersDefeat",
    0x08090DE4: "BonkersDefeatUpdate",
    0x08090E18: "BonkersCreateSlamStar",
    0x08090E54: "BonkersChooseNextState",
    0x08090EF0: "BonkersReactToDamage",
    0x08090F14: "BonkersReactToDefeat",
    0x08090F4C: "BonkersHitWall",
    0x08090FC0: "Task_BonkersHammerHitBox",
    0x08090FE0: "BonkersHammerHitBoxUpdate",
    0x080910C0: "Task_PoppyBrosSr",
    0x0809113C: "PoppyBrosSrInit",
    0x0809118C: "PoppyBrosSrEnterState",
    0x080911A8: "PoppyBrosSrUpdate",
    0x0809128C: "PoppyBrosSrIntro",
    0x080912F8: "PoppyBrosSrIntroUpdate",
    0x08091320: "PoppyBrosSrState1",
    0x08091368: "PoppyBrosSrState1Update",
    0x08091390: "PoppyBrosSrState2",
    0x08091558: "PoppyBrosSrState2Update",
    0x080915A4: "PoppyBrosSrState3",
    0x080915D0: "PoppyBrosSrState3Update",
    0x080915F8: "PoppyBrosSrState4",
    0x080916C4: "PoppyBrosSrState4Update",
    0x080916EC: "PoppyBrosSrState5",
    0x080917FC: "PoppyBrosSrState5Update",
    0x08091824: "PoppyBrosSrDefeat",
    0x0809191C: "PoppyBrosSrDefeatUpdate",
    0x08091B00: "PoppyBrosSrReactToDamage",
    0x08091B24: "PoppyBrosSrReactToDefeat",
    0x08091B60: "PoppyBrosSrHitWall",
    0x08091B6C: "Task_PoppyBrosSrHand",
    0x08091D24: "PoppyBrosSrHandUpdate",
    0x08091DDC: "Task_PoppyBrosSrHead",
    0x08091E18: "PoppyBrosSrHeadUpdate",
    0x08091F08: "Task_Bugzzy",
    0x08091F9C: "BugzzyInit",
    0x08091FE0: "BugzzyEnterState",
    0x08091FFC: "BugzzyUpdate",
    0x08092198: "BugzzyIntro",
    0x08092228: "BugzzyIntroUpdate",
    0x08092250: "BugzzyWalk",
    0x08092590: "BugzzyWalkUpdate",
    0x080925B8: "BugzzySummon",
    0x080926D4: "BugzzySummonUpdate",
    0x080926FC: "BugzzyCharge",
    0x080929EC: "BugzzyChargeUpdate",
    0x08092A14: "BugzzyState4",
    0x08092B30: "BugzzyState4Update",
    0x08092B58: "BugzzyState5",
    0x08092BD8: "BugzzyState5Update",
    0x08092C00: "BugzzyState6",
    0x08092CB4: "BugzzyState6Update",
    0x08092CDC: "BugzzyHop",
    0x08092E40: "BugzzyHopUpdate",
    0x08092E68: "BugzzyFall",
    0x08092F04: "BugzzyFallUpdate",
    0x08092F2C: "BugzzyBounceOffWall",
    0x08092FF4: "BugzzyBounceOffWallUpdate",
    0x0809301C: "BugzzyState11",
    0x08093084: "BugzzyState11Update",
    0x080930AC: "BugzzyBackdrop",
    0x08093354: "BugzzyBackdropUpdate",
    0x08093380: "BugzzyDefeat",
    0x08093488: "BugzzyDefeatUpdate",
    0x080934F8: "BugzzyChooseBackdrop",
    0x080937D0: "BugzzyHitWall",
    0x08093858: "BugzzyHitCeiling",
    0x08093868: "BugzzyReactToDamage",
    0x0809388C: "BugzzyReactToDefeat",
    0x080938E4: "Task_BugzzyAfterimage",
    0x0809397C: "BugzzyAfterimageUpdate",
    0x08093A00: "BugzzyPlaySfxForHeldPlayer",
    0x08093A24: "Task_BonkersNut",
    0x08093A64: "BonkersNutInit",
    0x08093A98: "BonkersNutUpdate",
    0x08093AC8: "BonkersNutState0",
    0x08093B80: "BonkersNutState0Update",
    0x08093BB0: "BonkersNutHitWall",
    0x08093BD4: "Task_PoppyBrosSrBomb",
    0x08093C30: "PoppyBrosSrBombInit",
    0x08093C60: "PoppyBrosSrBombEnterState",
    0x08093C7C: "PoppyBrosSrBombUpdate",
    0x08093CCC: "PoppyBrosSrBombHeld",
    0x08093CF8: "PoppyBrosSrBombHeldUpdate",
    0x08093DCC: "PoppyBrosSrBombFlight",
    0x08093E54: "PoppyBrosSrBombFlightUpdate",
    0x08093E58: "Task_PoppyBrosSrBombSpark",
    0x08093EDC: "PoppyBrosSrBombHitWall",
    0x08093F00: "PoppyBrosSrBombLand",
    0x08093F64: "Task_BugzzyLadybug",
    0x08093FE0: "BugzzyLadybugInit",
    0x08094010: "BugzzyLadybugUpdate",
    0x08094040: "BugzzyLadybugState0",
    0x08094144: "BugzzyLadybugState0Update",
    0x08094164: "BugzzyLadybugSetVelocity",
    0x080941AC: "Task_GrandWheelie",
    0x08094220: "GrandWheelieInit",
    0x08094290: "GrandWheelieEnterState",
    0x080942DC: "GrandWheelieUpdate",
    0x080945FC: "GrandWheelieFall",
    0x08094640: "GrandWheelieFallUpdate",
    0x0809465C: "GrandWheelieState1",
    0x0809467C: "GrandWheelieState1Update",
    0x080946B0: "GrandWheelieState2",
    0x08094758: "GrandWheelieState2Update",
    0x08094810: "GrandWheelieCheckSummon",
    0x08094844: "GrandWheelieHop",
    0x080948D4: "GrandWheelieHopUpdate",
    0x08094930: "GrandWheelieCharge",
    0x080949E0: "GrandWheelieChargeUpdate",
    0x08094B94: "GrandWheelieIsNearPlayer",
    0x08094DA4: "GrandWheelieState5",
    0x08094DEC: "GrandWheelieState5Update",
    0x08094E88: "GrandWheelieBrake",
    0x08094F28: "GrandWheelieState6",
    0x08094F68: "GrandWheelieState6Update",
    0x08094FB0: "GrandWheelieState7",
    0x080950B4: "GrandWheelieState7Update",
    0x0809513C: "GrandWheelieSummon",
    0x08095220: "GrandWheelieSummonUpdate",
    0x08095254: "GrandWheelieBounceOffWall",
    0x0809532C: "GrandWheelieBounceOffWallUpdate",
    0x08095360: "GrandWheelieDefeat",
    0x08095484: "GrandWheelieDefeatUpdate",
    0x080954F0: "GrandWheelieLand",
    0x080955A8: "GrandWheelieHitWall",
    0x08095674: "GrandWheelieReactToDamage",
    0x08095694: "GrandWheelieReactToDefeat",
    0x080956C8: "GrandWheelieStartAnim",
    0x080956E4: "GrandWheelieStepAnim",
    0x08095768: "GrandWheelieTickAnimFacingNearestPlayer",
    0x08095794: "GrandWheelieTickAnim",
    0x080957BC: "Task_FireLion",
    0x08095940: "FireLionEnterState",
    0x0809595C: "FireLionDropIn",
    0x080959E8: "FireLionDropInUpdate",
    0x08095A54: "FireLionWait",
    0x08095AEC: "FireLionHop",
    0x08095BE4: "FireLionHopUpdate",
    0x08095EAC: "FireLionSlash",
    0x08096058: "FireLionSlashUpdate",
    0x080960BC: "FireLionState9",
    0x0809619C: "FireLionCharge",
    0x08096278: "FireLionChargeUpdate",
    0x08096320: "FireLionBounceOffWall",
    0x080963DC: "FireLionPounce",
    0x08096640: "FireLionPounceUpdate",
    0x0809680C: "FireLionWaitForLanding",
    0x08096888: "FireLionJumpBack",
    0x080969C8: "FireLionUpdate",
    0x08096A40: "FireLionCheckCatch",
    0x08096B7C: "FireLionDefeat",
    0x08096D20: "FireLionDefeatUpdate",
    0x08096D64: "FireLionLand",
    0x08096DF4: "FireLionHitWall",
    0x08096E0C: "FireLionReactToDamage",
    0x08096E24: "FireLionReactToDefeat",
    0x08096E9C: "FireLionUpdatePalette",
    0x08096FC0: "FireLionChooseNextState",
    0x08097088: "FireLionCreateLandingStar",
    0x080970C4: "Task_FireLionFlame",
    0x080974C8: "FireLionFlameCheckParent",
    0x080974F8: "Task_PhanPhan",
    0x080975AC: "PhanPhanEnterState",
    0x080975FC: "PhanPhanUpdate",
    0x08097694: "PhanPhanDropIn",
    0x08097714: "PhanPhanDropInUpdate",
    0x08097844: "PhanPhanHopBackward",
    0x0809794C: "PhanPhanHopForward",
    0x08097A54: "PhanPhanState1Update",
    0x08097A7C: "PhanPhanHop",
    0x08097B4C: "PhanPhanHopUpdate",
    0x08097B74: "PhanPhanCharge",
    0x08097C44: "PhanPhanChargeUpdate",
    0x08097C78: "PhanPhanBounceOffWall",
    0x08097D7C: "PhanPhanBounceOffWallUpdate",
    0x08097DA4: "PhanPhanJump",
    0x08097E68: "PhanPhanJumpUpdate",
    0x08097E90: "PhanPhanThrowPlayer",
    0x0809816C: "PhanPhanThrowPlayerUpdate",
    0x08098194: "PhanPhanThrowApple",
    0x08098268: "PhanPhanThrowAppleUpdate",
    0x0809829C: "PhanPhanDefeat",
    0x080983A0: "PhanPhanDefeatUpdate",
    0x080983A4: "PhanPhanChooseNextState",
    0x08098450: "CreatePhanPhanApple",
    0x080984B4: "PhanPhanCheckCatch",
    0x08098528: "PhanPhanLand",
    0x08098540: "PhanPhanHitWall",
    0x0809857C: "PhanPhanReactToDamage",
    0x08098594: "PhanPhanReactToDefeat",
    0x080985E0: "Task_GrandWheelieMiniWheelie",
    0x0809869C: "GrandWheelieMiniWheelieUpdate",
    0x080986EC: "GrandWheelieMiniWheelieLand",
    0x08098718: "GrandWheelieMiniWheelieEnterWater",
    0x08098728: "GrandWheelieMiniWheelieHitWall",
    0x08098738: "GrandWheelieMiniWheelieHitCeiling",
    0x0809876C: "Task_PhanPhanApple",
    0x0809887C: "PhanPhanAppleUpdate",
    0x080988A4: "PhanPhanAppleLand",
    0x080988B4: "PhanPhanAppleHitWall",
    0x080988C4: "MrFrostyStartFall",
    0x080988F8: "MrFrostyLand",
    0x08098A04: "MrFrostyHitWall",
    0x08098AA0: "MrFrostyReactToDefeat",
    0x08098AD8: "MrFrostyReactToDamage",
    0x08098AFC: "TaskFreeDustTrail",
    0x08098B60: "MrFrostyChooseNextState",
    0x08098CF4: "MrFrostyPickToss",
    0x08098D58: "CreateMrFrostyIceCube",
    0x08098DA4: "MrFrostyCheckNearIceCube",
    0x08098DE4: "MrFrostyUpdateAttackBoxes",
    0x08098E64: "Task_MrFrosty",
    0x08098ED4: "MrFrostyInit",
    0x08098F38: "MrFrostyUpdate",
    0x08098FB0: "MrFrostyEnterState",
    0x08098FD0: "MrFrostyWait",
    0x08099004: "MrFrostyWaitUpdate",
    0x08099020: "MrFrostyHop",
    0x08099080: "MrFrostyHopUpdate",
    0x080990D4: "MrFrostyWalkBack",
    0x08099180: "MrFrostyWalkBackUpdate",
    0x080991AC: "MrFrostyDash",
    0x08099238: "MrFrostyDashUpdate",
    0x08099244: "MrFrostyBounceOffWall",
    0x080992A8: "MrFrostyBounceOffWallUpdate",
    0x080992AC: "MrFrostyState5",
    0x0809931C: "MrFrostyState5Update",
    0x08099350: "MrFrostyState6",
    0x08099394: "MrFrostyState6Update",
    0x080993DC: "MrFrostyWalkBackAtWall",
    0x08099448: "MrFrostyWalkBackAtWallUpdate",
    0x08099474: "MrFrostySpin",
    0x08099508: "MrFrostySpinUpdate",
    0x08099524: "MrFrostyTossIceCubeHopBack",
    0x080995B8: "MrFrostyTossIceCubeHopBackUpdate",
    0x080995F4: "MrFrostyTossIceCubeHopAway",
    0x08099690: "MrFrostyTossIceCubeHopAwayUpdate",
    0x080996D0: "MrFrostyTossIceCubeWalkBack",
    0x08099734: "MrFrostyTossIceCubeWalkBackUpdate",
    0x08099770: "MrFrostyKick",
    0x080997E4: "MrFrostyKickUpdate",
    0x08099818: "MrFrostyDefeat",
    0x08099890: "MrFrostyDefeatUpdate",
    0x080998A4: "MrFrostyState14",
    0x08099908: "MrFrostyState14Update",
    0x08099944: "MrFrostyState15",
    0x0809998C: "MrFrostyState15Update",
    0x080999C4: "MrFrostyFall",
    0x08099A0C: "MrFrostyFallUpdate",
    0x08099A10: "MrFrostyState17",
    0x08099A54: "MrFrostyState17Update",
    0x08099A7C: "MrFrostyDropIn",
    0x08099AD0: "MrFrostyDropInUpdate",
    0x08099AEC: "MrTickTockStartFall",
    0x08099B20: "MrTickTockLand",
    0x08099C4C: "MrTickTockHitWall",
    0x08099D40: "MrTickTockReactToDamage",
    0x08099D64: "MrTickTockReactToDefeat",
    0x08099DB0: "MrTickTockCheckJumpLow",
    0x08099DEC: "MrTickTockChooseNextState",
    0x08099E9C: "CreateMrTickTockRing",
    0x08099EE4: "CreateMrTickTockNote",
    0x08099FB4: "MrTickTockIsFarFromPlayer",
    0x0809A03C: "MrTickTockUpdateAttackBoxes",
    0x0809A0A8: "Task_MrTickTock",
    0x0809A118: "MrTickTockInit",
    0x0809A17C: "MrTickTockUpdate",
    0x0809A1F4: "MrTickTockEnterState",
    0x0809A214: "MrTickTockWaitLong",
    0x0809A270: "MrTickTockWaitLongUpdate",
    0x0809A2A0: "MrTickTockPickMove",
    0x0809A2C0: "MrTickTockPickMoveUpdate",
    0x0809A2E8: "MrTickTockHop",
    0x0809A32C: "MrTickTockHopUpdate",
    0x0809A36C: "MrTickTockWalkBack",
    0x0809A434: "MrTickTockWalkBackUpdate",
    0x0809A464: "MrTickTockJumpForward",
    0x0809A4F0: "MrTickTockJumpForwardUpdate",
    0x0809A528: "MrTickTockDashWindUp",
    0x0809A624: "MrTickTockDashWindUpUpdate",
    0x0809A654: "MrTickTockDashStart",
    0x0809A744: "MrTickTockDashStartUpdate",
    0x0809A798: "MrTickTockJumpHigh",
    0x0809A7D8: "MrTickTockJumpHighUpdate",
    0x0809A7DC: "MrTickTockRingFromJump",
    0x0809A82C: "MrTickTockRingFromJumpUpdate",
    0x0809A868: "MrTickTockJumpLow",
    0x0809A8B4: "MrTickTockJumpLowUpdate",
    0x0809A8B8: "MrTickTockJumpBack",
    0x0809A918: "MrTickTockJumpBackUpdate",
    0x0809A91C: "MrTickTockShootNotes",
    0x0809A974: "MrTickTockShootNotesUpdate",
    0x0809AA24: "MrTickTockDashLoop",
    0x0809AAF0: "MrTickTockDashLoopUpdate",
    0x0809AB70: "MrTickTockRingFromDash",
    0x0809AC54: "MrTickTockRingFromDashUpdate",
    0x0809ACBC: "MrTickTockWaitShort",
    0x0809AD6C: "MrTickTockWaitShortUpdate",
    0x0809ADF4: "MrTickTockBounceOffWall",
    0x0809AE3C: "MrTickTockBounceOffWallUpdate",
    0x0809AE40: "MrTickTockState16",
    0x0809AEFC: "MrTickTockState16Update",
    0x0809AF4C: "MrTickTockState17",
    0x0809B09C: "MrTickTockState17Update",
    0x0809B104: "MrTickTockState18",
    0x0809B214: "MrTickTockDefeat",
    0x0809B298: "MrTickTockDefeatUpdate",
    0x0809B2AC: "MrTickTockState20",
    0x0809B310: "MrTickTockState20Update",
    0x0809B34C: "MrTickTockState21",
    0x0809B394: "MrTickTockState21Update",
    0x0809B3D4: "MrTickTockFall",
    0x0809B404: "MrTickTockFallUpdate",
    0x0809B408: "MrTickTockDropIn",
    0x0809B438: "MrTickTockDropInUpdate",
    0x0809B454: "MrFrostyIceCubeLand",
    0x0809B4D8: "MrFrostyIceCubeStartFall",
    0x0809B4DC: "MrFrostyIceCubeHitWall",
    0x0809B4FC: "MrFrostyIceCubePickArc",
    0x0809B528: "Task_MrFrostyIceCube",
    0x0809B57C: "MrFrostyIceCubeInit",
    0x0809B5B4: "MrFrostyIceCubeUpdate",
    0x0809B5EC: "MrFrostyIceCubeEnterState",
    0x0809B608: "MrFrostyIceCubeState0",
    0x0809B6AC: "MrFrostyIceCubeState0Update",
    0x0809B6F8: "MrFrostyIceCubeFlight",
    0x0809B790: "MrFrostyIceCubeFlightUpdate",
    0x0809B794: "MrFrostyIceCubeBurst",
    0x0809B7EC: "MrFrostyIceCubeBurstUpdate",
    0x0809B7F0: "Task_MrTickTockRing",
    0x0809B830: "MrTickTockRingInit",
    0x0809B868: "MrTickTockRingUpdate",
    0x0809B8AC: "MrTickTockRingEnterState",
    0x0809B8C8: "MrTickTockRingState0",
    0x0809B964: "MrTickTockRingState0Update",
    0x0809B9C0: "MrTickTockNoteLand",
    0x0809B9E0: "MrTickTockNoteHitWall",
    0x0809BA00: "Task_MrTickTockNote",
    0x0809BA44: "MrTickTockNoteInit",
    0x0809BA94: "MrTickTockNoteUpdate",
    0x0809BAEC: "MrTickTockNoteEnterState",
    0x0809BB08: "MrTickTockNoteFlight",
    0x0809BB68: "MrTickTockNoteFlightUpdate",
    0x0809BB6C: "MrTickTockNoteVanish",
    0x0809BBD4: "MrTickTockNoteVanishUpdate",
    0x0809BBD8: "Task_MetaKnights",
    0x0809BFAC: "MetaKnightsLoadGfx",
    0x0809C404: "Task_MetaKnightsKnight",
    0x0809C4D0: "AxeKnightVariant0",
    0x0809C50C: "AxeKnightEnterState",
    0x0809C528: "AxeKnightUpdate",
    0x0809C570: "AxeKnightWalk",
    0x0809C638: "AxeKnightWalkUpdate",
    0x0809C74C: "AxeKnightSlash",
    0x0809C840: "AxeKnightSlashUpdate",
    0x0809C880: "AxeKnightThrow",
    0x0809C8F8: "AxeKnightThrowUpdate",
    0x0809C938: "AxeKnightCatchAxe",
    0x0809C984: "AxeKnightJumpThrow",
    0x0809CA0C: "AxeKnightJumpThrowUpdate",
    0x0809CA10: "AxeKnightState4",
    0x0809CAAC: "AxeKnightState4Update",
    0x0809CAB0: "AxeKnightVariant1",
    0x0809CC24: "AxeKnightVariant2",
    0x0809CD8C: "AxeKnightVariant3",
    0x0809CF04: "CreateAxeKnightAxe",
    0x0809CF60: "Task_AxeKnightAxe",
    0x0809CFE0: "AxeKnightAxeUpdate",
    0x0809D0A0: "AxeKnightLand",
    0x0809D0DC: "AxeKnightHitWall",
    0x0809D138: "AxeKnightHitCeiling",
    0x0809D18C: "JavelinKnightVariant0",
    0x0809D1C0: "JavelinKnightEnterState",
    0x0809D1DC: "JavelinKnightUpdate",
    0x0809D25C: "JavelinKnightState0",
    0x0809D280: "JavelinKnightHop",
    0x0809D308: "JavelinKnightState0Update",
    0x0809D30C: "JavelinKnightState2",
    0x0809D4A0: "JavelinKnightJumpThrow",
    0x0809D56C: "JavelinKnightJump",
    0x0809D638: "JavelinKnightState5",
    0x0809D71C: "JavelinKnightChooseNextState",
    0x0809D7A4: "JavelinKnightVariant1",
    0x0809D944: "CreateJavelinKnightJavelin",
    0x0809DA1C: "Task_JavelinKnightJavelin",
    0x0809DA9C: "JavelinKnightJavelinUpdate",
    0x0809DB48: "JavelinKnightLand",
    0x0809DBC4: "JavelinKnightHitWall",
    0x0809DC3C: "JavelinKnightHitCeiling",
    0x0809DCBC: "MaceKnightStand",
    0x0809DD08: "MaceKnightStandUpdate",
    0x0809DD7C: "MaceKnightVariant1",
    0x0809DDA0: "MaceKnightEnterState",
    0x0809DDBC: "MaceKnightUpdate",
    0x0809DE54: "MaceKnightWalk",
    0x0809DEE0: "MaceKnightWalkUpdate",
    0x0809DF08: "MaceKnightThrow",
    0x0809DF2C: "MaceKnightThrowUpdate",
    0x0809DF54: "MaceKnightVariant2",
    0x0809E04C: "MaceKnightThrowMace",
    0x0809E214: "CreateMaceKnightMace",
    0x0809E284: "Task_MaceKnightMace",
    0x0809E2C4: "MaceKnightMaceMove",
    0x0809E670: "MaceKnightMaceVariant2",
    0x0809E7C8: "MaceKnightLand",
    0x0809E7D4: "MaceKnightStartFall",
    0x0809E7E8: "MaceKnightHitWall",
    0x0809E820: "MaceKnightHitCeiling",
    0x0809E874: "TridentKnightVariant0",
    0x0809E8B0: "TridentKnightEnterState",
    0x0809E8CC: "TridentKnightUpdate",
    0x0809E914: "TridentKnightWalk",
    0x0809EAB8: "TridentKnightJump",
    0x0809EB10: "TridentKnightJumpUpdate",
    0x0809EB14: "TridentKnightThrow",
    0x0809EB50: "TridentKnightThrowUpdate",
    0x0809EB7C: "TridentKnightJumpThrow",
    0x0809EBBC: "TridentKnightJumpThrowUpdate",
    0x0809EDDC: "TridentKnightVariant1",
    0x0809EFC8: "TridentKnightVariant2",
    0x0809F120: "TridentKnightVariant3",
    0x0809F29C: "CreateTridentKnightTrident",
    0x0809F37C: "Task_TridentKnightTrident",
    0x0809F3E0: "TridentKnightTridentUpdate",
    0x0809F478: "TridentKnightTridentVariant0",
    0x0809F49C: "TridentKnightTridentVariant1",
    0x0809F4C0: "TridentKnightTridentVariant2",
    0x0809F4E4: "TridentKnightTridentVariant3",
    0x0809F508: "TridentKnightTridentVariant4",
    0x0809F52C: "TridentKnightLand",
    0x0809F588: "TridentKnightHitWall",
    0x0809F618: "TridentKnightHitCeiling",
    0x0809F7F8: "MetaKnightsKnightReactToDamage",
    0x0809F808: "MetaKnightsKnightTeardown",
    0x0809F8D4: "TaskFaceScreenCenter",
    0x0809F9DC: "MetaKnightsKnightFlashPalette",
    0x0809FB10: "MetaKnightsKnightRestorePalette",
    0x0809FC44: "Task_KingDedede",
    0x0809FCB4: "KingDededeStartHitStun",
    0x0809FD20: "KingDededeEndHitStun",
    0x0809FD64: "KingDededeReactToDefeat",
    0x0809FE10: "KingDededeReactToDamage",
    0x080A0028: "CreateKingDededeStar",
    0x080A0274: "CreateKingDededeLandingStar",
    0x080A028C: "CreateKingDededeAirPuff",
    0x080A02D4: "KingDededeCreateImpactStars",
    0x080A0358: "KingDededeStartWalk",
    0x080A043C: "KingDededePickSlamKind",
    0x080A0480: "KingDededeAimHighJump",
    0x080A0538: "KingDededeLand",
    0x080A0588: "KingDededeHitWall",
    0x080A0598: "KingDededeHitCeiling",
    0x080A0658: "KingDededeStartWait",
    0x080A06F0: "KingDededeChooseNextState",
    0x080A0768: "KingDededeJumpSlam",
    0x080A0844: "KingDededeGroundSlam",
    0x080A0A0C: "KingDededeInit",
    0x080A0A38: "KingDededeUpdate",
    0x080A0A84: "KingDededeHitStunLateUpdate",
    0x080A0B10: "KingDededeEnterState",
    0x080A0B30: "KingDededeIntro",
    0x080A0B74: "KingDededeIntroUpdate",
    0x080A0BB4: "KingDededeWait",
    0x080A0BDC: "KingDededeWaitUpdate",
    0x080A0C08: "KingDededeWalk",
    0x080A0C28: "KingDededeWalkUpdate",
    0x080A0CCC: "KingDededeJump",
    0x080A0DBC: "KingDededeJumpUpdate",
    0x080A0E08: "KingDededeFloat",
    0x080A0EC8: "KingDededeFloatUpdate",
    0x080A0F7C: "KingDededeExhale",
    0x080A1030: "KingDededeExhaleUpdate",
    0x080A1058: "KingDededeHighJump",
    0x080A1140: "KingDededeHighJumpUpdate",
    0x080A1168: "KingDededeSlam",
    0x080A11A0: "KingDededeSlamUpdate",
    0x080A11E0: "KingDededeInhale",
    0x080A12E0: "KingDededeInhaleUpdate",
    0x080A13D4: "KingDededeSpit",
    0x080A1400: "KingDededeSpitUpdate",
    0x080A146C: "KingDededeFall",
    0x080A14E4: "KingDededeFallUpdate",
    0x080A150C: "KingDededeDefeatedInit",
    0x080A1550: "KingDededeDefeatedUpdate",
    0x080A1570: "KingDededeDefeatedEnterState",
    0x080A1590: "KingDededeDefeatedFall",
    0x080A15F0: "KingDededeDefeatedFallUpdate",
    0x080A18D4: "KingDededeDefeatedHoldBack",
    0x080A1980: "KingDededeDefeatedHoldBackUpdate",
    0x080A19CC: "Task_MrShineAndMrBright",
    0x080A19EC: "MrShineAndMrBrightFillHpBars",
    0x080A1AD0: "CreateMrShineAndMrBright",
    0x080A1B94: "MrShineAndMrBrightPickFirstAscender",
    0x080A1BD8: "MrShineAndMrBrightSetStates",
    0x080A1C1C: "MrShineAndMrBrightMoveToMidpoint",
    0x080A1C90: "MrShineAndMrBrightStartDefeat",
    0x080A1DBC: "MrShineAndMrBrightDefeatSweepFilter",
    0x080A1DD4: "MrShineAndMrBrightCheckAscend",
    0x080A1DF8: "MrShineAndMrBrightLand",
    0x080A1E4C: "MrShineAndMrBrightHitWall",
    0x080A1EC4: "MrShineAndMrBrightReactToDefeat",
    0x080A1F90: "MrShineAndMrBrightHitStunUpdate",
    0x080A1FC8: "MrShineAndMrBrightReactToDamage",
    0x080A2030: "MrShineAndMrBrightStartDescend",
    0x080A2090: "MrShineAndMrBrightStartAscend",
    0x080A2164: "MrShineAndMrBrightDescendUpdate",
    0x080A21A0: "MrShineAndMrBrightAscendUpdate",
    0x080A2274: "MrShineAndMrBrightCheckChase",
    0x080A22B0: "MrShineAndMrBrightGetWaitTime",
    0x080A22D4: "MrShineAndMrBrightPickGroundMove",
    0x080A2390: "MrShineAndMrBrightClampToRoom",
    0x080A23C0: "MrShineAndMrBrightPickJumpDir",
    0x080A2400: "MrShineAndMrBrightEndSkyAttack",
    0x080A2448: "MrShineAndMrBrightEndChase",
    0x080A2590: "MrShineAndMrBrightCheckDescend",
    0x080A25C4: "MrShineAndMrBrightWaitToAttackUpdate",
    0x080A2608: "MrShineAndMrBrightChaseUpdate",
    0x080A263C: "MrShineAndMrBrightWaitToDescendUpdate",
    0x080A268C: "MrShineAndMrBrightStopNearParent",
    0x080A2754: "MrShineAndMrBrightAimAtParent",
    0x080A2814: "MrShineUpdatePalette",
    0x080A28D0: "MrShineUpdateWalkSpeed",
    0x080A291C: "CreateMrShineCrescent",
    0x080A2954: "CreateMrShineFallingStar",
    0x080A2994: "MrShineSetSkyFrame",
    0x080A29CC: "MrShineStartWaitToAttack",
    0x080A2A24: "MrShineStartDropStars",
    0x080A2A94: "MrBrightUpdateBob",
    0x080A2BC4: "MrBrightUpdatePalette",
    0x080A2C90: "MrShineAndMrBrightEndFlash",
    0x080A2CA8: "CreateMrBrightFireball",
    0x080A2CE8: "CreateMrBrightBeam",
    0x080A2D38: "CreateMrBrightBeamEffects",
    0x080A2E88: "MrBrightStartSkyAnim",
    0x080A2EDC: "MrBrightStartWaitToAttack",
    0x080A301C: "MrBrightMoveTowardNearestPlayer",
    0x080A306C: "MrBrightRemoveBeam",
    0x080A3114: "MrShineAndMrBrightInit",
    0x080A314C: "MrShineAndMrBrightUpdate",
    0x080A3168: "MrShineAndMrBrightEnterState",
    0x080A3184: "MrShineAndMrBrightState0",
    0x080A31A4: "MrShineAndMrBrightState0Update",
    0x080A31D0: "MrShineAndMrBrightState1",
    0x080A31F0: "MrShineAndMrBrightState1Update",
    0x080A3238: "MrShineAndMrBrightState2",
    0x080A3250: "MrShineAndMrBrightState2Update",
    0x080A3268: "MrShineAndMrBrightState3",
    0x080A3280: "MrShineAndMrBrightState3Update",
    0x080A3298: "MrShineInit",
    0x080A32C4: "MrShineUpdate",
    0x080A332C: "MrShineEnterState",
    0x080A3348: "MrShineFall",
    0x080A33A4: "MrShineFallUpdate",
    0x080A33DC: "MrShineWait",
    0x080A3424: "MrShineWaitUpdate",
    0x080A346C: "MrShineAscend",
    0x080A348C: "MrShineAscendUpdate",
    0x080A349C: "MrShineWaitToAttack",
    0x080A34B8: "MrShineWaitToAttackUpdate",
    0x080A34C8: "MrShineState4",
    0x080A34F0: "MrShineState4Update",
    0x080A352C: "MrShineDropStars",
    0x080A3548: "MrShineDropStarsUpdate",
    0x080A3588: "MrShineChase",
    0x080A35A8: "MrShineChaseUpdate",
    0x080A35B8: "MrShineWaitToDescend",
    0x080A35D8: "MrShineWaitToDescendUpdate",
    0x080A35E8: "MrShineDescend",
    0x080A361C: "MrShineDescendUpdate",
    0x080A3628: "MrShineWalk",
    0x080A368C: "MrShineWalkUpdate",
    0x080A36C8: "MrShineJump",
    0x080A3768: "MrShineJumpUpdate",
    0x080A37AC: "MrShineJumpBack",
    0x080A3840: "MrShineJumpBackUpdate",
    0x080A387C: "MrShineDash",
    0x080A38EC: "MrShineDashUpdate",
    0x080A3954: "MrShineRecoil",
    0x080A3A10: "MrShineRecoilUpdate",
    0x080A3A40: "MrShineThrow",
    0x080A3B10: "MrShineThrowUpdate",
    0x080A3B4C: "MrShineState15",
    0x080A3B78: "MrShineState15Update",
    0x080A3B7C: "MrShineState16",
    0x080A3BC0: "MrShineState17",
    0x080A3C28: "MrBrightInit",
    0x080A3C54: "MrBrightUpdate",
    0x080A3CBC: "MrBrightEnterState",
    0x080A3CD8: "MrBrightFall",
    0x080A3D3C: "MrBrightFallUpdate",
    0x080A3D84: "MrBrightWait",
    0x080A3DD0: "MrBrightWaitUpdate",
    0x080A3E10: "MrBrightAscend",
    0x080A3E3C: "MrBrightAscendUpdate",
    0x080A3E60: "MrBrightWaitToAttack",
    0x080A3E7C: "MrBrightWaitToAttackUpdate",
    0x080A3EA0: "MrBrightState4",
    0x080A3EDC: "MrBrightState4Update",
    0x080A3F24: "MrBrightState5",
    0x080A3F54: "MrBrightState5Update",
    0x080A3FB8: "MrBrightChase",
    0x080A3FD4: "MrBrightChaseUpdate",
    0x080A3FF8: "MrBrightWaitToDescend",
    0x080A4018: "MrBrightWaitToDescendUpdate",
    0x080A403C: "MrBrightDescend",
    0x080A406C: "MrBrightDescendUpdate",
    0x080A408C: "MrBrightHop",
    0x080A412C: "MrBrightHopUpdate",
    0x080A4180: "MrBrightJump",
    0x080A4200: "MrBrightJumpUpdate",
    0x080A4260: "MrBrightJumpBack",
    0x080A42F8: "MrBrightJumpBackUpdate",
    0x080A4350: "MrBrightDash",
    0x080A43D8: "MrBrightDashUpdate",
    0x080A4430: "MrBrightRecoil",
    0x080A44C4: "MrBrightRecoilUpdate",
    0x080A4508: "MrBrightThrow",
    0x080A45BC: "MrBrightThrowUpdate",
    0x080A4604: "MrBrightState15",
    0x080A4630: "MrBrightState15Update",
    0x080A4634: "MrBrightState16",
    0x080A4678: "MrBrightState17",
    0x080A4708: "MrShineAndMrBrightDropStarRodPiece",
    0x080A472C: "MrShineAndMrBrightVariant3",
    0x080A498C: "Task_KingDededeStar",
    0x080A49CC: "KingDededeStarReleasePlayer",
    0x080A4A1C: "KingDededeStarInit",
    0x080A4A60: "KingDededeStarUpdate",
    0x080A4AA8: "KingDededeStarEnterState",
    0x080A4B68: "Task_KingDededeAirPuff",
    0x080A4BA8: "KingDededeAirPuffInit",
    0x080A4BDC: "KingDededeAirPuffUpdate",
    0x080A4C3C: "KingDededeAirPuffState0",
    0x080A4C80: "KingDededeAirPuffState0Update",
    0x080A4C84: "Task_MrShineAndMrBrightAttack",
    0x080A4CC4: "MrShineCrescentInit",
    0x080A4D00: "MrShineCrescentUpdate",
    0x080A4D6C: "MrShineCrescentEnterState",
    0x080A4D88: "MrShineCrescentState0",
    0x080A4DD8: "MrShineCrescentState0Update",
    0x080A4DF4: "MrShineCrescentState1",
    0x080A4E14: "MrShineFallingStarSetStart",
    0x080A4E9C: "MrShineFallingStarInit",
    0x080A4EE0: "MrShineFallingStarUpdate",
    0x080A4F40: "MrShineFallingStarState0",
    0x080A5008: "MrShineFallingStarState0Update",
    0x080A5020: "MrShineFallingStarState1",
    0x080A5040: "MrBrightFireballInit",
    0x080A5084: "MrBrightFireballUpdate",
    0x080A50F0: "MrBrightFireballEnterState",
    0x080A510C: "MrBrightFireballState0",
    0x080A5164: "MrBrightFireballState0Update",
    0x080A5168: "MrBrightFireballState1",
    0x080A5188: "MrBrightBeamInit",
    0x080A51DC: "MrBrightBeamUpdate",
    0x080A523C: "MrBrightBeamDropStars",
    0x080A528C: "MrBrightBeamState0",
    0x080A52C8: "MrBrightBeamState0Update",
    0x080A5304: "MrBrightBeamState1",
    0x080A5324: "Task_KingDededeLandingStar",
    0x080A5388: "Task_KingDededeHammerHitBox",
    0x080A53A8: "KingDededeHammerHitBoxUpdate",
    0x080A5484: "Task_KingDededeInhaleHitBox",
    0x080A54A4: "KingDededeInhaleHitBoxUpdate",
    0x080A54E4: "Task_MrBrightBeamEffect",
    0x080A5524: "MrBrightBeamEffectVariant0",
    0x080A556C: "MrBrightBeamEffectVariant1",
    0x080A55AC: "MrBrightBeamEffectVariant2",
    0x080A55EC: "Task_MetaKnight",
    0x080A5644: "MetaKnightInit",
    0x080A5694: "MetaKnightEnterState",
    0x080A56B0: "MetaKnightUpdate",
    0x080A57D4: "MetaKnightIntro",
    0x080A5A78: "MetaKnightIntroUpdate",
    0x080A5AA0: "MetaKnightFollow",
    0x080A5AF4: "MetaKnightFollowUpdate",
    0x080A5DD0: "MetaKnightState2",
    0x080A5E30: "MetaKnightState2Update",
    0x080A5E60: "MetaKnightState3",
    0x080A5E9C: "MetaKnightState3Update",
    0x080A5ECC: "MetaKnightState4",
    0x080A5EF0: "MetaKnightState4Update",
    0x080A5F20: "MetaKnightState5",
    0x080A5F7C: "MetaKnightState5Update",
    0x080A5FAC: "MetaKnightApproach",
    0x080A6020: "MetaKnightApproachUpdate",
    0x080A60D8: "MetaKnightRun",
    0x080A6130: "MetaKnightRunUpdate",
    0x080A61B4: "MetaKnightJumpLow",
    0x080A6264: "MetaKnightJumpLowUpdate",
    0x080A6280: "MetaKnightJumpHigh",
    0x080A6330: "MetaKnightJumpHighUpdate",
    0x080A634C: "MetaKnightLand",
    0x080A638C: "MetaKnightLandUpdate",
    0x080A63A4: "MetaKnightStartJumpForward",
    0x080A63D8: "MetaKnightStartJumpForwardUpdate",
    0x080A63F0: "MetaKnightStartJumpUp",
    0x080A6420: "MetaKnightStartJumpUpUpdate",
    0x080A6438: "MetaKnightSwordSpin",
    0x080A650C: "MetaKnightSwordSpinUpdate",
    0x080A6544: "MetaKnightStartJumpDownThrust",
    0x080A6574: "MetaKnightStartJumpDownThrustUpdate",
    0x080A658C: "MetaKnightDownThrust",
    0x080A6628: "MetaKnightDownThrustUpdate",
    0x080A6650: "MetaKnightStartJumpUpwardSlash",
    0x080A6680: "MetaKnightStartJumpUpwardSlashUpdate",
    0x080A6698: "MetaKnightUpwardSlashInAir",
    0x080A66C8: "MetaKnightUpwardSlashInAirUpdate",
    0x080A6710: "MetaKnightUpwardSlashOnGround",
    0x080A6734: "MetaKnightUpwardSlashOnGroundUpdate",
    0x080A6754: "MetaKnightSlashShort",
    0x080A680C: "MetaKnightSlashShortUpdate",
    0x080A6868: "MetaKnightDoubleSlash",
    0x080A6988: "MetaKnightDoubleSlashUpdate",
    0x080A69E4: "MetaKnightSlashLong",
    0x080A6AAC: "MetaKnightSlashLongUpdate",
    0x080A6B3C: "MetaKnightState22",
    0x080A6C3C: "MetaKnightState22Update",
    0x080A6D08: "MetaKnightState23",
    0x080A6D60: "MetaKnightState23Update",
    0x080A6E1C: "MetaKnightGetHealthQuarter",
    0x080A6E58: "MetaKnightSetFollowTimer",
    0x080A6E98: "MetaKnightAnimateWalk",
    0x080A6F74: "MetaKnightClampToRoom",
    0x080A6FB4: "MetaKnightPickAirAttack",
    0x080A7080: "MetaKnightUpwardSlash",
    0x080A7190: "MetaKnightPickNextState",
    0x080A720C: "MetaKnightTryGuard",
    0x080A7260: "MetaKnightReactToDamage",
    0x080A72B0: "MetaKnightHitStunUpdate",
    0x080A72FC: "MetaKnightReactToDefeat",
    0x080A7334: "MetaKnightHitWall",
    0x080A73B4: "MetaKnightDefeatedInit",
    0x080A73FC: "MetaKnightDefeatedUpdate",
    0x080A741C: "MetaKnightDefeatedEnterState",
    0x080A7880: "Task_MetaKnightSwordHitBox",
    0x080A78A0: "MetaKnightSwordHitBoxUpdate",
    0x080A7998: "Task_MetaKnightCape",
    0x080A7AE4: "Task_MetaKnightMask",
    0x080A7B88: "Task_MetaKnightMaskHalf",
    0x080A7C9C: "Task_MetaKnightSparkle",
    0x080A7D2C: "Task_Kracko",
    0x080A7D98: "KrackoJrInit",
    0x080A7DF4: "KrackoJrEnterState",
    0x080A7E10: "KrackoJrUpdate",
    0x080A7E44: "KrackoJrState0",
    0x080A8038: "KrackoJrState0Update",
    0x080A85E4: "KrackoJrTransform",
    0x080A860C: "KrackoJrTransformUpdate",
    0x080A87C8: "KrackoInit",
    0x080A8834: "KrackoEnterState",
    0x080A8850: "KrackoUpdate",
    0x080A8878: "KrackoIntro",
    0x080A8948: "KrackoIntroUpdate",
    0x080A8970: "KrackoPickMove",
    0x080A8B44: "KrackoPickMoveUpdate",
    0x080A8B6C: "KrackoWait",
    0x080A8BCC: "KrackoWaitUpdate",
    0x080A8BF4: "KrackoCross",
    0x080A8C84: "KrackoCrossUpdate",
    0x080A8D1C: "KrackoLightningSweep",
    0x080A8F18: "KrackoLightningSweepUpdate",
    0x080A8F40: "KrackoSummon",
    0x080A8FB4: "KrackoSummonUpdate",
    0x080A8FDC: "KrackoSwoop",
    0x080A9304: "KrackoSwoopUpdate",
    0x080A932C: "KrackoJrClampToView",
    0x080A9738: "KrackoLookAtNearestPlayer",
    0x080A9760: "KrackoReactToDamage",
    0x080A9794: "KrackoHitStunUpdate",
    0x080A97D8: "KrackoReactToDefeat",
    0x080A9814: "KrackoDropStarRodPiece",
    0x080A983C: "Task_KrackoJrOrbs",
    0x080A9910: "KrackoJrOrbsUpdate",
    0x080A99A0: "Task_KrackoCloud",
    0x080A9C28: "Task_KrackoLightningTop",
    0x080A9CF0: "Task_KrackoLightningMiddle",
    0x080A9DA4: "Task_KrackoLightningBottom",
    0x080A9E14: "KrackoLightningUpdate",
    0x080A9E88: "KrackoDefeatSweepFilter",
    0x080A9ED8: "NightmareWizardDrawStreamedFrameNearView",
    0x080A9EF4: "NightmareWizardDrawStreamedFrame",
    0x080AA16C: "NightmareWizardTornadoDrawStreamedFrameNearView",
    0x080AA188: "NightmareWizardTornadoDrawStreamedFrame",
    0x080AA288: "Task_NightmareWizard",
    0x080AA338: "NightmareWizardInit",
    0x080AA38C: "NightmareWizardEnterState",
    0x080AA3A8: "NightmareWizardUpdate",
    0x080AA47C: "NightmareWizardState0",
    0x080AA52C: "NightmareWizardState0Update",
    0x080AA560: "NightmareWizardState1",
    0x080AA62C: "NightmareWizardState1Update",
    0x080AA67C: "NightmareWizardState2",
    0x080AA6A8: "NightmareWizardState2Update",
    0x080AA6D0: "NightmareWizardState3",
    0x080AA71C: "NightmareWizardState3Update",
    0x080AA744: "NightmareWizardState4",
    0x080AA970: "NightmareWizardState4Update",
    0x080AA998: "NightmareWizardPoint",
    0x080AAB4C: "NightmareWizardPointUpdate",
    0x080AAB80: "NightmareWizardOpenPalm",
    0x080AAD64: "NightmareWizardOpenPalmUpdate",
    0x080AAD98: "NightmareWizardOpenCloak",
    0x080AAF38: "NightmareWizardOpenCloakUpdate",
    0x080AAF6C: "NightmareWizardTwist",
    0x080AB158: "NightmareWizardTwistUpdate",
    0x080AB1A8: "NightmareWizardSwoop",
    0x080AB394: "NightmareWizardSwoopUpdate",
    0x080AB3C8: "NightmareWizardState10",
    0x080AB418: "NightmareWizardState10Update",
    0x080AB440: "NightmareWizardHurt",
    0x080AB46C: "NightmareWizardHurtUpdate",
    0x080AB4A0: "NightmareWizardMoveToNextSpot",
    0x080AB4D0: "NightmareWizardMoveToSpot",
    0x080AB570: "NightmareWizardSteerTowardNearestPlayer",
    0x080AB670: "NightmareWizardAppear",
    0x080AB728: "NightmareWizardVanish",
    0x080AB854: "NightmareWizardReactToDamage",
    0x080AB8A8: "NightmareWizardReactToDefeat",
    0x080AB93C: "NightmareWizardDefeat",
    0x080ABCB4: "NightmareWizardDefeatUpdate",
    0x080ABCD0: "NightmareWizardScatterStarsAt",
    0x080ABD04: "Task_NightmareWizardDefeatFlash",
    0x080ABE38: "Task_NightmareWizardPalm",
    0x080ABE7C: "NightmareWizardPalmFollowBody",
    0x080ABF10: "Task_NightmareWizardPalmTornado",
    0x080ABF94: "NightmareWizardPalmTornadoUpdate",
    0x080AC020: "Task_NightmareWizardPointingHand",
    0x080AC08C: "NightmareWizardPointingHandFollowBody",
    0x080AC124: "Task_NightmareWizardPointTornado",
    0x080AC1F0: "NightmareWizardPointTornadoUpdate",
    0x080AC27C: "Task_NightmareWizardCloakHands",
    0x080AC30C: "NightmareWizardCloakPartFollowBody",
    0x080AC3A4: "Task_NightmareWizardPendant",
    0x080AC410: "Task_NightmareWizardCloakTornado",
    0x080AC47C: "NightmareWizardCloakTornadoUpdate",
    0x080AC510: "Task_NightmareWizardHitBox",
    0x080AC530: "NightmareWizardHitBoxUpdate",
    0x080AC684: "Task_MetaKnightSword",
    0x080ACA90: "Task_KrackoStarman",
    0x080ACAF0: "KrackoStarmanUpdate",
    0x080ACB20: "KrackoStarmanState0",
    0x080ACC18: "KrackoStarmanCheckParent",
    0x080ACC9C: "Task_NightmareWizardStar",
    0x080ACCF0: "NightmareWizardStarUpdate",
    0x080ACD38: "NightmareWizardStarState0",
    0x080ACE60: "NightmareWizardStarState0Update",
    0x080ACEA8: "Task_PaintRoller",
    0x080ACF48: "PaintRollerEnterState",
    0x080ACF68: "PaintRollerRunToNextSpot",
    0x080ACFFC: "PaintRollerSummon",
    0x080AD0F8: "PaintRollerUpdate",
    0x080AD128: "PaintRollerReactToDamage",
    0x080AD13C: "PaintRollerReactToDefeat",
    0x080AD160: "PaintRollerDropStarRodPiece",
    0x080AD170: "PaintRollerPickNextSpot",
    0x080AD278: "PaintRollerRunMoveStep",
    0x080AD32C: "PaintRollerMoveToSpot",
    0x080AD37C: "PaintRollerHasHalfHealth",
    0x080AD3E8: "CreatePaintRollerPainting",
    0x080AD458: "PaintRollerStartHitStun",
    0x080AD47C: "PaintRollerHitStunUpdate",
    0x080AD4B8: "Task_HeavyMole",
    0x080AD650: "HeavyMoleMove",
    0x080AD710: "HeavyMoleUpdate",
    0x080AD7F0: "HeavyMoleStartNextMove",
    0x080AD9DC: "HeavyMoleSetPhaseFromHealth",
    0x080ADA20: "HeavyMolePickPattern",
    0x080ADB58: "HeavyMoleReactToDamage",
    0x080ADB90: "HeavyMoleHitStunUpdate",
    0x080ADBF0: "HeavyMoleReactToDefeat",
    0x080ADC44: "HeavyMoleExitAfterDefeat",
    0x080ADCA4: "Task_HeavyMoleMissileHatch",
    0x080ADD48: "HeavyMoleMissileHatchFollowBody",
    0x080ADDF8: "CreateHeavyMoleMissile",
    0x080ADEFC: "Task_HeavyMoleTurbines",
    0x080ADF50: "HeavyMoleTurbinesFollowBody",
    0x080AE038: "Task_HeavyMoleEye",
    0x080AE0B4: "HeavyMoleEyeFollowBody",
    0x080AE1F0: "Task_HeavyMoleSmoke",
    0x080AE380: "HeavyMoleDefeatSweepFilter",
    0x080AE3BC: "Task_NightmarePowerOrb",
    0x080AE470: "NightmarePowerOrbUpdate",
    0x080AE548: "NightmarePowerOrbHover",
    0x080AE628: "NightmarePowerOrbFigureEight",
    0x080AE79C: "NightmarePowerOrbShootFourStars",
    0x080AE7EC: "NightmarePowerOrbShoot",
    0x080AE870: "NightmarePowerOrbShootStar",
    0x080AE8E0: "NightmarePowerOrbDash",
    0x080AEEB0: "NightmarePowerOrbPickDashLane",
    0x080AEEF8: "NightmarePowerOrbFlyAway",
    0x080AEFD4: "NightmarePowerOrbStartAnim",
    0x080AEFF8: "NightmarePowerOrbTickAnim",
    0x080AF020: "NightmarePowerOrbStepAnim",
    0x080AF100: "NightmarePowerOrbGetAnimDelay",
    0x080AF1D4: "NightmarePowerOrbShowHurtFrames",
    0x080AF20C: "CreateNightmarePowerOrbStar",
    0x080AF26C: "NightmarePowerOrbReactToDamage",
    0x080AF278: "NightmarePowerOrbReactToDefeat",
    0x080AF294: "NightmarePowerOrbDefeat",
    0x080AF30C: "Task_NightmarePowerOrbStar",
    0x080AF358: "NightmarePowerOrbStarUpdate",
    0x080AF38C: "NightmarePowerOrbStarVariant0",
    0x080AF4A4: "NightmarePowerOrbStarVariant1",
    0x080AF5BC: "NightmarePowerOrbStarVariant2",
    0x080AF6C8: "NightmarePowerOrbStarVariant3",
    0x080AF844: "NightmarePowerOrbStarVariant4",
    0x080AF938: "NightmarePowerOrbStarVariant5",
    0x080AF9E4: "NightmarePowerOrbStarVariant6",
    0x080AFAA4: "NightmarePowerOrbStarVariant7",
    0x080AFB50: "NightmarePowerOrbStarVariant8",
    0x080AFC10: "NightmarePowerOrbStarVariant9",
    0x080AFCD4: "NightmarePowerOrbStarVariant10",
    0x080AFD9C: "NightmarePowerOrbStarVariant11",
    0x080AFDF0: "Task_NightmarePowerOrbStarTrail",
    0x080AFF40: "Task_NightmarePowerOrbStarTrailUp",
    0x080B0144: "Task_NightmarePowerOrbStarTrailDown",
    0x080B0338: "Task_NightmarePowerOrbStarAfterimage",
    0x080B05E8: "NightmarePowerOrbIntro",
    0x080B07D8: "NightmarePowerOrbIntroUpdate",
    0x080B0840: "Task_NightmarePowerOrbIntroScroll",
    0x080B08A0: "NightmarePowerOrbIntroScrollUpdate",
    0x080B08E4: "Task_NightmarePowerOrbStreak",
    0x080B0CC8: "Task_PaintRollerPainting",
    0x080B0DE4: "PaintRollerPaintingCar",
    0x080B0E80: "PaintRollerPaintingKirby",
    0x080B0F04: "PaintRollerPaintingWaddleDee",
    0x080B0F98: "PaintRollerPaintingMike",
    0x080B102C: "PaintRollerPaintingBaseball",
    0x080B10C8: "PaintRollerPaintingBomb",
    0x080B123C: "PaintRollerPaintingBombUpdate",
    0x080B1264: "PaintRollerPaintingCloud",
    0x080B1398: "PaintRollerPaintingCloudUpdate",
    0x080B13BC: "Task_PaintRollerLightning",
    0x080B14AC: "PaintRollerPaintingParasol",
    0x080B14FC: "PaintRollerPaintingParasolUpdate",
    0x080B1578: "PaintRollerPaintingBaseballUpdate",
    0x080B15C0: "PaintRollerPaintingPickSubject",
    0x080B1710: "Task_HeavyMoleUpperArm",
    0x080B1770: "Task_HeavyMoleLowerArm",
    0x080B17D0: "HeavyMoleUpperArmFollowBody",
    0x080B1830: "HeavyMoleLowerArmFollowBody",
    0x080B1890: "HeavyMoleLowerArmDraw",
    0x080B1910: "HeavyMoleArmUpdate",
    0x080B199C: "HeavyMoleArmCheckHits",
    0x080B1A00: "HeavyMoleArmEnterState",
    0x080B1A1C: "HeavyMoleArmState0",
    0x080B1B2C: "HeavyMoleArmState0Update",
    0x080B1C04: "HeavyMoleArmState1",
    0x080B1D2C: "HeavyMoleArmState1Update",
    0x080B1D98: "HeavyMoleArmState2",
    0x080B1E20: "HeavyMoleArmState2Update",
    0x080B1EF8: "HeavyMoleArmState3",
    0x080B1F80: "HeavyMoleArmState3Update",
    0x080B20D4: "HeavyMoleArmState5",
    0x080B214C: "HeavyMoleArmState5Update",
    0x080B21A0: "HeavyMoleArmState4",
    0x080B2228: "HeavyMoleArmState4Update",
    0x080B22F8: "Task_HeavyMoleYellowMissile",
    0x080B2418: "Task_HeavyMoleRedMissile",
    0x080B2538: "HeavyMoleMissileUpdate",
    0x080B2550: "WhispyWoodsReactToDefeat",
    0x080B2574: "WhispyWoodsReactToDamage",
    0x080B25E8: "WhispyWoodsHitStunUpdate",
    0x080B2664: "WhispyWoodsPickAttack",
    0x080B2768: "CreateWhispyWoodsAirPuff",
    0x080B27B0: "CreateWhispyWoodsApple",
    0x080B2A00: "Task_WhispyWoods",
    0x080B2A74: "WhispyWoodsInit",
    0x080B2AA4: "WhispyWoodsUpdate",
    0x080B2AC8: "WhispyWoodsEnterState",
    0x080B2AE4: "WhispyWoodsWait",
    0x080B2B28: "WhispyWoodsWaitUpdate",
    0x080B2B40: "WhispyWoodsBlowTwoPuffs",
    0x080B2BF0: "WhispyWoodsBlowTwoPuffsUpdate",
    0x080B2C18: "WhispyWoodsBlowFourPuffs",
    0x080B2CC8: "WhispyWoodsBlowFourPuffsUpdate",
    0x080B2CF0: "WhispyWoodsDropApples",
    0x080B2D80: "WhispyWoodsDropApplesUpdate",
    0x080B2DD4: "WhispyWoodsDefeatedInit",
    0x080B2E20: "WhispyWoodsDefeatedUpdate",
    0x080B2E3C: "WhispyWoodsDefeatedEnterState",
    0x080B3050: "Task_WhispyWoodsApple",
    0x080B3090: "WhispyWoodsAppleInit",
    0x080B30C8: "WhispyWoodsAppleUpdate",
    0x080B3110: "WhispyWoodsAppleEnterState",
    0x080B312C: "WhispyWoodsAppleFall",
    0x080B319C: "WhispyWoodsAppleFallUpdate",
    0x080B31A0: "WhispyWoodsAppleState1",
    0x080B31E0: "WhispyWoodsAppleState1Update",
    0x080B3214: "WhispyWoodsAppleState2",
    0x080B3258: "WhispyWoodsAppleState2Update",
    0x080B328C: "WhispyWoodsAppleState3",
    0x080B32D0: "WhispyWoodsAppleState3Update",
    0x080B3318: "Task_WhispyWoodsAirPuff",
    0x080B3368: "WhispyWoodsAirPuffInit",
    0x080B3398: "WhispyWoodsAirPuffUpdate",
    0x080B33D8: "WhispyWoodsAirPuffState0",
    0x080B3758: "WhispyWoodsAirPuffState0Update",
    0x080B37EC: "WhispyWoodsLeavesShiftTrail",
    0x080B38F0: "WhispyWoodsLeavesFillTrail",
    0x080B3A64: "WhispyWoodsLeavesDraw",
    0x080B3C68: "Task_WhispyWoodsLeaves",
    0x080B3E30: "WhispyWoodsLeavesUpdate",
    0x080B3E54: "Task_OneUp",
    0x080B3E94: "Task_MaximTomato",
    0x080B3ED4: "Task_InvincibleCandy",
    0x080B3F14: "Task_EnergyDrink",
    0x080B404C: "PickupStartFall",
    0x080B406C: "PickupLand",
    0x080B408C: "PickupEnterWater",
    0x080B40A4: "PickupInit",
    0x080B4100: "PickupUpdate",
    0x080B4158: "PickupEnterState",
    0x080B4174: "PickupState0",
    0x080B4190: "PickupState0Update",
    0x080B4194: "PickupFall",
    0x080B41C8: "PickupFallUpdate",
    0x080B41CC: "PickupFallInWater",
    0x080B4200: "PickupFallInWaterUpdate",
    0x080B4204: "HealPlayerStep",
    0x080B4240: "PickupHeal",
    0x080B429C: "MaximTomatoHeal",
    0x080B42F8: "EnergyDrinkHeal",
    0x080B4390: "AbilityStarBounceOffFloor",
    0x080B43D4: "AbilityStarEnterWater",
    0x080B43F4: "AbilityStarBounceOffWall",
    0x080B442C: "AbilityStarHitCeiling",
    0x080B447C: "AbilityStarInit",
    0x080B44F0: "AbilityStarAdvanceFrame",
    0x080B4524: "AbilityStarCheckPlayerFar",
    0x080B460C: "TaskBounceOffCameraBounds",
    0x080B4648: "AbilityStarCheckExpire",
    0x080B469C: "Task_AbilityStar",
    0x080B4714: "AbilityStarUpdate",
    0x080B4754: "AbilityStarEnterState",
    0x080B4770: "AbilityStarState0",
    0x080B4788: "AbilityStarState0Update",
    0x080B4794: "AbilityStarSink",
    0x080B47C0: "AbilityStarSinkUpdate",
    0x080B47CC: "Task_StarRodPiece",
    0x080B480C: "DefeatAllAbilityStars",
    0x080B4878: "StarRodPieceCollect",
    0x080B48E0: "StarRodPieceInitDanceWalk",
    0x080B48F8: "StarRodPieceStartDance",
    0x080B4968: "StarRodPieceGatherPlayers",
    0x080B4A34: "StarRodPieceGatherUpdate",
    0x080B4A5C: "StarRodPieceHoverInit",
    0x080B4A8C: "StarRodPieceHoverUpdate",
    0x080B4AFC: "StarRodPieceHoverEnterState",
    0x080B4B18: "StarRodPieceHoverState0",
    0x080B4B94: "StarRodPieceHoverState0Update",
    0x080B4BB0: "StarRodPieceHoverState1",
    0x080B4BD8: "StarRodPieceHoverState1Update",
    0x080B4BE4: "StarRodPieceSlideOutInit",
    0x080B4C14: "StarRodPieceSlideOutUpdate",
    0x080B4C84: "StarRodPieceSlideOutEnterState",
    0x080B4CA0: "StarRodPieceSlideOutState0",
    0x080B4D1C: "StarRodPieceSlideOutState0Update",
    0x080B4D50: "StarRodPieceSlideOutState1",
    0x080B4DB4: "StarRodPieceSlideOutState1Update",
    0x080B4DD0: "StarRodPieceSlideOutState2",
    0x080B4DF8: "StarRodPieceSlideOutState2Update",
    0x080B4E04: "StarRodPieceVariant2",
    0x080B4E40: "InitRoomObjects",
    0x080B4EA8: "LoadRoomObjectGfx",
    0x080B5024: "SpawnRoomObjectsOnLoad",
    0x080B5338: "SpawnRoomObject",
    0x080B54A4: "ReleaseRoomObject",
    0x080B54D0: "MarkRoomObjectUsed",
    0x080B5540: "TransferRoomObject",
    0x080B55D8: "AllocObjTilesAndPalettes",
    0x080B5628: "AllocObjTiles",
    0x080B5654: "AllocObjPalettes",
    0x080B5670: "LoadRoomEnemyGfx",
    0x080B5840: "LoadRoomMidBossGfx",
    0x080B590C: "LoadRoomBossGfx",
    0x080B59D8: "LoadRoomMetaKnightsGfx",
    0x080B5BDC: "SpawnRoomEnemy",
    0x080B603C: "HBlankScrollVBlankCallback",
    0x080B60E8: "UpdateHBlankScroll",
    0x080B6A90: "UpdateRoomHBlankScroll",
    0x080B6E44: "ResetHBlankScroll",
    0x080B6E60: "StopHBlankScroll",
    0x080B6E6C: "StartHBlankScroll",
    0x080B6EA0: "StartRoomHBlankScroll",
    0x080B6ED4: "HoldHBlankScroll",
    0x080B6EEC: "SuspendHBlankScroll",
    0x080B6F04: "RestoreRoomHBlankScroll",
    0x080B6F20: "ResumeHBlankScroll",
    0x080B6F38: "InputRecorderStart",
    0x080B72BC: "InputRecorderRestoreState",
    0x080B75A4: "InputRecorderRecordFrame",
    0x080B76A8: "InputRecorderPlayFrame",
    0x080B77D4: "InputRecorderUpdate",
    0x080B7800: "InitSaveSlots",
    0x080B78E4: "SelectLatestSaveSlot",
    0x080B7918: "ReadSaveSlot",
    0x080B798C: "InitNewSaveFile",
    0x080B79B8: "CalcCompletionPercent",
    0x080B7A9C: "WriteSaveSlot",
    0x080B7AF8: "WriteSramSignature",
    0x080B7B20: "WriteNewSaveFile",
    0x080B7B7C: "SaveProgress",
    0x080B7C00: "SaveMetaKnightmareBestTime",
    0x080B7CB4: "SaveBossEnduranceBestTime",
    0x080B7D74: "EraseSaveSlot",
    0x080B7D94: "ClearSaveSlot",
    0x080B7DD0: "CalcSaveSlotChecksum",
    0x080B7DF4: "UpdateSaveSlotChecksum",
    0x080B7E14: "StoreProgressInSaveSlot",
    0x080B7F58: "StoreProgressInBothHalves",
    0x080B8070: "LoadSaveSlot",
    0x080B81A0: "ResetLevelProgress",
    0x080B8200: "ResetProgress",
    0x080B8290: "CheckNewMilestones",
    0x080B8348: "ReadInputRecording",
    0x080B8374: "WriteInputRecording",
    0x080B83A0: "WriteInputRecordingEntry",
    0x080B83B8: "CopySaveSlotToLinkSlot",
    0x080B84F0: "FillSendCmdWithSaveSlot",
    0x080B8694: "ReceiveLinkSaveSlots",
    0x080B8888: "ExchangeLinkSaveSlots",
    0x080B8918: "MergeLinkSaveSlots",
    0x080B8B2C: "MergeProgressIntoSaveSlot",
    0x080B8EA0: "PlayerLifeRequestClear",
    0x080B8EBC: "PlayerClearOwnLifeRequests",
    0x080B8EF4: "PlayerLifeRequestPickStartState",
    0x080B8F8C: "PlayerLifeRequestLoadGfx",
    0x080B8FF0: "PlayerLifeRequestOpenMenu",
    0x080B902C: "PlayerLifeRequestOpenGiverList",
    0x080B9064: "PlayerLifeRequestStartAsking",
    0x080B9090: "PlayerLifeRequestTakeLife",
    0x080B90C8: "PlayerLifeRequestFinish",
    0x080B90F8: "PlayerLifeRequestStartFail",
    0x080B9108: "PlayerLifeRequestStartNoGiver",
    0x080B9118: "PlayerLifeRequestShowGameOver",
    0x080B9140: "PlayerLifeRequestMoveMenuCursor",
    0x080B9198: "PlayerLifeRequestSelectChoice",
    0x080B91FC: "PlayerLifeRequestMoveListCursor",
    0x080B927C: "PlayerLifeRequestSelectGiver",
    0x080B9344: "PlayerLifeRequestReceiveCheckPress",
    0x080B938C: "PlayerLifeRequestFailCheckPress",
    0x080B93D8: "PlayerLifeRequestNoGiverCheckPress",
    0x080B9424: "PlayerLifeRequestCountGivers",
    0x080B94B4: "PlayerLifeRequestRefreshGiverList",
    0x080B9578: "PlayerLifeRequestCheckGiven",
    0x080B95AC: "PlayerLifeRequestCheckTimeout",
    0x080B95EC: "PlayerLifeRequestCheckGiverLives",
    0x080B9610: "PlayerLifeRequestInit",
    0x080B963C: "PlayerLifeRequestUpdate",
    0x080B9658: "PlayerLifeRequestEnterState",
    0x080B9674: "PlayerLifeRequestChoose",
    0x080B9690: "PlayerLifeRequestChooseUpdate",
    0x080B96A0: "PlayerLifeRequestPickGiver",
    0x080B96BC: "PlayerLifeRequestPickGiverUpdate",
    0x080B9710: "PlayerLifeRequestWait",
    0x080B9730: "PlayerLifeRequestWaitUpdate",
    0x080B9740: "PlayerLifeRequestReceive",
    0x080B9764: "PlayerLifeRequestReceiveUpdate",
    0x080B9770: "PlayerLifeRequestFail",
    0x080B9798: "PlayerLifeRequestFailUpdate",
    0x080B97A4: "PlayerLifeRequestNoGiver",
    0x080B97D0: "PlayerLifeRequestNoGiverUpdate",
    0x080B97DC: "PlayerLifeRequestGameOver",
    0x080B97F8: "PlayerLifeRequestGameOverUpdate",
    0x080B97FC: "PlayerLifeRequestDrawMenu",
    0x080B9878: "PlayerLifeRequestDrawListTitle",
    0x080B98C0: "PlayerLifeRequestDrawGiverList",
    0x080B9968: "PlayerLifeRequestDrawGiverIcon",
    0x080B99E8: "PlayerLifeRequestDrawLives",
    0x080B9A88: "PlayerLifeRequestDrawListCursor",
    0x080B9B08: "PlayerLifeRequestDrawAsking",
    0x080B9B98: "PlayerLifeRequestDrawBorrowed",
    0x080B9C28: "PlayerLifeRequestDrawCannotBorrow",
    0x080B9C74: "PlayerLifeRequestDrawGotNothing",
    0x080B9CC0: "PlayerLifeRequestDrawGameOver",
    0x080B9D0C: "SubGameReplay",
    0x080B9D24: "SubGameQuit",
    0x080B9D48: "SubGameInit",
    0x080B9D68: "SubGameAnyPressedAOrStart",
    0x080B9DA8: "SubGameAnyPressedB",
    0x080B9DE8: "SubGameDimAndHalt",
    0x080B9E30: "SubGameCheckEnd",
    0x080B9E50: "SubGameLoadScreen",
    0x080B9EA0: "SubGameSetDisplayLayers",
    0x080B9F34: "SubGameRunScreen",
    0x080BA118: "SubGameRunLinkFrame",
    0x080BA134: "SubGameRunFrame",
    0x080BA150: "SubGameSyncLink",
    0x080BA31C: "FreeOtherTasks",
    0x080BA354: "SubGameMain",
    0x080BA404: "Task_SubGame",
    0x080BA42C: "SubGameStartBody",
    0x080BA454: "QuickDrawInit",
    0x080BA4E0: "QuickDrawMain",
    0x080BA50C: "QuickDrawFreeze",
    0x080BA578: "CreateQuickDrawTimer",
    0x080BA5BC: "CreateQuickDrawPlayers",
    0x080BA61C: "QuickDrawSetupRound",
    0x080BA63C: "CreateQuickDrawSignal",
    0x080BA688: "QuickDrawStartTimer",
    0x080BA6B4: "QuickDrawWaitForSignal",
    0x080BA708: "QuickDrawCountPresses",
    0x080BA774: "QuickDrawIsTimeUp",
    0x080BA78C: "QuickDrawEndRoundTimeUp",
    0x080BA7B0: "QuickDrawEndRoundFalseStart",
    0x080BA7FC: "QuickDrawEndRoundWin",
    0x080BA860: "QuickDrawEndRoundTie",
    0x080BA8CC: "QuickDrawResetRound",
    0x080BA900: "CreateQuickDrawRedrawSign",
    0x080BA94C: "CreateQuickDrawSlash",
    0x080BA978: "CreateQuickDrawBurst",
    0x080BA9A4: "QuickDrawUpdateRanking",
    0x080BAA38: "QuickDrawAwardRound",
    0x080BAABC: "QuickDrawPoseTiedPlayers",
    0x080BAB08: "QuickDrawRound",
    0x080BAB68: "QuickDrawRoundUpdate",
    0x080BABB0: "QuickDrawFalseStart",
    0x080BAC5C: "QuickDrawFindMatchWinner",
    0x080BACBC: "QuickDrawDecideRound",
    0x080BACE4: "QuickDrawEnterState",
    0x080BAD00: "QuickDrawRoundWait",
    0x080BAD44: "QuickDrawRoundWaitUpdate",
    0x080BAD80: "QuickDrawRoundSignal",
    0x080BAD9C: "QuickDrawRoundSignalUpdate",
    0x080BADE4: "QuickDrawRoundTimeUp",
    0x080BAE0C: "QuickDrawRoundTimeUpUpdate",
    0x080BAE34: "QuickDrawRoundAllFalseStart",
    0x080BAE5C: "QuickDrawRoundAllFalseStartUpdate",
    0x080BAE84: "QuickDrawRoundWin",
    0x080BAEB0: "QuickDrawRoundWinUpdate",
    0x080BAEF0: "QuickDrawRoundTie",
    0x080BAF18: "QuickDrawRoundTieUpdate",
    0x080BAF40: "QuickDrawRoundNext",
    0x080BAF9C: "QuickDrawRoundNextUpdate",
    0x080BAFC8: "QuickDrawFalseStartVsCpu",
    0x080BB074: "CreateQuickDrawOpponent",
    0x080BB0D8: "QuickDrawDecideRoundVsCpu",
    0x080BB120: "QuickDrawResetRoundVsCpu",
    0x080BB174: "QuickDrawEndRoundWinVsCpu",
    0x080BB19C: "QuickDrawEndRoundLoseVsCpu",
    0x080BB1C4: "QuickDrawEndRoundTieVsCpu",
    0x080BB1EC: "QuickDrawFindMatchWinnerVsCpu",
    0x080BB23C: "QuickDrawEnterStateVsCpu",
    0x080BB258: "QuickDrawRoundWaitVsCpu",
    0x080BB29C: "QuickDrawRoundWaitVsCpuUpdate",
    0x080BB2D8: "QuickDrawRoundSignalVsCpu",
    0x080BB2F4: "QuickDrawRoundSignalVsCpuUpdate",
    0x080BB328: "QuickDrawRoundFalseStartVsCpu",
    0x080BB358: "QuickDrawRoundFalseStartVsCpuUpdate",
    0x080BB380: "QuickDrawRoundWinVsCpu",
    0x080BB3A8: "QuickDrawRoundWinVsCpuUpdate",
    0x080BB3E8: "QuickDrawRoundLoseVsCpu",
    0x080BB410: "QuickDrawRoundLoseVsCpuUpdate",
    0x080BB42C: "QuickDrawRoundTieVsCpu",
    0x080BB454: "QuickDrawRoundTieVsCpuUpdate",
    0x080BB47C: "QuickDrawRoundNextVsCpu",
    0x080BB4FC: "QuickDrawRoundNextVsCpuUpdate",
    0x080BB528: "QuickDrawSetupResults",
    0x080BB554: "QuickDrawInitContinueMenu",
    0x080BB59C: "QuickDrawPlaySfxIfPlayer0",
    0x080BB5B8: "QuickDrawContinueMenuInput",
    0x080BB63C: "QuickDrawInitLevelMenu",
    0x080BB66C: "QuickDrawLevelMenuInput",
    0x080BB718: "CreateQuickDrawBonusSign",
    0x080BB760: "QuickDrawInitBonusSteps",
    0x080BB7A0: "QuickDrawCreateNextBonus",
    0x080BB7CC: "CreateQuickDrawBonus",
    0x080BB820: "CreateQuickDrawRankLabel",
    0x080BB874: "QuickDrawPlaceRankLabel",
    0x080BB8F8: "QuickDrawCreateRankLabels",
    0x080BBAD4: "QuickDrawPickResultsSong",
    0x080BBB70: "QuickDrawSetupResultsLink",
    0x080BBC04: "QuickDrawSetupResultsVsCpu",
    0x080BBC70: "CreateQuickDrawDefeatedLabel",
    0x080BBCDC: "CreateQuickDrawBestTimeLabel",
    0x080BBD4C: "CreateQuickDrawResultsPlayer",
    0x080BBD9C: "QuickDrawResults",
    0x080BBDE4: "QuickDrawResultsUpdate",
    0x080BBE04: "QuickDrawResultsEnterState",
    0x080BBE20: "QuickDrawResultsPlaySong",
    0x080BBE58: "QuickDrawResultsPlaySongUpdate",
    0x080BBE80: "QuickDrawResultsBonusSign",
    0x080BBEDC: "QuickDrawResultsBonusSignUpdate",
    0x080BBF04: "QuickDrawResultsAwardBonuses",
    0x080BBF44: "QuickDrawResultsAwardBonusesUpdate",
    0x080BBF6C: "QuickDrawResultsRanking",
    0x080BBF9C: "QuickDrawResultsRankingUpdate",
    0x080BBFC4: "QuickDrawResultsContinueMenu",
    0x080BC008: "QuickDrawResultsContinueMenuUpdate",
    0x080BC03C: "QuickDrawResultsLevelMenu",
    0x080BC06C: "QuickDrawResultsLevelMenuUpdate",
    0x080BC0A0: "QuickDrawResultsWaitQuit",
    0x080BC0B8: "QuickDrawResultsWaitQuitUpdate",
    0x080BC0CC: "Task_QuickDrawObject",
    0x080BC0EC: "QuickDrawPlayerSlideIn",
    0x080BC168: "QuickDrawPlayerStartSlideIn",
    0x080BC1C4: "QuickDrawPlacePlayer",
    0x080BC30C: "QuickDrawPlacePlayerStrike",
    0x080BC3C8: "QuickDrawPlayerSetKnockBack",
    0x080BC4B0: "CreateQuickDrawFalseStartMark",
    0x080BC54C: "CreateQuickDrawSweatDrop",
    0x080BC5CC: "CreateQuickDrawPlayerTag",
    0x080BC680: "CreateQuickDrawWinCountLabel",
    0x080BC740: "QuickDrawSetPlayerState",
    0x080BC79C: "QuickDrawSetAllPlayersState",
    0x080BC7C8: "QuickDrawIsTaskOnScreen",
    0x080BC800: "QuickDrawPlacePlayerForResults",
    0x080BC850: "QuickDrawPlayer",
    0x080BC8A8: "QuickDrawPlayerUpdate",
    0x080BC8C4: "QuickDrawPlayerEnterState",
    0x080BC8E0: "QuickDrawPlayerArrive",
    0x080BC948: "QuickDrawPlayerArriveUpdate",
    0x080BC988: "QuickDrawPlayerReady",
    0x080BC9C0: "QuickDrawPlayerReadyUpdate",
    0x080BC9C4: "QuickDrawPlayerStrike",
    0x080BCA68: "QuickDrawPlayerStrikeUpdate",
    0x080BCA6C: "QuickDrawPlayerLose",
    0x080BCAAC: "QuickDrawPlayerLoseUpdate",
    0x080BCADC: "QuickDrawPlayerFalseStart",
    0x080BCBBC: "QuickDrawPlayerFalseStartUpdate",
    0x080BCBC0: "QuickDrawPlayerResults",
    0x080BCBF8: "QuickDrawPlayerResultsUpdate",
    0x080BCBFC: "QuickDrawLabelDraw",
    0x080BCDAC: "QuickDrawLabel",
    0x080BCDE0: "QuickDrawTimerInit",
    0x080BCE28: "QuickDrawTimerCount",
    0x080BCE74: "QuickDrawTimerDraw",
    0x080BCF60: "QuickDrawTimer",
    0x080BCF8C: "QuickDrawTimerUpdate",
    0x080BCFA4: "QuickDrawSlash",
    0x080BD06C: "QuickDrawBurst",
    0x080BD110: "QuickDrawSweatDrop",
    0x080BD188: "QuickDrawLoadOpponentGraphics",
    0x080BD1D0: "QuickDrawSetOpponentState",
    0x080BD210: "QuickDrawOpponentSlideIn",
    0x080BD25C: "QuickDrawOpponentStartSlideIn",
    0x080BD290: "QuickDrawPlaceOpponent",
    0x080BD370: "CreateQuickDrawOpponentTag",
    0x080BD3DC: "QuickDrawPlaceOpponentStrike",
    0x080BD494: "QuickDrawOpponentSetKnockBack",
    0x080BD4BC: "QuickDrawOpponent",
    0x080BD508: "QuickDrawOpponentUpdate",
    0x080BD524: "QuickDrawOpponentEnterState",
    0x080BD544: "QuickDrawOpponentArrive",
    0x080BD594: "QuickDrawOpponentArriveUpdate",
    0x080BD5D4: "QuickDrawOpponentReady",
    0x080BD60C: "QuickDrawOpponentReadyUpdate",
    0x080BD610: "QuickDrawOpponentStrike",
    0x080BD62C: "QuickDrawOpponentStrikeUpdate",
    0x080BD630: "QuickDrawOpponentLose",
    0x080BD664: "QuickDrawOpponentLoseUpdate",
    0x080BD694: "QuickDrawOpponentTie",
    0x080BD6B0: "QuickDrawOpponentTieUpdate",
    0x080BD6B4: "QuickDrawOpponentTag",
    0x080BD7EC: "QuickDrawOpponentTagUpdate",
    0x080BD7F0: "QuickDrawBonusSign",
    0x080BD828: "QuickDrawGiveBonus",
    0x080BD8AC: "QuickDrawBonus",
    0x080BD938: "QuickDrawRankLabelDraw",
    0x080BD9B0: "QuickDrawRankLabel",
    0x080BD9E8: "BombRallyInit",
    0x080BDA0C: "BombRallyMain",
    0x080BDA2C: "BombRallyRound",
    0x080BDA78: "BombRallyRoundUpdate",
    0x080BDA98: "BombRallyEnterState",
    0x080BDAB4: "BombRallyRoundPass",
    0x080BDC18: "BombRallyRoundPassUpdate",
    0x080BDC40: "BombRallyRoundNext",
    0x080BDD00: "BombRallyRoundNextUpdate",
    0x080BDD28: "BombRallyKnockOutTurnPlayer",
    0x080BDD70: "BombRallyIsMatchOver",
    0x080BDDB8: "BombRallyInitSpeed",
    0x080BDE0C: "BombRallyRestartSpeed",
    0x080BDE78: "CreateBombRallyBomb",
    0x080BDEBC: "CreateBombRallyBombSmoke",
    0x080BDF3C: "CreateBombRallyStarBurst",
    0x080BDF9C: "CreateBombRallyBurstStar",
    0x080BE010: "CreateBombRallyStartSign",
    0x080BE04C: "BombRallySeatPlayers",
    0x080BE164: "BombRallyResults",
    0x080BE1B0: "BombRallyResultsUpdate",
    0x080BE1D0: "BombRallyResultsEnterState",
    0x080BE1EC: "BombRallyResultsShow",
    0x080BE2F0: "BombRallyResultsShowUpdate",
    0x080BE318: "BombRallyResultsMenu",
    0x080BE4A0: "BombRallyResultsMenuUpdate",
    0x080BE4A4: "CreateBombRallyResultsPoses",
    0x080BE550: "CreateBombRallyLivesIcons",
    0x080BE5FC: "CreateBombRallyPlaceLabels",
    0x080BE6B4: "CreateBombRallyContinueItems",
    0x080BE714: "CreateBombRallyLevelItems",
    0x080BE774: "BombRallyAwardLives",
    0x080BE7C0: "BombRallyPlaySfxIfPlayer0",
    0x080BE7DC: "Task_BombRallyObject",
    0x080BE7FC: "BombRallyPlayer",
    0x080BE850: "BombRallyPlayerUpdate",
    0x080BE8A8: "BombRallyPlayerEnterState",
    0x080BE8C4: "BombRallyPlayerServe",
    0x080BEAE0: "BombRallyPlayerServeUpdate",
    0x080BEB08: "BombRallyPlayerReady",
    0x080BEB54: "BombRallyPlayerReadyUpdate",
    0x080BEBD4: "BombRallyPlayerTurn",
    0x080BECC0: "BombRallyPlayerTurnUpdate",
    0x080BED08: "BombRallyPlayerThrow",
    0x080BEF1C: "BombRallyPlayerThrowUpdate",
    0x080BEF64: "BombRallyPlayerFollowThrough",
    0x080BF048: "BombRallyPlayerFollowThroughUpdate",
    0x080BF0AC: "BombRallyPlayerJudgePress",
    0x080BF154: "BombRallyPlayerUpdatePose",
    0x080BF1CC: "BombRallyPlayerCpuReady",
    0x080BF214: "BombRallyPlayerCpuReadyUpdate",
    0x080BF2AC: "BombRallyPlayerCpuTurn",
    0x080BF32C: "BombRallyPlayerCpuTurnUpdate",
    0x080BF394: "BombRallyPlayerCpuThrow",
    0x080BF67C: "BombRallyPlayerCpuThrowUpdate",
    0x080BF6A4: "BombRallyPlayerCpuFollowThrough",
    0x080BF788: "BombRallyPlayerCpuFollowThroughUpdate",
    0x080BF7F0: "BombRallyPlayerBlownUp",
    0x080BF934: "BombRallyPlayerBlownUpUpdate",
    0x080BF994: "BombRallyPlayerBubblesServe",
    0x080BFB24: "BombRallyPlayerBubblesServeUpdate",
    0x080BFB4C: "BombRallyPlayerBubblesWait",
    0x080BFB94: "BombRallyPlayerBubblesWaitUpdate",
    0x080BFBF4: "BombRallyPlayerBubblesThrow",
    0x080BFD58: "BombRallyPlayerBubblesThrowUpdate",
    0x080BFD80: "BombRallyBomb",
    0x080BFDB0: "BombRallyBombUpdate",
    0x080BFDCC: "BombRallyBombEnterState",
    0x080BFDE8: "BombRallyBombStart",
    0x080BFE64: "BombRallyBombStartUpdate",
    0x080BFF28: "BombRallyBombPass",
    0x080C0074: "BombRallyBombPassUpdate",
    0x080C0388: "BombRallyBombExplode",
    0x080C0540: "BombRallyBombExplodeUpdate",
    0x080C05F0: "BombRallyBombPlaceAtSeat",
    0x080C061C: "BombRallyBombSetArcPos",
    0x080C0704: "BombRallyScrollToSeat",
    0x080C072C: "BombRallyScrollAlongPass",
    0x080C0A10: "BombRallyBombDrawShadow",
    0x080C0B18: "BombRallyPanToSeat",
    0x080C0C58: "BombRallyBombSmoke",
    0x080C0CA4: "BombRallyBombSmokeUpdate",
    0x080C0D30: "BombRallyStarBurst",
    0x080C0DE8: "BombRallyStarBurstState0",
    0x080C0E88: "BombRallyStarBurstState4",
    0x080C0F54: "BombRallyStarBurstState5",
    0x080C0FE4: "BombRallyStarBurstState6",
    0x080C1068: "BombRallyStarBurstState7",
    0x080C11CC: "BombRallyStarBurstState8",
    0x080C1260: "BombRallyStarBurstState9",
    0x080C1300: "BombRallyStarBurstState10",
    0x080C1390: "BombRallyStarBurstState11",
    0x080C1424: "BombRallyStarBurstState2",
    0x080C14B8: "BombRallyStarBurstState12",
    0x080C1558: "BombRallyStarBurstState13",
    0x080C1608: "BombRallyStarBurstState14",
    0x080C168C: "BombRallyStarBurstState15",
    0x080C173C: "BombRallyStartSign",
    0x080C17AC: "BombRallyStartSignUpdate",
    0x080C17B0: "BombRallyResultsPlayer",
    0x080C1804: "BombRallyResultsPlayerUpdate",
    0x080C1820: "BombRallyResultsPlayerEnterState",
    0x080C183C: "BombRallyResultsPlayerPose",
    0x080C18C4: "BombRallyResultsPlayerPoseUpdate",
    0x080C18C8: "BombRallyResultsPlayerLives",
    0x080C1950: "BombRallyResultsPlayerLivesUpdate",
    0x080C1AB8: "BombRallyResultsPlayerPlace",
    0x080C1B2C: "BombRallyResultsPlayerPlaceUpdate",
    0x080C1B30: "BombRallyMenuItem",
    0x080C1B78: "BombRallyMenuItemUpdate",
    0x080C1B94: "BombRallyMenuItemContinue",
    0x080C1BE8: "BombRallyMenuItemContinueUpdate",
    0x080C1CEC: "BombRallyMenuItemLevel",
    0x080C1D84: "BombRallyMenuItemLevelUpdate",
    0x080C1EBC: "BombRallyShakeScreen",
    0x080C1F9C: "AirGrindInit",
    0x080C1FDC: "AirGrindMain",
    0x080C1FFC: "CreateAirGrindRacers",
    0x080C2038: "CreateAirGrindScenery",
    0x080C2078: "CreateAirGrindEffect",
    0x080C20B4: "AirGrindSetupRace",
    0x080C21B0: "AirGrindRace",
    0x080C241C: "AirGrindRaceUpdate",
    0x080C243C: "AirGrindResults",
    0x080C25C4: "AirGrindResultsDraw",
    0x080C2740: "AirGrindResultsStep",
    0x080C2B8C: "AirGrindResultsUpdate",
    0x080C2BA8: "AirGrindResultsDrawCursor",
    0x080C2CCC: "AirGrindResultsSetCursorBlend",
    0x080C2D38: "AirGrindBuildSky",
    0x080C2FB8: "AirGrindSkyVBlankCallback",
    0x080C2FF8: "Task_AirGrindObject",
    0x080C3018: "AirGrindRacer",
    0x080C3318: "AirGrindCpuRollTarget",
    0x080C33A0: "AirGrindCpuHoldsA",
    0x080C34AC: "AirGrindRacerUpdate",
    0x080C3648: "AirGrindRacerSlowDown",
    0x080C3670: "AirGrindScrollCourseTo",
    0x080C3698: "AirGrindRacerTryBoost",
    0x080C37B8: "AirGrindUpdateEngineSound",
    0x080C383C: "AirGrindRacerUpdateDepth",
    0x080C38C8: "AirGrindRacerRaceStep",
    0x080C3D58: "AirGrindRacerIdleStep",
    0x080C3E18: "AirGrindRacerUpdateScreenPos",
    0x080C3EFC: "AirGrindRacerRaceUpdate",
    0x080C3F20: "AirGrindRacerIdleUpdate",
    0x080C3F44: "AirGrindEffect",
    0x080C42DC: "AirGrindEffectFollowRacer",
    0x080C43E8: "AirGrindPenaltyScatterEffectUpdate",
    0x080C44F0: "AirGrindPressEffectUpdate",
    0x080C4568: "AirGrindReleaseEffectUpdate",
    0x080C45D4: "AirGrindBoostRatingUpdate",
    0x080C45FC: "AirGrindPenaltyEffectUpdate",
    0x080C4630: "AirGrindDrawSceneryObject",
    0x080C4664: "AirGrindRollSceneryObject",
    0x080C46EC: "AirGrindScenery",
    0x080C4790: "AirGrindSceneryUpdate",
    0x080C4818: "AirGrindCourseSignUpdate",
    0x080C4860: "AirGrindShowCourseSign",
    0x080C4890: "AirGrindStepPaletteFades",
    0x080C495C: "AirGrindStopAllPaletteFades",
    0x080C4974: "AirGrindStartPaletteFade",
    0x080C4A20: "AirGrindStopPaletteFade",
    0x080C4A48: "AirGrindSetDigitPalette",
    0x080C4A5C: "AirGrindDrawDigit",
    0x080C4A94: "AirGrindDrawSymbol",
    0x080C4AC4: "AirGrindDrawTime",
    0x080C4B64: "AirGrindDrawNumber",
    0x080C4BEC: "AirGrindDrawRatio",
    0x080C4C30: "AirGrindDrawRacerSprite",
    0x080C4C78: "AirGrindSeedRandom",
    0x080C4CA4: "AirGrindRandom",
    0x080C4CD4: "AirGrindRandomRange",
    0x080C4D08: "AirGrindRacerDraw",
    0x080C4E10: "AirGrindEffectDrawOrFree",
    0x080C4EA8: "AirGrindRacerMove",
    0x080C4F60: "AirGrindScaleSprite",
    0x080C51C0: "AirGrindClearScript",
    0x080C51D4: "AirGrindStepScript",
    0x080C523C: "AirGrindStartScript",
    0x080C5284: "AirGrindSin",
    0x080C52C4: "AirGrindCalcLanePoint",
    0x080C54A4: "AirGrindFindLaneSegment",
    0x080C553C: "AirGrindFindSegmentEnd",
    0x080C5580: "AirGrindCountSegmentBits",
    0x080C55D8: "AirGrindCourseToLanePos",
    0x080C5628: "AirGrindLaneToCoursePos",
    0x080C5678: "AirGrindLayOutCourse",
    0x080C59D8: "AirGrindBuildCourse",
    0x080C5B84: "AirGrindDrawCourse",
    0x080C623C: "AirGrindGetDepthScale",
    0x080C6260: "EndingMain",
    0x080C62F0: "EndingEpilogueScene",
    0x080C6354: "CreateEndingEpilogue",
    0x080C6388: "EndingStarRodReturnScene",
    0x080C63EC: "CreateEndingStarRodReturn",
    0x080C6420: "FinalResultsScreen",
    0x080C6600: "DrawLargeClockScreen",
    0x080C6750: "ShowMilestonePicture",
    0x080C680C: "ShowMilestonePictureForMode",
    0x080C68B0: "DrawScoreToBgMap",
    0x080C6AB4: "DrawClockToBgMap",
    0x080C6C3C: "CopyToBgMap",
    0x080C6C64: "Task_EndingEpilogue",
    0x080C6CA0: "EndingEpilogueLoadGraphics",
    0x080C6D38: "CreateEndingEpilogueObjects",
    0x080C6D84: "EndingEpilogueWarpStar",
    0x080C769C: "EndingEpilogueWarpStarDraw",
    0x080C77CC: "CreateEndingEpilogueWarpStarEffects",
    0x080C7810: "EndingEpilogueWarpStarEffect",
    0x080C7CC0: "EndingEpilogueTrailStar",
    0x080C7E4C: "EndingEpilogueKirby",
    0x080C8298: "EndingEpilogueKirbyDraw",
    0x080C8468: "CreateEndingEpilogueStarRod",
    0x080C8498: "EndingEpilogueStarRod",
    0x080C85D8: "EndingEpilogueKingDedede",
    0x080C8778: "EndingEpilogueKingDededeDraw",
    0x080C88F0: "EndingEpilogueExplosion",
    0x080C8924: "CreateEndingEpilogueExplosionSprites",
    0x080C8958: "EndingEpilogueExplosionSprite",
    0x080C8CD4: "EndingEpilogueCamera",
    0x080C8E88: "EndingEpilogueCameraUpdate",
    0x080C8EA8: "EndingEpilogueStoryText",
    0x080C9004: "Task_EndingStarRodReturn",
    0x080C9040: "EndingStarRodReturnLoadGraphics",
    0x080C90C8: "CreateEndingStarRodReturnObjects",
    0x080C9114: "EndingStarRodReturnStarRod",
    0x080C9418: "EndingStarRodReturnStarRodDraw",
    0x080C94CC: "EndingStarRodReturnWarpStar",
    0x080C972C: "EndingStarRodReturnWarpStarUpdate",
    0x080C97A0: "EndingStarRodReturnWarpStarDraw",
    0x080C9884: "EndingStarRodReturnTrailStar",
    0x080C98D8: "EndingStarRodReturnKirby",
    0x080C9974: "EndingStarRodReturnKirbyDraw",
    0x080C9D10: "EndingStarRodReturnConvergingStars",
    0x080C9E8C: "EndingStarRodReturnBurstStar",
    0x080CA344: "EndingStarRodReturnFallingStar",
    0x080CA570: "EndingStarRodReturnFallingStarDriftFast",
    0x080CA640: "EndingStarRodReturnFallingStarDriftSlow",
    0x080CA71C: "EndingStarRodReturnFountainJet",
    0x080CAA3C: "BootLogoInitObjects",
    0x080CAAB8: "BootLogoUpdateObjects",
    0x080CACF0: "GameOverMain",
    0x080CAD8C: "GameOverScreen",
    0x080CAEEC: "GameOverMetaKnightmareScreen",
    0x080CB030: "GameOverShowClock",
    0x080CB058: "GameOverBossEnduranceScreen",
    0x080CB0E8: "GameOverMoveCursor",
    0x080CB108: "GameOverIsUpDownPressed",
    0x080CB12C: "GameOverCheckConfirm",
    0x080CB178: "GameOverCheckTimeout",
    0x080CB1D8: "GameOverCheckChoice",
    0x080CB21C: "GameOverLoadGraphics",
    0x080CB2B0: "GameOverResetWait",
    0x080CB2CC: "CreateGameOverObjects",
    0x080CB354: "Task_GameOverSprite",
    0x080CB3A8: "Task_GameOverCursor",
    0x080CB418: "Task_GameOverPalette",
    0x080CB4F4: "Task_HalveScore",
    0x080CB588: "Task_GameOverObject",
    0x080CB5C4: "GameOverPlayer",
    0x080CB610: "GameOverPlayerUpdate",
    0x080CB62C: "GameOverPlayerEnterState",
    0x080CB64C: "GameOverPlayerWait",
    0x080CB6D8: "GameOverPlayerWaitUpdate",
    0x080CB70C: "GameOverPlayerContinue",
    0x080CBABC: "GameOverPlayerContinueUpdate",
    0x080CBAC0: "GameOverPlayerGiveUp",
    0x080CBEA4: "GameOverPlayerGiveUpUpdate",
    0x080CBED4: "GameOverChoice",
    0x080CBF18: "GameOverChoiceUpdate",
    0x080CBF34: "CreateGameOverObject",
    0x080CBF68: "GameOverPlayerShowChoice",
    0x080CBFC8: "GameOverChoiceEnterState",
    0x080CBFE4: "GameOverChoiceWait",
    0x080CC024: "GameOverChoiceWaitUpdate",
    0x080CC0A4: "GameOverChoiceMove",
    0x080CC14C: "GameOverChoiceMoveUpdate",
    0x080CC180: "GameOverChoiceContinueStart",
    0x080CC2B8: "GameOverChoiceContinueStartUpdate",
    0x080CC2E0: "GameOverChoiceContinueEnd",
    0x080CC5D4: "GameOverChoiceContinueEndUpdate",
    0x080CC608: "GameOverChoiceGiveUpStart",
    0x080CC740: "GameOverChoiceGiveUpStartUpdate",
    0x080CC768: "GameOverChoiceGiveUpEnd",
    0x080CCD10: "GameOverChoiceGiveUpEndUpdate",
    0x080CD2F8: "GameOverKnockedOutPlayers",
    0x080CD330: "CreditsMain",
    0x080CD674: "CreditsLoadScene",
    0x080CD70C: "CreditsInitText",
    0x080CD75C: "CreditsStreamText",
    0x080CD828: "CreditsScrollText",
    # ---- end of tools/rename.py names ----
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
    # 0x0801ECBA is not a function either (lesson 4.39): it is the shared
    # epilogue of the 2904-byte sub_0801e178 (`add sp, #12; pop {r3-r5};
    # mov r8-sl; pop {r4-r7}; pop {r0}; bx r0`), which the body reaches by
    # falling through from the `strb` at 0x0801ECB8, by eleven `b.n` and by
    # the long `bl` jump at 0x0801E470 (a Thumb `b.n` cannot reach it).  The
    # pool word behind it (0x0801ECCC, gUnk_03005530) is loaded by the `ldr`
    # at 0x0801ECB0.  sub_0801e178 really runs 0x0801E178-0x0801ECD0 (0xB58).
    0x0801ECBA,
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
    # --- M06 (issue #84), one dead export the reachability sweep found ---
    0x08020698,  # an empty `bx lr` stub (lesson 4.34) after sub_0801ff84's
                 # own `pop {r0}; bx r0` at 0x08020694 and its alignment pad;
                 # no ROM word or `bl` references it.  sub_0801ff84 really
                 # runs 0x0801FF84-0x08020698 (0x714, was 0x718).
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
    0x080CE51C,  # UnusedDummyFunc (empty, after MidiKeyToFreq's pool)
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
