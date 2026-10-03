# tools/archive — retired campaign helpers

Tools that served one decompilation campaign and that no Makefile target, CI
step, other tool or current workflow document uses any more.  They are kept
for the record (the lessons in `docs/lessons-learned.md` cite them) and are
not maintained: CI does not compile them (its `py_compile` covers
`tools/*.py` only), and they may need the tree of their time to run.  Run
them from the repository root.

| tool | what it was for | issue |
|---|---|---|
| `clobber_sweep.py` | Iterated the zero-byte `asm("" ::: "rN")` clobber search over one candidate function: inserted a clobber at every statement boundary, scored each with `tools/fnmatch.sh` (differing bytes + 1000 x size delta), kept the best and re-ran on its own winner (lessons 3.341, 3.350-3.352).  Retired by the natural-C campaign, which found the levers described the candidates rather than the functions (#154, lessons 3.495-3.514); the remaining levers are code exceptions in `docs/audit.md` section 3.  Usage of its time: `python3 tools/archive/clobber_sweep.py <file.c> <start> <end> [rounds] [--old\|--newpb]`. | #94 |
