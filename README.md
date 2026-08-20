# Kirby: Nightmare in Dream Land (USA) matching decompilation

This project aims to recreate **Kirby: Nightmare in Dream Land** (GBA) source code so that the built ROM matches the original USA cartridge release byte-for-byte.

- Target ROM: **USA** (`A7KE`)
- Built output: `knidl.gba`
- Expected SHA-1: `37a476567d133c146fee6b5e2eb0b07a215da6b0`

## Build and compare (Docker only)

All compilation is containerized.

```bash
make image
make
make compare
```

- `make image`: builds the `knidl-builder` Docker image.
- `make`: builds `knidl.gba`.
- `make compare`: verifies `knidl.gba` against `knidl.sha1` (fails on mismatch).

See `INSTALL.md` for full setup steps and `baserom.gba` requirements.

## Progress tracking

After a successful build, print code/data progress from `build/knidl.map`:

```bash
make progress
```

Equivalent direct command:

```bash
perl tools/calcrom.pl build/knidl.map
```

## ROM/header facts

- Internal title: `AGB KIRBY DX`
- Game code: `A7KE`
- Maker code: `01`
- Software version: `0`

## Legal

- This repository is **not affiliated with Nintendo or HAL Laboratory**.
- No ROM dumps or copyrighted game assets are included.
- You must dump your own legally obtained cartridge and provide it as `baserom.gba`.
- No OSS license is provided (all rights reserved by default, following pret convention).