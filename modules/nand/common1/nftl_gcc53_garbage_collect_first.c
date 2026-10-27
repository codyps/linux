/* SPDX-License-Identifier: GPL-2.0 */
/* Compile first GC alone with the OEM compiler. The views below reproduce
 * the offsets shared with nftl_recovered.c and are checked here as well. */
#include "nftl_ro_aliases.h"

struct physic_op_param {
	unsigned short plane;
	unsigned short block;
	unsigned short page;
	unsigned short reserved;
	void *data;
	void *oob;
};
struct invalid_page_location {
	unsigned short page : 12;
	unsigned short flags : 1;
	unsigned short reserved : 3;
	unsigned short block;
};
_Static_assert(sizeof(struct physic_op_param) == 24,
	       "first GC physical operation size");
_Static_assert(__builtin_offsetof(struct physic_op_param, oob) == 16,
	       "first GC OOB pointer offset");
_Static_assert(sizeof(struct invalid_page_location) == 4,
	       "first GC map entry size");
struct gc_first_geometry {
	unsigned char before_pages[20];
	unsigned short pages;
	unsigned char before_fixed_page[6];
	unsigned short fixed_page;
};
struct gc_first_map {
	unsigned char before_pages[16];
	unsigned int *pages;
	unsigned int bytes;
};
struct gc_first_stats {
	unsigned char before_runs[120];
	unsigned int runs;
	unsigned int copied_pages;
};
struct gc_first_block {
	unsigned short number;
	unsigned short state;
	unsigned short invalid_pages;
	unsigned short erase_count;
};
_Static_assert(sizeof(struct gc_first_block) == 8,
	       "first GC block prefix size");
struct gc_first_zone {
	unsigned char before_geometry[8];
	struct gc_first_geometry *geometry;
	unsigned char before_mode[8];
	unsigned char mode;
	unsigned char before_map[464 - 25];
	struct gc_first_map map;
	unsigned char before_data[512 - 496];
	void *data;
	void *logic_buffer;
	unsigned char before_active[686 - 528];
	unsigned short active;
	unsigned char before_stats[3440 - 688];
	struct gc_first_stats *stats;
	unsigned char before_read[3464 - 3448];
	int (*read)(struct gc_first_zone *, struct physic_op_param *);
};
_Static_assert(__builtin_offsetof(struct gc_first_zone, map) == 464,
	       "first GC map offset");
_Static_assert(__builtin_offsetof(struct gc_first_zone, stats) == 3440,
	       "first GC stats offset");
_Static_assert(__builtin_offsetof(struct gc_first_zone, read) == 3464,
	       "first GC read offset");
_Static_assert(__builtin_offsetof(struct invalid_page_location, block) == 2,
	       "first GC map entry block offset");

struct move_node;
struct move_zone;
struct invalid_node;
struct invalid_zone;
struct diag_current_zone;
struct page_map_recovery_zone;
struct new_write_block;
struct page_map_write_zone;
struct page_map_write_block;
struct logic_page_map_zone;
struct erase_block_zone;
struct erase_block_info;

extern void *nand_memset(void *, int, unsigned int);
extern void *nand_memcpy(void *, const void *, unsigned int);
extern int nand_print(const char *, ...);
extern int nand_print_dbg(const char *, ...);
extern struct move_node *out_phy_block_from_invalid_page_list(struct move_zone *);
extern int is_current_blcok(struct diag_current_zone *, void *);
extern struct invalid_node *out_phy_block_from_invalid_page_list_by_block(
	struct invalid_zone *, struct invalid_node *);
extern void set_physic_op_par(struct physic_op_param *, unsigned short,
			      unsigned short, unsigned short, void *, void *);
extern int is_phy_mapping_page(void *);
extern int recover_block_phy_page_mapping(struct page_map_recovery_zone *,
					   struct new_write_block *, unsigned int *);
extern int check_invalid_page_right(struct page_map_write_zone *, void *,
				    struct page_map_write_block *, unsigned short);
extern unsigned int *get_logic_page_map(struct logic_page_map_zone *,
					unsigned int);
extern unsigned int get_logic_page_from_oob(void *);
extern int nand_write_logic_page_no_gc(void *, unsigned int, void *);
extern int erase_block(struct erase_block_zone *, struct erase_block_info *,
		       unsigned short);
extern int put_phy_block_to_invalid_page_list(void *, struct invalid_node *);
extern int adjust_invaild_list(struct move_zone *);

#include "../common1/nftl_gc_first_body.h"
