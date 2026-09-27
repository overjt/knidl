"""linker.ld section blocks, shared by carve.py, carve_data.py and resegment.py.

Since #36 phase 2 run 2 the ROM sections follow each other without pinned
addresses (docs/data.md section 8.2): ld places each one at the end of the
previous one, aligned to its own input sections.  The address a matching
build needs is kept as an assertion right after the block, active when the
Makefile links with `--defsym MATCHING=1` (the default, and what CI and
`make compare` use):

    .name : {
        KEEP(*(.name)) KEEP(*(.name.tail))
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
    """The block of a split data/asm segment (its .name and .name.tail)."""
    return block(name, vma, 'KEEP(*(.%s)) KEEP(*(.%s.tail))' % (name, name))
