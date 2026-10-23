/* SPDX-License-Identifier: GPL-2.0 */
/* Compile the recovered NFTL translation unit with the OEM compiler; retain
 * only nftl_sector_cache_init through scripts/trimui_nftl_select_cc.py. */
#define TRIMUI_NFTL_SECTOR_CACHE_GCC53_BUILD
#include "nftl_recovered.c"
