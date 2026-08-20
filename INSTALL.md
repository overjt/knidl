# INSTALL

## Requirements

- Docker
- A legally dumped USA **Kirby: Nightmare in Dream Land** ROM

Expected baserom SHA-1:

`37a476567d133c146fee6b5e2eb0b07a215da6b0`

## 1) Place `baserom.gba`

Put your ROM at repository root and name it exactly:

`baserom.gba`

This file is intentionally gitignored and must never be committed.

## 2) Verify baserom hash

From repository root:

```bash
sha1sum baserom.gba
```

The output hash must match:

`37a476567d133c146fee6b5e2eb0b07a215da6b0`

## 3) Build with Docker

```bash
make image
make
```

## 4) Compare against target ROM hash

```bash
make compare
```

`make compare` fails if the built ROM hash does not match `knidl.sha1`.
