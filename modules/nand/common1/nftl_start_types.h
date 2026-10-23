/* SPDX-License-Identifier: GPL-2.0 */
/* Private OEM NFTL zone-start layouts and NAND-info chain. */
#ifndef NFTL_START_TYPES_H
#define NFTL_START_TYPES_H

struct start_physical_block;
struct start_geometry {
	unsigned char vendor_id[8];
	unsigned char sectors_per_page;
	unsigned char before_page_bytes[1];
	unsigned char chip_count;
	unsigned char before_cache_limit[5];
	unsigned short cache_limit;
	unsigned char before_pages[2];
	unsigned short pages;
	unsigned char before_blocks[2];
	unsigned short blocks;
	unsigned short page_bytes;
	unsigned short sectors;
	unsigned char before_block_array[2];
	struct start_physical_block *block_array;
};
struct start_zone {
	struct init_nftl_owner *owner;
	struct start_geometry *geometry;
	unsigned char before_mode[8];
	unsigned char mode;
	unsigned char flags;
	unsigned char before_backup_blocks[4];
	unsigned short backup_blocks;
	unsigned char before_config[3448 - 32];
	struct init_nftl_config *config;
};
struct start_physical_block {
	unsigned short number;
	unsigned short state;
	unsigned short page;
	unsigned short erase_count;
	unsigned int tag;
	unsigned int padding;
	void *links[6];
};
_Static_assert(sizeof(struct start_physical_block) == 64,
	       "NFTL physical block stride");
_Static_assert(__builtin_offsetof(struct start_zone, config) == 3448,
	       "NFTL start configuration offset");
_Static_assert(__builtin_offsetof(struct start_geometry, block_array) == 32,
	       "NFTL start block-array offset");
struct special_exit_chip {
	struct special_exit_chip *next;
};
struct special_exit_info {
	void *before_chips;
	struct special_exit_chip *chips;
};
#endif /* NFTL_START_TYPES_H */
