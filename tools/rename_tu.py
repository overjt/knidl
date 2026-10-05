#!/usr/bin/env python3
"""rename_tu.py - name the src/*.c translation units after their content
(issue #182, docs/naming.md section 8).

Usage:
    python3 tools/rename_tu.py            # dry run: validate, print the plan
    python3 tools/rename_tu.py --write    # git mv + rewrite every reference
    python3 tools/rename_tu.py --check    # the renamed tree is consistent

The table is docs/analysis/file-renames.csv, one row per renamed file:

    old,new,prefix_header,deviation,evidence

  old            the old stem (`<zone>_<addr>`), the name the file had until
                 #182; closed issues and PRs quote it.
  new            the new stem (`actor_defeat`): `<prefix>_<content>`.
  prefix_header  the include/*.h header that declares most of the file's
                 public functions, with its share (`actor.h 36/36`), or
                 `none 0/0` when no header declares them.
  deviation      empty when the prefix is that header's name; otherwise why
                 the header's name misdescribes the file (the functions that
                 prove its content).  Every file without a header has one.
  evidence       the functions and task bodies the name comes from.

A stem is a file name, a segment name (docs/analysis/segments.txt), an
output section and object path (linker.ld), and a word in the tools,
comments and docs, so --write rewrites the old stem wherever it stands as a
token (not preceded by a letter or digit, not followed by a letter, digit
or underscore: `src/old.c`, `.old`, `build/src/old.o` and `fn_old` all
follow) in every tracked text file except the table itself, after
`git mv src/old.c src/new.c` (so `git log --follow` sees a rename).  The
old stems survive in file-renames.csv only.

A stem that is also a symbol's name (a function in symbols.csv, or a name
renames.csv logs: the two files PR #133 named after their only function)
is a symbol wherever it stands alone, so for it only the file forms are
rewritten and checked: `old.c`, `old.o`, the section `.old`, the segment
name after `c_code` in segments.txt and linker.ld's `/* old - ` block
comment (lesson 4.177).

Validation (dry run, --write and --check): `new` is snake_case, starts with
a prefix of the vocabulary (the game headers' names plus PREFIX_EXTRAS),
holds no address-like hex word, and is unique among the new stems, the
segment names, the src/data/*.c, asm/ and data/ file stems; a row whose
prefix is not its header's name carries a deviation.  --check also proves
the tree: every new file exists, no old one does, no old stem is left
outside the table, and every src/*.c outside the table is a sanctioned
SDK/runtime name (SANCTIONED).  make audit runs check().

After --write: make clean && make compare, make symbols/split/modmap (no
diff), and the per-file assembly oracle (docs/naming.md section 8).
"""

import argparse
import csv
import fnmatch
import os
import re
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TABLE = "docs/analysis/file-renames.csv"
SEGMENTS = "docs/analysis/segments.txt"
FIELDS = ["old", "new", "prefix_header", "deviation", "evidence"]

# Prefixes that are not a header's name (the owner's decision on #182): the
# boot logo and the game-over screen are declared in ending.h, the M38
# "sequences" header, whose name misdescribes them.
PREFIX_EXTRAS = ("boot", "game_over")
# Headers that are not a subsystem: no file takes their name as a prefix.
NOT_SUBSYSTEMS = ("global", "task_vars")
# src/*.c names that are not <prefix>_<content> and stay: the SDK and runtime
# files that already have real names.
SANCTIONED = re.compile(r"^(main|agb_init|agb_sram|m4a_\w+)$")

STEM_RE = re.compile(r"^[a-z][a-z0-9]*(?:_[a-z0-9]+)*$")
# A word that reads as an address: 4+ hex digits with at least one digit.
ADDRESS_WORD_RE = re.compile(r"^(?=[0-9a-f]*[0-9])[0-9a-f]{4,}$")
PLACEHOLDER_RE = re.compile(r"(^|_)(sub|gunk|unk|loc|nullsub)(_|$)")


class TuError(Exception):
    pass


def path(p):
    return os.path.join(ROOT, p)


def git(*args):
    # safe.directory: make audit runs this inside the toolchain image, as
    # root over a tree another user owns.
    res = subprocess.run(["git", "-c", "safe.directory=*"] + list(args), cwd=ROOT,
                         stdout=subprocess.PIPE, stderr=subprocess.PIPE,
                         universal_newlines=True)
    if res.returncode:
        raise TuError("git %s: %s" % (" ".join(args), res.stderr.strip()))
    return res.stdout


def load_table():
    with open(path(TABLE), encoding="utf-8", newline="") as f:
        reader = csv.DictReader(f)
        if reader.fieldnames != FIELDS:
            raise TuError("%s: header must be %s" % (TABLE, ",".join(FIELDS)))
        return list(reader)


def vocabulary():
    heads = sorted(os.path.splitext(f)[0] for f in os.listdir(path("include"))
                   if f.endswith(".h"))
    return [h for h in heads if h not in NOT_SUBSYSTEMS] + list(PREFIX_EXTRAS)


def prefix_of(stem, vocab):
    """The longest vocabulary word the stem starts with (`game_over_choice`
    -> `game_over`), or None."""
    best = None
    for v in vocab:
        if stem == v or stem.startswith(v + "_"):
            if best is None or len(v) > len(best):
                best = v
    return best


def segment_names():
    names = set()
    with open(path(SEGMENTS), encoding="utf-8") as f:
        for ln in f:
            m = re.match(r"(0x[0-9A-Fa-f]+)\s+(0x[0-9A-Fa-f]+)\s+(\S+)\s+(\S+)", ln)
            if m:
                names.add(m.group(4))
    return names


def other_stems():
    """File stems a new stem must not repeat: src/data/*.c, asm/, data/."""
    out = set()
    for d, ext in (("src/data", ".c"), ("asm", ".s"), ("data", ".s")):
        for base, _, files in os.walk(path(d)):
            out.update(os.path.splitext(f)[0] for f in files if f.endswith(ext))
    return out


def validate(rows):
    vocab = vocabulary()
    errors = []
    olds = [r["old"] for r in rows]
    news = [r["new"] for r in rows]
    for name, seen in (("old", olds), ("new", news)):
        dup = sorted({s for s in seen if seen.count(s) > 1})
        if dup:
            errors.append("duplicate %s stems: %s" % (name, ", ".join(dup)))
    if set(olds) & set(news):
        errors.append("stems both old and new: %s" % ", ".join(sorted(set(olds) & set(news))))
    # Before --write the segments are named by the old stems, after it by
    # the new ones: either way a new stem may not repeat any other name.
    taken = (segment_names() - set(olds) - set(news)) | other_stems()
    for r in rows:
        old, new = r["old"], r["new"]
        where = "%s -> %s" % (old, new)
        if not STEM_RE.match(new):
            errors.append("%s: not snake_case" % where)
            continue
        words = new.split("_")
        if any(ADDRESS_WORD_RE.match(w) for w in words):
            errors.append("%s: holds an address-like word" % where)
        if PLACEHOLDER_RE.search(new):
            errors.append("%s: holds a placeholder word" % where)
        if SANCTIONED.match(new):
            errors.append("%s: takes a sanctioned SDK/runtime name" % where)
        if new in taken:
            errors.append("%s: already a segment or file name" % where)
        prefix = prefix_of(new, vocab)
        if prefix is None:
            errors.append("%s: prefix not in the vocabulary (%s)" % (where, ", ".join(vocab)))
            continue
        m = re.match(r"^(\S+)\.h (\d+)/(\d+)$|^none 0/0$", r["prefix_header"])
        if not m:
            errors.append("%s: prefix_header must read `<header>.h k/n` or `none 0/0`" % where)
            continue
        header = m.group(1)
        if prefix != header and not r["deviation"].strip():
            errors.append("%s: prefix %s_ is not its header's (%s) and the row has no deviation"
                          % (where, prefix, r["prefix_header"]))
        if prefix == header and r["deviation"].strip():
            errors.append("%s: prefix is its header's name but the row has a deviation" % where)
        if not r["evidence"].strip():
            errors.append("%s: no evidence" % where)
    return errors


def symbol_names():
    """Every function name symbols.csv has and every name renames.csv logs."""
    names = set()
    for f, cols in (("docs/analysis/symbols.csv", ("name",)),
                    ("docs/analysis/renames.csv", ("old", "new"))):
        with open(path(f), encoding="utf-8", newline="") as fh:
            for row in csv.DictReader(fh):
                names.update(row[c] for c in cols)
    return names


def stem_pattern(stems):
    """The token pattern of the stems; a stem that is also a symbol's name
    matches in its file forms only."""
    stems = sorted(stems, key=len, reverse=True)
    symbols = symbol_names()
    plain = [s for s in stems if s not in symbols]
    files = [s for s in stems if s in symbols]
    parts = []
    if plain:
        parts.append(r"(?<![A-Za-z0-9])(%s)(?![A-Za-z0-9_])"
                     % "|".join(re.escape(s) for s in plain))
    if files:
        alt = "|".join(re.escape(s) for s in files)
        parts.append(r"(?<![A-Za-z0-9])(%s)(?=\.[co]\b)" % alt)
        parts.append(r"(?<=\.)(%s)(?![A-Za-z0-9_])" % alt)
        parts.append(r"(?<=c_code )(%s)(?![A-Za-z0-9_])" % alt)
        parts.append(r"(?<=/\* )(%s)(?= \u2014 )" % alt)
    return re.compile("|".join(parts))


def stem_of(m):
    return next(g for g in m.groups() if g)


def tree_files():
    """The tracked files: `git ls-files`, or, where git cannot see the
    repository (a git worktree mounted alone into the toolchain image), a
    walk of the tree minus the top-level .gitignore's patterns."""
    try:
        return [f for f in git("ls-files", "-z").split("\0") if f]
    except TuError:
        pass
    dirs, names = {".git"}, []
    with open(path(".gitignore"), encoding="utf-8") as fh:
        for ln in fh:
            ln = ln.strip()
            if not ln or ln.startswith("#"):
                continue
            if ln.endswith("/"):
                dirs.add(ln.rstrip("/"))
            else:
                names.append(ln)
    out = []
    for base, subdirs, files in os.walk(ROOT):
        subdirs[:] = sorted(d for d in subdirs if d not in dirs)
        for f in sorted(files):
            if not any(fnmatch.fnmatch(f, n) for n in names):
                out.append(os.path.relpath(os.path.join(base, f), ROOT))
    return out


def tracked_text_files():
    out = []
    for f in tree_files():
        if f == TABLE:
            continue
        p = path(f)
        if not os.path.isfile(p):
            continue
        try:
            with open(p, encoding="utf-8", newline="") as fh:
                text = fh.read()
        except (UnicodeDecodeError, OSError):
            continue
        out.append((f, text))
    return out


def plan(rows):
    mapping = {r["old"]: r["new"] for r in rows}
    pat = stem_pattern(mapping)
    edits = []
    for f, text in tracked_text_files():
        n = len(pat.findall(text))
        if n:
            edits.append((f, n, pat.sub(lambda m: mapping[stem_of(m)], text)))
    return edits


def check(rows=None):
    """The renamed tree: the errors as a list (empty when it holds)."""
    rows = load_table() if rows is None else rows
    errors = validate(rows)
    for r in rows:
        if not os.path.exists(path("src/%s.c" % r["new"])):
            errors.append("src/%s.c (renamed from %s) is missing" % (r["new"], r["old"]))
        if os.path.exists(path("src/%s.c" % r["old"])):
            errors.append("src/%s.c still exists (renamed to %s)" % (r["old"], r["new"]))
    pat = stem_pattern(r["old"] for r in rows)
    new_of = {r["old"]: r["new"] for r in rows}
    for f, text in tracked_text_files():
        for i, ln in enumerate(text.split("\n"), 1):
            for m in pat.finditer(ln):
                errors.append("%s:%d: old file name %s (now %s)"
                              % (f, i, stem_of(m), new_of[stem_of(m)]))
    named = {r["new"] for r in rows}
    for f in sorted(os.listdir(path("src"))):
        stem, ext = os.path.splitext(f)
        if ext == ".c" and stem not in named and not SANCTIONED.match(stem):
            errors.append("src/%s is neither in %s nor a sanctioned SDK/runtime name" % (f, TABLE))
    return errors


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--write", action="store_true", help="git mv and rewrite the references")
    ap.add_argument("--check", action="store_true", help="verify the renamed tree")
    args = ap.parse_args()
    try:
        rows = load_table()
        if args.check:
            errors = check(rows)
            for e in errors:
                print("rename_tu: " + e)
            if errors:
                return 1
            print("rename_tu: %d files renamed, no old name outside %s" % (len(rows), TABLE))
            return 0
        errors = validate(rows)
        for e in errors:
            print("rename_tu: " + e)
        if errors:
            return 1
        todo = [r for r in rows if os.path.exists(path("src/%s.c" % r["old"]))]
        for r in todo:
            if os.path.exists(path("src/%s.c" % r["new"])):
                raise TuError("src/%s.c already exists" % r["new"])
        edits = plan(rows)
        for f, n, _ in sorted(edits, key=lambda e: -e[1]):
            print("%7d  %s" % (n, f))
        print("%d references in %d files; %d files to move" % (
            sum(n for _, n, _ in edits), len(edits), len(todo)))
        if not args.write:
            print("dry run: pass --write to apply")
            return 0
        for r in todo:
            git("mv", "src/%s.c" % r["old"], "src/%s.c" % r["new"])
        for f, _, text in plan(rows):
            with open(path(f), "w", encoding="utf-8", newline="") as fh:
                fh.write(text)
        print("moved %d files; now: make clean && make compare" % len(todo))
        return 0
    except TuError as e:
        print("rename_tu: error: %s" % e, file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
