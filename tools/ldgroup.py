#!/usr/bin/env python3
"""ldgroup.py - list a run of consecutive segments as INPUT sections of one
linker OUTPUT section (issue #36 phase 2 run 3, docs/data.md 5.2).

Usage:
    python3 tools/ldgroup.py <group> <first> <last> [--c-object OBJ] [--write]
    python3 tools/ldgroup.py --ungroup <group> [--write]

    group   the output section's name (snake_case); it may be <first>'s name.
    first, last
            the first and last segments.txt rows of the run (consecutive rows,
            each with its own block in linker.ld, as carve_data.py and
            resegment.py write them).
    --c-object OBJ
            the object that defines the run's c_data rows in named sections:
            a c_data row <row> is then read from OBJ(.<row>) instead of
            build/src/data/<row>.o(.rodata) (src/data/actor_records.c places
            each run of records with __attribute__((section(".<row>")))).

Why: tools/carve_data.py gives every carve its own output section, and a
carve inside a data segment also splits the segment, so N carves in one
segment add 2N output sections.  One output section per data zone keeps the
section count where it was: the rows stay (segments.txt, split.py's data
files and the c_data ranges are unchanged), only linker.ld lists them in
order inside one block.  Each row keeps its matching-mode address as an
assertion on its first symbol (split.py labels every data file's start with
the segment name; a c_data row starts with its first data_symbols label).

--ungroup writes the group's rows back as one block each (the form
carve_data.py, resegment.py and carve.py edit); a c_data row keeps reading
its named section.  So a later carve or re-partition inside a grouped zone
is: ldgroup.py --ungroup, the tool, ldgroup.py again.

Without --write this prints the diff.  Afterwards: make clean && make compare.
"""
import difflib
import json
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ldblocks  # noqa: E402

SEGMENTS = 'docs/analysis/segments.txt'
CONFIG = 'tools/split_config.json'
LINKER = 'linker.ld'


def die(msg):
    sys.exit('ldgroup: error: ' + msg)


def ungroup(group, write):
    rows = {}
    for ln in open(SEGMENTS):
        m = re.match(r'(0x[0-9A-Fa-f]+)\s+(0x[0-9A-Fa-f]+)\s+(\S+)\s+(\S+)', ln)
        if m:
            rows[m.group(4)] = (int(m.group(1), 16), m.group(3))
    ld = open(LINKER).read()
    m = re.search(r'(?:[ \t]*/\*([^\n]*?)\s*\(ldgroup\.py: \d+ rows in one output section\) \*/[ \t]*\n)?'
                  r'[ \t]*\.%s[ \t]*:[ \t]*\{([^}]*)\}[ \t]*>[ \t]*ROM'
                  r'(?:[ \t]*\n[ \t]*ASSERT\([^\n]*\))*' % re.escape(group), ld)
    if not m:
        die('group .%s not found' % group)
    comment = (m.group(1) or '').strip()
    blocks = []
    for ln in m.group(2).strip().splitlines():
        ln = ln.strip()
        d = re.fullmatch(r'KEEP\(\*\(\.(\w+)\)\) KEEP\(\*\(\.\1\.tail\)\)', ln)
        c = re.fullmatch(r'(\S+)\(\.(\w+)\)', ln)
        if d:
            name = d.group(1)
            text = ldblocks.data_block(name, rows[name][0])
        elif c:
            name = c.group(2)
            text = ldblocks.block(name, rows[name][0], ln)
        else:
            die('unexpected input line in .%s: %r' % (group, ln))
        if not blocks and comment:
            text = '    /* %s */\n' % comment + text
        blocks.append(text)
    new = ld[:m.start()] + '\n\n'.join(blocks) + ld[m.end():]
    print(''.join(difflib.unified_diff(ld.splitlines(True), new.splitlines(True),
                                       'a/' + LINKER, 'b/' + LINKER))[:3000])
    if write:
        open(LINKER, 'w').write(new)
        print('APPLIED: .%s split into %d blocks' % (group, len(blocks)))


def main():
    if '--ungroup' in sys.argv[1:]:
        rest = [a for a in sys.argv[1:] if not a.startswith('--')]
        if len(rest) != 1:
            die('usage: ldgroup.py --ungroup <group> [--write]')
        return ungroup(rest[0], '--write' in sys.argv[1:])
    args = [a for a in sys.argv[1:] if not a.startswith('--')]
    write = '--write' in sys.argv[1:]
    cobj = None
    for i, a in enumerate(sys.argv[1:]):
        if a == '--c-object':
            cobj = sys.argv[i + 2]
    if cobj:
        args.remove(cobj)
    if len(args) != 3:
        die('usage: ldgroup.py <group> <first> <last> [--c-object OBJ] [--write]')
    group, first, last = args
    if not re.fullmatch(r'[a-z][a-z0-9_]*', group):
        die('group must be snake_case')
    rows = []
    for ln in open(SEGMENTS):
        m = re.match(r'(0x[0-9A-Fa-f]+)\s+(0x[0-9A-Fa-f]+)\s+(\S+)\s+(\S+)', ln)
        if m:
            rows.append((int(m.group(1), 16), int(m.group(2), 16), m.group(3), m.group(4)))
    names = [r[3] for r in rows]
    if first not in names or last not in names:
        die('unknown row')
    i0, i1 = names.index(first), names.index(last)
    run = rows[i0:i1 + 1]
    if len(run) < 2:
        die('a group needs two rows or more')
    if group != first and group in names:
        die('group name %r is another row' % group)
    for s, e, k, n in run:
        if k not in ('data', 'c_data'):
            die('row %s is %s' % (n, k))
    cfg = json.load(open(CONFIG))
    ds = dict((int(a, 16), n) for a, n in cfg['data_symbols'].items())

    ld = open(LINKER).read()
    spans = []
    bodies = []
    for s, e, k, n in run:
        m = ldblocks.block_re(n, comment=True).search(ld)
        if not m:
            die('linker.ld block .%s not found (already grouped?)' % n)
        spans.append((m.start(), m.end()))
        b = re.search(r'\{([^}]*)\}', m.group(0)).group(1).strip()
        if k == 'c_data' and cobj:
            want = 'build/src/data/%s.o(.rodata)' % n
            if b not in (want, '%s(.%s)' % (cobj, n)):
                die('c_data block .%s reads %r, not %r' % (n, b, want))
            b = '%s(.%s)' % (cobj, n)
        bodies.append(b)
    for (a0, a1), (b0, _b1) in zip(spans, spans[1:]):
        if ld[a1:b0].strip():
            die('the blocks of the run are not consecutive in linker.ld')
    cm = re.match(r'[ \t]*/\*([^\n]*)\*/', ld[spans[0][0]:spans[0][1]])
    comment = cm.group(1).strip() if cm else first
    lines = ['    /* %s (ldgroup.py: %d rows in one output section) */' % (comment, len(run)),
             '    .%s : {' % group]
    for (s, e, k, n), b in zip(run, bodies):
        lines.append('        %s' % b)
    lines.append('    } > ROM')
    lines.append('    ASSERT(!MATCHING || ADDR(.%s) == 0x%08X, "MATCHING: .%s moved")'
                 % (group, run[0][0], group))
    for s, e, k, n in run[1:]:
        sym = n if k == 'data' else ds.get(s)
        if not sym:
            die('c_data row %s has no data_symbols label at its start' % n)
        lines.append('    ASSERT(!MATCHING || %s == 0x%08X, "MATCHING: %s moved")'
                     % (sym, s, n))
    new = ld[:spans[0][0]] + '\n'.join(lines) + ld[spans[-1][1]:]
    print(''.join(difflib.unified_diff(ld.splitlines(True), new.splitlines(True),
                                       'a/' + LINKER, 'b/' + LINKER))[:4000])
    if write:
        open(LINKER, 'w').write(new)
        print('APPLIED: %d rows in .%s' % (len(run), group))


if __name__ == '__main__':
    main()
