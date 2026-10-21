/* SPDX-License-Identifier: GPL-2.0 */
/* Verified stock-behavior NFTL helpers, compiled when selected by Kconfig.
 * Structure labels are inferred from observed offsets. */

int nftl_check_pattern(const unsigned char *buffer, unsigned char pattern,
		       int length)
{
	int index = 0;

	/* OEM treats zero and negative lengths as an empty, matching span. */
	while (index < length) {
		if (buffer[index] != pattern)
			return 1;
		index++;
	}
	return 0;
}

struct physic_op_param {
	unsigned short plane;
	unsigned short block;
	unsigned short page;
	unsigned short reserved;
	void *data;
	void *oob;
};

void set_physic_op_par(struct physic_op_param *op, unsigned short block,
		       unsigned short plane, unsigned short page,
		       void *data, void *oob)
{
	/* OEM leaves the reserved halfword untouched. */
	op->plane = plane;
	op->block = block;
	op->page = page;
	op->data = data;
	op->oob = oob;
}

int is_nouse_page(const unsigned char *bytes)
{
	unsigned int count;
	unsigned int index;

	/* OEM accepts nine FF bytes only when byte zero is also FF. This
	 * asymmetric test is retained even if the tenth byte is the sole
	 * non-FF byte. */
	count = 0;
	for (index = 0; index < 10; index++) {
		if (bytes[index] == 0xff)
			count++;
	}
	if (count > 8 && bytes[0] == 0xff)
		return 1;
	return 0;
}

struct samsung_set_chip {
	unsigned char until_count[184];
	unsigned int retry;
	unsigned char until_controller[84];
	void *controller;
};
struct samsung_retry_values {
	unsigned char bytes[4];
};
extern struct samsung_retry_values samsung_p1[15];
extern unsigned char samsung_read_retry_addr[4];
extern unsigned char samsung_read_retry_cmd[4];
extern void nand_enable_chip(struct samsung_set_chip *);
extern void ndfc_disable_randomize(void *);
extern void nand_print(const char *, ...);
extern void nand_print_dbg(const char *, ...);
extern void nand_disable_chip(struct samsung_set_chip *);
extern int set_cmd_with_nand_bus(void *, unsigned char *, int,
				 unsigned char *, unsigned char *, int,
				 unsigned int);

int samsung_vender_set_param(struct samsung_set_chip *chip,
			    unsigned char *values,
			    unsigned char *addresses, unsigned int count)
{
	int result;

	result = set_cmd_with_nand_bus(chip, samsung_read_retry_cmd, 0,
					 addresses, values, 1, count);
	/* OEM reads four diagnostic bytes even when count is smaller or the
	 * bus command failed. */
	nand_print_dbg("rr value %x %x %x %x!\n", values[0], values[1],
		       values[2], values[3]);
	return result;
}

int samsung_set_readretry(struct samsung_set_chip *chip)
{
	int result;

	nand_enable_chip(chip);
	ndfc_disable_randomize(chip->controller);
	/* OEM indexes the 15-entry table without a bounds check. */
	result = samsung_vender_set_param(chip, samsung_p1[chip->retry].bytes,
					   samsung_read_retry_addr, 4);
	if (result)
		nand_print("samsung set readretry error ! %x \n", chip->retry);
	nand_disable_chip(chip);
	return result;
}

extern void *nand_memcpy(void *, const void *, unsigned int);

void get_nftl_version(char *major, char *minor, char *build,
		      char *date, char *time)
{
	/* OEM assumes every caller buffer is large enough and ignores all
	 * copy results. These lengths include the terminating NUL bytes. */
	nand_memcpy(major, "5", 2);
	nand_memcpy(minor, "0", 2);
	nand_memcpy(build, "000", 4);
	nand_memcpy(date, "2020.12.24", 11);
	nand_memcpy(time, "09:50", 6);
}

extern int get_data_with_nand_bus_one_cmd(struct samsung_set_chip *,
					  unsigned char *, unsigned char *,
					  unsigned char *, int);

/* The inferred chip layout shares its controller pointer at offset 272
 * with the Samsung retry routines above. The 16nm/26nm stock bodies are
 * identical, including the command-byte initialization before chip setup.
 * Kernel-wide -fstack-protector swaps the result/index local slots even
 * though these functions receive no canary. Preserve the stock slot layout
 * with a per-function compiler option in this optional recovery path. */
#define HYNIX_GET_EARLY(name) \
__attribute__((optimize("-fno-stack-protector"))) \
int name(struct samsung_set_chip *chip, unsigned char *data, \
	 unsigned char *addresses, unsigned int count) \
{ \
	int result = 0; \
	unsigned char command = 55; \
	unsigned int index; \
	nand_enable_chip(chip); \
	ndfc_disable_randomize(chip->controller); \
	for (index = 0; index < count; index++) \
		get_data_with_nand_bus_one_cmd(chip, &command, addresses + index, \
					       data + index, 1); \
	nand_disable_chip(chip); \
	/* OEM ignores bus-read errors and always reports success. */ \
	return result; \
}

HYNIX_GET_EARLY(hynix16nm_vender_get_param)
HYNIX_GET_EARLY(hynix26nm_vender_get_param)

__attribute__((optimize("-fno-stack-protector")))
int hynix20nm_vender_get_param(struct samsung_set_chip *chip,
			      unsigned char *data, unsigned char *addresses,
			      unsigned int count)
{
	int result = 0;
	unsigned char command;
	unsigned int index;

	nand_enable_chip(chip);
	ndfc_disable_randomize(chip->controller);
	command = 55;
	for (index = 0; index < count; index++)
		get_data_with_nand_bus_one_cmd(chip, &command, addresses + index,
					       data + index, 1);
	nand_disable_chip(chip);
	/* OEM does not validate count or pointers, and discards read errors. */
	return result;
}

struct nftl_chip_block_ops {
	unsigned char before_callbacks[88];
	int (*erase)(struct nftl_chip_block_ops *, struct physic_op_param *);
	unsigned char between_callbacks[16];
	int (*is_good)(struct nftl_chip_block_ops *, struct physic_op_param *);
	int (*mark_bad)(struct nftl_chip_block_ops *, struct physic_op_param *);
};
struct nftl_zone_priv_block_ops {
	void *unused;
	struct nftl_chip_block_ops *chip;
};
extern struct nftl_zone_priv_block_ops *nftl_get_zone_priv(void *);

/* The kernel-wide stack-protector option reorders this local aggregate and
 * the two pointers without adding a canary. Keep OEM stack offsets in the
 * optional C path; these wrappers have no protectable character array. */
#define NFTL_BLOCK_WRAPPER(name, callback) \
__attribute__((optimize("-fno-stack-protector"))) \
int name(void *zone, struct physic_op_param *request) \
{ \
	struct nftl_zone_priv_block_ops *priv; \
	struct nftl_chip_block_ops *chip; \
	struct physic_op_param op; \
	priv = nftl_get_zone_priv(zone); \
	chip = priv->chip; \
	/* OEM leaves the unused halfword uninitialized, trusts both pointers, \
	 * and returns the callback result without translating errors. */ \
	op.block = request->block; \
	op.plane = request->plane; \
	op.page = request->page; \
	op.data = request->data; \
	op.oob = request->oob; \
	return chip->callback(priv->chip, &op); \
}

NFTL_BLOCK_WRAPPER(_nftl_nand_erase_superblk, erase)
NFTL_BLOCK_WRAPPER(_nftl_nand_is_blk_good, is_good)
NFTL_BLOCK_WRAPPER(_nftl_nand_mark_bad_blk, mark_bad)

struct nftl_free_erase_node {
	unsigned char before_erase[6];
	unsigned short erase_time;
	unsigned char before_next[24];
	struct nftl_free_erase_node *next;
};
struct nftl_free_erase_owner {
	unsigned char before_list[272];
	struct nftl_free_erase_node list;
};
struct nftl_invalid_erase_node {
	unsigned char before_erase[6];
	unsigned short erase_time;
	unsigned char before_next[8];
	struct nftl_invalid_erase_node *next;
};
struct nftl_invalid_erase_owner {
	unsigned char before_list[336];
	struct nftl_invalid_erase_node list;
};

/* As with the block wrappers, -fstack-protector changes local slot layout
 * without adding a canary. These list walkers have no character locals. */
#define NFTL_MAX_ERASE(name, owner_type, node_type) \
__attribute__((optimize("-fno-stack-protector"))) \
struct node_type *name(struct owner_type *owner, unsigned short limit) \
{ \
	struct node_type *best = 0; \
	unsigned short maximum = 0; \
	struct node_type *current = &owner->list; \
	current = current->next; \
	while (current) { \
		/* OEM picks the last tied maximum below limit and does not \
		 * detect cycles or validate the owner pointer. */ \
		if (limit > current->erase_time && \
		    maximum <= current->erase_time) { \
			maximum = current->erase_time; \
			best = current; \
		} \
		current = current->next; \
	} \
	if (best) \
		return best; \
	return 0; \
}

NFTL_MAX_ERASE(get_block_max_erase_time_from_free_list,
	       nftl_free_erase_owner, nftl_free_erase_node)
NFTL_MAX_ERASE(get_block_max_erase_time_from_invalid_list,
	       nftl_invalid_erase_owner, nftl_invalid_erase_node)

__attribute__((optimize("-fno-stack-protector")))
unsigned short get_max_erase_time_from_free_list(
	struct nftl_free_erase_owner *owner, unsigned short limit)
{
	struct nftl_free_erase_node *best = 0;
	unsigned short maximum = 0;
	struct nftl_free_erase_node *current = &owner->list;

	current = current->next;
	while (current) {
		/* OEM selects the last tied maximum below limit; a cycle loops
		 * forever, and no match returns the 0xffff sentinel. */
		if (limit > current->erase_time &&
		    maximum <= current->erase_time) {
			maximum = current->erase_time;
			best = current;
		}
		current = current->next;
	}
	if (best)
		return best->erase_time;
	return 65535;
}

extern struct nftl_free_erase_node *out_phy_block_from_free_list_by_block(
	struct nftl_free_erase_owner *, struct nftl_free_erase_node *);

__attribute__((optimize("-fno-stack-protector")))
struct nftl_free_erase_node *out_phy_block_from_free_list_by_erase_time(
	struct nftl_free_erase_owner *owner, unsigned short limit)
{
	struct nftl_free_erase_node *best = 0;
	unsigned short maximum = 0;
	struct nftl_free_erase_node *current = &owner->list;

	current = current->next;
	while (current) {
		/* OEM selects the last tied maximum below limit and never
		 * detects cycles or validates the owner pointer. */
		if (limit > current->erase_time &&
		    maximum <= current->erase_time) {
			maximum = current->erase_time;
			best = current;
		}
		current = current->next;
	}
	if (best)
		best = out_phy_block_from_free_list_by_block(owner, best);
	return best;
}

int samsung_readretry_exit(void *chip)
{
	/* OEM ignores the chip and reports success without exit work. */
	return 0;
}

int sandisk_A19_readretry_exit(void *chip)
{
	return 0;
}

int sandisk_readretry_exit(void *chip)
{
	return 0;
}

int toshiba_readretry_exit(void *chip)
{
	return 0;
}

int __shutdown_op(void *object)
{
	/* OEM returns success without using the device; keep its local frame. */
	int result = 0;
	return result;
}

int _nand_flush_sector_write_cache(void *object, int flag)
{
	/* OEM discards both arguments and reports a successful flush. */
	int result = 0;
	return result;
}

struct nftl_small_owner {
	unsigned char before_zone[24];
	struct nftl_small_zone *zone;
};
struct nftl_small_zone {
	unsigned char before_bad_count[28];
	unsigned short bad_block_count;
};

unsigned short nftl_get_bad_block_cnt(struct nftl_small_owner *owner)
{
	/* OEM dereferences both pointers without a null guard. */
	return owner->zone->bad_block_count;
}

extern void nand_free(void *object);

void nftl_free(void *object)
{
	nand_free(object);
}

/* Stock NFTL operation wrappers and spare-area marker classifiers. */
/* Bounded C candidates for retained NFTL assembly, not original declarations. */

extern void free_nand_partition(void *partition);
extern void nftl_zone_release(void *zone);
extern void nftl_free(void *allocation);
extern int __nand_read(void *zone, unsigned int sector, unsigned int count,
		       void *data);
extern int __nand_write(void *zone, unsigned int sector, unsigned int count,
			void *data);
extern int __nand_discard(void *zone, unsigned int sector,
			  unsigned int count);
extern unsigned int get_special_data_from_oob(void *oob);

struct nftl_exit_view {
	unsigned char before_partition[8];
	void *partition;
	unsigned char before_zone[8];
	void *zone;
	void *allocation;
};
_Static_assert(__builtin_offsetof(struct nftl_exit_view, partition) == 8, "partition offset");
_Static_assert(__builtin_offsetof(struct nftl_exit_view, zone) == 24, "zone offset");
_Static_assert(__builtin_offsetof(struct nftl_exit_view, allocation) == 32, "allocation offset");

int nftl_exit(struct nftl_exit_view *owner)
{
	/* OEM dereferences all three fields without a NULL guard. */
	free_nand_partition(owner->partition);
	nftl_zone_release(owner->zone);
	nftl_free(owner->allocation);
	return 0;
}

struct nftl_io_view {
	unsigned char before_zone[24];
	void *zone;
};
_Static_assert(__builtin_offsetof(struct nftl_io_view, zone) == 24, "I/O zone offset");

int _nand_read(struct nftl_io_view *owner, unsigned int sector,
	       unsigned int count, void *data)
{
	int result = 0;
	result = __nand_read(owner->zone, sector, count, data);
	return result;
}

int _nand_write(struct nftl_io_view *owner, unsigned int sector,
		unsigned int count, void *data)
{
	int result = 0;
	result = __nand_write(owner->zone, sector, count, data);
	return result;
}

int _nand_discard(struct nftl_io_view *owner, unsigned int sector,
		  unsigned int count)
{
	int result = 0;
	result = __nand_discard(owner->zone, sector, count);
	return result;
}

struct nftl_block_limit_view {
	unsigned char before_limit[24];
	unsigned short limit;
};

struct nftl_block_owner_view {
	unsigned char before_limit_ptr[8];
	struct nftl_block_limit_view *limits;
};
_Static_assert(__builtin_offsetof(struct nftl_block_owner_view, limits) == 8, "limit pointer offset");
_Static_assert(__builtin_offsetof(struct nftl_block_limit_view, limit) == 24, "limit offset");

int is_phy_block_valid(unsigned short *block,
		       struct nftl_block_owner_view *owner)
{
	/* OEM trusts both pointers and compares unsigned 16-bit block numbers. */
	if (*block < owner->limits->limit)
		return 1;
	return 0;
}

int is_fill_page(void *oob)
{
	unsigned int marker = get_special_data_from_oob(oob);
	if (marker == 0x55555555U)
		return 1;
	return 0;
}

int is_phy_mapping_page(void *oob)
{
	unsigned int marker = get_special_data_from_oob(oob);
	if (marker == 0xaaaaffffU)
		return 1;
	return 0;
}

int is_function_info_page(void *oob)
{
	unsigned int marker = get_special_data_from_oob(oob);
	if ((marker & 0xffff0000U) == 0xaa110000U)
		return 1;
	return 0;
}

int is_power_down_info_page(void *oob)
{
	unsigned int marker = get_special_data_from_oob(oob);
	if (marker == 0xaa113333U)
		return 1;
	return 0;
}

int is_ftl_start_flag_page(unsigned char *oob)
{
	/* OEM reads bytes 1 and 2 directly, with no length or NULL check. */
	return oob[1] == 0xaa && oob[2] == 0xee;
}

int is_ftl_end_flag_page(unsigned char *oob)
{
	return oob[1] == 0xaa && oob[2] == 0xcc;
}

int is_panic_data_page(unsigned char *oob)
{
	return oob[1] == 0xaa && oob[2] == 0x63;
}

int is_panic_dummy_page(unsigned char *oob)
{
	return oob[1] == 0xaa && oob[2] == 0x62;
}

/* Offset-limited behavioral candidates; these are not original declarations. */

extern int fill_no_use_page_to_current_block(void *object, int count);

int generic_special_exit(void)
{
	nand_print_dbg(" generic special exit \n");
	return 0;
}

int nand_before_shutdown(void *object)
{
	return fill_no_use_page_to_current_block(object, 8);
}

struct nftl_zone_cap_view {
	unsigned char preceding[32];
	unsigned int capacity;
};

unsigned int nftl_get_zone_cap(struct nftl_zone_cap_view *input)
{
	struct nftl_zone_cap_view *zone = input;
	return zone->capacity;
}

struct nftl_zone_priv_view {
	void *private_data;
};

struct nftl_zone_priv_block_ops *nftl_get_zone_priv(void *input)
{
	struct nftl_zone_priv_view *zone = input;
	return zone->private_data;
}

struct nftl_zone_cache_view {
	unsigned char preceding[0x434];
	unsigned short write_cache_count;
};

unsigned short nftl_get_zone_write_cache_nums(struct nftl_zone_cache_view *input)
{
	struct nftl_zone_cache_view *zone = input;
	return zone->write_cache_count;
}

int _nand_sector_read(void *object, unsigned int sector, unsigned int count,
		      void *buffer)
{
	/* OEM reports success without reading data. */
	int result = 0;
	return result;
}

int _nand_sector_write(void *object, unsigned int sector, unsigned int count,
		       void *buffer)
{
	/* OEM reports success without writing data. */
	int result = 0;
	return result;
}

struct nftl_boot_view {
	unsigned char preceding[0x8c];
	unsigned int boot_count;
};

struct nftl_boot_owner_view {
	unsigned char preceding[0xd70];
	struct nftl_boot_view *boot;
};

unsigned int nftl_get_boot_cnt(struct nftl_boot_owner_view *input)
{
	struct nftl_boot_owner_view *owner = input;
	/* OEM has no null guard for either pointer. */
	return owner->boot->boot_count;
}

/* C candidates for bounded OEM NFTL helpers; offsets are evidence, not types. */

extern unsigned int get_spare_data(void *oob, unsigned int offset,
				   unsigned int bytes);
extern int block_erase(unsigned short block, unsigned short chip);

unsigned int get_block_used_count_from_oob(void *oob)
{
	unsigned int value = get_spare_data(oob, 7, 4);
	return value;
}

unsigned short get_erase_count_from_oob(void *oob)
{
	unsigned short value = get_spare_data(oob, 5, 2);
	return value;
}

unsigned int get_special_data_from_oob(void *oob)
{
	unsigned int value = get_spare_data(oob, 1, 4);
	return value;
}

struct nftl_phy_inner {
	unsigned char preceding[32];
	unsigned char *blocks;
};

struct nftl_phy_outer {
	unsigned char preceding[8];
	struct nftl_phy_inner *inner;
};

unsigned char *get_phy_block_addr(struct nftl_phy_outer *outer,
				  unsigned short block)
{
	/* OEM performs no bounds check before indexing 64-byte blocks. */
	return outer->inner->blocks + (unsigned long)block * 64;
}

int nand_dbg_phy_erase(unsigned short chip, unsigned short block)
{
	int result = block_erase(chip, block);
	return result;
}

struct nftl_current_block {
	unsigned char preceding[0x38];
	unsigned short page;
};

int current_block_at_first_page(void *owner, struct nftl_current_block *block)
{
	/* OEM ignores owner and dereferences block without a guard. */
	if (block->page == 0)
		return 1;
	else
		return 0;
}

int is_ftl_special_data(unsigned char *oob)
{
	/* OEM reads byte one without checking the supplied buffer extent. */
	if (oob[1] == 0xaa)
		return 1;
	else
		return 0;
}

/* Bounded behavior-level NFTL C candidates, not original source claims. */

extern int __nand_flush_write_cache(void *cache, int force);
extern int write_data_to_page(void *zone, unsigned int marker,
			      unsigned int page, void *data);
extern int nand_write_data_page(void *zone, unsigned int type,
				unsigned int page, void *data);
extern void print_nftl_zone(void *zone);

struct nftl_logic_owner {
	unsigned char preceding[24];
	void *cache;
};

int _nand_flush_write_cache(struct nftl_logic_owner *owner, int force)
{
	int result = 0;
	result = __nand_flush_write_cache(owner->cache, force);
	return result;
}

int is_ftl_logic_page_data(unsigned char *oob)
{
	/* OEM reads byte one without a buffer-length check. */
	if ((oob[1] & 0xf0) == 0xc0)
		return 1;
	else
		return 0;
}

int _nftl_discard_logic_page(void *zone, unsigned int page)
{
	int result = write_data_to_page(zone, 0xaa111111, page, 0);
	return result;
}

unsigned int get_logic_page_from_oob(void *oob)
{
	unsigned int page = get_spare_data(oob, 1, 4);
	/* OEM silently drops the upper nibble of the page number. */
	page &= 0x0fffffff;
	return page;
}

int nand_write_logic_page_no_gc(void *zone, unsigned int page, void *data)
{
	int result = nand_write_data_page(zone, 0xc0, page, data);
	return result;
}

struct nftl_test_zone {
	unsigned char preceding[0x290];
	unsigned int test_mode;
};

void nftl_set_zone_test(struct nftl_test_zone *input, unsigned int enabled)
{
	struct nftl_test_zone *zone = input;
	zone->test_mode = enabled;
	print_nftl_zone(zone);
}

/* Isolated NFTL list-search C candidates; structure labels are inferred. */

struct free_node {
	unsigned char before_next[32];
	struct free_node *next;
};
struct free_zone {
	unsigned char before_head[272];
	struct free_node head;
};
int is_block_in_free_list(struct free_zone *zone, struct free_node *target)
{
	struct free_node *node = &zone->head;
	node = node->next;
	/* OEM does not detect list cycles. */
	while (node) {
		if (node == target)
			return 1;
		node = node->next;
	}
	return 0;
}

struct invalid_node {
	unsigned char before_next[16];
	struct invalid_node *next;
};
struct invalid_zone {
	unsigned char before_head[336];
	struct invalid_node head;
};
int is_block_in_invalid_list(struct invalid_zone *zone,
			     struct invalid_node *target)
{
	struct invalid_node *node = &zone->head;
	node = node->next;
	/* OEM does not detect list cycles. */
	while (node) {
		if (node == target)
			return 1;
		node = node->next;
	}
	return 0;
}

struct cache_read_node {
	unsigned char before_key[8];
	int key;
	unsigned char before_next[12];
	struct cache_read_node *next;
};
struct cache_read_zone {
	unsigned char before_head[1688];
	struct cache_read_node head;
};
/* Four key searches below compile with the two equality operands loaded into
 * opposite registers from stock. The memory reads, comparison, branches and
 * return paths are otherwise identical; equality is symmetric. */
struct cache_read_node *search_from_cache_read_list(
	struct cache_read_zone *zone, int key)
{
	struct cache_read_node *node = &zone->head;
	node = node->next;
	while (node) {
		if (key == node->key)
			return node;
		node = node->next;
	}
	return 0;
}

struct cache_write_node {
	unsigned char before_key[8];
	int key;
	unsigned char before_next[28];
	struct cache_write_node *next;
};
struct cache_write_zone {
	unsigned char before_head[1744];
	struct cache_write_node head;
};
struct cache_write_node *search_from_cache_write_list(
	struct cache_write_zone *zone, int key)
{
	struct cache_write_node *node = &zone->head;
	node = node->next;
	while (node) {
		if (key == node->key)
			return node;
		node = node->next;
	}
	return 0;
}

struct sector_cache_node {
	unsigned char before_key[4];
	int key;
	unsigned char before_next[8];
	struct sector_cache_node *next;
};
struct sector_cache_zone {
	unsigned char before_head[528];
	struct sector_cache_node head;
};
struct sector_cache_node *search_from_sector_cache_write_list(
	struct sector_cache_zone *zone, int key)
{
	struct sector_cache_node *node = &zone->head;
	node = node->next;
	while (node) {
		if (key == node->key)
			return node;
		node = node->next;
	}
	return 0;
}

struct gc_search_node {
	unsigned char before_key[8];
	void *key;
	struct gc_search_node *next;
};
struct gc_search_zone {
	unsigned char before_head[328];
	struct gc_search_node head;
};
struct gc_search_node *search_gc_node(struct gc_search_zone *zone, void *key)
{
	struct gc_search_node *node = &zone->head;
	node = node->next;
	while (node) {
		if (key == node->key)
			return node;
		node = node->next;
	}
	return 0;
}

/* Isolated C equivalents; field names and aggregate types are inferred. */

int get_bad_flag_from_oob(void *oob)
{
	int bad = get_spare_data(oob, 1, 4);
	if (bad == 0)
		return 0;
	return 1;
}

struct current_page {
	unsigned char before_page[56];
	unsigned short page;
};
struct page_geometry {
	unsigned char before_pages[20];
	unsigned short pages_per_block;
};
struct current_zone {
	unsigned char before_geometry[8];
	struct page_geometry *geometry;
};
int current_block_at_last_page(struct current_zone *zone,
			       struct current_page *block)
{
	if (block->page == zone->geometry->pages_per_block - 1)
		return 1;
	return 0;
}

struct physical_block {
	unsigned short number;
};
struct physical_geometry {
	unsigned char before_blocks[24];
	unsigned short blocks_per_chip;
};
struct physical_zone {
	unsigned char before_geometry[8];
	struct physical_geometry *geometry;
};
int is_last_phy_block(struct physical_block *block,
		      struct physical_zone *zone)
{
	if (block->number == zone->geometry->blocks_per_chip - 1)
		return 1;
	return 0;
}

/* Isolated NFTL C candidates; private structure names are inferred. */

struct nftl_chain {
	unsigned char before_next[16];
	struct nftl_chain *next;
};

void add_nftl_blk_list(struct nftl_chain *head, struct nftl_chain *new_node)
{
	struct nftl_chain *current = head;
	new_node->next = 0;
	/* OEM does not detect an existing cycle or duplicate node. */
	while (current->next)
		current = current->next;
	current->next = new_node;
}

struct nrc_buffers {
	unsigned char before_first[8];
	void *first;
	void *second;
	void *third;
};
int free_nrc(struct nrc_buffers *buffers)
{
	if (buffers == 0)
		return 0;
	nftl_free(buffers->second);
	nftl_free(buffers->first);
	nftl_free(buffers->third);
	nftl_free(buffers);
	return 0;
}

extern void set_spare_data(unsigned char *oob, int value, int offset,
			   int count);
void set_start_block_flag(void *unused, unsigned char *oob, int number)
{
	/* OEM ignores the first argument and all spare-data write failures. */
	oob[0] = 0xff;
	set_spare_data(oob, 0xaa, 1, 1);
	set_spare_data(oob, 0xee, 2, 1);
	set_spare_data(oob, number, 3, 4);
}

struct read_claim_settings {
	unsigned char before_flags[172];
	int flag;
	int page;
};
struct read_claim_zone {
	unsigned char before_settings[3440];
	struct read_claim_settings *settings;
};
struct read_claim_node {
	unsigned char before_next[16];
	struct read_claim_node *next;
	struct read_claim_zone *zone;
};
int set_nftl_read_claim_flag(struct read_claim_node *head, int flag, int page)
{
	struct read_claim_node *node = head;
	/* OEM follows unchecked list and settings pointers. */
	while (node) {
		node->zone->settings->flag = flag;
		node->zone->settings->page = page;
		node = node->next;
	}
	return 0;
}
