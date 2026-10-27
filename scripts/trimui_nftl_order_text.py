#!/usr/bin/env python3
"""Keep selected native GCC 5.3 bodies at their source-order NFTL places.

GCC 7 emits the other recovered C functions in source order.  Per-function
sections retain their bodies byte for byte, so this script restores that order
while substituting the selected GCC 5.3 bodies. It reads only ELF64 little-endian
section headers; no build-host Python packages are needed.
"""

import pathlib
import re
import struct
import sys


def text_sections(path):
    data = path.read_bytes()
    if data[:6] != b"\x7fELF\x02\x01":
        raise ValueError("expected little-endian ELF64: " + str(path))
    offset = struct.unpack_from("<Q", data, 0x28)[0]
    size, count, names_index = struct.unpack_from("<HHH", data, 0x3a)
    if size != 64 or not count or names_index >= count:
        raise ValueError("unsupported section table: " + str(path))
    headers = [struct.unpack_from("<IIQQQQIIQQ", data, offset + i * size)
               for i in range(count)]
    names_header = headers[names_index]
    names = data[names_header[4]:names_header[4] + names_header[5]]
    result = []
    for header in headers:
        start = header[0]
        end = names.find(b"\0", start)
        if end < 0:
            raise ValueError("unterminated section name")
        name = names[start:end].decode("ascii")
        if name == ".text" or name.startswith(".text."):
            if header[2] & 4 and header[5]:
                result.append((name, header[5]))
    return result


def main():
    if len(sys.argv) not in (4, 5):
        raise SystemExit("usage: order_text.py gcc7.o native_scan.o [native_selector.o] output.lds")
    paths = list(map(pathlib.Path, sys.argv[1:]))
    gcc7, scan, output = paths[0], paths[1], paths[-1]
    selector = paths[2] if len(paths) == 4 else None
    for path in (gcc7, scan, *([selector] if selector else [])):
        if not re.fullmatch(r"[A-Za-z0-9_./+-]+", str(path)):
            raise ValueError("unsafe linker-script input path: " + str(path))
    sections = text_sections(gcc7)
    count = 219 if selector else 220
    if len(sections) != count or len({name for name, _ in sections}) != count:
        raise ValueError("recovered GCC 7 text section inventory changed")
    if sections.count((".text.build_zone_list", 200)) != 1:
        raise ValueError("missing or changed build_zone_list placement")
    if any(name == ".text.second_scan_all_blocks" for name, _ in sections):
        raise ValueError("GCC 7 still defines second_scan_all_blocks")
    if text_sections(scan) != [(".text.second_scan_all_blocks", 3124)]:
        raise ValueError("native second scan text inventory changed")
    if selector:
        if any(name == ".text.get_current_used_block" for name, _ in sections):
            raise ValueError("GCC 7 still defines current-used selector")
        if sections.count((".text.get_new_current_block", 316)) != 1:
            raise ValueError("missing selector insertion anchor")
        if text_sections(selector) != [(".text.get_current_used_block", 1440)]:
            raise ValueError("native current-used text inventory changed")
    lines = ["/* Generated from the two selected source objects. */",
             "SECTIONS {", "  .text : {"]
    for name, _ in sections:
        lines.append("    %s(%s)" % (gcc7, name))
        if name == ".text.build_zone_list":
            lines.append("    %s(.text.second_scan_all_blocks)" % scan)
        if selector and name == ".text.get_new_current_block":
            lines.append("    %s(.text.get_current_used_block)" % selector)
    lines.extend(["  }", "}", ""])
    output.write_text("\n".join(lines))


if __name__ == "__main__":
    main()
