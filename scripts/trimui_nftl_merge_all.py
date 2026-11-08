#!/usr/bin/env python3
"""Place the recovered C NAND functions in the stock text-section order.

The manifest is checked against the freshly compiled ELF objects. It contains
only names, sizes and ordering; no stock machine code is needed by the build.
"""

from collections import Counter
from pathlib import Path
import re
import sys

from trimui_nftl_order_text import text_sections


SAFE = re.compile(r"[A-Za-z0-9_./+-]+\Z")


def main():
    if len(sys.argv) < 5:
        raise SystemExit("usage: merge_all.py manifest output.lds object.o ...")
    manifest, output = map(Path, sys.argv[1:3])
    objects = [Path(argument) for argument in sys.argv[3:]]
    if not all(SAFE.fullmatch(str(path)) for path in
               (manifest, output, *objects)):
        raise ValueError("unsafe linker-script path")
    by_name = {path.name: path for path in objects}
    if len(by_name) != len(objects):
        raise ValueError("duplicate selected object basename")
    found = Counter((path.name, name, size)
                    for path in objects for name, size in text_sections(path))
    entries = []
    previous = -1
    for line in manifest.read_text().splitlines():
        if not line or line.startswith("#"):
            continue
        fields = line.split()
        if len(fields) != 4:
            raise ValueError("bad NAND order row: " + line)
        address_text, name, section, size_text = fields
        if not all(SAFE.fullmatch(field) for field in fields):
            raise ValueError("unsafe NAND order row: " + line)
        address = int(address_text, 16)
        size = int(size_text, 10)
        if address <= previous or size <= 0:
            raise ValueError("NAND manifest addresses/sizes not ordered")
        previous = address
        if name not in by_name:
            raise ValueError("unknown NAND source object: " + name)
        entries.append((name, section, size))
    expected = Counter(entries)
    if expected != found:
        missing = expected - found
        unexpected = found - expected
        raise ValueError("NAND text inventory changed: missing=%r extra=%r" %
                         (missing, unexpected))
    # The OEM-exact reclaim-completion leaf now has its own C producer.
    # Function inventory, sizes and stock order remain unchanged.
    if len(entries) != 357 or len(objects) != 126:
        raise ValueError("NAND source inventory changed")
    lines = ["/* Generated from recovered C objects and stock order manifest. */",
             "SECTIONS {", "  .text : {"]
    for name, section, _ in entries:
        lines.append("    %s(%s)" % (by_name[name], section))
    lines.extend(["  }", "}", ""])
    output.write_text("\n".join(lines))


if __name__ == "__main__":
    main()
