#!/usr/bin/env python3
"""carve.py — carve a ROM range out of a split asm segment so a C module
can own it (the mechanical half of landing a decompiled function, i.e.
what issue #28 did by hand for AgbInit).

Usage:
    python3 tools/carve.py <start> <end> <name> [--write]

    start   first VMA of the C module (must be a function entry in
            docs/analysis/symbols.csv, or the segment start; even).
    end     first VMA NOT in the module (a function entry or the segment
            end; even).  Include trailing literal pools: a ~1KB function's
            pool lives AFTER its epilogue behind a pool-skip branch
            (lessons 3.6/4.12) — the range must cover it or the next
            build will misplace every later byte.
    name    new segment/section/source name (snake_case); the build will
            expect src/<name>.c to provide the bytes.

Without --write this is a DRY RUN: it prints the exact edits as unified
diffs.  With --write it rewrites:

    docs/analysis/segments.txt   (segment split into pre / c_code / post)
    tools/split_config.json      (segments list + external_defined for
                                  every DB function inside the range)
    linker.ld                    (section pinned to build/src/<name>.o)

It does NOT create src/<name>.c and does NOT delete asm — `make split`
regenerates the surrounding chunks from the new boundaries.  Afterwards:

    1. put the (already fnmatch-verified) C in src/<name>.c
    2. add a Makefile per-file override if the zone is not the default
       recipe (agbcc -O2 -mthumb-interwork)
    3. make symbols && make split
    4. make clean && make compare      # must print: knidl.gba: OK

Verify the candidate C with tools/fnmatch.sh BEFORE carving; carve with a
mismatching module leaves the tree red until the C is fixed.
"""
import csv
import difflib
import json
import re
import sys

SEGMENTS = 'docs/analysis/segments.txt'
CONFIG = 'tools/split_config.json'
LINKER = 'linker.ld'
SYMBOLS = 'docs/analysis/symbols.csv'


def die(msg):
    sys.exit('carve: error: ' + msg)


def load_segments():
    rows = []
    for ln in open(SEGMENTS):
        m = re.match(r'(0x[0-9A-Fa-f]+)\s+(0x[0-9A-Fa-f]+)\s+(\S+)\s+(\S+)'
                     r'(\s+\[split\])?(\s*#.*)?$', ln.strip())
        if m:
            rows.append({
                'start': int(m.group(1), 16), 'end': int(m.group(2), 16),
                'kind': m.group(3), 'name': m.group(4),
                'split': bool(m.group(5)), 'line': ln.rstrip('\n'),
            })
    return rows


def seg_line(start, end, kind, name, split, comment=''):
    tail = ' [split]' if split else ''
    if comment:
        tail += ' # ' + comment
    return '0x%08X 0x%08X %s %s%s' % (start, end, kind, name, tail)


def main():
    args = [a for a in sys.argv[1:] if a != '--write']
    write = '--write' in sys.argv[1:]
    if len(args) != 3:
        die('usage: carve.py <start> <end> <name> [--write]\n' + __doc__)
    start, end = int(args[0], 0), int(args[1], 0)
    name = args[2]

    if not re.fullmatch(r'[a-z][a-z0-9_]*', name):
        die('name must be snake_case: %r' % name)
    if start % 2 or end % 2:
        die('start/end must be even (lesson 4.14: ld pads odd-start '
            'input sections and shifts everything after them)')
    if end <= start:
        die('end must be > start')

    segs = load_segments()
    if any(s['name'] == name for s in segs):
        die('segment name %r already exists' % name)
    seg = next((s for s in segs
                if s['start'] <= start and end <= s['end']), None)
    if seg is None:
        die('[0x%08X,0x%08X) does not sit inside a single segment — '
            'cross-segment carves (boundary corrections like #28) stay '
            'manual' % (start, end))
    if not seg['split'] or seg['kind'] not in ('thumb_code', 'arm_code'):
        die('segment %r is %s%s, not split code' %
            (seg['name'], seg['kind'], ' [split]' if seg['split'] else ''))

    funcs = []
    for row in csv.reader(open(SYMBOLS)):
        if row and row[0].startswith('0x'):
            a = int(row[0], 16)
            if seg['start'] <= a < seg['end']:
                funcs.append((a, row[4]))
    funcs.sort()
    entries = {a for a, _ in funcs}
    if start != seg['start'] and start not in entries:
        die('start 0x%08X is not a function entry in %s (nor the segment '
            'start 0x%08X)' % (start, SYMBOLS, seg['start']))
    if end != seg['end'] and end not in entries:
        die('end 0x%08X is not a function entry in %s (nor the segment '
            'end 0x%08X) — remember trailing pools belong to the range'
            % (end, SYMBOLS, seg['end']))
    inside = [(a, n) for a, n in funcs if start <= a < end]
    if not inside:
        die('no DB functions inside the range?')

    pre = (seg['start'], start) if start > seg['start'] else None
    post = (end, seg['end']) if end < seg['end'] else None
    post_name = seg['name'] if not pre else '%s_%08x' % (seg['name'], end)

    # ── segments.txt ────────────────────────────────────────────────────────
    old_txt = open(SEGMENTS).read()
    new_lines = []
    if pre:
        new_lines.append(seg_line(pre[0], pre[1], seg['kind'], seg['name'],
                                  True))
    new_lines.append(seg_line(start, end, 'c_code', name,
                              False, 'src/%s.c (carved by tools/carve.py)'
                              % name))
    if post:
        new_lines.append(seg_line(post[0], post[1], seg['kind'], post_name,
                                  True))
    new_txt = old_txt.replace(seg['line'], '\n'.join(new_lines))
    if new_txt == old_txt:
        die('internal: segment line not found verbatim: %r' % seg['line'])

    # ── split_config.json ───────────────────────────────────────────────────
    cfg_raw = open(CONFIG).read()
    cfg = json.loads(cfg_raw)
    seg_entries = cfg['segments']
    idx = next((i for i, e in enumerate(seg_entries)
                if e['name'] == seg['name']), None)
    if idx is None:
        die('segment %r not in %s "segments"' % (seg['name'], CONFIG))
    template = seg_entries[idx]
    repl = []
    if pre:
        repl.append(dict(template))
    if post:
        e = dict(template)
        e['name'] = post_name
        repl.append(e)
    cfg['segments'][idx:idx + 1] = repl
    ext = set(cfg.get('external_defined', []))
    ext.update(n for _, n in inside)
    cfg['external_defined'] = sorted(ext)
    new_cfg = json.dumps(cfg, indent=2) + '\n'

    # ── linker.ld ───────────────────────────────────────────────────────────
    old_ld = open(LINKER).read()
    block_re = re.compile(
        r'([ \t]*\.%s[ \t]+0x[0-9A-Fa-f]+[ \t]*:[ \t]*\{[^}]*\}[ \t]*>'
        r'[ \t]*ROM)' % re.escape(seg['name']))
    m = block_re.search(old_ld)
    if not m:
        die('linker.ld section .%s not found' % seg['name'])
    blocks = []
    if pre:
        blocks.append(
            '    .%s 0x%08X : {\n'
            '        KEEP(*(.%s)) KEEP(*(.%s.tail))\n'
            '    } > ROM' % (seg['name'], pre[0], seg['name'], seg['name']))
    blocks.append(
        '    /* %s — decompiled to C (src/%s.c, carved by tools/carve.py) */\n'
        '    .%s 0x%08X : {\n'
        '        build/src/%s.o(.text)\n'
        '    } > ROM' % (name, name, name, start, name))
    if post:
        blocks.append(
            '    .%s 0x%08X : {\n'
            '        KEEP(*(.%s)) KEEP(*(.%s.tail))\n'
            '    } > ROM' % (post_name, post[0], post_name, post_name))
    new_ld = old_ld[:m.start(1)] + '\n\n'.join(blocks) + old_ld[m.end(1):]

    # ── emit ────────────────────────────────────────────────────────────────
    changes = [(SEGMENTS, old_txt, new_txt), (CONFIG, cfg_raw, new_cfg),
               (LINKER, old_ld, new_ld)]
    for path, old, new in changes:
        diff = ''.join(difflib.unified_diff(
            old.splitlines(True), new.splitlines(True),
            'a/' + path, 'b/' + path))
        print(diff or ('(%s unchanged)\n' % path))

    print('functions absorbed by src/%s.c: %s'
          % (name, ', '.join(n for _, n in inside)))
    if not write:
        print('\nDRY RUN — re-run with --write to apply, then:\n'
              '  1. src/%s.c (fnmatch-verified) provides the bytes\n'
              '  2. make symbols && make split\n'
              '  3. make clean && make compare' % name)
        return
    for path, _, new in changes:
        open(path, 'w').write(new)
    print('\nAPPLIED. Now: add src/%s.c, then '
          'make symbols && make split && make clean && make compare' % name)


if __name__ == '__main__':
    main()
