/* SPDX-License-Identifier: GPL-2.0 */
/* Recompile recovered NFTL with the OEM compiler, retaining only the
 * NAND page-copy helper through scripts/trimui_nftl_select_cc.py. */
#define TRIMUI_NFTL_COPY_PAGE_GCC53_BUILD
#include "nftl_recovered.c"
