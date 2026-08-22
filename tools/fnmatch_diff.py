#!/usr/bin/env python3
"""Instruction-level diff of two Thumb binaries mapped at the same VMA,
with pc-relative `ldr rN, [pc, #off]` operands replaced by the pool WORD
VALUE they load.  Literal pools shift layout for every source tweak; raw
objdump diffs then disagree on every ldr encoding even when the loaded
values are identical.  Folding the loads to their values leaves only the
real differences (used by tools/fnmatch.sh; from the issue #28 workflow).

Usage: fnmatch_diff.py <rom.bin> <candidate.bin> <base-vma>
"""
import difflib
import re
import struct
import subprocess
import sys


def disassemble(path, base, data):
    out = subprocess.run(
        ['arm-none-eabi-objdump', '-D', '-bbinary', '-marmv4t',
         '-Mforce-thumb', '--adjust-vma=0x%X' % base, path],
        capture_output=True, text=True, check=True).stdout
    lines = []
    for m in re.finditer(r'^\s*([0-9a-f]+):\s*([0-9a-f ]+?)\s+(\S+)\s*(.*)$',
                         out, re.M):
        addr = int(m.group(1), 16)
        mnem, ops = m.group(3), m.group(4)
        lm = re.match(r'(\w+), \[pc, #(\d+)\]', ops)
        if mnem.startswith('ldr') and lm:
            pool = ((addr + 4) & ~3) + int(lm.group(2))
            off = pool - base
            if 0 <= off and off + 4 <= len(data):
                val = struct.unpack_from('<I', data, off)[0]
                ops = '%s, =0x%X' % (lm.group(1), val)
        lines.append((addr, '%s\t%s' % (mnem, ops)))
    return lines


def main():
    if len(sys.argv) != 4:
        sys.exit(__doc__)
    rom_path, cand_path, base = sys.argv[1], sys.argv[2], int(sys.argv[3], 0)
    rom = disassemble(rom_path, base, open(rom_path, 'rb').read())
    cand = disassemble(cand_path, base, open(cand_path, 'rb').read())
    sm = difflib.SequenceMatcher(None, [t for _, t in rom],
                                 [t for _, t in cand], autojunk=False)
    clean = True
    for tag, i1, i2, j1, j2 in sm.get_opcodes():
        if tag == 'equal':
            continue
        clean = False
        print('== %s  rom@%08x  cand@%08x' % (
            tag,
            rom[i1][0] if i1 < len(rom) else 0,
            cand[j1][0] if j1 < len(cand) else 0))
        for k in range(i1, min(i2, i1 + 8)):
            print('  rom : %08x  %s' % rom[k])
        for k in range(j1, min(j2, j1 + 8)):
            print('  cand: %08x  %s' % cand[k])
    if clean:
        print('(instruction streams identical after pool folding — '
              'differences are pool word values or data)')


if __name__ == '__main__':
    main()
