# Research Index — Kirby: Nightmare in Dream Land Matching Decompilation

Research date: 2026-08-20. Produced by four parallel research tasks (Orca orchestration run
`run_82f70b44d845`). All documents are in English and cite sources inline.

| Document | Scope |
| --- | --- |
| [prior-art.md](prior-art.md) | Existing NiDL reverse-engineering efforts, the KATAM sibling decomp, pret projects as the canonical template, what to copy/reuse |
| [toolchain-build.md](toolchain-build.md) | Compiler identification (agbcc family), per-file compiler boundaries, Docker strategies, pret-style Makefile/linker/compare loop |
| [tooling-pipeline.md](tooling-pipeline.md) | ROM splitting (splat vs pret), m2c, decomp.me, asm-differ/objdiff, permuter, progress tracking, CI patterns |
| [rom-and-bootstrap.md](rom-and-bootstrap.md) | ROM revisions/hashes (No-Intro), save type, recommended target ROM, repo bootstrap checklist |

## Key decisions (synthesized)

1. **Target ROM**: USA `A7KE` (8 MiB), SHA-1 `37a476567d133c146fee6b5e2eb0b07a215da6b0`.
   Note: internal codes are `A7K*`, not `AKT*`. Save type SRAM_V112 (32 KiB).
2. **No existing NiDL decompilation exists** — this is a greenfield project. The closest
   reference is **jiangzhengwenjz/katam** (Kirby & The Amazing Mirror, same engine lineage,
   active, ~145 decompiled modules).
3. **Toolchain**: pret-family `agbcc` (GCC 2.9-arm-000512); try zhade's `new_newlib_pret`
   fork first (it was needed to match KATAM), stock pret agbcc second. `old_agbcc` for
   m4a/SDK files. Validate empirically with 5–10 leaf functions on decomp.me before
   committing.
4. **Pipeline**: pret/KATAM-style Makefile (no splat — it has no GBA support), symbol-pinned
   `linker.ld`, `make compare` via sha1, m2c (gba target) + decomp.me for functions,
   asm-differ for diffs.
5. **Docker**: single pinned Debian/Ubuntu image with host deps + `arm-none-eabi` binutils;
   build agbcc from source inside the image (pret CI pattern). All compilation in Docker
   per repo policy.
6. **Repo layout**: mirror katam/pret (`src/`, `asm/`, `data/`, `include/`, `tools/`,
   `linker.ld`, `<game>.sha1`, gitignored `baserom.gba`).

## Recommended next steps

1. User places `baserom.gba` (USA) at repo root; verify SHA-1.
2. Scaffold repo skeleton (Makefile, linker.ld, Dockerfile, .gitignore, gbafix, crt0, header).
3. Bootstrap agbcc (fork `new_newlib_pret`) inside Docker; get a first shiftable build.
4. Validate compiler choice on decomp.me (leaf functions); request a NiDL preset upstream.
5. Begin decompilation order: header/crt0 → m4a boundary → task/object engine (use KATAM
   symbol knowledge as hypotheses, verify per function).
