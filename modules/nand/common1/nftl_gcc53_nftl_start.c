/* SPDX-License-Identifier: GPL-2.0 */
/* Stock-shaped NFTL zone start, compiled by the OEM GCC 5.3.1. */
#include "nftl_ro_aliases.h"
#include "nftl_initialize_types.h"
#include "nftl_start_types.h"

extern void nand_print(const char *, ...);
extern void nand_print_dbg(const char *, ...);
extern void *nftl_malloc(unsigned int);
extern void *nand_memset(void *, int, unsigned int);
extern void *nand_memcpy(void *, const void *, unsigned int);
extern struct special_exit_info *g_nsi;
extern int zone_param_init(void *, unsigned short, unsigned int, unsigned int);
struct first_list_zone;
extern int build_zone_list_first(struct first_list_zone *, unsigned int);
struct build_zone;
extern int build_zone_list(struct build_zone *);
struct no_use_zone;
extern int is_no_use_device(struct no_use_zone *, unsigned int);
struct nftl_ops;
extern void nftl_ops_init(struct nftl_ops *);
struct cache_init_zone;
extern int nftl_cache_init(struct cache_init_zone *);

int nftl_start(struct init_nftl_owner *owner, struct init_nftl_config *config,
		struct init_nftl_zone **output, void *start_bytes,
		void *capacity_bytes, unsigned int area_bytes,
		unsigned int erase_bytes, unsigned int chips,
		struct init_partition *partition, unsigned int number,
		unsigned char mode)
{
	unsigned int i;
	unsigned int attempts = 0;
	struct start_zone *zone;
	unsigned int start, capacity, area, erase;
	unsigned int total, total_for_blocks, reserved, area_for_param, result;
	start_bytes = (void *)((unsigned long)start_bytes >> 9);
	capacity_bytes = (void *)((unsigned long)capacity_bytes >> 9);
	area_bytes >>= 9;
	erase_bytes >>= 9;
	start = (unsigned long)start_bytes;
	capacity = (unsigned long)capacity_bytes;
	area = area_bytes;
	/* Stock shifts the full 32-bit erase argument before narrowing its
	 * resulting geometry fields.  Premature u16 narrowing changes large
	 * direct-call inputs even though the initializer passes a u16 value. */
	erase = erase_bytes;

	/* Stock publishes then reloads the zone pointer.  Callback code can
	 * change *output before later call sites, which reload it again. */
	*output = nftl_malloc(3528);
	zone = (struct start_zone *)*output;
	if (!zone) {
		nand_print(nftl_ro_lc_474, nftl_ro_func_8536);
		return -2;
	}
	/* OEM reuses the zone on retry, leaking its previous geometry and
	 * block array when the first list build fails. */
	for (;;) {
		attempts++;
		nand_memset(zone, 0, 3528);
		zone->owner = owner;
		zone->config = config;
		/* Reload the published geometry pointer on every field access. */
		zone->geometry = nftl_malloc(40);
		if (!zone->geometry) {
			nand_print(nftl_ro_lc_475, nftl_ro_func_8536);
			return -2;
		}
		nand_memset(zone->geometry, 0, 8);
		/* Stock follows the global NAND-info pointers unchecked. */
		nand_memcpy(zone->geometry->vendor_id,
			    (char *)g_nsi->chips + 16, 8);
		zone->geometry->page_bytes = (unsigned short)(erase << 9);
		zone->geometry->chip_count = chips;
		/* The OEM emits AArch64 UDIV directly.  Its zero divisor result is
		 * zero; ISO C leaves division by zero undefined.  This fixed
		 * GCC 5.3.1 -O0 build is checked byte-for-byte against stock. */
		total = capacity / area;
		if (zone->config->limit_a >= total)
			return -1;
		zone->geometry->blocks = total;
		zone->geometry->pages = area / erase;
		zone->geometry->sectors_per_page = erase;
		zone->geometry->sectors = zone->geometry->sectors_per_page;
		zone->geometry->cache_limit = 3000;
		/* OEM copies total before the allocation and uses the copy for
		 * the sentinel loop; retain that call-boundary snapshot. */
		total_for_blocks = total;
		zone->geometry->block_array = nftl_malloc((unsigned int)((total_for_blocks + 1) << 6));
		if (!zone->geometry->block_array) {
			nand_print(nftl_ro_lc_476,
				   nftl_ro_func_8536);
			return -2;
		}
		/* The last sentinel block is not covered by this fill. */
		nand_memset(zone->geometry->block_array, -1, total_for_blocks << 6);
		for (i = 0; i < total_for_blocks + 1; i++) {
			zone->geometry->block_array[i].number = i;
			zone->geometry->block_array[i].page = 0;
			zone->geometry->block_array[i].state = 0;
			zone->geometry->block_array[i].tag = ~0U;
			zone->geometry->block_array[i].erase_count = 0;
			zone->geometry->block_array[i].links[0] = 0;
			zone->geometry->block_array[i].links[1] = 0;
			zone->geometry->block_array[i].links[2] = 0;
			zone->geometry->block_array[i].links[3] = 0;
			zone->geometry->block_array[i].links[4] = 0;
			zone->geometry->block_array[i].links[5] = 0;
		}
		nftl_ops_init((struct nftl_ops *)*output);
		reserved = start / area;
		zone->backup_blocks = (unsigned short)
			((unsigned short)total - (unsigned short)reserved);
		if (zone->backup_blocks < zone->config->limit_a) {
			nand_print(nftl_ro_lc_477,
				   zone->backup_blocks);
			return -2;
		}
		nand_print_dbg(nftl_ro_lc_478, total,
			       zone->backup_blocks);
		/* OEM snapshots area after the diagnostic callback. */
		area_for_param = area;
		result = zone_param_init(*output, 0,
			(total - zone->backup_blocks) * area_for_param,
			zone->backup_blocks * area_for_param);
		if (result) {
			nand_print(nftl_ro_lc_479);
			return result;
		}
		/* The stock frame loads stack argument nine, the NFTL number;
		 * the final mode byte is never used. */
		zone->mode = number;
		if (zone->config->device_mode)
			zone->flags |= 1;
		if (zone->config->fifth &&
		    is_no_use_device((struct no_use_zone *)*output, total) == 1) {
			result = build_zone_list_first(
				(struct first_list_zone *)*output, total);
			if (result) {
				nand_print(nftl_ro_lc_480);
				return result;
			}
		}
		result = build_zone_list((struct build_zone *)*output);
		if (result) {
			nand_print(nftl_ro_lc_481);
			if (attempts <= 1) {
				nand_print(nftl_ro_lc_482);
				continue;
			}
			return result;
		}
		/* OEM ignores cache-initialization failures. */
		nftl_cache_init((struct cache_init_zone *)*output);
		return 0;
	}
}
