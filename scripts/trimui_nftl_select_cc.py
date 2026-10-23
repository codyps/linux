#!/usr/bin/env python3
"""Compile selected OEM NFTL bodies, retaining only their live ELF sections.

The recovered C translation unit also supplies functions already compiled by
the regular kernel compiler.  A GCC 5.3.1 pass is needed for the exact stock
initializers, so discard every other function from this pass before link.
"""

import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile


def main():
    compiler, linker, objcopy, entry = sys.argv[1:5]
    entries = entry.split(',')
    if not entries or any(not re.fullmatch(r'[A-Za-z_][A-Za-z_0-9]*', name)
                          for name in entries):
        raise SystemExit('expected comma-separated C symbol names')
    flags = sys.argv[6:]  # argv[5] is the separating '--'.
    if sys.argv[5] != '--' or '-o' not in flags:
        raise SystemExit('expected compiler linker objcopy ENTRY -- ... -o OUTPUT')
    target = Path(flags[flags.index('-o') + 1])
    with tempfile.TemporaryDirectory(prefix='.nftl-select-', dir=target.parent) as tmp:
        tmp = Path(tmp)
        whole = tmp / 'whole.o'
        retained = tmp / 'retained.o'
        final = tmp / 'final.o'
        compile_flags = flags.copy()
        compile_flags[compile_flags.index('-o') + 1] = str(whole)
        subprocess.run([compiler, *compile_flags], check=True)
        subprocess.run([linker, '-r', '--gc-sections',
                        '--entry=' + entries[0],
                        *['--undefined=' + name for name in entries[1:]],
                        '-o', str(retained),
                        str(whole)], check=True)
        # ld -r retains irrelevant undefined symbols in the symbol table even
        # after discarding their sections. Drop those unreferenced names.
        subprocess.run([objcopy, '--strip-unneeded', str(retained),
                        str(final)], check=True)
        os.replace(final, target)


if __name__ == '__main__':
    main()
