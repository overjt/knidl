#!/usr/bin/env python3
"""constants.py - pret-style named constants for the game's magic numbers.

Usage:
    tools/constants.py --defs DEFS.csv [--write]
    tools/constants.py --sites SITES.csv [--write]
    tools/constants.py --scan FAMILY [--out SITES.csv]
    tools/constants.py --verify-cpp REF
    tools/constants.py --census [--json]
    tools/constants.py --check

The code is full of numbers whose meaning the names prove: a task type in
`CreateChildTaskHere(214, 0)`, an ability id in `a->ability == 1`, a state
index in `ActorSetState(3)`.  Like pret and katam (`include/constants/`),
each enumeration gets object-like `#define`s in a header
`include/constants/<topic>.h`, and the sites spell the constant
(docs/naming.md section 7; #155 run 6, the owner's decision D6).

DEFS.csv (`header,block,constant,value,evidence[,note]`): one row per
constant.  `header` is the topic (`tasks` for include/constants/tasks.h),
`block` the block's header comment (`Task types - ...`; a new block is
created when no block has that title; rows of a block may give it once),
`value` the integer as most of its sites spell it (decimal or hex),
`evidence` the consumer that proves the value (docs/naming.md section 4),
`note` an optional one-line comment written after the define.  --write
writes the headers (one block per title, the defines in value order) and
appends one row per constant to docs/analysis/constants.csv.

SITES.csv (`file,line,col,literal,constant`): one row per literal to
respell.  `line`/`col` (1-based) locate the literal's first character in
FILE, `literal` is its current spelling; the constant must exist (in a
header or in the same run's DEFS) and have the literal's value.  --write
replaces each literal; a file that does not reach the constants header
through its includes (the subsystem headers include theirs) gets
`#include "constants/<topic>.h"` after its last #include, and docs/audit.md's
file:line references below it move by one.

--scan FAMILY prints the SITES rows of a mechanical family (FAMILIES below:
the argument positions and members whose values belong to that
enumeration), for every literal whose value has a constant.

The proof of a constants commit:
    tools/constants.py --verify-cpp HEAD    # every translation unit's cpp -P
                                            # output equals REF's, token by
                                            # token, integer literals compared
                                            # by value
    make clean && make compare
--verify-cpp REF lexes the whitespace-collapsed `cpp -P -I include` output
of every src/*.c, src/data/*.c and tools/header_smoke*.c of REF and of the
tree (in the knidl-builder image) and accepts only one difference: an
integer literal spelled differently with the same value and the same suffix,
both at most 0x7FFFFFFF (so its type cannot change).  `tools/rename.py
--verify-diff REF` accepts the logged constants too (a site where REF has an
integer literal and the tree a constant of the same value, the new headers,
the new #include lines), so a branch of renames, aliases and constants
verifies in one pass.

--bitexprs FAMILY prints the SITES rows of a `bits` family's complements
and unions of its proven bits (`&= 0x7FFF` -> `~A`, `& 0x6000` -> `(A | B)`;
#155 run 7, docs/naming.md 7.0's second form): a SITES row's `constant` may
be such an expression, which --sites checks against the literal (the OR, or
its complement in 8, 16 or 32 bits).  These are token changes: their proof
is the per-file assembly oracle (every unit identical to the parent), make
compare and tools/rename.py --verify-diff, which accepts them.

--census counts the constants per header, the sites that spell them, and
the literals left at a mechanical family's positions whose value has a
constant (tools/audit.py prints it in docs/naming.md section 5.1).
--check verifies that the headers and docs/analysis/constants.csv agree.
"""

import argparse
import csv
import io
import json
import os
import re
import shutil
import subprocess
import sys
import tempfile

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CONST_DIR = os.path.join(ROOT, "include", "constants")
LOG = os.path.join(ROOT, "docs", "analysis", "constants.csv")
LOG_FIELDS = ["constant", "value", "header", "evidence", "issue"]
IMAGE = "knidl-builder"

TOK_RE = re.compile(r"""
 (?P<ws>\s+)
|(?P<comment>/\*.*?\*/|//[^\n]*)
|(?P<pp>\#(?:\\\n|[^\n])*)
|(?P<str>"(?:\\.|[^"\\\n])*"|'(?:\\.|[^'\\\n])*')
|(?P<num>(?:0[xX][0-9A-Fa-f]+|[0-9]+)[uUlL]*(?![\w.]))
|(?P<float>[0-9]*\.[0-9]+(?:[eE][-+]?[0-9]+)?[fFlL]?|[0-9]+[eE][-+]?[0-9]+[fFlL]?)
|(?P<id>[A-Za-z_]\w*)
|(?P<op>\.\.\.|->|\+\+|--|<<=|>>=|<<|>>|<=|>=|==|!=|&&|\|\||[-+*/%&|^!~<>=]=?|[()\[\]{};,.?:])
|(?P<other>.)
""", re.S | re.X)
NAME_RE = re.compile(r"^[A-Z][A-Z0-9_]*$")
DEFINE_RE = re.compile(r"^#define (\w+) +(0[xX][0-9A-Fa-f]+|[0-9]+)(?: +/\* (.*) \*/)?$")
BLOCK_RE = re.compile(r"^/\* (.+) \*/$")
INCLUDE_RE = re.compile(r'^[ \t]*#[ \t]*include[ \t]+"constants/(\w+)\.h"[ \t]*$', re.M)
MAX_VALUE = 0x7FFFFFFF

# The mechanical families: where a value of the enumeration is spelled.
#   calls:   {function: argument index (0-based)}
#   members: struct members a literal is stored into or compared with
#            (`x.member = N`, `x->member == N`, `!=`)
#   indexed: arrays whose element is compared with a literal
#            (`gTaskSlotTypes[i] == N`)
#   bits:    flag-word members whose single bits are the family's values
#            (#155 run 7, R3): a literal is at a position when it is the whole
#            right operand of `&`, `|`, `^`, `&=`, `|=` or `^=` whose left
#            operand is the member (`x->immunityFlags & 4`,
#            `t->spriteFlags |= 0x8000`); complements and masks of several
#            bits are not values of the family and stay out of the scan
#   bit_vars: the same for plain variables
#   bit_calls: {function: argument index} whose argument is such a flag word
#            (a single bit there takes the constant, a union of bits the
#            second form's `(A | B)`)
#   width:   the flag word's width in bits (16 when not given), for the
#            complements of the second form
#   prefix:  only the header's constants with this prefix (a header may hold
#            two enumerations: GAME_STATE_ and STAGE_REQUEST_)
#   skip_files / skip_sites: where the same member or variable holds another
#            enumeration's values (file; or file, function, line hint,
#            literal, reason)
FAMILIES = {
    "tasks": {
        "header": "tasks",
        "calls": {
            "TaskCreate": 0, "TaskCreateFrom": 0, "TaskCreateInRange": 0,
            "TaskCreateHighSlot": 0, "TaskCreatePausedInScreenAttack": 0,
            "CreateChildTask": 0, "CreateChildTaskAt": 0,
            "CreateChildTaskAtOffsetFacing": 0, "CreateChildTaskHere": 0,
            "CreateActor": 2, "CreateItemOrObject": 1, "CreateItemHere": 1, "CreateItemAt": 1,
        },
        "members": ["taskType"],
        "indexed": ["gTaskSlotTypes"],
        "skip_values": [-1],
    },
    "abilities": {
        "header": "abilities",
        "calls": {
            "SetPlayerAbility": 0, "SetPlayerAbilityNoHud": 0, "HudShowAbility": 0,
            "HudShowAbilityAnimated": 0, "HudLoadAbilityPicture": 0,
        },
        "members": ["ability", "attackAbility", "pendingAbility"],
        "indexed": ["gPlayerAbilities", "gSavedPlayerAbilities"],
    },
    "game_states": {
        "header": "game_states",
        "prefix": "GAME_STATE_",
        "vars": ["gGameState", "gPrevGameState"],
    },
    "stage_requests": {
        "header": "game_states",
        "prefix": "STAGE_REQUEST_",
        "vars": ["gStageRequest"],
    },
    "hits": {
        "header": "hits",
        "prefix": "HIT_KIND_",
        "calls": {"ActorReactToHitKind": 0, "CreateBlockStar": 3},
        "members": ["hitKind"],
        "vars": ["gHitKind"],
        "skip_files": ["src/plobj_509ec.c", "src/plobj_514f8.c", "src/plobj_52f6c.c"],
    },
    "actors": {
        "header": "actors",
        "prefix": "ACTOR_KIND_",
        "calls": {"CreateActor": 0, "CreateActorByKind": 0},
        "members": ["actorKind"],
    },
    "rooms": {
        "header": "rooms",
        "prefix": "ROOM_ENTRY_",
        "members": ["roomEntryMode"],
        "vars": ["gRoomEntryMode"],
    },
    "sound": {
        "header": "sound",
        "prefix": "SE_",
        "calls": {"PlaySfx": 0, "PlaySfxIfLocalPlayer": 0, "PlayerStartSfx": 0},
    },
    "songs": {
        "header": "sound",
        "prefix": "BGM_",
        "calls": {"PlayBgm": 0},
    },
    "player_effect_variants": {
        "header": "variants",
        "prefix": "PLAYER_EFFECT_VARIANT_",
        "calls": {"CreatePlayerEffect": 1, "CreatePlayerEffectHighSlot": 1},
    },
    "player_object_variants": {
        "header": "variants",
        "prefix": "PLAYER_OBJECT_VARIANT_",
        "calls": {"CreatePlayerObject": 1, "CreatePlayerObjectLowSlot": 1},
    },
    "attack_box_immunity": {
        "header": "hits",
        "prefix": "ATTACK_BOX_IMMUNITY_",
        "bits": ["immunityFlags"],
    },
    "attack_box_flags": {
        "header": "hits",
        "prefix": "ATTACK_BOX_FLAG_",
        "bits": ["attackFlags"],
    },
    "body_box_flags": {
        "header": "hits",
        "prefix": "BODY_BOX_FLAG_",
        "bits": ["bodyFlags"],
    },
    "body_box_guard": {
        "header": "hits",
        "prefix": "BODY_BOX_GUARD_",
        "bits": ["guardFlags"],
    },
    "player_action_flags": {
        "header": "player",
        "prefix": "PLAYER_ACTION_FLAG_",
        "bits": ["actionFlags"],
    },
    "player_status": {
        "header": "player",
        "prefix": "PLAYER_STATUS_",
        "bits": ["statusFlags"],
    },
    "sprite_flags": {
        "header": "sprites",
        "prefix": "SPRITE_FLAG_",
        "bits": ["spriteFlags"],
    },
    "task_skip": {
        "header": "task_skip",
        "prefix": "TASK_SKIP_",
        "bits": ["skipMask"],
        "bit_calls": {"TaskSetSkipMask": 0, "TaskSetOthersSkipMask": 0, "TaskSetAllSkipMask": 0,
                      "TaskFreezeOrThawOthers": 0, "FreezeOtherTasks": 0},
        "width": 8,
    },
    "camera": {
        "header": "camera",
        "prefix": "CAMERA_MODE_",
        "vars": ["gCameraMode"],
        "skip_files": ["src/level_23948.c"],
        "skip_sites": [
            ["src/stage_273a0.c", "CameraStartHoldAnchorAt", "gCameraMode = 4", "4", "hub branch (gInHub != 0): the hub's hold-anchor mode"],
            ["src/camera_2d01c.c", "CameraStartHoldAnchor", "gCameraMode = 4", "4", "hub branch (gInHub != 0): the hub's hold-anchor mode"],
            ["src/camera_28b8c.c", "SetRoomEntryPoint", "gCameraMode == 2 || gCameraMode == 4", "2", "hub branch: the hub's hold-anchor modes 2/4"],
            ["src/camera_28b8c.c", "SetRoomEntryPoint", "gCameraMode == 2 || gCameraMode == 4", "4", "hub branch: the hub's hold-anchor modes 2/4"],
            ["src/stage_261c0.c", "SetCameraFocusOrAnchor", "gCameraMode != 2 && gCameraMode != 4", "2", "hub branch: the hub's hold-anchor modes 2/4"],
            ["src/stage_261c0.c", "SetCameraFocusOrAnchor", "gCameraMode != 2 && gCameraMode != 4", "4", "hub branch: the hub's hold-anchor modes 2/4"],
        ],
    },
}


class ConstError(Exception):
    pass


def rel(path):
    return os.path.relpath(path, ROOT)


def read(path):
    with open(path, encoding="utf-8") as f:
        return f.read()


def write(path, text):
    with open(path, "w", encoding="utf-8") as f:
        f.write(text)


def lex(text):
    """[(kind, text, start)] of the C tokens of `text` (no whitespace or
    comments; a preprocessor line is one `pp` token)."""
    out = []
    for m in TOK_RE.finditer(text):
        k = m.lastgroup
        if k in ("ws", "comment"):
            continue
        out.append((k, m.group(0), m.start()))
    return out


def int_value(s):
    """(value, suffix) of an integer literal."""
    body = s.rstrip("uUlL")
    suffix = s[len(body):].lower()
    if body[:2] in ("0x", "0X"):
        v = int(body, 16)
    elif len(body) > 1 and body[0] == "0":
        v = int(body, 8)
    else:
        v = int(body)
    return v, "".join(sorted(suffix))


def header_path(topic):
    return os.path.join(CONST_DIR, topic + ".h")


# ---- headers --------------------------------------------------------------------

def parse_header(topic):
    """(preamble lines, [[title, [(name, value, note)]]]) of a constants
    header, or None if it does not exist."""
    path = header_path(topic)
    if not os.path.exists(path):
        return None
    lines = read(path).split("\n")
    guard = "GUARD_CONSTANTS_%s_H" % topic.upper()
    if lines[:2] != ["#ifndef " + guard, "#define " + guard]:
        raise ConstError("%s: not a constants header (guard)" % rel(path))
    i = 2
    preamble = []
    while i < len(lines) and not BLOCK_RE.match(lines[i]) and not lines[i].startswith("#endif"):
        preamble.append(lines[i])
        i += 1
    blocks = []
    for ln in lines[i:]:
        if not ln.strip() or ln.startswith("#endif"):
            continue
        m = BLOCK_RE.match(ln)
        if m and not ln.startswith("#"):
            blocks.append([m.group(1), []])
            continue
        m = DEFINE_RE.match(ln)
        if not m or not blocks:
            raise ConstError("%s: unexpected line %r" % (rel(path), ln))
        blocks[-1][1].append((m.group(1), m.group(2), m.group(3) or ""))
    while preamble and not preamble[-1].strip():
        preamble.pop()
    return preamble, blocks


def render_header(topic, preamble, blocks):
    guard = "GUARD_CONSTANTS_%s_H" % topic.upper()
    out = ["#ifndef " + guard, "#define " + guard] + preamble
    for title, defs in blocks:
        out.append("")
        out.append("/* %s */" % title)
        width = max(len(n) for n, _, _ in defs) + 1
        vwidth = max(len(v) for _, v, _ in defs)
        for name, value, note in sorted(defs, key=lambda d: (int_value(d[1])[0], d[0])):
            ln = "#define %-*s %s" % (width, name, value)
            if note:
                ln = "#define %-*s %-*s /* %s */" % (width, name, vwidth, value, note)
            out.append(ln.rstrip())
    out.append("")
    out.append("#endif // " + guard)
    out.append("")
    return "\n".join(out)


def all_constants():
    """{name: (value, topic)} of every header in include/constants/."""
    out = {}
    if not os.path.isdir(CONST_DIR):
        return out
    for fn in sorted(os.listdir(CONST_DIR)):
        if not fn.endswith(".h"):
            continue
        topic = fn[:-2]
        _, blocks = parse_header(topic)
        for _, defs in blocks:
            for name, value, _ in defs:
                out[name] = (int_value(value)[0], topic)
    return out


def tree_identifiers():
    ids = set()
    for d in ("src", "include", "asm", "tools"):
        for dp, _, fns in os.walk(os.path.join(ROOT, d)):
            for fn in fns:
                if fn.endswith((".c", ".h", ".s", ".inc")):
                    ids.update(re.findall(r"[A-Za-z_]\w*", read(os.path.join(dp, fn))))
    return ids


# ---- --defs ---------------------------------------------------------------------

PREAMBLES = {}


def preamble_for(topic, text):
    lines = ["", "/*"]
    for para in text.split("\n"):
        lines += [(" * " + l).rstrip() for l in wrap_words(para, 74)] if para else [" *"]
    lines.append(" */")
    return lines


def wrap_words(text, width):
    out, cur = [], ""
    for w in text.split():
        if cur and len(cur) + 1 + len(w) > width:
            out.append(cur)
            cur = w
        else:
            cur = (cur + " " + w).strip()
    if cur:
        out.append(cur)
    return out


def apply_defs(rows, do_write, preambles=None):
    existing = all_constants()
    ids = tree_identifiers()
    headers = {}
    problems = []
    seen = set()
    for r in rows:
        topic, name, value = r["header"], r["constant"], r["value"]
        if not re.match(r"^[a-z][a-z0-9_]*$", topic):
            problems.append("%s: bad header topic %r" % (name, topic))
            continue
        if not NAME_RE.match(name):
            problems.append("%s: not an UPPER_CASE name" % name)
        if name in existing or name in seen:
            problems.append("%s: already defined" % name)
        elif name in ids:
            problems.append("%s: already an identifier in the tree" % name)
        seen.add(name)
        if not re.match(r"^(0[xX][0-9A-Fa-f]+|[0-9]+)$", value):
            problems.append("%s: value %r is not a plain integer literal" % (name, value))
        elif int_value(value)[0] > MAX_VALUE:
            problems.append("%s: value %s is above 0x7FFFFFFF" % (name, value))
        if not r.get("evidence", "").strip():
            problems.append("%s: no evidence" % name)
        if topic not in headers:
            parsed = parse_header(topic)
            if parsed is None:
                pre = (preambles or {}).get(topic)
                if not pre:
                    problems.append("%s: new header %s needs a preamble" % (name, topic))
                    pre = ""
                parsed = (preamble_for(topic, pre), [])
            headers[topic] = parsed
        blocks = headers[topic][1]
        title = r["block"].strip()
        for b in blocks:
            if b[0] == title or (title and b[0].split(" - ")[0] == title.split(" - ")[0]):
                b[1].append((name, value, r.get("note", "").strip()))
                if title and len(title) > len(b[0]):
                    b[0] = title
                break
        else:
            if not title:
                problems.append("%s: no block title" % name)
            blocks.append([title, [(name, value, r.get("note", "").strip())]])
    if problems:
        raise ConstError("\n".join(problems))
    for topic, (pre, blocks) in headers.items():
        for title, defs in blocks:
            vals = {}
            for n, v, _ in defs:
                vals.setdefault(int_value(v)[0], []).append(n)
            dup = {k: v for k, v in vals.items() if len(v) > 1}
            if dup:
                raise ConstError("block %r of %s: one value, two names: %s" % (title, topic, dup))
    print("defs: %d constants in %d header(s)" % (len(rows), len(headers)))
    if not do_write:
        print("dry run; --write applies")
        return
    os.makedirs(CONST_DIR, exist_ok=True)
    for topic, (pre, blocks) in headers.items():
        write(header_path(topic), render_header(topic, pre, blocks))
    new = not os.path.exists(LOG)
    with open(LOG, "a", newline="", encoding="utf-8") as f:
        w = csv.writer(f, lineterminator="\n")
        if new:
            w.writerow(LOG_FIELDS)
        for r in rows:
            w.writerow([r["constant"], r["value"], "include/constants/%s.h" % r["header"],
                        r["evidence"], r.get("issue") or "155"])
    print("wrote %s and %d constants.csv rows" % (", ".join(
        rel(header_path(t)) for t in sorted(headers)), len(rows)))


# ---- --sites --------------------------------------------------------------------

def line_starts(text):
    starts = [0]
    for i, c in enumerate(text):
        if c == "\n":
            starts.append(i + 1)
    return starts


INC_ANY_RE = re.compile(r'^[ \t]*#[ \t]*include[ \t]+"([^"]+)"', re.M)
AUDIT_DOC = os.path.join(ROOT, "docs", "audit.md")


def reaches(path, target, seen=None):
    """True if the file at `path` includes `target` (a path under include/),
    directly or through the headers it includes."""
    seen = seen if seen is not None else set()
    if path in seen:
        return False
    seen.add(path)
    for m in INC_ANY_RE.finditer(read(path)):
        inc = os.path.join(ROOT, "include", m.group(1))
        if os.path.normpath(inc) == os.path.normpath(target):
            return True
        if os.path.exists(inc) and reaches(inc, target, seen):
            return True
    return False


def shift_audit_lines(path, after):
    """docs/audit.md section 3 cites code exceptions by file:line; a line
    inserted after line `after` of `path` moves the later ones by one."""
    text = read(AUDIT_DOC)
    pat = re.compile(r"`%s:(\d+)(?:-(\d+))?`" % re.escape(path))

    def bump(m):
        a = int(m.group(1))
        b = m.group(2)
        a2 = a + 1 if a > after else a
        if b is None:
            return "`%s:%d`" % (path, a2)
        b2 = int(b) + 1 if int(b) > after else int(b)
        return "`%s:%d-%d`" % (path, a2, b2)
    new = pat.sub(bump, text)
    if new != text:
        write(AUDIT_DOC, new)


def add_include(text, topic):
    inc = '#include "constants/%s.h"' % topic
    if re.search(r'^[ \t]*#[ \t]*include[ \t]+"constants/%s\.h"' % topic, text, re.M):
        return text
    last = None
    for m in re.finditer(r'^[ \t]*#[ \t]*include[^\n]*\n', text, re.M):
        last = m
    if last is None:
        raise ConstError("no #include line to follow")
    # keep the constants includes together, in name order
    pos = last.end()
    for m in INCLUDE_RE.finditer(text):
        if m.group(1) > topic:
            pos = m.start()
            break
    return text[:pos] + inc + "\n" + text[pos:]


BIT_EXPR_RE = re.compile(r"^(~)?(?:([A-Z][A-Z0-9_]*)|\(([A-Z][A-Z0-9_]*(?: \| [A-Z][A-Z0-9_]*)+)\))$")
BIT_WIDTHS = (8, 16, 32)


def bit_expr_value(expr, consts):
    """(value of the OR, complemented?, topic) of a bit expression `~A`,
    `~(A | B)` or `(A | B)` whose constants are single bits of one header;
    None otherwise."""
    m = BIT_EXPR_RE.match(expr)
    if not m or (not m.group(1) and m.group(2)):
        return None
    names = [m.group(2)] if m.group(2) else m.group(3).split(" | ")
    value, topics = 0, set()
    for n in names:
        c = consts.get(n)
        if c is None or c[0] <= 0 or c[0] & (c[0] - 1) or value & c[0]:
            return None
        value |= c[0]
        topics.add(c[1])
    if len(topics) != 1 or names != sorted(names, key=lambda n: consts[n][0]):
        return None
    return value, bool(m.group(1)), topics.pop()


def bit_expr_matches(literal, value, comp):
    """True if an integer literal equals the bit expression: the OR itself,
    or (complemented) the OR's complement in a field of 8, 16 or 32 bits."""
    if not comp:
        return literal == value
    return any(literal == (~value) & ((1 << w) - 1) for w in BIT_WIDTHS if value < (1 << w))


def apply_sites(rows, do_write, consts=None):
    consts = dict(consts or all_constants())
    by_file = {}
    for r in rows:
        by_file.setdefault(r["file"], []).append(r)
    problems = []
    edits = {}
    count = 0
    for path, rs in sorted(by_file.items()):
        full = os.path.join(ROOT, path)
        text = read(full)
        starts = line_starts(text)
        toks = {t[2]: t for t in lex(text)}
        spans = []
        topics = set()
        for r in rs:
            ln, col = int(r["line"]), int(r["col"])
            off = starts[ln - 1] + col - 1
            tok = toks.get(off)
            shift = re.match(r"^(0[xX][0-9A-Fa-f]+|[0-9]+) << (0[xX][0-9A-Fa-f]+|[0-9]+)$", r["literal"])
            if shift:
                # a shift `N << M` spelling one value (the second form)
                if text[off:off + len(r["literal"])] != r["literal"]:
                    problems.append("%s:%d:%d: no %s there" % (path, ln, col, r["literal"]))
                    continue
            elif not tok or tok[0] != "num" or tok[1] != r["literal"]:
                problems.append("%s:%d:%d: no literal %s there" % (path, ln, col, r["literal"]))
                continue
            if shift:
                v = int_value(shift.group(1))[0] << int_value(shift.group(2))[0]
                c = consts.get(r["constant"])
                if c is not None:
                    if v != c[0]:
                        problems.append("%s:%d: %s is %d, %s is %d" % (path, ln, r["literal"], v,
                                                                       r["constant"], c[0]))
                        continue
                    topic = c[1]
                else:
                    ev = bit_expr_value(r["constant"], consts)
                    if ev is None or ev[1] or ev[0] != v:
                        problems.append("%s:%d: %s does not equal %s" % (path, ln, r["literal"],
                                                                         r["constant"]))
                        continue
                    topic = ev[2]
            elif re.match(r"^[A-Z][A-Z0-9_]*$", r["constant"]):
                c = consts.get(r["constant"])
                if c is None:
                    problems.append("%s:%d: unknown constant %s" % (path, ln, r["constant"]))
                    continue
                v, suffix = int_value(r["literal"])
                if v != c[0] or suffix:
                    problems.append("%s:%d: %s is %d, %s is %d" % (path, ln, r["literal"], v,
                                                                   r["constant"], c[0]))
                    continue
                topic = c[1]
            else:
                # a bit expression (docs/naming.md 7.0, the second form):
                # `~A`, `~(A | B)` or `(A | B)` of one header's constants
                v, suffix = int_value(r["literal"])
                ev = bit_expr_value(r["constant"], consts)
                if ev is None or suffix:
                    problems.append("%s:%d: %r is not a bit expression of known constants"
                                    % (path, ln, r["constant"]))
                    continue
                value, comp, topic = ev
                if not bit_expr_matches(v, value, comp):
                    problems.append("%s:%d: %s does not equal %s" % (path, ln, r["literal"],
                                                                     r["constant"]))
                    continue
            spans.append((off, off + len(r["literal"]), r["constant"]))
            topics.add(topic)
        if len(set(s[0] for s in spans)) != len(spans):
            problems.append("%s: a literal is listed twice" % path)
        for a, b, name in sorted(spans, reverse=True):
            text = text[:a] + name + text[b:]
        added = []
        for t in sorted(topics):
            if reaches(full, header_path(t)):
                continue
            try:
                before = text
                text = add_include(text, t)
                pos = next(i for i in range(len(text)) if i >= len(before) or text[i] != before[i])
                added.append(text.count("\n", 0, pos))
            except ConstError as e:
                problems.append("%s: %s" % (path, e))
        edits[full] = (text, added)
        count += len(spans)
    if problems:
        raise ConstError("\n".join(problems))
    print("sites: %d literals in %d files" % (count, len(edits)))
    if not do_write:
        print("dry run; --write applies")
        return
    for full, (text, added) in edits.items():
        write(full, text)
        for after in added:
            shift_audit_lines(rel(full), after)
    includes = sum(len(a) for _, a in edits.values())
    if includes:
        print("added %d #include lines (docs/audit.md line refs shifted)" % includes)
    print("wrote %d files; now: tools/constants.py --verify-cpp HEAD, make clean && make compare"
          % len(edits))


# ---- --scan / --census ------------------------------------------------------------

def src_files():
    out = []
    for d in ("src", os.path.join("src", "data")):
        for fn in sorted(os.listdir(os.path.join(ROOT, d))):
            if fn.endswith(".c"):
                out.append(os.path.join(d, fn))
    return out


def literal_at(toks, i0, i1):
    """The literal of the token range [i0, i1) when it is one integer
    literal (or a negative one), else None: (index, value, is_negative)."""
    seg = toks[i0:i1]
    if len(seg) == 1 and seg[0][0] == "num":
        return i0, int_value(seg[0][1])[0], False
    if len(seg) == 2 and seg[0][1] == "-" and seg[1][0] == "num":
        return i0 + 1, -int_value(seg[1][1])[0], True
    return None


def _close(toks, j):
    """Index of the bracket that closes the one at toks[j]."""
    depth = 0
    for k in range(j, len(toks)):
        if toks[k][1] in ("(", "[", "{"):
            depth += 1
        elif toks[k][1] in (")", "]", "}"):
            depth -= 1
            if depth == 0:
                return k
    return len(toks) - 1


def _operand_end(toks, i):
    """Index of the last token of the operand that starts at `i`: a member
    or variable, with its `[...]` index if any."""
    if i + 1 < len(toks) and toks[i + 1][1] == "[":
        return _close(toks, i + 1)
    return i


def _value_after(toks, j):
    """The literal right of the operator at toks[j] when it is the whole
    right-hand side: (token index, value)."""
    n = len(toks)
    lit = literal_at(toks, j + 1, j + 2) or literal_at(toks, j + 1, j + 3)
    if lit and lit[0] + 1 < n and toks[lit[0] + 1][1] in (";", ")", "&&", "||", ",", "?", "}"):
        return lit[0], lit[1]
    return None


def _switch_cases(toks, open_brace):
    """yield the token index of each `case` label's literal of the switch
    whose body opens at toks[open_brace] (nested switches excluded)."""
    end = _close(toks, open_brace)
    k = open_brace + 1
    while k < end:
        t = toks[k][1]
        if t == "switch" and toks[k + 1][1] == "(":
            c = _close(toks, k + 1)
            if c + 1 < end and toks[c + 1][1] == "{":
                k = _close(toks, c + 1) + 1
                continue
        if t == "case":
            lit = literal_at(toks, k + 1, k + 2) or literal_at(toks, k + 1, k + 3)
            if lit and toks[lit[0] + 1][1] == ":":
                yield lit[0], lit[1]
        k += 1


def call_argument(toks, i, want):
    """(first, end) token range of argument `want` of the call whose name is
    toks[i], or None (a declaration, or fewer arguments)."""
    n = len(toks)
    if i > 0 and toks[i - 1][0] == "id" and toks[i - 1][1] not in ("return", "else", "case"):
        return None
    depth, a0, args = 0, i + 2, []
    j = i + 1
    while j < n:
        x = toks[j][1]
        if x in ("(", "[", "{"):
            depth += 1
        elif x in (")", "]", "}"):
            depth -= 1
            if depth == 0:
                args.append((a0, j))
                break
        elif x == "," and depth == 1:
            args.append((a0, j))
            a0 = j + 1
        j += 1
    return args[want] if want < len(args) else None


def family_positions(fam, toks):
    """yield (token index of the literal, value) of every literal at the
    family's positions in the token list: the argument of a listed call,
    a literal stored into or compared with a listed member (after `.` or
    `->`), variable (a plain identifier) or array element, and the `case`
    labels of a switch on one of them."""
    spec = FAMILIES[fam]
    n = len(toks)
    calls = spec.get("calls", {})
    members = set(spec.get("members", []))
    variables = set(spec.get("vars", []))
    indexed = set(spec.get("indexed", []))
    bit_members = set(spec.get("bits", []))
    bit_vars = set(spec.get("bit_vars", []))
    bit_calls = spec.get("bit_calls", {})

    def is_bit_operand(i):
        k, t = toks[i][0], toks[i][1]
        if k != "id":
            return False
        prev = toks[i - 1][1] if i > 0 else ""
        if t in bit_members and prev in (".", "->"):
            return True
        return t in bit_vars and prev not in (".", "->")

    def is_operand(i):
        k, t = toks[i][0], toks[i][1]
        if k != "id":
            return False
        prev = toks[i - 1][1] if i > 0 else ""
        if t in members and prev in (".", "->"):
            return True
        if t in variables and prev not in (".", "->"):
            return True
        if t in indexed and prev not in (".", "->") and i + 1 < n and toks[i + 1][1] == "[":
            return True
        return False

    for i in range(n):
        k, t = toks[i][0], toks[i][1]
        if k == "id" and t in bit_calls and i + 1 < n and toks[i + 1][1] == "(":
            arg = call_argument(toks, i, bit_calls[t])
            if arg:
                lit = literal_at(toks, *arg)
                if lit and lit[1] > 0 and not lit[1] & (lit[1] - 1):
                    yield lit[0], lit[1]
            continue
        if k == "id" and t in calls and i + 1 < n and toks[i + 1][1] == "(":
            if i > 0 and toks[i - 1][0] == "id" and toks[i - 1][1] not in ("return", "else", "case"):
                continue  # a declaration or definition
            want = calls[t]
            depth, a0, args = 0, i + 2, []
            j = i + 1
            while j < n:
                x = toks[j][1]
                if x in ("(", "[", "{"):
                    depth += 1
                elif x in (")", "]", "}"):
                    depth -= 1
                    if depth == 0:
                        args.append((a0, j))
                        break
                elif x == "," and depth == 1:
                    args.append((a0, j))
                    a0 = j + 1
                j += 1
            if want < len(args):
                lit = literal_at(toks, *args[want])
                if lit:
                    yield lit[0], lit[1]
        elif is_bit_operand(i):
            j = _operand_end(toks, i) + 1
            if j < n and toks[j][1] in ("&", "|", "^", "&=", "|=", "^="):
                v = _value_after(toks, j)
                if v and v[1] > 0 and v[1] & (v[1] - 1) == 0:
                    yield v
        elif is_operand(i):
            j = _operand_end(toks, i) + 1
            if j < n and toks[j][1] in ("=", "==", "!="):
                v = _value_after(toks, j)
                if v:
                    yield v
        elif t == "switch" and i + 1 < n and toks[i + 1][1] == "(":
            c = _close(toks, i + 1)
            if c + 1 >= n or toks[c + 1][1] != "{":
                continue
            # the switch expression's operand: its last member, variable or
            # array (casts allowed before it)
            last = c - 1
            if toks[last][1] == "]":
                d = 0
                for q in range(last, i, -1):
                    if toks[q][1] == "]":
                        d += 1
                    elif toks[q][1] == "[":
                        d -= 1
                        if d == 0:
                            last = q - 1
                            break
            if last > i + 1 and is_operand(last) and _operand_end(toks, last) == c - 1:
                for pos in _switch_cases(toks, c + 1):
                    yield pos


def function_spans(toks):
    """[(name, first token index, last token index)] of the function
    bodies of a file's token list."""
    out, depth, cur = [], 0, None
    for i, t in enumerate(toks):
        if t[1] == "{":
            if depth == 0 and i > 0 and toks[i - 1][1] == ")":
                j = i - 1
                d = 0
                while j >= 0:
                    if toks[j][1] == ")":
                        d += 1
                    elif toks[j][1] == "(":
                        d -= 1
                        if d == 0:
                            break
                    j -= 1
                if j > 0 and toks[j - 1][0] == "id":
                    cur = (toks[j - 1][1], i)
            depth += 1
        elif t[1] == "}":
            depth -= 1
            if depth == 0 and cur:
                out.append((cur[0], cur[1], i))
                cur = None
    return out


def bit_expr_sites(fam):
    """[(file, line, col, literal, expression)] of the literals at a bits
    family's positions that are a complement or a union of its proven bits
    (`&= 0x7FFF` -> `&= ~A`, `& 0x6000` -> `& (A | B)`; docs/naming.md 7.0,
    the second form), and [(file, line, col, literal)] of those that are
    not.  The spec's `width` is the field's width in bits."""
    spec = FAMILIES[fam]
    width = spec.get("width", 16)
    mask = (1 << width) - 1
    consts = all_constants()
    bits = sorted((v, n) for n, (v, topic) in consts.items()
                  if topic == spec["header"] and n.startswith(spec.get("prefix", ""))
                  and v > 0 and not v & (v - 1))
    allbits = 0
    for v, _ in bits:
        allbits |= v

    def expr(v, comp):
        names = [n for b, n in bits if v & b]
        if len(names) == 1:
            return ("~" if comp else "") + names[0]
        return ("~" if comp else "") + "(" + " | ".join(names) + ")"
    members = set(spec.get("bits", []))
    variables = set(spec.get("bit_vars", []))
    calls = spec.get("bit_calls", {})
    ends = (";", ")", "&&", "||", ",", "?", "}")
    rows, other = [], []

    def operand(toks, j, text):
        """(token index, end index, literal text, value) of the literal or
        `N << M` shift that is the whole operand starting at toks[j]."""
        if j + 1 < len(toks) and toks[j][0] == "num" and toks[j + 1][1] in ends:
            return j, j, toks[j][1], int_value(toks[j][1])[0]
        if (j + 3 < len(toks) and toks[j][0] == "num" and toks[j + 1][1] == "<<"
                and toks[j + 2][0] == "num" and toks[j + 3][1] in ends):
            a, b = int_value(toks[j][1])[0], int_value(toks[j + 2][1])[0]
            return j, j + 2, text[toks[j][2]:toks[j + 2][2] + len(toks[j + 2][1])], a << b
        return None
    for path in src_files():
        text = read(os.path.join(ROOT, path))
        starts = line_starts(text)
        toks = lex(text)
        n = len(toks)
        for i in range(n):
            k, tk = toks[i][0], toks[i][1]
            prev = toks[i - 1][1] if i > 0 else ""
            op = None
            if k == "id" and tk in calls and i + 1 < n and toks[i + 1][1] == "(":
                arg = call_argument(toks, i, calls[tk])
                if not arg:
                    continue
                o = operand(toks, arg[0], text)
                if not o or o[1] + 1 != arg[1]:
                    continue
                op = "call"
            elif k == "id" and ((tk in members and prev in (".", "->"))
                                or (tk in variables and prev not in (".", "->"))):
                j = _operand_end(toks, i) + 1
                if j >= n or toks[j][1] not in ("&", "|", "^", "&=", "|=", "^="):
                    continue
                o = operand(toks, j + 1, text)
                if not o:
                    continue
                op = toks[j][1]
            else:
                continue
            pos, _end, lit, v = o
            if v <= 0 or (not v & (v - 1) and pos == _end):
                continue  # zero, or a single-bit literal: the first form
            off = toks[pos][2]
            ln = max(x for x in range(len(starts)) if starts[x] <= off)
            where = (path, ln + 1, off - starts[ln] + 1, lit)
            comp = (~v) & mask
            if op in ("&", "&=") and comp and not comp & ~allbits and v <= mask:
                rows.append(where + (expr(comp, True),))
            elif not v & ~allbits:
                rows.append(where + (expr(v, False),))
            else:
                other.append(where)
    return rows, other


def scan(fam):
    """[(file, line, col, literal, constant)] of the family's literals whose
    value has a constant in the family's header, and the count of those
    without one."""
    spec = FAMILIES[fam]
    consts = all_constants()
    by_value = {}
    for name, (v, topic) in consts.items():
        if topic == spec["header"] and name.startswith(spec.get("prefix", "")):
            by_value.setdefault(v, []).append(name)
    skip_sites = {}
    for f, func, hint, lit, _why in spec.get("skip_sites", []):
        skip_sites.setdefault(f, []).append((func, hint, lit))
    rows, missing = [], []
    for path in src_files():
        if path in spec.get("skip_files", []):
            continue
        text = read(os.path.join(ROOT, path))
        starts = line_starts(text)
        toks = lex(text)
        spans = function_spans(toks) if path in skip_sites else []
        for i, v in family_positions(fam, toks):
            if v in spec.get("skip_values", []):
                continue
            if path in skip_sites:
                func = next((n for n, a, b in spans if a <= i <= b), None)
                ls = text.rfind("\n", 0, toks[i][2]) + 1
                le = text.find("\n", toks[i][2])
                line = text[ls:le if le >= 0 else len(text)]
                if any(func == f and h in line and toks[i][1] == l for f, h, l in skip_sites[path]):
                    continue
            off = toks[i][2]
            lo, hi = 0, len(starts) - 1
            while lo < hi:
                mid = (lo + hi + 1) // 2
                if starts[mid] <= off:
                    lo = mid
                else:
                    hi = mid - 1
            ln, col = lo + 1, off - starts[lo] + 1
            names = by_value.get(v, [])
            if len(names) == 1:
                rows.append((path, ln, col, toks[i][1], names[0]))
            else:
                missing.append((path, ln, col, toks[i][1], v))
    return rows, missing


def census():
    consts = all_constants()
    per_topic = {}
    for name, (v, topic) in consts.items():
        per_topic.setdefault(topic, set()).add(name)
    uses = {t: 0 for t in per_topic}
    for path in src_files():
        for k, t, _ in lex(read(os.path.join(ROOT, path))):
            if k == "id" and t in consts:
                uses[consts[t][1]] += 1
    left = {}
    for fam in FAMILIES:
        rows, _ = scan(fam)
        left[fam] = len(rows)
    return {"constants": {t: len(n) for t, n in sorted(per_topic.items())},
            "uses": dict(sorted(uses.items())),
            "literals_left": left}


def check():
    consts = all_constants()
    problems = []
    logged = {}
    if os.path.exists(LOG):
        with open(LOG, encoding="utf-8") as f:
            for r in csv.DictReader(f):
                logged[r["constant"]] = r
    for name, (v, topic) in sorted(consts.items()):
        r = logged.get(name)
        if not r:
            problems.append("%s: defined in %s but not logged" % (name, topic))
        elif int_value(r["value"])[0] != v or r["header"] != "include/constants/%s.h" % topic:
            problems.append("%s: logged as %s in %s" % (name, r["value"], r["header"]))
        elif not r["evidence"].strip():
            problems.append("%s: no evidence" % name)
    for name in logged:
        if name not in consts:
            problems.append("%s: logged but not defined" % name)
    for p in problems:
        print("  PROBLEM: %s" % p)
    if problems:
        raise ConstError("include/constants/ and %s disagree" % rel(LOG))
    print("check: OK - %d constants, every one logged with evidence" % len(consts))


# ---- --verify-cpp -----------------------------------------------------------------

CPP_SCRIPT = r"""
cd /w
for tree in a b; do
  for f in $(cd $tree && ls src/*.c src/data/*.c tools/header_smoke*.c 2>/dev/null); do
    o=/w/out/$tree/$(echo $f | tr / _)
    (cd $tree && cpp -P -I include "$f" 2>&1) | tr -s ' \t\n' '   ' > "$o"
  done
done
"""


def same_tokens(x, y):
    """None if the token streams of x and y differ only by integer literals
    of equal value, else a description of the first difference; also the
    number of respelled literals."""
    a, b = lex(x), lex(y)
    if len(a) != len(b):
        return "token count %d != %d" % (len(a), len(b)), 0
    respelled = 0
    for (ka, ta, _), (kb, tb, _) in zip(a, b):
        if ta == tb:
            continue
        if ka == kb == "num":
            va, sa = int_value(ta)
            vb, sb = int_value(tb)
            if va == vb and sa == sb and va <= MAX_VALUE:
                respelled += 1
                continue
        return "%r != %r" % (ta, tb), respelled
    return None, respelled


def verify_cpp(ref):
    os.makedirs(os.path.join(ROOT, "build"), exist_ok=True)
    tmp = tempfile.mkdtemp(prefix="constants_cpp_", dir=os.path.join(ROOT, "build"))
    try:
        a = os.path.join(tmp, "a")
        b = os.path.join(tmp, "b")
        os.makedirs(a)
        os.makedirs(os.path.join(tmp, "out", "a"))
        os.makedirs(os.path.join(tmp, "out", "b"))
        arch = subprocess.run(["git", "archive", ref, "src", "include", "tools/header_smoke.c",
                               "tools/header_smoke_game.c"], cwd=ROOT, stdout=subprocess.PIPE)
        if arch.returncode:
            raise ConstError("git archive %s failed" % ref)
        subprocess.run(["tar", "-x", "-C", a], input=arch.stdout, check=True)
        os.makedirs(os.path.join(b, "tools"))
        for d in ("src", "include"):
            shutil.copytree(os.path.join(ROOT, d), os.path.join(b, d))
        for f in ("header_smoke.c", "header_smoke_game.c"):
            shutil.copy(os.path.join(ROOT, "tools", f), os.path.join(b, "tools", f))
        res = subprocess.run(["docker", "run", "--rm", "-v", "%s:/w" % tmp, IMAGE, "bash", "-c",
                              CPP_SCRIPT], stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                             universal_newlines=True)
        if res.returncode:
            raise ConstError("cpp run failed: %s" % res.stdout[-2000:])
        oa = sorted(os.listdir(os.path.join(tmp, "out", "a")))
        ob = sorted(os.listdir(os.path.join(tmp, "out", "b")))
        problems = []
        if oa != ob:
            problems.append("translation units differ: %s" % sorted(set(oa) ^ set(ob)))
        same = identical = respelled = 0
        for n in sorted(set(oa) & set(ob)):
            x = read(os.path.join(tmp, "out", "a", n))
            y = read(os.path.join(tmp, "out", "b", n))
            if x == y:
                same += 1
                identical += 1
                continue
            diff, k = same_tokens(x, y)
            if diff:
                problems.append("%s: %s" % (n.replace("_", "/", 1), diff))
            else:
                same += 1
                respelled += k
        print("verify-cpp %s: %d translation units, %d identical after cpp -P, %d more equal "
              "with %d integer literals respelled (same value)"
              % (ref, len(ob), identical, same - identical, respelled))
        if problems:
            for p in problems:
                print("  PROBLEM: %s" % p)
            raise ConstError("the tree preprocesses differently from %s" % ref)
        print("verify-cpp: OK - every translation unit preprocesses to the same tokens and values")
    finally:
        shutil.rmtree(tmp, ignore_errors=True)


# ---- main -----------------------------------------------------------------------

def read_csv(path):
    with open(path, encoding="utf-8") as f:
        return list(csv.DictReader(f))


def main():
    ap = argparse.ArgumentParser(description=__doc__.split("\n\n")[0])
    ap.add_argument("--defs")
    ap.add_argument("--preambles", help="JSON {topic: preamble text} for new headers")
    ap.add_argument("--sites")
    ap.add_argument("--scan")
    ap.add_argument("--bitexprs", help="a bits family: its complement / union sites (second form)")
    ap.add_argument("--out")
    ap.add_argument("--verify-cpp")
    ap.add_argument("--census", action="store_true")
    ap.add_argument("--json", action="store_true")
    ap.add_argument("--check", action="store_true")
    ap.add_argument("--write", action="store_true")
    args = ap.parse_args()
    try:
        if args.verify_cpp:
            verify_cpp(args.verify_cpp)
        elif args.defs or args.sites:
            consts = all_constants()
            if args.defs:
                rows = read_csv(args.defs)
                pre = json.load(open(args.preambles)) if args.preambles else None
                apply_defs(rows, args.write, pre)
                for r in rows:
                    consts[r["constant"]] = (int_value(r["value"])[0], r["header"])
            if args.sites:
                apply_sites(read_csv(args.sites), args.write, consts)
        elif args.bitexprs:
            rows, other = bit_expr_sites(args.bitexprs)
            out = open(args.out, "w", newline="") if args.out else sys.stdout
            w = csv.writer(out, lineterminator="\n")
            w.writerow(["file", "line", "col", "literal", "constant"])
            for r in rows:
                w.writerow(r)
            print("bitexprs %s: %d complement / union sites, %d other multi-bit literals"
                  % (args.bitexprs, len(rows), len(other)), file=sys.stderr)
            for o in other:
                print("  not of proven bits: %s:%d:%d %s" % o, file=sys.stderr)
        elif args.scan:
            rows, missing = scan(args.scan)
            out = open(args.out, "w", newline="") if args.out else sys.stdout
            w = csv.writer(out, lineterminator="\n")
            w.writerow(["file", "line", "col", "literal", "constant"])
            for r in rows:
                w.writerow(r)
            print("scan %s: %d sites with a constant, %d without" % (args.scan, len(rows),
                                                                     len(missing)), file=sys.stderr)
            for m in missing:
                print("  no constant: %s:%d:%d %s (%d)" % m, file=sys.stderr)
        elif args.census:
            c = census()
            if args.json:
                print(json.dumps(c, indent=1))
            else:
                for t, n in c["constants"].items():
                    print("%-12s %4d constants, %5d sites" % (t, n, c["uses"][t]))
                for f, n in c["literals_left"].items():
                    print("family %s: %d literals left where a constant exists" % (f, n))
        elif args.check:
            check()
        else:
            ap.print_help()
    except ConstError as e:
        sys.exit("constants.py: %s" % e)


if __name__ == "__main__":
    main()
