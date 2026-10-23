/* SPDX-License-Identifier: GPL-2.0 */
/* Recovered OEM cache layout shared by the two NFTL translation units. */
#ifndef NFTL_CACHE_INIT_LAYOUT_H
#define NFTL_CACHE_INIT_LAYOUT_H
struct cache_init_entry {
	unsigned short index;
	unsigned short status;
	unsigned short field0;
	unsigned short field1;
	unsigned int page;
	unsigned int untouched;
	void *allocation;
	void *links[4];
};
struct cache_init_data {
	unsigned short count;
	unsigned short field0;
	unsigned short field1;
	unsigned short field2;
	struct cache_init_entry entries[32];
	void *page_buffer;
};
struct cache_init_geometry {
	unsigned char before_allocation_size[26];
	unsigned short allocation_size;
};
struct cache_init_configuration {
	unsigned char before_count[22];
	unsigned short count;
};
struct cache_init_zone {
	unsigned char before_geometry[8];
	struct cache_init_geometry *geometry;
	unsigned char before_cache[1056];
	struct cache_init_data cache;
	unsigned char before_configuration[568];
	struct cache_init_configuration *configuration;
};
_Static_assert(sizeof(struct cache_init_entry) == 56, "cache entry stride");
_Static_assert(__builtin_offsetof(struct cache_init_entry, allocation) == 16,
	       "cache entry allocation offset");
_Static_assert(__builtin_offsetof(struct cache_init_data, page_buffer) == 1800,
	       "cache page buffer offset");
_Static_assert(__builtin_offsetof(struct cache_init_zone, cache) == 1072,
	       "zone cache offset");
_Static_assert(__builtin_offsetof(struct cache_init_zone, configuration) == 3448,
	       "zone configuration offset");


#endif
