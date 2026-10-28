#!/usr/bin/env python3
"""Merge recovered arelink C functions in stock text order."""

from collections import Counter
from pathlib import Path
import re
import sys

from trimui_nftl_order_text import text_sections


SAFE = re.compile(r"[A-Za-z0-9_./+-]+\Z")


def main():
    if len(sys.argv) != 4:
        raise SystemExit("usage: order_text.py manifest raw.o output.lds")
    manifest, obj, output = map(Path, sys.argv[1:])
    if not all(SAFE.fullmatch(str(path)) for path in
               (manifest, obj, output)):
        raise ValueError("unsafe arelink linker-script path")
    expected = []
    previous = -1
    for line in manifest.read_text().splitlines():
        if not line or line.startswith("#"):
            continue
        fields = line.split()
        if len(fields) != 3 or not all(SAFE.fullmatch(field)
                                      for field in fields):
            raise ValueError("bad arelink order row: " + line)
        address_text, name, size_text = fields
        address = int(address_text, 16)
        size = int(size_text)
        if address <= previous or size <= 0 or not name.startswith(".text."):
            raise ValueError("arelink order address, size, or section invalid")
        previous = address
        expected.append((name, size))
    if len(expected) != 57 or sum(size for _, size in expected) != 13552:
        raise ValueError("arelink C text inventory changed")
    actual = Counter(text_sections(obj))
    if actual != Counter(expected):
        raise ValueError("arelink C text sections differ from stock manifest: "
                         "%r missing, %r unexpected" %
                         (Counter(expected)-actual, actual-Counter(expected)))
    lines = ["/* Generated from recovered C sections and stock order. */",
             "SECTIONS {", "  .text : {"]
    lines.extend("    %s(%s)" % (obj, name) for name, _ in expected)
    lines.extend(["  }", "}", ""])
    output.write_text("\n".join(lines))


if __name__ == "__main__":
    main()
