/* SPDX-License-Identifier: GPL-2.0 */
/* Select the stock GCC 5.3 priority-GC body with its natural C locals. */
#define TRIMUI_NFTL_GCC7_PASS
#define TRIMUI_NFTL_GC_FIRST_EXTERNAL
#define TRIMUI_NFTL_STATIC_DONE_GCC53_BUILD
#define TRIMUI_NFTL_PRIO_GC_GCC53_BUILD
#include "nftl_recovered.c"
