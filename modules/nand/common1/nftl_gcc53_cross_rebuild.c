/* SPDX-License-Identifier: GPL-2.0 */
/* Compile only the recovered OEM cross-talk rebuild body with GCC 5.3. */
#define TRIMUI_NFTL_GCC7_PASS
#define TRIMUI_NFTL_GC_FIRST_EXTERNAL
#define TRIMUI_NFTL_CROSS_REBUILD_GCC53_BUILD
#include "nftl_recovered.c"
