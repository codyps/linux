/* SPDX-License-Identifier: GPL-2.0 */
/* Private NFTL initializer layouts recovered from OEM offsets. */
#ifndef NFTL_INITIALIZE_TYPES_H
#define NFTL_INITIALIZE_TYPES_H

struct nftl_zone_cap_view {
	unsigned char preceding[32];
	unsigned int capacity;
};

struct init_nand_parameters {
	unsigned char before_retry[2494];
	unsigned short retry_setting;
	unsigned char before_limits[2];
	unsigned short limit_a;
	unsigned short limit_b;
	unsigned char before_words[2];
	unsigned int max_erase_times;
	unsigned int erased_block_limit;
	unsigned int format_revision;
};
struct init_nand_device {
	unsigned char before_mode[2];
	unsigned short mode;
	unsigned char before_params[36];
	struct init_nand_parameters *params;
};
struct init_partition {
	unsigned char before_pages[34];
	unsigned short pages;
	unsigned char before_small[4];
	unsigned short small;
	unsigned char before_capacity[2];
	unsigned int capacity;
	unsigned char before_callbacks[16];
	void *read;
	void *write;
	struct init_nand_device *device;
};
struct init_nftl_config {
	unsigned short mode;
	unsigned short first;
	unsigned short second;
	unsigned short third;
	unsigned short fourth;
	unsigned short fifth;
	unsigned short limit_a;
	unsigned short eighth;
	unsigned short limit_b;
	unsigned short ninth;
	unsigned short tenth;
	unsigned short erase_limit;
	unsigned short interval;
	unsigned short device_mode;
	unsigned short boot_crc;
	unsigned short reserved;
	unsigned int revision;
};
struct init_nftl_geometry {
	unsigned char before_erase[16];
	unsigned short max_erase_times;
	unsigned char before_secondary[12];
	unsigned short secondary_erase_times;
};
struct init_nftl_zone {
	unsigned char before_geometry[8];
	struct init_nftl_geometry *geometry;
};
struct init_nftl_owner {
	unsigned int capacity;
	unsigned char before_partition[4];
	struct init_partition *partition;
	void *next;
	struct init_nftl_zone *zone;
	struct init_nftl_config *config;
	unsigned char before_callbacks[32];
	void (*read)(void);
	void (*write)(void);
	void (*flush)(void);
	void (*discard)(void);
	void (*shutdown)(void);
	void (*sector_read)(void);
	void (*sector_write)(void);
	void (*sector_flush)(void);
};
_Static_assert(__builtin_offsetof(struct init_nftl_owner, sector_flush) == 128,
	       "NFTL owner callback layout");
_Static_assert(__builtin_offsetof(struct init_nand_parameters, format_revision) == 2512,
	       "NFTL NAND parameter layout");
_Static_assert(__builtin_offsetof(struct init_partition, device) == 80,
	       "NFTL partition device layout");
#endif /* NFTL_INITIALIZE_TYPES_H */
