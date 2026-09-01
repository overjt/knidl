---
description: Decompile and land one verified KnIDL function or module range
argument-hint: "<module ID, function name, or address range>"
---
Use the knidl-decomp skill to decompile `$@`. Establish the authoritative
half-open range from `symbols.csv`/`segments.txt`, inspect its containing asm
and call context, and use the exact current Makefile compiler recipe. Work in
one bounded function first unless this target is an already-approved batch.

Iterate with fnmatch until it says MATCH. Only then dry-run and apply carve,
regenerate symbols/split, and verify a clean Docker `make compare`. Update the
minimal required maps and record only genuinely new lessons. Do not merge a PR.
