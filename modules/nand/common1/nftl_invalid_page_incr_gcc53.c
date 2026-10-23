/* SPDX-License-Identifier: GPL-2.0 */
/* Compile the recovered NFTL translation unit with the OEM compiler; retain
 * only phy_block_from_invalid_page_incr through the selector. */
#define TRIMUI_NFTL_INVALID_PAGE_GCC53_BUILD
#include "nftl_recovered.c"
