# Installation & Build Instructions

This document outlines how to set up the build environment and compile **Kirby: Nightmare in Dream Land** (USA).

## Prerequisites

- **Docker** (or Docker Desktop on macOS / Windows)
- A legally dumped copy of **Kirby: Nightmare in Dream Land (USA)**

## 1. Base ROM Placement

Place your dumped GBA ROM in the root directory of the repository and rename it to `baserom.gba`.

Verify that your ROM matches the expected SHA-1 hash:

```sh
sha1sum baserom.gba
```

Expected SHA-1 hash:
`37a476567d133c146fee6b5e2eb0b07a215da6b0`

> **Note:** `baserom.gba` is listed in `.gitignore` and must **never** be committed to the repository.

## 2. Docker Workflow

All builds and toolchains are managed inside Docker. Do not attempt to install native compilers or toolchains on your host system.

### Build the Docker Image

Before running build commands for the first time, build the `knidl-builder` image:

```sh
make image
```

### Build the ROM

To compile the ROM (`knidl.gba`):

```sh
make
```

### Verify SHA-1 Hash

To build the ROM and verify that its SHA-1 hash matches `knidl.sha1`:

```sh
make compare
```

If the built ROM matches `baserom.gba`, `make compare` will complete successfully.

### Check Progress

To display code/data decompilation progress and symbol statistics:

```sh
make progress
```

### Clean Build Artifacts

To remove built binaries and temporary build directory:

```sh
make clean
```
