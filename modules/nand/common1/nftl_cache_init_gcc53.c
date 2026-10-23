/* SPDX-License-Identifier: GPL-2.0 */
/* Recompile the recovered NFTL translation unit with the OEM compiler, then
 * retain only nftl_cache_init through scripts/trimui_nftl_select_cc.py. */
#define TRIMUI_NFTL_CACHE_GCC53_BUILD
#include "nftl_recovered.c"
