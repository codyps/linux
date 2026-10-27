/* SPDX-License-Identifier: GPL-2.0 */
/* Build the recovered second scan with the OEM GCC 5.3 code schedule.
 * The GCC7-pass declarations keep crc_wrong in its existing shared object. */
#define TRIMUI_NFTL_GCC7_PASS
#define TRIMUI_NFTL_GC_FIRST_EXTERNAL
#define TRIMUI_NFTL_SECOND_SCAN_GCC53_BUILD
#define TRIMUI_NFTL_SCAN_BLOCKS_GCC53_BUILD
#include "nftl_recovered.c"
