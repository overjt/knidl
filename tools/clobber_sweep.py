#!/usr/bin/env python3
"""clobber_sweep.py — iterate the empty-asm-clobber search over one function.

`asm("" ::: "rN")` costs no code and forbids the hard register rN for every
pseudo whose live range covers that point (docs/lessons-learned.md §3.341).
When a candidate is byte-identical to the ROM except for which registers the
allocator picked, the fix is usually one such clobber — but they compose, and a
single round of the search rarely finds the pair that closes the function
(§3.352).  This inserts a clobber at every statement boundary, keeps the best
scoring one, and repeats until a round stops paying.

Usage:
    python3 tools/clobber_sweep.py <file.c> <start> <end> [rounds] [--old|--newpb]

<file.c> must live inside the repository (fnmatch.sh compiles it inside the
Docker image with the repo bind-mounted) and must contain a marker line

    /* --- functions --- */

with exactly one function after it; everything before it (includes, externs) is
reused verbatim.  The winner is written to <file.c>.best.c after every round, so
the search can be interrupted at any time.

Score is `differing_bytes + 1000 * |size_delta|`, i.e. a candidate that reaches
the ROM's exact byte count always sorts above a smaller-diff one that is still
missing instructions — the size is the structural signal, the diff is the
allocation one.
"""
import re
import subprocess
import sys

REGS = ['"r0"', '"r1"', '"r2"', '"r3"', '"r4"', '"r5"', '"r6"', '"ip"', '"sl"',
        '"r8"', '"r9"']
# r7 is Thumb's frame-pointer register; global.c clears the eliminable
# registers out of every conflict set, so clobbering it is a no-op (§3.351).
MIN_GAIN = 2
BLOCK_OPENERS = ('if', 'else', 'while', 'for', 'do')


def main(argv):
    if len(argv) < 4:
        sys.stderr.write(__doc__)
        return 2
    path, start, end = argv[1:4]
    rounds = 6
    flags = ['--newpb', '--no-werror']
    for a in argv[4:]:
        if a.isdigit():
            rounds = int(a)
        elif a in ('--old', '--old2', '--newpb'):
            flags[0] = a
    src = open(path).read()
    if '/* --- functions --- */\n' not in src:
        sys.stderr.write('%s: missing the /* --- functions --- */ marker\n' % path)
        return 2
    hdr, body = src.split('/* --- functions --- */\n', 1)
    tmp = path + '.sweep.c'

    def score(text):
        open(tmp, 'w').write(hdr + '/* --- functions --- */\n' + text)
        r = subprocess.run(['./tools/fnmatch.sh', start, end, tmp] + flags,
                           capture_output=True, text=True)
        if 'MATCH (' in r.stdout:
            return 0
        m = re.search(r'candidate=(\d+) bytes, target=(\d+) bytes, (\d+) differing',
                      r.stdout)
        if not m:
            return 10 ** 9
        cand, target, diff = map(int, m.groups())
        return diff + abs(cand - target) * 1000

    cur = body
    best = score(cur)
    print('start %d' % best, flush=True)
    for rnd in range(rounds):
        if best == 0:
            break
        lines = cur.split('\n')
        points = []
        for i, line in enumerate(lines):
            s = line.strip()
            if not s or s in ('{', '}') or s.startswith(('/*', 'asm(')) or s.endswith(':'):
                continue
            if i <= 2:
                continue
            # never insert between an unbraced if/else/while header and its body:
            # the asm would become the body and the real statement unconditional
            prev = lines[i - 1].strip()
            if prev.startswith(BLOCK_OPENERS) and not prev.endswith(('{', ';', '}')):
                continue
            points.append(i)
        pick, pick_score = None, best
        for reg in REGS:
            for i in points:
                indent = re.match(r'\s*', lines[i]).group(0)
                trial = lines[:i] + [indent + 'asm("" ::: %s);' % reg] + lines[i:]
                s = score('\n'.join(trial))
                if s < pick_score:
                    pick_score, pick = s, ('\n'.join(trial), reg, lines[i].strip()[:60])
        if pick is None or best - pick_score < MIN_GAIN:
            print('round %d: no further gain (best %d)' % (rnd, best), flush=True)
            break
        cur, best = pick[0], pick_score
        print('round %d: %d  <- asm %s before %r' % (rnd, best, pick[1], pick[2]),
              flush=True)
        open(path + '.best.c', 'w').write(hdr + '/* --- functions --- */\n' + cur)
    open(path + '.best.c', 'w').write(hdr + '/* --- functions --- */\n' + cur)
    print('final %d -> %s.best.c' % (best, path))
    return 0


if __name__ == '__main__':
    sys.exit(main(sys.argv))
