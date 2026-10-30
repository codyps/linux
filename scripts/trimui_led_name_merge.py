#!/usr/bin/env python3
"""Mark the Brick's C effect-name tail as mergeable NUL strings.

GCC 7 emits a named const struct in .rodata.str1.1 with SHF_ALLOC but no
SHF_MERGE/SHF_STRINGS. objcopy can set those flags, but it leaves sh_entsize
zero. The ELF linker then drops merging for the entire combined section.
Validate the exact 74-byte C object and set its entry size to one byte before
Kbuild's partial link. This is build metadata, not a patch to kernel code.
TODO: Recover an original source arrangement that emits the OEM section
without this post-compile metadata correction.
"""

import os
from pathlib import Path
import struct
import sys
import tempfile


TAIL = (b"linear\0breath\0sniff\0static\0blink1\0blink2\0blink3\0"
        b"tg5040_exterme_brightness\0")
SECTION = b".rodata.str1.1"


def section_header(data, number, table_offset, header_size):
    at = table_offset + number * header_size
    assert at + 64 <= len(data)
    return at, struct.unpack_from("<IIQQQQIIQQ", data, at)


def main(path):
    source = Path(path)
    data = bytearray(source.read_bytes())
    assert data[:6] == b"\x7fELF\x02\x01" and struct.unpack_from("<H", data, 18)[0] == 183
    table_offset = struct.unpack_from("<Q", data, 40)[0]
    header_size = struct.unpack_from("<H", data, 58)[0]
    count = struct.unpack_from("<H", data, 60)[0]
    names_index = struct.unpack_from("<H", data, 62)[0]
    assert header_size == 64 and 0 < names_index < count
    _, names_header = section_header(data, names_index, table_offset, header_size)
    names_start, names_size = names_header[4], names_header[5]
    names = data[names_start:names_start + names_size]
    matches = []
    for index in range(count):
        offset, row = section_header(data, index, table_offset, header_size)
        name_start = row[0]
        if names[name_start:names.find(0, name_start)] == SECTION:
            matches.append((offset, row))
    assert len(matches) == 1
    offset, row = matches[0]
    assert row[1] == 1 and row[2] == 0x32 and row[5] == len(TAIL) == 74
    assert row[8] == 1 and row[9] == 0
    assert data[row[4]:row[4] + row[5]] == TAIL
    struct.pack_into("<Q", data, offset + 56, 1)
    with tempfile.NamedTemporaryFile(dir=source.parent, prefix=".led-name-",
                                     delete=False) as stream:
        temporary = Path(stream.name)
        stream.write(data)
    try:
        os.chmod(temporary, source.stat().st_mode)
        os.replace(temporary, source)
    finally:
        temporary.unlink(missing_ok=True)


if __name__ == "__main__":
    assert len(sys.argv) == 2
    main(sys.argv[1])
