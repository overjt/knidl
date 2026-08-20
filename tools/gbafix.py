#!/usr/bin/env python3
"""Minimal gbafix: writes GBA cartridge header fields and computes the
complement check byte. Fields default to Kirby: Nightmare in Dream Land (USA)."""

import argparse
import sys


def main() -> None:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("rom")
    parser.add_argument("-t", "--title", default="AGB KIRBY DX")
    parser.add_argument("-c", "--code", default="A7KE")
    parser.add_argument("-m", "--maker", default="01")
    parser.add_argument("-r", "--version", type=lambda x: int(x, 0), default=0)
    args = parser.parse_args()

    with open(args.rom, "r+b") as f:
        data = bytearray(f.read())
    if len(data) < 0xC0:
        sys.exit("error: ROM smaller than the GBA header")

    def put(offset: int, text: str, size: int) -> None:
        blob = text.encode("ascii")[:size]
        data[offset : offset + size] = blob + b"\0" * (size - len(blob))

    put(0xA0, args.title, 12)
    put(0xAC, args.code, 4)
    put(0xB0, args.maker, 2)
    data[0xB2] = 0x96
    data[0xBC] = args.version
    data[0xBD] = (-(0x19 + sum(data[0xA0:0xBD]))) & 0xFF

    with open(args.rom, "wb") as f:
        f.write(data)


if __name__ == "__main__":
    main()
