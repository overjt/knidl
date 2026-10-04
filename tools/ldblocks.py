"""linker.ld section blocks, shared by carve.py, carve_data.py and resegment.py.

Since #36 phase 2 run 2 the ROM sections follow each other without pinned
addresses (docs/data.md section 8.2): ld places each one at the end of the
previous one, aligned to its own input sections.  The address a matching
build needs is kept as an assertion right after the block, active when the
Makefile links with `--defsym MATCHING=1` (the default, and what CI and
`make compare` use):

    .name : {
        KEEP(*(.name))
    } > ROM
    ASSERT(!MATCHING || ADDR(.name) == 0x08XXXXXX, "MATCHING: .name moved")

A section that must stay at a fixed address in every build keeps the
address in its header instead (`.name 0x08XXXXXX : {`); only the cartridge
header does.  `block_re` matches both forms, with or without the assertion.
"""

import re


def block_re(name, comment=False):
    """Regex for the block of section `.name` (group 0 = the whole block,
    its assertion line included).  With comment=True a one-line comment
    directly above the block is part of the match."""
    head = r'(?:[ \t]*/\*[^\n]*\*/[ \t]*\n)?' if comment else ''
    return re.compile(
        head
        + r'[ \t]*\.%s(?:[ \t]+0x[0-9A-Fa-f]+)?[ \t]*:[ \t]*\{[^}]*\}'
          r'[ \t]*>[ \t]*ROM'
          r'(?:[ \t]*\n[ \t]*ASSERT\(!MATCHING \|\| ADDR\(\.%s\) == '
          r'0x[0-9A-Fa-f]+, "[^"\n]*"\))?' % (re.escape(name), re.escape(name)))


def block(name, vma, body):
    """An unpinned section block with its matching-mode assertion.  `body` is
    the input-section line (without indentation or newline)."""
    return ('    .%s : {\n'
            '        %s\n'
            '    } > ROM\n'
            '    ASSERT(!MATCHING || ADDR(.%s) == 0x%08X, "MATCHING: .%s moved")'
            % (name, body, name, vma, name))


def data_block(name, vma):
    """The block of a split data/asm segment (its section .name)."""
    return block(name, vma, 'KEEP(*(.%s))' % name)


GROUP_RE = re.compile(r'^[ \t]*\.([A-Za-z0-9_]+)[ \t]*:[ \t]*\{([^}]*)\}', re.M)


def group_of(text, name):
    """The output section that lists segment `name` as one row of a group
    (tools/ldgroup.py), or None: its data pieces `KEEP(*(.name))` or its
    C pieces `obj(.name)` inside another section's block."""
    pat = re.compile(r'(?:KEEP\(\*\(|\()\.%s\)' % re.escape(name))
    for m in GROUP_RE.finditer(text):
        if m.group(1) != name and pat.search(m.group(2)):
            return m.group(1)
    return None


def not_found(text, name):
    """The error text for a segment whose own block is missing."""
    g = group_of(text, name)
    if g:
        return ('linker.ld section .%s is a row of the grouped output section '
                '.%s: run `python3 tools/ldgroup.py --ungroup %s --write` first '
                'and group the rows again afterwards (docs/data.md 5.2)'
                % (name, g, g))
    return 'linker.ld section .%s not found' % name


def check_not_group(block_text, name):
    """An error text if the block found for `name` is a group of rows
    (tools/ldgroup.py names a group after its first row, so block_re finds
    it under that name), else None.  A plain block lists one line of input
    sections."""
    body = block_text[block_text.index('{') + 1:block_text.index('}')]
    if len([ln for ln in body.splitlines() if ln.strip()]) > 1:
        return ('linker.ld section .%s is a grouped output section '
                '(tools/ldgroup.py): run `python3 tools/ldgroup.py --ungroup %s '
                '--write` first and group the rows again afterwards '
                '(docs/data.md 5.2)' % (name, name))
    return None
