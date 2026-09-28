#!/usr/bin/env python3
"""resegment.py - rename or re-partition consecutive structure-only data
segments (issue #36 phase 2: segs 13 and 20 got content-true names).

Usage:
    python3 tools/resegment.py --end <addr> \\
        --seg <start> <name> "<description>" [--seg ...] [--write]

The --seg starts (ascending) and --end describe the new partition.  It must
cover exactly the union of the consecutive `data` segments it overlaps: the
first start is the first old segment's start and --end is the last old
segment's end, so only the boundaries in between move.  A new segment
inherits the "asset" flag of the old segment its start lies in.

It rewrites, as structure only (a data segment holds labels, symbolic
words and .incbin slices, so a boundary is only where one file ends):

    docs/analysis/segments.txt   the rows, with the description as comment
    tools/split_config.json      the "segments" entries
    linker.ld                    the section blocks and their comments

and with --write deletes data/<old>.s for every old name that is gone.
Without --write it prints the diffs.  Afterwards:

    make split && make clean && make compare && make datastats
"""
import argparse
import difflib
import json
import os
import re
import sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ldblocks  # noqa: E402  (tools/ldblocks.py: linker.ld section blocks)

SEGMENTS = 'docs/analysis/segments.txt'
CONFIG = 'tools/split_config.json'
LINKER = 'linker.ld'


def die(msg):
    sys.exit('resegment: error: ' + msg)


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument('--end', required=True)
    ap.add_argument('--seg', nargs=3, action='append', required=True,
                    metavar=('START', 'NAME', 'DESCRIPTION'))
    ap.add_argument('--write', action='store_true')
    args = ap.parse_args()
    end = int(args.end, 0)
    new = [(int(s, 0), n, d) for s, n, d in args.seg]
    for (a, n, _d), (b, _m, _e) in zip(new, new[1:] + [(end, None, None)]):
        if not re.fullmatch(r'[a-z][a-z0-9_]*', n):
            die('name must be snake_case: %r' % n)
        if b <= a:
            die('segment %s: starts must ascend and end after the last' % n)
    rows = []
    for ln in open(SEGMENTS):
        m = re.match(r'(0x[0-9A-Fa-f]+)\s+(0x[0-9A-Fa-f]+)\s+(\S+)\s+(\S+)', ln)
        if m:
            rows.append((int(m.group(1), 16), int(m.group(2), 16), m.group(3),
                         m.group(4), ln.rstrip('\n')))
    covered = [r for r in rows if r[0] < end and r[1] > new[0][0]]
    if not covered or covered[0][0] != new[0][0] or covered[-1][1] != end:
        die('the new partition must start at an old segment start and end at '
            'an old segment end')
    for r in covered:
        if r[2] != 'data':
            die('segment %s is %s, not data' % (r[3], r[2]))
    old_names = [r[3] for r in covered]
    names = [n for _a, n, _d in new]
    others = set(r[3] for r in rows) - set(old_names)
    clash = [n for n in names if n in others]
    if clash or len(set(names)) != len(names):
        die('name clash: %s' % (clash or names))

    cfg_raw = open(CONFIG).read()
    cfg = json.loads(cfg_raw)
    idx = [i for i, e in enumerate(cfg['segments']) if e['name'] in old_names]
    if len(idx) != len(old_names) or idx != list(range(idx[0], idx[0] + len(idx))):
        die('config segment entries of %s are not consecutive' % old_names)
    flags = dict((e['name'], e) for e in cfg['segments'])

    def asset_of(addr):
        for r in covered:
            if r[0] <= addr < r[1]:
                return bool(flags[r[3]].get('asset'))
        die('internal: 0x%08X' % addr)

    bounds = [a for a, _n, _d in new] + [end]
    # ---- segments.txt
    old_txt = open(SEGMENTS).read()
    block = '\n'.join(r[4] for r in covered)
    if block not in old_txt:
        die('segment rows are not consecutive')
    new_rows = ['0x%08X 0x%08X data %s # %s' % (bounds[i], bounds[i + 1], n, d)
                for i, (_a, n, d) in enumerate(new)]
    new_txt = old_txt.replace(block, '\n'.join(new_rows))
    # ---- config
    entries = []
    for a, n, _d in new:
        e = {'name': n}
        if asset_of(a):
            e['asset'] = True
        entries.append(e)
    cfg['segments'][idx[0]:idx[-1] + 1] = entries
    new_cfg = json.dumps(cfg, indent=2) + '\n'
    # ---- linker.ld: the run of blocks (each with its comment line above)
    old_ld = open(LINKER).read()
    spans = []
    for n in old_names:
        m = ldblocks.block_re(n, comment=True).search(old_ld)
        if not m:
            die(ldblocks.not_found(old_ld, n))
        if ldblocks.check_not_group(m.group(0), n):
            die(ldblocks.check_not_group(m.group(0), n))
        spans.append((m.start(), m.end()))
    for (a0, a1), (b0, _b1) in zip(spans, spans[1:]):
        if old_ld[a1:b0].strip():
            die('linker.ld blocks of %s are not consecutive' % old_names)
    blocks = ['    /* %s */\n' % d + ldblocks.data_block(n, bounds[i])
              for i, (_a, n, d) in enumerate(new)]
    new_ld = old_ld[:spans[0][0]] + '\n\n'.join(blocks) + old_ld[spans[-1][1]:]

    for path, old, newt in ((SEGMENTS, old_txt, new_txt), (CONFIG, cfg_raw, new_cfg),
                            (LINKER, old_ld, new_ld)):
        print(''.join(difflib.unified_diff(old.splitlines(True), newt.splitlines(True),
                                           'a/' + path, 'b/' + path)))
    gone = [n for n in old_names if n not in names]
    print('old -> new:')
    for r in covered:
        print('    %-28s 0x%08X-0x%08X' % (r[3], r[0], r[1]))
    for i, (_a, n, _d) in enumerate(new):
        print('    -> %-25s 0x%08X-0x%08X%s' % (n, bounds[i], bounds[i + 1],
                                              ' (asset)' if entries[i].get('asset') else ''))
    if not args.write:
        print('\nDRY RUN - re-run with --write, then make split && make clean && make compare')
        return
    open(SEGMENTS, 'w').write(new_txt)
    open(CONFIG, 'w').write(new_cfg)
    open(LINKER, 'w').write(new_ld)
    for n in gone:
        p = os.path.join('data', n + '.s')
        if os.path.exists(p):
            os.remove(p)
            print('removed %s' % p)
    print('APPLIED.  Now: make split && make clean && make compare')


if __name__ == '__main__':
    main()
