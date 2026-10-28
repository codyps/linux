#!/usr/bin/env python3
"""Merge separately compiled LED C functions in stock text order.

The manifest contains ordering and section sizes, not OEM machine code.
Reject source-body changes that invalidate its verified layout.
"""

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
        raise ValueError("unsafe LED linker-script path")
    sections = text_sections(obj)
    cold = [(name, size) for name, size in sections
            if name.startswith(".text.unlikely.")]
    if cold != [(".text.unlikely.output_mode_write.part.7", 48)]:
        raise ValueError("LED cold helper inventory changed: %r" % cold)
    # The OEM places this cold helper in .text.unlikely outside the main
    # 22,544-byte LED span. The partial link restores that section name.
    actual = Counter((name, size) for name, size in sections
                     if (name, size) not in cold)
    expected = []
    previous = -1
    for line in manifest.read_text().splitlines():
        if not line or line.startswith("#"):
            continue
        fields = line.split()
        if len(fields) != 3 or not all(SAFE.fullmatch(f) for f in fields):
            raise ValueError("bad LED order row: " + line)
        address_text, name, size_text = fields
        address = int(address_text, 16)
        size = int(size_text)
        if address <= previous or size <= 0 or not name.startswith(".text."):
            raise ValueError("LED order address, size, or section invalid")
        previous = address
        expected.append((name, size))
    if len(expected) != 141 or sum(size for _, size in expected) != 22544:
        raise ValueError("LED text inventory changed")
    if Counter(expected) != actual:
        raise ValueError("LED C section inventory differs from stock manifest: "
                         "%r missing, %r unexpected" %
                         (Counter(expected)-actual, actual-Counter(expected)))
    lines = ["/* Generated from selected C sections and stock order. */",
             "SECTIONS {", "  .text : {"]
    lines.extend("    %s(%s)" % (obj, name) for name, _ in expected)
    lines.extend(["  }", "}", ""])
    output.write_text("\n".join(lines))


if __name__ == "__main__":
    main()
