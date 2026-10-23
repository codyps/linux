/* SPDX-License-Identifier: GPL-2.0 */
/* Included at the OEM declaration point during the GCC 5.3.1 pass. */

__attribute__((optimize("-fno-stack-protector")))
int nftl_sector_cache_init(void *zone)
{
	struct sector_cache_init_view *cache =
		(struct sector_cache_init_view *)((unsigned char *)zone + 2880);
	unsigned int index;
	unsigned char *buffer;

	cache->capacity = 16;
	cache->count = 0;
	cache->buffer = 0;
	cache->buffer = nftl_malloc(8192);
	if (!cache->buffer) {
		/* OEM leaves the count and allocation fields changed on failure,
		 * and never clears a preexisting list or frees an old buffer. */
		nand_print(nftl_ro_lc_141, nftl_ro_func_6755);
		return 1;
	}
	cache->head.next = 0;
	cache->head.previous = 0;
	buffer = cache->buffer;
	for (index = 0; index < 16; index++) {
		cache->nodes[index].unknown = index;
		cache->nodes[index].flag = 255;
		cache->nodes[index].sector = -1;
		cache->nodes[index].next = 0;
		cache->nodes[index].previous = 0;
		cache->nodes[index].data = buffer;
		buffer += 512;
	}
	return 0;
}
