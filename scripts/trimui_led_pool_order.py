#!/usr/bin/env python3
"""Reorder three OEM LED literals in an ELF relocatable C object.

The driver remains C. GCC 7 emits these three literals in a different order
from the shipped Image even when their source functions are moved; moving the
functions also changes verified executable bodies. This link-layout step
changes only the mergeable string section and its section-relative RELA
addends. Fail closed if the compiler's input shape changes.
"""

from bisect import bisect_right
from hashlib import sha256
from pathlib import Path
import struct
import sys


EXPECTED_POOL_SHA256 = "b258e72e4e80be304c9a0aae1a9891c4d69ccca1623aceb483a477ab4fee8df8"
TIMEOUT = b"\x013%s()%d - led xfer timeout\n"
COMPLETE = b"\x014%s()%d - xfer complete\n"
PINCTRL = b"\x013%s()%d - pinctrl_lookup_state(%s) failed! return %p\n"
THREAD = b"ledc thread error\n"
AXP = b"axp2202-bldo3"
ENABLE = b"\x013%s()%d - enable regulator %s failed!\n"
SECTION = struct.Struct("<IIQQQQIIQQ")
RELA = struct.Struct("<QQq")


def sections(blob):
    assert blob[:4] == b"\x7fELF" and blob[4:6] == b"\x02\x01"
    assert struct.unpack_from("<HH", blob, 16) == (1, 183)  # ET_REL, AArch64
    offset = struct.unpack_from("<Q", blob, 0x28)[0]
    stride, count, names = struct.unpack_from("<HHH", blob, 0x3a)
    assert stride == SECTION.size and count > 0 and names < count
    rows = [SECTION.unpack_from(blob, offset + n * stride)
            for n in range(count)]
    strings = rows[names]
    names_blob = blob[strings[4]:strings[4] + strings[5]]

    def name(row):
        begin = row[0]
        end = names_blob.index(0, begin)
        return names_blob[begin:end].decode("ascii")

    return [(name(row), row) for row in rows]


def reorder(pool):
    assert sha256(pool).hexdigest() == EXPECTED_POOL_SHA256
    items = pool.split(b"\0")
    assert len(items) == 158 and items[-1] == b""
    items.pop()
    assert len(items) == len(set(items)) == 157
    assert [items.index(value) for value in
            (PINCTRL, TIMEOUT, COMPLETE, ENABLE, AXP, THREAD)] == \
           [17, 26, 27, 37, 94, 93]
    moved = items.copy()
    for value in (TIMEOUT, COMPLETE, ENABLE):
        moved.remove(value)
    before_pinctrl = moved.index(PINCTRL)
    moved[before_pinctrl:before_pinctrl] = [TIMEOUT, COMPLETE]
    # Explicit create/error/wake C now emits THREAD immediately before AXP.
    # Preserve that native order; only ENABLE still needs moving here.
    assert moved.index(AXP) == moved.index(THREAD) + 1
    after_axp = moved.index(AXP) + 1
    moved[after_axp:after_axp] = [ENABLE]
    old_starts, new_starts = {}, {}
    for values, starts in ((items, old_starts), (moved, new_starts)):
        cursor = 0
        for value in values:
            starts[value] = cursor
            cursor += len(value) + 1
        assert cursor == len(pool)
    new_pool = b"\0".join(moved) + b"\0"
    assert len(new_pool) == len(pool)
    starts = sorted(old_starts.values())
    by_start = {start: value for value, start in old_starts.items()}

    def remap(addend):
        assert 0 <= addend < len(pool)
        start = starts[bisect_right(starts, addend) - 1]
        value = by_start[start]
        relocated = new_starts[value] + addend - start
        assert pool[addend:pool.index(b"\0", addend) + 1] == \
               new_pool[relocated:new_pool.index(b"\0", relocated) + 1]
        return relocated

    return new_pool, remap


def main(source, destination):
    data = bytearray(Path(source).read_bytes())
    rows = sections(data)
    matches = [(number, row) for number, (name, row) in enumerate(rows)
               if name == ".rodata.str1.1"]
    assert len(matches) == 1
    pool_index, pool_section = matches[0]
    assert (pool_section[1], pool_section[2], pool_section[9]) == (1, 0x32, 1)
    pool_begin, pool_size = pool_section[4:6]
    assert pool_size == 5804
    new_pool, remap = reorder(bytes(data[pool_begin:pool_begin + pool_size]))
    relocation_count = 0
    addends = set()
    for _, section in rows:
        if section[1] != 4:  # SHT_RELA
            continue
        begin, size, symtab_index, stride = (section[4], section[5],
                                              section[6], section[9])
        assert stride == RELA.size and size % stride == 0
        symtab = rows[symtab_index][1]
        assert symtab[1] == 2 and symtab[9] == 24
        for index in range(size // stride):
            entry = begin + index * stride
            _, info, addend = RELA.unpack_from(data, entry)
            symbol_index = info >> 32
            symbol = symtab[4] + symbol_index * 24
            assert symbol + 24 <= symtab[4] + symtab[5]
            target = struct.unpack_from("<H", data, symbol + 6)[0]
            if target != pool_index:
                continue
            assert struct.unpack_from("<Q", data, symbol + 8)[0] == 0
            addends.add(addend)
            RELA.pack_into(data, entry, RELA.unpack_from(data, entry)[0],
                           info, remap(addend))
            relocation_count += 1
    assert relocation_count == 435 and len(addends) == 157
    data[pool_begin:pool_begin + pool_size] = new_pool
    assert Path(source).resolve() != Path(destination).resolve()
    Path(destination).write_bytes(data)


if __name__ == "__main__":
    assert len(sys.argv) == 3
    main(sys.argv[1], sys.argv[2])
