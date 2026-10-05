#!/usr/bin/env python3
"""task_alias.py - name struct Task's per-family registers with alias macros.

Usage:
    tools/task_alias.py --defs DEFS.csv --sites SITES.csv [--write]
    tools/task_alias.py --verify-cpp REF
    tools/task_alias.py --verify-types
    tools/task_alias.py --list
    tools/task_alias.py --rename OLD NEW [--family F --header H --type T --role R] --evidence E [--write]

`struct Task`'s registers (`unk18`-`unk34`, `unk46`, `unk6C`-`unk70`,
`unk74`) hold a different thing in every task family.  pret names such
registers with object-like macros over the shared member (pokeemerald's
`#define tState data[0]` over `struct Task`'s `data[16]`); this tool does the
same (docs/header-conventions.md, "Per-family registers"; #155 run 5).  An
alias such as

    #define fireLionHopCount unk6C /* s16: hops left in FireLionHop */

lives in include/task_vars.h (included at the end of include/task.h), in the
block of its family, and a family's code then writes
`gCurTask->fireLionHopCount`.  After preprocessing nothing has changed.

DEFS.csv (`family,header,alias,field,type,role,evidence`): one row per alias.
`family` is the word the family's functions use (`FireLion`); the alias must
start with it in lowerCamelCase.  `header` describes the family (its task
types, its state table) and is needed only for the family's first alias.
`type` is how the code reads the register (`s16` when the sites cast
`(s16)` or read `*(s16 *)&`), `role` one line.

SITES.csv (`file,function,pointer,field,alias`): where an alias is used.
Inside the body of FUNCTION in FILE, every `POINTER->FIELD` /
`POINTER.FIELD` outside comments becomes `POINTER->ALIAS`.  POINTER is
written as the code writes it (`gCurTask`, a local `t`); a row that matches
nothing is an error.  Only pointers proven to hold a task of the family get
a row (its own bodies on gCurTask and the locals that copy it, a spawner on
the child it just created); helpers several families share keep `unkXX`.

Dry run by default; --write edits src/, writes include/task_vars.h and
appends one renames.csv row per alias (kind `alias`, `Task.unk6C` ->
`Task.fireLionHopCount`).  The proof:
    tools/task_alias.py --verify-cpp HEAD    # before committing: every
                                             # translation unit's cpp -P
                                             # output equals REF's
    tools/task_alias.py --verify-types       # gcc 12: every alias is used
                                             # on a struct Task
    make clean && make compare
`--verify-cpp REF` compares whitespace-collapsed `cpp -P -I include` output
of every src/*.c and tools/header_smoke*.c between REF and the tree (in the
knidl-builder image); REF must differ from the tree by aliases only (the
parent of an alias commit).  `--verify-types` compiles a copy of the tree in
which each aliased member of struct Task is an anonymous union of the member
and its aliases and the macros are gone: an alias used on any other struct
is then a gcc error the unmodified tree does not have.
`tools/rename.py --verify-diff REF` accepts the aliases too (renames.csv
rows of kind `alias`), so a branch of renames and aliases verifies in one
pass.
"""

import argparse
import csv
import os
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
VARS_H = os.path.join(ROOT, "include", "task_vars.h")
TASK_H = os.path.join(ROOT, "include", "task.h")
RENAMES = os.path.join(ROOT, "docs", "analysis", "renames.csv")
IMAGE = "knidl-builder"
WORK = os.path.join(ROOT, "build", "task_alias")

# struct Task's per-family registers, by offset.
REGISTERS = {
    "unk18": 0x18, "unk1C": 0x1C, "unk20": 0x20, "unk24": 0x24, "unk28": 0x28,
    "unk2C": 0x2C, "unk30": 0x30, "unk34": 0x34, "unk46": 0x46, "unk6C": 0x6C,
    "unk6E": 0x6E, "unk70": 0x70, "unk74": 0x74,
}
TYPES = ("s32", "u32", "s16", "u16", "s8", "u8")
ALIAS_RE = re.compile(r"^[a-z][A-Za-z0-9]*$")
FAMILY_RE = re.compile(r"^[A-Z][A-Za-z0-9]*$")
COMMENT_RE = re.compile(r"/\*.*?\*/|//[^\n]*", re.S)
IDENT_RE = re.compile(r"[A-Za-z_][A-Za-z0-9_]*")
DEFINE_RE = re.compile(r"^#define (\w+) (unk[0-9A-F]+) /\* (\w+): (.*) \*/$")
HEADER_START_RE = re.compile(r"^/\* [A-Z]\w* - ")
HEADER_RE = re.compile(r"^/\* (\w+) - (.*) \*/$")

PREAMBLE = """\
#ifndef GUARD_TASK_VARS_H
#define GUARD_TASK_VARS_H

/*
 * Per-family names of struct Task's registers (#155 run 5).
 *
 * unk18-unk34, unk46, unk6C-unk70 and unk74 hold a different thing in every
 * task family: a timer, a counter, a child's slot.  Like pret's
 * `#define tState data[0]` over struct Task's data[16], each family names
 * the registers it uses with object-like macros, and its code writes
 * `gCurTask->fireLionHopCount` for `gCurTask->unk6C`.  A macro changes no
 * token after preprocessing; it is used only on a pointer proven to hold a
 * task of its family, and shared helpers keep the plain unkXX.  Each line
 * gives the member, the type the code reads it as (the sites keep their
 * casts) and the role.  Generated and checked by tools/task_alias.py
 * (docs/header-conventions.md, "Per-family registers").
 */
"""
POSTAMBLE = "\n#endif // GUARD_TASK_VARS_H\n"


class AliasError(Exception):
    pass


def rel(path):
    return os.path.relpath(path, ROOT)


def read(path):
    with open(path, encoding="utf-8") as f:
        return f.read()


def write(path, text):
    with open(path, "w", encoding="utf-8") as f:
        f.write(text)


def blank_comments(text):
    return COMMENT_RE.sub(lambda m: re.sub(r"[^\n]", " ", m.group(0)), text)


def lcfirst(s):
    """The family word in lowerCamelCase: `FireLion` -> `fireLion`, and a
    leading acronym in lower case: `UFO` -> `ufo`, `UFOLaser` -> `ufoLaser`."""
    m = re.match(r"^([A-Z]+)(?=[A-Z][a-z]|$)", s)
    if m and len(m.group(1)) > 1:
        return m.group(1).lower() + s[len(m.group(1)):]
    return s[:1].lower() + s[1:]


# ---- include/task_vars.h --------------------------------------------------------

def parse_vars():
    """[(family, header, [(alias, field, type, role)])] in file order."""
    if not os.path.exists(VARS_H):
        return []
    blocks = []
    lines = read(VARS_H).split("\n")
    i = 0
    while i < len(lines):
        ln = lines[i]
        m = HEADER_START_RE.match(ln)
        if m:
            text = ln
            while "*/" not in text:
                i += 1
                text += "\n" + lines[i]
            m = HEADER_RE.match(" ".join(x.strip() for x in text.split("\n")))
            if not m:
                raise AliasError("%s: bad family header %r" % (rel(VARS_H), text))
            blocks.append((m.group(1), m.group(2), []))
        else:
            m = DEFINE_RE.match(ln)
            if m:
                if not blocks:
                    raise AliasError("%s: a #define before any family header" % rel(VARS_H))
                blocks[-1][2].append((m.group(1), m.group(2), m.group(3), m.group(4)))
            elif ln.startswith("#define") and not ln.startswith("#define GUARD_"):
                raise AliasError("%s: unexpected line %r" % (rel(VARS_H), ln))
        i += 1
    return blocks


def wrap(text, first, rest, width=78):
    out = []
    cur = first
    for w in text.split():
        if len(cur) + 1 + len(w) > width and cur.strip() not in ("", "/*"):
            out.append(cur)
            cur = rest + w
        else:
            cur = cur + (" " if cur and not cur.endswith(" ") else "") + w
    out.append(cur)
    return out


def render_vars(blocks):
    out = [PREAMBLE.rstrip("\n")]
    for family, header, defs in sorted(blocks, key=lambda b: b[0].lower()):
        out.append("")
        out.extend(wrap("%s - %s */" % (family, header), "/* ", "   "))
        for alias, field, typ, role in sorted(defs, key=lambda d: (REGISTERS[d[1]], d[0])):
            out.append("#define %s %s /* %s: %s */" % (alias, field, typ, role))
    return "\n".join(out) + "\n" + POSTAMBLE


# ---- validation -----------------------------------------------------------------

def tree_identifiers():
    """Identifiers in the code (comments excluded) of src/, include/, asm/."""
    ids = set()
    for top in ("src", "include", "asm"):
        for base, dirs, files in os.walk(os.path.join(ROOT, top)):
            for f in files:
                if f.endswith((".c", ".h", ".s", ".inc")):
                    ids.update(x.lower() for x in IDENT_RE.findall(
                        blank_comments(read(os.path.join(base, f)))))
    return ids


def validate(defs, sites, blocks):
    problems = []
    known = {d[0]: (fam, d[1]) for fam, _, ds in blocks for d in ds}
    families = {b[0] for b in blocks}
    ids = tree_identifiers()
    new = {}
    for r in defs:
        a, fam, field = r["alias"], r["family"], r["field"]
        if not FAMILY_RE.match(fam):
            problems.append("%s: family %r is not PascalCase" % (a, fam))
        if not ALIAS_RE.match(a):
            problems.append("%s: not lowerCamelCase" % a)
        if not a.startswith(lcfirst(fam)) or len(a) == len(fam):
            problems.append("%s: must start with %s and add a role" % (a, lcfirst(fam)))
        if field not in REGISTERS:
            problems.append("%s: %s is not a per-family register" % (a, field))
        if r["type"] not in TYPES:
            problems.append("%s: type %r" % (a, r["type"]))
        if not r["role"].strip() or "*/" in r["role"] or "\n" in r["role"] or len(r["role"]) > 90:
            problems.append("%s: role must be one line of at most 90 characters" % a)
        if not r.get("evidence", "").strip():
            problems.append("%s: no evidence" % a)
        if a in known or a in new:
            problems.append("%s: defined twice" % a)
        elif a.lower() in ids:
            problems.append("%s: already an identifier in the tree" % a)
        if fam not in families and not r.get("header", "").strip() and \
                not any(x["family"] == fam and x.get("header", "").strip() for x in defs):
            problems.append("%s: family %s is new and has no header" % (a, fam))
        new[a] = r
    for s in sites:
        a = s["alias"]
        field = new[a]["field"] if a in new else (known[a][1] if a in known else None)
        if field is None:
            problems.append("site %s:%s: alias %s is not defined" % (s["file"], s["function"], a))
        elif field != s["field"]:
            problems.append("site %s:%s: alias %s is %s, not %s" % (s["file"], s["function"], a, field, s["field"]))
    return problems


# ---- applying the sites ---------------------------------------------------------

def function_span(code, name):
    """(start, end) of the body of top-level function `name` (comments blanked)."""
    for m in re.finditer(r"(?m)^[^\n;{}#]*\b%s\s*\([^;{}]*\)\s*\{" % re.escape(name), code):
        # depth 0 only
        if code.count("{", 0, m.start()) - code.count("}", 0, m.start()) != 0:
            continue
        i = m.end()
        depth = 1
        while depth and i < len(code):
            if code[i] == "{":
                depth += 1
            elif code[i] == "}":
                depth -= 1
            i += 1
        return m.end() - 1, i
    return None


def apply_sites(sites, write_files):
    byfile = {}
    for s in sites:
        byfile.setdefault(s["file"], []).append(s)
    edits_total = 0
    problems = []
    changed = {}
    for path, rows in sorted(byfile.items()):
        full = os.path.join(ROOT, path)
        text = read(full)
        code = blank_comments(text)
        edits = {}
        for s in rows:
            span = function_span(code, s["function"])
            if span is None:
                problems.append("%s: no function %s" % (path, s["function"]))
                continue
            ptr = re.escape(s["pointer"])
            pat = re.compile(r"(?<![\w.>])%s\s*(?:->|\.)\s*(%s)(?![A-Za-z0-9_])" % (ptr, re.escape(s["field"])))
            n = 0
            for m in pat.finditer(code, span[0], span[1]):
                if m.start(1) in edits and edits[m.start(1)][1] != s["alias"]:
                    problems.append("%s:%s: %s->%s given two aliases" % (path, s["function"], s["pointer"], s["field"]))
                edits[m.start(1)] = (s["field"], s["alias"])
                n += 1
            if n == 0:
                problems.append("%s:%s: no %s->%s" % (path, s["function"], s["pointer"], s["field"]))
        for pos in sorted(edits, reverse=True):
            field, alias = edits[pos]
            text = text[:pos] + alias + text[pos + len(field):]
        edits_total += len(edits)
        changed[full] = text
    if problems:
        raise AliasError("\n".join(problems))
    if write_files:
        for full, text in changed.items():
            write(full, text)
    return edits_total, len(changed)


def run(defs_path, sites_path, do_write):
    with open(defs_path, encoding="utf-8") as f:
        defs = list(csv.DictReader(f))
    sites = []
    if sites_path:
        with open(sites_path, encoding="utf-8") as f:
            sites = list(csv.DictReader(f))
    blocks = parse_vars()
    problems = validate(defs, sites, blocks)
    if problems:
        raise AliasError("\n".join(problems))
    edits, files = apply_sites(sites, do_write)
    used = {s["alias"] for s in sites}
    unused = [r["alias"] for r in defs if r["alias"] not in used]
    if unused:
        raise AliasError("aliases with no site: %s" % ", ".join(unused))
    print("%d aliases, %d sites rows, %d accesses in %d files%s"
          % (len(defs), len(sites), edits, files, "" if do_write else " (dry run)"))
    if not do_write:
        return
    byfam = {b[0]: b for b in blocks}
    for r in defs:
        fam = r["family"]
        if fam not in byfam:
            header = r.get("header", "").strip() or next(
                x["header"].strip() for x in defs if x["family"] == fam and x.get("header", "").strip())
            byfam[fam] = (fam, header, [])
            blocks.append(byfam[fam])
        byfam[fam][2].append((r["alias"], r["field"], r["type"], r["role"].strip()))
    first = not os.path.exists(VARS_H)
    text = render_vars(blocks)
    write(VARS_H, text)
    norm = lambda bs: sorted((f, h, sorted(d)) for f, h, d in bs)
    if norm(parse_vars()) != norm(blocks):
        raise AliasError("%s does not parse back to what was written" % rel(VARS_H))
    if first:
        t = read(TASK_H)
        mark = "\n#endif // GUARD_TASK_H"
        if '#include "task_vars.h"' not in t:
            t = t.replace(mark, '\n#include "task_vars.h"\n' + mark, 1)
            write(TASK_H, t)
    with open(RENAMES, "a", newline="", encoding="utf-8") as f:
        w = csv.writer(f, lineterminator="\n")
        for r in defs:
            w.writerow(["Task." + r["field"], "Task." + r["alias"], "alias",
                        r["evidence"], r.get("issue") or "155"])
    print("wrote %s and %d renames.csv rows; now: tools/task_alias.py --verify-cpp HEAD, "
          "--verify-types, make clean && make compare" % (rel(VARS_H), len(defs)))


# ---- proofs ---------------------------------------------------------------------

CPP_SCRIPT = r"""
cd /w
for tree in a b; do
  for f in $(cd $tree && ls src/*.c src/data/*.c tools/header_smoke*.c 2>/dev/null); do
    o=/w/out/$tree/$(echo $f | tr / _)
    (cd $tree && cpp -P -I include "$f" 2>&1) | tr -s ' \t\n' '   ' > "$o"
  done
done
"""


def verify_cpp(ref):
    tmp = tempfile.mkdtemp(prefix="task_alias_cpp_", dir=os.path.join(ROOT, "build"))
    try:
        a = os.path.join(tmp, "a")
        b = os.path.join(tmp, "b")
        os.makedirs(a)
        os.makedirs(os.path.join(tmp, "out", "a"))
        os.makedirs(os.path.join(tmp, "out", "b"))
        arch = subprocess.run(["git", "archive", ref, "src", "include", "tools/header_smoke.c",
                               "tools/header_smoke_game.c"], cwd=ROOT, stdout=subprocess.PIPE)
        if arch.returncode:
            raise AliasError("git archive %s failed" % ref)
        subprocess.run(["tar", "-x", "-C", a], input=arch.stdout, check=True)
        os.makedirs(os.path.join(b, "tools"))
        for d in ("src", "include"):
            shutil.copytree(os.path.join(ROOT, d), os.path.join(b, d))
        for f in ("header_smoke.c", "header_smoke_game.c"):
            shutil.copy(os.path.join(ROOT, "tools", f), os.path.join(b, "tools", f))
        res = subprocess.run(["docker", "run", "--rm", "-v", "%s:/w" % tmp, IMAGE, "bash", "-c", CPP_SCRIPT],
                             stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True)
        if res.returncode:
            raise AliasError("cpp run failed: %s" % res.stdout[-2000:])
        oa = sorted(os.listdir(os.path.join(tmp, "out", "a")))
        ob = sorted(os.listdir(os.path.join(tmp, "out", "b")))
        problems = []
        if oa != ob:
            problems.append("translation units differ: %s" % sorted(set(oa) ^ set(ob)))
        same = 0
        for n in sorted(set(oa) & set(ob)):
            x = read(os.path.join(tmp, "out", "a", n))
            y = read(os.path.join(tmp, "out", "b", n))
            if x != y:
                problems.append("%s: cpp output differs" % n.replace("_", "/", 1))
            else:
                same += 1
        print("verify-cpp %s: %d translation units, %d identical after cpp -P" % (ref, len(ob), same))
        if problems:
            raise AliasError("\n".join(problems))
        print("verify-cpp: OK - every translation unit preprocesses to the same tokens")
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


GCC_SCRIPT = r"""
cd /w/%s && for f in $(ls src/*.c src/data/*.c); do gcc -fsyntax-only -std=gnu89 -w \
  -fdiagnostics-column-unit=byte -fno-diagnostics-show-caret -I include "$f"; done; true
"""
ERR_RE = re.compile(r"^([^:\n]+):(\d+):(\d+): error: (.*)$")


def gcc_errors(sub):
    res = subprocess.run(["docker", "run", "--rm", "-v", "%s:/w" % WORK, IMAGE, "bash", "-c", GCC_SCRIPT % sub],
                         stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True)
    return {m.groups() for m in map(ERR_RE.match, res.stdout.split("\n")) if m}


def verify_types():
    blocks = parse_vars()
    by_field = {}
    for _, _, ds in blocks:
        for alias, field, typ, role in ds:
            by_field.setdefault(field, []).append(alias)
    if os.path.exists(WORK):
        shutil.rmtree(WORK)
    for sub in ("base", "check"):
        for d in ("src", "include"):
            shutil.copytree(os.path.join(ROOT, d), os.path.join(WORK, sub, d))
    # check tree: no macros, the aliases are members of an anonymous union
    cv = os.path.join(WORK, "check", "include", "task_vars.h")
    if os.path.exists(cv):
        write(cv, "\n".join(ln for ln in read(cv).split("\n")
                            if not DEFINE_RE.match(ln)) + "\n")
    ct = os.path.join(WORK, "check", "include", "task.h")
    t = read(ct)
    s = t.index("struct Task\n{")
    e = t.index("\n};", s)
    body = t[s:e]
    for field, aliases in by_field.items():
        m = re.search(r"(?m)^(\s*/\*0x[0-9A-F]+\*/ )(\w+) %s;$" % field, body)
        if not m:
            raise AliasError("struct Task has no plain member %s" % field)
        typ = m.group(2)
        union = "union { %s %s; %s };" % (
            typ, field, " ".join("%s %s;" % (typ, a) for a in sorted(aliases)))
        body = body[:m.start()] + m.group(1) + union + body[m.end():]
    write(ct, t[:s] + body + t[e:])
    base = gcc_errors("base")
    check = gcc_errors("check")
    new = check - base
    gone = base - check
    n = sum(len(v) for v in by_field.values())
    print("verify-types: %d aliases on %d registers; gcc errors: %d before, %d with the aliases as members"
          % (n, len(by_field), len(base), len(check)))
    for e in sorted(new)[:40]:
        print("  NEW: %s:%s:%s: %s" % e)
    for e in sorted(gone)[:10]:
        print("  GONE: %s:%s:%s: %s" % e)
    if new or gone:
        raise AliasError("an alias is used where struct Task's member is not")
    print("verify-types: OK - every alias names a member of struct Task")


def rename_alias(old, new, family, header, typ, role, evidence, do_write):
    """Rename an applied alias, or merge it into an existing alias of the same
    member (a family alias that turned out to be a shared role)."""
    blocks = parse_vars()
    where = {d[0]: (b, d) for b in blocks for d in b[2]}
    if old not in where:
        raise AliasError("%s is not an alias" % old)
    ob, od = where[old]
    field = od[1]
    if new in where:
        if where[new][1][1] != field:
            raise AliasError("%s is %s, not %s" % (new, where[new][1][1], field))
        merge = True
    else:
        merge = False
        if not ALIAS_RE.match(new) or new.lower() in tree_identifiers():
            raise AliasError("%s: not a fresh lowerCamelCase identifier" % new)
        if not family:
            family = ob[0]
        if not new.startswith(lcfirst(family)):
            raise AliasError("%s must start with %s" % (new, lcfirst(family)))
    edits = 0
    changed = {}
    for top in ("src", "include"):
        for base, dirs, files in os.walk(os.path.join(ROOT, top)):
            for f in files:
                if not f.endswith((".c", ".h")) or f == "task_vars.h":
                    continue
                path = os.path.join(base, f)
                text = read(path)
                code = blank_comments(text)
                pos = [m.start(1) for m in re.finditer(r"(?:->|\.)\s*(%s)(?![A-Za-z0-9_])" % re.escape(old), code)]
                for p in sorted(pos, reverse=True):
                    text = text[:p] + new + text[p + len(old):]
                if pos:
                    edits += len(pos)
                    changed[path] = text
    print("%s -> %s (%s): %d accesses in %d files%s" % (old, new, "merge" if merge else "rename",
          edits, len(changed), "" if do_write else " (dry run)"))
    if not do_write:
        return
    ob[2].remove(od)
    if not ob[2]:
        blocks.remove(ob)
    if not merge:
        byfam = {b[0]: b for b in blocks}
        if family not in byfam:
            if not header:
                raise AliasError("family %s is new: give --header" % family)
            byfam[family] = (family, header, [])
            blocks.append(byfam[family])
        byfam[family][2].append((new, field, typ or od[2], role or od[3]))
    for path, text in changed.items():
        write(path, text)
    write(VARS_H, render_vars(blocks))
    with open(RENAMES, "a", newline="", encoding="utf-8") as f:
        csv.writer(f, lineterminator="\n").writerow(["Task." + old, "Task." + new, "alias", evidence, "155"])


def list_aliases():
    blocks = parse_vars()
    total = 0
    for fam, header, ds in sorted(blocks):
        total += len(ds)
        print("%-32s %3d  %s" % (fam, len(ds), " ".join(d[0] for d in ds)))
    print("%d aliases in %d families" % (total, len(blocks)))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--defs")
    ap.add_argument("--sites")
    ap.add_argument("--write", action="store_true")
    ap.add_argument("--verify-cpp", metavar="REF")
    ap.add_argument("--verify-types", action="store_true")
    ap.add_argument("--list", action="store_true")
    ap.add_argument("--rename", nargs=2, metavar=("OLD", "NEW"),
                    help="rename an applied alias, or merge it into an existing one of the same member")
    ap.add_argument("--family")
    ap.add_argument("--header")
    ap.add_argument("--type")
    ap.add_argument("--role")
    ap.add_argument("--evidence")
    args = ap.parse_args()
    try:
        if args.verify_cpp:
            verify_cpp(args.verify_cpp)
        elif args.verify_types:
            verify_types()
        elif args.list:
            list_aliases()
        elif args.rename:
            if not args.evidence:
                ap.error("--rename needs --evidence")
            rename_alias(args.rename[0], args.rename[1], args.family, args.header, args.type,
                         args.role, args.evidence, args.write)
        elif args.defs:
            run(args.defs, args.sites, args.write)
        else:
            ap.error("nothing to do")
    except AliasError as e:
        sys.exit("task_alias.py: %s" % e)


if __name__ == "__main__":
    main()
