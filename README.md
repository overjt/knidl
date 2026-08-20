# Kirby: Nightmare in Dream Land

This is a matching decompilation of **Kirby: Nightmare in Dream Land** for the Game Boy Advance (USA release).

The target output is a ROM that matches the original retail release byte-for-byte:

- **Game Title:** `AGB KIRBY DX`
- **Game Code:** `A7KE`
- **Maker Code:** `01`
- **Version:** `0`
- **Target SHA-1:** `37a476567d133c146fee6b5e2eb0b07a215da6b0`

## Disclaimer

This repository is **not affiliated with Nintendo or HAL Laboratory**.

No ROM file, copyrighted code, or game assets are included in this repository. You must provide your own legally-dumped GBA cartridge ROM (`baserom.gba`) to build and verify the target binary.

## License

No open-source license is provided (all rights reserved per pret decompilation conventions).

## Quick Start (Docker)

All compilation and verification run inside a Docker container. Host toolchains (such as `arm-none-eabi` binutils or `agbcc`) are not required.

Detailed build and setup instructions can be found in [INSTALL.md](INSTALL.md).

### Basic Commands

Build the Docker build environment:
```sh
make image
```

Build `knidl.gba`:
```sh
make
```

Build and verify the output ROM against `knidl.sha1`:
```sh
make compare
```

Check decompilation and documentation progress:
```sh
make progress
```
