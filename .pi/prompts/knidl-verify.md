---
description: Verify the current KnIDL tree and report trustworthy evidence
---
Use the knidl-decomp skill. Do not change source. Run
`python3 tools/pi_harness.py preflight`, then run the strongest checks available:
`make check-headers`, `make report`, and, if the legal local baserom exists,
`python3 tools/pi_harness.py verify --full`. Distinguish passed checks from
checks skipped because the baserom is unavailable. Inspect the working tree and
report exactly what would need review before a PR.
