/* SPDX-License-Identifier: GPL-2.0 */
/* Included at the original declaration point during the GCC 5.3.1 pass. */

int nftl_cache_init(struct cache_init_zone *zone)
{
	struct cache_init_data *cache = &zone->cache;
	unsigned int index;

	cache->count = zone->configuration->count;
	cache->field0 = 0;
	cache->field1 = 0;
	cache->field2 = 0;
	/* These eight slots are also the links of entries 30 and 31. OEM
	 * clears them before the allocation loop, even when count is zero. */
	cache->entries[30].links[0] = 0;
	cache->entries[30].links[1] = 0;
	cache->entries[30].links[2] = 0;
	cache->entries[30].links[3] = 0;
	cache->entries[31].links[0] = 0;
	cache->entries[31].links[1] = 0;
	cache->entries[31].links[2] = 0;
	cache->entries[31].links[3] = 0;
	/* OEM does not clamp the count to the 32-entry array. Larger counts
	 * write through following zone fields; preserve the unchecked walk. */
	for (index = 0; index < cache->count; index++) {
		cache->entries[index].index = index;
		cache->entries[index].status = 255;
		cache->entries[index].page = -1;
		cache->entries[index].links[0] = 0;
		cache->entries[index].links[1] = 0;
		cache->entries[index].links[2] = 0;
		cache->entries[index].links[3] = 0;
		cache->entries[index].field0 = 0;
		cache->entries[index].field1 = 0;
		cache->entries[index].allocation = nftl_malloc(
			zone->geometry->allocation_size);
		if (!cache->entries[index].allocation) {
			/* OEM leaks all earlier allocations on this failure. */
			nand_print(nftl_ro_lc_413, nftl_ro_func_7884);
			return 1;
		}
	}
	cache->page_buffer = nftl_malloc(zone->geometry->allocation_size);
	if (!cache->page_buffer) {
		/* OEM leaves the node allocations live on this failure. */
		nand_print(nftl_ro_lc_414, nftl_ro_func_7884);
		return 1;
	}
	return 0;
}
