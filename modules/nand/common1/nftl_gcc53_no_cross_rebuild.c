/* SPDX-License-Identifier: GPL-2.0 */
/* Keep the OEM's separate erase and replay operation slots and GCC 5.3
 * stack order. The wrapper retains only this function at partial link. */
#define TRIMUI_NFTL_GCC7_PASS
#define TRIMUI_NFTL_GC_FIRST_EXTERNAL
#define TRIMUI_NFTL_NO_CROSS_REBUILD_GCC53_BUILD
#include "nftl_recovered.c"
