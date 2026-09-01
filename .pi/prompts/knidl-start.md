---
description: Start a safe, autonomous KnIDL development task
argument-hint: "[module, function, issue, or goal]"
---
Use the knidl-decomp skill for this task. Goal: ${@:-select the next eligible unresolved unit using the repository's recommended order}.

First run `python3 tools/pi_harness.py preflight` and `status`. Read all mandatory
project instructions before modifying files. Preserve any unrelated working-tree
changes. If no target was supplied, use `next` and the module map to select one
small, unowned function rather than making broad speculative edits.

Carry the selected task through the byte-exact matching loop. Do not carve or
commit until `fnmatch.sh` reports MATCH. At every terminal outcome, state the
exact range, evidence, changed files, validation result, and the next bounded
step. If prerequisites (especially the legal local baserom) are absent, do the
safe analysis possible and clearly report the blocked verification.
