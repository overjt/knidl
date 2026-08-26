# decomp.dev integration

This repository generates progress reports in the
[objdiff report schema v2](https://github.com/encounter/objdiff/blob/main/objdiff-core/protos/report.proto)
(JSON flavour) and publishes them as GitHub Actions artifacts for
[decomp.dev](https://decomp.dev), the decompilation-community progress hub.

## How it works

```
docs/analysis/segments.txt  ─┐
docs/analysis/symbols.csv   ─┼─►  tools/gen_report.py  ──►  report.json
docs/analysis/module-map.csv─┘            │
                                          ▼
                        .github/workflows/report.yml  (push to master)
                                          │
                                          ▼
                        artifact  A7KE_report  (report.json inside)
                                          │
                                          ▼
                        decomp.dev workflow-run downloader ingests it
```

- **`tools/gen_report.py`** derives the report from the repo's own ground
  truth — no baserom or toolchain needed:
  - a function is **matched** iff its VMA lies inside a `c_code` segment
    (exactly the ranges `linker.ld` pins to `build/src/<name>.o`; the
    byte-exactness of every one of those ranges is enforced by
    `make compare` in CI, so "in c_code" ⇒ "verified matching C"),
  - units follow the functional clustering of `module-map.csv`
    (M01..M38) plus one unit per early carve-out / SDK segment,
  - hand-written asm zones that stay asm **by design** (SDK libc,
    `svc` thunks, m4a engine core, task-switch helpers) are reported
    as their own units suffixed `[asm]`, with honest 0% — they are
    excluded from the "decompilable" expectation, not silently
    counted as done.
- **`report.json`** (gitignored, generated) is validated against the
  schema in CI before upload.
- **`.github/workflows/report.yml`** runs on every push to `master`,
  uploads the artifact as **`A7KE_report`** — decomp.dev scans workflow
  runs of registered projects and looks for artifacts named
  `<version>_report` (or `report`/`progress`), then parses the JSON with
  `Report::parse` + `migrate()` (JSON schema v2 is accepted natively;
  protobuf is not required).

## After making the repository public

1. **Register**: sign in at decomp.dev with GitHub (owner-side admin of
   this repo) → https://decomp.dev/manage/new → add `overjt/<repo>`.
2. **Install the GitHub App** (recommended): https://github.com/apps/decomp-dev
   → install for this repository. This switches decomp.dev from 5-minute
   polling to instant workflow-run webhooks, and enables automatic
   PR progress comments (configure per-project in /manage).
3. **Version**: the artifact is named after the game code (`A7KE`), which
   becomes the report "version" tab on the project page.
4. The badge in the README starts rendering as soon as the first report
   is ingested: `https://decomp.dev/<owner>/<repo>?mode=shield`.

## Local use

```sh
make report          # writes report.json (host Python, no Docker)
python3 tools/gen_report.py --check   # validate coverage without writing
```

`--check` fails closed if any function in `symbols.csv` is not covered by
exactly one unit — this catches segment/module map drift early (run it in
CI whenever the analysis CSVs change).

## Numbers semantics

| Measure | Meaning here |
| --- | --- |
| `matched_code` | bytes in `c_code` segments (verified matching C) |
| `total_code` | all code bytes in tracked segments (excl. pure data/pool) |
| `matched_functions` | symbols.csv functions inside `c_code` ranges |
| `[asm]` units | named-asm-forever zones; counted in totals at 0% |

Data (`8.3 MB` of assets) is intentionally not tracked as "code progress";
the ROM's data segments are incbins by design until asset tooling exists.
