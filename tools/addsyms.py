#!/usr/bin/env python3
"""addsyms.py — fold `undefined reference to gUnk_<hexaddr>' link errors into
tools/split_config.json's data_symbols map.

Usage:  make compare 2>&1 | python3 tools/addsyms.py [--dry-run]

Every gUnk_<8 hex digits> name a freshly-carved C file references resolves to
an absolute address (a RAM cell, or a ROM data word the splitter does not label).
The linker only learns that mapping from data_symbols, so after each carve the
build fails once with a wall of `undefined reference to gUnk_*'. This reads that
wall from stdin and writes the entries back, sorted, leaving everything else in
split_config.json untouched.
"""
import json, re, sys

DRY = '--dry-run' in sys.argv[1:]
names = sorted(set(re.findall(r"undefined reference to `(gUnk_[0-9A-Fa-f]{8})'", sys.stdin.read())))
if not names:
    print('addsyms: no gUnk_<addr> undefined references on stdin'); sys.exit(0)

path = 'tools/split_config.json'
cfg = json.load(open(path))
ds = cfg.setdefault('data_symbols', {})
added = []
for n in names:
    key = '0x%08X' % int(n[5:], 16)
    if key in ds:
        if ds[key] != n:
            sys.exit('addsyms: %s already maps to %s, refusing to clobber' % (key, ds[key]))
        continue
    ds[key] = n
    added.append(key)

cfg['data_symbols'] = {k: ds[k] for k in sorted(ds, key=lambda s: int(s, 16))}
print('addsyms: %d new, %d already present (%d total)' % (len(added), len(names) - len(added), len(ds)))
for k in added:
    print('  %s -> %s' % (k, cfg['data_symbols'][k]))
if not DRY:
    with open(path, 'w') as f:
        json.dump(cfg, f, indent=2)
        f.write('\n')
