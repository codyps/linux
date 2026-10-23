/* SPDX-License-Identifier: GPL-2.0 */
/* Recompile recovered NFTL with the OEM compiler, retaining only the
 * Micron retry initializer through scripts/trimui_nftl_select_cc.py. */
#define TRIMUI_NFTL_MICRON_INIT_GCC53_BUILD
#include "nftl_recovered.c"
