/* SPDX-License-Identifier: GPL-2.0 */
/* Keep ECC and its private dump helper in their native GCC5 C profile. */
#define TRIMUI_NFTL_GCC7_PASS
#define TRIMUI_NFTL_ECC_GCC53_BUILD
#define TRIMUI_NFTL_GC_FIRST_EXTERNAL
#define TRIMUI_NFTL_SPECIAL_PAGE_END_SPLIT
#include "nftl_recovered.c"
