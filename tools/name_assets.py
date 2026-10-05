#!/usr/bin/env python3
"""name_assets.py - name the asset labels after the record that owns them
(issue #183, docs/naming.md section 2.5).

Usage:
    tools/name_assets.py                       # dry run: counts per rule, owner, batch
    tools/name_assets.py --list                # every proposal and every reason left
    tools/name_assets.py --batch frames --csv b.csv   # a tools/rename.py batch
    tools/name_assets.py --batch player --csv b.csv --issue 186
    tools/name_assets.py --root gBonkersFrames --list # one owner's chain

The labels of the asset segments (`"asset": true` in tools/split_config.json)
name graphics, palettes, OAM template streams, maps and tile frames that no
code reads: the code passes an owning record's pointer to a loader.  The
tool reads the committed tree only (no build, no baserom) and builds the
reference graph with offsets:

  * every symbolic `.word` of data/*.s, at its byte offset inside its label;
  * every leaf of the C initializers of src/ (a slot index, a designated or
    positional struct member), and every C function and asm label that
    mentions a label (code);
  * the referrer's C type: its definition or extern declaration, else the
    pointee of the typed table element that points at it, else a word array
    for a pointer list of split_config.json; the struct layouts come from the
    `/*0xNN*/` offset comments of include/ and src/ (else natural alignment).
    The player's frame records are declared struct TaskGfxExtended
    (src/data/frame_tables.c, #186), so their words at +0xC and +0x10 are
    the fields nextBankPalette and upperTiles.

A word of a `"proof": "format"` pointer table (docs/data.md 5.3) is format
only.  Every asset placeholder is then decided once all its referrers are
settled, so a chain is named from its owner down (a frame table -> its
TaskGfx record -> the record's OAM template, palette and tiles), by the
rules A1-A5 and Q1-Q7 of docs/naming.md 2.5; what is left gets a reason
(`shared`, `via-unnamed`, `via-unnamed-field`, `positional-table`,
`no-owner`, or `by-consumer` for a label only one named function's code
mentions, whose docs/analysis/unnamed.csv row gives the reason), which
tools/audit.py's census uses (census()).  The tool proposes; the
names are applied with tools/rename.py (kind `asset`).
"""

import argparse
import collections
import csv
import importlib.util
import json
import os
import re
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))


def _load(name):
    spec = importlib.util.spec_from_file_location(name, os.path.join(ROOT, "tools", name + ".py"))
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


audit = _load("audit")
read = audit.read
files_under = audit.files_under
strip_c = audit.strip_c

LABEL_RE = re.compile(r"^([A-Za-z_][\w.$]*):")
INCBIN_RE = re.compile(r'^\s*\.incbin\s+"baserom\.gba",\s*(0x[0-9A-Fa-f]+),\s*(0x[0-9A-Fa-f]+)')
WORD_RE = re.compile(r"^\s*\.word\s+([A-Za-z_]\w*)(\+1)?\s*(?:@.*)?$")
SECTION_RE = re.compile(r"\s*\.section\s+\.(\w+)")
IDENT_RE = re.compile(r"\b[A-Za-z_]\w*\b")

BASE_SIZES = {"u8": 1, "s8": 1, "char": 1, "bool8": 1, "u16": 2, "s16": 2, "short": 2,
              "u32": 4, "s32": 4, "int": 4, "long": 4, "bool32": 4, "vu32": 4, "vu16": 2,
              "vu8": 1, "fixed": 4, "f32": 4, "uintptr_t": 4}


# ---------------------------------------------------------------------------
# the tree: labels, symbolic words, declarations, struct layouts

def load_config():
    return json.loads(read("tools/split_config.json"))


def label_addresses(cfg):
    addr = {v: int(k, 16) for k, v in cfg["data_symbols"].items()}
    for k, v in cfg.get("extra_labels", {}).items():
        addr[v] = int(k, 16)
    for line in read("docs/analysis/segments.txt").split("\n"):
        f = line.split("#", 1)[0].split()
        if len(f) >= 4:
            addr.setdefault(f[3], int(f[0], 16))
    return addr


def format_ranges(cfg):
    """[start, end) of every pointer_tables entry with "proof": "format"
    (docs/data.md 5.3): words no code reads."""
    out = []
    for t in cfg["pointer_tables"]:
        if t.get("proof") != "format":
            continue
        s = int(t["start"], 16)
        if "end" in t:
            e = int(t["end"], 16)
        else:
            e = s + t.get("count", 1) * t.get("stride", 4)
        out.append((s, e))
    return sorted(out)


class Edge(object):
    __slots__ = ("ref", "kind", "slot", "target", "fmt", "where")

    def __init__(self, ref, kind, slot, target, fmt=False, where=""):
        self.ref = ref          # referrer: a record label, a C record or a function
        self.kind = kind        # "record" or "function"
        self.slot = slot        # ("off", n) in a data/*.s record, ("path", (...)) in C
        self.target = target
        self.fmt = fmt          # a format-only word (no code reads it)
        self.where = where      # file

    def __repr__(self):
        return "Edge(%s %s %r -> %s%s)" % (self.kind, self.ref, self.slot, self.target,
                                          " fmt" if self.fmt else "")


def data_labels_and_words(cfg):
    """labels {name: (addr, seg, file)} of data/*.s and their symbolic words
    as Edges with byte offsets."""
    addr_of = label_addresses(cfg)
    fmts = format_ranges(cfg)
    labels = {}
    edges = []

    def is_fmt(a):
        lo, hi = 0, len(fmts)
        while lo < hi:
            mid = (lo + hi) // 2
            if fmts[mid][1] <= a:
                lo = mid + 1
            else:
                hi = mid
        return lo < len(fmts) and fmts[lo][0] <= a < fmts[lo][1]

    for path in sorted(files_under("data", (".s",))):
        seg = os.path.splitext(os.path.basename(path))[0]
        cur = cur_addr = addr = None
        for line in read(path).split("\n"):
            m = SECTION_RE.match(line)
            if m:
                seg = m.group(1)
                continue
            m = LABEL_RE.match(line)
            if m:
                name = m.group(1)
                a = addr_of.get(name, addr)
                if name == seg:
                    addr = a
                    continue
                cur, cur_addr, addr = name, a, a
                labels[name] = (a, seg, path)
                continue
            m = INCBIN_RE.match(line)
            if m:
                addr = 0x08000000 + int(m.group(1), 16) + int(m.group(2), 16)
                continue
            m = WORD_RE.match(line)
            if m:
                if cur is not None:
                    edges.append(Edge(cur, "record", ("off", addr - cur_addr), m.group(1),
                                      is_fmt(addr), path))
                addr += 4
    return labels, edges


# ---- C declarations ---------------------------------------------------------

class CType(object):
    """base (`struct X`, `u32`...), pointer depth, array dims (None = [])"""
    __slots__ = ("base", "ptr", "dims")

    def __init__(self, base, ptr, dims):
        self.base, self.ptr, self.dims = base, ptr, dims

    def __repr__(self):
        return "%s%s%s" % (self.base, "*" * self.ptr, "".join("[%s]" % (d or "") for d in self.dims))


DECL_HEAD_RE = re.compile(
    r"^\s*(?:extern\s+|static\s+)?(?:const\s+|volatile\s+)*((?:struct|union)\s+\w+|\w+)\s*(?:const\s+)*")


def parse_declarators(rest):
    """yield (name, ptr, dims) of a comma-separated declarator list"""
    depth = 0
    parts, cur = [], ""
    for ch in rest:
        if ch in "([{":
            depth += 1
        elif ch in ")]}":
            depth -= 1
        if ch == "," and depth == 0:
            parts.append(cur)
            cur = ""
        else:
            cur += ch
    parts.append(cur)
    for p in parts:
        p = p.split("=", 1)[0]
        p = re.sub(r"__attribute__\s*\(\(.*?\)\)", " ", p)
        p = re.sub(r"\b[A-Z][A-Z0-9_]*\s*\([^)]*\)", " ", p)  # section macros
        p = re.sub(r"\b[A-Z][A-Z0-9_]+\b", " ", p)
        if "(" in p:
            continue  # a function or a function pointer
        m = re.match(r"^\s*((?:\*\s*(?:const\s*)?)*)\s*([A-Za-z_]\w*)\s*((?:\[[^\]]*\]\s*)*)\s*$", p)
        if not m:
            continue
        ptr = m.group(1).count("*")
        dims = [d.strip() or None for d in re.findall(r"\[([^\]]*)\]", m.group(3))]
        yield m.group(2), ptr, dims


def top_statements(text):
    """yield the depth-0 statements of a C text (comments stripped), with a
    brace body kept (initializers and function bodies)."""
    depth = 0
    start = 0
    for i, ch in enumerate(text):
        if ch == "{":
            depth += 1
        elif ch == "}":
            depth -= 1
            if depth == 0:
                # a function body ends a statement; an initializer ends at `;`
                j = i + 1
                while j < len(text) and text[j] in " \t\n":
                    j += 1
                if j < len(text) and text[j] != ";" and text[j] != ",":
                    yield text[start:i + 1]
                    start = i + 1
        elif ch == ";" and depth == 0:
            yield text[start:i]
            start = i + 1


def declarations():
    """{name: [CType, ...]} of every object declared or defined in src/ and
    include/ (extern declarations and initialised definitions)."""
    out = {}
    for top in ("include", "src"):
        for path in files_under(top, (".h", ".c")):
            text = strip_c(read(path))
            text = re.sub(r"^\s*#.*$", "", text, flags=re.M)
            for st in top_statements(text):
                head = st.split("{", 1)[0] if "=" not in st.split("{", 1)[0] else st.split("=", 1)[0]
                if "(" in head.split("[", 1)[0] and "__attribute__" not in head and not re.search(r"\b[A-Z][A-Z0-9_]*\s*\(", head):
                    continue
                m = DECL_HEAD_RE.match(head)
                if not m:
                    continue
                base = re.sub(r"\s+", " ", m.group(1))
                if base in ("typedef", "return", "if", "else", "while", "for", "do", "switch"):
                    continue
                rest = head[m.end():]
                for name, ptr, dims in parse_declarators(rest):
                    out.setdefault(name, []).append((CType(base, ptr, dims), path))
    return out


# ---- struct layouts ---------------------------------------------------------

FIELD_RE = re.compile(r"^\s*(?:/\*0x([0-9A-Fa-f]+)\*/\s*)?((?:const\s+|volatile\s+)*(?:struct\s+\w+|union\s+\w+|\w+))\s*((?:\*\s*)*)(\w+)\s*((?:\[[^\]]*\]\s*)*)(?::\s*\d+)?\s*;")


def struct_layouts():
    """{struct name: [(offset, field, size)]} of the structs defined in
    include/*.h and src/*.c, from the `/*0xNN*/` offset comments where
    present, else computed (natural alignment)."""
    out = {}
    for top in ("include", "src"):
        for path in files_under(top, (".h", ".c")):
            raw = read(path)
            for m in re.finditer(r"\bstruct\s+(\w+)\s*\{", raw):
                name = m.group(1)
                i = m.end()
                depth = 1
                j = i
                while j < len(raw) and depth:
                    if raw[j] == "{":
                        depth += 1
                    elif raw[j] == "}":
                        depth -= 1
                    j += 1
                body = raw[i:j - 1]
                if "{" in body:
                    continue  # nested unions/structs: not needed here
                fields = []
                off = 0
                for line in body.split("\n"):
                    line = re.sub(r"//.*", "", line)
                    fm = FIELD_RE.match(re.sub(r"/\*(?!0x)[^*]*\*/", "", line))
                    if not fm:
                        continue
                    typ = fm.group(2).replace("const ", "").replace("volatile ", "").strip()
                    ptr = fm.group(3).count("*")
                    dims = re.findall(r"\[([^\]]*)\]", fm.group(5))
                    if ptr:
                        size = align = 4
                    elif typ in BASE_SIZES:
                        size = align = BASE_SIZES[typ]
                    else:
                        size = align = None
                    n = 1
                    for d in dims:
                        try:
                            n *= int(d, 0) if d.strip() else 0
                        except ValueError:
                            n = None
                            break
                    if fm.group(1):
                        off = int(fm.group(1), 16)
                    elif align and off is not None:
                        off = (off + align - 1) // align * align
                    else:
                        off = None
                    total = size * n if (size is not None and n is not None) else None
                    fields.append((off, fm.group(4), total))
                    if off is not None and total is not None:
                        off += total
                    else:
                        off = None
                if fields and name not in out:
                    out[name] = fields
    return out


def field_at(layouts, struct, off):
    """the field of `struct` that starts at byte `off`, or None"""
    for o, f, size in layouts.get(struct, ()):
        if o == off:
            return f
    return None


def struct_size(layouts, struct):
    fl = layouts.get(struct)
    if not fl:
        return None
    o, f, s = fl[-1]
    if o is None or s is None:
        return None
    end = o + s
    return (end + 3) // 4 * 4 if any(sz == 4 for _, _, sz in fl) else end


# ---- C initializers ---------------------------------------------------------

def c_initializer_leaves(body):
    """yield (path, expr) for every leaf of a brace initializer body (the
    text from its `{` to its `}`); path is a tuple of ints (positional) or
    `.field` strings (designated)."""
    stack = []  # [index, designator]
    path_stack = []
    cur = ""
    pending = None
    i = 0
    n = len(body)

    def flush():
        nonlocal cur, pending
        e = cur.strip()
        if e:
            m = re.match(r"^\.(\w+)\s*=\s*(.*)$", e, re.S)
            key = None
            if m:
                key = "." + m.group(1)
                e = m.group(2).strip()
            if stack:
                if key is not None:
                    stack[-1][1] = key
                p = tuple(path_stack) + (stack[-1][1] if stack[-1][1] is not None else stack[-1][0],)
                if e:
                    yield_list.append((p, e))
        cur = ""

    yield_list = []
    while i < n:
        ch = body[i]
        if ch == "{":
            pre = cur.strip()
            key = None
            m = re.match(r"^\.(\w+)\s*=\s*$", pre)
            if m:
                key = "." + m.group(1)
            if stack:
                if key is not None:
                    stack[-1][1] = key
                path_stack.append(stack[-1][1] if stack[-1][1] is not None else stack[-1][0])
            stack.append([0, None])
            cur = ""
        elif ch == "}":
            for _ in flush() or ():
                pass
            stack.pop()
            if path_stack:
                path_stack.pop()
            cur = ""
            # the `,` after a closing brace advances the parent
            j = i + 1
            while j < n and body[j] in " \t\n":
                j += 1
            if stack and j < n and body[j] == ",":
                stack[-1][0] += 1
                stack[-1][1] = None
                i = j
        elif ch == ",":
            for _ in flush() or ():
                pass
            if stack:
                stack[-1][0] += 1
                stack[-1][1] = None
        else:
            cur += ch
        i += 1
    return yield_list


def c_records():
    """{name: (CType, file, [(path, expr)])} of every initialised C record in
    src/ and include/"""
    out = {}
    for top in ("src", "include"):
        for path in files_under(top, (".c", ".h")):
            text = strip_c(read(path))
            text = re.sub(r"^\s*#.*$", "", text, flags=re.M)
            for st in top_statements(text):
                if "=" not in st.split("{", 1)[0]:
                    continue
                head, _, init = st.partition("=")
                init = init.strip()
                if not init.startswith("{"):
                    continue
                m = DECL_HEAD_RE.match(head)
                if not m:
                    continue
                base = re.sub(r"\s+", " ", m.group(1))
                decl = list(parse_declarators(head[m.end():]))
                if len(decl) != 1:
                    continue
                name, ptr, dims = decl[0]
                out[name] = (CType(base, ptr, dims), path, c_initializer_leaves(init))
    return out


# ---------------------------------------------------------------------------
# the model

PLACEHOLDER_RE = re.compile(r"^(gUnk_|sub_)")


def is_placeholder(name):
    return name is None or bool(PLACEHOLDER_RE.match(name))


def camel_words(name):
    return re.findall(r"[A-Z]+(?=[A-Z][a-z]|[0-9]|$)|[A-Z][a-z]*|[0-9]+", name)


def pascal(field):
    return field[0].upper() + field[1:]


def renames_rows():
    path = os.path.join(ROOT, "docs/analysis/renames.csv")
    with open(path, encoding="utf-8") as f:
        return list(csv.DictReader(f))


SLOT_EV_RE = re.compile(r"^slot:\s*([A-Za-z_]\w*)\[(\d+)\]")


class Model(object):
    def __init__(self):
        self.cfg = load_config()
        self.labels, data_edges = data_labels_and_words(self.cfg)
        self.assets = audit.asset_segments()
        self.decls = declarations()
        self.layouts = struct_layouts()
        self.crecords = c_records()
        self.rows = renames_rows()
        self.positions = set(r["new"] for r in self.rows
                             if r["evidence"].startswith("slot:") and r["kind"] != "asset")
        # a position record's table and slot (renames.csv `slot: gX[k]`)
        self.slot_of = {}
        for r in self.rows:
            m = SLOT_EV_RE.match(r["evidence"])
            if m and r["kind"] != "asset":
                self.slot_of[r["new"]] = (m.group(1), int(m.group(2)))
        self.position_tables = set(t for t, k in self.slot_of.values())
        known = set(self.labels) | set(self.crecords)
        self.edges = list(data_edges)
        # C records (src/data/ and the module-local tables)
        for name, (ctype, path, leaves) in self.crecords.items():
            for lpath, expr in leaves:
                ids = [w for w in IDENT_RE.findall(expr) if w in known and w != name]
                for w in ids:
                    self.edges.append(Edge(name, "record", ("path", lpath), w, False, path))
        # code: C function bodies and asm functions
        self.functions = set()
        for top in ("src", "include"):
            for path in files_under(top, (".c", ".h")):
                for name, kind, body in audit.c_definitions(strip_c(read(path))):
                    if kind != "function":
                        continue
                    self.functions.add(name)
                    for w in set(IDENT_RE.findall(body)) & known:
                        self.edges.append(Edge(name, "function", None, w, False, path))
        for path in files_under("asm", (".s",)):
            cur = None
            for line in read(path).split("\n"):
                m = LABEL_RE.match(line)
                if m:
                    cur = m.group(1)
                    continue
                if line.lstrip().startswith((".global", ".globl", ".type", ".size")):
                    continue
                for w in set(IDENT_RE.findall(line.split("@", 1)[0])) & known:
                    if w != cur:
                        self.edges.append(Edge(cur, "function", None, w, False, path))
        self._types = {}
        self._fam = {}
        self._fft = None
        # pointer lists of split_config.json (no stride: every word a pointer)
        starts = sorted(a for a, seg, p in self.labels.values())
        import bisect
        self.ptr_lists = []
        for t in self.cfg["pointer_tables"]:
            if "stride" in t or "pointers" in t:
                continue
            st = int(t["start"], 16)
            if "end" in t:
                en = int(t["end"], 16)
            elif "count" in t:
                en = st + 4 * t["count"]
            else:
                k = bisect.bisect_right(starts, st)
                en = starts[k] if k < len(starts) else st + 4
            self.ptr_lists.append((st, en, 4))
        self.by_target = collections.defaultdict(list)
        for e in self.edges:
            if e.target != e.ref:
                self.by_target[e.target].append(e)
        # identities: Task_<X> bodies (docs/naming.md 2.3)
        cfgsyms = _load("symdb")
        names = set(cfgsyms.KNOWN_SYMBOLS.values())
        self.identities = set(n[5:] for n in names if n.startswith("Task_"))

    # ---- types and slots -----------------------------------------------------

    def ctype(self, name):
        """the record's C type: its definition or extern declaration, else
        the pointee of the typed table element that points at it, else an
        array of words for a pointer table (split_config.json), else None"""
        if name in self._types:
            return self._types[name]
        t = None
        if name in self.crecords:
            t = self.crecords[name][0]
        elif self.decls.get(name):
            t = self.decls[name][0][0]
        else:
            for e in self.by_target.get(name, ()):
                if e.kind != "record" or e.ref == name:
                    continue
                ot = self.decls.get(e.ref, [(None,)])[0][0] if e.ref not in self.crecords else self.crecords[e.ref][0]
                if ot is not None and ot.dims and ot.ptr >= 1 and len(ot.dims) == 1:
                    t = CType(ot.base, ot.ptr - 1, [None] if not ot.base.startswith("struct") or ot.ptr > 1 else [])
                    break
            if t is None and name in self.labels and self.in_pointer_list(name):
                t = CType("u32", 0, [None])
        self._types[name] = t
        return t

    def in_pointer_list(self, name):
        a = self.labels[name][0]
        for s, e, stride in self.ptr_lists:
            if s <= a < e:
                return True
        return False

    def slot(self, e):
        """what the referrer's word is: ("slot", k), ("slot2", k, j),
        ("field", f), ("slot-field", k, f), ("nofield", desc), ("code",)"""
        if e.kind == "function":
            return ("code",)
        t = self.ctype(e.ref)
        kind, where = e.slot
        if kind == "path":
            p = where
            if t is None:
                return ("nofield", "untyped")
            is_struct = t.base.startswith("struct ") and t.ptr == 0
            if t.dims:
                if is_struct:
                    if len(p) >= 2:
                        f = p[1][1:] if isinstance(p[1], str) else self.field_by_index(t.base[7:], p[1])
                        return ("slot-field", p[0], f, t.base[7:]) if f else ("nofield", "slot %s member %s" % (p[0], p[1]))
                    return ("nofield", "path %r" % (p,))
                if len(t.dims) == 1 and len(p) == 1:
                    return ("slot", p[0])
                if len(t.dims) == 2 and len(p) == 2:
                    return ("slot2", p[0], p[1])
                return ("nofield", "path %r" % (p,))
            if is_struct:
                if len(p) == 1:
                    f = p[0][1:] if isinstance(p[0], str) else self.field_by_index(t.base[7:], p[0])
                    return ("field", f, t.base[7:]) if f else ("nofield", "member %s" % p[0])
            return ("nofield", "path %r" % (p,))
        off = where
        if t is None:
            return ("nofield", "untyped +0x%X" % off)
        if t.base.startswith("struct ") and t.ptr == 0:
            sname = t.base[7:]
            size = struct_size(self.layouts, sname)
            if t.dims:
                if not size:
                    return ("nofield", "unsized %s" % sname)
                f = field_at(self.layouts, sname, off % size)
                return ("slot-field", off // size, f, sname) if f else ("nofield", "+0x%X" % off)
            f = field_at(self.layouts, sname, off)
            return ("field", f, sname) if f else ("nofield", "%s +0x%X" % (sname, off))
        if t.dims:
            elem = 4 if t.ptr else BASE_SIZES.get(t.base, 4)
            k = off // elem
            if len(t.dims) == 2 and t.dims[1]:
                try:
                    m = int(t.dims[1], 0)
                except ValueError:
                    return ("nofield", "dims %r" % t.dims)
                return ("slot2", k // m, k % m)
            return ("slot", k)
        return ("nofield", "%r +0x%X" % (t, off))

    def field_by_index(self, sname, i):
        fl = self.layouts.get(sname)
        if not fl or i >= len(fl):
            return None
        return fl[i][1]

    def asset_labels(self):
        return sorted(n for n, (a, seg, p) in self.labels.items() if seg in self.assets)



    # ---- naming --------------------------------------------------------------

    def used_names(self):
        used = set(self.labels) | set(self.crecords) | set(self.functions) | set(self.decls)
        used.update(self.cfg["data_symbols"].values())
        used.update(_load("symdb").KNOWN_SYMBOLS.values())
        return set(n.lower() for n in used)

    def propose(self, fmt_policy="consumer-first"):
        """({old: proposal}, {old: (reason, note)}) for the asset placeholders.
        A label is decided once every referrer it has is settled (named, or
        decided, or not an asset placeholder), so a chain is named from its
        owner down."""
        cur = {}       # old -> new name
        props = {}     # old -> proposal
        reasons = {}
        used = self.used_names()
        targets = [n for n in self.asset_labels() if is_placeholder(n)]
        tset = set(targets)
        settled = set()

        def name(n):
            return cur.get(n, n)
        self._name = name

        def chain(n):
            return props[n]["chain"] if n in props else name(n)

        pending = list(targets)
        while pending:
            ready = [t for t in pending
                     if all(e.ref not in tset or e.ref in settled for e in self.by_target.get(t, ()))]
            if not ready:
                ready = pending
            rset = set(ready)
            new = {}
            for t in ready:
                d = self.decide(t, name, chain, fmt_policy)
                if d[0] == "name":
                    new[t] = d[1]
                else:
                    reasons[t] = d[1:]
            byname = collections.defaultdict(list)
            for t, pr in new.items():
                byname[pr["new"].lower()].append(t)
            for low, ts in byname.items():
                if len(ts) > 1 or low in used:
                    for t in ts:
                        del new[t]
                        reasons[t] = ("collision", "%s: also %s" % (low, ", ".join(ts)))
            for t, pr in new.items():
                cur[t] = pr["new"]
                props[t] = pr
                used.add(pr["new"].lower())
            settled |= rset
            pending = [t for t in pending if t not in rset]
        return props, reasons

    run = propose

    def census(self):
        """(unapplied, reasons) for the audit: the asset placeholders the
        rules would still name ({old: proposal}, empty in a consistent tree)
        and the reason of every other asset placeholder ({old: (code,
        note)})"""
        return self.propose()

    # ---- the rules -----------------------------------------------------------

    # family: the table whose slots hold a record (A3)
    def family(self, old):
        """(table, lowest slot) when every consumer word that points at
        record `old` is a slot of one table, else (old, -1); old names"""
        if old in self._fam:
            return self._fam[old]
        es = [e for e in self.by_target.get(old, ()) if not e.fmt and e.kind == "record"]
        r = (old, -1)
        if es and len(set(e.ref for e in es)) == 1:
            ss = [self.slot(e) for e in es]
            if all(x[0] in ("slot", "slot2") for x in ss):
                r = (es[0].ref, min(x[1] for x in ss))
        self._fam[old] = r
        return r

    def family_field_targets(self, fam, field):
        """every target of `field` in the records of family `fam`"""
        if self._fft is None:
            self._fft = collections.defaultdict(set)
            for e in self.edges:
                if e.kind != "record" or e.fmt:
                    continue
                s = self.slot(e)
                if s[0] == "field":
                    self._fft[(self.family(e.ref)[0], s[1])].add(e.target)
        return self._fft.get((fam, field), set())

    def cmd_op(self, script, k):
        """the op of command k of a BG animation script (C record)"""
        rec = self.crecords.get(script)
        if not rec:
            return None
        for path, expr in rec[2]:
            if path == (k, 0):
                try:
                    return int(expr, 0)
                except ValueError:
                    return None
        return None

    def decide(self, t, name, chain, fmt_policy):
        es = self.by_target.get(t, [])
        if not es:
            return ("reason", "no-owner", "nothing points at it")
        use = [e for e in es if not e.fmt]
        fmt = [e for e in es if e.fmt]
        fmt_only = False
        if fmt_policy == "count" or not use:
            fmt_only = bool(fmt) and not use
            use = es
            fmt = []
        code = [e for e in use if e.kind == "function"]
        recs = [e for e in use if e.kind == "record"]
        if code and not recs:
            fns = sorted(set(name(e.ref) for e in code))
            if len(fns) > 1:
                return ("reason", "shared", "code only, several functions: " + ", ".join(fns))
            if is_placeholder(fns[0]):
                return ("reason", "no-owner", "code only, a placeholder function: " + fns[0])
            # one named consumer: named by the kind its call proves, else its
            # docs/analysis/unnamed.csv row says why not (docs/naming.md 2.5)
            return ("reason", "by-consumer", "code only: " + fns[0])
        if code:
            return ("reason", "shared", "records and code: " + ", ".join(sorted(set(name(e.ref) for e in use))))
        notes = ""
        if fmt:
            notes = "; also format-only " + ", ".join(sorted(set(
                "%s[%d]" % (name(e.ref), e.slot[1] // 4) if e.slot[0] == "off" else name(e.ref) for e in fmt)))
        slots = sorted(((e.ref, self.slot(e), e) for e in recs), key=lambda x: self._order(x[0]) + (x[1],))
        owners = sorted(set(name(o) for o, s, e in slots))
        kinds = set(s[0] for o, s, e in slots)
        fams = sorted(set(self.family(o)[0] for o, s, e in slots))
        famn = [name(f) for f in fams]
        common = dict(note=notes, fmt_only=fmt_only)
        if all(is_placeholder(o) for o in owners):
            return ("reason", "via-unnamed", "referrers " + ", ".join(owners))
        # A1: one table's slots (A3: the lowest slot)
        if kinds == {"slot"}:
            if len(owners) != 1:
                return ("reason", "shared", "slots of " + ", ".join(owners))
            o = slots[0][0]
            on = name(o)
            ks = sorted(set(s[1] for _, s, _ in slots))
            sing = singular(on)
            if sing is None:
                return ("reason", "positional-table", "%s[%d]: the table's name is a position, not a plural" % (on, ks[0]))
            return ("name", dict(new="%s%d" % (sing, ks[0]), rule="A1" if len(ks) == 1 else "A3",
                                 chain="%s[%s]" % (chain(o), ",".join(str(k) for k in ks)),
                                 root=self._root(o, chain), where=slots[0][2].where, **common))
        if not kinds <= {"field", "slot-field"}:
            desc = "; ".join(sorted(set("%s%s" % (name(o), self.slot_text(s)) for o, s, e in slots)))[:300]
            if kinds <= {"slot2", "nofield"} and len(famn) == 1 and not is_placeholder(famn[0]):
                return ("reason", "via-unnamed-field", desc)
            return ("reason", "shared", desc)
        fields = set(s[1] if s[0] == "field" else s[2] for o, s, e in slots)
        if len(fields) != 1:
            return ("reason", "shared", "fields " + ", ".join(sorted(fields)))
        f = fields.pop()
        if re.match(r"^(unk|filler|pad)", f):
            return ("reason", "via-unnamed-field", "%s.%s" % (owners[0], f))

        def a2(o, s):
            """(name, evidence chain) of field f of record o, or (None, why)"""
            on = name(o)
            if s[0] == "field":
                return on + pascal(f), "%s -> %s.%s" % (chain(o), s[2], f)
            k, sname = s[1], s[3]
            if sname == "BgAnimCmd" and f == "ptr" and self.cmd_op(o, k) == 0:
                # op 0 copies the struct BgAnimTileFrame its ptr names
                # (BgAnimCopyTiles(cmd->ptr), src/camera_bg_anims.c): the
                # #155 pattern of op 1's ...Cmd<K>PaletteFade (Q2)
                return "%sCmd%dTileFrame" % (on, k), "%s[%d] -> BgAnimCmd.ptr (op 0, struct BgAnimTileFrame)" % (chain(o), k)
            sing = singular(on)
            if sing is None:
                return None, "%s[%d].%s: the table's name is a position (no plural)" % (on, k, f)
            return "%s%d%s" % (sing, k, pascal(f)), "%s[%d] -> %s.%s" % (chain(o), k, sname, f)
        named = [x for x in slots if not is_placeholder(name(x[0]))]
        if len(slots) == 1:
            o, s, e = slots[0]
            new, ev = a2(o, s)
            if new is None:
                return ("reason", "positional-table", ev)
            return ("name", dict(new=new, rule="A2", chain=ev, root=self._root(o, chain), where=e.where, **common))
        # A3: the records of one family (the stem only when the family has
        # one target of that field, Q5), else the lowest slot's A2 name; or
        # one proven identity
        ev = ", ".join(a2(o, s)[1] if not is_placeholder(name(o)) else "%s -> %s.%s" % (name(o), s[-1], f)
                       for o, s, e in slots)
        lo = named[0]
        lo_new, lo_ev = a2(lo[0], lo[1])
        if len(fams) == 1 and not is_placeholder(famn[0]):
            st = stem(famn[0])
            single = len(self.family_field_targets(fams[0], f)) == 1
            if st and single and lo[1][0] == "field":
                return ("name", dict(new=st + pascal(f), rule="A3", chain=ev, root=self._root(lo[0], chain),
                                     where=lo[2].where, **common))
            if lo_new is None:
                return ("reason", "positional-table", lo_ev)
            return ("name", dict(new=lo_new, rule="A3", chain=ev, root=self._root(lo[0], chain),
                                 where=lo[2].where, **common))
        if any(is_placeholder(x) for x in famn):
            return ("reason", "shared", "named and unnamed referrers " + ", ".join(owners)[:300])
        ident = self.common_identity(famn)
        if ident and lo[1][0] == "field":
            idfams = set(x for x in fams)
            single = len(set().union(*(self.family_field_targets(x, f) for x in idfams))) == 1
            if single:
                return ("name", dict(new="g" + ident + pascal(f), rule="A3-identity", chain=ev,
                                     root=self._root(lo[0], chain), where=lo[2].where, **common))
            return ("reason", "shared", "identity %s has several %s targets: %s" % (ident, f, ", ".join(famn)))
        return ("reason", "shared", "owners " + ", ".join(famn))

    def slot_text(self, s):
        if s[0] == "slot":
            return "[%d]" % s[1]
        if s[0] == "slot2":
            return "[%d][%d]" % (s[1], s[2])
        if s[0] == "field":
            return "." + s[1]
        if s[0] == "slot-field":
            return "[%d].%s" % (s[1], s[2])
        return " (%s)" % s[1]

    def _order(self, o):
        """sort key: a record's lowest slot in its family table; a named
        record before a placeholder"""
        f, k = self.family(o)
        n = self._name(o)
        return (is_placeholder(n), f, k, n)

    def owner_class(self, root, fmt_only=False):
        """`format` for a label only format-only words point at (Q1),
        `position` when the chain's named owner is a position name or a
        table whose slots are position names (docs/naming.md 2.4), else
        `semantic`"""
        if fmt_only:
            return "format"
        if root in self.positions or root in self.position_tables:
            return "position"
        return "semantic"

    def _root(self, o, chain):
        c = chain(o)
        m = IDENT_RE.match(c)
        return m.group(0) if m else o

    def common_identity(self, fams):
        words = [camel_words(f[1:]) for f in fams]
        common = []
        for ws in zip(*words):
            if len(set(ws)) == 1:
                common.append(ws[0])
            else:
                break
        for k in range(len(common), 0, -1):
            cand = "".join(common[:k])
            if cand in self.identities:
                return cand
        return None


def singular(table):
    """gBonkersFrames -> gBonkersFrame; None when the name is not a plural"""
    if table.endswith("List") and len(table) > 5:
        return table[:-4]  # a frame list's entry: gMrTickTockGfxFrameList -> gMrTickTockGfxFrame
    if table.endswith("ies"):
        return table[:-3] + "y"
    if table.endswith("s") and not table.endswith(("ss", "us")):
        return table[:-1]
    return None


def stem(table):
    """the table's name without its kind suffix: gBonkersFrames -> gBonkers"""
    w = camel_words(table[1:])
    if len(w) >= 2 and w[-1].endswith("s"):
        return "g" + "".join(w[:-1])
    return None


BATCHES = ("frames", "pictures", "rooms", "bganims", "player")


def batch_of(m, p):
    """the owner kind of a proposal, from its chain's named owner"""
    r = p["root"]
    if r.startswith("gPlayerFrame"):
        return "player"
    if re.match(r"^gLevel\d+Stage\d+", r):
        return "rooms"
    if r.startswith("gRoomBgAnimSet"):
        return "bganims"
    if r in m.crecords and m.crecords[r][1].endswith("src/data/frame_tables.c"):
        return "frames"
    return "pictures"


def root_file(m, r):
    if r in m.crecords:
        return m.crecords[r][1]
    if r in m.labels:
        return m.labels[r][2]
    return "?"


def evidence(m, p):
    return "slot: %s (%s); docs/naming.md 2.5 %s%s%s" % (
        p["chain"], os.path.relpath(root_file(m, p["root"]), ROOT) if os.path.isabs(root_file(m, p["root"])) else root_file(m, p["root"]),
        p["rule"], ", format-only" if p.get("fmt_only") else "", p["note"])


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n")[0])
    ap.add_argument("--fmt", default="consumer-first", choices=("consumer-first", "count"))
    ap.add_argument("--list", action="store_true", help="print every proposal and reason")
    ap.add_argument("--batch", choices=BATCHES, help="only the proposals of one owner kind")
    ap.add_argument("--root", help="only the proposals whose chain starts at this record")
    ap.add_argument("--csv", help="write the proposals as a tools/rename.py batch")
    ap.add_argument("--issue", default="183", help="the issue column of the --csv batch")
    args = ap.parse_args()
    m = Model()
    props, reasons = m.run(args.fmt)
    sel = dict((t, p) for t, p in props.items()
               if (not args.batch or batch_of(m, p) == args.batch)
               and (not args.root or p["root"] == args.root))
    rules = collections.Counter(p["rule"] for p in sel.values())
    cls = collections.Counter(m.owner_class(p["root"], p.get("fmt_only")) for p in sel.values())
    bat = collections.Counter(batch_of(m, p) for p in sel.values())
    print("proposals: %d  rules %s  owners %s  batches %s" % (len(sel), dict(rules), dict(cls), dict(bat)))
    rc = collections.Counter(r[0] for r in reasons.values())
    print("left: %d %s" % (len(reasons), dict(rc)))
    if args.list:
        for t, p in sorted(sel.items()):
            print("%s -> %s  [%s] %s" % (t, p["new"], p["rule"], evidence(m, p)))
        if not (args.batch or args.root):
            for t, r in sorted(reasons.items()):
                print("%s : %s  %s" % (t, r[0], r[1]))
    if args.csv:
        with open(args.csv, "w", encoding="utf-8", newline="") as f:
            w = csv.writer(f, lineterminator="\n")
            w.writerow(["old", "new", "kind", "evidence", "issue"])
            for t, p in sorted(sel.items(), key=lambda x: m.labels[x[0]][0]):
                w.writerow([t, p["new"], "asset", evidence(m, p), args.issue])
        print("wrote %d rows to %s" % (len(sel), args.csv))


if __name__ == "__main__":
    main()
