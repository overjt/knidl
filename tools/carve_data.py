#!/usr/bin/env python3
"""carve_data.py - carve a functional table out of the structure-only data
segments so a C definition in src/data/ owns it (issue #36 phase 2; the
data twin of tools/carve.py).

Usage:
    python3 tools/carve_data.py <start> <end> <name> [--c-file F] [--section S]
                                [--record-in-asset] [--write]
    python3 tools/carve_data.py --runs FILE [--record-in-asset] [--write]

    start   first VMA of the table: a data_symbols label (the C object that
            starts the range), 4-aligned.
    end     first VMA NOT in the table (4-aligned).  The range may cover
            several consecutive data segments (the task-type table starts in
            one and ends in the next); segments it covers completely vanish.
    name    new segment/section/source name (snake_case); the build will
            expect src/data/<name>.c to provide the bytes as .rodata.  It may
            be the name of a data segment the carve consumes entirely.
    --c-file src/data/F.c
            the C file that defines the range (default src/data/<name>.c);
            several rows may share one file (a family of records, #167).
    --section .S
            the input section the row reads (default .rodata): a run of
            records that the C file places with
            __attribute__((section(".S"))), as src/data/actor_records.c
            does for its runs.  The linker.ld block then reads
            build/src/data/F.o(.S).
    --record-in-asset
            allow a range inside an "asset": true segment: a functional
            record (a RoomDef header, a BG animation script) between asset
            bytes.  The reviewer checks the asset/functional line
            (docs/data.md section 1); the asset bytes around it stay
            structure.
    --runs FILE
            many carves at once: one `start end name [c_file [section]]`
            line each (# comments allowed), applied last range first.

A carve inside a segment keeps the data file: the piece after the range is
named `<zone>_<start>` and gets the config key "zone" (tools/split.py then
writes all the pieces of the zone into data/<zone>.s, one section each,
#167).  A carve inside a grouped output section (tools/ldgroup.py) needs
`ldgroup.py --ungroup <group> --write` first, and the group again after.

Without --write this is a DRY RUN that prints the edits as unified diffs.
With --write it rewrites:

    docs/analysis/segments.txt   (pre / c_data / post rows)
    tools/split_config.json      (segments list; pre/post keep the flags of
                                  the segment they came from)
    linker.ld                    (.<name> reading
                                  build/src/data/<name>.o(.rodata), or the
                                  --c-file object's --section)

It prints the labels inside the range: the C file must define each of them,
in address order and at its offset, because C objects are laid out in
definition order.  tools/split.py treats a `c_data` row as C-owned: its
data_symbols are C symbols (no rom_syms.s absolutes), and a pointer_tables
entry inside it emits no words but still marks its "targets" records.

Only functional tables move this way, each with the consumer that proves
its layout (docs/data.md sections 1 and 7).  Assets never do.

Before --write, verify the C against the ROM like a function:

    ./tools/fnmatch.sh <start> <end> src/data/<name>.c --rodata [--section=.S]

Afterwards:

    1. git add src/data/<name>.c
    2. make split          # the neighbouring data files shrink
    3. make clean && make compare      # must print: knidl.gba: OK
"""
import difflib
import json
import re
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
import ldblocks  # noqa: E402  (tools/ldblocks.py: linker.ld section blocks)

SEGMENTS = 'docs/analysis/segments.txt'
CONFIG = 'tools/split_config.json'
LINKER = 'linker.ld'


def die(msg):
    sys.exit('carve_data: error: ' + msg)


def load_segments(text):
    rows = []
    for ln in text.splitlines():
        m = re.match(r'(0x[0-9A-Fa-f]+)\s+(0x[0-9A-Fa-f]+)\s+(\S+)\s+(\S+)'
                     r'(\s+\[split\])?(\s*#.*)?$', ln.strip())
        if m:
            rows.append({
                'start': int(m.group(1), 16), 'end': int(m.group(2), 16),
                'kind': m.group(3), 'name': m.group(4),
                'comment': (m.group(6) or '').strip().lstrip('#').strip(),
                'line': ln.rstrip('\n'),
            })
    return rows


def seg_line(start, end, kind, name, comment=''):
    return '0x%08X 0x%08X %s %s%s' % (start, end, kind, name,
                                     (' # ' + comment) if comment else '')


def section_block(name, vma):
    return ldblocks.data_block(name, vma)


def carve(st, start, end, name, c_file=None, section=None,
          record_in_asset=False):
    """Apply one carve to the in-memory state st = {'seg', 'cfg', 'ld'}
    (segments.txt text, split_config.json dict, linker.ld text).  Returns
    the labels the C file must define, [(addr, name)] in order."""
    if not re.fullmatch(r'[a-z][a-z0-9_]*', name):
        die('name must be snake_case: %r' % name)
    if start % 4 or end % 4:
        die('start/end must be 4-aligned (a C object is word-aligned)')
    if end <= start:
        die('end must be > start')
    c_file = c_file or 'src/data/%s.c' % name
    m = re.fullmatch(r'src/data/([a-z][a-z0-9_]*)\.c', c_file)
    if not m:
        die('--c-file must be src/data/<snake_case>.c: %r' % c_file)
    obj = 'build/src/data/%s.o' % m.group(1)
    if section is not None and not re.fullmatch(r'\.[a-z][a-z0-9_]*', section):
        die('--section must be .<snake_case>: %r' % section)

    cfg = st['cfg']
    data_symbols = dict((int(a, 16), n) for a, n in cfg['data_symbols'].items())
    extra = dict((int(a, 16), n) for a, n in cfg.get('extra_labels', {}).items())
    if start not in data_symbols:
        die('start 0x%08X has no data_symbols name: the C object that starts '
            'the range needs one' % start)

    segs = load_segments(st['seg'])
    first = next((i for i, s in enumerate(segs)
                  if s['start'] <= start < s['end']), None)
    last = next((i for i, s in enumerate(segs)
                 if s['start'] < end <= s['end']), None)
    if first is None or last is None:
        die('range is not inside the segment table')
    covered = segs[first:last + 1]
    # the name may be that of a segment the carve consumes entirely (a
    # whole data segment becoming C keeps its name), never another one's
    reuse = [s for s in covered
             if s['name'] == name and s['start'] >= start and s['end'] <= end]
    if any(s['name'] == name for s in segs) and not reuse:
        die('segment name %r already exists' % name)
    for a, b in zip(covered, covered[1:]):
        if a['end'] != b['start']:
            die('segments %s and %s are not adjacent' % (a['name'], b['name']))
    for s in covered:
        if s['kind'] != 'data':
            die('segment %s is %s, not a structure-only data segment'
                % (s['name'], s['kind']))
    cfg_segs = cfg['segments']
    cfg_idx = {}
    for s in covered:
        idx = next((i for i, e in enumerate(cfg_segs) if e['name'] == s['name']), None)
        if idx is None:
            die('segment %r not in %s "segments"' % (s['name'], CONFIG))
        if cfg_segs[idx].get('asset') and not record_in_asset:
            die('segment %s is an asset segment: assets never become C '
                '(docs/data.md section 1); a functional record that lies '
                'inside an asset zone (a RoomDef header, a BG animation '
                'script) needs --record-in-asset' % s['name'])
        cfg_idx[s['name']] = idx

    # labels in the range: the C file defines them, in this order
    inside = sorted((a, n) for a, n in data_symbols.items() if start <= a < end)
    bad = [(a, n) for a, n in extra.items() if start <= a < end]
    if bad:
        die('extra_labels inside the range (%s): move them to data_symbols, '
            'C defines them' % ', '.join('%s 0x%08X' % (n, a) for a, n in bad))
    for t in cfg.get('pointer_tables', []):
        ts = int(t['start'], 16)
        if 'count' in t:
            te = ts + int(str(t['count']), 0) * int(str(t.get('stride', 4)), 0)
        elif 'end' in t:
            te = int(t['end'], 16)
        else:
            te = ts + 4
        if (ts < start < te) or (ts < end < te):
            die('pointer_tables entry at %s straddles the range' % t['start'])

    pre_seg, post_seg = covered[0], covered[-1]
    pre = (pre_seg['start'], start) if start > pre_seg['start'] else None
    post = (end, post_seg['end']) if end < post_seg['end'] else None
    # the zone (data file, tools/split.py) the pieces stay in: a segment
    # cut by a carve keeps one data/<zone>.s, its pieces named
    # <zone>_<start> (#167)
    post_cfg = cfg_segs[cfg_idx[post_seg['name']]]
    zone = post_cfg.get('zone', post_seg['name'])
    if post:
        if post_seg is pre_seg and pre:
            post_name = '%s_%08x' % (zone, end)
            if any(s['name'] == post_name for s in segs):
                die('segment name %r already exists' % post_name)
        else:
            post_name = post_seg['name']
    # ---- segments.txt
    new_lines = []
    if pre:
        new_lines.append(seg_line(pre[0], pre[1], 'data', pre_seg['name'],
                                  pre_seg['comment']))
    if section:
        note = '%s section %s (carved by tools/carve_data.py)' % (c_file, section)
    else:
        note = '%s (carved by tools/carve_data.py)' % c_file
    new_lines.append(seg_line(start, end, 'c_data', name, note))
    if post:
        new_lines.append(seg_line(post[0], post[1], 'data', post_name,
                                  post_seg['comment'] if post_name == post_seg['name'] else ''))
    block = '\n'.join(s['line'] for s in covered)
    if block not in st['seg']:
        die('internal: segment rows are not consecutive in %s' % SEGMENTS)
    st['seg'] = st['seg'].replace(block, '\n'.join(new_lines))

    # ---- split_config.json
    repl = []
    if pre:
        repl.append(dict(cfg_segs[cfg_idx[pre_seg['name']]]))
    if post:
        e = dict(post_cfg)
        e['name'] = post_name
        if post_name != zone:
            e['zone'] = zone
        repl.append(e)
    lo = min(cfg_idx.values())
    hi = max(cfg_idx.values())
    if hi - lo != len(covered) - 1:
        die('internal: config segment entries are not consecutive')
    cfg['segments'][lo:hi + 1] = repl

    # ---- linker.ld
    old_ld = st['ld']
    spans = []
    for s in covered:
        m = ldblocks.block_re(s['name']).search(old_ld)
        if not m:
            die(ldblocks.not_found(old_ld, s['name']))
        if ldblocks.check_not_group(m.group(0), s['name']):
            die(ldblocks.check_not_group(m.group(0), s['name']))
        spans.append((m.start(), m.end()))
    cblock = ('    /* %s - functional table in C (%s, carved by '
              'tools/carve_data.py) */\n' % (name, c_file)
              + ldblocks.block(name, start,
                               '%s(%s)' % (obj, section or '.rodata')))
    # one replacement per covered section block (comments between the
    # blocks stay where they are): the first gets pre + the C section, the
    # last gets post, the ones in between vanish
    repls = [None] * len(covered)
    head = ([section_block(pre_seg['name'], pre[0])] if pre else []) + [cblock]
    if len(covered) == 1:
        repls[0] = '\n\n'.join(head + ([section_block(post_name, post[0])] if post else []))
    else:
        repls[0] = '\n\n'.join(head)
        repls[-1] = section_block(post_name, post[0]) if post else ''
        for i in range(1, len(covered) - 1):
            repls[i] = ''
    # the comment line above the first block describes that segment: with
    # no pre part it now belongs above the post part
    cm = re.search(r'([ \t]*/\*[^\n]*\*/[ \t]*\n)$', old_ld[:spans[0][0]])
    if cm and not pre and post and len(covered) == 1:
        spans[0] = (cm.start(1), spans[0][1])
        repls[0] = repls[0].replace(section_block(post_name, post[0]),
                                    cm.group(1).rstrip('\n') + '\n' + section_block(post_name, post[0]))
    new_ld = old_ld
    for (a, b), r in sorted(zip(spans, repls), reverse=True):
        new_ld = new_ld[:a] + r + new_ld[b:]
    st['ld'] = new_ld
    return inside


def parse_runs(path):
    """`start end name [c_file [section]]` lines (# comments allowed)."""
    runs = []
    for ln in open(path):
        ln = ln.split('#', 1)[0].split()
        if not ln:
            continue
        if not 3 <= len(ln) <= 5:
            die('%s: bad line %r' % (path, ' '.join(ln)))
        runs.append((int(ln[0], 0), int(ln[1], 0), ln[2],
                     ln[3] if len(ln) > 3 else None,
                     ln[4] if len(ln) > 4 else None))
    return runs


def main():
    argv = sys.argv[1:]
    write = '--write' in argv
    record_in_asset = '--record-in-asset' in argv
    opts = {}
    pos = []
    i = 0
    while i < len(argv):
        a = argv[i]
        if a in ('--write', '--record-in-asset'):
            pass
        elif a in ('--c-file', '--section', '--runs'):
            if i + 1 >= len(argv):
                die('%s needs a value' % a)
            opts[a] = argv[i + 1]
            i += 1
        elif a.startswith('--'):
            die('unknown option %s\n%s' % (a, __doc__))
        else:
            pos.append(a)
        i += 1
    if '--runs' in opts:
        if pos or '--c-file' in opts or '--section' in opts:
            die('--runs takes no other range: put c_file and section on each line')
        runs = parse_runs(opts['--runs'])
    elif len(pos) == 3:
        runs = [(int(pos[0], 0), int(pos[1], 0), pos[2],
                 opts.get('--c-file'), opts.get('--section'))]
    else:
        die('usage: carve_data.py <start> <end> <name> [--write]\n' + __doc__)

    old_seg = open(SEGMENTS).read()
    cfg_raw = open(CONFIG).read()
    old_ld = open(LINKER).read()
    st = {'seg': old_seg, 'cfg': json.loads(cfg_raw), 'ld': old_ld}
    # last run first, so the data piece after each run is cut from the
    # segment the earlier runs still share (its name: <zone>_<start>)
    defines = []
    for start, end, name, c_file, section in sorted(runs, reverse=True):
        inside = carve(st, start, end, name, c_file, section, record_in_asset)
        defines.append((start, end, name, c_file or 'src/data/%s.c' % name,
                        section, inside))
    new_cfg = json.dumps(st['cfg'], indent=2) + '\n'

    changes = [(SEGMENTS, old_seg, st['seg']), (CONFIG, cfg_raw, new_cfg),
               (LINKER, old_ld, st['ld'])]
    for path, old, new in changes:
        diff = ''.join(difflib.unified_diff(
            old.splitlines(True), new.splitlines(True),
            'a/' + path, 'b/' + path))
        if len(runs) > 1 and len(diff) > 6000:
            diff = diff[:6000] + '\n... (%d more characters)\n' % (len(diff) - 6000)
        print(diff or ('(%s unchanged)\n' % path))
    for start, end, name, c_file, section, inside in sorted(defines):
        print('%s%s must define, in this order (0x%08X-0x%08X, row %s):'
              % (c_file, (' section %s' % section) if section else '',
                 start, end, name))
        for a, n in inside:
            print('    +0x%04X  %s' % (a - start, n))
    if not write:
        start, end, name, c_file, section, _i = sorted(defines)[0]
        print('\nDRY RUN - verify first:\n'
              '  ./tools/fnmatch.sh 0x%08X 0x%08X %s --rodata%s\n'
              'then re-run with --write, make split, make clean && make compare'
              % (start, end, c_file,
                 (' --section=%s' % section) if section else ''))
        return
    for path, _, new in changes:
        open(path, 'w').write(new)
    print('\nAPPLIED %d carve(s).  Now: make split && make clean && make compare'
          % len(runs))


if __name__ == '__main__':
    main()
