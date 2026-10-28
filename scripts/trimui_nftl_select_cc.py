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

from trimui_nftl_order_text import text_sections


# These five selected objects feed the source-order linker script. Their named
# sections must stay separate until nftl_recovered_merged.o is constructed.
MERGED_TEXT_INPUTS = {
    'nftl_gcc53_second_scan.o',
    'nftl_gcc53_current_used.o',
    'nftl_gcc53_do_prio_gc.o',
    'nftl_gcc53_write_error_build.o',
    'nftl_gcc53_crc_wrong.o',
}


def main():
    compiler, linker, objcopy, entry = sys.argv[1:5]
    selection, separator, retained_names = entry.partition(':')
    entries = selection.split(',')
    keep_symbols = retained_names.split(',') if separator else []
    if not entries or any(not re.fullmatch(r'[A-Za-z_][A-Za-z_0-9]*', name)
                          for name in entries):
        raise SystemExit('expected comma-separated C symbol names')
    if separator and (not keep_symbols or any(
            not re.fullmatch(r'[A-Za-z_][A-Za-z_0-9]*', name)
            for name in keep_symbols)):
        raise SystemExit('expected comma-separated retained local symbols')
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
        subprocess.run([objcopy, '--strip-unneeded',
                        *['--keep-symbol=' + name for name in keep_symbols],
                        str(retained),
                        str(final)], check=True)
        if (target.parent.name == 'sun50iw10p1' and
                target.name not in MERGED_TEXT_INPUTS):
            # GCC 5's selected functions are already isolated by the GC pass.
            # Keep their C code in .text at this NAND object's normal link
            # position. Leaving .text.<name> orphaned moves 130 stock NAND
            # functions after the rest of the kernel's main text.
            names = [name for name, size in text_sections(final)
                     if name.startswith('.text.')]
            if names:
                subprocess.run([objcopy, *[
                    '--rename-section=' + name + '=.text'
                    for name in names], str(final)], check=True)
        os.replace(final, target)


if __name__ == '__main__':
    main()
