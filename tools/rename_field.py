#!/usr/bin/env python3
"""rename_field.py - give a struct field a real name at every access, guided by a compiler.

Usage:
    tools/rename_field.py STRUCT OLD NEW --evidence TEXT [--copies] [--write]
    tools/rename_field.py --csv FILE [--write]

Dry run by default: it validates every rename, runs the whole procedure on a
copy of the tree under build/rename_field/ and prints what it would touch.
With --write it applies the result to src/, include/ and
tools/header_smoke.c and appends the renames to docs/analysis/renames.csv
(kind `field`, `old`/`new` written `Struct.field`).  A CSV has the header
`struct,old,new,evidence` and the optional columns `offset` (hex, checked),
`copies` (1 = also rename the differently-named member at the same offset
in the other definitions of the struct, see below) and `issue`.  Naming
rules: docs/naming.md (fields are camelCase).

A plain word replace is wrong: dozens of structs have an `unk14`.  So the
tool lets a compiler find the accesses:
  1. it finds every definition of `struct STRUCT` (or a typedef named
     STRUCT) in src/, include/ and tools/header_smoke.c, and in each one the
     member at the field's offset (the `/*0x14*/` comment; a definition
     without one is matched by name only);
  2. it renames the member there, then runs `gcc -fsyntax-only` (Debian's
     gcc 12, inside the knidl-builder image) over every translation unit
     and reads the errors "'struct STRUCT' has no member named 'OLD'",
     which carry a file, a line and a column, so two structs' `unk14` on
     one line are told apart;
  3. it renames exactly the member name those errors point at (a field
     access or a designated initializer; inside a macro body gcc points at
     the macro's own line), recompiles and repeats until no error names an
     old field;
  4. the final error set must equal the unmodified tree's (a handful of
     agbcc-only constructs gcc 12 rejects), so no access was renamed into a
     struct that lacks the new name.
agbcc is still the ground truth: after --write, `make clean && make
compare` must pass, and `tools/rename.py --verify-diff REF` checks the
branch token by token (field renames are accepted only after `.` / `->` or
at a member declarator inside a definition of their struct).

Local copies: several files declare their own copy of a shared struct with
other member names (src/task_init.c's `struct Task` calls Task.unk14
`b14`).  With `copies` the member at the same offset in every definition of
the struct is renamed too, whatever its old name, and each such copy is
logged as its own renames.csv row; a copy whose member at that offset is an
array or padding that spans more than the field is reported and left alone.
Without `copies` only members spelled OLD are renamed.

Comments: a qualified mention `STRUCT.OLD` in a comment of src/ or include/
becomes `STRUCT.NEW`.  Unqualified mentions (`task->unk14` in prose) and
docs/ are left alone: the lessons and the rom-map keep the names of their
time, and renames.csv maps them.
"""

import argparse
import csv
import os
import re
import shutil
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
RENAMES = os.path.join(ROOT, "docs", "analysis", "renames.csv")
WORK = os.path.join(ROOT, "build", "rename_field")
IMAGE = "knidl-builder"
TEXT_DIRS = ("src", "include")
TEXT_FILES = ("tools/header_smoke.c",)

IDENT_RE = re.compile(r"^[A-Za-z_][A-Za-z0-9_]*$")
FIELD_STYLE = re.compile(r"^[a-z][A-Za-z0-9]*$")
PLACEHOLDER_RE = re.compile(r"^(?:unk|filler|pad|sub_|gUnk_)", re.I)
C_KEYWORDS = set("""
auto break case char const continue default do double else enum extern float
for goto if inline int long register restrict return short signed sizeof
static struct switch typedef union unsigned void volatile while asm typeof
""".split())
COMMENT_RE = re.compile(r"/\*.*?\*/|//[^\n]*", re.S)
OFFSET_RE = re.compile(r"/\*\s*(0x[0-9A-Fa-f]+)\s*\*/")
ERR_RE = re.compile(
    r"^(?P<file>[^:\n]+):(?P<line>\d+):(?P<col>\d+): error: "
    r"(?P<msg>.*)$")
NOMEMBER_RE = re.compile(
    r"^'(?:(?:const|volatile) )*(?:(?:struct|union) )?(?P<a>\w+)'"
    r"(?: \{aka '(?:(?:const|volatile) )*(?:(?:struct|union) )?(?P<b>[\w ]+)'\})?"
    r" has no member named '(?P<field>\w+)'(?:; did you mean '\w+'\?)?$")
UNKNOWN_FIELD_RE = re.compile(r"^unknown field '(?P<field>\w+)' specified in initializer$")


# Local copies of a shared struct under another tag, renamed with `copies`
# like the same-tag copies: (file, tag) per shared struct.
ALIASES = {
    # task_draw_world.c's draw callbacks see the task block as `struct Sprite`.
    "Task": [("src/task_draw_world.c", "Sprite")],
}


class FieldError(Exception):
    pass


def rel(path):
    return os.path.relpath(path, ROOT)


def tree_files():
    out = []
    for d in TEXT_DIRS:
        for base, dirs, files in os.walk(os.path.join(ROOT, d)):
            dirs.sort()
            for f in sorted(files):
                if f.endswith((".c", ".h", ".inc")):
                    out.append(rel(os.path.join(base, f)))
    out.extend(TEXT_FILES)
    return out


def blank_comments(text):
    """Comments replaced by spaces (newlines kept), so offsets stay valid."""
    return COMMENT_RE.sub(lambda m: re.sub(r"[^\n]", " ", m.group(0)), text)


def line_of(text, pos):
    return text.count("\n", 0, pos) + 1


# ---- struct definitions --------------------------------------------------------

DEF_START_RE = re.compile(r"\b(struct|union)\s+(\w+)\s*\{|\btypedef\s+(struct|union)\s*(\w*)\s*\{")


def match_brace(code, pos):
    """Index of the `}` matching the `{` at pos."""
    depth = 0
    for i in range(pos, len(code)):
        c = code[i]
        if c == "{":
            depth += 1
        elif c == "}":
            depth -= 1
            if depth == 0:
                return i
    raise FieldError("unbalanced braces")


class Member:
    def __init__(self, name, pos, offset, array, stmt):
        self.name = name      # declarator identifier
        self.pos = pos        # its offset in the file text
        self.offset = offset  # from the /*0xNN*/ comment, or None
        self.array = array    # declared with [..] (spans more than one element)
        self.stmt = stmt


class Definition:
    def __init__(self, path, names, body_start, body_end, members):
        self.path = path
        self.names = names    # tag and/or typedef name
        self.body_start = body_start
        self.body_end = body_end
        self.members = members

    def member_at(self, offset):
        return [m for m in self.members if m.offset == offset]

    def member_named(self, name):
        return [m for m in self.members if m.name == name]


def parse_members(text, code, start, end):
    """Depth-1 member declarations of the body code[start:end]."""
    members = []
    depth = 0
    stmt_start = start
    i = start
    while i < end:
        c = code[i]
        if c in "{(":
            depth += 1
        elif c in "})":
            depth -= 1
        elif c == ";" and depth == 0:
            stmt = code[stmt_start:i]
            members.extend(declarators(text, code, stmt_start, i))
            stmt_start = i + 1
        i += 1
    return members


def declarators(text, code, s, e):
    stmt = code[s:e]
    if not stmt.strip() or "{" in stmt:
        return []  # nested struct/union bodies are not renamed here
    out = []
    # Function pointer: `void (*name)(args)`.
    m = re.search(r"\(\s*\*\s*(\w+)\s*\)", stmt)
    parts = []
    if m:
        parts.append((s + m.start(1), m.group(1), False))
    else:
        # Split at depth-0 commas; the declarator is the last identifier
        # before any `[...]` or `: bits`.
        depth, last = 0, 0
        pieces = []
        for j, ch in enumerate(stmt):
            if ch in "([":
                depth += 1
            elif ch in ")]":
                depth -= 1
            elif ch == "," and depth == 0:
                pieces.append((last, j))
                last = j + 1
        pieces.append((last, len(stmt)))
        for a, b in pieces:
            piece = stmt[a:b]
            head = re.split(r"[\[:]", piece, maxsplit=1)[0]
            ids = list(re.finditer(r"[A-Za-z_]\w*", head))
            if not ids:
                continue
            idm = ids[-1]
            parts.append((s + a + idm.start(), idm.group(0), "[" in piece))
    for pos, name, array in parts:
        # The offset comment sits on the declarator's line.
        ls = text.rfind("\n", 0, pos) + 1
        le = text.find("\n", pos)
        om = OFFSET_RE.search(text[ls:le if le >= 0 else len(text)])
        off = int(om.group(1), 16) if om else None
        out.append(Member(name, pos, off, array, stmt.strip()))
    return out


def find_definitions(contents, struct):
    out = []
    for path, text in contents.items():
        if struct not in text:
            continue
        code = blank_comments(text)
        for m in DEF_START_RE.finditer(code):
            brace = m.end() - 1
            close = match_brace(code, brace)
            names = set()
            if m.group(2):
                names.add(m.group(2))
            else:
                if m.group(4):
                    names.add(m.group(4))
                tm = re.match(r"\s*(\w+)\s*;", code[close + 1:])
                if tm:
                    names.add(tm.group(1))
            if struct not in names:
                continue
            members = parse_members(text, code, brace + 1, close)
            out.append(Definition(path, names, brace + 1, close, members))
    return out


# ---- the compiler loop ---------------------------------------------------------

def gcc_errors(files):
    """{(file, line, col, msg)} from gcc -fsyntax-only over the work tree."""
    tus = [f for f in files if f.endswith(".c")]
    script = ("cd /w && for f in %s; do gcc -fsyntax-only -std=gnu89 -w "
              "-fdiagnostics-column-unit=byte -fno-diagnostics-show-caret "
              "-I include \"$f\"; done; true" % " ".join(tus))
    res = subprocess.run(
        ["docker", "run", "--rm", "-v", "%s:/w" % WORK, IMAGE, "bash", "-c", script],
        stdout=subprocess.PIPE, stderr=subprocess.STDOUT, universal_newlines=True)
    errs = set()
    for ln in res.stdout.split("\n"):
        m = ERR_RE.match(ln)
        if m:
            errs.add((os.path.normpath(m.group("file")), int(m.group("line")),
                      int(m.group("col")), m.group("msg")))
    return errs


def write_work(contents):
    for path, text in contents.items():
        dst = os.path.join(WORK, path)
        os.makedirs(os.path.dirname(dst), exist_ok=True)
        with open(dst, "w", encoding="utf-8") as f:
            f.write(text)


def locate(text, line, col, field):
    """Offset of the member name `field` the error at line:col points at:
    gcc points at the `.`/`->` of an access or at the name of a designator."""
    ls = 0
    for _ in range(line - 1):
        ls = text.index("\n", ls) + 1
    p = ls + col - 1
    m = re.compile(r"(?:->|\.)?\s*(%s)(?![A-Za-z0-9_])" % re.escape(field)).match(text, p)
    if not m:
        raise FieldError("line %d col %d: no `%s` where gcc points" % (line, col, field))
    return m.start(1)


def run(rows, write):
    files = tree_files()
    contents = {}
    for p in files:
        with open(os.path.join(ROOT, p), encoding="utf-8") as f:
            contents[p] = f.read()
    # Plan: every (definition, member) to rename, per row.
    plan = []      # (path, pos, oldname, newname, row)
    log = []       # renames.csv rows
    wanted = {}    # (struct name, old member name) -> new
    for r in rows:
        struct, old, new = r["struct"], r["old"], r["new"]
        where = "%s.%s -> %s" % (struct, old, new)
        if not IDENT_RE.match(new) or new in C_KEYWORDS:
            raise FieldError("%s: %r is not a usable C identifier" % (where, new))
        if not FIELD_STYLE.match(new):
            raise FieldError("%s: fields are camelCase (docs/naming.md)" % where)
        if PLACEHOLDER_RE.match(new):
            raise FieldError("%s: %r uses a placeholder prefix" % (where, new))
        if not r["evidence"]:
            raise FieldError("%s: evidence is required" % where)
        defs = find_definitions(contents, struct)
        if not defs:
            raise FieldError("%s: no definition of %s found" % (where, struct))
        offs = set()
        for d in defs:
            for m in d.member_named(old):
                offs.add(m.offset)
        if r.get("offset"):
            off = int(r["offset"], 16)
            if offs and offs != {off}:
                raise FieldError("%s: %s sits at %s, not 0x%X" % (
                    where, old, ", ".join("?" if o is None else "0x%X" % o for o in offs), off))
        else:
            if len(offs) != 1:
                raise FieldError("%s: %s found at offsets %s; give `offset`" % (
                    where, old, sorted("?" if o is None else hex(o) for o in offs) or "none"))
            off = offs.pop()
        copies = {}   # (tag, old member name) -> [paths]
        cands = [(d, struct) for d in defs]
        if r.get("copies"):
            for path, tag in ALIASES.get(struct, ()):
                cands += [(d, tag) for d in find_definitions({path: contents[path]}, tag)]
        for d, tag in cands:
            if d.member_named(new):
                raise FieldError("%s: %s already has a member %s (%s)" % (
                    where, tag, new, d.path))
            hits = [m for m in d.member_named(old) if m.offset in (off, None)]
            if not hits and r.get("copies") and off is not None:
                at = d.member_at(off)
                if len(at) == 1 and not at[0].array and not re.match(r"(?:pad|filler)", at[0].name):
                    hits = at
                elif at:
                    print("note: %s: %s's member at 0x%X is %r; left alone" % (
                        d.path, tag, off, at[0].stmt))
                else:
                    print("note: %s: %s has no member at 0x%X (a wider member or "
                          "padding spans it); left alone" % (d.path, tag, off))
            for m in hits:
                plan.append((d.path, m.pos, m.name, new, r))
                for nm in d.names:
                    wanted[(nm, m.name)] = new
                copies.setdefault((tag, m.name), []).append(d.path)
        if (struct, old) not in copies:
            raise FieldError("%s: no definition has a member %s" % (where, old))
        for (tag, oldname), paths in sorted(copies.items(), key=lambda x: x[0] != (struct, old)):
            ev = r["evidence"]
            if (tag, oldname) != (struct, old):
                ev = ("local copy of %s.%s (offset 0x%X) in %s; %s" % (
                    struct, old, off, ", ".join(sorted(set(paths))), ev))
            log.append([tag + "." + oldname, tag + "." + new, "field", ev,
                        r.get("issue") or "155"])
    # Apply the definition edits (right to left within a file).
    edits = {}
    for path, pos, oldname, newname, _ in plan:
        edits.setdefault(path, []).append((pos, oldname, newname))
    new_contents = dict(contents)
    counts = {}

    def apply(path, items):
        text = new_contents[path]
        for pos, oldname, newname in sorted(items, reverse=True):
            if text[pos:pos + len(oldname)] != oldname:
                raise FieldError("%s: expected %s at %d" % (path, oldname, pos))
            text = text[:pos] + newname + text[pos + len(oldname):]
        new_contents[path] = text

    for path, items in edits.items():
        apply(path, items)
    if os.path.isdir(WORK):
        shutil.rmtree(WORK)
    write_work(contents)
    base = gcc_errors(files)
    base_keys = set((f, l, msg) for f, l, _, msg in base)
    write_work(new_contents)
    for it in range(1, 30):
        errs = gcc_errors(files)
        fixes = {}
        for f, l, c, msg in sorted(errs):
            m = NOMEMBER_RE.match(msg)
            if not m:
                continue
            field = m.group("field")
            names = [m.group("a")] + ([m.group("b")] if m.group("b") else [])
            new = tag = None
            for nm in names:
                nm = nm.split()[-1]
                if not new and (nm, field) in wanted:
                    new, tag = wanted[(nm, field)], nm
            if not new:
                continue
            text = new_contents[f]
            pos = locate(text, l, c, field)
            fixes.setdefault(f, set()).add((pos, field, new, tag))
        if not fixes:
            break
        for f, items in fixes.items():
            apply(f, [x[:3] for x in items])
            for _, field, new, tag in items:
                counts[(tag, field)] = counts.get((tag, field), 0) + 1
        write_work(new_contents)
    else:
        raise FieldError("no fixed point after 30 compiles")
    left = set((f, l, msg) for f, l, _, msg in errs) - base_keys
    gone = base_keys - set((f, l, msg) for f, l, _, msg in errs)
    if left or gone:
        for e in sorted(left)[:20]:
            print("  new error: %s:%d: %s" % e)
        for e in sorted(gone)[:20]:
            print("  error gone: %s:%d: %s" % e)
        raise FieldError("the renamed tree does not compile like the original "
                         "(%d new errors, %d gone)" % (len(left), len(gone)))
    # Qualified comment mentions: STRUCT.OLD -> STRUCT.NEW.
    comment_edits = 0
    for r in rows:
        for (nm, oldname), new in wanted.items():
            if nm != r["struct"] and nm not in [t for _, t in ALIASES.get(r["struct"], ())]:
                continue
            pat = re.compile(r"(?<![A-Za-z0-9_])%s\.%s(?![A-Za-z0-9_])" % (
                re.escape(nm), re.escape(oldname)))
            for path, text in new_contents.items():
                def sub_comment(m):
                    return pat.sub(nm + "." + new, m.group(0))
                t2 = COMMENT_RE.sub(sub_comment, text)
                if t2 != text:
                    comment_edits += len(pat.findall(text))
                    new_contents[path] = t2
    changed = [p for p in files if new_contents[p] != contents[p]]
    for row in log:
        o, n = row[0], row[1]
        uses = counts.get(tuple(o.split(".", 1)), 0)
        print("field  %-28s -> %-28s %4d accesses" % (o, n, uses))
    print("files: %d; %d compiles; %d comment mentions" % (len(changed), it + 1, comment_edits))
    if not write:
        print("dry run: nothing written (use --write)")
        return
    for p in changed:
        with open(os.path.join(ROOT, p), "w", encoding="utf-8") as f:
            f.write(new_contents[p])
    with open(RENAMES, "a", encoding="utf-8", newline="") as f:
        w = csv.writer(f, lineterminator="\n")
        for row in log:
            w.writerow(row)
    print("wrote %d field renames; now run: make clean && make compare && "
          "tools/rename.py --verify-diff <ref>" % len(log))


def main():
    ap = argparse.ArgumentParser(
        description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("struct", nargs="?")
    ap.add_argument("old", nargs="?")
    ap.add_argument("new", nargs="?")
    ap.add_argument("--csv", help="batch: struct,old,new,evidence[,offset,copies,issue]")
    ap.add_argument("--evidence")
    ap.add_argument("--offset", help="the field's offset (hex), checked")
    ap.add_argument("--copies", action="store_true",
                    help="also rename the member at the same offset in local copies")
    ap.add_argument("--issue", default="155")
    ap.add_argument("--write", action="store_true")
    args = ap.parse_args()
    if args.csv:
        with open(args.csv, encoding="utf-8") as f:
            rows = [dict((k, (v or "").strip()) for k, v in r.items())
                    for r in csv.DictReader(f)
                    if (r.get("struct") or "").strip() and not r["struct"].lstrip().startswith("#")]
        for r in rows:
            r["copies"] = r.get("copies") in ("1", "yes", "true")
            r.setdefault("issue", args.issue)
    else:
        if not (args.struct and args.old and args.new):
            ap.error("STRUCT OLD NEW are required without --csv")
        rows = [{"struct": args.struct, "old": args.old, "new": args.new,
                 "evidence": (args.evidence or "").strip(), "offset": args.offset or "",
                 "copies": args.copies, "issue": args.issue}]
    seen = set()
    for r in rows:
        k = (r["struct"], r["old"])
        if k in seen:
            sys.exit("rename_field.py: error: %s.%s is renamed twice" % k)
        seen.add(k)
    try:
        run(rows, args.write)
    except FieldError as e:
        sys.exit("rename_field.py: error: %s" % e)


if __name__ == "__main__":
    main()
