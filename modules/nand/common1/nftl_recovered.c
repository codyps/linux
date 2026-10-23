/* SPDX-License-Identifier: GPL-2.0 */
/* Verified stock-behavior NFTL helpers, compiled when selected by Kconfig.
 * Structure labels are inferred from observed offsets. */

/* Reuse the instruction-rooted OEM readonly pool. Separate C string literals
 * would duplicate these payloads and move later readonly and BSS addresses. */
#include "nftl_ro_aliases.h"

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
extern int nand_set_feature(void *chip, unsigned char *address,
			    unsigned char *values);
extern int nand_get_feature(void *chip, unsigned char *address,
			    unsigned char *values);
/* OEM uses a 48-byte frame without a canary: readback at offset 32,
 * outgoing bytes at 40 and the original address copy at 47. */
__attribute__((optimize("-fno-stack-protector")))
int micron_intel_set_feature(void *chip, unsigned char address,
			     unsigned char value)
{
	unsigned char original_address = address;
	unsigned char set_values[4] __attribute__((aligned(8))) = {value, 0, 0, 0};
	unsigned char read_values[8] __attribute__((aligned(8)));

	/* OEM ignores both command results. If the read fails without writing
	 * byte zero, the comparison uses an uninitialized stack byte. */
	nand_set_feature(chip, &address, set_values);
	nand_get_feature(chip, &address, read_values);
	if (read_values[0] != set_values[0]) {
		nand_print(nftl_ro_lc_69,
			   original_address, value);
		return -72;
	}
	return 0;
}

int samsung_vender_set_param(struct samsung_set_chip *chip,
			    unsigned char *values,
			    unsigned char *addresses, unsigned int count)
{
	int result;

	result = set_cmd_with_nand_bus(chip, samsung_read_retry_cmd, 0,
					 addresses, values, 1, count);
	/* OEM reads four diagnostic bytes even when count is smaller or the
	 * bus command failed. */
	nand_print_dbg(nftl_ro_lc_35, values[0], values[1],
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
		nand_print(nftl_ro_lc_36, chip->retry);
	nand_disable_chip(chip);
	return result;
}

extern void *nand_memcpy(void *, const void *, unsigned int);

void get_nftl_version(char *major, char *minor, char *build,
		      char *date, char *time)
{
	/* OEM assumes every caller buffer is large enough and ignores all
	 * copy results. These lengths include the terminating NUL bytes. */
	nand_memcpy(major, nftl_ro_lc_14, 2);
	nand_memcpy(minor, nftl_ro_lc_13, 2);
	nand_memcpy(build, nftl_ro_lc_12, 4);
	nand_memcpy(date, nftl_ro_lc_11, 11);
	nand_memcpy(time, nftl_ro_lc_10, 6);
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

struct command_chip;
extern int set_one_cmd(struct command_chip *chip, unsigned int command,
		       unsigned int wait_ready);
int hynix20nm_vender_set_param(void *chip, unsigned char *values,
			      unsigned char *addresses, unsigned int count)
{
	int result = 0;
	unsigned char command = 54;
	unsigned char confirm = 22;
	unsigned int index;

	nand_enable_chip(chip);
	ndfc_disable_randomize(((struct samsung_set_chip *)chip)->controller);
	/* OEM ignores both command results on every register and returns zero. */
	for (index = 0; index < count; index++) {
		set_cmd_with_nand_bus(chip, &command, 0, addresses + index,
				      values + index, 1, 1);
		set_one_cmd((struct command_chip *)chip, confirm, 0);
	}
	nand_disable_chip(chip);
	return result;
}

/* Preserve the OEM result/index slots at FP+0x38/+0x3c as in the 16nm
 * programmer; callbacks can observe the otherwise reordered frame. */
__attribute__((optimize("-fno-stack-protector")))
int hynix26nm_vender_set_param(void *chip, unsigned char *values,
			      unsigned char *addresses, unsigned int count)
{
	int result = 0;
	unsigned char command = 54;
	unsigned char confirm = 22;
	unsigned int index;

	nand_enable_chip(chip);
	ndfc_disable_randomize(((struct samsung_set_chip *)chip)->controller);
	/* OEM ignores every bus-command result, including a failing confirm,
	 * and reports success after disabling the chip. */
	for (index = 0; index < count; index++) {
		set_cmd_with_nand_bus(chip, &command, 0, addresses + index,
				      values + index, 1, 1);
		set_one_cmd((struct command_chip *)chip, confirm, 0);
	}
	nand_disable_chip(chip);
	/* OEM reads four diagnostic value bytes even when count is smaller
	 * or the caller provided fewer than four bytes. */
	nand_print_dbg(nftl_ro_lc_35, values[0],
		       values[1], values[2], values[3]);
	return result;
}

struct nftl_chip_block_ops {
	unsigned char before_callbacks[88];
	int (*erase)(struct nftl_chip_block_ops *, struct physic_op_param *);
	int (*read)(struct nftl_chip_block_ops *, struct physic_op_param *);
	int (*write)(struct nftl_chip_block_ops *, struct physic_op_param *);
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

struct nftl_page_stats {
	unsigned char before_reads[80];
	unsigned long long reads;
	unsigned long long writes;
	unsigned char before_errors[36];
	unsigned int read_errors;
	unsigned int write_errors;
};
struct nftl_page_zone {
	unsigned char before_stats[3440];
	struct nftl_page_stats *stats;
};

/* OEM increments the attempt counter before calling the chip. Read errors
 * count only -2; write errors count every nonzero result. Both return the
 * callback result unmodified and trust the zone, stats, and chip pointers. */
#define NFTL_PAGE_WRAPPER(name, callback, counter, error, failed) \
__attribute__((optimize("-fno-stack-protector"))) \
int name(struct nftl_page_zone *zone, struct physic_op_param *request) \
{ \
	struct nftl_zone_priv_block_ops *priv; \
	struct nftl_chip_block_ops *chip; \
	struct physic_op_param op; \
	int result; \
	priv = nftl_get_zone_priv(zone); \
	chip = priv->chip; \
	zone->stats->counter++; \
	op.block = request->block; \
	op.plane = request->plane; \
	op.page = request->page; \
	op.data = request->data; \
	op.oob = request->oob; \
	result = chip->callback(priv->chip, &op); \
	if (failed) \
		zone->stats->error++; \
	return result; \
}

NFTL_PAGE_WRAPPER(_nftl_nand_read_page, read, reads, read_errors, result == -2)
NFTL_PAGE_WRAPPER(_nftl_nand_write_page, write, writes, write_errors, result != 0)

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
	/* The stock comparison emits the signed-field load schedule, then
	 * compares with an unsigned limit. Negative counts rank as huge. */
	int count;
	unsigned char before_next[4];
	struct nftl_invalid_erase_node *next;
};
struct nftl_invalid_erase_chip {
	unsigned char before_limit[16];
	unsigned short erase_limit;
};
struct nftl_invalid_erase_owner {
	unsigned char before_chip[8];
	struct nftl_invalid_erase_chip *chip;
	unsigned char before_list[320];
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
struct nftl_invalid_erase_node *get_block_min_erase_time_from_invalid_list(
	struct nftl_invalid_erase_owner *owner, unsigned short limit,
	unsigned int count_limit)
{
	struct nftl_invalid_erase_node *best = 0;
	unsigned short minimum = owner->chip->erase_limit;
	struct nftl_invalid_erase_node *current = &owner->list;

	current = current->next;
	while (current) {
		/* OEM picks the last tie and trusts every link. It requires all
		 * three strict/weak bounds exactly as shipped. */
		if (limit > current->erase_time && current->count < count_limit &&
		    minimum >= current->erase_time) {
			minimum = current->erase_time;
			best = current;
		}
		current = current->next;
	}
	if (best)
		return best;
	return 0;
}

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
struct nftl_zone_release_view;
extern void nftl_zone_release(struct nftl_zone_release_view *zone);
extern void nftl_free(void *allocation);
struct nand_rw_inner {
	unsigned char before_read_count[64];
	unsigned long long read_count;
	unsigned long long write_count;
};
struct nand_rw_zone {
	unsigned char before_debug_level[656];
	unsigned int debug_level;
	unsigned char before_inner[3440 - 660];
	struct nand_rw_inner *inner;
};
struct nand_op_zone;
extern int nand_op(unsigned char kind, struct nand_op_zone *zone,
		   unsigned int sector,
		   unsigned int count, void *data);
int __nand_read(void *zone, unsigned int sector, unsigned int count,
		void *data)
{
	struct nand_rw_zone *owner = zone;
	struct nand_rw_inner *const volatile *slot =
		(struct nand_rw_inner *const volatile *)&owner->inner;
	struct nand_rw_inner *destination;
	struct nand_rw_inner *source;

	if (owner->debug_level == 3)
		nand_print_dbg(nftl_ro_lc_415, sector, count);
	/* OEM fetches the inner pointer twice, destination before source.
	 * It updates the counter before the operation, even on failure. */
	destination = *slot;
	source = *slot;
	destination->read_count = source->read_count + count;
	return nand_op(0, zone, sector, count, data);
}
int __nand_write(void *zone, unsigned int sector, unsigned int count,
		 void *data)
{
	struct nand_rw_zone *owner = zone;
	struct nand_rw_inner *const volatile *slot =
		(struct nand_rw_inner *const volatile *)&owner->inner;
	struct nand_rw_inner *destination;
	struct nand_rw_inner *source;

	if (owner->debug_level == 3)
		nand_print_dbg(nftl_ro_lc_416, sector, count);
	destination = *slot;
	source = *slot;
	destination->write_count = source->write_count + count;
	return nand_op(1, zone, sector, count, data);
}
struct discard_zone;
extern int __nand_discard(struct discard_zone *zone, unsigned int sector,
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
	nand_print_dbg(nftl_ro_lc_106);
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

unsigned int get_spare_data(void *oob, unsigned char offset,
			    unsigned char bytes)
{
	/* OEM stores both arguments as bytes on entry, discarding high bits. */
	unsigned int index = 0;
	unsigned int value = 0;

	while (bytes > index) {
		value <<= 8;
		value |= ((unsigned char *)oob)[(unsigned int)offset + index];
		index++;
	}
	/* OEM keeps only the final four bytes for lengths above four. */
	return value;
}
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

extern int __nand_flush_write_cache(void *cache, unsigned int force);
extern int write_logic_data_to_current_block(void *zone, void *current,
				     unsigned int marker, unsigned int page,
				     void *data);
struct write_page_zone {
	unsigned char before_flags[25];
	unsigned char flags;
};
int write_data_to_page(void *zone, unsigned int marker,
		       unsigned int page, void *data)
{
	if (write_logic_data_to_current_block(zone,
			(unsigned char *)zone + 528, marker, page, data)) {
		nand_print(nftl_ro_lc_161);
		return 1;
	}
	/* OEM writes the second plane only when bit zero of byte 25 is set.
	 * The first plane may already have committed before a second failure. */
	if ((((struct write_page_zone *)zone)->flags & 1) &&
	    write_logic_data_to_current_block(zone,
			(unsigned char *)zone + 592, marker, page, data)) {
		nand_print(nftl_ro_lc_161);
		return 1;
	}
	return 0;
}
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
	unsigned char before_erase[4];
	unsigned short erase_time;
	unsigned char after_erase[2];
	/* Signed in stock. Unsigned scan limits convert negative counts to
	 * large values during comparison; keep that OEM behavior. */
	volatile int count;
	unsigned char before_next[4];
	struct invalid_node *next;
	struct invalid_node *previous;
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
/* Keep the OEM's private scan slots at SP+24/36/40. The gap is private
 * stack layout, not an on-media or externally visible structure. */
struct invalid_count_scan {
	struct invalid_node *node;
	unsigned char gap[4];
	unsigned int limit;
	struct invalid_node *best;
};
struct invalid_node *get_block_max_count_from_invalid_list(
	struct invalid_zone *zone)
{
	struct invalid_count_scan local;

	local.best = 0;
	local.limit = 0;
	local.node = &zone->head;
	local.node = local.node->next;
	/* OEM selects the later node on ties and reads the chosen count
	 * twice. It never detects a cycle. */
	while (local.node) {
		if (local.node->count >= local.limit) {
			local.limit = local.node->count;
			local.best = local.node;
		}
		local.node = local.node->next;
	}
	if (local.best)
		return local.best;
	return 0;
}
struct invalid_node *get_block_min_count_from_invalid_list(
	struct invalid_zone *zone)
{
	struct invalid_count_scan local;

	local.best = 0;
	local.limit = ~0U;
	local.node = &zone->head;
	local.node = local.node->next;
	while (local.node) {
		if (local.node->count <= local.limit) {
			local.limit = local.node->count;
			local.best = local.node;
		}
		local.node = local.node->next;
	}
	if (local.best)
		return local.best;
	return 0;
}

struct block_count_node {
	unsigned short block;
	unsigned char before_count[6];
	int count;
	unsigned char before_next[36];
	struct block_count_node *next;
	struct block_count_node *previous;
};
struct block_count_zone {
	unsigned char before_head[400];
	struct block_count_node head;
	unsigned char before_debug[656 - 464];
	unsigned int debug_level;
};
void print_block_count_list(struct block_count_zone *zone)
{
	struct block_count_node *node = &zone->head;

	if (zone->debug_level == 0)
		return;
	nand_print_dbg(nftl_ro_lc_406);
	node = node->next;
	/* OEM reads count and next after the preceding print calls, so
	 * callbacks can change what the next iteration observes. */
	while (node) {
		nand_print_dbg(nftl_ro_lc_407, node->block);
		nand_print_dbg(nftl_ro_lc_408, node->count);
		node = node->next;
	}
}

struct cache_read_node {
	unsigned char before_key[8];
	unsigned int key;
	unsigned char before_next[12];
	struct cache_read_node *next;
};
struct cache_read_zone {
	unsigned char before_head[1688];
	struct cache_read_node head;
};
struct cache_read_delete_node {
	unsigned char before_state[2];
	unsigned short state;
	unsigned char before_next[20];
	struct cache_read_delete_node *next;
	struct cache_read_delete_node *previous;
};
struct cache_read_delete_owner {
	unsigned char before_first[2];
	unsigned short first_count;
	unsigned char before_second[2];
	unsigned short second_count;
};
struct cache_read_delete_node *del_from_cache_read_list(
	struct cache_read_delete_owner *owner,
	struct cache_read_delete_node *target)
{
	struct cache_read_delete_node *previous = target->previous;

	/* OEM assumes both target and predecessor are valid and decrements an
	 * unsigned 16-bit count even at zero, wrapping to 65535. */
	previous->next = target->next;
	if (target->next)
		target->next->previous = previous;
	if (target->state == 0)
		owner->first_count--;
	else
		owner->second_count--;
	target->state = 255;
	target->next = 0;
	target->previous = 0;
	return target;
}
struct cache_read_add_zone {
	unsigned char before_first[2];
	unsigned short first_count;
	unsigned char before_second[2];
	unsigned short second_count;
	unsigned char before_head[1680];
	struct cache_read_delete_node head;
};
struct cache_read_empty_node {
	unsigned char before_state[2];
	unsigned short state;
	unsigned char before_next[20];
	struct cache_read_empty_node *next;
	unsigned char remainder[24];
};
struct cache_read_empty_zone {
	unsigned short node_count;
	unsigned char before_second_count[4];
	unsigned short second_count;
	struct cache_read_empty_node nodes[30];
	struct cache_read_empty_node head;
};
/* OEM stores the walk node/index at FP+0x20/+0x2e. Keep the original
 * misspelled symbol and unchecked node_count while restoring that frame. */
__attribute__((optimize("-fno-stack-protector")))
struct cache_read_empty_node *get_empty_cahce_node(
	struct cache_read_empty_zone *zone)
{
	struct {
		struct cache_read_empty_node *node;
		unsigned char untouched[6];
		unsigned short index;
	} local;

	local.node = 0;

	for (local.index = 0; local.index < zone->node_count; local.index++) {
		/* OEM trusts node_count, even beyond the 30 in-place nodes. */
		if (zone->nodes[local.index].state == 255)
			return &zone->nodes[local.index];
	}
	/* OEM branches into separate state-2 and state-0 walks. Neither walk
	 * falls back to the other state or detects a list cycle. */
	if (zone->second_count > 9) {
		local.node = &zone->head;
		local.node = local.node->next;
		while (local.node) {
			if (local.node->state == 2)
				return (struct cache_read_empty_node *)
					del_from_cache_read_list(
					(struct cache_read_delete_owner *)zone,
					(struct cache_read_delete_node *)local.node);
			local.node = local.node->next;
		}
	} else {
		local.node = &zone->head;
		local.node = local.node->next;
		while (local.node) {
			if (local.node->state == 0)
				return (struct cache_read_empty_node *)
					del_from_cache_read_list(
					(struct cache_read_delete_owner *)zone,
					(struct cache_read_delete_node *)local.node);
			local.node = local.node->next;
		}
	}
	return 0;
}
int add_to_cache_read_list_tail(struct cache_read_add_zone *zone,
				struct cache_read_delete_node *node,
				unsigned int state)
{
	struct cache_read_delete_node *current = &zone->head;

	/* OEM does not check cycles, duplicate insertion, or counter overflow. */
	while (current->next)
		current = current->next;
	current->next = node;
	node->next = 0;
	node->previous = current;
	node->state = state;
	if (state == 0)
		zone->first_count++;
	else
		zone->second_count++;
	return 0;
}
/* OEM compares the cached sector keys as unsigned 32-bit values. */
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
	unsigned int key;
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
	/* OEM compares the stored sector as an unsigned 32-bit key. */
	unsigned int key;
	unsigned char before_next[8];
	struct sector_cache_node *next;
};
struct sector_cache_zone {
	unsigned char before_head[528];
	struct sector_cache_node head;
};
extern int nand_sector_cache_read(void *zone, unsigned int sector, void *data);
extern int nand_sector_cache_write(void *zone, unsigned int sector, void *data);
/* OEM's declaration order and no-stack-protector layout put the four loop
 * locals in fixed slots even though no canary appears in either wrapper. */
__attribute__((optimize("-fno-stack-protector")))
int __nand_sector_read(void *zone, unsigned int sector, unsigned int count,
		       void *data)
{
	int result;
	unsigned int index, current;
	unsigned char *pointer;

	result = 0;
	current = sector;
	pointer = data;
	for (index = 0; index < count; index++) {
		/* OEM ORs every result and never stops on an error. */
		result |= nand_sector_cache_read(zone, current, pointer);
		current++;
		pointer += 512;
	}
	return result;
}
__attribute__((optimize("-fno-stack-protector")))
int __nand_sector_write(void *zone, unsigned int sector, unsigned int count,
			void *data)
{
	int result;
	unsigned int index, current;
	unsigned char *pointer;

	result = 0;
	current = sector;
	pointer = data;
	for (index = 0; index < count; index++) {
		/* OEM also discards intermediate write errors after OR-ing. */
		result |= nand_sector_cache_write(zone, current, pointer);
		current++;
		pointer += 512;
	}
	return result;
}
struct sector_cache_node *search_from_sector_cache_write_list(
	struct sector_cache_zone *zone, volatile unsigned int key)
{
	/* The by-value volatile key retains OEM's per-iteration stack reload;
	 * callers pass the full unsigned sector without a conversion temporary. */
	struct sector_cache_node *node = &zone->head;
	node = node->next;
	while (node) {
		if (key == node->key)
			return node;
		node = node->next;
	}
	return 0;
}
struct sector_read_hit {
	unsigned char before_data[8];
	void *data;
};
/* The OEM read wrapper uses the no-stack-protector local slot order, with
 * no canary to remove; preserve it without changing its call behavior. */
__attribute__((optimize("-fno-stack-protector")))
int nand_sector_cache_read(void *zone, unsigned int sector, void *data)
{
	unsigned int bytes = 512;
	struct sector_cache_zone *cache =
		(struct sector_cache_zone *)((unsigned char *)zone + 2880);
	struct sector_read_hit *hit = (struct sector_read_hit *)
		search_from_sector_cache_write_list(cache, sector);
	int result;

	if (hit) {
		/* OEM ignores the copy result and reports a cache hit as success. */
		nand_memcpy(data, hit->data, bytes);
		return 0;
	}
	result = __nand_read(zone, sector, 1, data);
	/* OEM converts every nonzero error to one and prints a fixed string. */
	if (result) {
		nand_print(nftl_ro_lc_142);
		return 1;
	}
	return result;
}

struct gc_search_node {
	unsigned char before_key[8];
	/* OEM loads this key as an integer when comparing pointer-sized IDs. */
	unsigned long key;
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
		if ((unsigned long)key == node->key)
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

struct do_after_zone;
extern int do_after_write(struct do_after_zone *zone);
extern int do_before_write(void *zone);
int nand_write_data_page(void *zone, unsigned int marker,
			 unsigned int page, void *data)
{
	if (current_block_at_last_page((struct current_zone *)zone,
				       (struct current_page *)((char *)zone + 528)) == 1) {
		if (do_after_write((struct do_after_zone *)zone)) {
			nand_print(nftl_ro_lc_152);
			return 1;
		}
	}
	if (do_before_write(zone)) {
		nand_print(nftl_ro_lc_153);
		return 1;
	}
	if (write_data_to_page(zone, marker, page, data)) {
		nand_print(nftl_ro_lc_154);
		return 1;
	}
	if (do_after_write((struct do_after_zone *)zone)) {
		nand_print(nftl_ro_lc_155);
		return 1;
	}
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
struct new_write_geometry {
	unsigned char before_erase_limit[16];
	unsigned short erase_limit;
	unsigned char before_page[10];
	unsigned short page;
};
struct new_write_block {
	unsigned short number;
	unsigned char before_page[2];
	unsigned short page;
	unsigned short erase_count;
	unsigned int tag;
};
struct new_write_zone {
	unsigned char before_geometry[8];
	struct new_write_geometry *geometry;
	unsigned char before_max_erase[660];
	unsigned int max_erase;
	unsigned char before_erase[2776];
	int (*erase)(struct new_write_zone *, struct physic_op_param *);
	unsigned char before_mark_bad[32];
	int (*mark_bad)(struct new_write_zone *, struct physic_op_param *);
};
int new_block_init_for_write(struct new_write_zone *zone,
			     struct new_write_block *block, unsigned int tag)
{
	/* OEM keeps the operation and erase result at +0x30/+0x4c in its
	 * 0x50-byte frame. Neither the gap nor op.reserved is initialized. */
	struct {
		struct physic_op_param op;
		unsigned char gap[4];
		int result;
	} local;

	set_physic_op_par(&local.op, block->number, 0,
			  zone->geometry->page, 0, 0);
	local.result = zone->erase(zone, &local.op);
	/* OEM resets page and increments erase count before checking erase
	 * failure; retain the resulting metadata update on a failed erase. */
	block->page = 0;
	block->erase_count++;
	if (block->erase_count > zone->max_erase &&
	    block->erase_count < zone->geometry->erase_limit)
		zone->max_erase = block->erase_count;
	block->tag = tag;
	if (local.result) {
		zone->mark_bad(zone, &local.op);
		nand_print(nftl_ro_lc_174);
		return 1;
	}
	return 0;
}
struct write_error_geometry {
	unsigned char before_page_bytes[28];
	unsigned short page_bytes;
};
struct write_error_zone {
	unsigned char before_geometry[8];
	struct write_error_geometry *geometry;
	unsigned char before_data[512 - 16];
	void *data;
	unsigned char before_current[528 - 520];
	struct new_write_block *current;
	unsigned char before_current_page[584 - 536];
	unsigned short current_page;
	unsigned char before_erase[3456 - 586];
	int (*erase)(struct write_error_zone *, struct physic_op_param *);
	unsigned char before_copy[3480 - 3464];
	int (*copy)(struct write_error_zone *, struct new_write_block *,
		    struct new_write_block *, void *, unsigned short);
	unsigned char before_mark_bad[3496 - 3488];
	int (*mark_bad)(struct write_error_zone *, struct physic_op_param *);
};
struct smart_info_geometry {
	unsigned char before_pages[20];
	unsigned short pages;
	unsigned char before_page_bytes[6];
	unsigned short page_bytes;
};
struct smart_info_payload {
	unsigned char before_magic[112];
	unsigned int magic;
	unsigned char before_init_count[24];
	unsigned int init_count;
	unsigned char before_field_164[20];
	unsigned int field_164;
	unsigned char before_field_172[4];
	unsigned int field_172;
	unsigned int field_176;
	unsigned char trailing[320 - 180];
};
struct smart_info_zone {
	unsigned char before_geometry[8];
	struct smart_info_geometry *geometry;
	unsigned char before_data[512 - 16];
	unsigned char *data;
	unsigned char before_offset[560 - 520];
	unsigned int offset;
	unsigned char before_print_enabled[656 - 564];
	unsigned int print_enabled;
	unsigned char before_smart[3440 - 660];
	struct smart_info_payload *smart;
	unsigned char before_read[3464 - 3448];
	int (*read)(struct smart_info_zone *, struct physic_op_param *);
};
struct smart_print_payload {
	unsigned char before_totals[64];
	unsigned long long recv_read_sectors;
	unsigned long long recv_write_sectors;
	unsigned long long real_read_pages;
	unsigned long long real_write_pages;
	unsigned long long recv_discard_sectors;
	unsigned long long real_discard_sectors;
	unsigned int version;
	unsigned int read_claim_pages;
	unsigned int gc_times;
	unsigned int gc_pages;
	unsigned int wl_times;
	unsigned int read_errors;
	unsigned int write_errors;
	unsigned int normal_power_cycles;
	unsigned int unusual_power_cycles;
	unsigned int max_erase_times;
	unsigned int min_erase_times;
	unsigned int max_used_count;
	unsigned int min_used_count;
	unsigned int read_reclaim_utc;
	unsigned int cross_talk_times;
	unsigned int read_reclaim_zone;
	unsigned int read_reclaim_page;
	unsigned int wl_gc_times;
};
void print_smart(struct smart_info_zone *input)
{
	/* OEM keeps the payload at FP+0x20 and the zone at FP+0x28. */
	struct {
		struct smart_print_payload *smart;
		struct smart_info_zone *zone;
	} frame;

	frame.zone = input;

	if (!frame.zone->print_enabled)
		return;
	frame.smart = (struct smart_print_payload *)frame.zone->smart;
	nand_print_dbg(nftl_ro_lc_176, frame.smart->version);
	nand_print_dbg(nftl_ro_lc_177, frame.smart->recv_read_sectors);
	nand_print_dbg(nftl_ro_lc_178, frame.smart->recv_write_sectors);
	nand_print_dbg(nftl_ro_lc_179, frame.smart->real_read_pages);
	nand_print_dbg(nftl_ro_lc_180, frame.smart->real_write_pages);
	nand_print_dbg(nftl_ro_lc_181, frame.smart->recv_discard_sectors);
	nand_print_dbg(nftl_ro_lc_182, frame.smart->real_discard_sectors);
	nand_print_dbg(nftl_ro_lc_183, frame.smart->read_claim_pages);
	nand_print_dbg(nftl_ro_lc_184, frame.smart->gc_times);
	nand_print_dbg(nftl_ro_lc_185, frame.smart->gc_pages);
	nand_print_dbg(nftl_ro_lc_186, frame.smart->wl_times);
	nand_print_dbg(nftl_ro_lc_187, frame.smart->wl_gc_times);
	nand_print_dbg(nftl_ro_lc_188, frame.smart->read_errors);
	nand_print_dbg(nftl_ro_lc_189, frame.smart->write_errors);
	nand_print_dbg(nftl_ro_lc_190, frame.smart->normal_power_cycles);
	nand_print_dbg(nftl_ro_lc_191, frame.smart->unusual_power_cycles);
	nand_print_dbg(nftl_ro_lc_192, frame.smart->max_erase_times);
	nand_print_dbg(nftl_ro_lc_193, frame.smart->min_erase_times);
	nand_print_dbg(nftl_ro_lc_194, frame.smart->max_used_count);
	nand_print_dbg(nftl_ro_lc_195, frame.smart->min_used_count);
	nand_print_dbg(nftl_ro_lc_196, frame.smart->read_reclaim_utc);
	nand_print_dbg(nftl_ro_lc_197, frame.smart->cross_talk_times);
	nand_print_dbg(nftl_ro_lc_198, frame.smart->read_reclaim_zone);
	nand_print_dbg(nftl_ro_lc_199, frame.smart->read_reclaim_page);
}
struct smart_info_block {
	unsigned short number;
};
extern void *nand_memset(void *destination, int value, unsigned int size);
extern void print_smart(struct smart_info_zone *zone);
/* The OEM uses these private stack slots in init_smart_info. Keep the OOB
 * bytes uninitialized: its marker check can consume them when read() returns
 * without filling spare data. A fixed implementation should initialize OOB
 * or reject such reads, but that would change stock behavior. */
struct smart_info_init_frame {
	unsigned char preceding[24];
	struct smart_info_payload *stored;
	unsigned char before_op[8];
	struct physic_op_param op;
	unsigned char oob[16];
};
_Static_assert(__builtin_offsetof(struct smart_info_init_frame, stored) == 24,
	       "SMART stored pointer slot");
_Static_assert(__builtin_offsetof(struct smart_info_init_frame, op) == 40,
	       "SMART operation slot");
_Static_assert(__builtin_offsetof(struct smart_info_init_frame, oob) == 64,
	       "SMART OOB slot");
__attribute__((optimize("-fno-stack-protector")))
int init_smart_info(struct smart_info_zone *zone, struct smart_info_block *block)
{
	struct smart_info_init_frame frame;
	int read_result;

	nand_memset(zone->smart, 0, 320);
	zone->smart->magic = 0xaaaaaa01U;
	zone->smart->init_count++;
	if (!block)
		return 0;
	set_physic_op_par(&frame.op, block->number,
			   (unsigned short)(zone->geometry->pages - 1),
			   zone->geometry->page_bytes, zone->data, frame.oob);
	read_result = zone->read(zone, &frame.op);
	/* OEM probes the OOB marker even when the page read failed. */
	if (is_phy_mapping_page(frame.oob) == 1 &&
	    (read_result == 0 || read_result == 10)) {
		frame.stored = (struct smart_info_payload *)(zone->data + zone->offset);
		if (frame.stored->magic == 0xaaaaaa01U) {
			nand_memcpy(zone->smart, frame.stored, 320);
			zone->smart->init_count++;
			if (zone->smart->field_164 == ~0U)
				zone->smart->field_164 = 0;
			if (zone->smart->field_172 == ~0U)
				zone->smart->field_172 = 0;
			if (zone->smart->field_176 == ~0U)
				zone->smart->field_176 = 0;
		}
	}
	print_smart(zone);
	return 0;
}
struct write_error_build_frame {
	struct physic_op_param op;
	unsigned char before_result[4];
	int result;
	struct new_write_block *retry;
	unsigned char before_index[4];
	unsigned int index;
};
_Static_assert(__builtin_offsetof(struct write_error_build_frame, result) == 28,
	       "write-error result slot");
_Static_assert(__builtin_offsetof(struct write_error_build_frame, retry) == 32,
	       "write-error failed-block pointer slot");
_Static_assert(__builtin_offsetof(struct write_error_build_frame, index) == 44,
	       "write-error index slot");
int do_write_error_in_build_list(struct write_error_zone *zone,
				 struct new_write_block *source,
				 struct new_write_block *failed,
				 unsigned short pages)
{
	/* The two gaps are OEM private-frame bytes, not initialized data. */
	struct write_error_build_frame local;

	local.retry = failed;

	set_physic_op_par(&local.op, local.retry->number, 0,
			   zone->geometry->page_bytes, 0, 0);
	local.result = zone->erase(zone, &local.op);
	/* OEM updates metadata even when erase fails, without guarding the
	 * 16-bit erase-count overflow. */
	local.retry->page = 0;
	local.retry->erase_count++;
	if (local.result) {
		nand_print(nftl_ro_lc_277);
		zone->mark_bad(zone, &local.op);
		return 1;
	}
	for (local.index = 0; pages > local.index; local.index++) {
		local.result = zone->copy(zone, source, local.retry, zone->data,
				    (unsigned short)local.index);
		/* Stock logs a failed copy and continues with the next page. */
		if (local.result)
			nand_print(nftl_ro_lc_278);
	}
	set_physic_op_par(&local.op, source->number, 0,
			   zone->geometry->page_bytes, 0, 0);
	/* OEM ignores this result, including on the zero-page path. */
	zone->mark_bad(zone, &local.op);
	if (!pages) {
		zone->current = failed;
		zone->current_page = 0;
	}
	return 0;
}
int is_last_phy_block(struct physical_block *block,
		      struct physical_zone *zone)
{
	if (block->number == zone->geometry->blocks_per_chip - 1)
		return 1;
	return 0;
}

struct valid_block_chip {
	unsigned char before_blocks[32];
	struct physical_block *blocks;
};
struct valid_block_zone {
	unsigned char before_chip[8];
	struct valid_block_chip *chip;
	unsigned char before_check[3472];
	int (*check)(struct valid_block_zone *, struct physic_op_param *);
};
int get_vaild_blocks(struct valid_block_zone *zone, unsigned int start,
		     unsigned int end)
{
	/* The OEM keeps the operation parameter below the loop locals in its
	 * 0x50-byte frame. The four-byte gap is observed, not initialized. */
	struct {
		struct physic_op_param op;
		unsigned char gap[4];
		unsigned int range;
		struct physical_block *block;
		unsigned int index;
		unsigned int remaining;
	} local;

	local.block = (struct physical_block *)
		((unsigned char *)zone->chip->blocks + ((unsigned long)start << 6));
	local.range = end - start;
	local.remaining = local.range;
	local.index = start;

	/* OEM compares the absolute index with the range length. Preserve this
	 * mistake: nonzero start can skip work or stop before end. */
	while (local.index < local.range) {
		local.op.block = local.block->number;
		if (zone->check(zone, &local.op) == 159) {
			local.remaining--;
			if (local.remaining == 0)
				return 0;
		}
		if (is_last_phy_block(local.block, (struct physical_zone *)zone) == 1)
			break;
		local.block = (struct physical_block *)
			((unsigned char *)local.block + 64);
		local.index++;
	}
	return local.remaining;
}

struct gc_enhance_geometry {
	unsigned char before_half_count[20];
	unsigned short half_count;
};
struct gc_enhance_zone {
	unsigned char before_geometry[8];
	struct gc_enhance_geometry *geometry;
	unsigned char before_used[14];
	unsigned short used;
	unsigned char before_free[648];
	unsigned short free_blocks;
	unsigned short invalid_blocks;
};
struct gc_phyinfo_view {
	unsigned char before_pages[4];
	unsigned int pages;
	unsigned short blocks;
	unsigned char chips;
};
extern unsigned char phyinfo[20];
extern int gc_all(void *zone, unsigned short count);
/* OEM places info, used and quantity in the no-stack-protector local slots. */
__attribute__((optimize("-fno-stack-protector")))
int gc_all_enhance(struct gc_enhance_zone *zone)
{
	struct gc_phyinfo_view *info = (struct gc_phyinfo_view *)phyinfo;
	unsigned int used = zone->used;
	unsigned long quantity;

	if (zone->free_blocks > used)
		return 0;
	/* The 16-bit invalid count promotes to int; its product by three fits
	 * there. Comparison with unsigned used then follows OEM unsigned order. */
	if (zone->invalid_blocks * 3 < used)
		return 0;
	quantity = (unsigned int)(info->pages * info->blocks);
	quantity *= info->chips;
	quantity >>= 11;
	if (quantity > 128)
		return 0;
	return gc_all(zone, zone->geometry->half_count >> 1);
}
struct gc_policy_zone;
extern int gc_all_cnt(struct gc_policy_zone *zone, unsigned short pages,
		      unsigned int count);
int gc_all_base_on_free_blks(struct gc_enhance_zone *zone)
{
	struct gc_phyinfo_view *info = (struct gc_phyinfo_view *)phyinfo;
	struct gc_enhance_geometry *geometry = zone->geometry;
	unsigned int used = zone->used;
	unsigned long quantity;
	unsigned int pages;
	unsigned int base;

	if (zone->free_blocks > used)
		return 0;
	if ((unsigned int)zone->invalid_blocks * 3 < used)
		return 0;
	quantity = (unsigned int)(info->pages * info->blocks);
	quantity *= info->chips;
	quantity >>= 11;
	if (quantity > 128)
		return 0;
	base = geometry->half_count >> 1;
	pages = base - 3;
	pages *= used - zone->free_blocks;
	/* OEM does not check a zero divisor. AArch64 UDIV returns zero there;
	 * preserve the stock machine behavior under the pinned compiler. */
	pages /= (unsigned int)zone->invalid_blocks * 3 - zone->free_blocks;
	pages += geometry->half_count >> 1;
	return gc_all_cnt((struct gc_policy_zone *)zone, (unsigned short)pages, 3);
}

struct hynix16nm_retry_zone {
	unsigned char before_values[56];
	unsigned char values[128];
	unsigned int retry;
};
extern unsigned char hynix16nm_read_retry_cycle;
extern unsigned char hynix16nm_read_retry_reg_num;
extern unsigned char hynix16nm_read_retry_reg_adr[];
extern int hynix16nm_vender_set_param(struct hynix16nm_retry_zone *zone,
				      unsigned char *values,
				      unsigned char *addresses,
				      unsigned int count);
/* The stock frame puts the temporary values at FP+0x20 and the loop index at
 * FP+0x2c. GCC's stack-protector layout would move both, so an unchecked
 * register count would corrupt different stack bytes. */
__attribute__((optimize("-fno-stack-protector")))
int hynix16nm_set_readretry(struct hynix16nm_retry_zone *zone)
{
	int result = 0;
	unsigned int index;
	unsigned char values[8];

	if (zone->retry > hynix16nm_read_retry_cycle)
		return -74;
	/* OEM trusts the register count. Counts beyond the inferred eight-byte
	 * array write through frame padding and then into the index at +0x2c.
	 * Keep that bug and its stack placement for stock behavior. */
	for (index = 0; index < hynix16nm_read_retry_reg_num; index++)
		values[index] = zone->values[(zone->retry << 2) + index];
	result = hynix16nm_vender_set_param(zone, values,
					 hynix16nm_read_retry_reg_adr,
					 hynix16nm_read_retry_reg_num);
	return result;
}

/* OEM keeps result at FP+0x38 and index at +0x3c. Stack-protector's
 * reordered slots would expose different scratch state to NAND callbacks. */
__attribute__((optimize("-fno-stack-protector")))
int hynix16nm_vender_set_param(struct hynix16nm_retry_zone *zone,
			      unsigned char *values,
			      unsigned char *addresses, unsigned int count)
{
	int result = 0;
	unsigned char command = 54;
	unsigned char confirm = 22;
	unsigned int index;

	nand_enable_chip((struct samsung_set_chip *)zone);
	ndfc_disable_randomize(((struct samsung_set_chip *)zone)->controller);
	/* OEM ignores both command results for every register and returns
	 * success. It also reads four values for the final diagnostic even
	 * when the caller's count is less than four. */
	for (index = 0; index < count; index++) {
		set_cmd_with_nand_bus(zone, &command, 0, addresses + index,
				      values + index, 1, 1);
		set_one_cmd((struct command_chip *)zone, confirm, 0);
	}
	nand_disable_chip((struct samsung_set_chip *)zone);
	nand_print_dbg(nftl_ro_lc_35, values[0],
		       values[1], values[2], values[3]);
	return result;
}

/* The OEM leaves this unchecked stack array without a canary. */
__attribute__((optimize("-fno-stack-protector")))
int hynix16nm_set_default_param(void *zone_ptr)
{
	struct {
		unsigned char values[8];
		int result;
		unsigned int index;
	} locals;

	locals.result = 0;
	/* OEM's mutable register count can overwrite the result and then the
	 * index after the eight-byte local buffer. */
	for (locals.index = 0; locals.index < hynix16nm_read_retry_reg_num;
	     locals.index++)
		locals.values[locals.index] =
			((struct hynix16nm_retry_zone *)zone_ptr)->values[locals.index];
	locals.result = hynix16nm_vender_set_param(
					   (struct hynix16nm_retry_zone *)zone_ptr,
					   locals.values,
					   hynix16nm_read_retry_reg_adr,
					   hynix16nm_read_retry_reg_num);
	nand_print_dbg(nftl_ro_lc_56);
	for (locals.index = 0; locals.index < hynix16nm_read_retry_reg_num;
	     locals.index++)
		nand_print_dbg(nftl_ro_lc_57, locals.values[locals.index]);
	nand_print_dbg(nftl_ro_lc_45);
	return locals.result;
}

struct hynix16nm_otp_controller {
	unsigned char before_channel[12];
	unsigned int channel;
};
struct hynix16nm_otp_zone {
	unsigned char before_chip[28];
	unsigned int chip;
	unsigned char before_values[24];
	unsigned char values[216];
	struct hynix16nm_otp_controller *controller;
};
extern void *nand_get_temp_buf(unsigned int size);
extern void nand_free_temp_buf(void *buffer);
struct hynix16nm_otp_info_chip;
extern int hynix16nm_read_otp_info_hynix(struct hynix16nm_otp_info_chip *chip,
					  unsigned char *buffer);
extern int hynix16nm_major_check_byte(unsigned char *value,
				      unsigned int mode, unsigned int level,
				      const unsigned char *primary,
				      const unsigned char *secondary,
				      unsigned int length);
/* OEM's 144-byte frame has no stack canary around its OTP vote buffers. */
__attribute__((optimize("-fno-stack-protector")))
int hynix16nm_get_read_retry_cfg(unsigned char *mode, unsigned char *cycle,
				 unsigned char *values, void *raw_buffer)
{
	const unsigned char *buffer = raw_buffer;
	unsigned char primary[8] = {0};
	unsigned char secondary[8] = {0};
	unsigned int index;
	unsigned int copy;
	int result;
	int error = 0;

	for (copy = 0; copy <= 7; copy++)
		primary[copy] = buffer[copy];
	result = hynix16nm_major_check_byte(mode, 0, 4,
					  primary, secondary, 8);
	if (result < 0) {
		nand_print(nftl_ro_lc_116);
		return result;
	}
	nand_print_dbg(nftl_ro_lc_117, *mode);
	for (copy = 0; copy <= 7; copy++)
		primary[copy] = buffer[copy + 8];
	result = hynix16nm_major_check_byte(cycle, 0, 4,
					  primary, secondary, 8);
	if (result < 0) {
		nand_print(nftl_ro_lc_118);
		return result;
	}
	nand_print_dbg(nftl_ro_lc_119, *cycle);
	if (*mode != 8 || *cycle != 4) {
		nand_print(nftl_ro_lc_120,
			   *mode, *cycle);
		return -81;
	}
	for (index = 0; index < 32; index++) {
		for (copy = 0; copy <= 7; copy++) {
			primary[copy] = 0;
			secondary[copy] = 0;
			primary[copy] = buffer[16 + copy * 64 + index];
			secondary[copy] = buffer[48 + copy * 64 + index];
		}
		result = hynix16nm_major_check_byte(values + index, 1, 4,
						  primary, secondary, 8);
		if (result < 0) {
			nand_print(nftl_ro_lc_121,
				   index);
			error = result;
			break;
		}
	}
	/* OEM prints all 32 result bytes, even after the first failed vote left
	 * the remaining output bytes untouched. Keep this stale-data exposure. */
	for (index = 0; index < 32; index++) {
		if ((index & 7) == 0 && index != 0)
			nand_print_dbg(nftl_ro_lc_45);
		nand_print_dbg(nftl_ro_lc_122, values[index]);
	}
	nand_print_dbg(nftl_ro_lc_45);
	return error;
}
/* Match OEM frame slots for the private mode, cycle and temporary buffer. */
__attribute__((optimize("-fno-stack-protector")))
int hynix16nm_get_rr_value_otp_hynix(struct hynix16nm_otp_zone *zone)
{
	int result = 0;
	void *buffer = nand_get_temp_buf(528);
	unsigned char mode;
	unsigned char cycle;

	if (!buffer) {
		nand_print(nftl_ro_lc_125);
		return -78;
	}
	result = hynix16nm_read_otp_info_hynix(
				(struct hynix16nm_otp_info_chip *)zone, buffer);
	/* OEM still parses the buffer after a failed OTP read. */
	if (result < 0)
		nand_print(nftl_ro_lc_126,
			   zone->controller->channel, zone->chip);
	result = hynix16nm_get_read_retry_cfg(&mode, &cycle, zone->values,
					       buffer);
	if (result < 0)
		nand_print(nftl_ro_lc_127,
			   zone->controller->channel, zone->chip);
	nand_free_temp_buf(buffer);
	return result;
}
int hynix16nm_get_otp_param(struct hynix16nm_otp_zone *zone,
			    unsigned char *values)
{
	/* Keep the five OEM scalar slots together in their original frame order.
	 * The retry loop remains unbounded when the OTP helper keeps failing. */
	struct {
		int result;
		unsigned int complete;
		unsigned int attempt;
		unsigned int index;
		unsigned int group;
	} local;

	local.attempt = 0;
	local.complete = 0;

	/* OEM retries forever when OTP parsing never returns zero; keep the
	 * unbounded loop for stock behavior. */
	while (local.complete == 0) {
		nand_print_dbg(nftl_ro_lc_54,
			       local.attempt);
		local.result = hynix16nm_get_rr_value_otp_hynix(zone);
		if (local.result == 0)
			local.complete = 1;
		local.attempt++;
	}
	for (local.group = 0; local.group <= 7; local.group++) {
		for (local.index = 0; local.index <= 3; local.index++)
			values[(local.group << 2) + local.index] =
				zone->values[(local.group << 2) + local.index];
	}
	return 0;
}
extern unsigned char sandisk_acti_start_cmd[2];
extern unsigned char sandisk_read_retry_mode;
extern unsigned char sandisk_read_retry_cycle;
extern unsigned char sandisk_read_retry_reg_cnt;
extern unsigned char sandisk_read_retry_cmd_19[3];
extern unsigned char sandisk_read_retry_cmd_1y[1];
int sandisk_vender_set_param(void *chip, unsigned char *data,
			     unsigned char *address, unsigned int ignored)
{
	int result = 0;

	if (sandisk_read_retry_mode == 48 || sandisk_read_retry_mode == 49) {
		result = set_cmd_with_nand_bus(chip, sandisk_acti_start_cmd,
					       0, 0, 0, 0, 2);
		/* OEM ORs status codes rather than returning the first error. */
		result |= set_cmd_with_nand_bus(chip, sandisk_read_retry_cmd_19,
						0, address, data, 1, 3);
	}
	if (sandisk_read_retry_mode == 50)
		result = set_cmd_with_nand_bus(chip, sandisk_read_retry_cmd_1y,
						1, address, data, 4, 1);
	return result;
}
extern unsigned char sandisk_acti_start_addr[9];
extern const unsigned char sandisk_activation_commands[9];
/* OEM activation has no stack-canary prologue despite its local command data. */
__attribute__((optimize("-fno-stack-protector")))
int sandisk_activation_seq(struct samsung_set_chip *chip)
{
	unsigned char commands[12];
	unsigned char data[16];
	int result;

	__builtin_memcpy(commands, sandisk_activation_commands, 9);
	data[0] = 0;
	data[1] = 0;
	data[2] = 0;
	data[3] = 0;
	data[4] = 0;
	data[5] = 0;
	data[6] = 0;
	data[7] = 0;
	data[8] = 0;

	nand_enable_chip(chip);
	ndfc_disable_randomize(chip->controller);
	result = set_cmd_with_nand_bus(chip, sandisk_acti_start_cmd,
					0, 0, 0, 0, 2);
	/* OEM sends the second command after a failed first command and
	 * combines status codes by bitwise OR. */
	result |= set_cmd_with_nand_bus(chip, commands, 0,
					 sandisk_acti_start_addr,
					 data, 1, 9);
	nand_disable_chip(chip);
	if (result)
		nand_print(nftl_ro_lc_136, result);
	return result;
}
struct cross_talk_geometry {
	unsigned char before_pages[20];
	unsigned short pages;
	unsigned char before_fixed_page[6];
	unsigned short fixed_page;
};
struct cross_talk_stats {
	unsigned char before_max[168];
	unsigned int max;
};
struct cross_talk_zone {
	unsigned char before_geometry[8];
	struct cross_talk_geometry *geometry;
	unsigned char before_data[496];
	void *data;
	unsigned char before_stats[2920];
	struct cross_talk_stats *stats;
	unsigned char before_read[16];
	int (*read)(struct cross_talk_zone *, struct physic_op_param *);
};
/* OEM's page scan has an OOB stack array but no canary prologue. The raw
 * callback receives this pointer with a geometry-derived spare length; stock
 * does not check that length against the 16-byte local array. */
__attribute__((optimize("-fno-stack-protector")))
int check_cross_talk(struct cross_talk_zone *zone,
		     struct physical_block *block)
{
	void *data = zone->data;
	unsigned int index = 0;
	/* The stock frame puts this array at x29+0x40, after the page op. */
	unsigned char oob[16] __attribute__((aligned(8)));
	struct physic_op_param op;
	unsigned char *oobptr;
	int result;

	for (; index < zone->geometry->pages; index++) {
		data = 0;
		oobptr = oob;
		set_physic_op_par(&op, block->number, index,
				  zone->geometry->fixed_page, data, oobptr);
		result = zone->read(zone, &op);
		if (result == 0 && oobptr[14] <= 99 &&
		    zone->stats->max <= oobptr[14]) {
			/* Keep the OEM's final store order: fetch stats before
			 * reading OOB[14]. GCC 7.4.1 otherwise evaluates the OOB
			 * byte first. The volatile local uses the spare frame slot
			 * at FP+0x20 without moving op or OOB; the OEM's unchecked
			 * 16-byte OOB buffer remains unchanged. */
			struct cross_talk_stats *volatile target = zone->stats;

			target->max = oobptr[14];
			break;
		}
	}
	/* OEM reports success even if every physical read failed. */
	return 0;
}
struct used_page_geometry {
	unsigned char before_sectors[8];
	unsigned char sectors_per_page;
	unsigned char before_pages[11];
	unsigned short pages;
	unsigned char before_page_bytes[4];
	unsigned short page_bytes;
	unsigned short fixed_page;
};
struct used_page_stats {
	unsigned char before_cross_talk[168];
	unsigned int cross_talk;
};
struct used_page_config {
	unsigned char before_cross_talk[28];
	unsigned short cross_talk;
};
struct used_page_zone {
	unsigned char before_geometry[8];
	struct used_page_geometry *geometry;
	unsigned char before_data[496];
	void *data;
	unsigned char before_stats[2920];
	struct used_page_stats *stats;
	struct used_page_config *config;
	unsigned char before_read[8];
	int (*read)(struct used_page_zone *, struct physic_op_param *);
};
struct used_page_cache {
	unsigned int count;
	unsigned char before_valid[4];
	unsigned char *valid;
	unsigned char *data;
	unsigned int *map;
};
struct poweroff_ecc_zone;
extern int nand_power_off_ecc_error(struct poweroff_ecc_zone *,
				     const unsigned char *, unsigned char *);
_Static_assert(__builtin_offsetof(struct used_page_zone, data) == 512,
	       "used-page data offset");
_Static_assert(__builtin_offsetof(struct used_page_zone, stats) == 3440,
	       "used-page stats offset");
_Static_assert(__builtin_offsetof(struct used_page_zone, read) == 3464,
	       "used-page read offset");

__attribute__((optimize("-fno-stack-protector")))
int get_used_page_num_no_crosstalk(struct used_page_zone *zone,
				   struct physical_block *block,
				   unsigned int *good_pages,
				   struct used_page_cache *cache)
{
	struct physic_op_param op;
	unsigned char local_oob[16];
	unsigned char *data;
	unsigned char *oob;
	int first_unused = -1;
	unsigned int good = 0;
	unsigned int unused_count = 0;
	unsigned int page;
	int status;

	/* OEM zeroes this output and never writes the later good-page count. */
	*good_pages = 0;
	for (page = 0; page < zone->geometry->pages; page++) {
		if (cache && cache->valid[page] == 0) {
			/* OEM multiplies in 32 bits and trusts the cache extent. */
			data = cache->data +
				(unsigned int)(page * (zone->geometry->page_bytes + 64));
			oob = data + zone->geometry->page_bytes;
		} else {
			data = 0;
			if (zone->geometry->sectors_per_page == 4)
				data = zone->data;
			oob = local_oob;
		}
		set_physic_op_par(&op, block->number, page,
				  zone->geometry->fixed_page, data, oob);
		status = zone->read(zone, &op);
		if (status != 0 && status != 10) {
			nand_print(nftl_ro_lc_310,
				   block->number, page);
			if (first_unused == -1)
				first_unused = page;
			continue;
		}
		if (is_nouse_page(oob) == 1) {
			if (first_unused == -1)
				first_unused = page;
			/* Stock does not reset this count after a good page, so
			 * three nonconsecutive unused pages also stop the scan. */
			unused_count++;
			if (unused_count == 3)
				break;
			continue;
		}
		if (nand_power_off_ecc_error(
				(struct poweroff_ecc_zone *)zone, data, oob)) {
			nand_print(nftl_ro_lc_311);
			continue;
		}
		good++;
		if (first_unused == -1) {
			/* OEM dereferences cache unchecked on this path. */
			cache->valid[page] = 1;
			cache->map[page] = get_logic_page_from_oob(oob);
			if (cache->count < page + 1)
				cache->count = page + 1;
		}
	}
	if (first_unused == -1)
		first_unused = zone->geometry->pages;
	else if (good != (unsigned int)first_unused) {
		nand_print(nftl_ro_lc_312,
			   block->number, good, first_unused);
		zone->stats->cross_talk++;
		/* OEM wraps the product at 32 bits, then widens before adding the
		 * page size. Keep that order for oversized or corrupt cache counts. */
		for (page = 0; page < cache->count; page++) {
			if (cache->valid[page])
				cache->data[
					(unsigned long)(unsigned int)(page *
					(zone->geometry->page_bytes + 64)) +
					zone->geometry->page_bytes + 14] =
					zone->stats->cross_talk;
		}
	}
	return first_unused;
}
extern unsigned char sandisk_A19_read_retry_reg_adr_1y;
extern unsigned char sandisk_A19_1z_16g[128];
extern unsigned char sandisk_A19_1z_8g[124];
extern unsigned char sandisk_A19_1y[124];
extern unsigned char sandisk_A19_read_retry_mode;
extern unsigned char sandisk_A19_read_retry_cycle;
extern unsigned char sandisk_A19_read_retry_reg_cnt;
struct sandisk_a19_init_params {
	unsigned char before_retry_spec[52];
	unsigned int retry_spec;
};
struct sandisk_a19_init_chip {
	unsigned char before_retry[184];
	unsigned int retry;
	unsigned char before_params[92];
	struct sandisk_a19_init_params *params;
};
int sandisk_A19_readretry_init(struct sandisk_a19_init_chip *chip)
{
	chip->retry = 0;
	/* OEM separately reloads the packed word for all three fields and
	 * dereferences the inner pointer without checking it. */
	sandisk_A19_read_retry_mode = chip->params->retry_spec >> 16;
	sandisk_A19_read_retry_cycle = chip->params->retry_spec >> 8;
	sandisk_A19_read_retry_reg_cnt = chip->params->retry_spec;
	if (sandisk_A19_read_retry_mode != 50 &&
	    sandisk_A19_read_retry_mode != 51 &&
	    sandisk_A19_read_retry_mode != 52 &&
	    sandisk_A19_read_retry_mode != 53) {
		nand_print(nftl_ro_lc_98,
			   sandisk_A19_read_retry_mode);
		return -99;
	}
	nand_print_dbg(nftl_ro_lc_99,
		       sandisk_A19_read_retry_cycle);
	return 0;
}
/* The standard SanDisk initializer uses the same chip/configuration offsets
 * as A19. Retain separate global settings and its activation branch. */
int sandisk_readretry_init(struct sandisk_a19_init_chip *chip)
{
	chip->retry = 0;
	/* OEM reloads the configuration pointer for each field, without a
	 * null check, and does not use the activation return value. */
	sandisk_read_retry_mode = chip->params->retry_spec >> 16;
	sandisk_read_retry_cycle = chip->params->retry_spec >> 8;
	sandisk_read_retry_reg_cnt = chip->params->retry_spec;
	if (sandisk_read_retry_mode == 48 || sandisk_read_retry_mode == 49)
		(void)sandisk_activation_seq((struct samsung_set_chip *)chip);
	if (sandisk_read_retry_mode != 48 &&
	    sandisk_read_retry_mode != 49 &&
	    sandisk_read_retry_mode != 50)
		return -99;
	nand_print_dbg(nftl_ro_lc_137,
		       sandisk_read_retry_cycle);
	return 0;
}
extern int sandisk_A19_vender_set_param(void *chip, void *value,
					void *address, int unused);
int sandisk_A19_set_readretry(struct samsung_set_chip *chip,
			      unsigned short ignored)
{
	int result = 0;
	unsigned char *values;
	unsigned char *address;
	unsigned int count;

	nand_enable_chip(chip);
	ndfc_disable_randomize(chip->controller);
	/* OEM indexes unchecked four-byte rows; an out-of-range retry reads
	 * beyond the vendor table and sends the resulting bytes to NAND. */
	if (sandisk_A19_read_retry_mode == 52)
		values = sandisk_A19_1z_16g + ((unsigned long)chip->retry << 2);
	else if (sandisk_A19_read_retry_mode == 53)
		values = sandisk_A19_1z_8g + ((unsigned long)chip->retry << 2);
	else
		values = sandisk_A19_1y + ((unsigned long)chip->retry << 2);
	address = &sandisk_A19_read_retry_reg_adr_1y;
	count = 1;
	result = sandisk_A19_vender_set_param(chip, values, address, count);
	/* OEM ignores the final command result. */
	set_one_cmd((struct command_chip *)chip, 93, 0);
	nand_disable_chip(chip);
	return result;
}
extern int page_write(unsigned short chip, unsigned short block,
		      unsigned short page, unsigned int mode,
		      void *data, void *oob);
extern void *nftl_malloc(unsigned int size);
extern void *nand_memset(void *destination, int value, unsigned int size);
/* OEM's diagnostic writer has no stack-canary prologue despite its local OOB. */
__attribute__((optimize("-fno-stack-protector")))
int nand_dbg_phy_write(unsigned short chip, unsigned short block,
		       unsigned short page)
{
	/* The stock stack reserves twenty bytes for eleven initialized OOB bytes. */
	unsigned char oob[20];
	void *buffer = nftl_malloc(8192);
	int result;

	/* OEM logs allocation failure but still passes NULL to nand_memset and
	 * page_write. Preserve that behavior for the pinned build. */
	if (!buffer)
		nand_print(nftl_ro_lc_385, nftl_ro_func_7589);
	nand_memset(buffer, -91, 4096);
	oob[0] = 0xff;
	oob[1] = 0xff;
	oob[2] = 0xff;
	oob[3] = 0xff;
	oob[4] = 0xff;
	oob[5] = 0xa5;
	oob[6] = 0xa5;
	oob[7] = 0xff;
	oob[8] = 0xff;
	oob[9] = 0xff;
	oob[10] = 0xff;
	result = page_write(chip, block, page, 8, buffer, oob);
	nftl_free(buffer);
	return result;
}
struct dbg_zone_write_geometry {
	unsigned char before_page[28];
	unsigned short page;
};
struct dbg_zone_write_owner {
	unsigned char before_geometry[8];
	struct dbg_zone_write_geometry *geometry;
	unsigned char before_write[3456];
	int (*write)(struct dbg_zone_write_owner *, struct physic_op_param *);
};
/* OEM's diagnostic writer has no canary despite its stack OOB buffer. */
__attribute__((optimize("-fno-stack-protector")))
int nand_dbg_zone_phy_write(struct dbg_zone_write_owner *zone,
			    unsigned short block, unsigned short page)
{
	struct physic_op_param op;
	unsigned char oob[16];
	void *buffer = nftl_malloc(8192);
	struct physical_block *physical;
	int result;

	/* OEM logs allocation failure, then passes NULL to nand_memset and
	 * the write callback. Preserve the fault-prone diagnostic path. */
	if (!buffer)
		nand_print(nftl_ro_lc_385, nftl_ro_func_7580);
	physical = (struct physical_block *)
		get_phy_block_addr((struct nftl_phy_outer *)zone, block);
	nand_memset(buffer, -91, 4096);
	oob[0] = 0xff;
	oob[1] = 0xff;
	oob[2] = 0xff;
	oob[3] = 0xff;
	oob[4] = 0xff;
	oob[5] = 0xa5;
	oob[6] = 0xa5;
	oob[7] = 0xff;
	oob[8] = 0xff;
	oob[9] = 0xff;
	oob[10] = 0xff;
	set_physic_op_par(&op, physical->number, page, zone->geometry->page,
			  buffer, oob);
	result = zone->write(zone, &op);
	nftl_free(buffer);
	return result;
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
/* The nrc and used-page cache prefixes are the same stock allocation. */
_Static_assert(sizeof(struct nrc_buffers) == sizeof(struct used_page_cache),
	       "nrc cache prefix");

__attribute__((optimize("-fno-stack-protector")))
unsigned int get_used_page_num(struct used_page_zone *zone,
			       struct physical_block *block,
			       unsigned int *read_errors,
			       struct used_page_cache *cache)
{
	struct physic_op_param op;
	unsigned char local_oob[16];
	unsigned char *data, *oob;
	unsigned int page, first_unused, trailing_unused;
	int status;

	*read_errors = 0;
	trailing_unused = 0;
	for (page = 0; page < zone->geometry->pages; page++) {
		if (cache && cache->valid[page] == 0) {
			/* Stock multiplies in 32 bits, even on overflow. */
			data = cache->data + (unsigned int)(page *
				(zone->geometry->page_bytes + 64));
			oob = data + zone->geometry->page_bytes;
		} else {
			data = 0;
			if (zone->geometry->sectors_per_page == 8)
				data = zone->data;
			oob = local_oob;
		}
		if (zone->config->cross_talk)
			set_physic_op_par(&op, block->number, page,
				zone->geometry->fixed_page, data, oob);
		else
			set_physic_op_par(&op, block->number, page, 0, 0, oob);
		status = zone->read(zone, &op);
		if (status && status != 10) {
			nand_print(nftl_ro_lc_307,
				   block->number, page);
			(*read_errors)++;
		}
		if (is_nouse_page(oob) == 1)
			break;
		if ((!status || status == 10) && data) {
			if (nand_power_off_ecc_error(
				(struct poweroff_ecc_zone *)zone, data, oob)) {
				nand_print(nftl_ro_lc_308);
			} else {
				/* OEM assumes cache exists whenever data is nonnull. */
				cache->valid[page] = 1;
				if (cache->count < page + 1)
					cache->count = page + 1;
			}
		}
	}
	first_unused = page;
	/* OEM still reads every remaining page, but never adds second-pass
	 * read failures to the caller's error count. */
	for (; page < zone->geometry->pages; page++) {
		if (cache && cache->valid[page] == 0) {
			data = cache->data + (unsigned int)(page *
				(zone->geometry->page_bytes + 64));
			oob = data + zone->geometry->page_bytes;
		} else {
			data = 0;
			oob = local_oob;
		}
		if (zone->config->cross_talk)
			set_physic_op_par(&op, block->number, page,
				zone->geometry->fixed_page, data, oob);
		else
			set_physic_op_par(&op, block->number, page, 0, 0, oob);
		status = zone->read(zone, &op);
		if (is_nouse_page(oob) == 1) {
			trailing_unused++;
			continue;
		}
		nand_print(nftl_ro_lc_309,
			   block->number, page, oob[0], oob[1], oob[2],
			   oob[3], oob[4], oob[5], oob[6], oob[7],
			   oob[8], oob[9], oob[10], oob[11],
			   oob[12], oob[13], oob[14], oob[15]);
		if ((!status || status == 10) && data) {
			cache->valid[page] = 1;
			if (cache->count < page + 1)
				cache->count = page + 1;
		}
	}
	return first_unused;
}
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

void set_spare_data(unsigned char *oob, unsigned int value, unsigned char offset,
		    unsigned char count)
{
	unsigned int index = 0;
	unsigned int shift;

	/* OEM stores offset/count as bytes at entry, dropping higher bits. */
	while (count > index) {
		shift = count - index;
		shift--;
		shift <<= 3;
		/* AArch64 masks variable 32-bit shift counts to five bits.
		 * Preserve that OEM wraparound for counts above four. */
		oob[(unsigned int)offset + index] =
			(unsigned char)((unsigned int)value >> (shift & 31));
		index++;
	}
}
void set_start_block_flag(void *unused, unsigned char *oob,
			  unsigned int number)
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
	unsigned char before_need_claim[24];
	unsigned char need_claim;
	unsigned char before_complete[660 - 25];
	unsigned int read_complete;
	unsigned char before_settings[3440 - 664];
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

/* Isolated NFTL C candidates; private aggregate labels are inferred. */

static void nftl_version(void)
{
	nand_print(nftl_ro_lc_15,
		   nftl_ro_lc_14, nftl_ro_lc_13, nftl_ro_lc_12, nftl_ro_lc_11, nftl_ro_lc_10);
}

struct discard_zone {
	unsigned char before_debug[0x290];
	int debug_level;
};
struct nand_discard_view;
extern int nand_discard(struct nand_discard_view *zone, unsigned int sector,
			unsigned int length);
int __nand_discard(struct discard_zone *zone, unsigned int sector,
		   unsigned int length)
{
	int result;

	if (zone->debug_level == 3)
		/* Preserve OEM's misspelled diagnostic. */
		nand_print_dbg(nftl_ro_lc_417,
			       sector, length);
	result = nand_discard((struct nand_discard_view *)zone, sector, length);
	return result;
}

struct exit_zone {
	unsigned char before_a[0x1d0];
	void *a;
	unsigned char before_d[0x200 - 0x1d8];
	void *d;
	void *e;
	unsigned char before_b[0x218 - 0x210];
	void *b;
	unsigned char before_c[0x258 - 0x220];
	void *c;
};
struct exit_zone_slots {
	unsigned char before_slots[8];
	void *volatile slots[33];
};
_Static_assert(__builtin_offsetof(struct exit_zone_slots, slots) == 8,
	       "logic-page slot base");
extern void nftl_free(void *ptr);
/* Stock reloads the caller's zone pointer before each slot address. The
 * volatile pointer and slots preserve those reads, including a possible
 * concurrent change between the non-null probe and the free argument.
 * The OEM also leaves every freed slot dangling. */
int free_logic_page_map(struct exit_zone *volatile zone)
{
	unsigned int index;

	for (index = 0; index <= 24; index++) {
		/* OEM probes each pointer and then reads it again for the free.
		 * The zone layout is inferred from slot offsets 72..264.
		 * It leaves freed pointers dangling and never checks zone. */
		if (((struct exit_zone_slots *)zone)->slots[(unsigned long)index + 8])
			nftl_free(((struct exit_zone_slots *)zone)->slots[(unsigned long)index + 8]);
	}
	return 0;
}
int zone_param_exit(struct exit_zone *zone)
{
	/* OEM ignores every cleanup result and leaves dangling pointers. */
	free_logic_page_map(zone);
	nftl_free(zone->a);
	nftl_free(zone->b);
	nftl_free(zone->c);
	nftl_free(zone->d);
	nftl_free(zone->e);
	return 0;
}

extern int fill_no_use_data_to_page(void *zone);
/* Kernel -fstack-protector reorders two integer locals without adding a
 * canary; disabling it for this function restores the stock stack layout. */
__attribute__((optimize("-fno-stack-protector")))
int fill_no_use_page_to_current_block(void *zone, int count)
{
	int index;
	int result;

	for (index = 0; index < count; index++) {
		result = fill_no_use_data_to_page(zone);
		/* OEM treats zero as early success and nonzero as continuation. */
		if (result == 0)
			return 0;
	}
	return 1;
}

struct no_use_data_extent {
	unsigned char before_count[20];
	unsigned short count;
};
struct no_use_data_zone {
	unsigned char before_extent[8];
	struct no_use_data_extent *extent;
};
__attribute__((optimize("-fno-stack-protector")))
int fill_no_use_data_to_current_block(struct no_use_data_zone *zone)
{
	int index;
	int result;

	for (index = 0; index < zone->extent->count; index++) {
		result = fill_no_use_data_to_page(zone);
		/* OEM treats zero as early success and re-reads count each
		 * iteration, including after the helper mutates the extent. */
		if (result == 0)
			return 0;
	}
	return 1;
}

/* Bounded C candidates for stock NFTL assembly, not OEM declaration claims. */

extern void *nand_malloc(unsigned int size);
extern void nftl_free(void *object);
extern int hynix16nm_set_default_param(void *chip);
struct otp_chip;
extern int hynix20nm_set_default_param(struct otp_chip *chip);
extern int hynix26nm_setdefaultparam(void *chip);
extern int hynix26nm_getdefaultparam(void *chip, unsigned char *values);

struct hynix_default_channel {
	unsigned char before_number[12];
	unsigned int number;
};
struct hynix_default_chip {
	unsigned char before_chip[28];
	unsigned int chip;
	unsigned char before_channel[240];
	struct hynix_default_channel *channel;
};
int hynix26nm_get_default_param(struct hynix_default_chip *chip)
{
	/* Eight-byte alignment keeps the scratch bytes at OEM frame offset 40. */
	unsigned char values[4] __attribute__((aligned(8)));

	/* OEM ignores the helper result and may print uninitialized bytes
	 * before resetting the device to defaults. */
	hynix26nm_getdefaultparam(chip, values);
	nand_print_dbg(nftl_ro_lc_89,
		       chip->channel->number, chip->chip,
		       values[0], values[1], values[2], values[3]);
	hynix26nm_setdefaultparam(chip);
	return 0;
}
extern unsigned char hynix26nm_read_retry_reg_num;
extern unsigned char hynix26nm_read_retry_reg_adr[4];
struct hynix_saved_chip {
	unsigned char before_saved[56];
	unsigned char saved[4];
};
/* The global build's stack protector swaps these two scalar local slots;
 * the OEM frame uses the unprotected layout despite having no canary. */
__attribute__((optimize("-fno-stack-protector")))
int hynix26nm_getdefaultparam(void *chip, unsigned char *values)
{
	int index;
	int result = 0;

	result = hynix26nm_vender_get_param((struct samsung_set_chip *)chip,
			((struct hynix_saved_chip *)chip)->saved,
		hynix26nm_read_retry_reg_adr,
		hynix26nm_read_retry_reg_num);
	/* OEM copies from the chip scratch bytes even when the bus read failed.
	 * It reloads the mutable global count on every loop condition. */
	for (index = 0; hynix26nm_read_retry_reg_num > index; index++)
		values[index] = ((struct hynix_saved_chip *)chip)->saved[index];
	return result;
}

struct nftl_retry_chip {
	unsigned char preceding[0xb8];
	unsigned int retry_count;
};

int hynix16nm_readretry_exit(struct nftl_retry_chip *chip)
{
	nand_print_dbg(nftl_ro_lc_130);
	hynix16nm_set_default_param(chip);
	chip->retry_count = 0;
	return 0;
}

int hynix20nm_readretry_exit(struct nftl_retry_chip *chip)
{
	nand_print_dbg(nftl_ro_lc_65);
	hynix20nm_set_default_param((struct otp_chip *)chip);
	chip->retry_count = 0;
	return 0;
}

int hynix26nm_readretry_exit(struct nftl_retry_chip *chip)
{
	nand_print_dbg(nftl_ro_lc_90);
	hynix26nm_setdefaultparam(chip);
	chip->retry_count = 0;
	return 0;
}

void *nftl_malloc(unsigned int size)
{
	/* OEM warns above 1.5 MiB but still attempts the allocation. */
	if (size > 0x180000)
		nand_print(nftl_ro_lc_9, size);
	return nand_malloc(size);
}

struct nftl_sector_cache {
	unsigned char preceding[8];
	void *allocation;
};

struct nftl_cache_owner {
	unsigned char preceding[0xb40];
	struct nftl_sector_cache cache;
};

int nftl_sector_cache_exit(struct nftl_cache_owner *owner)
{
	struct nftl_sector_cache *cache = &owner->cache;
	nftl_free(cache->allocation);
	return 0;
}

/* Inferred NFTL C body, checked against both linked stock object builds.
 * Layout and parameter names remain provisional. */

__attribute__((optimize("-fno-stack-protector")))
int is_all_byte_ff(const unsigned char *buffer, int length, int tolerance)
{
	int count = 0;
	int required = length - tolerance;
	int index;

	/* OEM scans the entire length, then allows `tolerance` non-FF bytes.
	 * A negative length scans nothing and can still report success. Signed
	 * overflow in length - tolerance is also unguarded. Preserve both. */
	for (index = 0; index < length; index++) {
		if (buffer[index] == 0xff)
			count++;
	}
	if (count >= required)
		return 1;
	return 0;
}

/* Inferred NFTL per-byte bit counter. */
/* The byte mask compiles as AND #0xff, where stock uses UXTB. Both discard
 * the upper 24 bits before the same bit-count loop. */
__attribute__((optimize("-fno-stack-protector")))
int check_bit_nums(const unsigned char *data, int length)
{
	int index;
	int bit;
	int total;
	unsigned char mask;

	total = 0;
	for (index = 0; index < length; index++) {
		mask = 1;
		for (bit = 0; bit <= 7; bit++) {
			if (data[index] & mask)
				total++;
			mask <<= 1;
		}
	}
	/* OEM skips the loop for a negative length and does not guard a
	 * positive length against the actual buffer extent. */
	return total;
}

/* Isolated OEM C equivalents; aggregate names are inferred from access. */

void set_end_block_flag(void *unused, unsigned char *oob)
{
	/* OEM ignores the first argument and does not propagate write errors. */
	oob[0] = 0xff;
	set_spare_data(oob, 0xaa, 1, 1);
	set_spare_data(oob, 0xcc, 2, 1);
}

void set_no_use_page(void *unused, unsigned char *oob)
{
	/* OEM ignores the first argument and does not propagate write errors. */
	oob[0] = 0xff;
	set_spare_data(oob, 0xaa, 1, 1);
	set_spare_data(oob, 0xbb, 2, 1);
}

void set_oob_special_page(void *unused, unsigned char *oob,
			  unsigned int block, unsigned int page,
			  unsigned short erase_count)
{
	/* OEM ignores the first argument and every helper result. */
	oob[0] = 0xff;
	set_spare_data(oob, block, 1, 4);
	set_spare_data(oob, erase_count, 5, 2);
	set_spare_data(oob, page, 7, 4);
}

void set_oob_logic_page(void *unused, unsigned char *oob,
			unsigned int block, unsigned int page,
			unsigned short erase_count)
{
	/* OEM ignores the first argument and every helper result. */
	oob[0] = 0xff;
	set_spare_data(oob, block, 1, 4);
	set_spare_data(oob, erase_count, 5, 2);
	set_spare_data(oob, page, 7, 4);
	/* OEM forces the two high bits of byte one after encoding the block. */
	oob[1] |= 0xc0;
}

struct invalid_page_node {
	unsigned short number;
	unsigned char before_count[2];
	unsigned short invalid_pages;
	unsigned char before_next[10];
	struct invalid_page_node *next;
	struct invalid_page_node *previous;
};
struct invalid_page_zone {
	unsigned char before_geometry[8];
	struct page_geometry *geometry;
	unsigned char before_head[336 - 16];
	struct invalid_page_node head;
};

/* Keep the OEM node/previous private slots at FP+0x20/+0x28. GCC's
 * stack-protector layout swaps them even though this body has no canary. */
__attribute__((optimize("-fno-stack-protector")))
int phy_block_from_invalid_page_incr(struct invalid_page_zone *zone,
				     struct invalid_page_node *node)
{
	struct invalid_page_node *moving;
	struct invalid_page_node *previous;

	if (!node->next && !node->previous) {
		node->invalid_pages++;
		if (node->invalid_pages > zone->geometry->pages_per_block)
			nand_print_dbg(nftl_ro_lc_411,
				       node->number, node->invalid_pages);
		return 0;
	}
	node->invalid_pages++;
	if (node->invalid_pages > zone->geometry->pages_per_block)
		nand_print_dbg(nftl_ro_lc_412);
	previous = node->previous;
	/* Stock copies the argument into FP+0x20 before walking the list. */
	moving = node;
	if (previous == &zone->head)
		return 0;
	/* OEM assumes a coherent doubly linked list. A malformed link may be
	 * dereferenced, and the 16-bit page count wraps before the bound check. */
	while (previous->invalid_pages < moving->invalid_pages) {
		moving->previous = previous->previous;
		previous->previous->next = moving;
		previous->next = moving->next;
		if (moving->next)
			moving->next->previous = previous;
		moving->next = previous;
		previous->previous = moving;
		previous = moving->previous;
		if (!previous->previous)
			break;
	}
	return 0;
}

unsigned int get_total_invalid_page(void *zone)
{
	struct invalid_page_node *node;
	unsigned int total;
	total = 0;
	node = (struct invalid_page_node *)((char *)zone + 336);
	/* OEM assumes a finite, intact list and allows unsigned wraparound. */
	while (node->next) {
		total += node->next->invalid_pages;
		node = node->next;
	}
	return total;
}

/* OEM shifts a signed byte count and subtracts unchecked exclusions; bad
 * caller counts can overflow before the bit count is compared. */
__attribute__((optimize("-fno-stack-protector")))
int is_all_bit_ff(void *data, int bytes, int excluded_bits)
{
	int expected = bytes << 3;
	int counted;
	expected -= excluded_bits;
	counted = check_bit_nums(data, bytes);
	if (counted >= expected)
		return 1;
	return 0;
}

extern int do_prio_gc(void *zone);
__attribute__((optimize("-fno-stack-protector")))
int prio_gc_all(void *zone)
{
	unsigned int index;
	int result;
	result = 0;
	index = 0;
	while (index <= 9) {
		result |= do_prio_gc(zone);
		++index;
	}
	return result;
}

/* OEM NFTL CRC16 helper; lookup data lives in the stock-ordered C pool. */
extern const unsigned short nand_crc16_table[256];

/* The compiler uses AND #0xffff at two sites where stock uses UXTH.
 * Both keep the same low halfword; the table and all other instructions
 * are verified against the retained stock object. */
static unsigned short nand_crc16_byte(unsigned short crc, unsigned char value)
{
	return (crc >> 8) ^ nand_crc16_table[(crc ^ value) & 0xff];
}

unsigned short nand_crc16(unsigned short crc, const unsigned char *data,
			  unsigned int length)
{
	/* OEM decrements the 32-bit count until zero. A negative caller count
	 * becomes a huge unsigned span and can read well beyond the buffer. */
	while (length--)
		crc = nand_crc16_byte(crc, *data++);
	return crc;
}

/* Isolated C equivalents for four NFTL bodies, not original type claims. */

extern void nftl_free(void *allocation);
extern void clean_block_list(void *zone);
struct nftl_clean_zone;
extern int nand_clean_zone_table2(struct nftl_clean_zone *zone);

struct nftl_cache_entry_view {
	unsigned char before_allocation[24];
	void *allocation;
	unsigned char after_allocation[24];
};
/* The 32 slots and terminal page buffer occupy bytes 8..1807 of the
 * embedded cache. The OEM loop trusts the count from the configuration;
 * it does not cap it at the physical number of slots. */
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

int nftl_cache_init(void *owner)
{
	struct cache_init_zone *zone = owner;
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
	for (index = 0; index < cache->count; index++) {
		struct cache_init_entry *entry = &cache->entries[index];

		entry->index = index;
		entry->status = 255;
		entry->page = -1;
		entry->links[0] = 0;
		entry->links[1] = 0;
		entry->links[2] = 0;
		entry->links[3] = 0;
		entry->field0 = 0;
		entry->field1 = 0;
		entry->allocation = nftl_malloc(zone->geometry->allocation_size);
		if (!entry->allocation) {
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

struct nand_cache_read_geometry {
	unsigned char before_sectors[8];
	unsigned char sectors_per_page;
	unsigned char before_page_bytes[17];
	unsigned short page_bytes;
};
struct nand_cache_read_stats {
	unsigned char before_ecc_reads[116];
	unsigned int ecc_reads;
};
struct nand_cache_read_configuration {
	unsigned short count;
};
struct nand_cache_read_zone {
	unsigned char before_geometry[8];
	struct nand_cache_read_geometry *geometry;
	unsigned char before_cache[1056];
	struct cache_init_data cache;
	unsigned char before_stats[560];
	struct nand_cache_read_stats *stats;
	struct nand_cache_read_configuration *configuration;
	unsigned char before_callbacks[48];
	int (*refresh)(struct nand_cache_read_zone *, int, void *);
	int (*read)(struct nand_cache_read_zone *, int, void *);
};
_Static_assert(__builtin_offsetof(struct nand_cache_read_zone, cache) == 1072,
	       "NAND read cache offset");
_Static_assert(__builtin_offsetof(struct nand_cache_read_zone, stats) == 3440,
	       "NAND read statistics offset");
_Static_assert(__builtin_offsetof(struct nand_cache_read_zone, read) == 3512,
	       "NAND read callback offset");
int nand_cache_read(struct nand_cache_read_zone *zone, int page,
		    unsigned short sector_range,
		    void *destination)
{
	struct cache_init_data *cache;
	struct cache_init_entry *node;
	unsigned char *buffer;
	unsigned int offset;
	unsigned int length;
	unsigned short written_start;
	unsigned short written_length;
	int result;

	if (!destination) {
		nand_print(nftl_ro_lc_419);
		return 0;
	}
	result = 0;
	offset = (unsigned short)(sector_range >> 8);
	length = sector_range & 0xff;
	offset <<= 9;
	length <<= 9;
	cache = &zone->cache;
	if (zone->configuration->count) {
		buffer = cache->entries[0].allocation;
		result = zone->read(zone, page, buffer);
		/* OEM copies even when the NAND read failed. */
		nand_memcpy(destination, buffer + offset, length);
		if (result == 10) {
			zone->stats->ecc_reads++;
			zone->refresh(zone, page, buffer);
			result = 0;
		}
		return result;
	}
	node = (struct cache_init_entry *)search_from_cache_write_list(
			(struct cache_write_zone *)cache, page);
	if (node) {
		buffer = node->allocation;
		written_start = (unsigned short)(node->field0 << 9);
		written_length = (unsigned short)(node->field1 << 9);
		/* OEM tests only the upper bound. A request below written_start
		 * can return unwritten cache bytes. */
		if (written_start + written_length >= offset &&
		    offset + length <= written_start + written_length) {
			nand_memcpy(destination, buffer + offset, length);
			return 0;
		}
		result = zone->read(zone, page, cache->page_buffer);
		/* The read failure is ignored, and page_bytes - written_length
		 * underflows if the cached span exceeds the page. */
		nand_memcpy(buffer + written_length,
			    (unsigned char *)cache->page_buffer + written_length,
			    zone->geometry->page_bytes - written_length);
		node->field1 = zone->geometry->sectors_per_page;
		nand_memcpy(destination, buffer + offset, length);
		return 0;
	}
	node = (struct cache_init_entry *)search_from_cache_read_list(
			(struct cache_read_zone *)cache, page);
	if (node) {
		if (node->status == 0) {
			del_from_cache_read_list(
				(struct cache_read_delete_owner *)cache,
				(struct cache_read_delete_node *)node);
			buffer = node->allocation;
			nand_memcpy(destination, buffer + offset, length);
			add_to_cache_read_list_tail(
				(struct cache_read_add_zone *)cache,
				(struct cache_read_delete_node *)node, 0);
		} else {
			buffer = node->allocation;
			nand_memcpy(destination, buffer + offset, length);
		}
		return 0;
	}
	node = (struct cache_init_entry *)get_empty_cahce_node(
			(struct cache_read_empty_zone *)cache);
	if (!node) {
		nand_print(nftl_ro_lc_420, cache->field1,
			   cache->field0, cache->field2);
		return 1;
	}
	node->page = page;
	buffer = node->allocation;
	result = zone->read(zone, page, buffer);
	node->field0 = 0;
	node->field1 = zone->geometry->sectors_per_page;
	add_to_cache_read_list_tail((struct cache_read_add_zone *)cache,
				    (struct cache_read_delete_node *)node, 0);
	nand_memcpy(destination, buffer + offset, length);
	if (result == 10) {
		zone->stats->ecc_reads++;
		zone->refresh(zone, page, buffer);
		result = 0;
	}
	return result;
}

/* GCC's stack-protector pass reorders these two locals even though this
 * function has no canary. Keep the OEM's cache pointer at FP+0x20 and signed
 * index at FP+0x2c; nftl_free callbacks can observe the caller's stack. */
__attribute__((optimize("-fno-stack-protector")))
int nftl_cache_exit(void *zone)
{
	unsigned char *cache = (unsigned char *)zone + 1072;
	int index;

	for (index = 0; index < *(unsigned short *)cache; index++) {
		/* OEM calls nftl_free even for null entries and re-reads the
		 * count after each call. It never clears an allocation slot.
		 * This view starts at the cache header; its allocation at +24
		 * matches entries[0].allocation after the eight-byte header. */
		nftl_free(((struct nftl_cache_entry_view *)cache)[index].allocation);
	}
	nftl_free(((struct cache_init_data *)cache)->page_buffer);
	return 0;
}

struct cache_clean_geometry {
	unsigned char before_slots[22];
	unsigned short slots;
};
struct cache_clean_zone {
	unsigned char before_geometry[3448];
	struct cache_clean_geometry *geometry;
};
int nftl_cache_clean(struct cache_clean_zone *zone)
{
	struct cache_init_data *cache = (void *)zone + 1072;
	unsigned int index;

	cache->count = zone->geometry->slots;
	cache->field0 = 0;
	cache->field1 = 0;
	cache->field2 = 0;
	cache->entries[30].links[0] = 0;
	cache->entries[30].links[1] = 0;
	cache->entries[30].links[2] = 0;
	cache->entries[30].links[3] = 0;
	cache->entries[31].links[0] = 0;
	cache->entries[31].links[1] = 0;
	cache->entries[31].links[2] = 0;
	cache->entries[31].links[3] = 0;
	/* OEM trusts the count; values above 32 overrun the embedded entries. */
	for (index = 0; index < cache->count; index++) {
		cache->entries[index].index = index;
		cache->entries[index].status = 255;
		cache->entries[index].page = -1;
		cache->entries[index].links[0] = 0;
		cache->entries[index].links[1] = 0;
		cache->entries[index].links[2] = 0;
		/* links[3] is the eight-byte store at cache+index*56+56. */
		cache->entries[index].links[3] = 0;
		cache->entries[index].field0 = 0;
		cache->entries[index].field1 = 0;
	}
	return 0;
}

struct nftl_zone_storage_view {
	unsigned char before_allocation[32];
	void *allocation;
};

struct nftl_zone_release_view {
	unsigned char before_storage[8];
	struct nftl_zone_storage_view *storage;
};

void nftl_zone_release(struct nftl_zone_release_view *zone)
{
	/* OEM ignores every release return and dereferences storage unchecked. */
	nftl_cache_exit(zone);
	zone_param_exit((struct exit_zone *)zone);
	nftl_free(zone->storage->allocation);
	nftl_free(zone->storage);
	nftl_free(zone);
}

int nand_clean_zone_table(void *zone)
{
	void *current = zone;
	/* OEM attempts all stages even if the flush reports failure. */
	__nand_flush_write_cache(current, 1000);
	clean_block_list(current);
	nand_clean_zone_table2(current);
	nftl_cache_exit(current);
	return 0;
}

struct nftl_list_node {
	unsigned char before_next[16];
	struct nftl_list_node *next;
	void *current_block;
	unsigned char before_flush[56];
	int (*flush)(struct nftl_list_node *node, unsigned int force);
};

/* -fstack-protector reorders two pointer locals here without adding a
 * canary; this function-level setting restores the stock stack slots. */
__attribute__((optimize("-fno-stack-protector")))
struct nftl_list_node *get_last_nftl(struct nftl_list_node *first)
{
	struct nftl_list_node *current = first;
	struct nftl_list_node *last = first;
	while (current) {
		last = current;
		current = current->next;
	}
	return last;
}

/* Isolated Hynix LSB disable C candidates. */

extern unsigned char hynix20nm_lsb_mode_reg_num;
extern unsigned char hynix20nm_lsb_mode_reg_adr[];
extern unsigned char hynix20nm_lsb_mode_default_val[];
extern unsigned char hynix20nm_lsb_mode_val[];
extern unsigned char hynix20nm_read_retry_mode;
extern unsigned char hynix20nm_read_retry_cycle;
extern unsigned char hynix20nm_read_retry_reg_num;
struct hynix20nm_lsb_params {
	unsigned char before_retry_spec[52];
	unsigned int retry_spec;
};
struct hynix20nm_lsb_chip {
	unsigned char before_params[280];
	struct hynix20nm_lsb_params *params;
};
int hynix20nm_lsb_init(struct hynix20nm_lsb_chip *chip)
{
	/* OEM reloads the nested retry word for each byte and trusts its
	 * pointer unchecked. Other modes leave the LSB tables unchanged. */
	hynix20nm_read_retry_mode = chip->params->retry_spec >> 16;
	hynix20nm_read_retry_cycle = chip->params->retry_spec >> 8;
	hynix20nm_read_retry_reg_num = chip->params->retry_spec;
	if (hynix20nm_read_retry_mode == 2) {
		hynix20nm_lsb_mode_reg_num = 4;
		hynix20nm_lsb_mode_reg_adr[0] = 0xb0;
		hynix20nm_lsb_mode_reg_adr[1] = 0xb1;
		hynix20nm_lsb_mode_reg_adr[2] = 0xa0;
		hynix20nm_lsb_mode_reg_adr[3] = 0xa1;
		hynix20nm_lsb_mode_val[0] = 10;
		hynix20nm_lsb_mode_val[1] = 10;
		hynix20nm_lsb_mode_val[2] = 10;
		hynix20nm_lsb_mode_val[3] = 10;
	} else if (hynix20nm_read_retry_mode == 3) {
		hynix20nm_lsb_mode_reg_num = 4;
		hynix20nm_lsb_mode_reg_adr[0] = 0xa0;
		hynix20nm_lsb_mode_reg_adr[1] = 0xa1;
		hynix20nm_lsb_mode_reg_adr[2] = 0xa7;
		hynix20nm_lsb_mode_reg_adr[3] = 0xa8;
		hynix20nm_lsb_mode_val[0] = 10;
		hynix20nm_lsb_mode_val[1] = 10;
		hynix20nm_lsb_mode_val[2] = 10;
		hynix20nm_lsb_mode_val[3] = 10;
	}
	return 0;
}
extern int hynix20nm_vender_set_param(void *chip, unsigned char *values,
				     unsigned char *addresses,
				     unsigned int count);
int hynix20nm_lsb_enable(void *chip)
{
	struct {
		unsigned char values[4];
		unsigned int index;
	} locals;

	/* OEM ignores both vendor transfer results. The shared defaults are
	 * overwritten by the read before the LSB offsets are applied. */
	hynix20nm_vender_get_param(chip, hynix20nm_lsb_mode_default_val,
				   hynix20nm_lsb_mode_reg_adr,
				   hynix20nm_lsb_mode_reg_num);
	/* OEM trusts the mutable register count even when it exceeds the
	 * four-byte destination and source tables. */
	for (locals.index = 0; locals.index < hynix20nm_lsb_mode_reg_num;
	     locals.index++) {
		locals.values[locals.index] =
			hynix20nm_lsb_mode_default_val[locals.index];
		locals.values[locals.index] =
			locals.values[locals.index] +
			hynix20nm_lsb_mode_val[locals.index];
	}
	hynix20nm_vender_set_param(chip, locals.values,
				   hynix20nm_lsb_mode_reg_adr,
				   hynix20nm_lsb_mode_reg_num);
	return 0;
}
extern unsigned char hynix26nm_lsb_mode_reg_num;
extern unsigned char hynix26nm_lsb_mode_reg_adr[];
extern unsigned char hynix26nm_lsb_mode_default_val[];
extern unsigned char hynix26nm_lsb_mode_val[];
extern unsigned char hynix26nm_read_retry_mode;
extern unsigned char hynix26nm_read_retry_cycle;
extern unsigned char hynix26nm_read_retry_reg_num;
struct hynix26nm_lsb_params {
	unsigned char before_retry_spec[52];
	unsigned int retry_spec;
};
struct hynix26nm_lsb_chip {
	unsigned char before_params[280];
	struct hynix26nm_lsb_params *params;
};
int hynix26nm_lsb_init(struct hynix26nm_lsb_chip *chip)
{
	/* OEM separately reloads the packed retry word for all three fields
	 * and trusts the nested pointer without checking it. */
	hynix26nm_read_retry_mode = chip->params->retry_spec >> 16;
	hynix26nm_read_retry_cycle = chip->params->retry_spec >> 8;
	hynix26nm_read_retry_reg_num = chip->params->retry_spec;
	hynix26nm_lsb_mode_reg_num = 5;
	hynix26nm_lsb_mode_reg_adr[0] = 0xa4;
	hynix26nm_lsb_mode_reg_adr[1] = 0xa5;
	hynix26nm_lsb_mode_reg_adr[2] = 0xb0;
	hynix26nm_lsb_mode_reg_adr[3] = 0xb1;
	hynix26nm_lsb_mode_reg_adr[4] = 0xc9;
	hynix26nm_lsb_mode_val[0] = 37;
	hynix26nm_lsb_mode_val[1] = 37;
	hynix26nm_lsb_mode_val[2] = 37;
	hynix26nm_lsb_mode_val[3] = 37;
	hynix26nm_lsb_mode_val[4] = 1;
	return 0;
}
extern unsigned char hynix16nm_read_retry_mode;
extern unsigned char hynix16nm_read_retry_cycle;
extern unsigned char hynix16nm_read_retry_reg_num;
extern unsigned char hynix16nm_read_retry_reg_adr[];
struct hynix16nm_init_params {
	unsigned char before_retry_spec[52];
	unsigned int retry_spec;
};
struct hynix16nm_init_chip {
	unsigned char before_otp[56];
	unsigned char otp[128];
	unsigned int retry;
	unsigned char before_params[92];
	struct hynix16nm_init_params *params;
};
struct hynix16nm_readretry_zone;
extern int hynix16nm_init_readretry_param(struct hynix16nm_readretry_zone *);
int hynix16nm_readretry_init(struct hynix16nm_init_chip *chip)
{
	unsigned int index;

	/* OEM dereferences the nested configuration unchecked and reloads it
	 * separately for all three global fields. */
	hynix16nm_read_retry_mode = chip->params->retry_spec >> 16;
	hynix16nm_read_retry_cycle = chip->params->retry_spec >> 8;
	hynix16nm_read_retry_reg_num = chip->params->retry_spec;
	if (hynix16nm_read_retry_mode == 4) {
		hynix16nm_read_retry_reg_adr[0] = 56;
		hynix16nm_read_retry_reg_adr[1] = 57;
		hynix16nm_read_retry_reg_adr[2] = 58;
		hynix16nm_read_retry_reg_adr[3] = 59;
		chip->retry = 0;
	} else {
		nand_print(nftl_ro_lc_64,
			   hynix16nm_read_retry_mode);
		return -75;
	}
	for (index = 0; index <= 127; index++)
		chip->otp[index] = 0;
	/* OEM discards OTP setup failure and reports initialization success. */
	hynix16nm_init_readretry_param((struct hynix16nm_readretry_zone *)chip);
	return 0;
}
extern int hynix26nm_vender_set_param(void *chip, unsigned char *values,
				     unsigned char *addresses,
				     unsigned int count);
/* The stock body has no canary despite its unchecked local array index. */
__attribute__((optimize("-fno-stack-protector")))
int hynix26nm_lsb_enable(void *chip)
{
	struct {
		unsigned char values[12];
		unsigned int index;
	} locals;

	/* OEM ignores both vendor transfer results and trusts the mutable
	 * count, which can index beyond the five-byte global tables. The
	 * stack index follows the twelve-byte local buffer in the stock body. */
	hynix26nm_vender_get_param(chip, hynix26nm_lsb_mode_default_val,
				   hynix26nm_lsb_mode_reg_adr,
				   hynix26nm_lsb_mode_reg_num);
	for (locals.index = 0; locals.index < hynix26nm_lsb_mode_reg_num;
	     locals.index++) {
		locals.values[locals.index] =
			hynix26nm_lsb_mode_default_val[locals.index];
		locals.values[locals.index] =
			locals.values[locals.index] +
			hynix26nm_lsb_mode_val[locals.index];
	}
	hynix26nm_vender_set_param(chip, locals.values,
				   hynix26nm_lsb_mode_reg_adr,
				   hynix26nm_lsb_mode_reg_num);
	return 0;
}

int hynix20nm_lsb_disable(void *chip)
{
	/* OEM reports success even if programming the defaults fails. */
	hynix20nm_vender_set_param(chip, hynix20nm_lsb_mode_default_val,
				    hynix20nm_lsb_mode_reg_adr,
				    hynix20nm_lsb_mode_reg_num);
	return 0;
}

int hynix26nm_lsb_disable(void *chip)
{
	/* OEM reports success even if programming the defaults fails. */
	hynix26nm_vender_set_param(chip, hynix26nm_lsb_mode_default_val,
				    hynix26nm_lsb_mode_reg_adr,
				    hynix26nm_lsb_mode_reg_num);
	return 0;
}

/* Isolated NFTL list and diagnostic C candidates; type labels are inferred. */

struct diag_free_node {
	unsigned char before_links[0x20];
	struct diag_free_node *next;
	struct diag_free_node *previous;
};
struct diag_free_zone {
	unsigned char before_head[0x110];
	struct diag_free_node head;
};
struct diag_free_node *del_free_list(struct diag_free_zone *zone)
{
	struct diag_free_node *node = &zone->head;

	node = node->next;
	if (node) {
		/* The redundant stock store is intentional, even though it appears
		 * useless; removing it changes the shipped instruction body. */
		zone->head.next = node->next;
		zone->head.next = node->next;
		if (node->next)
			node->next->previous = &zone->head;
	}
	return node;
}

struct gc_node {
	unsigned char before_links[0x10];
	struct gc_node *next;
	struct gc_node *previous;
};
struct gc_zone {
	unsigned short count;
};
/* Stock uses UXTH for the count decrement; this C body uses AND #0xffff.
 * Both retain the same low halfword, including wraparound at zero. */
struct gc_node *del_from_gc_list(struct gc_zone *zone, struct gc_node *node)
{
	struct gc_node *previous = node->previous;

	previous->next = node->next;
	if (node->next)
		node->next->previous = previous;
	node->next = 0;
	node->previous = 0;
	/* OEM permits 16-bit underflow when the count is already zero. */
	zone->count--;
	return node;
}

struct diag_current_zone {
	unsigned char before_flags[0x19];
	unsigned char flags;
	unsigned char before_current[0x210 - 0x1a];
	void *current;
	unsigned char before_other[0x250 - 0x218];
	void *other;
};
/* Three equality comparisons load the same operands into exchanged
 * registers; equality and every branch target remain unchanged. */
int is_current_blcok(struct diag_current_zone *zone, void *block)
{
	int result = 0;

	if (zone->flags & 1) {
		if (block == zone->other || block == zone->current)
			result = 1;
	} else if (block == zone->current) {
		result = 1;
	}
	return result;
}

extern int nand_clean_zone_table(void *zone);
extern int nand_find_zone_table(void *zone);
extern void *nand_memset(void *destination, int value, unsigned int size);
/* No canary is emitted here, but GCC's stack-protector pass still moves the
 * OEM's private buffer and sector slots. Keep their stock placement. */
__attribute__((optimize("-fno-stack-protector")))
int write_data_to_nand(void *zone)
{
	int sector;
	void *buffer = nftl_malloc(512);

	/* OEM logs allocation failure, then still dereferences the null
	 * buffer in nand_memset and continues. Keep this broken path. */
	if (!buffer)
		nand_print(nftl_ro_lc_392, nftl_ro_func_7642);
	nand_memset(buffer, -86, 512);
	/* OEM diagnostic overwrites all 38,912 sectors with 0xaa. Its fixed
	 * bound does not consult the zone capacity; never invoke casually. */
	for (sector = 0; sector <= 38911; sector++)
		__nand_write(zone, sector, 1, buffer);
	nftl_free(buffer);
	/* OEM ignores every write result, even after a failed allocation. */
	return 0;
}
static void _DumpHexData(const unsigned char *data, int length)
{
	const unsigned char *current = data;
	int index = 0;

	/* OEM inserts a newline after each sixteenth byte and another at the
	 * end, even when length is zero or the final row is exactly full. */
	while (index < length) {
		nand_print(nftl_ro_lc_394, *current++);
		if (index % 16 == 15)
			nand_print(nftl_ro_lc_45);
		index++;
	}
	nand_print(nftl_ro_lc_45);
}
void dump_buf(const unsigned char *data, unsigned int length)
{
	int index = 0;

	nand_print(nftl_ro_lc_45);
	index = 0;
	/* OEM emits both a line terminator at each 16-byte boundary and a
	 * final terminator, so a full final row gets two newlines. */
	while (index < length) {
		if ((index & 15) == 0)
			nand_print(nftl_ro_lc_200, index);
		nand_print(nftl_ro_lc_122, data[index]);
		if (((index + 1) & 15) == 0)
			nand_print(nftl_ro_lc_45);
		index++;
	}
	nand_print(nftl_ro_lc_45);
}
int nand_check_table(void *zone)
{
	void *block = zone;

	__nand_flush_write_cache(block, 1000);
	nand_print_dbg(nftl_ro_lc_393);
	print_nftl_zone(block);
	nand_print_dbg(nftl_ro_lc_393);
	nand_clean_zone_table(block);
	nand_print_dbg(nftl_ro_lc_393);
	nand_find_zone_table(block);
	write_data_to_nand(block);
	nand_print_dbg(nftl_ro_lc_393);
	/* OEM ignores every call's error and returns success. */
	return 0;
}

struct otp_chip {
	unsigned char before_saved[0x38];
	unsigned char saved[64];
};
void hynix16nm_save_otp_value(struct otp_chip *chip,
			      const unsigned char *value)
{
	unsigned int index;

	/* OEM copies a fixed span without validating source or destination. */
	for (index = 0; index < 32; index++)
		chip->saved[index] = value[index];
}

/* OEM copies all 64 bytes without checking either pointer or extent. */
void hynix20nm_saveotpvalue(struct otp_chip *chip,
			    const unsigned char *value)
{
	unsigned int index;

	for (index = 0; index < 64; index++)
		chip->saved[index] = value[index];
}

struct hynix16nm_readretry_params {
	unsigned char before_page_count[20];
	unsigned int page_count;
};
struct hynix16nm_readretry_zone {
	unsigned char before_chip[24];
	unsigned int chip;
	unsigned char before_controller[272 - 28];
	struct hynix16nm_otp_controller *controller;
	struct hynix16nm_readretry_params *params;
};
extern int hynix16nm_get_special_info(unsigned char *buffer,
				      void *destination, unsigned int bytes,
				      unsigned int index);
__attribute__((optimize("-fno-stack-protector")))
int hynix16nm_init_readretry_param(struct hynix16nm_readretry_zone *zone)
{
	/* The OEM reserves 64 bytes for the fallback OTP table even though its
	 * helper currently copies only 32. Retain the observed unused slots and
	 * zero-store order; callbacks can inspect the caller's private frame. */
	struct {
		unsigned int unused_status[10];
		unsigned char otp_values[64];
		unsigned char *copy_buffer;
		unsigned int unused_before_result;
		int result;
		unsigned char *page_buffer;
		unsigned int unused_before_index;
		unsigned int index;
	} frame;

	frame.index = 0;
	frame.page_buffer = 0;
	frame.result = 0;
	frame.copy_buffer = 0;
	__builtin_memset(frame.unused_status, 0, sizeof(frame.unused_status));

	/* OEM multiplies the untrusted page count in 32 bits, with no overflow
	 * check, and returns silently if the first allocation fails. */
	frame.page_buffer = nand_get_temp_buf(zone->params->page_count << 9);
	if (!frame.page_buffer)
		return -77;
	frame.copy_buffer = nand_get_temp_buf(32768);
	if (!frame.copy_buffer) {
		nand_free_temp_buf(frame.page_buffer);
		nand_print(nftl_ro_lc_59);
		return -76;
	}
	frame.result = 0;
	frame.result = hynix16nm_get_special_info(frame.copy_buffer,
						frame.page_buffer, 32, zone->chip);
	if (!frame.result) {
		/* Stock prints the zero-initialized scratch word as the chip
		 * number here, rather than the zone's chip field. */
		nand_print_dbg(nftl_ro_lc_128,
			       zone->controller->channel, frame.unused_status[0]);
		for (frame.index = 0; frame.index <= 31; frame.index++) {
			nand_print_dbg(nftl_ro_lc_61, frame.page_buffer[frame.index]);
			if ((frame.index & 7) == 7)
				nand_print_dbg(nftl_ro_lc_45);
		}
		hynix16nm_save_otp_value((struct otp_chip *)zone,
					 frame.page_buffer);
		hynix16nm_set_default_param(zone);
	} else {
		nand_print_dbg(nftl_ro_lc_62,
			       zone->controller->channel);
		/* OEM ignores both fallback and default-program results. */
		hynix16nm_get_otp_param((struct hynix16nm_otp_zone *)zone,
					frame.otp_values);
		hynix16nm_set_default_param(zone);
		nand_print_dbg(nftl_ro_lc_129,
			       zone->controller->channel, frame.unused_status[0]);
	}
	nand_free_temp_buf(frame.copy_buffer);
	nand_free_temp_buf(frame.page_buffer);
	return 0;
}

struct hynix20nm_readretry_zone {
	unsigned char before_chip[24];
	unsigned int chip;
	unsigned char before_controller[272 - 28];
	struct hynix16nm_otp_controller *controller;
	struct hynix16nm_readretry_params *params;
};
extern int hynix20nm_getotpparam(struct otp_chip *, unsigned char *);
__attribute__((optimize("-fno-stack-protector")))
int hynix20nm_init_readretry_param(struct hynix20nm_readretry_zone *zone)
{
	/* OEM keeps the special-info result apart from its final return value.
	 * The four-byte gap after the OTP table is present in the stock frame. */
	struct {
		unsigned char otp_values[64];
		unsigned int unused_after_otp;
		int special_result;
		unsigned char *copy_buffer;
		unsigned char *page_buffer;
		int result;
		unsigned int index;
	} frame;

	frame.index = 0;
	frame.page_buffer = 0;
	frame.copy_buffer = 0;
	frame.result = 0;
	frame.special_result = 0;

	/* Stock shifts the count in 32 bits and never checks whether the
	 * resulting allocation can hold the 64-byte table. */
	frame.page_buffer = nand_get_temp_buf(zone->params->page_count << 9);
	if (!frame.page_buffer) {
		nand_print_dbg(nftl_ro_lc_58);
		return -94;
	}
	frame.copy_buffer = nand_get_temp_buf(32768);
	if (!frame.copy_buffer) {
		nand_free_temp_buf(frame.page_buffer);
		nand_print(nftl_ro_lc_59);
		return -76;
	}
	frame.special_result = 0;
	frame.special_result = hynix16nm_get_special_info(frame.copy_buffer,
						frame.page_buffer, 64, zone->chip);
	if (frame.special_result == 0) {
		nand_print_dbg(nftl_ro_lc_60,
			       zone->controller->channel, zone->chip);
		for (frame.index = 0; frame.index <= 63; frame.index++) {
			nand_print_dbg(nftl_ro_lc_61, frame.page_buffer[frame.index]);
			if ((frame.index & 7) == 7)
				nand_print_dbg(nftl_ro_lc_45);
		}
		hynix20nm_saveotpvalue((struct otp_chip *)zone,
					 frame.page_buffer);
		frame.result = hynix20nm_set_default_param((struct otp_chip *)zone);
	} else {
		nand_print_dbg(nftl_ro_lc_62,
			       zone->controller->channel);
		frame.result = hynix20nm_getotpparam((struct otp_chip *)zone,
						frame.otp_values);
		if (frame.result == 0) {
			/* OEM ignores the default-program result on this fallback. */
			hynix20nm_set_default_param((struct otp_chip *)zone);
			nand_print_dbg(nftl_ro_lc_63,
				       zone->controller->channel);
		}
	}
	/* OEM frees only the page buffer here. The 32 KiB copy buffer leaks
	 * on every normal completion; retain that behavior for equivalence. */
	nand_free_temp_buf(frame.page_buffer);
	return frame.result;
}

extern int hynix20nm_vender_get_param_otp_hynix(struct otp_chip *chip);
int hynix20nm_getotpparam(struct otp_chip *chip, unsigned char *value)
{
	unsigned int attempt = 0;
	unsigned int column;
	unsigned int row;
	int result;

	/* The OEM makes five attempts and reports the last vendor error.
	 * Its debug message prints the zero-based attempt number. Stock tests
	 * the loop bound before entering the first attempt. */
	while (attempt <= 4) {
		nand_print_dbg(nftl_ro_lc_54,
			       attempt);
		result = hynix20nm_vender_get_param_otp_hynix(chip);
		if (result == 0)
			break;
		attempt++;
	}
	if (result != 0) {
		nand_print(nftl_ro_lc_55);
		return result;
	}
	for (row = 0; row <= 7; row++)
		for (column = 0; column <= 7; column++)
			value[(row << 3) + column] =
				chip->saved[(row << 3) + column];
	return 0;
}

struct command_chip;
extern int set_one_addr(struct command_chip *, unsigned int);
__attribute__((optimize("-fno-stack-protector")))
int hynix20nm_vender_get_param_otp_hynix(struct otp_chip *owner)
{
	unsigned char *buffer;
	/* These eight-byte slots reproduce the OEM stack layout. Only the
	 * command byte and the first two address/value bytes are initialized. */
	unsigned char command[8];
	unsigned char complement[64];
	unsigned char addresses[8];
	unsigned char values[8];
	unsigned int index;
	unsigned int copy;
	int invalid_copy;
	int result;

	result = 0;
	addresses[0] = 0;
	addresses[1] = 0;
	values[0] = 0;
	values[1] = 0;

	if (hynix20nm_read_retry_mode == 2) {
		addresses[0] = 0xff;
		addresses[1] = 0xcc;
		values[0] = 0x40;
		values[1] = 0x4d;
	} else if (hynix20nm_read_retry_mode == 3) {
		addresses[0] = 0xae;
		addresses[1] = 0xb0;
		values[0] = 0;
		values[1] = 0x4d;
	} else {
		return -97;
	}
	/* OEM never checks allocation or any NAND command/read result. */
	buffer = nand_get_temp_buf(1024);
	nand_enable_chip((struct samsung_set_chip *)owner);
	ndfc_disable_randomize(((struct samsung_set_chip *)owner)->controller);
	set_one_cmd((struct command_chip *)owner, -1, 1);
	command[0] = 54;
	set_cmd_with_nand_bus((struct samsung_set_chip *)owner, command, 0,
				  &addresses[0], &values[0], 1, 1);
	set_cmd_with_nand_bus((struct samsung_set_chip *)owner, 0, 0,
				  &addresses[1], &values[1], 1, 1);
	set_one_cmd((struct command_chip *)owner, 22, 0);
	set_one_cmd((struct command_chip *)owner, 23, 0);
	set_one_cmd((struct command_chip *)owner, 4, 0);
	set_one_cmd((struct command_chip *)owner, 25, 0);
	set_one_cmd((struct command_chip *)owner, 0, 0);
	set_one_addr((struct command_chip *)owner, 0);
	set_one_addr((struct command_chip *)owner, 0);
	set_one_addr((struct command_chip *)owner, 0);
	set_one_addr((struct command_chip *)owner, 2);
	set_one_addr((struct command_chip *)owner, 0);
	/* Stock reuses the setup-command byte for the read command. */
	command[0] = 48;
	get_data_with_nand_bus_one_cmd((struct samsung_set_chip *)owner, command, 0, buffer, 2);
	if (buffer[0] != 8 || buffer[1] != 8) {
		nand_print(nftl_ro_lc_51,
			   buffer[0], buffer[1]);
		result = -96;
	}
	get_data_with_nand_bus_one_cmd((struct samsung_set_chip *)owner, 0, 0, buffer, 1024);
	/* OEM overwrites saved bytes before validating each copy. A bad copy
	 * therefore leaves a partial update until a later good copy replaces it. */
	for (copy = 0; copy <= 7; copy++) {
		invalid_copy = 0;
		for (index = 0; index <= 63; index++) {
			/* OEM widens the 32-bit copy stride before adding index. */
			owner->saved[index] =
				buffer[(unsigned long)(copy * 128) + index];
			complement[index] =
				buffer[(unsigned long)(copy * 128) + 64 + index];
			if (owner->saved[index] + complement[index] != 255) {
				invalid_copy = 1;
				break;
			}
		}
		if (!invalid_copy) {
			nand_print_dbg(nftl_ro_lc_52, copy);
			break;
		}
	}
	if (invalid_copy) {
		nand_print_dbg(nftl_ro_lc_53);
		/* OEM overwrites a previous -96 register-count error here. */
		result = -95;
	}
	set_one_cmd((struct command_chip *)owner, -1, 1);
	set_one_cmd((struct command_chip *)owner, 56, 1);
	nand_disable_chip((struct samsung_set_chip *)owner);
	nand_free_temp_buf(buffer);
	return result;
}

extern unsigned char hynix20nm_read_retry_reg_num;
extern unsigned char hynix20nm_read_retry_reg_adr[];
extern unsigned char hynix20nm_read_retry_cycle;
struct hynix20nm_retry_chip {
	unsigned char before_saved[56];
	unsigned char saved[128];
	unsigned int cycle;
	unsigned char before_params[280 - 188];
	struct hynix20nm_lsb_params *params;
};
/* The mutable OEM register count is unchecked against this eight-byte stack
 * buffer. Keep the overflow and frame layout for stock compatibility. */
__attribute__((optimize("-fno-stack-protector")))
int hynix20nm_set_readretry(struct hynix20nm_retry_chip *chip)
{
	struct {
		unsigned char values[8];
		int result;
		unsigned int index;
	} locals;

	locals.result = 0;
	if (chip->cycle > hynix20nm_read_retry_cycle)
		return -92;
	for (locals.index = 0; locals.index < hynix20nm_read_retry_reg_num;
	     locals.index++)
		locals.values[locals.index] =
			chip->saved[(chip->cycle << 3) + locals.index];
	locals.result = hynix20nm_vender_set_param(
		chip, locals.values, hynix20nm_read_retry_reg_adr,
		hynix20nm_read_retry_reg_num);
	nand_print_dbg(nftl_ro_lc_43);
	for (locals.index = 0; locals.index < hynix20nm_read_retry_reg_num;
	     locals.index++)
		nand_print_dbg(nftl_ro_lc_44, locals.values[locals.index]);
	nand_print_dbg(nftl_ro_lc_45);
	return locals.result;
}
/* Stock omits a canary although its unchecked count can overrun the buffer. */
__attribute__((optimize("-fno-stack-protector")))
int hynix20nm_set_default_param(struct otp_chip *chip)
{
	struct {
		int result;
		unsigned int index;
	} locals;
	unsigned char values[8];

	locals.result = 0;
	/* The OEM trusts the mutable register count. Past eight bytes it
	 * overwrites the result, then the loop index, in its own stack frame.
	 * Keep the OEM's count-first comparison in both loops. */
	for (locals.index = 0; hynix20nm_read_retry_reg_num > locals.index;
	     locals.index++)
		values[locals.index] = chip->saved[locals.index];
	locals.result = hynix20nm_vender_set_param(chip, values,
					hynix20nm_read_retry_reg_adr,
					hynix20nm_read_retry_reg_num);
	nand_print_dbg(nftl_ro_lc_56);
	for (locals.index = 0; hynix20nm_read_retry_reg_num > locals.index;
	     locals.index++)
		nand_print_dbg(nftl_ro_lc_57, values[locals.index]);
	nand_print_dbg(nftl_ro_lc_45);
	return locals.result;
}

int hynix20nm_readretry_init(struct hynix20nm_retry_chip *chip)
{
	/* Keep the stock private return slot ahead of the signed loop index. */
	struct {
		int result;
		int index;
	} frame;

	hynix20nm_read_retry_mode = chip->params->retry_spec >> 16;
	hynix20nm_read_retry_cycle = chip->params->retry_spec >> 8;
	hynix20nm_read_retry_reg_num = chip->params->retry_spec;
	if (hynix20nm_read_retry_mode == 2) {
		hynix20nm_read_retry_reg_adr[0] = 0xcc;
		hynix20nm_read_retry_reg_adr[1] = 0xbf;
		hynix20nm_read_retry_reg_adr[2] = 0xaa;
		hynix20nm_read_retry_reg_adr[3] = 0xab;
		hynix20nm_read_retry_reg_adr[4] = 0xcd;
		hynix20nm_read_retry_reg_adr[5] = 0xad;
		hynix20nm_read_retry_reg_adr[6] = 0xae;
		hynix20nm_read_retry_reg_adr[7] = 0xaf;
	} else if (hynix20nm_read_retry_mode == 3) {
		hynix20nm_read_retry_reg_adr[0] = 0xb0;
		hynix20nm_read_retry_reg_adr[1] = 0xb1;
		hynix20nm_read_retry_reg_adr[2] = 0xb2;
		hynix20nm_read_retry_reg_adr[3] = 0xb3;
		hynix20nm_read_retry_reg_adr[4] = 0xb4;
		hynix20nm_read_retry_reg_adr[5] = 0xb5;
		hynix20nm_read_retry_reg_adr[6] = 0xb6;
		hynix20nm_read_retry_reg_adr[7] = 0xb7;
	} else {
		/* Stock already changed all three global retry selectors before
		 * rejecting this mode, leaving the address table untouched. */
		nand_print(nftl_ro_lc_64,
			   hynix20nm_read_retry_mode);
		return -93;
	}
	chip->cycle = 0;
	for (frame.index = 0; frame.index <= 127; frame.index++)
		chip->saved[frame.index] = 0;
	/* OEM permits a register count larger than its eight address bytes. */
	frame.result = hynix20nm_init_readretry_param(
			(struct hynix20nm_readretry_zone *)chip);
	return frame.result;
}

extern unsigned char hynix26nm_read_retry_cycle;
extern unsigned char hynix26nm_read_retry_reg_num;
extern unsigned char hynix26nm_read_retry_reg_adr[];
extern signed short hynix26nm_read_retry_val[];
__attribute__((optimize("-fno-stack-protector")))
int hynix26nm_set_readretry(struct hynix20nm_retry_chip *chip)
{
	struct {
		unsigned char values[8];
		unsigned short padding;
		unsigned short base;
		int result;
		unsigned short padding2;
		unsigned short adjusted;
		unsigned int index;
	} locals;

	locals.result = 0;
	if (chip->cycle > hynix26nm_read_retry_cycle)
		return -53;
	/* OEM trusts the mutable register count and does not guard this
	 * eight-byte stack buffer against overflow. */
	for (locals.index = 0; locals.index < hynix26nm_read_retry_reg_num;
	     locals.index++) {
		locals.base = chip->saved[locals.index];
		locals.adjusted = (unsigned short)(
			hynix26nm_read_retry_val[chip->cycle * 4 + locals.index]
			+ locals.base);
		/* Stock truncates to 16 bits before interpreting the sum as
		 * signed and clamping it to the byte range. */
		if ((short)locals.adjusted > 255)
			locals.adjusted = 255;
		else if ((short)locals.adjusted < 0)
			locals.adjusted = 0;
		else
			locals.adjusted &= 255;
		locals.values[locals.index] = locals.adjusted;
	}
	if (chip->cycle >= 2 && chip->cycle <= 6)
		locals.values[0] = 0;
	if (chip->cycle == 5 || chip->cycle == 6)
		locals.values[1] = 0;
	locals.result = hynix26nm_vender_set_param(
		chip, locals.values, hynix26nm_read_retry_reg_adr,
		hynix26nm_read_retry_reg_num);
	nand_print_dbg(nftl_ro_lc_85);
	for (locals.index = 0; locals.index < hynix26nm_read_retry_reg_num;
	     locals.index++)
		nand_print_dbg(nftl_ro_lc_44, locals.values[locals.index]);
	nand_print_dbg(nftl_ro_lc_45);
	return locals.result;
}

extern unsigned char hynix26nm_read_retry_reg_num;
extern unsigned char hynix26nm_read_retry_reg_adr[];
/* Stock uses a signed index and omits a canary around its unchecked buffer. */
__attribute__((optimize("-fno-stack-protector")))
int hynix26nm_setdefaultparam(void *chip)
{
	struct {
		unsigned char values[8];
		int result;
		int index;
	} locals;

	locals.result = 0;
	/* OEM's mutable count can overwrite the result and then the index. */
	for (locals.index = 0; locals.index < hynix26nm_read_retry_reg_num;
	     locals.index++)
		locals.values[locals.index] =
			((struct otp_chip *)chip)->saved[locals.index];
	locals.result = hynix26nm_vender_set_param(chip, locals.values,
					   hynix26nm_read_retry_reg_adr,
					   hynix26nm_read_retry_reg_num);
	nand_print_dbg(nftl_ro_lc_56);
	for (locals.index = 0; locals.index < hynix26nm_read_retry_reg_num;
	     locals.index++)
		nand_print_dbg(nftl_ro_lc_57, locals.values[locals.index]);
	nand_print_dbg(nftl_ro_lc_45);
	return locals.result;
}

extern struct nftl_list_node nftl_blk_head;
/* Stock uses a different local-slot order under the kernel's
 * -fstack-protector flag, without emitting a canary. */
__attribute__((optimize("-fno-stack-protector")))
unsigned int get_nftl_num(void)
{
	unsigned int count = 0;
	struct nftl_list_node *node = &nftl_blk_head;
	while (node->next) {
		++count;
		node = node->next;
	}
	return count;
}

/* Isolated C candidates for the OEM NFTL capacity and diagnostic list walks. */

struct nftl_cap_node {
	int capacity;
	unsigned char before_next[12];
	struct nftl_cap_node *next;
};

/* Restore the stock integer-local slot order; the list walk still has
 * no cycle check and its signed sum may overflow. */
__attribute__((optimize("-fno-stack-protector")))
int get_nftl_cap(void)
{
	int capacity;
	struct nftl_cap_node *node;
	capacity = 0;
	node = (struct nftl_cap_node *)&nftl_blk_head;
	/* OEM has no cycle check and sums in signed 32-bit arithmetic. */
	while (node->next) {
		capacity += node->next->capacity;
		node = node->next;
	}
	return capacity;
}

void print_all_nftl_blk(void)
{
	struct nftl_cap_node *node = (struct nftl_cap_node *)&nftl_blk_head;
	/* OEM follows the list without checking cycles or pointer validity. */
	while (node->next) {
		nand_print_dbg(nftl_ro_lc_7, node->next->capacity);
		node = node->next;
	}
}

struct nftl_first_head_view {
	unsigned char preceding[16];
	unsigned int *capacity;
};

unsigned int get_first_nftl_cap(void)
{
	struct nftl_first_head_view *head = (struct nftl_first_head_view *)&nftl_blk_head;
	/* OEM assumes the first-block pointer is non-null. */
	return *head->capacity;
}

extern struct nftl_list_node *del_last_nftl_blk(struct nftl_list_node *head);

int nand_ftl_exit(void)
{
	struct nftl_list_node *node = del_last_nftl_blk(&nftl_blk_head);
	while (node) {
		/* OEM ignores release failures and continues removing nodes. */
		nftl_exit((struct nftl_exit_view *)node);
		nftl_free(node);
		node = del_last_nftl_blk(&nftl_blk_head);
	}
	return 0;
}

int nftl_flush_write_cache(void)
{
	struct nftl_list_node *node = nftl_blk_head.next;
	while (node) {
		/* OEM ignores the callback result and does not check NULL flush. */
		node->flush(node, 65535);
		node = node->next;
	}
	return 0;
}

int nftl_write_end(void)
{
	struct nftl_list_node *node = nftl_blk_head.next;
	while (node) {
		/* OEM reports success even if page filling fails. */
		fill_no_use_page_to_current_block(node->current_block, 8);
		node = node->next;
	}
	return 0;
}
struct nftl_discard_node {
	unsigned int capacity;
	unsigned char before_next[12];
	struct nftl_discard_node *next;
	unsigned char before_read[48];
	int (*read)(struct nftl_discard_node *node, unsigned int sector,
		    unsigned int count, void *data);
	int (*write)(struct nftl_discard_node *node, unsigned int sector,
		     unsigned int count, void *data);
	unsigned char before_discard[8];
	int (*discard)(struct nftl_discard_node *node, unsigned int sector,
		       unsigned int count);
};
int nftl_discard(unsigned int sector, unsigned int count)
{
	struct nftl_discard_node *node =
		(struct nftl_discard_node *)nftl_blk_head.next;

	if (count == 0)
		return 0;
	/* OEM does not validate the first node, detect cycles, or split a
	 * request crossing the selected node's capacity. */
	while (node->capacity <= sector) {
		sector -= node->capacity;
		node = node->next;
		if (!node) {
			nand_print(nftl_ro_lc_8, sector, count);
			return 1;
		}
	}
	return node->discard(node, sector, count);
}

/* OEM treats a zero-length request as success, walks unchecked links, and
 * forwards the whole count to one callback without spanning partitions. */
#define NFTL_RW(name, member) \
int name(unsigned int sector, unsigned int count, void *data) \
{ \
	struct nftl_discard_node *node = \
		(struct nftl_discard_node *)nftl_blk_head.next; \
	if (count == 0) \
		return 0; \
	while (node->capacity <= sector) { \
		sector -= node->capacity; \
		node = node->next; \
		if (!node) { \
			nand_print(nftl_ro_lc_8, sector, count); \
			return 1; \
		} \
	} \
	return node->member(node, sector, count, data); \
}

NFTL_RW(nftl_read, read)
NFTL_RW(nftl_write, write)

/* Inferred NFTL orchestration body; callback signatures remain provisional. */

extern void clean_block_list(void *);
extern int nand_clean_zone_table2(struct nftl_clean_zone *);
struct build_zone;
extern int build_zone_list(struct build_zone *);
struct invalid_print_zone;
extern void print_block_invalid_list(struct invalid_print_zone *);
struct free_print_zone;
extern void print_free_list(struct free_print_zone *);

int nand_find_zone_table(void *zone)
{
	void *current = zone;

	/* OEM ignores each stage's result, including the zone build and cache
	 * initialization; it always reports success. Retain that behavior. */
	clean_block_list(current);
	nand_clean_zone_table2(current);
	nand_print_dbg(nftl_ro_lc_388);
	build_zone_list(current);
	nand_print_dbg(nftl_ro_lc_389);
	nftl_cache_init(current);
	nand_print_dbg(nftl_ro_lc_390);
	print_block_invalid_list(current);
	print_free_list(current);
	nand_print_dbg(nftl_ro_lc_390);
	nand_print_dbg(nftl_ro_lc_391);
	return 0;
}

struct build_zone {
	unsigned char before_count[26];
	unsigned short block_count;
};
struct scan_page_geometry {
	unsigned char before_factor[8];
	unsigned char page_factor;
	unsigned char before_pages[11];
	unsigned short pages_per_block;
};
struct scan_reserve_geometry {
	unsigned char before_min_free[12];
	unsigned short min_free_blocks;
	unsigned short min_gc_blocks;
	unsigned char before_reserve_ratio[2];
	unsigned short reserve_numerator;
	unsigned short reserve_denominator;
};
struct scan_zone {
	unsigned char before_geometry[8];
	struct scan_page_geometry *geometry;
	unsigned char before_logical_pages[16];
	unsigned int logical_pages;
	unsigned char before_free_capacity[4];
	unsigned int free_capacity;
	unsigned char before_gc_reserve[636];
	unsigned short gc_reserve;
	unsigned short write_reserve;
	unsigned short initialized;
	unsigned short status;
	unsigned char before_reserve_geometry[2760];
	struct scan_reserve_geometry *reserve_geometry;
};
int init_zone_after_first_scan(struct scan_zone *zone,
			       unsigned int available_blocks)
{
	unsigned int needed_blocks;

	/* Stock performs both unsigned divisions without checking for zero.
	 * AArch64 UDIV yields zero for a zero divisor in the shipped binary;
	 * the corresponding C expression is undefined if geometry is corrupt.
	 * Keep the unchecked OEM path for stock-compatible code generation. */
	needed_blocks = zone->logical_pages / zone->geometry->page_factor;
	needed_blocks /= zone->geometry->pages_per_block;
	nand_print_dbg(nftl_ro_lc_230,
		       available_blocks, needed_blocks);
	if (available_blocks <= needed_blocks) {
		nand_print(nftl_ro_lc_231,
			   available_blocks, needed_blocks);
		return 1;
	}
	/* OEM recomputes this difference at each use, including after writing
	 * free_capacity; retain that evaluation and instruction order. */
	zone->free_capacity = (available_blocks - needed_blocks) *
		zone->geometry->page_factor *
		zone->geometry->pages_per_block;
	/* The OEM subtracts in 32 bits, so a zero minimum wraps to UINT_MAX. */
	if (available_blocks - needed_blocks <
	    (unsigned int)zone->reserve_geometry->min_free_blocks - 1) {
		nand_print(nftl_ro_lc_232,
			   available_blocks, needed_blocks);
		return 1;
	}
	zone->gc_reserve = (available_blocks - needed_blocks) / 5;
	if (zone->gc_reserve < zone->reserve_geometry->min_gc_blocks)
		zone->gc_reserve = zone->reserve_geometry->min_gc_blocks;
	/* Preserve 32-bit product wrap, unchecked divisor and halfword truncation.
	 * Stock UDIV returns zero when reserve_denominator is zero; C does not
	 * define that case, despite the current compiled body using UDIV. */
	zone->write_reserve = ((available_blocks - needed_blocks) *
		zone->reserve_geometry->reserve_numerator) /
		zone->reserve_geometry->reserve_denominator;
	if (zone->write_reserve < zone->gc_reserve + 2)
		zone->write_reserve = zone->gc_reserve + 2;
	zone->initialized = 1;
	zone->status = 0;
	return 0;
}
extern int first_scan_all_blocks(struct build_zone *);
extern int second_scan_all_blocks(struct build_zone *);
struct power_gc_zone;
extern int power_on_gc(struct power_gc_zone *);
extern int panic_backup_block(struct build_zone *);
int build_zone_list(struct build_zone *zone)
{
	int result;

	nand_print_dbg(nftl_ro_lc_211);
	if (first_scan_all_blocks(zone))
		return 1;
	result = init_zone_after_first_scan((struct scan_zone *)zone,
					    zone->block_count);
	if (result) {
		nand_print(nftl_ro_lc_212);
		return 1;
	}
	result = second_scan_all_blocks(zone);
	if (result) {
		nand_print(nftl_ro_lc_213);
		return 1;
	}
	/* OEM still reports success if GC or panic backup fails. */
	result = power_on_gc((struct power_gc_zone *)zone);
	print_nftl_zone(zone);
	nand_print(nftl_ro_lc_214);
	panic_backup_block(zone);
	return 0;
}

struct first_scan_block {
	unsigned short number;
	unsigned short state;
	unsigned short invalid_pages;
	unsigned short erase_count;
	unsigned int used_count;
	unsigned char before_links[36];
	void *next;
	void *previous;
};
struct first_scan_geometry {
	unsigned char before_pages[20];
	unsigned short pages;
	unsigned char before_blocks[2];
	unsigned short blocks;
};
struct first_scan_gc {
	unsigned char before_claim[4];
	unsigned short claim;
};
struct first_scan_zone {
	unsigned char before_geometry[8];
	struct first_scan_geometry *geometry;
	unsigned char before_partition_id[8];
	unsigned char partition_id;
	unsigned char before_free_count[1];
	unsigned short free_count;
	unsigned short bad_count;
	unsigned char before_first_block[18];
	struct first_scan_block *first_block;
	struct first_scan_block *last_read;
	unsigned char before_data[448];
	void *data;
	unsigned char before_gc[2928];
	struct first_scan_gc *gc;
	unsigned char before_read[8];
	int (*read)(struct first_scan_zone *, struct physic_op_param *);
	unsigned char before_good[16];
	int (*is_good)(struct first_scan_zone *, struct physic_op_param *);
};
_Static_assert(sizeof(struct first_scan_block) == 64,
	       "first-scan block stride");
_Static_assert(__builtin_offsetof(struct first_scan_zone, first_block) == 48,
	       "first-scan block offset");
_Static_assert(__builtin_offsetof(struct first_scan_zone, gc) == 3448,
	       "first-scan GC offset");
_Static_assert(__builtin_offsetof(struct first_scan_zone, is_good) == 3488,
	       "first-scan good callback offset");
struct move_zone;
struct move_node;
struct erase_block_zone;
struct erase_block_info;
struct torture_zone;
struct prio_gc_owner;
struct panic_header;
struct panic_block;
extern char mod[];
extern char *nand_fdt_get_prop_string_by_path(void *, const char *,
						const char *);
extern int nand_memcmp(const void *, const void *, unsigned long);
extern int panic_recover_info(struct panic_header *, struct panic_block *);
extern int add_block_count_list(struct block_count_zone *,
				 struct block_count_node *);
extern int put_phy_block_to_free_list(struct move_zone *, struct move_node *);
extern int erase_block(struct erase_block_zone *, struct erase_block_info *,
		       unsigned short);
extern int torture_block_and_mark(struct torture_zone *,
				   struct erase_block_info *);
extern int add_prio_gc(struct prio_gc_owner *, void *, unsigned short);

extern const char nftl_vendor_sunxi[5];
extern const char nftl_vendor_allwinner[9];

__attribute__((optimize("-fno-stack-protector")))
int first_scan_all_blocks(struct build_zone *raw_zone)
{
	struct first_scan_zone *zone = (struct first_scan_zone *)raw_zone;
	struct first_scan_block *block = zone->first_block;
	struct physic_op_param op;
	unsigned char oob[16];
	const char path[2] = {'/', 0};
	char vendor[5];
	char *compatible;
	unsigned int last_erase = 0, used;
	int status;

	__builtin_memcpy(vendor, nftl_vendor_sunxi, sizeof(vendor));
	compatible = nand_fdt_get_prop_string_by_path(0, path, mod);
	if (!compatible || nand_memcmp(compatible, vendor, 3))
		return -1;
	nand_print_dbg(nftl_ro_lc_215);
	zone->free_count = 0;
	while (block->number < zone->geometry->blocks) {
		set_physic_op_par(&op, block->number, 0, 0, 0, oob);
		if (zone->is_good(zone, &op) != 1) {
			nand_print(nftl_ro_lc_229, block->number);
			block->state = 255;
			block->erase_count = 0;
			zone->bad_count++;
			goto next;
		}
		status = zone->read(zone, &op);
		zone->last_read = block;
		used = get_block_used_count_from_oob(oob);
		if (is_ftl_end_flag_page(oob) == 1) {
			nand_print(nftl_ro_lc_216, block->number);
			return 0;
		}
		if (is_panic_data_page(oob) == 1) {
			if (oob[3] == zone->partition_id)
				panic_recover_info((struct panic_header *)oob,
						   (struct panic_block *)block);
			goto next;
		}
		if (is_nouse_page(oob) == 1) {
			zone->free_count++;
			block->state = 0;
			block->erase_count = 0;
			block->invalid_pages = 0;
			put_phy_block_to_free_list((struct move_zone *)zone,
						    (struct move_node *)block);
			goto next;
		}
		if ((is_ftl_logic_page_data(oob) == 1 ||
		     is_function_info_page(oob) == 1) &&
		    (status == 0 || status == 10))
			goto counted;
		if (is_ftl_start_flag_page(oob) == 1) {
			nand_print(nftl_ro_lc_218);
			goto next;
		}
		if (is_fill_page(oob) == 1) {
			zone->free_count++;
			block->state = 0;
			block->erase_count = get_erase_count_from_oob(oob);
			last_erase = block->erase_count;
			block->invalid_pages = zone->geometry->pages;
			put_phy_block_to_free_list((struct move_zone *)zone,
						    (struct move_node *)block);
			nand_print_dbg(nftl_ro_lc_219, block->number);
			goto next;
		}
		/* OEM prints all 16 spare bytes, then increments the last valid
		 * erase count even when this block has no trustworthy OOB. */
		nand_print(nftl_ro_lc_220,
			   block->number, oob[0], oob[1], oob[2], oob[3],
			   oob[4], oob[5], oob[6], oob[7], oob[8], oob[9],
			   oob[10], oob[11], oob[12], oob[13], oob[14], oob[15]);
		block->erase_count = (unsigned short)(last_erase + 1);
		if (status == 0 || status == 10) {
			if (oob[0] != 255) {
				nand_print(nftl_ro_lc_221,
					   block->number);
				block->state = 255;
				zone->bad_count++;
			} else if (erase_block((struct erase_block_zone *)zone,
				    (struct erase_block_info *)block,
				    (unsigned short)(block->erase_count + 1)) == 0) {
				zone->free_count++;
			}
			goto next;
		}
		nand_print(nftl_ro_lc_222, block->number);
		/* OEM puts the last-page index in the plane field and reads page
		 * zero. Preserve this apparent parameter mix-up. */
		set_physic_op_par(&op, block->number,
				  (unsigned short)(zone->geometry->pages - 1), 0,
				  0, oob);
		status = zone->read(zone, &op);
		if (status == 0 || status == 10) {
			if (is_phy_mapping_page(oob) == 1) {
				nand_print(nftl_ro_lc_223,
					   block->number);
				used = get_block_used_count_from_oob(oob);
				goto counted_last;
			}
			if (is_nouse_page(oob) == 1) {
				nand_print(nftl_ro_lc_225,
					   block->number);
				if (erase_block((struct erase_block_zone *)zone,
				    (struct erase_block_info *)block,
				    (unsigned short)(block->erase_count + 1)) == 0)
					zone->free_count++;
			} else {
				nand_print(nftl_ro_lc_226,
					   block->number);
				if (erase_block((struct erase_block_zone *)zone,
				    (struct erase_block_info *)block,
				    (unsigned short)(block->erase_count + 1)) == 0)
					zone->free_count++;
			}
			goto next;
		}
		nand_print(nftl_ro_lc_227,
			   block->number);
		nand_print(nftl_ro_lc_228,
			   block->number, oob[0], oob[1], oob[2], oob[3],
			   oob[4], oob[5], oob[6], oob[7], oob[8], oob[9],
			   oob[10], oob[11], oob[12], oob[13], oob[14], oob[15]);
		if (torture_block_and_mark((struct torture_zone *)zone,
					   (struct erase_block_info *)block) == 0)
			zone->free_count++;
		goto next;
counted:
		zone->free_count++;
		block->state = 1;
		block->erase_count = get_erase_count_from_oob(oob);
		last_erase = block->erase_count;
		block->invalid_pages = 0;
		block->used_count = used;
		block->next = 0;
		block->previous = 0;
		add_block_count_list((struct block_count_zone *)zone,
				     (struct block_count_node *)block);
		if (status == 10 && zone->gc->claim) {
			add_prio_gc((struct prio_gc_owner *)zone, block, 1);
			nand_print_dbg(nftl_ro_lc_217);
		}
		goto next;
counted_last:
		zone->free_count++;
		block->state = 1;
		block->erase_count = get_erase_count_from_oob(oob);
		last_erase = block->erase_count;
		block->invalid_pages = 0;
		block->used_count = used;
		block->next = 0;
		block->previous = 0;
		add_block_count_list((struct block_count_zone *)zone,
				     (struct block_count_node *)block);
		if (status == 10 && zone->gc->claim) {
			add_prio_gc((struct prio_gc_owner *)zone, block, 1);
			nand_print(nftl_ro_lc_224);
		}
next:
		block++;
	}
	return 0;
}

struct second_scan_smart {
	unsigned char before_erase_max[148];
	unsigned int erase_max;
	unsigned int erase_min;
	unsigned int used_last;
	unsigned int used_first;
};
struct second_scan_gc {
	unsigned char before_crc[28];
	unsigned short crc_enabled;
};
struct second_scan_zone {
	unsigned char before_geometry[8];
	struct first_scan_geometry *geometry;
	unsigned char before_mode[8];
	unsigned char subtype;
	unsigned char flags;
	unsigned short free_count;
	unsigned short bad_count;
	unsigned short spare_count;
	unsigned int logical_pages;
	unsigned int logical_blocks;
	unsigned char before_cached[64 - 40];
	struct first_scan_block *cached_current;
	unsigned char before_blocks[448 - 72];
	struct first_scan_block *blocks;
	unsigned char before_data[512 - 456];
	void *data;
	unsigned char before_current[528 - 520];
	struct first_scan_block *current;
	unsigned char before_map[552 - 536];
	unsigned int *map;
	unsigned int scan_length_bytes;
	unsigned char before_max_erase[676 - 564];
	unsigned int max_erase;
	unsigned char before_smart[3440 - 680];
	struct second_scan_smart *smart;
	struct second_scan_gc *gc;
	unsigned char before_read[8];
	int (*read)(struct second_scan_zone *, struct physic_op_param *);
};
_Static_assert(__builtin_offsetof(struct second_scan_zone, blocks) == 448,
	       "second-scan block-list offset");
_Static_assert(__builtin_offsetof(struct second_scan_zone, current) == 528,
	       "second-scan current-block offset");
_Static_assert(__builtin_offsetof(struct second_scan_zone, map) == 552,
	       "second-scan map offset");
_Static_assert(__builtin_offsetof(struct second_scan_zone, smart) == 3440,
	       "second-scan smart-info offset");
_Static_assert(__builtin_offsetof(struct second_scan_zone, read) == 3464,
	       "second-scan read callback offset");
struct crc_page_zone;
struct logic_page_map_zone;
struct invalid_page_node;
struct page_map_recovery_zone;
struct current_used_zone;
struct cross_rebuild_zone;
struct cross_rebuild_block;
struct invalid_node;
extern volatile unsigned int *g_nreg_base;
static int crc_wrong(struct crc_page_zone *, const unsigned char *,
		     unsigned char *);
extern int recover_logic_page_mapping(struct logic_page_map_zone *,
				       struct invalid_page_node *,
				       const unsigned int *, unsigned int);
extern int recover_phy_page_mapping(struct page_map_recovery_zone *,
				     struct new_write_block *, unsigned int *);
extern int recover_block_phy_page_mapping(struct page_map_recovery_zone *,
					   struct new_write_block *,
					   unsigned int *);
extern int put_phy_block_to_invalid_page_list(void *, struct invalid_node *);
extern int adjust_invaild_list(struct move_zone *);
extern struct cross_rebuild_block *cross_talk_rebuild_current_block(
	struct cross_rebuild_zone *, struct cross_rebuild_block *,
	struct cross_rebuild_block *);
extern struct cross_rebuild_block *no_cross_talk_rebuild_current_block(
	struct cross_rebuild_zone *, struct cross_rebuild_block *,
	struct cross_rebuild_block *);
extern int get_current_used_block(struct current_used_zone *,
				   struct new_write_block *, unsigned int);

__attribute__((optimize("-fno-stack-protector")))
int second_scan_all_blocks(struct build_zone *raw_zone)
{
	struct second_scan_zone *zone = (struct second_scan_zone *)raw_zone;
	struct first_scan_block *block, *first = 0, *second = 0;
	struct first_scan_block *last_recorded = 0, *last_mapping = 0;
	struct first_scan_block *erased = 0, *candidate;
	struct physic_op_param op;
	unsigned char oob[16];
	unsigned int read_page, last_used = ~0U, previous_used = ~0U;
	unsigned int controller_mode = g_nreg_base[188];
	int status;

	if (controller_mode != 0x30001 && controller_mode != 1 &&
	    controller_mode != 2)
		return -1;
	/* OEM divides by the geometry factor without checking for zero. */
	zone->logical_blocks = zone->logical_pages /
		((struct scan_page_geometry *)zone->geometry)->page_factor;
	for (block = zone->blocks; block; block = block->next) {
		previous_used = block->used_count;
		zone->smart->used_last = block->used_count;
	}
	if (zone->blocks)
		zone->smart->used_first = zone->blocks->used_count;
	for (block = zone->blocks; block; block = block->next) {
		read_page = zone->scan_length_bytes >> 9;
		if (zone->scan_length_bytes & 511)
			read_page++;
		set_physic_op_par(&op, block->number,
				  (unsigned short)(zone->geometry->pages - 1),
				  (unsigned short)read_page, zone->data, oob);
		status = zone->read(zone, &op);
		/* OEM lets a CRC failure overwrite any NAND read status. */
		if (zone->gc->crc_enabled &&
		    crc_wrong((struct crc_page_zone *)zone, zone->data, oob))
			status = -2;
		last_used = block->used_count;
		if (block->erase_count > zone->max_erase &&
		    block->erase_count <= 49999) {
			zone->max_erase = block->erase_count;
			zone->smart->erase_max = zone->max_erase;
		}
		if (block->erase_count < zone->smart->erase_min)
			zone->smart->erase_min = block->erase_count;
		if (is_phy_mapping_page(oob) == 1 &&
		    (status == 0 || status == 10)) {
			last_recorded = block;
			block->invalid_pages++;
			recover_logic_page_mapping(
				(struct logic_page_map_zone *)zone,
				(struct invalid_page_node *)block,
				zone->data, zone->geometry->pages - 1);
			last_mapping = block;
		} else if (is_nouse_page(oob) == 1) {
			if (!first) {
				first = block;
			} else if (!second) {
				second = block;
				nand_print(nftl_ro_lc_233,
					   first->number, second->number);
				if (!(zone->flags & 1) &&
				    first->used_count != second->used_count) {
					nand_print(nftl_ro_lc_234,
						   first->number, second->number,
						   first->used_count,
						   second->used_count);
					recover_phy_page_mapping(
					    (struct page_map_recovery_zone *)zone,
					    (struct new_write_block *)first,
					    zone->map);
					recover_logic_page_mapping(
					    (struct logic_page_map_zone *)zone,
					    (struct invalid_page_node *)first,
					    zone->map, zone->geometry->pages - 1);
					add_prio_gc((struct prio_gc_owner *)zone,
						    first, 3);
					first = second;
					second = 0;
				}
			} else {
				nand_print(nftl_ro_lc_235,
					   first->number, first->used_count);
				nand_print(nftl_ro_lc_236,
					   second->number, second->used_count);
				nand_print(nftl_ro_lc_237,
					   block->number, last_used);
				add_prio_gc((struct prio_gc_owner *)zone, first, 3);
				first = second;
				second = block;
			}
		} else {
			nand_print(nftl_ro_lc_238,
				   oob[0], oob[1], oob[2], oob[3], oob[4],
				   oob[5], oob[6], oob[7], oob[8], oob[9],
				   oob[10], oob[11], oob[12], oob[13], oob[14],
				   oob[15]);
			if (status == 0 || status == 10)
				nand_print(nftl_ro_lc_239,
					   zone->subtype, block->number);
			else
				nand_print(nftl_ro_lc_240,
					   zone->subtype, block->number);
			if ((zone->flags & 1) && previous_used == last_used) {
				erase_block((struct erase_block_zone *)zone,
					    (struct erase_block_info *)block,
					    (unsigned short)(block->erase_count + 1));
				erased = block;
			} else {
				last_recorded = block;
				block->state = 170;
				block->invalid_pages++;
				recover_block_phy_page_mapping(
					    (struct page_map_recovery_zone *)zone,
					    (struct new_write_block *)block,
					    zone->map);
				recover_logic_page_mapping(
					    (struct logic_page_map_zone *)zone,
					    (struct invalid_page_node *)block,
					    zone->map, zone->geometry->pages - 1);
				add_prio_gc((struct prio_gc_owner *)zone, block, 3);
			}
		}
	}
	init_smart_info((struct smart_info_zone *)zone,
			(struct smart_info_block *)last_mapping);
	for (block = zone->blocks; block; block = block->next) {
		if (block != first && block != second && block != erased)
			put_phy_block_to_invalid_page_list(zone,
					 (struct invalid_node *)block);
	}
	adjust_invaild_list((struct move_zone *)zone);
	if (last_recorded && first &&
	    last_recorded->used_count + 1 != first->used_count)
		nand_print(nftl_ro_lc_241,
			   last_recorded->used_count, first->used_count);
	zone->current = 0;
	candidate = 0;
	if (!first && !second) {
		nand_print(nftl_ro_lc_242);
		/* OEM has two separate call sites and reads the flag before
		 * checking last_recorded. Keep that order even though the calls
		 * have identical arguments. */
		if (zone->flags & 1) {
			if (last_recorded)
				add_prio_gc((struct prio_gc_owner *)zone,
					    last_recorded, 3);
		} else if (last_recorded && last_recorded->state == 170) {
			add_prio_gc((struct prio_gc_owner *)zone,
				    last_recorded, 3);
		}
	} else if (!second) {
		candidate = first;
		if (zone->flags & 1) {
			nand_print_dbg(nftl_ro_lc_243,
				       first->number, first->used_count);
			if (last_recorded && last_recorded->state == 170)
				add_prio_gc((struct prio_gc_owner *)zone,
					    last_recorded, 3);
			candidate = (struct first_scan_block *)
				cross_talk_rebuild_current_block(
				    (struct cross_rebuild_zone *)zone,
				    (struct cross_rebuild_block *)first, 0);
			if (!candidate)
				return 1;
		} else {
			if (last_recorded && last_recorded->state == 170)
				add_prio_gc((struct prio_gc_owner *)zone,
					    last_recorded, 3);
			if (zone->subtype)
				candidate = (struct first_scan_block *)
					no_cross_talk_rebuild_current_block(
					    (struct cross_rebuild_zone *)zone,
					    (struct cross_rebuild_block *)first, 0);
		}
	} else if (zone->flags & 1) {
		candidate = (struct first_scan_block *)
			cross_talk_rebuild_current_block(
			    (struct cross_rebuild_zone *)zone,
			    (struct cross_rebuild_block *)first,
			    (struct cross_rebuild_block *)second);
		if (!candidate)
			return 1;
	} else if (first->used_count == second->used_count) {
		nand_print(nftl_ro_lc_244,
			   first->number, first->used_count);
		nand_print_dbg(nftl_ro_lc_245,
			       second->number, second->used_count);
		candidate = (struct first_scan_block *)
			no_cross_talk_rebuild_current_block(
			    (struct cross_rebuild_zone *)zone,
			    (struct cross_rebuild_block *)first,
			    (struct cross_rebuild_block *)second);
	} else {
		nand_print(nftl_ro_lc_246);
		/* OEM dereferences last_recorded without a null check. */
		nand_print(nftl_ro_lc_247,
			   last_recorded->number, last_recorded->used_count);
		nand_print(nftl_ro_lc_244,
			   first->number, first->used_count);
		nand_print_dbg(nftl_ro_lc_245,
			       second->number, second->used_count);
		recover_block_phy_page_mapping(
			(struct page_map_recovery_zone *)zone,
			(struct new_write_block *)first, zone->map);
		recover_logic_page_mapping((struct logic_page_map_zone *)zone,
			(struct invalid_page_node *)first, zone->map,
			zone->geometry->pages - 1);
		add_prio_gc((struct prio_gc_owner *)zone, first, 3);
		recover_block_phy_page_mapping(
			(struct page_map_recovery_zone *)zone,
			(struct new_write_block *)second, zone->map);
		recover_logic_page_mapping((struct logic_page_map_zone *)zone,
			(struct invalid_page_node *)second, zone->map,
			zone->geometry->pages - 1);
		add_prio_gc((struct prio_gc_owner *)zone, second, 3);
		/* OEM leaves candidate null and asks for a new free block. */
	}
	if (get_current_used_block((struct current_used_zone *)zone,
				   (struct new_write_block *)candidate, last_used)) {
		nand_print(nftl_ro_lc_248);
		return 1;
	}
	adjust_invaild_list((struct move_zone *)zone);
	zone->cached_current = zone->current;
	nand_print_dbg(nftl_ro_lc_249,
		       zone->spare_count, zone->smart->erase_max,
		       zone->smart->erase_min, zone->smart->used_last,
		       zone->smart->used_first);
	return 0;
}

struct first_list_geometry {
	unsigned char before_factor[8];
	unsigned char factor;
	unsigned char before_pages[11];
	unsigned short pages;
	unsigned char before_blocks[2];
	unsigned short blocks;
	unsigned short page_bytes;
	unsigned short fixed_page;
};
_Static_assert(__builtin_offsetof(struct first_list_geometry, page_bytes) == 26,
	       "panic-write page-byte count offset");
struct first_list_block {
	unsigned short number;
	unsigned short state;
	unsigned short erase_count;
	unsigned short used_pages;
	unsigned char before_links[8];
	void *links[3];
	unsigned char tail[24];
};
struct first_list_zone {
	unsigned char before_geometry[8];
	struct first_list_geometry *geometry;
	unsigned char before_count[10];
	unsigned short count;
	unsigned char before_pages[4];
	unsigned int logical_pages;
	unsigned char before_extra_pages[4];
	unsigned int extra_pages;
	unsigned char before_cursor[4];
	struct first_list_block *cursor;
	struct first_list_block *last;
	unsigned char before_data[448];
	void *data;
	unsigned char before_callbacks[2936];
	int (*erase)(struct first_list_zone *, struct physic_op_param *);
	void *unused_3464;
	int (*write)(struct first_list_zone *, struct physic_op_param *);
	void *unused_3480;
	int (*is_good)(struct first_list_zone *, struct physic_op_param *);
	int (*mark_bad)(struct first_list_zone *, struct physic_op_param *);
};
_Static_assert(sizeof(struct first_list_block) == 64,
	       "first-scan block stride");
_Static_assert(__builtin_offsetof(struct first_list_zone, count) == 26,
	       "first-scan count offset");
_Static_assert(__builtin_offsetof(struct first_list_zone, cursor) == 48,
	       "first-scan cursor offset");
_Static_assert(__builtin_offsetof(struct first_list_zone, data) == 512,
	       "first-scan data offset");
_Static_assert(__builtin_offsetof(struct first_list_zone, is_good) == 3488,
	       "first-scan good callback offset");
/* OEM keeps the stack OOB buffer without a canary. */
__attribute__((optimize("-fno-stack-protector")))
int build_zone_list_first(struct first_list_zone *zone, unsigned int number)
{
	unsigned int limit;
	unsigned int page;
	int result;
	/* Stock passes the OOB pointer at FP+0x38. A twenty-byte local places
	 * it there; callbacks and memset use only the first sixteen bytes. */
	struct first_list_block *block;
	unsigned char oob[20];
	struct physic_op_param op;

	nand_print_dbg(nftl_ro_lc_209);
	/* OEM loads cursor after the debug callback; that callback can change
	 * the initial block before the scan begins. */
	block = zone->cursor;
	/* OEM performs two unguarded divisions, including when either divisor
	 * is zero. Keep this crash behavior until stock compatibility ends. */
	if (zone->logical_pages) {
		limit = (zone->logical_pages + zone->extra_pages) /
			zone->geometry->factor;
		limit /= zone->geometry->pages;
	} else {
		limit = ~0U;
	}
	zone->count = 0;
	while (zone->count < limit) {
		zone->last = block;
		set_physic_op_par(&op, block->number, 0,
				  zone->geometry->fixed_page, zone->data, oob);
		if (zone->is_good(zone, &op) == 1) {
			result = zone->erase(zone, &op);
			if (result) {
				block->state = 255;
				zone->mark_bad(zone, &op);
			} else {
				zone->count++;
				block->state = 0;
				block->used_pages = 0;
				if (zone->count == 1) {
					nand_memset(oob, -1, 16);
					set_start_block_flag(zone, oob, number);
					set_physic_op_par(&op, block->number, 0,
						zone->geometry->fixed_page,
						zone->data, oob);
					/* OEM ignores every write result and advances only
					 * the operation's plane field. */
					for (page = 0; page < zone->geometry->pages;
					     page++) {
						zone->write(zone, &op);
						op.plane++;
					}
					zone->cursor = block + 1;
				}
			}
		} else {
			block->state = 255;
		}
		block->erase_count = 0xffff;
		block->links[2] = 0;
		block->links[0] = 0;
		block->links[1] = 0;
		zone->last = block;
		if (is_last_phy_block((struct physical_block *)block,
				      (struct physical_zone *)zone) == 1)
			break;
		block++;
	}
	if (zone->count <= 1) {
		nand_print(nftl_ro_lc_210, zone->count);
		return 1;
	}
	zone->count--;
	if (limit == ~0U)
		return 0;
	if (is_last_phy_block((struct physical_block *)block,
			      (struct physical_zone *)zone) == 1)
		return 0;
	while (block->number < zone->geometry->blocks) {
		set_physic_op_par(&op, block->number, 0,
				  zone->geometry->fixed_page, zone->data, oob);
		if (zone->is_good(zone, &op) == 1) {
			result = zone->erase(zone, &op);
			if (result) {
				block->state = 255;
				zone->mark_bad(zone, &op);
			} else {
				nand_memset(oob, -1, 16);
				set_end_block_flag(zone, oob);
				set_physic_op_par(&op, block->number, 0,
						  zone->geometry->fixed_page,
						  zone->data, oob);
				zone->write(zone, &op);
				/* OEM ignores the end-flag write result. */
				break;
			}
		} else {
			block->state = 255;
		}
		block++;
	}
	/* OEM also reports success when the search exhausted all blocks. */
	return 0;
}

/* Inferred cache write-list bodies. Private layout names are provisional.
 * Each 16-bit count update uses AND #0xffff where stock uses UXTH; both
 * preserve the same low halfword, including wraparound. */
struct cache_list_node {
    unsigned short unknown;
    unsigned short flag;
    unsigned char until_next[36];
    struct cache_list_node *next;
    struct cache_list_node *previous;
};
struct cache_list_owner {
    unsigned char unknown[4];
    unsigned short count;
    unsigned char until_head[1738];
    struct cache_list_node head;
};
int add_to_cache_write_list_tail(struct cache_list_owner *owner, struct cache_list_node *node)
{
    struct cache_list_node *cursor = &owner->head;
    /* OEM never checks for a cycle in either list. */
    while (cursor->next)
        cursor = cursor->next;
    cursor->next = node;
    node->next = 0;
    node->previous = cursor;
    node->flag = 1;
    owner->count++;
    return 0;
}
struct sector_node {
    unsigned short unknown;
    unsigned short flag;
    unsigned int sector;
    void *data;
    struct sector_node *next;
    struct sector_node *previous;
};
struct empty_sector_pool {
	unsigned char before_nodes[16];
	struct sector_node nodes[16];
};
struct sector_node *get_empty_sector_cahce_node(struct empty_sector_pool *pool)
{
	unsigned short index;

	for (index = 0; index <= 15; index++) {
		/* Preserve OEM's 16-bit 0x00ff sentinel and misspelled symbol. */
		if (pool->nodes[index].flag == 255)
			return &pool->nodes[index];
	}
	return 0;
}
struct sector_owner {
    unsigned char unknown[2];
    unsigned short count;
    unsigned char until_head[524];
    struct sector_node head;
};
int add_to_sector_cache_write_list_tail(struct sector_owner *owner, struct sector_node *node)
{
    struct sector_node *cursor = &owner->head;
    /* OEM never checks for a cycle in either list. */
    while (cursor->next)
        cursor = cursor->next;
    cursor->next = node;
    node->next = 0;
    node->previous = cursor;
    node->flag = 1;
    owner->count++;
    return 0;
}
struct cache_list_node *del_from_cache_write_list(struct cache_list_owner *owner, struct cache_list_node *node)
{
    struct cache_list_node *previous = node->previous;
    /* OEM dereferences previous without checking whether node is linked. */
    previous->next = node->next;
    if (node->next)
        node->next->previous = previous;
    node->next = 0;
    node->previous = 0;
    node->flag = 255;
    /* Deliberately allow the 16-bit list count to wrap below zero. */
    owner->count--;
    return node;
}
struct sector_node *del_from_sector_cache_write_list(struct sector_owner *owner, struct sector_node *node)
{
    struct sector_node *previous = node->previous;
    /* OEM dereferences previous without checking whether node is linked. */
    previous->next = node->next;
    if (node->next)
        node->next->previous = previous;
    node->next = 0;
    node->previous = 0;
    node->flag = 255;
    /* Deliberately allow the 16-bit list count to wrap below zero. */
    owner->count--;
    return node;
}

struct sector_cache_init_view {
	unsigned short capacity;
	unsigned short count;
	unsigned char before_buffer[4];
	unsigned char *buffer;
	struct sector_node nodes[16];
	struct sector_node head;
};
/* OEM keeps cache, buffer and index at FP+0x28/+0x30/+0x3c. The initializer
 * recomputes each node field address and only stores the buffer local after
 * the allocation has succeeded and the list head has been cleared. */
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

/* The two zero stores below are redundant for results, but the OEM emits
 * both before deriving the cache pointer and then overwrites bytes with 512. */
__attribute__((optimize("-fno-stack-protector")))
int nand_sector_cache_write(void *zone, unsigned int sector, void *data)
{
	int result;
	unsigned short bytes;
	struct sector_owner *cache;
	struct sector_node *node;

	result = 0;
	bytes = 0;
	cache = (struct sector_owner *)((unsigned char *)zone + 2880);
	bytes = 512;
	node = (struct sector_node *)search_from_sector_cache_write_list(
		(struct sector_cache_zone *)cache, sector);

	if (node) {
		del_from_sector_cache_write_list(cache, node);
		/* OEM discards the copy and list-operation return values. */
		nand_memcpy(node->data, data, bytes);
		add_to_sector_cache_write_list_tail(cache, node);
		return 0;
	}
	if (cache->count > 7) {
		/* OEM assumes a nonempty list whenever count exceeds seven. */
		node = del_from_sector_cache_write_list(cache, cache->head.next);
		result = __nand_write(zone, node->sector, 1, node->data);
		/* On write failure the evicted node stays unlinked and its data
		 * is lost; retain this stock ordering and return convention. */
		if (result) {
			nand_print(nftl_ro_lc_143);
			return 1;
		}
	} else {
		node = get_empty_sector_cahce_node(
			(struct empty_sector_pool *)cache);
		if (!node) {
			nand_print(nftl_ro_lc_144);
			return 1;
		}
	}
	node->sector = sector;
	nand_memcpy(node->data, data, bytes);
	add_to_sector_cache_write_list_tail(cache, node);
	return 0;
}

/* Restore the OEM's node and bookkeeping slots without changing its
 * unchecked list walk or wrapping zero limit. */
__attribute__((optimize("-fno-stack-protector")))
int __nand_flush_sector_write_cache(void *zone, unsigned int limit)
{
	struct sector_owner *cache =
		(struct sector_owner *)((char *)zone + 2880);
	int result = 0;
	int had_entries;
	struct sector_node *node;

	/* OEM sets this flag but never reads it. Retain its stock stack write. */
	if (cache->count)
		had_entries = 1;
	else
		had_entries = 0;
	while (cache->head.next) {
		node = del_from_sector_cache_write_list(cache, cache->head.next);
		result = __nand_write(zone, node->sector, 1, node->data);
		/* Zero limit wraps and drains the list instead of doing no work. */
		limit--;
		if (limit == 0)
			break;
	}
	if (result)
		nand_print(nftl_ro_lc_145);
	return result;
}

struct write_flush_list {
	unsigned char before_head[1784];
	void *head;
};
struct write_flush_zone {
	unsigned char before_id[24];
	unsigned char id;
	unsigned char before_debug[656 - 25];
	unsigned int debug_level;
	unsigned char before_cache[1072 - 660];
	struct write_flush_list cache;
};
struct flush_cache_zone;
extern int flush_write_cache_to_nand(struct flush_cache_zone *zone, int count);
/* GCC otherwise moves the private loop state to different frame slots.
 * Keep the OEM stack layout and its zero-limit underflow behavior. */
__attribute__((optimize("-fno-stack-protector")))
int __nand_flush_write_cache(void *zone, unsigned int limit)
{
	int result;
	unsigned int flushed = 0;
	struct write_flush_list *cache =
		&((struct write_flush_zone *)zone)->cache;
	result = 0;

	while (cache->head) {
		flushed++;
		result |= flush_write_cache_to_nand(zone, 1);
		/* OEM wraps a zero limit and may keep calling a helper which did
		 * not remove the head; preserve both behaviors. */
		limit--;
		if (limit == 0)
			break;
	}
	if (result)
		nand_print(nftl_ro_lc_145);
	if (flushed && ((struct write_flush_zone *)zone)->debug_level)
		nand_print_dbg(nftl_ro_lc_429,
			       ((struct write_flush_zone *)zone)->id, flushed);
	return result;
}

struct flush_cache_geometry {
	unsigned char before_sectors_per_page[8];
	unsigned char sectors_per_page;
	unsigned char before_page_bytes[17];
	unsigned short page_bytes;
};
struct flush_cache_node {
	unsigned char before_state[4];
	unsigned short state;
	unsigned short sectors;
	unsigned int page;
	unsigned int untouched;
	unsigned char *data;
	unsigned char before_links[16];
	struct flush_cache_node *next;
	struct flush_cache_node *previous;
};
struct flush_cache_data {
	unsigned char before_write_count[6];
	unsigned short write_count;
	unsigned char before_write_head[1776];
	struct flush_cache_node *write_head;
	unsigned char before_buffer[8];
	unsigned char *page_buffer;
};
struct flush_cache_zone {
	unsigned char before_geometry[8];
	struct flush_cache_geometry *geometry;
	unsigned char before_pages[568];
	unsigned short pages;
	unsigned char before_cache[486];
	struct flush_cache_data cache;
	unsigned char before_callbacks[624];
	int (*write)(struct flush_cache_zone *, unsigned int, void *);
	int (*read)(struct flush_cache_zone *, unsigned int, void *);
};
_Static_assert(sizeof(struct flush_cache_node) == 56, "flush cache node size");
_Static_assert(__builtin_offsetof(struct flush_cache_node, next) == 40,
	       "flush cache node next offset");
_Static_assert(__builtin_offsetof(struct flush_cache_zone, pages) == 584,
	       "flush cache zone page offset");
_Static_assert(__builtin_offsetof(struct flush_cache_zone, cache) == 1072,
	       "flush cache zone cache offset");
_Static_assert(__builtin_offsetof(struct flush_cache_zone, write) == 3504,
	       "flush cache write callback offset");
_Static_assert(__builtin_offsetof(struct flush_cache_zone, read) == 3512,
	       "flush cache read callback offset");
extern int nand_memcmp(const void *, const void *, unsigned long);
extern int nand_phy_get_page_type(unsigned int);

__attribute__((optimize("-fno-stack-protector")))
int flush_write_cache_to_nand(struct flush_cache_zone *zone, int ignored_count)
{
	struct flush_cache_geometry *geometry = 0;
	unsigned char signature_match = 1;
	/* The OEM frame reserves six unused bytes behind this eight-byte
	 * signature. Keep the private frame shape without reading them. */
	unsigned char signature0[14];
	unsigned char signature1[8];
	int status;
	struct flush_cache_data *cache;
	struct flush_cache_node *node;
	unsigned char *data;
	unsigned short offset;

	(void)ignored_count;
	signature0[0] = 0x2c;
	signature0[1] = 0x64;
	signature0[2] = 0x44;
	signature0[3] = 0x32;
	signature0[4] = 0xa5;
	signature0[5] = 0xff;
	signature0[6] = 0xff;
	signature0[7] = 0xff;
	signature1[0] = 0x2c;
	signature1[1] = 0x84;
	signature1[2] = 0x44;
	signature1[3] = 0x32;
	signature1[4] = 0xaa;
	signature1[5] = 0x04;
	signature1[6] = 0x00;
	signature1[7] = 0x00;
	status = 0;
	cache = &zone->cache;
	if (!cache->write_head) {
		nand_print(nftl_ro_lc_426);
		return 0;
	}
	node = (struct flush_cache_node *)del_from_cache_write_list(
		(struct cache_list_owner *)cache,
		(struct cache_list_node *)cache->write_head);
	if (!node)
		return 0;
	data = node->data;
	if (node->sectors != zone->geometry->sectors_per_page) {
		status = zone->read(zone, node->page, cache->page_buffer);
		/* OEM copies from the read buffer even after a read failure. */
		/* OEM masks the sector count to seven bits and trusts that the
		 * resulting offset does not exceed page_bytes. The explicit
		 * truncation preserves its 16-bit offset and bitfield insert. */
		offset = (unsigned short)((node->sectors & 127) << 9);
		nand_memcpy(data + offset, cache->page_buffer + offset,
			    (unsigned int)zone->geometry->page_bytes - offset);
		if (status == 10)
			nand_print(nftl_ro_lc_427,
				   node->page, cache->write_count);
	}
	node->state = 0;
	node->sectors = zone->geometry->sectors_per_page;
	status = zone->write(zone, node->page, data);
	if (status)
		nand_print(nftl_ro_lc_428);
	/* OEM snapshots the geometry after the write callback. A later
	 * comparison callback can replace zone->geometry, but both signature
	 * checks still use this original pointer. */
	geometry = zone->geometry;
	/* Only the low byte of nand_memcmp's signed result is retained. */
	signature_match = nand_memcmp(geometry, signature0, 8);
	if (signature_match)
		signature_match = nand_memcmp(geometry, signature1, 8);
	if (!signature_match && zone->pages &&
	    nand_phy_get_page_type(zone->pages - 1) == 2) {
		status = zone->write(zone, node->page, data);
		if (status)
			nand_print(nftl_ro_lc_428);
	}
	add_to_cache_read_list_tail((struct cache_read_add_zone *)cache,
				    (struct cache_read_delete_node *)node, 2);
	/* OEM reports success even after both writes failed. */
	return 0;
}

struct gc_list_node {
	unsigned char before_next[0x10];
	struct gc_list_node *next;
	struct gc_list_node *previous;
};
struct gc_list {
	unsigned short count;
	unsigned char before_head[0x148 - 2];
	struct gc_list_node head;
};
int add_to_gc_list_tail(struct gc_list *list, struct gc_list_node *target)
{
	struct gc_list_node *node;

	/* OEM neither checks duplicates nor detects cycles. */
	for (node = &list->head; node->next; node = node->next)
		;
	node->next = target;
	target->next = 0;
	target->previous = node;
	/* OEM wraps this 16-bit count on overflow. */
	list->count++;
	return 0;
}


struct invalid_node *del_block_invalid_list(struct invalid_zone *zone)
{
	struct invalid_node *node = &zone->head;

	node = node->next;
	if (node) {
		zone->head.next = node->next;
		if (node->next)
			node->next->previous = &zone->head;
	}
	return node;
}

/* -fstack-protector reorders pointer locals without a canary here. */
__attribute__((optimize("-fno-stack-protector")))
struct nftl_list_node *del_last_nftl_blk(struct nftl_list_node *head)
{
	struct nftl_list_node *last = 0;
	struct nftl_list_node *current = head;

	/* OEM follows unchecked links and can loop forever on a cycle. */
	while (current->next) {
		last = current->next;
		if (last->next == 0) {
			current->next = 0;
			return last;
		}
		current = current->next;
	}
	return 0;
}

struct move_geometry {
	unsigned char before_pages[0x14];
	unsigned short pages;
};
struct move_node {
	unsigned char before_block[4];
	unsigned short block;
};
struct move_zone {
	unsigned char before_geometry[8];
	struct move_geometry *geometry;
	unsigned char before_first[0x160 - 0x10];
	struct move_node *first;
};
extern struct move_node *out_phy_block_from_invalid_page_list(
	struct move_zone *zone);
extern int put_phy_block_to_free_list(struct move_zone *zone,
				     struct move_node *node);
int adjust_invaild_list(struct move_zone *zone)
{
	struct move_node *node;

	while (zone->first) {
		node = zone->first;
		if (node->block != zone->geometry->pages)
			break;
		node = out_phy_block_from_invalid_page_list(zone);
		/* OEM ignores a failed free-list insertion. */
		put_phy_block_to_free_list(zone, node);
	}
	return 0;
}

extern unsigned char micron_read_retry_mode;
extern unsigned char micron_read_retry_cycle;
extern unsigned char *micron_p1_recovered;
extern unsigned char micron_p1_1_recovered[];
extern unsigned char micron_p1_2_recovered[];
extern unsigned char micron_p1_3_recovered[];
extern unsigned char micron_p1_4_recovered[];
struct micron_init_params {
	unsigned char before_retry_spec[52];
	unsigned int retry_spec;
};
struct micron_init_chip {
	unsigned char before_retry_count[184];
	unsigned int retry_count;
	unsigned char before_params[280 - 188];
	struct micron_init_params *params;
};
int micron_readretry_init(struct micron_init_chip *chip)
{
	chip->retry_count = 0;
	micron_read_retry_mode = 0;
	micron_p1_recovered = micron_p1_2_recovered;
	/* Stock explicitly truncates both fields to one byte (UXTB), and
	 * reloads the unchecked nested configuration for each assignment. */
	micron_read_retry_mode = (unsigned char)(chip->params->retry_spec >> 16);
	micron_read_retry_cycle = (unsigned char)(chip->params->retry_spec >> 8);
	/* OEM ignores feature-command errors and leaves these global writes in
	 * place even when the mode is unsupported. */
	micron_intel_set_feature(chip, 0x89, 0);
	if (micron_read_retry_mode == 65) {
		micron_read_retry_cycle = 12;
		micron_p1_recovered = micron_p1_2_recovered;
	} else if (micron_read_retry_mode == 64) {
		micron_p1_recovered = micron_p1_1_recovered;
		micron_read_retry_cycle = 10;
	} else if (micron_read_retry_mode == 80) {
		micron_p1_recovered = micron_p1_3_recovered;
		micron_read_retry_cycle = 7;
		micron_intel_set_feature(chip, 0x93, 0);
	} else if (micron_read_retry_mode == 66) {
		micron_p1_recovered = micron_p1_4_recovered;
		micron_read_retry_cycle = 18;
	} else {
		nand_print(nftl_ro_lc_72);
		return -71;
	}
	nand_print_dbg(nftl_ro_lc_73,
		       micron_read_retry_cycle);
	return 0;
}
int micron_readretry_exit(void *chip)
{
	/* OEM resets global retry state but ignores the chip argument. */
	micron_read_retry_mode = 0;
	micron_read_retry_cycle = 0;
	return 0;
}

extern int generic_read_page_end_not_retry(void *);
extern void (*df_read_page_end)(void);
int generic_special_init(void)
{
	df_read_page_end = (void (*)(void))generic_read_page_end_not_retry;
	nand_print_dbg(nftl_ro_lc_105);
	return 0;
}

extern unsigned int nand_on_panic;
extern unsigned int panic_enable;

static void panic_mark_panic(void)
{
	nand_on_panic = 1;
}

void panic_mark_enable(void)
{
	panic_enable = 1;
}
struct panic_part_state {
	const char *name;
	unsigned int offset;
	unsigned int size;
};
extern struct panic_part_state panic_part;
int panic_init_part(const char *name, unsigned int offset, unsigned int size)
{
	if (!name || !size)
		return -1;
	/* OEM uses nonzero size as a permanent initialized marker and never
	 * validates overlap or range against the underlying partition. */
	if (panic_part.size)
		return -1;
	panic_part.name = name;
	panic_part.offset = offset;
	panic_part.size = size;
	panic_mark_enable();
	nand_print(nftl_ro_lc_18, name, offset, size);
	return 0;
}

/* AND #0xff and stock UXTB both zero-extend the comparison result. */
unsigned char is_on_panic(void)
{
	return nand_on_panic != 0;
}

/* Preserve OEM's unchecked shared BSS flag and byte result. */
int is_panic_enable(void)
{
	return panic_enable != 0;
}

struct panic_read_ops {
	unsigned char before_read[72];
	int (*read)(struct panic_read_ops *, unsigned int, unsigned int, void *);
};
int panic_read(struct panic_read_ops *ops, unsigned int sector,
	       unsigned int count, void *buffer)
{
	void *cursor = buffer;

	if (!is_panic_enable())
		return -1;
	panic_mark_panic();
	/* OEM splits at 32768 sectors and ignores every read callback result. */
	while (count > 32767) {
		ops->read(ops, sector, 32768, cursor);
		cursor += 32768;
		sector += 32768;
		count -= 32768;
	}
	if (count)
		ops->read(ops, sector, count, cursor);
	return 0;
}

struct panic_write_info {
	struct first_list_block *blocks[3];
	unsigned int remaining;
	unsigned int block_index;
	unsigned int page;
};
extern void *panic_info[5];
struct panic_write_owner {
	unsigned char before_zone[24];
	struct first_list_zone *zone;
};
/* The OEM OOB buffer is stack-resident without a stack canary. */
__attribute__((optimize("-fno-stack-protector")))
int panic_write(struct panic_write_owner *owner, unsigned int sector,
		unsigned int count, void *buffer)
{
	struct panic_write_info *info = (void *)panic_info;
	struct first_list_zone *zone;
	struct first_list_geometry *geometry;
	struct first_list_block *block;
	/* This order preserves the OEM operation and scalar stack slots.
	 * The OOB array still lands four bytes above its stock slot. */
	unsigned char oob[16];
	struct physic_op_param op;
	unsigned char *cursor;
	unsigned int pages, blocks;
	unsigned int dummy;

	if (!is_panic_enable())
		return -1;
	panic_mark_panic();
	zone = owner->zone;
	geometry = zone->geometry;
	/* OEM divides without checking factor or pages for zero. */
	pages = (geometry->factor + count - 1) / geometry->factor;
	blocks = (geometry->pages + pages - 1) / geometry->pages;
	nand_print(nftl_ro_lc_22,
		   sector, count, pages, blocks);
	/* OEM subtracts three from an unsigned remaining count. A value below
	 * three wraps, allowing an out-of-bounds block pointer later. */
	if (info->remaining - 3 < pages) {
		nand_print(nftl_ro_lc_23,
		   (info->remaining - 3) * geometry->factor);
		return -1;
	}
	nand_memset(oob, -91, 16);
	cursor = buffer;
	block = info->blocks[info->block_index];
	while (pages) {
		if (info->page >= geometry->pages) {
			info->block_index++;
			block = info->blocks[info->block_index];
			info->page = 0;
		}
		if (!info->page) {
			set_physic_op_par(&op, block->number, 0,
					  geometry->fixed_page, 0, 0);
			/* OEM ignores the erase result. */
			zone->erase(zone, &op);
			block->used_pages = (unsigned short)(block->used_pages + 1);
			block->erase_count = 0;
		}
		nand_print(nftl_ro_lc_24,
			   block->number, info->page);
		oob[0] = 0xff;
		oob[1] = -86;
		oob[2] = 99;
		oob[3] = *(unsigned char *)((char *)zone + 24);
		oob[4] = (unsigned char)info->block_index;
		set_spare_data(oob, block->used_pages, 5, 2);
		set_spare_data(oob, sector, 7, 4);
		set_spare_data(oob, geometry->factor < count ? geometry->factor : count,
			       11, 4);
		set_physic_op_par(&op, block->number,
				  (unsigned short)info->page,
				  geometry->fixed_page, cursor, oob);
		/* OEM ignores the write result. */
		zone->write(zone, &op);
		/* OEM advances by the configured byte count even for a short page. */
		cursor += geometry->page_bytes;
		/* OEM reloads factor after the write callback. A callback can
		 * change it; retaining the repeated reads preserves that quirk. */
		sector += geometry->factor < count ? geometry->factor : count;
		count -= geometry->factor < count ? geometry->factor : count;
		info->remaining--;
		info->page++;
		pages--;
	}
	oob[2] = 98;
	for (dummy = 0; dummy < 3; dummy++) {
		if (!info->page || info->page >= geometry->pages)
			break;
		nand_print(nftl_ro_lc_25,
			   block->number, info->page);
		set_physic_op_par(&op, block->number,
				  (unsigned short)info->page,
				  geometry->fixed_page, cursor, oob);
		/* OEM ignores dummy write failures too. */
		zone->write(zone, &op);
		info->page++;
		info->remaining--;
		if (info->page >= geometry->pages) {
			info->block_index++;
			block = info->blocks[info->block_index];
			info->page = 0;
		}
	}
	return 0;
}

struct gc_policy_geometry {
	unsigned char before_pages[0x14];
	unsigned short pages;
};
struct gc_policy_zone {
	unsigned char before_geometry[8];
	struct gc_policy_geometry *geometry;
	unsigned char before_used[0x1e - 0x10];
	unsigned short used;
	unsigned char before_target[0x2aa - 0x20];
	unsigned short target;
	unsigned short active;
};
extern unsigned int gc_src_type;
struct gc_one_state {
	unsigned char before_limit[4];
	unsigned short limit;
};
struct gc_one_zone {
	unsigned char before_first_stat[30];
	unsigned short first_stat;
	unsigned char before_state[320];
	struct gc_one_state *state;
	unsigned char before_second_stat[320];
	unsigned short second_stat;
	unsigned short third_stat;
};
extern int garbage_collect_first(struct gc_one_zone *, void *, int);
extern const char *nand_get_cur_task_name(void);
extern int nand_get_cur_task_pid(void);
int gc_one(struct gc_one_zone *zone, unsigned short priority)
{
	struct gc_one_state *state = zone->state;
	int result;

	if (!state)
		return 8;
	if (priority < state->limit) {
		result = garbage_collect_first(zone, 0, -1);
		if (result == 8)
			return 8;
		if (result == 0) {
			nand_print_dbg(nftl_ro_lc_459,
				       gc_src_type, zone->first_stat, zone->second_stat,
				       zone->third_stat, nand_get_cur_task_name(),
				       nand_get_cur_task_pid());
			return 0;
		}
		/* OEM collapses every nonzero, non-eight result to one. */
		nand_print(nftl_ro_lc_460);
		return 1;
	}
	return 8;
}

struct power_gc_geometry {
	unsigned char before_divisor[8];
	unsigned char divisor;
	unsigned char before_pages[11];
	unsigned short pages;
};
struct power_gc_zone {
	unsigned char before_geometry[8];
	struct power_gc_geometry *geometry;
	unsigned char before_reserved[26 - 16];
	unsigned short reserved;
	unsigned char before_available[30 - 28];
	unsigned short available;
	unsigned int count_32;
	unsigned char before_count_40[40 - 36];
	unsigned int count_40;
	unsigned char before_target[680 - 44];
	unsigned short target;
	unsigned short emergency;
};
int power_on_gc(struct power_gc_zone *zone)
{
	unsigned short attempts = 0;
	unsigned short priority;
	int result;

	if (zone->available >= zone->target) {
		/* The OEM C divisions are undefined when either divisor is zero.
		 * On the shipped AArch64/GCC build they are bare UDIV instructions,
		 * which return zero. A portable fix must explicitly guard them. */
		nand_print_dbg(nftl_ro_lc_462,
			       zone->available, zone->target, zone->emergency,
			       zone->reserved,
			       zone->count_32 / zone->geometry->divisor /
					zone->geometry->pages,
			       zone->count_40 / zone->geometry->divisor /
					zone->geometry->pages);
		return 0;
	}
	gc_src_type = 2;
	priority = zone->geometry->pages - 10;
	/* OEM relies on GC to increase availability; if it never does and
	 * never returns eight, this loop does not terminate. */
	while (zone->available < zone->target) {
		result = gc_one((struct gc_one_zone *)zone, priority);
		if (result == 8)
			break;
		attempts++;
	}
	priority = zone->geometry->pages >> 3;
	while (zone->available <= 4) {
		nand_print_dbg(nftl_ro_lc_463,
			       zone->available);
		result = gc_one((struct gc_one_zone *)zone, priority);
		if (result == 8)
			break;
		attempts++;
	}
	nand_print_dbg(nftl_ro_lc_464, attempts);
	return 0;
}

struct gc_win_chip {
	unsigned char before_pages[20];
	unsigned short pages_per_block;
};
struct gc_win_node {
	unsigned char before_count[4];
	unsigned short count;
};
struct gc_win_zone {
	unsigned char before_chip[8];
	struct gc_win_chip *chip;
	unsigned char before_current[336];
	struct gc_win_node *current;
};
/* The OEM's signed progress can overflow after count - 1 wraps. Preserve
 * its two's-complement wrap rather than allowing C overflow optimization. */
__attribute__((optimize("-fwrapv")))
int gc_win_block(struct gc_win_zone *zone, unsigned short blocks)
{
	struct {
		int result;
		int limit;
		int progress;
	} locals;

	/* OEM uses a signed comparison on wrapped 32-bit progress and silently
	 * succeeds if the current node is absent. */
	locals.progress = 0;
	locals.limit = (int)((unsigned int)blocks * zone->chip->pages_per_block);
	while ((int)locals.progress < locals.limit) {
		if (!zone->current)
			return 0;
		locals.progress = zone->current->count + locals.progress;
		locals.progress--;
		locals.result = gc_one((struct gc_one_zone *)zone, 1);
		if (locals.result) {
			if (locals.result == 1)
				nand_print_dbg(nftl_ro_lc_461);
			return locals.result;
		}
	}
	return 0;
}

int gc_all_cnt(struct gc_policy_zone *zone, unsigned short pages,
	       unsigned int count)
{
	int result;

	gc_src_type = 0xff;
	/* OEM ignores all failures except result 8. A negative signed caller
	 * count becomes a huge unsigned retry span. */
	while (count--) {
		result = gc_one((struct gc_one_zone *)zone, pages);
		if (result == 8)
			break;
	}
	return 0;
}

int gc_all(void *zone, unsigned short count)
{
	int result;
	gc_src_type = 255;
	/* OEM retries without a timeout until the callback returns nonzero. */
	while (1) {
		result = gc_one((struct gc_one_zone *)zone, count);
		if (result != 0)
			break;
	}
	return 0;
}

int all_time_gc(struct gc_policy_zone *zone)
{
	/* OEM dereferences geometry unchecked and wraps this 16-bit page
	 * count when the source has fewer than three pages. */
	unsigned short pages = zone->geometry->pages - 3;

	if (zone->used < zone->target && zone->active != 0) {
		gc_src_type = 1;
		/* OEM ignores GC failure. */
		gc_one((struct gc_one_zone *)zone, pages);
	}
	return 0;
}
struct shutdown_gc_zone {
	unsigned char before_first[30];
	unsigned short first;
	unsigned char before_current[320];
	void *current;
	unsigned char before_second[320];
	unsigned short second;
	unsigned short third;
	unsigned short enabled;
};
int shutdown_on_gc(struct shutdown_gc_zone *zone)
{
	void *current = zone->current;

	gc_src_type = 4;
	/* Preserve the malformed trailing percent in the OEM diagnostic. */
	nand_print_dbg(nftl_ro_lc_465, nftl_ro_func_8331, current,
		       zone->enabled, zone->first, zone->second, zone->third);
	while (zone->first <= zone->third) {
		current = zone->current;
		if (!current) {
			nand_print(nftl_ro_lc_466,
				   zone->first, zone->second, zone->third);
			break;
		}
		/* With enabled==0, stock may loop forever without changing the
		 * counters. When enabled, it ignores gc_one failure and still logs
		 * success. Both behaviors are retained. */
		if (zone->enabled) {
			gc_one((struct gc_one_zone *)zone, 0);
			nand_print_dbg(nftl_ro_lc_467,
				       zone->first, zone->second, zone->third);
		}
	}
	return 0;
}

struct nftl_shutdown_view {
	unsigned char preceding[24];
	void *operation;
};
int _shutdown_op(struct nftl_shutdown_view *object)
{
	/* OEM dereferences the owner even though its shutdown callee always
	 * reports success without using the resulting operation pointer. */
	return __shutdown_op(object->operation);
}

extern int nand_get_support_boot_check_crc(void);
unsigned char nftl_support_boot_check_crc_from_dts(void)
{
	return nand_get_support_boot_check_crc() != 0;
}

extern int nand_physic_erase_block(unsigned short chip, unsigned short block);
int nand_dbg_single_phy_erase(unsigned short chip, unsigned short block)
{
	int result = nand_physic_erase_block(chip, block);
	return result;
}

int get_nftl_read_claim_page_no(struct read_claim_node *head)
{
	struct read_claim_node *last =
		(struct read_claim_node *)get_last_nftl((struct nftl_list_node *)head);
	/* OEM does not check any pointer in this chain. */
	return last->zone->settings->page;
}

/* Private stack slots observed at FP+40/52/56 in the stock read-claim
 * search. The gap has no persistent layout or ABI significance. */
struct read_claim_scan {
	struct read_claim_node *last;
	unsigned char gap[4];
	int flag;
	struct read_claim_node *node;
};
struct read_claim_node *get_nftl_need_read_claim(struct read_claim_node *head)
{
	struct read_claim_scan local;

	local.node = head;
	local.flag = 0;
	local.last = (struct read_claim_node *)
		get_last_nftl((struct nftl_list_node *)head);
	local.flag = local.last->zone->settings->flag;
	/* OEM redundantly resets the cursor after the final-node lookup. It
	 * compares an unsigned one-byte marker to a 32-bit signed setting and
	 * does not validate any pointer in the initial lookup. */
	local.node = head;
	while (local.node) {
		if (local.node->zone->need_claim == local.flag)
			break;
		local.node = local.node->next;
	}
	return local.node;
}

struct special_exit_chip {
	struct special_exit_chip *next;
};
struct special_exit_info {
	void *before_chips;
	struct special_exit_chip *chips;
};
extern struct special_exit_info *g_nsi;

#define SPECIAL_EXIT(vendor, message) \
	int vendor##_special_exit(void) \
	{ \
		struct special_exit_chip *chip; \
		/* OEM ignores retry-exit errors and follows unchecked links. */ \
		for (chip = g_nsi->chips; chip != 0; chip = chip->next) \
			vendor##_readretry_exit((void *)chip); \
		nand_print_dbg(message); \
		return 0; \
	}

SPECIAL_EXIT(samsung, nftl_ro_lc_42)
SPECIAL_EXIT(hynix20nm, nftl_ro_lc_68)
SPECIAL_EXIT(micron, nftl_ro_lc_76)
SPECIAL_EXIT(toshiba, nftl_ro_lc_84)
SPECIAL_EXIT(hynix26nm, nftl_ro_lc_93)
SPECIAL_EXIT(hynix16nm, nftl_ro_lc_133)
SPECIAL_EXIT(sandisk, nftl_ro_lc_140)

struct sandisk_a19_exit_chip {
	struct sandisk_a19_exit_chip *next;
	unsigned char before_setting[196];
	unsigned int setting;
	unsigned char before_toggle[54];
	unsigned char toggle;
};
extern int sandisk_toggle_vendor_specific_setting(void *chip);
int sandisk_A19_special_exit(void)
{
	struct sandisk_a19_exit_chip *chip =
		(struct sandisk_a19_exit_chip *)g_nsi->chips;
	int result;

	/* OEM trusts the global and every next pointer; a cycle never ends. */
	while (chip) {
		/* OEM ignores retry-exit failures. */
		sandisk_A19_readretry_exit(chip);
		if (chip->toggle) {
			chip->setting = 0;
			result = sandisk_toggle_vendor_specific_setting(chip);
			if (result) {
				nand_print(nftl_ro_lc_102,
					   nftl_ro_func_6409);
				return result;
			}
		}
		chip = chip->next;
	}
	nand_print_dbg(nftl_ro_lc_103);
	return 0;
}

/* Isolated vendor clock-restoration C candidates. */

struct clock_channel {
	unsigned char before_index[12];
	unsigned int index;
};
struct clock_chip {
	unsigned char before_channel[272];
	struct clock_channel *channel;
};
extern char aw_ndfc;
extern unsigned int sandisk_A19_sclk0_bak, sandisk_A19_sclk1_bak;
extern unsigned int sandisk_sclk0_bak, sandisk_sclk1_bak;
extern unsigned int toshiba_sclk0_bak, toshiba_sclk1_bak;
extern int nand_set_clk(void *controller, unsigned int channel,
			unsigned int sclk0, unsigned int sclk1);

int sandisk_A19_read_retry_clock_recover(struct clock_chip *chip)
{
	/* OEM ignores clock-programming errors and reports success. */
	nand_set_clk(&aw_ndfc, chip->channel->index,
		     sandisk_A19_sclk0_bak, sandisk_A19_sclk1_bak);
	return 0;
}

int sandisk_read_retry_clock_recover(struct clock_chip *chip)
{
	/* OEM ignores clock-programming errors and reports success. */
	nand_set_clk(&aw_ndfc, chip->channel->index,
		     sandisk_sclk0_bak, sandisk_sclk1_bak);
	return 0;
}

int toshiba_read_retry_clock_recover(struct clock_chip *chip)
{
	/* OEM ignores clock-programming errors and reports success. */
	nand_set_clk(&aw_ndfc, chip->channel->index,
		     toshiba_sclk0_bak, toshiba_sclk1_bak);
	return 0;
}

extern int nand_get_clk(void *controller, unsigned int channel,
			unsigned int *sclk0, unsigned int *sclk1);
#define CLOCK_SAVE(vendor) \
	int vendor##_read_retry_clock_save(struct clock_chip *chip) \
	{ \
		/* OEM ignores both clock errors and always reports success. */ \
		nand_get_clk(&aw_ndfc, chip->channel->index, \
			     &vendor##_sclk0_bak, &vendor##_sclk1_bak); \
		nand_set_clk(&aw_ndfc, chip->channel->index, 10, 20); \
		return 0; \
	}

CLOCK_SAVE(sandisk_A19)
CLOCK_SAVE(sandisk)
CLOCK_SAVE(toshiba)

struct move_node *out_phy_block_from_invalid_page_list(struct move_zone *zone)
{
	struct invalid_node *block =
		del_block_invalid_list((struct invalid_zone *)zone);

	if (block) {
		block->next = 0;
		block->previous = 0;
		return (struct move_node *)block;
	}
	return (struct move_node *)block;
}

extern struct invalid_node *del_block_invalid_list_by_block(
	struct invalid_zone *zone, struct invalid_node *target);
struct invalid_node *out_phy_block_from_invalid_page_list_by_block(
	struct invalid_zone *zone, struct invalid_node *target)
{
	struct invalid_node *block =
		del_block_invalid_list_by_block(zone, target);

	if (block) {
		block->next = 0;
		block->previous = 0;
		return block;
	}
	return block;
}

struct count_list {
	unsigned char before_next[0x30];
	struct count_list *next;
};
struct count_zone {
	unsigned char before_list[0x190];
	struct count_list list;
};
struct count_list *del_block_count_list(struct count_zone *zone)
{
	struct count_list *list = &zone->list;

	/* OEM removes the first link without cycle or validity checks. */
	if (list->next != 0)
		list->next = list->next->next;
	return list;
}


struct garbage_collect_zone;
extern int garbage_collect(struct garbage_collect_zone *zone);
int _nftl_nand_write_logic_page(void *zone, unsigned int page, void *data)
{
	/* OEM collapses every nonzero error into one, discarding its code. */
	if (nand_write_logic_page_no_gc(zone, page, data) != 0) {
		nand_print(nftl_ro_lc_146,
			   page);
		return 1;
	}
	garbage_collect(zone);
	all_time_gc((struct gc_policy_zone *)zone);
	do_prio_gc(zone);
	return 0;
}

struct nftl_build_node {
	unsigned short number;
	unsigned char preceding[62];
	struct nftl_build_node *next;
};
struct nftl_build_owner {
	unsigned char preceding[72];
	struct nftl_build_node *first;
};
struct nftl_add_block {
	unsigned char before_partition[8];
	void *partition;
};
extern void *build_nand_partition(struct nftl_build_node *node);
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
extern char com[];
extern char *nand_fdt_get_prop_string_by_path(void *, const char *,
						const char *);
extern void nftl_get_nand_phyinfo(void *);
extern int nftl_start(struct init_nftl_owner *, struct init_nftl_config *,
		      struct init_nftl_zone **, void *, void *, unsigned int,
		      unsigned short, unsigned short, struct init_partition *,
		      unsigned int, unsigned char);

__attribute__((optimize("-fno-stack-protector")))
/* OEM forwards all 32 partition-number bits to nftl_start. */
int nftl_initialize(struct init_nftl_owner *owner, int number)
{
	struct init_partition *part = owner->partition;
	/* Stock reserves these slots but initializes only the bytes it reads.
	 * Their remaining bytes must not be treated as terminated strings. */
	char vendor[12];
	char path[8];
	char *compatible = 0;
	int result;

	__builtin_memcpy(vendor, nftl_vendor_allwinner, 9);
	path[0] = '/';
	path[1] = 0;
	compatible = nand_fdt_get_prop_string_by_path(0, path, com);
	if (!compatible)
		return -1;
	if (nand_memcmp(compatible, vendor, 9))
		return -1;
	nftl_version();
	nftl_get_nand_phyinfo(phyinfo);
	if (part->pages <= 15)
		return -1;
	owner->config = nftl_malloc(36);
	if (!owner->config) {
		nand_print(nftl_ro_lc_16, nftl_ro_func_5346);
		return 1;
	}
	/* OEM reloads both shared pointers for each field access. These reads
	 * can race with another writer; add synchronization only in a separate
	 * non-stock fix. */
	owner->config->mode = 0;
	owner->config->first = 1;
	owner->config->second = 1;
	owner->config->third = 1;
	owner->config->fourth = 0;
	owner->config->fifth = 0;
	{
		struct init_nftl_config *target = owner->config;
		target->limit_a = part->device->params->limit_a;
	}
	{
		struct init_nftl_config *target = owner->config;
		target->limit_b = part->device->params->limit_b;
	}
	owner->config->eighth = 8;
	owner->config->ninth = 2;
	owner->config->tenth = 3;
	owner->config->revision = 0x0013c680;
	if (part->device->params->retry_setting == 0) {
		owner->config->erase_limit = 30;
		owner->config->interval = 3;
	} else if (part->device->params->retry_setting == 255) {
		owner->config->mode = 1;
		owner->config->erase_limit = 50;
		owner->config->interval = 40;
	} else if (part->device->params->retry_setting > 10) {
		/* Stock changes the caller's NAND settings in place. */
		part->device->params->retry_setting = 10;
		owner->config->erase_limit = 30;
		owner->config->interval = 3;
	} else {
		{
			struct init_nftl_config *target = owner->config;
			target->erase_limit = (unsigned short)(
				part->device->params->retry_setting * 50);
		}
		{
			struct init_nftl_config *target = owner->config;
			target->interval = (unsigned short)(
				part->device->params->retry_setting * 40);
		}
	}
	{
		struct init_nftl_config *target = owner->config;
		target->device_mode = part->device->mode;
	}
	if (owner->config->device_mode)
		owner->config->interval = 3;
	else
		owner->config->interval = 1;
	if (owner->config->erase_limit > 30)
		owner->config->erase_limit = 30;
	/* OEM overwrites all interval decisions above unconditionally. */
	owner->config->interval = 1;
	/* OEM checks the upper bound first, then reloads for the lower bound. */
	if (part->device->params->format_revision <= 0x76b2b8 &&
	    part->device->params->format_revision > 0x1517f) {
		struct init_nftl_config *target = owner->config;
		target->revision = part->device->params->format_revision;
	}
	owner->config->boot_crc = 1;
	if (!nftl_support_boot_check_crc_from_dts())
		owner->config->boot_crc = 0;
	result = nftl_start(owner, owner->config, &owner->zone, part->read, part->write,
			    part->capacity, part->small, part->pages,
			    part, number, 0);
	if (result)
		return result;
	{
		struct init_nftl_geometry *target = owner->zone->geometry;
		target->max_erase_times = part->device->params->max_erase_times;
	}
	{
		struct init_nftl_geometry *target = owner->zone->geometry;
		target->secondary_erase_times =
			part->device->params->erased_block_limit;
	}
	nand_print_dbg(nftl_ro_lc_17,
		       owner->zone->geometry->max_erase_times);
	owner->capacity = nftl_get_zone_cap((struct nftl_zone_cap_view *)owner->zone);
	owner->read = (void (*)(void))_nand_read;
	owner->write = (void (*)(void))_nand_write;
	owner->discard = (void (*)(void))_nand_discard;
	owner->flush = (void (*)(void))_nand_flush_write_cache;
	owner->shutdown = (void (*)(void))_shutdown_op;
	owner->sector_read = (void (*)(void))_nand_sector_read;
	owner->sector_write = (void (*)(void))_nand_sector_write;
	owner->sector_flush = (void (*)(void))_nand_flush_sector_write_cache;
	owner->next = 0;
	return 0;
}
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
	void *block_array;
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
extern int zone_param_init(void *, unsigned short, unsigned int, unsigned int);
extern int build_zone_list_first(struct first_list_zone *, unsigned int);
extern int build_zone_list(struct build_zone *);
struct no_use_zone;
extern int is_no_use_device(struct no_use_zone *, unsigned int);
struct nftl_ops;
extern void nftl_ops_init(struct nftl_ops *);

static __attribute__((always_inline)) inline unsigned int nftl_stock_udiv(unsigned int numerator,
				    unsigned int denominator)
{
	/* AArch64 UDIV returns zero on a zero divisor; C division does not. */
	return denominator ? numerator / denominator : 0;
}

int nftl_start(struct init_nftl_owner *owner, struct init_nftl_config *config,
		struct init_nftl_zone **output, void *start_bytes,
		void *capacity_bytes, unsigned int area_bytes,
		unsigned short erase_bytes, unsigned short chips,
		struct init_partition *partition, unsigned int number,
		unsigned char mode)
{
	struct start_zone *zone;
	struct start_geometry *geometry;
	struct start_physical_block *blocks;
	unsigned int start = (unsigned long)start_bytes >> 9;
	unsigned int capacity = (unsigned long)capacity_bytes >> 9;
	unsigned int area = area_bytes >> 9;
	unsigned int erase = erase_bytes >> 9;
	unsigned int total, reserved, i, attempts = 0, result;

	zone = nftl_malloc(3528);
	*output = (struct init_nftl_zone *)zone;
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
		geometry = nftl_malloc(40);
		zone->geometry = geometry;
		if (!geometry) {
			nand_print(nftl_ro_lc_475, nftl_ro_func_8536);
			return -2;
		}
		nand_memset(geometry, 0, 8);
		/* Stock follows the global NAND-info pointers unchecked. */
		nand_memcpy(geometry->vendor_id,
			    (char *)g_nsi->chips + 16, 8);
		geometry->page_bytes = (unsigned short)(erase << 9);
		geometry->chip_count = chips;
		total = nftl_stock_udiv(capacity, area);
		if (config->limit_a >= total)
			return -1;
		geometry->blocks = total;
		geometry->pages = nftl_stock_udiv(area, erase);
		geometry->sectors_per_page = erase;
		geometry->sectors = geometry->sectors_per_page;
		geometry->cache_limit = 3000;
		blocks = nftl_malloc((unsigned int)((total + 1) << 6));
		geometry->block_array = blocks;
		if (!blocks) {
			nand_print(nftl_ro_lc_476,
				   nftl_ro_func_8536);
			return -2;
		}
		/* The last sentinel block is not covered by this fill. */
		nand_memset(blocks, -1, total << 6);
		for (i = 0; i < total + 1; i++) {
			blocks[i].number = i;
			blocks[i].page = 0;
			blocks[i].state = 0;
			blocks[i].tag = ~0U;
			blocks[i].erase_count = 0;
			blocks[i].links[0] = blocks[i].links[1] =
				blocks[i].links[2] = blocks[i].links[3] =
				blocks[i].links[4] = blocks[i].links[5] = 0;
		}
		nftl_ops_init((struct nftl_ops *)zone);
		reserved = nftl_stock_udiv(start, area);
		zone->backup_blocks = (unsigned short)
			((unsigned short)total - (unsigned short)reserved);
		if (zone->backup_blocks < config->limit_a) {
			nand_print(nftl_ro_lc_477,
				   zone->backup_blocks);
			return -2;
		}
		nand_print_dbg(nftl_ro_lc_478, total,
			       zone->backup_blocks);
		result = zone_param_init(zone, 0,
			(total - zone->backup_blocks) * area,
			zone->backup_blocks * area);
		if (result) {
			nand_print(nftl_ro_lc_479);
			return result;
		}
		/* The stock frame loads stack argument nine, the NFTL number;
		 * the final mode byte is never used. */
		zone->mode = number;
		if (config->device_mode)
			zone->flags |= 1;
		if (config->fifth &&
		    is_no_use_device((struct no_use_zone *)zone, total) == 1) {
			result = build_zone_list_first(
				(struct first_list_zone *)zone, total);
			if (result) {
				nand_print(nftl_ro_lc_480);
				return result;
			}
		}
		result = build_zone_list((struct build_zone *)zone);
		if (!result) {
			/* OEM ignores cache-initialization failures. */
			nftl_cache_init(zone);
			return 0;
		}
		nand_print(nftl_ro_lc_481);
		if (attempts > 1)
			return result;
		nand_print(nftl_ro_lc_482);
	}
}
extern int panic_recover_data(void *block);
struct nftl_build_node *nftl_add(struct nftl_build_node *node)
{
	struct nftl_add_block *block = nftl_malloc(136);

	if (!block) {
		nand_print(nftl_ro_lc_4, nftl_ro_func_5194);
		return 0;
	}
	block->partition = build_nand_partition(node);
	if (nftl_initialize((struct init_nftl_owner *)block, node->number)) {
		/* OEM leaks the allocated block on initialization failure. */
		nand_print(nftl_ro_lc_5);
		return 0;
	}
	add_nftl_blk_list((struct nftl_chain *)&nftl_blk_head,
			  (struct nftl_chain *)block);
	/* OEM ignores panic recovery failure after publishing the new block. */
	panic_recover_data(block);
	nand_print_dbg(nftl_ro_lc_6);
	return (struct nftl_build_node *)block;
}
int nftl_build_all(struct nftl_build_owner *owner)
{
	struct nftl_build_node *node = owner->first;

	nftl_blk_head.next = 0;
	while (node) {
		if (!nftl_add(node)) {
			/* OEM discards the underlying add failure. */
			nand_print(nftl_ro_lc_3);
			return 1;
		}
		node = node->next;
	}
	return 0;
}
int nftl_build_one(struct nftl_build_owner *owner, int number)
{
	struct nftl_build_node *node = owner->first;

	if (!number)
		nftl_blk_head.next = 0;
	while (node) {
		/* Compare all 32 request bits. The explicit promotion preserves
		 * stock's operand order; negative requests remain unmatched. */
		if ((unsigned int)node->number == number && !nftl_add(node)) {
			/* OEM discards the underlying add failure. */
			nand_print(nftl_ro_lc_3);
			return 1;
		}
		node = node->next;
	}
	return 0;
}

struct nftl_ops {
	unsigned char before[3456];
	void (*erase)(void);
	void (*read)(void);
	void (*write)(void);
	void (*copy)(void);
	void (*good)(void);
	void (*bad)(void);
	void (*write_logic)(void);
	void (*read_logic)(void);
	void (*discard)(void);
};
struct copy_page_geometry {
	unsigned char before_pages[28];
	unsigned short pages;
};
struct copy_page_zone {
	unsigned char before_geometry[8];
	struct copy_page_geometry *geometry;
	unsigned char before_callbacks[3448];
	int (*read)(struct copy_page_zone *, struct physic_op_param *);
	int (*write)(struct copy_page_zone *, struct physic_op_param *);
};
struct copy_page_source {
	unsigned short block;
	unsigned char before_logic_page[6];
	/* OEM call setup treats this as signed; forwarding preserves all 32 bits. */
	int logic_page;
};
struct copy_page_destination {
	unsigned short block;
	unsigned char before_next_page[4];
	unsigned short next_page;
};
__attribute__((optimize("-fno-stack-protector")))
int _nftl_nand_copy_page(struct copy_page_zone *zone,
			struct copy_page_source *source,
			struct copy_page_destination *destination,
			void *data, unsigned short plane)
{
	unsigned char oob[16];
	struct physic_op_param source_op;
	struct physic_op_param destination_op;
	int write_result;
	unsigned int special;
	unsigned int result;
	int read_result;

	set_physic_op_par(&source_op, source->block, plane, 0, 0, 0);
	set_physic_op_par(&destination_op, destination->block,
			  plane, 0, 0, 0);
	result = 0;
	source_op.page = zone->geometry->pages;
	destination_op.page = zone->geometry->pages;
	source_op.data = data;
	destination_op.data = data;
	source_op.oob = oob;
	destination_op.oob = oob;
	read_result = zone->read(zone, &source_op);
	/* OEM folds ECC_LIMIT (10) into success. It also writes the page after
	 * every other read error, using whatever OOB/data the callback left. */
	if (read_result == 10)
		read_result = 0;
	special = get_special_data_from_oob(oob);
	set_oob_special_page(zone, oob, special,
			     source->logic_page,
			     (unsigned short)(destination->next_page + 1));
	write_result = zone->write(zone, &destination_op);
	/* OEM ORs the full signed read result into the shifted write result.
	 * Negative read errors can mask the write status entirely. */
	result = ((unsigned int)write_result << 8) | read_result;
	return result;
}
struct logic_read_zone;
extern int _nftl_nand_read_logic_page(struct logic_read_zone *,
				      unsigned int, void *);
void nftl_ops_init(struct nftl_ops *ops)
{
	/* OEM never clears other slots or checks the pointer. */
	ops->erase = (void (*)(void))_nftl_nand_erase_superblk;
	ops->read = (void (*)(void))_nftl_nand_read_page;
	ops->write = (void (*)(void))_nftl_nand_write_page;
	ops->good = (void (*)(void))_nftl_nand_is_blk_good;
	ops->bad = (void (*)(void))_nftl_nand_mark_bad_blk;
	ops->copy = (void (*)(void))_nftl_nand_copy_page;
	ops->read_logic = (void (*)(void))_nftl_nand_read_logic_page;
	ops->write_logic = (void (*)(void))_nftl_nand_write_logic_page;
	ops->discard = (void (*)(void))_nftl_discard_logic_page;
}

extern int add_block_invalid_list(struct invalid_zone *zone,
				  struct invalid_node *block);
int put_phy_block_to_invalid_page_list(void *zone,
				       struct invalid_node *block)
{
	int result;

	/* OEM clears both links before checking whether insertion succeeds. */
	block->next = 0;
	block->previous = 0;
	result = add_block_invalid_list(zone, block);
	if (result != 0)
		nand_print_dbg(nftl_ro_lc_410);
	return result;
}

struct panic_header {
	unsigned char unknown[4];
	unsigned char index;
};
struct panic_block {
	unsigned short block;
};
extern void *panic_info[5];
extern int is_panic_enable(void);
struct panic_backup_geometry {
	unsigned char before_pages[20];
	unsigned short pages;
};
struct panic_backup_zone {
	unsigned char before_geometry[8];
	struct panic_backup_geometry *geometry;
};
struct free_block;
struct free_block_zone;
extern struct free_block *out_phy_block_from_free_list(struct free_block_zone *zone);
int panic_backup_block(struct build_zone *zone)
{
	/* OEM keeps geometry at FP+0x28, block at +0x30 and index at +0x3c.
	 * A separate-locals declaration lets GCC reuse different slots across
	 * the allocator and printer calls. Preserve the private frame layout. */
	struct {
		struct panic_backup_geometry *geometry;
		struct physical_block *block;
		unsigned char gap[4];
		int index;
	} local;

	if (!is_panic_enable())
		return 0;
	for (local.index = 0; local.index <= 2; local.index++) {
		local.block = panic_info[local.index];
		/* OEM returns immediately on the first existing reservation,
		 * even if a later slot is empty. Preserve this early exit. */
		if (local.block) {
			nand_print(nftl_ro_lc_19,
				   local.block->number);
			return 0;
		}
		local.block = (struct physical_block *)
			out_phy_block_from_free_list((struct free_block_zone *)zone);
		if (local.block) {
			nand_print(nftl_ro_lc_20,
				   local.block->number);
			panic_info[local.index] = local.block;
		} else {
			nand_print(nftl_ro_lc_21);
			return -1;
		}
	}
	local.geometry = ((struct panic_backup_zone *)zone)->geometry;
	/* OEM stores a 32-bit page count over the fourth pointer slot. */
	*(unsigned int *)&panic_info[3] = local.geometry->pages * 3;
	return 0;
}
int panic_recover_info(struct panic_header *header, struct panic_block *block)
{
	/* OEM follows both pointers unchecked and rejects indices 3 and 4\n	 * despite reserving five pointer slots. */
	unsigned int index = ((unsigned char *)header + 4)[0];

	if (index <= 2) {
		nand_print(nftl_ro_lc_26, block->block);
		panic_info[index] = block;
		return 0;
	}
	nand_print(nftl_ro_lc_27, index);
	return -1;
}

union panic_recover_state {
	void *slots[5];
	struct {
		void *blocks[3];
		unsigned int reserved_pages;
		/* OEM zero-extends slot indices but signed-tests the erase exit. */
		unsigned int block_index;
		unsigned int page_index;
		unsigned int unused;
	};
};
struct erase_block_zone;
struct erase_block_info;
extern int erase_block(struct erase_block_zone *, struct erase_block_info *,
		       unsigned short);
struct panic_recover_block {
	unsigned short number;
	unsigned char before_erase_count[4];
	unsigned short erase_count;
};
struct panic_recover_owner {
	unsigned char before_zone[24];
	struct panic_recover_zone *zone;
};
struct panic_recover_geometry {
	unsigned char before_pages[20];
	unsigned short pages;
	unsigned char before_page_bytes[4];
	unsigned short page_bytes;
	unsigned short fixed_page;
};
struct panic_recover_zone {
	unsigned char before_geometry[8];
	struct panic_recover_geometry *geometry;
	unsigned char before_read[3448];
	int (*read)(struct panic_recover_zone *, struct physic_op_param *);
};
_Static_assert(__builtin_offsetof(union panic_recover_state, block_index) == 28,
	       "panic recovery block index offset");
_Static_assert(__builtin_offsetof(union panic_recover_state, page_index) == 32,
	       "panic recovery page index offset");
_Static_assert(__builtin_offsetof(struct panic_recover_zone, read) == 3464,
	       "panic recovery read offset");

__attribute__((optimize("-fno-stack-protector")))
int panic_recover_data(void *owner)
{
	struct panic_recover_zone *zone =
		((struct panic_recover_owner *)owner)->zone;
	struct panic_recover_geometry *geometry = zone->geometry;
	union panic_recover_state *state =
		(union panic_recover_state *)panic_info;
	struct panic_recover_block *block;
	struct physic_op_param op;
	unsigned char oob[16];
	unsigned char *data = 0;
	unsigned int logic_page;
	unsigned int sectors;

	if (!state->blocks[0]) {
		nand_print(nftl_ro_lc_28);
		return 0;
	}
	nand_print(nftl_ro_lc_29);
	data = nand_malloc(geometry->page_bytes);
	if (!data) {
		nand_print(nftl_ro_lc_30, nftl_ro_func_5524, 308);
		return -1;
	}
	nand_memset(data, 0, geometry->page_bytes);
	state->block_index = 0;
	while ((unsigned int)state->block_index <= 2) {
		block = state->slots[state->block_index];
		state->page_index = 0;
		while (state->page_index < geometry->pages) {
			set_physic_op_par(&op, block->number,
					  state->page_index,
					  geometry->fixed_page, data, oob);
			/* OEM ignores the page-read result entirely. */
			zone->read(zone, &op);
			if (is_panic_dummy_page(oob)) {
				nand_print(nftl_ro_lc_31,
					   block->number, state->page_index);
			} else if (!is_panic_data_page(oob)) {
				if (!state->block_index && !state->page_index) {
					nand_print(nftl_ro_lc_32,
						   block->number, state->page_index);
					goto done;
				}
				nand_print(nftl_ro_lc_33,
					   block->number, state->page_index);
				goto erase;
			} else {
				logic_page = get_spare_data(oob, 7, 4);
				sectors = get_spare_data(oob, 11, 4);
				nand_print(nftl_ro_lc_34,
					   block->number, state->page_index, logic_page);
				/* OEM discards the replay write result. */
				nftl_write(logic_page, sectors, data);
			}
			state->page_index++;
		}
		state->block_index++;
	}
erase:
	while ((int)state->block_index >= 0) {
		/* A complete three-block scan arrives here with index three.
		 * OEM then treats bookkeeping slot three as a block pointer and
		 * can fault. Preserve the bad access for binary equivalence. */
		block = state->slots[state->block_index];
		erase_block((struct erase_block_zone *)zone,
			    (struct erase_block_info *)block,
			    (unsigned short)(block->erase_count + 1));
		state->block_index--;
	}
done:
	nand_free(data);
	return 0;
}

/* Keep this address in the stock-ordered table span. The vendor bus helper
 * trusts its count, so bytes beyond the declared array can be observed. */
extern unsigned char toshiba_read_retry_start_cmd[2];
int toshiba_vender_pre_condition(void *chip)
{
	int result;

	result = set_cmd_with_nand_bus(chip, toshiba_read_retry_start_cmd,
				       0, 0, 0, 0, 2);
	if (result != 0) {
		nand_print(nftl_ro_lc_78);
		return result;
	}
	return 0;
}

/* As above, the command byte must keep its stock table neighbors. */
extern unsigned char sandisk_A19_read_retry_cmd_1y;
int sandisk_A19_vender_set_param(void *chip, void *value, void *address,
				 int unused)
{
	int result = 0;

	/* OEM ignores the fourth argument and does no command error recovery. */
	result = set_cmd_with_nand_bus(chip, &sandisk_A19_read_retry_cmd_1y,
				       1, address, value, 4, 1);
	return result;
}

struct erase_block_geometry {
	unsigned char before_pages[20];
	unsigned short pages;
	unsigned char before_fixed_page[6];
	unsigned short fixed_page;
};
struct erase_block_info {
	unsigned short number;
	unsigned short state;
	unsigned short pages;
	unsigned short erase_count;
	unsigned int tag;
};
struct erase_block_zone {
	unsigned char before_geometry[8];
	struct erase_block_geometry *geometry;
	unsigned char before_bad_count[12];
	unsigned short bad_count;
	unsigned char before_erase[3426];
	int (*erase)(struct erase_block_zone *, struct physic_op_param *);
	unsigned char before_mark_bad[32];
	int (*mark_bad)(struct erase_block_zone *, struct physic_op_param *);
};
struct mark_free_zone {
	unsigned char before_geometry[8];
	struct erase_block_geometry *geometry;
	unsigned char before_page_data[512 - 16];
	void *page_data;
	unsigned char before_write[3472 - 520];
	int (*write)(struct mark_free_zone *, struct physic_op_param *);
};
__attribute__((optimize("-fno-stack-protector")))
int mark_free_block(struct erase_block_zone *owner,
		    struct erase_block_info *block)
{
	/* A cast expression keeps the OEM's fresh owner-field reads without
	 * introducing a separate stack-resident view pointer at -O0. */
#define MARK_ZONE ((struct mark_free_zone *)owner)
	struct physic_op_param op;
	unsigned char oob[16];

	nand_memset(oob, -91, 16);
	/* OEM rereads geometry, block metadata and callback pointers for each
	 * of four independent writes. A helper may change them between calls.
	 * Page counts below two wrap; helper/write errors are ignored. */
	set_physic_op_par(&op, block->number, 0, MARK_ZONE->geometry->fixed_page,
			   MARK_ZONE->page_data, oob);
	set_oob_special_page(MARK_ZONE, oob, 0x55555555,
			     block->tag, block->erase_count);
	MARK_ZONE->write(MARK_ZONE, &op);

	set_physic_op_par(&op, block->number, 1, MARK_ZONE->geometry->fixed_page,
			   MARK_ZONE->page_data, oob);
	set_oob_special_page(MARK_ZONE, oob, 0x55555555,
			     block->tag, block->erase_count);
	MARK_ZONE->write(MARK_ZONE, &op);

	set_physic_op_par(&op, block->number, MARK_ZONE->geometry->pages - 2,
			   MARK_ZONE->geometry->fixed_page, MARK_ZONE->page_data, oob);
	set_oob_special_page(MARK_ZONE, oob, 0x55555555,
			     block->tag, block->erase_count);
	MARK_ZONE->write(MARK_ZONE, &op);

	set_physic_op_par(&op, block->number, MARK_ZONE->geometry->pages - 1,
			   MARK_ZONE->geometry->fixed_page, MARK_ZONE->page_data, oob);
	set_oob_special_page(MARK_ZONE, oob, 0x55555555,
			     block->tag, block->erase_count);
	MARK_ZONE->write(MARK_ZONE, &op);
#undef MARK_ZONE
	return 0;
}
extern int mark_free_block(struct erase_block_zone *, struct erase_block_info *);
int erase_block(struct erase_block_zone *zone, struct erase_block_info *block,
		unsigned short mode)
{
	/* Match the OEM's operation/result slots within its 0x50-byte frame.
	 * The padding and op.reserved are left untouched as stock does. */
	struct {
		struct physic_op_param op;
		unsigned char gap[4];
		int result;
	} local;

	nand_print(nftl_ro_lc_299, block->number);
	set_physic_op_par(&local.op, block->number, 0, zone->geometry->fixed_page,
			  0, 0);
	local.result = zone->erase(zone, &local.op);
	if (local.result) {
		nand_print(nftl_ro_lc_300,
			   block->number);
		/* OEM ignores the bad-block callback result and returns the
		 * original erase status after updating the metadata. */
		zone->mark_bad(zone, &local.op);
		block->pages = 0;
		block->state = 255;
		zone->bad_count++;
	} else {
		block->erase_count = mode;
		block->tag = -1;
		put_phy_block_to_free_list((struct move_zone *)zone,
					   (struct move_node *)block);
		mark_free_block(zone, block);
		block->pages = zone->geometry->pages;
		block->state = 0;
	}
	return local.result;
}
struct torture_geometry {
	unsigned char before_pages[20];
	unsigned short pages;
	unsigned char before_page_bytes[4];
	unsigned short page_bytes;
	unsigned short fixed_page;
};
struct torture_zone {
	unsigned char before_geometry[8];
	struct torture_geometry *geometry;
	unsigned char before_bad_count[12];
	unsigned short bad_count;
	unsigned char before_page_data[482];
	void *page_data;
	unsigned char before_callbacks[2936];
	int (*erase)(struct torture_zone *, struct physic_op_param *);
	int (*read)(struct torture_zone *, struct physic_op_param *);
	int (*write)(struct torture_zone *, struct physic_op_param *);
	unsigned char before_mark_bad[16];
	int (*mark_bad)(struct torture_zone *, struct physic_op_param *);
};
extern unsigned char patterns[1];
extern void *memset(void *, int, unsigned long);
_Static_assert(__builtin_offsetof(struct torture_zone, page_data) == 512,
	       "torture page-data offset");
_Static_assert(__builtin_offsetof(struct torture_zone, erase) == 3456,
	       "torture erase callback offset");
_Static_assert(__builtin_offsetof(struct torture_zone, mark_bad) == 3496,
	       "torture bad-block callback offset");
__attribute__((optimize("-fno-stack-protector")))
int torture_block_and_mark(struct torture_zone *zone,
			   struct erase_block_info *block)
{
	struct physic_op_param op;
	unsigned char oob[15] = {0};
	unsigned int page;
	int result;

	nand_print(nftl_ro_lc_301, block->number);
	set_physic_op_par(&op, block->number, 0,
			  zone->geometry->fixed_page, 0, 0);
	result = zone->erase(zone, &op);
	if (result) {
		nand_print(nftl_ro_lc_300, block->number);
		goto bad;
	}
	block->erase_count++;
	/* OEM's initialized table has one pattern and tests only three pages,
	 * regardless of the advertised geometry. Reload the writable table
	 * around callbacks as the stock object does. */
	memset(zone->page_data, patterns[0], zone->geometry->page_bytes);
	memset(oob, patterns[0], 15);
	for (page = 0; page < 3; page++) {
		set_physic_op_par(&op, block->number, (unsigned short)page,
				  zone->geometry->fixed_page,
				  zone->page_data, oob);
		result = zone->write(zone, &op);
		if (result) {
			nand_print(nftl_ro_lc_302,
				   nftl_ro_func_7402, block->number, page);
			goto bad;
		}
	}
	for (page = 0; page < 3; page++) {
		memset(zone->page_data, 0, zone->geometry->page_bytes);
		memset(oob, 0, 15);
		set_physic_op_par(&op, block->number, (unsigned short)page,
				  zone->geometry->fixed_page,
				  zone->page_data, oob);
		result = zone->read(zone, &op);
		if (result) {
			nand_print(nftl_ro_lc_303,
				   nftl_ro_func_7402, block->number, page);
			goto bad;
		}
		result = nftl_check_pattern(zone->page_data, patterns[0],
					    zone->geometry->page_bytes);
		if (result) {
			nand_print(nftl_ro_lc_304,
				   nftl_ro_func_7402, block->number, page, patterns[0]);
			goto bad;
		}
		result = nftl_check_pattern(oob, patterns[0], 15);
		if (result) {
			nand_print(nftl_ro_lc_305,
				   nftl_ro_func_7402, block->number, page, patterns[0]);
			goto bad;
		}
	}
	set_physic_op_par(&op, block->number, 0,
			  zone->geometry->fixed_page, 0, 0);
	result = zone->erase(zone, &op);
	if (result) {
		nand_print(nftl_ro_lc_306, block->number);
		goto bad;
	}
	block->erase_count++;
	block->tag = -1;
	/* OEM ignores both free-list and marking results. */
	put_phy_block_to_free_list((struct move_zone *)zone,
				   (struct move_node *)block);
	mark_free_block((struct erase_block_zone *)zone, block);
	block->pages = zone->geometry->pages;
	block->state = 0;
	return 0;
bad:
	/* OEM ignores the bad-block callback status and returns the preceding
	 * erase, I/O, or comparison status. */
	zone->mark_bad(zone, &op);
	block->pages = 0;
	block->state = 255;
	zone->bad_count++;
	return result;
}
int nand_dbg_zone_erase(void *zone, unsigned short block,
			unsigned short mode)
{
	void *physical = get_phy_block_addr((struct nftl_phy_outer *)zone, block);
	int result;

	physical = out_phy_block_from_free_list_by_block(
		(struct nftl_free_erase_owner *)zone,
		(struct nftl_free_erase_node *)physical);
	/* OEM passes a null removal result directly to erase_block. */
	result = erase_block((struct erase_block_zone *)zone,
			     (struct erase_block_info *)physical, mode);
	return result;
}

/* The offsets and widths in these diagnostic views are recovered from the
 * stock instructions.  Only fields read by print_nftl_zone are declared;
 * the intervening bytes belong to other private NFTL state. */
struct zone_diag_chip {
	unsigned char unknown_0[16];
	unsigned short blocks_per_chip;
	unsigned char unknown_18[2];
	unsigned short pages_per_block;
	unsigned char unknown_22[2];
	unsigned short spare_bytes;
	unsigned short page_bytes;
	unsigned char unknown_28[2];
	unsigned short total_blocks;
};
struct zone_diag_block {
	unsigned short number;
	unsigned char unknown_2[4];
	unsigned short second_number;
	unsigned int erase_count;
};
struct zone_diag_counters {
	unsigned short field[14];
	unsigned char unknown_28[4];
	unsigned int total;
};
struct zone_diag_blob { unsigned char bytes[48]; };
struct zone_diag_view {
	unsigned char unknown_0[8];
	const struct zone_diag_chip *chip;
	unsigned char unknown_16[8];
	unsigned char block_flag;
	unsigned char current_block_flag;
	unsigned short logic_blocks;
	unsigned short free_blocks;
	unsigned short bad_blocks;
	unsigned int sectors;
	unsigned char unknown_36[4];
	unsigned int backup_sectors;
	unsigned char unknown_44[4];
	const struct zone_diag_block *first_block;
	const struct zone_diag_block *last_block;
	const struct zone_diag_block *gc_block;
	unsigned char unknown_72[392];
	struct zone_diag_blob zone_phy_page_map_for_gc;
	void *logic_page_map;
	unsigned char unknown_520[8];
	const struct zone_diag_block *current_block;
	struct zone_diag_blob current_block_user_info;
	unsigned short current_block_page;
	unsigned char unknown_586[6];
	const struct zone_diag_block *assist_block;
	struct zone_diag_blob assist_block_user_info;
	unsigned short assist_block_page;
	unsigned char unknown_650[6];
	unsigned int initialized;
	unsigned int block_count;
	unsigned char unknown_664[4];
	unsigned int first_sector;
	unsigned int last_sector;
	unsigned int reclaim_count;
	unsigned short free_count;
	unsigned short bad_count;
	unsigned short gc_count;
	unsigned char unknown_686[2];
	unsigned short reserved_count;
	unsigned char unknown_690[8];
	unsigned short full_count;
	unsigned char unknown_700[372];
	unsigned short smart_count;
	unsigned char unknown_1074[2374];
	const struct zone_diag_counters *counters;
};
/* These anchors also document the ABI consumed by the diagnostic code. */
#define DIAG_OFFSET_ASSERT(field, offset) \
	typedef char zone_diag_##field##_offset[ \
		__builtin_offsetof(struct zone_diag_view, field) == (offset) ? 1 : -1]
DIAG_OFFSET_ASSERT(chip, 8);
DIAG_OFFSET_ASSERT(zone_phy_page_map_for_gc, 464);
DIAG_OFFSET_ASSERT(logic_page_map, 512);
DIAG_OFFSET_ASSERT(current_block_user_info, 536);
DIAG_OFFSET_ASSERT(assist_block_user_info, 600);
DIAG_OFFSET_ASSERT(initialized, 656);
DIAG_OFFSET_ASSERT(smart_count, 1072);
DIAG_OFFSET_ASSERT(counters, 3448);
#undef DIAG_OFFSET_ASSERT
/* OEM bugs: these pointers are dereferenced without null checks; the 48-byte
 * blobs are passed to a %x formatter by address, not decoded as integers. */
#define DIAG_ZONE ((const struct zone_diag_view *)zone)
__attribute__((optimize("-fno-stack-protector")))
void print_nftl_zone(void *zone)
{
	if (!DIAG_ZONE->initialized)
		return;
	nand_print_dbg(nftl_ro_lc_329);
	nand_print_dbg(nftl_ro_lc_330, DIAG_ZONE->chip->spare_bytes);
	nand_print_dbg(nftl_ro_lc_331, DIAG_ZONE->chip->page_bytes);
	nand_print_dbg(nftl_ro_lc_332, DIAG_ZONE->chip->pages_per_block);
	nand_print_dbg(nftl_ro_lc_333, DIAG_ZONE->chip->blocks_per_chip);
	nand_print_dbg(nftl_ro_lc_334, DIAG_ZONE->chip->total_blocks);
	nand_print_dbg(nftl_ro_lc_335, DIAG_ZONE->initialized);
	nand_print_dbg(nftl_ro_lc_336, DIAG_ZONE->block_flag);
	nand_print_dbg(nftl_ro_lc_337, DIAG_ZONE->current_block_flag);
	nand_print_dbg(nftl_ro_lc_338, DIAG_ZONE->logic_blocks);
	nand_print_dbg(nftl_ro_lc_339, DIAG_ZONE->free_blocks);
	nand_print_dbg(nftl_ro_lc_340, DIAG_ZONE->sectors);
	nand_print_dbg(nftl_ro_lc_341, DIAG_ZONE->backup_sectors);
	nand_print_dbg(nftl_ro_lc_342, DIAG_ZONE->bad_blocks);
	nand_print_dbg(nftl_ro_lc_343, DIAG_ZONE->free_count);
	nand_print_dbg(nftl_ro_lc_344, DIAG_ZONE->bad_count);
	nand_print_dbg(nftl_ro_lc_345, DIAG_ZONE->reserved_count);
	nand_print_dbg(nftl_ro_lc_346, DIAG_ZONE->gc_count);
	nand_print_dbg(nftl_ro_lc_347, DIAG_ZONE->full_count);
	nand_print_dbg(nftl_ro_lc_348, DIAG_ZONE->first_block->number);
	nand_print_dbg(nftl_ro_lc_349, DIAG_ZONE->last_block->number);
	/* Stock computes the address from the following map pointer field. */
	nand_print_dbg(nftl_ro_lc_350,
		*(const struct zone_diag_blob *)
		((const char *)&DIAG_ZONE->logic_page_map - sizeof(struct zone_diag_blob)));
	nand_print_dbg(nftl_ro_lc_351,
		*(const struct zone_diag_blob *)
		((const char *)&DIAG_ZONE->logic_page_map + 24));
	nand_print_dbg(nftl_ro_lc_352, DIAG_ZONE->current_block);
	nand_print_dbg(nftl_ro_lc_353, DIAG_ZONE->current_block->number);
	nand_print_dbg(nftl_ro_lc_354, DIAG_ZONE->current_block_page);
	nand_print_dbg(nftl_ro_lc_355, DIAG_ZONE->current_block->erase_count);
	nand_print_dbg(nftl_ro_lc_356, DIAG_ZONE->gc_block->number, DIAG_ZONE->gc_block->second_number);
	if (DIAG_ZONE->current_block_flag & 1) {
		nand_print_dbg(nftl_ro_lc_357,
			*(const struct zone_diag_blob *)
			((const char *)&DIAG_ZONE->logic_page_map + 88));
		nand_print_dbg(nftl_ro_lc_358, DIAG_ZONE->assist_block);
		nand_print_dbg(nftl_ro_lc_359, DIAG_ZONE->assist_block->number);
		nand_print_dbg(nftl_ro_lc_360, DIAG_ZONE->assist_block_page);
		nand_print_dbg(nftl_ro_lc_361, DIAG_ZONE->assist_block->erase_count);
	}
	nand_print_dbg(nftl_ro_lc_362, DIAG_ZONE->block_count);
	nand_print_dbg(nftl_ro_lc_363, DIAG_ZONE->logic_page_map);
	nand_print_dbg(nftl_ro_lc_364, DIAG_ZONE->reclaim_count);
	nand_print_dbg(nftl_ro_lc_365, DIAG_ZONE->smart_count);
	nand_print_dbg(nftl_ro_lc_366, DIAG_ZONE->first_sector);
	nand_print_dbg(nftl_ro_lc_367, DIAG_ZONE->last_sector);
	nand_print_dbg(nftl_ro_lc_368, DIAG_ZONE->counters->field[0]);
	nand_print_dbg(nftl_ro_lc_369, DIAG_ZONE->counters->field[1]);
	nand_print_dbg(nftl_ro_lc_370, DIAG_ZONE->counters->field[2]);
	nand_print_dbg(nftl_ro_lc_371, DIAG_ZONE->counters->field[3]);
	nand_print_dbg(nftl_ro_lc_372, DIAG_ZONE->counters->field[5]);
	nand_print_dbg(nftl_ro_lc_373, DIAG_ZONE->counters->field[6]);
	nand_print_dbg(nftl_ro_lc_374, DIAG_ZONE->counters->field[7]);
	nand_print_dbg(nftl_ro_lc_375, DIAG_ZONE->counters->field[8]);
	nand_print_dbg(nftl_ro_lc_376, DIAG_ZONE->counters->field[9]);
	nand_print_dbg(nftl_ro_lc_377, DIAG_ZONE->counters->field[10]);
	nand_print_dbg(nftl_ro_lc_378, DIAG_ZONE->counters->field[11]);
	nand_print_dbg(nftl_ro_lc_379, DIAG_ZONE->counters->field[12]);
	nand_print_dbg(nftl_ro_lc_380, DIAG_ZONE->counters->field[13]);
	nand_print_dbg(nftl_ro_lc_381, DIAG_ZONE->counters->total);
	print_smart((struct smart_info_zone *)zone);
}
#undef DIAG_ZONE

/* The OEM's private zone layout is not public. These typed accessors keep
 * each recovered offset visible until the full declaration is established. */
#define INIT_U8(p, off) (*(unsigned char *)((char *)(p) + (off)))
#define INIT_U16(p, off) (*(unsigned short *)((char *)(p) + (off)))
#define INIT_U32(p, off) (*(unsigned int *)((char *)(p) + (off)))
#define INIT_PTR(p, off) (*(void **)((char *)(p) + (off)))
struct logic_page_map_zone;
extern int malloc_logic_page_map(struct logic_page_map_zone *, unsigned int);
int zone_param_init(void *zone, unsigned short first_block,
		    unsigned int sectors, unsigned int backup_sectors)
{
	char *p = zone;
	char *chip = INIT_PTR(p, 8);
	unsigned int map_bytes;
	unsigned int allocation_bytes;
	unsigned int index;
	void *buffer;

	/* OEM returns from each failure without freeing earlier allocations. */
	buffer = nftl_malloc(INIT_U16(chip, 26));
	INIT_PTR(p, 512) = buffer;
	if (!buffer) {
		nand_print(nftl_ro_lc_202, nftl_ro_func_7098);
		return -2;
	}
	buffer = nftl_malloc(INIT_U16(chip, 26));
	INIT_PTR(p, 520) = buffer;
	if (!buffer) {
		nand_print(nftl_ro_lc_203, nftl_ro_func_7098);
		return -2;
	}
	INIT_U32(p, 32) = sectors;
	INIT_U32(p, 40) = backup_sectors;
	INIT_U16(p, 44) = first_block;
	INIT_PTR(p, 48) = get_phy_block_addr((struct nftl_phy_outer *)zone,
					     first_block);
	INIT_U16(p, 26) = 0;
	INIT_U16(p, 28) = 0;
	INIT_U16(p, 30) = 0;
	INIT_PTR(p, 56) = 0;
	/* Stock performs an unchecked division by the chip's sector factor. */
	if (malloc_logic_page_map((struct logic_page_map_zone *)zone,
				  sectors / INIT_U8(chip, 8))) {
		nand_print(nftl_ro_lc_204);
		return -2;
	}
	map_bytes = (unsigned int)INIT_U16(chip, 20) << 2;
	allocation_bytes = map_bytes + 320;
	INIT_U32(p, 560) = map_bytes;
	INIT_U32(p, 576) = 320;
	INIT_U32(p, 544) = allocation_bytes;
	buffer = nftl_malloc(allocation_bytes);
	INIT_PTR(p, 536) = buffer;
	if (!buffer) {
		nand_print(nftl_ro_lc_205,
			   nftl_ro_func_7098);
		return -2;
	}
	INIT_PTR(p, 552) = buffer;
	INIT_PTR(p, 568) = (char *)buffer + map_bytes;
	INIT_PTR(p, 3440) = INIT_PTR(p, 568);
	nand_memset(buffer, -1, allocation_bytes);
	INIT_U32(p, 624) = map_bytes;
	INIT_U32(p, 608) = allocation_bytes;
	buffer = nftl_malloc(allocation_bytes);
	INIT_PTR(p, 600) = buffer;
	if (!buffer) {
		nand_print(nftl_ro_lc_206,
			   nftl_ro_func_7098);
		return -2;
	}
	INIT_PTR(p, 616) = buffer;
	INIT_PTR(p, 632) = (char *)buffer + map_bytes;
	nand_memset(buffer, -1, allocation_bytes);
	INIT_U32(p, 488) = map_bytes;
	INIT_U32(p, 504) = 320;
	INIT_U32(p, 472) = allocation_bytes;
	buffer = nftl_malloc(allocation_bytes);
	INIT_PTR(p, 464) = buffer;
	if (!buffer) {
		nand_print(nftl_ro_lc_207,
			   nftl_ro_func_7098);
		return -2;
	}
	INIT_PTR(p, 480) = buffer;
	INIT_PTR(p, 496) = (char *)buffer + map_bytes;
	INIT_U32(p, 656) = 0;
	INIT_U32(p, 676) = 0;
	INIT_U32(p, 660) = 0;
	INIT_U32(p, 664) = 0;
	INIT_U32(p, 668) = 0;
	INIT_U32(p, 672) = 0;
	INIT_U16(p, 1056) = INIT_U16(chip, 16) >> 1;
	INIT_U16(p, 1058) = INIT_U16(chip, 16) / 6;
	INIT_U16(p, 1060) = 0;
	INIT_U16(p, 1062) = 0;
	INIT_PTR(p, 1064) = 0;
	INIT_PTR(p, 528) = 0;
	INIT_U16(p, 584) = 0xffff;
	INIT_PTR(p, 592) = 0;
	INIT_U16(p, 648) = 0xffff;
	for (index = 0; index < 3; index++) {
		unsigned int base = 288 + index * 64;

		INIT_PTR(p, base) = 0;
		INIT_PTR(p, base + 8) = 0;
		INIT_PTR(p, base + 16) = 0;
		INIT_PTR(p, base + 24) = 0;
		INIT_PTR(p, base + 32) = 0;
		INIT_PTR(p, base + 40) = 0;
	}
	INIT_U16(p, 696) = 0;
	for (index = 0; index < 10; index++) {
		unsigned int base = 704 + index * 32;

		INIT_U16(p, base) = index;
		INIT_U16(p, base + 2) = 0;
		INIT_PTR(p, base + 8) = 0;
		INIT_PTR(p, base + 16) = 0;
		INIT_PTR(p, base + 24) = 0;
	}
	INIT_U16(p, 1024) = 255;
	INIT_U16(p, 698) = 0;
	INIT_U16(p, 1026) = 0;
	INIT_PTR(p, 1032) = 0;
	INIT_PTR(p, 1040) = 0;
	INIT_PTR(p, 1048) = 0;
	return 0;
}
#undef INIT_U8
#undef INIT_U16
#undef INIT_U32
#undef INIT_PTR

struct wear_options {
	unsigned char before_enabled[6];
	unsigned short enabled;
};
struct wear_zone {
	unsigned char before_active[0x2ac];
	unsigned short active;
	unsigned char before_options[0xd78 - 0x2ae];
	struct wear_options *options;
};
extern int check_static_wear_leveling(struct wear_zone *zone);
extern int done_static_wear_leveling(struct wear_zone *zone);
struct wear_check_geometry {
	unsigned char before_erase_limit[16];
	unsigned short erase_limit;
};
struct wear_check_stats {
	unsigned char before_count[8];
	unsigned int count;
};
struct wear_check_zone {
	unsigned char before_geometry[8];
	struct wear_check_geometry *geometry;
	unsigned char before_blocks[10];
	unsigned short blocks;
	unsigned char before_free_head[276];
	void *free_head;
	unsigned char before_invalid_head[40];
	void *invalid_head;
	unsigned char before_stats[168];
	struct wear_check_stats *stats;
	unsigned char before_threshold[522];
	unsigned short threshold;
	unsigned char before_selected[4];
	struct nftl_invalid_erase_node *selected;
};
_Static_assert(__builtin_offsetof(struct wear_check_zone, blocks) == 26,
	       "wear zone block count offset");
_Static_assert(__builtin_offsetof(struct wear_check_zone, free_head) == 304,
	       "wear free list head offset");
_Static_assert(__builtin_offsetof(struct wear_check_zone, invalid_head) == 352,
	       "wear invalid list head offset");
_Static_assert(__builtin_offsetof(struct wear_check_zone, stats) == 528,
	       "wear stats offset");
_Static_assert(__builtin_offsetof(struct wear_check_zone, threshold) == 1058,
	       "wear threshold offset");
_Static_assert(__builtin_offsetof(struct wear_check_zone, selected) == 1064,
	       "wear selected block offset");

int check_static_wear_leveling(struct wear_zone *owner)
{
	struct wear_check_zone *zone = (struct wear_check_zone *)owner;
	struct nftl_free_erase_node *maximum;
	struct nftl_invalid_erase_node *minimum;
	unsigned short max_erase;
	unsigned short min_erase;
	unsigned short limit;
	unsigned int blocks;

	/* The stock pointer gates precede every geometry/list dereference. */
	if (zone->selected || !zone->invalid_head || !zone->free_head)
		return 1;
	if ((int)zone->stats->count < zone->blocks)
		return 1;
	limit = zone->geometry->erase_limit;
	blocks = limit;
	maximum = get_block_max_erase_time_from_free_list(
		(struct nftl_free_erase_owner *)zone, limit);
	if (!maximum)
		return 1;
	max_erase = maximum->erase_time;
	if (max_erase < blocks / 5)
		return 1;
	minimum = get_block_min_erase_time_from_invalid_list(
		(struct nftl_invalid_erase_owner *)zone, limit,
		zone->stats->count - zone->blocks);
	if (!minimum)
		return 1;
	min_erase = minimum->erase_time;
	if (max_erase <= min_erase)
		return 1;
	/* OEM selects four nonmonotonic ratios as the maximum erase count
	 * grows. Keep the odd thresholds rather than smoothing the policy. */
	if (blocks * 2 < (unsigned int)max_erase * 3)
		zone->threshold = blocks / 12;
	else if (blocks < (unsigned int)max_erase * 2)
		zone->threshold = blocks / 10;
	else if (blocks < (unsigned int)max_erase * 3)
		zone->threshold = blocks / 7;
	else
		zone->threshold = blocks / 5;
	if ((unsigned int)(max_erase - min_erase) < zone->threshold)
		return 1;
	nand_print_dbg(nftl_ro_lc_452,
		       max_erase, min_erase);
	nand_print_dbg(nftl_ro_lc_453, minimum->before_erase[0]
		       | minimum->before_erase[1] << 8);
	nand_print_dbg(nftl_ro_lc_454, zone->blocks);
	zone->selected = minimum;
	return 0;
}

struct wear_done_block {
	unsigned short number;
	unsigned char before_erase_count[4];
	unsigned short erase_count;
};
struct wear_done_stats {
	unsigned char before_count[128];
	unsigned int count;
};
struct wear_done_zone {
	unsigned char before_active[1062];
	unsigned short active;
	struct wear_done_block *selected;
	unsigned char before_stats[3440 - 1072];
	struct wear_done_stats *stats;
};
int done_static_wear_leveling(struct wear_zone *owner)
{
	struct wear_done_zone *zone = (struct wear_done_zone *)owner;
	int result;

	if (!zone->selected)
		return 2;
	if (is_current_blcok((struct diag_current_zone *)owner,
				zone->selected) == 1) {
		zone->selected = 0;
		return 2;
	}
	if (!is_block_in_invalid_list((struct invalid_zone *)owner,
				(struct invalid_node *)zone->selected)) {
		nand_print_dbg(nftl_ro_lc_455,
			       zone->selected->number);
		zone->selected = 0;
		return 2;
	}
	nand_print_dbg(nftl_ro_lc_456,
		       zone->selected->number, zone->selected->erase_count);
	zone->active = 1;
	zone->stats->count++;
	/* Stock discards this result even if the page fill failed. */
	fill_no_use_data_to_current_block((struct no_use_data_zone *)owner);
	result = garbage_collect_first((struct gc_one_zone *)owner,
				zone->selected, -1);
	if (result) {
		/* OEM reports the block number as the failure value and leaves
		 * active set; preserve both bugs for stock behavior. */
		nand_print(nftl_ro_lc_457,
			   zone->selected->number);
		zone->selected = 0;
		return 1;
	}
	zone->active = 0;
	if (!is_block_in_free_list((struct free_zone *)owner,
				(struct free_node *)zone->selected))
		nand_print(nftl_ro_lc_458,
			   zone->selected->number);
	zone->selected = 0;
	return 0;
}
int do_static_wear_leveling(struct wear_zone *zone)
{
	/* OEM dereferences options unchecked and reports one when inactive. */
	if (zone->options->enabled == 0)
		return 0;
	if (zone->active == 0)
		return 1;
	if (check_static_wear_leveling(zone) == 0)
		return done_static_wear_leveling(zone);
	return 0;
}

struct free_block {
	unsigned char before_erase[6];
	unsigned short erase_time;
	unsigned char after_erase[0x18];
	struct free_block *next;
	struct free_block *previous;
};
struct free_block_zone {
	unsigned char before_count[0x1e];
	unsigned short count;
	unsigned char before_head[240];
	struct free_block head;
};

/* OEM inserts equal erase counts before the existing block and trusts both
 * backlinks. A damaged or cyclic list is neither rejected nor repaired. */
int add_free_list(struct free_block_zone *zone, struct free_block *target)
{
	struct free_block *node = &zone->head;

	while (node->next) {
		if (node->next->erase_time >= target->erase_time) {
			node->next->previous = target;
			target->next = node->next;
			node->next = target;
			target->previous = node;
			return 0;
		}
		node = node->next;
	}
	node->next = target;
	target->next = 0;
	target->previous = node;
	return 0;
}

/* The invalid list sorts descending by unsigned erase time. Equal values
 * insert before the old node; all links are followed without validation. */
int add_block_invalid_list(struct invalid_zone *zone,
			   struct invalid_node *target)
{
	struct invalid_node *node = &zone->head;

	while (node->next) {
		if (node->next->erase_time <= target->erase_time) {
			node->next->previous = target;
			target->next = node->next;
			node->next = target;
			target->previous = node;
			return 0;
		}
		node = node->next;
	}
	node->next = target;
	target->next = 0;
	target->previous = node;
	return 0;
}

/* This signed count comparison is stock behavior, despite the 32-bit count
 * field being treated as unsigned by some other list operations. */
int add_block_count_list(struct block_count_zone *zone,
			 struct block_count_node *target)
{
	struct block_count_node *node = &zone->head;

	while (node->next) {
		if (node->next->count >= target->count) {
			node->next->previous = target;
			target->next = node->next;
			node->next = target;
			target->previous = node;
			return 0;
		}
		node = node->next;
	}
	node->next = target;
	target->next = 0;
	target->previous = node;
	return 0;
}
struct count_delete_node {
	unsigned char before_links[48];
	struct count_delete_node *next;
	struct count_delete_node *previous;
};
struct count_delete_zone {
	unsigned char before_head[400];
	struct count_delete_node head;
};

/* OEM searches from the sentinel's next pointer. It does not validate the
 * target or the predecessor backlink, detect cycles, or repair corruption. */
#define NFTL_DELETE_BY_BLOCK(name, zone_type, node_type) \
struct node_type *name(struct zone_type *zone, struct node_type *target) \
{ \
	struct node_type *node = &zone->head; \
	node = node->next; \
	while (node) { \
		if (node == target) { \
			node->previous->next = node->next; \
			if (node->next) \
				node->next->previous = node->previous; \
			node->previous = 0; \
			node->next = 0; \
			break; \
		} \
		node = node->next; \
	} \
	return node; \
}

NFTL_DELETE_BY_BLOCK(del_free_list_by_block, free_block_zone, free_block)
NFTL_DELETE_BY_BLOCK(del_block_invalid_list_by_block, invalid_zone, invalid_node)
NFTL_DELETE_BY_BLOCK(del_block_count_by_block, count_delete_zone, count_delete_node)

struct adjust_free_zone {
	unsigned char before_current[64];
	void *current;
};
struct adjust_free_block {
	unsigned short number;
	unsigned char before_erase[4];
	unsigned short erase_time;
};
extern struct free_block *out_phy_block_from_free_list(struct free_block_zone *zone);
int adjust_free_list(struct adjust_free_zone *zone)
{
	struct adjust_free_block *block = (struct adjust_free_block *)
		out_phy_block_from_free_list((struct free_block_zone *)zone);

	/* OEM logs a missing block but dereferences it immediately afterward. */
	if (!block)
		nand_print(nftl_ro_lc_327);
	erase_block((struct erase_block_zone *)zone,
		    (struct erase_block_info *)block,
		    (unsigned short)(block->erase_time + 1));
	block = (struct adjust_free_block *)out_phy_block_from_free_list_by_block(
		(struct nftl_free_erase_owner *)zone,
		(struct nftl_free_erase_node *)block);
	zone->current = block;
	/* The second removal may also return null, which OEM dereferences. */
	nand_print(nftl_ro_lc_328,
		   block->number, block->erase_time);
	return 0;
}
struct free_print_node {
	unsigned short number;
	unsigned short info;
	unsigned char before_erase[2];
	unsigned short erase_time;
	unsigned int count;
	unsigned char before_next[20];
	struct free_print_node *next;
};
struct free_print_zone {
	unsigned char before_head[272];
	struct free_print_node head;
	unsigned char before_debug[344];
	unsigned int debug_level;
};
void print_free_list(struct free_print_zone *zone)
{
	struct free_print_node *node = &zone->head;

	if (zone->debug_level) {
		nand_print_dbg(nftl_ro_lc_400);
		node = node->next;
		/* OEM prints every node without checking cycles or link validity. */
		while (node) {
			nand_print_dbg(nftl_ro_lc_401,
				       node->number, node->erase_time);
			nand_print_dbg(nftl_ro_lc_402, node->count);
			nand_print_dbg(nftl_ro_lc_403, node->info);
			node = node->next;
	}
	}
}
struct invalid_print_node {
	unsigned short number;
	unsigned short info;
	unsigned short invalid;
	unsigned short erase_time;
	unsigned int count;
	unsigned char before_next[4];
	struct invalid_print_node *next;
	struct invalid_print_node *previous;
};
struct invalid_print_zone {
	unsigned char before_head[336];
	struct invalid_print_node head;
	unsigned char before_debug[656 - 368];
	unsigned int debug_level;
};
void print_block_invalid_list(struct invalid_print_zone *zone)
{
	struct invalid_print_node *node = &zone->head;

	if (zone->debug_level) {
		nand_print_dbg(nftl_ro_lc_404);
		node = node->next;
		/* OEM trusts every forward link and can loop on a cycle. */
		while (node) {
			nand_print_dbg(nftl_ro_lc_401,
				       node->number, node->erase_time);
			nand_print_dbg(nftl_ro_lc_405,
				       node->count, node->invalid);
			nand_print_dbg(nftl_ro_lc_403, node->info);
			node = node->next;
		}
	}
}
struct free_block *out_phy_block_from_free_list(struct free_block_zone *zone)
{
	struct free_block *block = (struct free_block *)
		del_free_list((struct diag_free_zone *)zone);

	if (block) {
		block->next = 0;
		block->previous = 0;
		/* OEM silently preserves zero count after successful removal. */
		if (zone->count != 0)
			zone->count--;
	}
	return block;
}

struct clean_list_block {
	unsigned char number[2];
	unsigned short info;
	unsigned short invalid;
	unsigned char before_count[2];
	int count;
	unsigned char before_links[4];
	void *links[6];
};
#define CLEAR_CLEAN_BLOCK(block) do { \
	(block)->invalid = 0; \
	(block)->info = 0; \
	(block)->count = -1; \
	(block)->links[0] = 0; \
	(block)->links[1] = 0; \
	(block)->links[2] = 0; \
	(block)->links[3] = 0; \
	(block)->links[4] = 0; \
	(block)->links[5] = 0; \
} while (0)
void clean_block_list(void *zone)
{
	struct clean_list_block *block;

	/* OEM trusts the removal helpers and clears six pointer slots without
	 * checking whether another owner still references the block. */
	for (;;) {
		block = (struct clean_list_block *)out_phy_block_from_free_list(
			(struct free_block_zone *)zone);
		if (!block)
			break;
		CLEAR_CLEAN_BLOCK(block);
	}
	for (;;) {
		block = (struct clean_list_block *)
			out_phy_block_from_invalid_page_list((struct move_zone *)zone);
		if (!block)
			break;
		CLEAR_CLEAN_BLOCK(block);
	}
}

struct used_count_block {
	unsigned short number;
};
struct used_count_zone {
	unsigned char before_read[0xd88];
	int (*read)(struct used_count_zone *zone, struct physic_op_param *op);
};
__attribute__((optimize("-fno-stack-protector")))
int get_used_block_count(struct used_count_zone *zone,
			 struct used_count_block *block)
{
	int count;
	struct physic_op_param op;
	unsigned char oob[16];

	set_physic_op_par(&op, block->number, 0, 0, 0, oob);
	/* OEM ignores callback errors and reads potentially stale OOB data. */
	zone->read(zone, &op);
	count = get_block_used_count_from_oob(oob);
	return count;
}

struct command_chip {
	unsigned char before_controller[0x110];
	void *controller;
};
extern struct command_chip *nci_get_from_nsi(struct special_exit_info *nsi,
					      unsigned int index);
extern int generic_read_page_start(void *);
extern unsigned char samsung_read_retry_cycle;
struct samsung_read_request {
	unsigned int channel;
	unsigned int block;
	unsigned int page;
	unsigned char before_data[24 - 12];
	unsigned char *data;
};
struct samsung_read_controller {
	unsigned char before_channel[12];
	unsigned int channel;
};
struct samsung_read_chip_id {
	unsigned char before_id[28];
	unsigned int chip;
};
/* The four vendor page-end paths keep chip/result/retry in OEM private slots
 * FP+0x20/+0x28/+0x2c. Callback failures can observe these frame bytes. */
__attribute__((optimize("-fno-stack-protector")))
int samsung_read_page_end(struct samsung_read_request *request)
{
	struct samsung_set_chip *chip;
	int retry;
	int result;

	retry = 0;
	result = 0;
	chip = (struct samsung_set_chip *)nci_get_from_nsi(g_nsi,
						      request->channel);
	result = generic_read_page_end_not_retry(request);
	if (result == -2) {
		nand_print_dbg(nftl_ro_lc_37);
		retry = 1;
		while (samsung_read_retry_cycle > retry) {
			chip->retry = retry;
			result = samsung_set_readretry(chip);
			if (result == 0) {
				result = generic_read_page_start(request);
				result |= generic_read_page_end_not_retry(request);
				if (result == 10 || result == 0) {
					result = 10;
					nand_print_dbg(
						nftl_ro_lc_38,
						((struct samsung_read_controller *)chip->controller)->channel,
						((struct samsung_read_chip_id *)chip)->chip,
						request->block, request->page,
						chip->retry);
					break;
				}
			}
			retry++;
		}
		/* OEM resets retry mode even after errors and ignores reset failure.
		 * It also returns ECC_LIMIT (10) after an eventual clean retry. */
		chip->retry = 0;
		samsung_set_readretry(chip);
	}
	return result;
}
/* C-visible name for the OEM-local eight-byte pointer at BSS offset 416. */
extern unsigned char *micron_p1_recovered;
int micron_set_readretry(struct samsung_set_chip *chip)
{
	unsigned char step;
	int result = 0;

	nand_enable_chip(chip);
	ndfc_disable_randomize(chip->controller);
	if (micron_read_retry_mode == 64 || micron_read_retry_mode == 65 ||
	    micron_read_retry_mode == 80 || micron_read_retry_mode == 66) {
		step = (unsigned char)chip->retry;
		if (step == 255)
			step = 0;
		/* OEM indexes the mutable table pointer without a bounds check. */
		if (micron_read_retry_mode == 80) {
			if (micron_p1_recovered[step] == 0) {
				result = micron_intel_set_feature(chip, 0x93, 1);
				if (result)
					goto done;
			}
			if (chip->retry == 255) {
				result = micron_intel_set_feature(chip, 0x89, 0);
				result |= micron_intel_set_feature(chip, 0x93, 0);
				goto done;
			}
		}
		result = micron_intel_set_feature(chip, 0x89,
						micron_p1_recovered[step]);
	}
done:
	/* OEM clears retry=255 even for unsupported modes and after a
	 * feature failure, but never translates the feature error. */
	if (chip->retry == 255)
		chip->retry = 0;
	nand_disable_chip(chip);
	return result;
}
__attribute__((optimize("-fno-stack-protector")))
int micron_read_page_end(struct samsung_read_request *request)
{
	struct samsung_set_chip *chip;
	int retry;
	int result;

	retry = 0;
	result = 0;
	chip = (struct samsung_set_chip *)nci_get_from_nsi(g_nsi,
						      request->channel);
	result = generic_read_page_end_not_retry(request);
	if (result == -2) {
		nand_print_dbg(nftl_ro_lc_70);
		retry = 0;
		while (micron_read_retry_cycle > retry) {
			chip->retry = retry;
			result = micron_set_readretry(chip);
			if (result == 0) {
				result = generic_read_page_start(request);
				result |= generic_read_page_end_not_retry(request);
				if (result == 10 || result == 0) {
					result = 10;
					nand_print_dbg(
						nftl_ro_lc_71,
						((struct samsung_read_controller *)chip->controller)->channel,
						((struct samsung_read_chip_id *)chip)->chip,
						request->block, request->page,
						chip->retry);
					break;
				}
			}
			retry++;
		}
		/* OEM uses 255 as the reset step, writes after a read error,
		 * ignores the reset failure and returns 10 after a clean retry. */
		chip->retry = 255;
		micron_set_readretry(chip);
	}
	return result;
}
/* An exported alias retains the original local BSS symbol and its assembly
 * relocations while letting this C body address the same four-byte slot. */
extern unsigned int toshiba_retry_flag_recovered;
extern unsigned char toshiba_read_retry_cycle;
extern int toshiba_set_readretry(struct samsung_set_chip *);
extern int toshiba_exit_readretry(struct samsung_set_chip *);
__attribute__((optimize("-fno-stack-protector")))
int toshiba_read_page_end(struct samsung_read_request *request)
{
	struct samsung_set_chip *chip;
	int retry;
	int result;

	retry = 0;
	result = 0;
	chip = (struct samsung_set_chip *)nci_get_from_nsi(g_nsi,
						      request->channel);
	result = generic_read_page_end_not_retry(request);
	if (result == -2) {
		nand_print_dbg(nftl_ro_lc_79);
		toshiba_retry_flag_recovered = 1;
		retry = 0;
		while (toshiba_read_retry_cycle > retry) {
			chip->retry = retry;
			result = toshiba_set_readretry(chip);
			if (result == 0) {
				result = generic_read_page_start(request);
				result |= generic_read_page_end_not_retry(request);
				if (result == 10 || result == 0) {
					result = 10;
					nand_print_dbg(
						nftl_ro_lc_80,
						((struct samsung_read_controller *)chip->controller)->channel,
						((struct samsung_read_chip_id *)chip)->chip,
						request->block, request->page,
						chip->retry);
					break;
				}
			}
			retry++;
		}
		/* OEM clears the global flag and resets retry even after failures,
		 * then ignores the exit callback's result. */
		toshiba_retry_flag_recovered = 0;
		chip->retry = 0;
		toshiba_exit_readretry(chip);
	}
	return result;
}

extern int sandisk_set_readretry(struct samsung_set_chip *, unsigned short);
extern int sandisk_end_one_read_retry(struct samsung_set_chip *);
extern int sandisk_exit_readretry(struct samsung_set_chip *);
__attribute__((optimize("-fno-stack-protector")))
int sandisk_read_page_end(struct samsung_read_request *request)
{
	struct samsung_set_chip *chip;
	int retry = 0;
	int result = 0;

	chip = (struct samsung_set_chip *)nci_get_from_nsi(g_nsi,
						      request->channel);
	result = generic_read_page_end_not_retry(request);
	if (result == -2) {
		nand_print_dbg(nftl_ro_lc_134);
		retry = 0;
		while (sandisk_read_retry_cycle > retry) {
			chip->retry = retry;
			result = sandisk_set_readretry(chip,
					       (unsigned short)request->page);
			if (result == 0) {
				result = generic_read_page_start(request);
				result |= generic_read_page_end_not_retry(request);
				/* OEM runs this command after every completed retry read,
				 * including reads that still report an error. */
				sandisk_end_one_read_retry(chip);
				if (is_nouse_page(request->data) == 1) {
					result = -2;
					nand_print_dbg(
						nftl_ro_lc_47,
						((struct samsung_read_controller *)chip->controller)->channel,
						((struct samsung_read_chip_id *)chip)->chip,
						request->block, request->page,
						chip->retry);
				} else if (result == 10 || result == 0) {
					result = 10;
					nand_print_dbg(
						nftl_ro_lc_135,
						((struct samsung_read_controller *)chip->controller)->channel,
						((struct samsung_read_chip_id *)chip)->chip,
						request->block, request->page,
						chip->retry);
					break;
				}
			}
			retry++;
		}
		/* Stock resets the retry count and ignores exit failure even when
		 * every read or the setter failed. */
		chip->retry = 0;
		sandisk_exit_readretry(chip);
	}
	return result;
}

extern int hynix26nm_set_readretry(struct hynix20nm_retry_chip *);
/* Both Hynix generations use this OEM retry state machine. In particular,
 * a setter failure causes an immediate reset call and then a second reset
 * call after the loop. The reset results never replace the failing status.
 * Keep the OEM frame slots by disabling GCC's stack-protector slot shuffle. */
#define HYNIX_READ_PAGE_END(name, cycle_var, setter, opening, success) \
__attribute__((optimize("-fno-stack-protector"))) \
int name(struct samsung_read_request *request) \
{ \
	struct samsung_set_chip *chip; \
	int retry = 0; \
	int result = 0; \
	chip = (struct samsung_set_chip *)nci_get_from_nsi(g_nsi, \
						      request->channel); \
	result = generic_read_page_end_not_retry(request); \
	if (result == -2) { \
		nand_print_dbg(opening); \
		retry = 0; \
		while (cycle_var > retry) { \
			result = 0; \
			chip->retry++; \
			if (chip->retry > cycle_var) \
				chip->retry = 0; \
			result = setter((void *)chip); \
			if (result) { \
				chip->retry = 0; \
				setter((void *)chip); \
				break; \
			} \
			result = generic_read_page_start(request); \
			result |= generic_read_page_end_not_retry(request); \
			if (result == 10 || result == 0) { \
				result = 10; \
				nand_print_dbg(success, \
					((struct samsung_read_controller *)chip->controller)->channel, \
					((struct samsung_read_chip_id *)chip)->chip, \
					request->block, request->page, chip->retry); \
				break; \
			} \
			retry++; \
		} \
		chip->retry = 0; \
		setter((void *)chip); \
	} \
	return result; \
}
HYNIX_READ_PAGE_END(hynix16nm_read_page_end, hynix16nm_read_retry_cycle,
		    hynix16nm_set_readretry, nftl_ro_lc_110,
		    nftl_ro_lc_111)
HYNIX_READ_PAGE_END(hynix26nm_read_page_end, hynix26nm_read_retry_cycle,
		    hynix26nm_set_readretry, nftl_ro_lc_86,
		    nftl_ro_lc_87)
#undef HYNIX_READ_PAGE_END

extern int set_one_cmd(struct command_chip *chip, unsigned int command,
			unsigned int argument);
extern unsigned char sandisk_A19_read_retry_reg_adr_1y;
/* Keep the OEM private result and parameter slots beside the aligned buffer. */
__attribute__((optimize("-fno-stack-protector")))
int sandisk_A19_exit_readretry(struct samsung_set_chip *chip)
{
	int result = 0;
	/* Match the OEM value buffer's frame offset despite stack packing. */
	unsigned char values[4] __attribute__((aligned(8))) = {0, 0, 0, 0};
	unsigned char *address;
	int count;

	nand_enable_chip(chip);
	ndfc_disable_randomize(chip->controller);
	/* OEM ignores both command results and always disables the chip. */
	set_one_cmd((struct command_chip *)chip, -1, 1);
	address = &sandisk_A19_read_retry_reg_adr_1y;
	count = 1;
	result = sandisk_A19_vender_set_param(chip, values, address, count);
	nand_disable_chip(chip);
	return result;
}
#define SANDISK_COMMAND(name, opcode) \
	int name(const unsigned int *index) \
	{ \
		struct command_chip *chip = nci_get_from_nsi(g_nsi, *index); \
		/* OEM ignores every command error and always reports success. */ \
		nand_enable_chip((struct samsung_set_chip *)chip); \
		ndfc_disable_randomize(chip->controller); \
		set_one_cmd(chip, opcode, 0); \
		nand_disable_chip((struct samsung_set_chip *)chip); \
		return 0; \
	}
SANDISK_COMMAND(sandisk_A19_cmd25, 0x25)
SANDISK_COMMAND(sandisk_A19_dsp_on, 0x26)

int sandisk_A19_set_lmflgfix_next(const unsigned int *index,
				  unsigned char next_value)
{
	struct command_chip *chip = nci_get_from_nsi(g_nsi, *index);
	unsigned char command;
	unsigned char value;
	unsigned char address;

	nand_enable_chip((struct samsung_set_chip *)chip);
	ndfc_disable_randomize(chip->controller);
	set_one_cmd(chip, 0x5c, 0);
	set_one_cmd(chip, 0xffffffc5, 0);
	/* Stock does not initialize these stack bytes until after setup. */
	address = 0;
	value = 1;
	command = 0x55;
	/* OEM ignores every command result, including the two setup writes. */
	set_cmd_with_nand_bus(chip, &command, 1, &address, &value, 1, 1);
	if (sandisk_A19_read_retry_mode == 52)
		address = 34;
	else if (sandisk_A19_read_retry_mode == 53)
		address = 37;
	else
		address = 35;
	value = next_value;
	command = 0x55;
	set_cmd_with_nand_bus(chip, &command, 1, &address, &value, 1, 1);
	address = 0;
	value = 0;
	command = 0x55;
	set_cmd_with_nand_bus(chip, &command, 1, &address, &value, 1, 1);
	nand_disable_chip((struct samsung_set_chip *)chip);
	return 0;
}

#define A19_REPORT(message) \
	nand_print_dbg(message, \
		((struct samsung_read_controller *)state.chip->controller)->channel, \
		((struct samsung_read_chip_id *)state.chip)->chip, \
		request->block, request->page, state.chip->retry)
int sandisk_A19_read_page_end(struct samsung_read_request *request)
{
	/* OEM's private 16-byte record places chip/result/retry at FP+32/40/44. */
	struct {
		struct samsung_set_chip *chip;
		int result;
		int retry;
	} state;

	state.retry = 0;
	state.result = 0;
	state.chip = (struct samsung_set_chip *)
		nci_get_from_nsi(g_nsi, request->channel);
	state.result = generic_read_page_end_not_retry(request);

	if (state.result == -2) {
		nand_print_dbg(nftl_ro_lc_94);
		/* Keep four separately expanded loops: the OEM rereads the retry-cycle
		 * bound and mutable mode at their own call boundaries. */
	#define A19_PHASE(phase) do { \
			for (state.retry = 0; sandisk_A19_read_retry_cycle > state.retry; state.retry++) { \
				state.chip->retry = state.retry; \
				state.result = sandisk_A19_set_readretry(state.chip, \
						      (unsigned short)request->page); \
				if (state.result) { \
					nand_print_dbg(nftl_ro_lc_95); \
					continue; \
				} \
				if (phase == 1) \
					sandisk_A19_dsp_on(&request->channel); \
				if (phase == 2) \
					sandisk_A19_cmd25(&request->channel); \
				if (phase == 3) { \
					if (sandisk_A19_read_retry_mode == 52) \
						sandisk_A19_set_lmflgfix_next( \
							&request->channel, -58); \
					else if (sandisk_A19_read_retry_mode == 53) \
						sandisk_A19_set_lmflgfix_next( \
							&request->channel, 26); \
					else \
						sandisk_A19_set_lmflgfix_next( \
							&request->channel, -64); \
					sandisk_A19_cmd25(&request->channel); \
					if (sandisk_A19_read_retry_mode == 50 || \
					    sandisk_A19_read_retry_mode == 52 || \
					    sandisk_A19_read_retry_mode == 53) \
						sandisk_A19_dsp_on(&request->channel); \
				} \
				state.result = generic_read_page_start(request); \
				state.result |= generic_read_page_end_not_retry(request); \
				if (phase == 3) { \
					if (sandisk_A19_read_retry_mode == 52) \
						sandisk_A19_set_lmflgfix_next( \
							&request->channel, -62); \
					else if (sandisk_A19_read_retry_mode == 53) \
						sandisk_A19_set_lmflgfix_next( \
							&request->channel, 10); \
					else \
						sandisk_A19_set_lmflgfix_next( \
							&request->channel, 64); \
				} \
				/* OEM checks spare data even after a read error. */ \
				if (is_nouse_page(request->data) == 1) { \
					state.result = -2; \
					A19_REPORT(nftl_ro_lc_47); \
					continue; \
				} \
				if (state.result == 10 || state.result == 0) { \
					if (state.result == 0) \
						A19_REPORT(nftl_ro_lc_96); \
					else \
						A19_REPORT(nftl_ro_lc_97); \
					state.result = 10; \
					goto done; \
				} \
			} \
	} while (0)
		A19_PHASE(0);
		/* Stock samples mode 51 once after phase zero. That one decision skips
		 * both the DSP and plain CMD25 loops, even if callbacks change mode. */
		if (sandisk_A19_read_retry_mode != 51) {
			A19_PHASE(1);
			A19_PHASE(2);
		}
		A19_PHASE(3);
	#undef A19_PHASE
	done:
		state.chip->retry = 0;
		/* Stock ignores the reset result even when all retries failed. */
		sandisk_A19_exit_readretry(state.chip);
	}
	return state.result;
}
#undef A19_REPORT

extern int set_one_addr(struct command_chip *chip, unsigned int address);
#define HYNIX_LSB_EXIT(name, message) \
int name(struct samsung_set_chip *chip) \
{ \
	int delay = 1048575; \
	nand_enable_chip(chip); \
	ndfc_disable_randomize(chip->controller); \
	/* OEM ignores every command result and sends five zero addresses. */ \
	set_one_cmd((struct command_chip *)chip, 0, 0); \
	set_one_addr((struct command_chip *)chip, 0); \
	set_one_addr((struct command_chip *)chip, 0); \
	set_one_addr((struct command_chip *)chip, 0); \
	set_one_addr((struct command_chip *)chip, 0); \
	set_one_addr((struct command_chip *)chip, 0); \
	set_one_cmd((struct command_chip *)chip, 48, 1); \
	/* OEM burns 1,048,576 iterations rather than waiting on readiness. */ \
	while (delay-- > 0) \
		; \
	nand_print_dbg(message); \
	nand_disable_chip(chip); \
	return 0; \
}

HYNIX_LSB_EXIT(hynix20nm_lsb_exit, nftl_ro_lc_50)
HYNIX_LSB_EXIT(hynix26nm_lsb_exit, nftl_ro_lc_88)

struct prio_gc_owner;
extern int add_prio_gc(struct prio_gc_owner *zone, void *physical,
		       unsigned short priority);
int prio_gc_one(void *zone, unsigned short block, unsigned int priority)
{
	void *physical = get_phy_block_addr((struct nftl_phy_outer *)zone,
					block);

	/* OEM redundantly writes valid priorities and clamps others to 3. */
	if (priority == 1)
		priority = 1;
	else if (priority == 2)
		priority = 2;
	else if (priority == 3)
		priority = 3;
	else
		priority = 3;
	/* OEM ignores failed insertion and reports success. */
	add_prio_gc((struct prio_gc_owner *)zone, physical,
		    (unsigned short)priority);
	return 0;
}

extern int add_free_list(struct free_block_zone *zone,
                         struct free_block *node);
int put_phy_block_to_free_list(struct move_zone *zone,
                               struct move_node *node)
{
    int result;

    /* OEM clears links before insertion and increments only on success. */
    ((struct free_block *)node)->next = 0;
    ((struct free_block *)node)->previous = 0;
    result = add_free_list((struct free_block_zone *)zone,
                           (struct free_block *)node);
    if (result == 0)
        ((struct free_block_zone *)zone)->count++;
    else
        nand_print_dbg(nftl_ro_lc_409);
    return result;
}

extern struct free_block *del_free_list_by_block(
    struct free_block_zone *zone, struct free_block *target);
struct nftl_free_erase_node *out_phy_block_from_free_list_by_block(
    struct nftl_free_erase_owner *zone, struct nftl_free_erase_node *target)
{
    struct nftl_free_erase_node *node =
        (struct nftl_free_erase_node *)del_free_list_by_block(
            (struct free_block_zone *)zone, (struct free_block *)target);

    if (node) {
        ((struct free_block *)node)->next = 0;
        ((struct free_block *)node)->previous = 0;
        /* OEM leaves an already-zero count unchanged. */
        if (((struct free_block_zone *)zone)->count != 0)
            ((struct free_block_zone *)zone)->count--;
        return node;
    }
    return node;
}

struct prio_gc_node {
	unsigned short state;
	unsigned short priority;
	unsigned char pad[4];
	void *block;
	void *next;
	void *prev;
};
struct prio_gc_pool {
	unsigned char before_nodes[8];
	struct prio_gc_node nodes[10];
};
struct prio_gc_owner { unsigned char before[696]; struct prio_gc_pool pool; };
struct prio_gc_node *get_empty_gc_node(struct prio_gc_pool *pool)
{
	unsigned short index;

	for (index = 0; index <= 9; index++) {
		/* OEM treats priority zero as an available slot. This can
		 * reuse a live node inserted with zero priority. */
		if (pool->nodes[index].priority == 0)
			return &pool->nodes[index];
	}
	return 0;
}
int add_prio_gc(struct prio_gc_owner *owner, void *block,
		unsigned short priority)
{
	struct prio_gc_node *node;
	node = (struct prio_gc_node *)search_gc_node(
		(struct gc_search_zone *)&owner->pool, block);
	/* OEM reports one for both existing entries and pool exhaustion. */
	if (node)
		return 1;
	node = get_empty_gc_node(&owner->pool);
	if (!node)
		return 1;
	/* OEM trusts owner, block, and every helper result. */
	node->block = block;
	node->priority = priority;
	node->next = 0;
	node->prev = 0;
	add_to_gc_list_tail((struct gc_list *)&owner->pool,
			    (struct gc_list_node *)node);
	return 0;
}

struct prio_gc_queue {
	unsigned short count;
	unsigned short active_priority;
	unsigned char before_head[324];
	struct prio_gc_node head;
};
struct do_prio_gc_zone {
	unsigned char before_queue[696];
	struct prio_gc_queue queue;
};
struct do_prio_gc_block {
	unsigned short number;
	unsigned char before_erase[4];
	unsigned short erase_count;
};
_Static_assert(__builtin_offsetof(struct do_prio_gc_zone,
				  queue.active_priority) == 698,
	       "priority GC active field offset");
_Static_assert(__builtin_offsetof(struct do_prio_gc_zone,
				  queue.head.next) == 1040,
	       "priority GC head offset");

int do_prio_gc(void *owner)
{
	struct do_prio_gc_zone *zone = owner;
	struct prio_gc_queue *queue = &zone->queue;
	struct prio_gc_node *node;
	struct do_prio_gc_block *block;
	struct do_prio_gc_block *removed;
	int result;

	if (!queue->head.next)
		return 0;
	node = (struct prio_gc_node *)del_from_gc_list(
		(struct gc_zone *)queue,
		(struct gc_node *)queue->head.next);
	block = node->block;
	if (is_current_blcok((struct diag_current_zone *)zone, block) == 1) {
		add_to_gc_list_tail((struct gc_list *)queue,
				    (struct gc_list_node *)node);
		return 0;
	}
	if (!block) {
		nand_print_dbg(nftl_ro_lc_447);
		goto clear;
	}
	if (!is_block_in_invalid_list((struct invalid_zone *)zone,
				      (struct invalid_node *)block)) {
		nand_print_dbg(nftl_ro_lc_448,
			       block->number, node->priority);
		goto clear;
	}
	queue->active_priority = node->priority;
	if (node->priority == 5) {
		/* OEM ignores priority-five collection failures. */
		garbage_collect_first((struct gc_one_zone *)zone, block, 4);
		goto clear;
	}
	result = garbage_collect_first((struct gc_one_zone *)zone, block, -1);
	if (result) {
		nand_print(nftl_ro_lc_449,
			   block->number, node->priority);
		goto clear_error;
	}
	if (!is_block_in_free_list((struct free_zone *)zone,
				   (struct free_node *)block)) {
		nand_print_dbg(nftl_ro_lc_450,
			       block->number, node->priority);
		goto clear;
	}
	removed = (struct do_prio_gc_block *)
		out_phy_block_from_free_list_by_block(
			(struct nftl_free_erase_owner *)zone,
			(struct nftl_free_erase_node *)block);
	if (!removed) {
		nand_print_dbg(nftl_ro_lc_451,
			       block->number, node->priority);
		goto clear;
	}
	/* Stock has four branches (priority 1, 2, 4, and other) that all
	 * issue the same erase call. Keep the branches to record this bug. */
	if (queue->active_priority == 1)
		erase_block((struct erase_block_zone *)zone,
			    (struct erase_block_info *)removed,
			    (unsigned short)(removed->erase_count + 1));
	else if (queue->active_priority == 2)
		erase_block((struct erase_block_zone *)zone,
			    (struct erase_block_info *)removed,
			    (unsigned short)(removed->erase_count + 1));
	else if (queue->active_priority == 4)
		erase_block((struct erase_block_zone *)zone,
			    (struct erase_block_info *)removed,
			    (unsigned short)(removed->erase_count + 1));
	else
		erase_block((struct erase_block_zone *)zone,
			    (struct erase_block_info *)removed,
			    (unsigned short)(removed->erase_count + 1));
clear:
	node->priority = 0;
	queue->active_priority = 0;
	node->block = 0;
	return 0;
clear_error:
	node->priority = 0;
	queue->active_priority = 0;
	node->block = 0;
	return 1;
}

struct static_wear_stats {
	unsigned char before_attempts[180];
	unsigned int attempts;
};
struct static_wear_zone {
	unsigned char before_chip[8];
	struct gc_win_chip *chip;
	unsigned char before_available[14];
	unsigned short available;
	unsigned char before_policy[668 - 32];
	unsigned int policy;
	unsigned int progress;
	unsigned char before_threshold[4];
	unsigned short threshold;
	unsigned char before_stats[3440 - 682];
	struct static_wear_stats *stats;
};
int static_wear_leveling(struct static_wear_zone *zone)
{
	struct invalid_node *block;
	int result;

	if (zone->available <= zone->threshold)
		return 0;
	if (zone->policy > 19) {
		zone->stats->attempts++;
		block = get_block_min_count_from_invalid_list(
			(struct invalid_zone *)zone);
		result = add_prio_gc((struct prio_gc_owner *)zone, block, 5);
		if (result == 0)
			zone->progress += 4;
		if (zone->progress >= zone->chip->pages_per_block) {
			zone->policy = 0;
			zone->progress = 0;
		}
	}
	/* OEM ignores null selection and GC insertion errors in its return. */
	return 0;
}

struct samsung_init_inner {
	unsigned char before_flags[52];
	unsigned int flags;
};
struct samsung_init_chip {
	unsigned char before_state[184];
	unsigned int state;
	unsigned char until_inner[92];
	struct samsung_init_inner *inner;
};
extern unsigned char samsung_read_retry_mode;
extern unsigned char samsung_read_retry_cycle;
int samsung_readretry_init(struct samsung_init_chip *chip)
{
	chip->state = 0;
	samsung_read_retry_mode = (unsigned char)(chip->inner->flags >> 16);
	samsung_read_retry_cycle = 15;
	/* Preserve OEM typo and fixed cycle count. */
	nand_print_dbg(nftl_ro_lc_39,
		       samsung_read_retry_cycle);
	/* OEM rejects other modes after mutating global state. */
	if (samsung_read_retry_mode != 32)
		return -100;
	return 0;
}
extern unsigned char toshiba_read_retry_mode;
extern unsigned char toshiba_read_retry_cycle;
extern unsigned char toshiba_read_retry_reg_cnt;
int toshiba_readretry_init(struct samsung_init_chip *chip)
{
	chip->state = 0;
	/* OEM rereads the configuration word for each byte and trusts the
	 * nested pointer. Keep the diagnostic typo and global write order. */
	toshiba_read_retry_mode =
		(unsigned char)(chip->inner->flags >> 16);
	toshiba_read_retry_cycle =
		(unsigned char)(chip->inner->flags >> 8);
	toshiba_read_retry_reg_cnt =
		(unsigned char)chip->inner->flags;
	nand_print_dbg(nftl_ro_lc_81,
		       toshiba_read_retry_cycle);
	return 0;
}

extern int micron_readretry_init(struct micron_init_chip *);
extern int hynix20nm_readretry_init(struct hynix20nm_retry_chip *);
struct hynix26nm_init_chip;
extern int hynix26nm_readretry_init(struct hynix26nm_init_chip *);
extern int hynix20nm_read_page_end(struct samsung_read_request *);
extern int sandisk_read_page_end(struct samsung_read_request *);
extern int sandisk_A19_read_page_end(struct samsung_read_request *);
extern unsigned char hynix16nm_read_retry_mode, hynix16nm_read_retry_cycle,
	hynix16nm_read_retry_reg_num;
extern unsigned char hynix20nm_read_retry_mode, hynix20nm_read_retry_cycle,
	hynix20nm_read_retry_reg_num;
extern unsigned char hynix26nm_read_retry_mode, hynix26nm_read_retry_cycle;
extern unsigned char sandisk_read_retry_mode, sandisk_read_retry_cycle,
	sandisk_read_retry_reg_cnt;
extern unsigned char sandisk_A19_read_retry_mode,
	sandisk_A19_read_retry_cycle, sandisk_A19_read_retry_reg_cnt;

/* OEM ORs retry-init results across every chip, publishes the read-end
 * callback only when the aggregate is zero, and never checks list cycles. */
/* Model the observed stock local slots: chip at FP+16, result at FP+28.
 * The middle word is unused. Preserve that layout and the OEM's unchecked
 * chip-list traversal; the aggregate OR is intentionally kept after each call. */
struct special_init_state {
	struct special_exit_chip *chip;
	int unused;
	int result;
};
_Static_assert(sizeof(struct special_init_state) == 16,
	       "special-init stock local span");
_Static_assert(__builtin_offsetof(struct special_init_state, result) == 12,
	       "special-init stock result offset");
#define SPECIAL_INIT_TWO(name, init, end, mode, cycle, success, failure) \
int name(void) \
{ \
	struct special_init_state state; \
	state.result = 0; \
	state.chip = g_nsi->chips; \
	while (state.chip) { \
		state.result |= init((void *)state.chip); \
		state.chip = state.chip->next; \
	} \
	if (state.result == 0) { \
		df_read_page_end = (void (*)(void))end; \
		nand_print_dbg(success, mode, cycle); \
	} else { \
		nand_print(failure, mode, cycle); \
	} \
	return state.result; \
}

#define SPECIAL_INIT_THREE(name, init, end, mode, cycle, reg, success, failure) \
int name(void) \
{ \
	struct special_init_state state; \
	state.result = 0; \
	state.chip = g_nsi->chips; \
	while (state.chip) { \
		state.result |= init((void *)state.chip); \
		state.chip = state.chip->next; \
	} \
	if (state.result == 0) { \
		df_read_page_end = (void (*)(void))end; \
		nand_print_dbg(success, mode, cycle, reg); \
	} else { \
		nand_print(failure, mode, cycle, reg); \
	} \
	return state.result; \
}

SPECIAL_INIT_TWO(samsung_special_init, samsung_readretry_init,
	samsung_read_page_end, samsung_read_retry_mode, samsung_read_retry_cycle,
	nftl_ro_lc_40,
	nftl_ro_lc_41)
SPECIAL_INIT_TWO(micron_special_init, micron_readretry_init,
	micron_read_page_end, micron_read_retry_mode, micron_read_retry_cycle,
	nftl_ro_lc_74,
	nftl_ro_lc_75)
SPECIAL_INIT_THREE(toshiba_special_init, toshiba_readretry_init,
	toshiba_read_page_end, toshiba_read_retry_mode, toshiba_read_retry_cycle,
	toshiba_read_retry_reg_cnt,
	nftl_ro_lc_82,
	nftl_ro_lc_83)
SPECIAL_INIT_THREE(hynix16nm_special_init, hynix16nm_readretry_init,
	hynix16nm_read_page_end, hynix16nm_read_retry_mode,
	hynix16nm_read_retry_cycle, hynix16nm_read_retry_reg_num,
	nftl_ro_lc_131,
	nftl_ro_lc_132)
SPECIAL_INIT_THREE(hynix20nm_special_init, hynix20nm_readretry_init,
	hynix20nm_read_page_end, hynix20nm_read_retry_mode,
	hynix20nm_read_retry_cycle, hynix20nm_read_retry_reg_num,
	nftl_ro_lc_66,
	nftl_ro_lc_67)
SPECIAL_INIT_THREE(hynix26nm_special_init, hynix26nm_readretry_init,
	hynix26nm_read_page_end, hynix26nm_read_retry_mode,
	hynix26nm_read_retry_cycle, hynix26nm_read_retry_reg_num,
	nftl_ro_lc_91,
	nftl_ro_lc_92)
SPECIAL_INIT_THREE(sandisk_special_init, sandisk_readretry_init,
	sandisk_read_page_end, sandisk_read_retry_mode,
	sandisk_read_retry_cycle, sandisk_read_retry_reg_cnt,
	nftl_ro_lc_138,
	nftl_ro_lc_139)
SPECIAL_INIT_THREE(sandisk_A19_special_init, sandisk_A19_readretry_init,
	sandisk_A19_read_page_end, sandisk_A19_read_retry_mode,
	sandisk_A19_read_retry_cycle, sandisk_A19_read_retry_reg_cnt,
	nftl_ro_lc_100,
	nftl_ro_lc_101)

extern unsigned int sandisk_toggle_mode_flag;
extern unsigned char sandisk_read_retry_disable_cmd_19[];
extern int ndfc_is_toggle_interface(void *);
extern int ndfc_set_legacy_interface(void *);
extern int ndfc_set_toggle_interface(void *);
int sandisk_end_one_read_retry(struct samsung_set_chip *chip)
{
	if (sandisk_read_retry_mode != 48 && sandisk_read_retry_mode != 49)
		return 0;
	nand_enable_chip(chip);
	ndfc_disable_randomize(chip->controller);
	if (ndfc_is_toggle_interface(chip->controller)) {
		ndfc_set_legacy_interface(chip->controller);
		sandisk_toggle_mode_flag = 1;
	}
	/* OEM ignores the command result and tests a sticky global flag: a
	 * prior toggle operation can force restoration on a later chip. */
	set_cmd_with_nand_bus(chip, sandisk_read_retry_disable_cmd_19,
			      0, 0, 0, 0, 1);
	if (sandisk_toggle_mode_flag == 1)
		ndfc_set_toggle_interface(chip->controller);
	nand_disable_chip(chip);
	return 0;
}

extern unsigned char sandisk_read_retry_reg_adr_19[];
extern unsigned char sandisk_read_retry_reg_adr_24[];
extern unsigned char sandisk_read_retry_reg_adr_1y[];
/* The four-byte value array has no canary in the stock object. Initialize
 * result first: OEM clears FP+0x3c before the four bytes at FP+0x28. */
__attribute__((optimize("-fno-stack-protector")))
int sandisk_exit_readretry(struct samsung_set_chip *chip)
{
	int result = 0;
	unsigned char values[4] = {0, 0, 0, 0};
	unsigned char *addresses;
	int count;

	nand_enable_chip(chip);
	ndfc_disable_randomize(chip->controller);
	if (ndfc_is_toggle_interface(chip->controller)) {
		ndfc_set_legacy_interface(chip->controller);
		sandisk_toggle_mode_flag = 1;
	}
	if (sandisk_read_retry_mode == 48) {
		addresses = sandisk_read_retry_reg_adr_19;
		count = 3;
		result = sandisk_vender_set_param(chip, values, addresses, count);
	} else if (sandisk_read_retry_mode == 49) {
		addresses = sandisk_read_retry_reg_adr_24;
		count = 3;
		result = sandisk_vender_set_param(chip, values, addresses, count);
	} else if (sandisk_read_retry_mode == 50) {
		/* OEM ignores the command result before programming one byte. */
		set_one_cmd((struct command_chip *)chip, -1, 1);
		addresses = sandisk_read_retry_reg_adr_1y;
		count = 1;
		result = sandisk_vender_set_param(chip, values, addresses, count);
	}
	/* OEM retains this flag across calls, so a different chip can trigger
	 * restoration even when this invocation never switched interface. */
	if (sandisk_toggle_mode_flag == 1)
		ndfc_set_toggle_interface(chip->controller);
	nand_disable_chip(chip);
	return result;
}

extern unsigned int toshiba_toggle_mode_flag_recovered;
extern unsigned char toshiba_p1_1[], toshiba_p1_2[], toshiba_p1_3[];
extern unsigned char toshiba_read_retry_addr_1[], toshiba_read_retry_addr_2[];
extern unsigned char toshiba_read_retry_cmd_1[], toshiba_read_retry_cmd_2[];
extern unsigned char toshiba_read_retry_exit_cmd[];
extern unsigned char toshiba_read_retry_last_cmd[];
extern unsigned char toshiba_read_retry_end_cmd[];
extern unsigned int toshiba_retry_flag_recovered;
int toshiba_vender_set_param(struct samsung_set_chip *chip,
			    unsigned char *values, unsigned char *addresses,
			    unsigned int count)
{
	int result = 0;

	if (toshiba_read_retry_mode == 18) {
		result |= set_cmd_with_nand_bus(chip,
				  toshiba_read_retry_cmd_2, 0,
				  addresses, values, 1, count);
		if (values[0] == 0 && values[1] == 0 &&
		    values[2] == 0 && values[3] == 0 &&
		    toshiba_retry_flag_recovered == 0) {
			result |= set_one_cmd((struct command_chip *)chip,
					      toshiba_read_retry_exit_cmd[0], 1);
			return result;
		}
	}
	if (toshiba_read_retry_mode == 17) {
		result |= set_cmd_with_nand_bus(chip,
				  toshiba_read_retry_cmd_2, 0,
				  addresses, values, 1, count);
		if (values[0] == 0 && values[1] == 0 &&
		    values[2] == 0 && values[3] == 0) {
			result |= set_one_cmd((struct command_chip *)chip,
					      toshiba_read_retry_exit_cmd[0], 1);
			return result;
		}
		if (values[0] == 2 && values[1] == 0 &&
		    values[2] == 126 && values[3] == 124)
			result |= set_one_cmd((struct command_chip *)chip,
					      toshiba_read_retry_last_cmd[0], 0);
	}
	if (toshiba_read_retry_mode == 16) {
		if (values[0] == 255 && values[1] == 255 &&
		    values[2] == 255 && values[3] == 255) {
			result |= set_one_cmd((struct command_chip *)chip,
					      toshiba_read_retry_exit_cmd[0], 1);
			return result;
		}
		result |= set_cmd_with_nand_bus(chip,
				  toshiba_read_retry_cmd_1, 0,
				  addresses, values, 1, count);
	}
	result |= set_one_cmd((struct command_chip *)chip,
			      toshiba_read_retry_end_cmd[0], 0);
	result |= set_one_cmd((struct command_chip *)chip,
			      toshiba_read_retry_end_cmd[1], 0);
	/* OEM unconditionally inspects four bytes, including when count is zero;
	 * the mode-16 exit caller uses that zero count. The NAND error results
	 * are ORed together, so callers cannot recover the individual failure. */
	nand_print_dbg(nftl_ro_lc_77,
		       values[0], values[1], values[2], values[3]);
	return result;
}
/* Stock keeps values/addresses/count/result at FP+0x38/+0x30/+0x2c/+0x28,
 * matching its exit path; the normal stack-protector layout reverses them. */
__attribute__((optimize("-fno-stack-protector")))
int toshiba_set_readretry(struct samsung_set_chip *chip)
{
	unsigned char *values;
	unsigned char *addresses;
	int count;
	int result;

	if (toshiba_read_retry_mode != 16 &&
	    toshiba_read_retry_mode != 17 &&
	    toshiba_read_retry_mode != 18)
		return -89;
	toshiba_read_retry_clock_save((struct clock_chip *)chip);
	nand_enable_chip(chip);
	ndfc_disable_randomize(chip->controller);
	if (chip->retry == 0)
		/* OEM ignores precondition failure and still programs retry. */
		toshiba_vender_pre_condition(chip);
	if (ndfc_is_toggle_interface(chip->controller)) {
		ndfc_set_legacy_interface(chip->controller);
		toshiba_toggle_mode_flag_recovered = 1;
	}
	if (toshiba_read_retry_mode == 17) {
		values = toshiba_p1_2 + chip->retry * 5;
		addresses = toshiba_read_retry_addr_2;
		count = 5;
	} else if (toshiba_read_retry_mode == 18) {
		values = toshiba_p1_3 + chip->retry * 5;
		addresses = toshiba_read_retry_addr_2;
		count = 5;
	} else {
		values = toshiba_p1_1 + chip->retry * 4;
		addresses = toshiba_read_retry_addr_1;
		count = 4;
	}
	/* Retry indexes are unchecked against the tables. Stock also keeps
	 * the toggle flag sticky across chips and ignores restore failures. */
	result = toshiba_vender_set_param(chip, values, addresses, count);
	if (toshiba_toggle_mode_flag_recovered == 1)
		ndfc_set_toggle_interface(chip->controller);
	nand_disable_chip(chip);
	toshiba_read_retry_clock_recover((struct clock_chip *)chip);
	return result;
}
/* OEM keeps values/addresses/count/result at FP+0x38/+0x30/+0x2c/+0x28.
 * GCC's stack-protector pass reverses these private slots without a canary. */
__attribute__((optimize("-fno-stack-protector")))
int toshiba_exit_readretry(struct samsung_set_chip *chip)
{
	unsigned char *values;
	unsigned char *addresses;
	int count;
	int result;

	if (toshiba_read_retry_mode != 16 &&
	    toshiba_read_retry_mode != 17 &&
	    toshiba_read_retry_mode != 18)
		return -88;
	nand_enable_chip(chip);
	ndfc_disable_randomize(chip->controller);
	if (ndfc_is_toggle_interface(chip->controller)) {
		ndfc_set_legacy_interface(chip->controller);
		toshiba_toggle_mode_flag_recovered = 1;
	}
	if (toshiba_read_retry_mode == 17) {
		values = toshiba_p1_2 + 35;
		addresses = toshiba_read_retry_addr_2;
		count = 5;
	} else if (toshiba_read_retry_mode == 18) {
		values = toshiba_p1_3 + 50;
		addresses = toshiba_read_retry_addr_2;
		count = 5;
	} else {
		values = toshiba_p1_1 + 24;
		addresses = toshiba_read_retry_addr_1;
		/* OEM passes zero rather than the available register count in
		 * mode 16. Preserve this apparent reset-programming bug. */
		count = 0;
	}
	result = toshiba_vender_set_param(chip, values, addresses, count);
	/* OEM's global flag is sticky. A previous chip can force toggle
	 * restoration even when this call did not enter legacy mode. */
	if (toshiba_toggle_mode_flag_recovered == 1)
		ndfc_set_toggle_interface(chip->controller);
	nand_disable_chip(chip);
	return result;
}

struct reclaim_flag_zone {
	unsigned char before_complete[664];
	unsigned int complete;
};
void clean_read_reclaim_complete_flag(struct read_claim_node *head)
{
	struct read_claim_node *node = head;

	while (node) {
		/* OEM reads the zone pointer twice, destination before source.
		 * These volatile reads preserve the order even if another actor
		 * changes the node between them. Neither pointer is checked.
		 * A direct node->zone->complete assignment compiles to the stock
		 * size but GCC evaluates its source pointer first, losing the OEM
		 * destination-before-source behavior on a racing pointer change. */
		struct read_claim_zone *const volatile *slot =
			(struct read_claim_zone *const volatile *)&node->zone;
		struct reclaim_flag_zone *destination =
			(struct reclaim_flag_zone *)*slot;
		struct reclaim_flag_zone *source =
			(struct reclaim_flag_zone *)*slot;

		destination->complete = source->complete == 0;
		node = node->next;
	}
}
int set_nftl_read_claim_complete(struct read_claim_node *head,
				 struct read_claim_node *selected)
{
	struct read_claim_node *last;

	selected->zone->read_complete = 1;
	last = (struct read_claim_node *)get_last_nftl(
		(struct nftl_list_node *)head);
	if (!selected->next) {
		last->zone->settings->flag = 0;
		clean_read_reclaim_complete_flag(head);
	} else {
		struct read_claim_settings *destination =
			last->zone->settings;

		/* OEM increments the one-byte marker as a 32-bit value,
		 * so 0xff becomes 0x100 rather than wrapping to zero. Resolve
		 * the destination first, as the OEM does before reading marker. */
		destination->flag = selected->zone->need_claim + 1;
	}
	last->zone->settings->page = 0;
	return 0;
}

struct reclaim_cache_mode {
	unsigned char before_mode[8];
	unsigned char mode;
};
struct reclaim_page_zone {
	unsigned char before_cache[8];
	struct reclaim_cache_mode *cache;
	unsigned char before_marker[8];
	unsigned char marker;
	unsigned char before_pages[11];
	unsigned int pages;
};
struct logic_page_map_zone {
	unsigned char before_geometry[8];
	struct logic_page_geometry *geometry;
	unsigned char before_summary[16];
	unsigned int summary;
	unsigned int total_pages;
	unsigned char before_maps[32];
	unsigned int *maps[];
};
int malloc_logic_page_map(struct logic_page_map_zone *zone,
			  unsigned int remaining_pages)
{
	unsigned int index;
	unsigned int bytes;
	unsigned int chunk_pages = 262144;

	for (index = 0; index <= 24; index++)
		zone->maps[index] = 0;
	for (index = 0; index <= 24; index++) {
		if (remaining_pages >= chunk_pages) {
			bytes = chunk_pages << 2;
			zone->maps[index] = nftl_malloc(bytes);
			if (!zone->maps[index]) {
				/* OEM passes the index as the sole argument to a
				 * format expecting a string and an integer. */
				nand_print(nftl_ro_lc_471,
					   index);
				return -2;
			}
			remaining_pages -= chunk_pages;
			nand_memset(zone->maps[index], -1, bytes);
		} else {
			/* Stock allocates even for zero pages, so a zero-size
			 * allocation may decide success or failure. */
			bytes = remaining_pages << 2;
			zone->maps[index] = nftl_malloc(bytes);
			if (!zone->maps[index]) {
				nand_print(nftl_ro_lc_472,
					   index);
				return -2;
			}
			nand_memset(zone->maps[index], -1, bytes);
			remaining_pages = 0;
			break;
		}
	}
	/* The OEM leaves prior allocations in place on either failure path,
	 * and retains at most 25 chunks before reporting the remainder. */
	if (remaining_pages) {
		nand_print(nftl_ro_lc_473,
			   remaining_pages);
		return -2;
	}
	return 0;
}
struct logic_page_geometry {
	unsigned char before_divisor[8];
	unsigned char divisor;
};
struct crc_page_geometry {
	unsigned char before_page_size[8];
	unsigned char page_size_factor;
};
struct crc_page_zone {
	unsigned char before_geometry[8];
	struct crc_page_geometry *geometry;
	unsigned char before_map_length[544];
	unsigned int map_length;
};
static int crc_wrong(struct crc_page_zone *zone, const unsigned char *data,
	      unsigned char *oob)
{
	unsigned short stored = 0;
	unsigned short actual = 0;
	unsigned int length = 0;

	length = zone->geometry->page_size_factor << 9;
	if (is_nouse_page(oob))
		return 0;
	if (is_phy_mapping_page(oob))
		length = zone->map_length;
	actual = nand_crc16(-1, data, length);
	stored = (short)((short)oob[13] | (short)(oob[12] << 8));
	if (actual != stored) {
		nand_print(nftl_ro_lc_201,
			   actual, stored, length);
		return 1;
	}
	return 0;
}
struct poweroff_ecc_geometry {
	unsigned char before_crc_enabled[28];
	unsigned short crc_enabled;
};
struct poweroff_ecc_zone {
	unsigned char before_geometry[3448];
	struct poweroff_ecc_geometry *geometry;
};
int nand_power_off_ecc_error(struct poweroff_ecc_zone *zone,
			     const unsigned char *data, unsigned char *oob)
{
	int error = 0;

	/* OEM probes OOB before the unchecked zone/geometry pointer, then
	 * uses CRC only when the geometry flag is set. Keep this fault order. */
	if (oob[0] != 0xff) {
		nand_print(nftl_ro_lc_395);
		error = 1;
	} else if (zone->geometry->crc_enabled) {
		if (crc_wrong((struct crc_page_zone *)zone, data, oob)) {
			nand_print(nftl_ro_lc_396);
			error = 1;
		}
	} else if (oob[11] != 0xa5 || oob[15] != 0xa5) {
		nand_print(nftl_ro_lc_397);
		error = 1;
	}
	if (error == 1) {
		/* OEM prints its original source line and dumps 16 bytes of OOB;
		 * preserve that diagnostic even if the source line moves. */
		nand_print(nftl_ro_lc_398, nftl_ro_function_7666, 3046, error);
		nand_print(nftl_ro_lc_399);
		_DumpHexData(oob, 16);
	}
	return error;
}
unsigned int *get_logic_page_map(struct logic_page_map_zone *zone,
				 unsigned int page)
{
	unsigned int chunk_pages;
	unsigned int chunk;
	unsigned int *map;
	unsigned int *result;

	if (page >= zone->total_pages) {
		nand_print(nftl_ro_lc_470,
			   nftl_ro_func_8479, page, zone->total_pages);
		result = 0;
		return result;
	}
	chunk_pages = 262144;
	chunk = 0;
	/* OEM repeatedly subtracts rather than dividing. It trusts the map
	 * table entry, even when it is null or the zone count is corrupt. */
	while (page >= chunk_pages) {
		chunk++;
		page -= chunk_pages;
	}
	map = zone->maps[chunk];
	result = map + page;
	return result;
}

struct logic_page_print_entry {
	unsigned short code;
	unsigned short marker;
};
/* Recovery writes the low 12 bits as an OEM bitfield assignment. */
struct logic_page_recover_entry {
	unsigned short page : 12;
	unsigned short read_flag : 1;
	unsigned short preserve : 3;
	unsigned short marker;
};
int recover_logic_page_mapping(struct logic_page_map_zone *zone,
			       struct invalid_page_node *current,
			       const unsigned int *pages, unsigned int count)
{
	unsigned int index;

	for (index = 0; index < count; index++) {
		unsigned int page = pages[index];

		if (page < zone->total_pages) {
			struct logic_page_recover_entry *entry =
				(struct logic_page_recover_entry *)
				get_logic_page_map(zone, page);
			if (!entry) {
				nand_print(nftl_ro_lc_273);
				current->invalid_pages++;
				continue;
			}
			if (entry->marker != 0xffff) {
				struct invalid_page_node *previous =
					(struct invalid_page_node *)
					get_phy_block_addr(
						(struct nftl_phy_outer *)zone,
						entry->marker);
				if (previous != current)
					phy_block_from_invalid_page_incr(
						(struct invalid_page_zone *)zone,
						previous);
				else
					previous->invalid_pages++;
				if (previous->invalid_pages >
				    ((struct page_geometry *)zone->geometry)->pages_per_block)
					nand_print(nftl_ro_lc_274,
						   previous->number);
			}
			/* OEM replaces only the low 12 bits of the map code. */
			entry->page = (unsigned short)index;
			entry->marker = current->number;
		} else {
			if (page != 0xffffffff)
				nand_print(nftl_ro_lc_275,
					   current->number, index, page);
			current->invalid_pages++;
			if (current->invalid_pages >
			    ((struct page_geometry *)zone->geometry)->pages_per_block)
				nand_print(nftl_ro_lc_276);
		}
	}
	return 0;
}

struct logic_read_geometry {
	unsigned char before_erased_size[26];
	unsigned short erased_size;
	unsigned short page_size;
};
struct logic_read_zone {
	unsigned char before_geometry[8];
	struct logic_read_geometry *geometry;
	unsigned char before_read_flag[648];
	unsigned int read_flag;
	unsigned char before_read[2796];
	int (*read)(struct logic_read_zone *, struct physic_op_param *);
};
/* Stock has a stack OOB array but no canary in this body. */
__attribute__((optimize("-fno-stack-protector")))
int _nftl_nand_read_logic_page(struct logic_read_zone *zone,
			       unsigned int page, void *data)
{
	struct logic_page_recover_entry *entry =
		(struct logic_page_recover_entry *)get_logic_page_map(
			(struct logic_page_map_zone *)zone, page);
	/* OEM keeps op at FP+0x30 and OOB at FP+0x48. The read callback can
	 * overrun this 16-byte OOB buffer into later private stack slots. */
	unsigned char oob[16];
	struct physic_op_param op;
	int result;

	if (!entry) {
		nand_print_dbg(nftl_ro_lc_147);
		return 1;
	}
	if (entry->marker == 0xffff) {
		nand_memset(data, -1, zone->geometry->erased_size);
		return 0;
	}
	/* OEM edits the map flag before reading and leaves it changed even if
	 * the read fails. Only bit four is replaced. */
	entry->read_flag = ((unsigned char)zone->read_flag) & 1;
	set_physic_op_par(&op, entry->marker, entry->page,
			zone->geometry->page_size, data, oob);
	result = zone->read(zone, &op);
	if (result == -2) {
		void *physical = get_phy_block_addr(
			(struct nftl_phy_outer *)zone, entry->marker);
		add_prio_gc((struct prio_gc_owner *)zone, physical, 1);
		nand_print(nftl_ro_lc_148, page);
	}
	/* OEM performs priority GC after every nonempty read and ignores its
	 * result, including when the read itself has already failed. */
	do_prio_gc(zone);
	return result;
}

int print_logic_page_map(struct logic_page_map_zone *zone, unsigned int ignored)
{
	/* OEM keeps the map entry, total and index at FP+0x20/+0x28/+0x2c.
	 * Grouping the private locals preserves those callback-visible slots. */
	struct {
		struct logic_page_print_entry *entry;
		unsigned int total;
		unsigned int index;
	} local;

	local.total = zone->summary / zone->geometry->divisor;
	local.index = 0;

	/* OEM divides by the unchecked geometry byte; AArch64 UDIV produces
	 * zero when it is zero. It also walks unchecked map pointers. */
	while (local.index < local.total) {
		local.entry = (struct logic_page_print_entry *)
			get_logic_page_map(zone, local.index);
		if (!local.entry)
			nand_print(nftl_ro_lc_431);
		else if (local.entry->marker != 0xffff)
			nand_print_dbg(nftl_ro_lc_432, local.index,
				       local.entry->marker,
				       local.entry->code & 0xfff,
				       (((unsigned char *)local.entry)[1] >> 4) & 1);
		local.index++;
	}
	return 0;
}
struct cold_logic_zone {
	unsigned char before_flag[664];
	unsigned int flag;
};
struct cold_map_entry {
	unsigned char first;
	unsigned char lower:4;
	unsigned char cold:1;
	unsigned char upper:3;
	unsigned short marker;
};
/* Stock uses the no-stack-protector slot order for current and entry. Its
 * extra UXTB before masking the low cold bit is retained as a documented
 * instruction-level difference: it cannot affect the stored low bit. */
__attribute__((optimize("-fno-stack-protector")))
int get_cold_logic_page(void *zone, int page, unsigned int pages)
{
	unsigned int current = (unsigned int)page;
	struct cold_map_entry *entry;

	while (current < pages) {
		entry = (struct cold_map_entry *)get_logic_page_map(
			(struct logic_page_map_zone *)zone, current);
		if (!entry) {
			nand_print(nftl_ro_lc_431);
		} else if (entry->marker != 0xffff &&
			   entry->cold !=
			   ((struct cold_logic_zone *)zone)->flag) {
			/* OEM compares the whole flag but inserts only its low bit;
			 * values above one can cause repeated false mismatches. */
			entry->cold = ((struct cold_logic_zone *)zone)->flag;
			return current;
		}
		current++;
	}
	return -1;
}
extern int nand_cache_read(struct nand_cache_read_zone *zone, int page,
			   unsigned short mode,
			   void *buffer);
/* Keep the OEM's private read/claim slots; the byte truncation and ignored
 * callback results below are intentional stock behavior. */
__attribute__((optimize("-fno-stack-protector")))
int read_reclaim(struct read_claim_node *head,
		 struct read_claim_node *selected, void *buffer)
{
	int cold_page;
	int claim_page;
	int result;

	result = 0;
	claim_page = get_nftl_read_claim_page_no(head);
	cold_page = get_cold_logic_page(selected->zone, claim_page,
		((struct reclaim_page_zone *)selected->zone)->pages);

	if (cold_page == -1) {
		result = set_nftl_read_claim_complete(head, selected);
	} else {
		/* OEM discards cache-read and flag-set errors, and follows every
		 * private pointer without validation. */
		nand_cache_read((struct nand_cache_read_zone *)selected->zone,
			cold_page,
			((struct reclaim_page_zone *)selected->zone)->cache->mode,
			buffer);
		set_nftl_read_claim_flag(head,
			((struct reclaim_page_zone *)selected->zone)->marker,
			cold_page);
	}
	return result;
}

struct page_map_write_geometry {
	unsigned char before_pages[20];
	unsigned short pages;
};
struct page_map_write_block {
	unsigned char before_used_pages[4];
	unsigned short used_pages;
};
struct page_map_write_current {
	struct page_map_write_block *block;
	void *data;
	unsigned char before_destination[24];
	void *destination;
	unsigned char before_page[8];
	unsigned short page;
};
struct page_map_write_zone {
	unsigned char before_geometry[8];
	struct page_map_write_geometry *geometry;
	unsigned char before_source[552];
	void *source;
	unsigned char before_current[16];
	struct page_map_write_current current;
};
struct phy_write_zone;
struct phy_write_state;
extern int write_phy_data_to_current_block(struct phy_write_zone *,
					   struct phy_write_state *,
					   const unsigned char *,
					   unsigned int, unsigned int);
struct invalid_page_map_data {
	unsigned char before_pages[16];
	unsigned int *pages;
};
struct invalid_page_location {
	unsigned short page : 12;
	unsigned short flags : 4;
	unsigned short block;
};
int check_invalid_page_right(struct page_map_write_zone *zone,
			     void *raw_map,
			     struct page_map_write_block *block,
			     unsigned short page_count)
{
	struct invalid_page_location *location;
	unsigned int logic_page;
	unsigned int invalid = 0;
	unsigned int page = 0;

	while (page_count > page) {
		logic_page = ((struct invalid_page_map_data *)raw_map)->pages[page];
		if (logic_page != ~0U) {
			location = (struct invalid_page_location *)
				get_logic_page_map((struct logic_page_map_zone *)zone,
						   logic_page);
			if (!location) {
				nand_print_dbg(nftl_ro_lc_147);
				invalid++;
			} else if (location->block != *(unsigned short *)block ||
			   location->page != page) {
				invalid++;
			}
		} else {
			invalid++;
		}
		page++;
	}
	if (block->used_pages != invalid) {
		/* OEM misspells "invalid"; its "used_page" field is the input
		 * page count, while the second bracket is block->used_pages. */
		nand_print(nftl_ro_lc_175,
			   invalid, block->used_pages, page_count);
		return 1;
	}
	return 0;
}
int write_phy_page_map_to_current_block(struct page_map_write_zone *zone,
					struct page_map_write_current *current)
{
	int result;

	if (current == &zone->current)
		nand_memcpy(zone->current.destination, zone->source, 320);
	result = write_phy_data_to_current_block(
					(struct phy_write_zone *)zone,
					(struct phy_write_state *)current,
					current->data, 0, 0xaaaaffff);
	if (result) {
		nand_print(nftl_ro_lc_166);
		return result;
	}
	current->page++;
	current->block->used_pages++;
	/* OEM ignores the invalid-page validation result and subsequent list
	 * operation results after a successful page-map write. */
	check_invalid_page_right(zone, &current->data, current->block,
				 current->page);
	if (current->page == zone->geometry->pages) {
		put_phy_block_to_invalid_page_list(
			zone, (struct invalid_node *)current->block);
		adjust_invaild_list((struct move_zone *)zone);
	}
	return 0;
}

struct do_after_zone {
	unsigned char before_flags[25];
	unsigned char flags;
	unsigned char before_current[502];
	unsigned char current[64];
	unsigned char page_map[76];
	unsigned int completed;
};
struct get_new_zone;
struct get_new_current_entry;
extern int get_new_current_block(struct get_new_zone *,
				 struct get_new_current_entry *);
int do_before_write(void *owner)
{
	struct do_after_zone *zone = owner;
	struct new_write_block *block;

	while (current_block_at_first_page(owner,
			(struct nftl_current_block *)zone->current) == 1) {
		block = *(struct new_write_block **)zone->current;
		if (!new_block_init_for_write((struct new_write_zone *)zone,
				block, block->tag))
			break;
		/* The OEM decrements the tag after a failed erase, including its
		 * unsigned wraparound, and decrements it twice in dual-slot mode.
		 * A corrected implementation would guard against underflow. */
		block->tag--;
		if (zone->flags & 1)
			block->tag--;
		if (get_new_current_block((struct get_new_zone *)zone,
				(struct get_new_current_entry *)zone->current)) {
			nand_print(nftl_ro_lc_156);
			return 1;
		}
	}
	if (zone->flags & 1) {
		/* Stock tests exactly one above, but any nonzero here. */
		while (current_block_at_first_page(owner,
				(struct nftl_current_block *)zone->page_map)) {
			block = *(struct new_write_block **)zone->page_map;
			if (!new_block_init_for_write((struct new_write_zone *)zone,
				block, block->tag))
				break;
			block->tag--;
			if (zone->flags & 1)
				block->tag--;
			if (get_new_current_block((struct get_new_zone *)zone,
				(struct get_new_current_entry *)zone->page_map)) {
				nand_print(nftl_ro_lc_157);
				return 1;
			}
		}
	}
	return 0;
}
int do_after_write(struct do_after_zone *zone)
{
	if (current_block_at_last_page((struct current_zone *)zone,
				       (struct current_page *)zone->current) == 1) {
		if (write_phy_page_map_to_current_block(
				(struct page_map_write_zone *)zone,
				(struct page_map_write_current *)zone->current)) {
			nand_print(nftl_ro_lc_158);
			return 1;
		}
		if (zone->flags & 1) {
			/* OEM logs the second map-write error and continues. */
			if (write_phy_page_map_to_current_block(
					(struct page_map_write_zone *)zone,
					(struct page_map_write_current *)zone->page_map))
				nand_print(nftl_ro_lc_158);
		}
		if (get_new_current_block((struct get_new_zone *)zone,
				(struct get_new_current_entry *)zone->current)) {
			nand_print(nftl_ro_lc_159);
			return 1;
		}
		if (zone->flags & 1) {
			if (get_new_current_block((struct get_new_zone *)zone,
				    (struct get_new_current_entry *)zone->page_map)) {
				nand_print(nftl_ro_lc_160);
				return 1;
			}
		}
		zone->completed++;
	}
	return 0;
}

int fill_no_use_data_to_page(void *owner)
{
	struct do_after_zone *zone = owner;
	if (current_block_at_last_page((struct current_zone *)zone,
				       (struct current_page *)zone->current)) {
		if (write_phy_page_map_to_current_block(
				(struct page_map_write_zone *)zone,
				(struct page_map_write_current *)zone->current)) {
			nand_print(nftl_ro_lc_158);
			return 1;
		}
		if (zone->flags & 1) {
			/* OEM only logs a failure for the secondary page map. */
			if (write_phy_page_map_to_current_block(
					(struct page_map_write_zone *)zone,
					(struct page_map_write_current *)zone->page_map))
				nand_print(nftl_ro_lc_158);
		}
		if (get_new_current_block((struct get_new_zone *)zone,
				(struct get_new_current_entry *)zone->current)) {
			nand_print(nftl_ro_lc_159);
			return 1;
		}
		if (zone->flags & 1) {
			if (get_new_current_block((struct get_new_zone *)zone,
					(struct get_new_current_entry *)zone->page_map)) {
				nand_print(nftl_ro_lc_160);
				return 1;
			}
		}
		return 0;
	}
	if (current_block_at_first_page(zone,
					(struct nftl_current_block *)zone->current))
		return 0;
	/* OEM ignores page-map failures on a non-last page and returns eight
	 * even if both writes failed. The diagnostic is the only indication. */
	if (write_phy_page_map_to_current_block(
			(struct page_map_write_zone *)zone,
			(struct page_map_write_current *)zone->current))
		nand_print(nftl_ro_lc_158);
	if (zone->flags & 1) {
		if (write_phy_page_map_to_current_block(
				(struct page_map_write_zone *)zone,
				(struct page_map_write_current *)zone->page_map))
			nand_print(nftl_ro_lc_158);
	}
	return 8;
}

extern int physic_info_get_offset(unsigned int *offset);
extern int physic_info_get_one_copy(unsigned int block, unsigned int offset,
				    unsigned int *scratch, unsigned char *buffer);
int hynix16nm_get_special_info(unsigned char *buffer, void *destination,
			      unsigned int length, unsigned int copy_index)
{
	unsigned int scratch;
	unsigned int offset;
	unsigned int block;
	unsigned int index;
	unsigned char checksum;
	int offset_result;
	int result;

	nand_print_dbg(nftl_ro_lc_107);
	offset_result = physic_info_get_offset(&offset);
	if (offset_result) {
		nand_print(nftl_ro_lc_108);
		return 1;
	}
	/* OEM only initializes the search failure result after the offset
	 * lookup succeeds; the failed lookup returns one directly. */
	result = -1;
	for (block = 4; block <= 50; block++) {
		nand_print_dbg(nftl_ro_lc_109, block);
		/* OEM ignores a failed copy helper and judges only the header in
		 * the shared buffer, which may still hold earlier contents. */
		if (buffer[8192 + 1536] != 0xa5)
			physic_info_get_one_copy(block, offset, &scratch, buffer);
		if (buffer[8192 + 1536] != 0xa5)
			continue;
		checksum = 0;
		for (index = 2; index <= 1023; index++)
			checksum += buffer[8192 + 1536 + index];
		if (checksum == buffer[8192 + 1537]) {
			result = 0;
			/* OEM performs 32-bit index arithmetic and never bounds-checks
			 * the resulting payload address or requested length. */
			nand_memcpy(destination,
				    buffer + 8192 + 1536 +
				    (unsigned int)(copy_index * length + 2), length);
			break;
		}
		buffer[8192 + 1536] = 0;
	}
	return result;
}

struct get_new_geometry {
	unsigned char before_limit[16];
	unsigned short erase_limit;
};
struct get_new_block_info {
	unsigned short number;
	unsigned char before_erase[4];
	unsigned short erase_count;
	unsigned int sequence;
};
struct get_new_current_entry {
	struct get_new_block_info *block;
	unsigned char before_buffer[16];
	void *buffer;
	unsigned int buffer_size;
	unsigned char before_page[20];
	unsigned short page;
};
struct get_new_zone {
	unsigned char before_geometry[8];
	struct get_new_geometry *geometry;
	unsigned char before_flags[9];
	unsigned char flags;
	unsigned char before_wear[1036];
	unsigned short wear;
};
int get_new_current_block(struct get_new_zone *zone,
			  struct get_new_current_entry *current)
{
	unsigned int previous = current->block->sequence;

	/* OEM dereferences the old block before checking it and clears the
	 * buffer even if no replacement free block can be found. */
	nand_memset(current->buffer, -1, current->buffer_size);
	if (zone->wear) {
		current->block = (struct get_new_block_info *)
			out_phy_block_from_free_list_by_erase_time(
				(struct nftl_free_erase_owner *)zone,
				zone->geometry->erase_limit);
		if (current->block)
			nand_print_dbg(nftl_ro_lc_172,
				       current->block->number,
				       current->block->erase_count);
	} else {
		current->block = (struct get_new_block_info *)
			out_phy_block_from_free_list((struct free_block_zone *)zone);
	}
	if (!current->block) {
		nand_print(nftl_ro_lc_173);
		return 1;
	}
	current->page = 0;
	current->block->sequence = previous + 1;
	if (zone->flags & 1)
		current->block->sequence++;
	return 0;
}

struct nand_discard_geometry {
	unsigned char before_sectors_per_page[8];
	unsigned char sectors_per_page;
};
struct nand_discard_view {
	unsigned char before_geometry[8];
	struct nand_discard_geometry *geometry;
	unsigned char before_sector_count[16];
	unsigned int sector_count;
	unsigned char before_cache[1072 - 36];
	unsigned char cache[3440 - 1072];
	struct smart_print_payload *smart;
	unsigned short *write_cache_active;
	unsigned char before_discard[3520 - 3456];
	void (*discard_page)(struct nand_discard_view *, unsigned int);
};
_Static_assert(__builtin_offsetof(struct nand_discard_view, cache) == 1072,
	       "discard cache offset");
_Static_assert(__builtin_offsetof(struct nand_discard_view, smart) == 3440,
	       "discard SMART offset");
_Static_assert(__builtin_offsetof(struct nand_discard_view, discard_page) == 3520,
	       "discard callback offset");
struct nand_discard_frame {
	void *node;
	unsigned int unused;
	unsigned short first_page_sectors;
	unsigned short remainder;
	struct cache_write_zone *cache;
	int result;
	unsigned int page;
};
int nand_discard(struct nand_discard_view *zone, unsigned int sector,
		 unsigned int count)
{
	struct nand_discard_frame frame;

	/* Keep OEM's private slots and its single reused cache-node pointer. */
	frame.result = 0;
	frame.cache = (struct cache_write_zone *)zone->cache;

	/* OEM's 32-bit addition wraps, allowing an out-of-range request through. */
	if (sector + count > zone->sector_count) {
		nand_print(nftl_ro_lc_430);
		return 1;
	}
	zone->smart->recv_discard_sectors += count;
	frame.page = sector / zone->geometry->sectors_per_page;
	frame.remainder = sector % zone->geometry->sectors_per_page;
	frame.first_page_sectors = zone->geometry->sectors_per_page - frame.remainder;
	if (frame.remainder) {
		frame.page++;
		if (frame.first_page_sectors > count)
			count = 0;
		else
			count -= frame.first_page_sectors;
	} else if (zone->geometry->sectors_per_page > count) {
		count = 0;
	}
	count /= zone->geometry->sectors_per_page;
	if (!count)
		return 0;
	do {
		if (*zone->write_cache_active == 0) {
			frame.node = search_from_cache_write_list(frame.cache, frame.page);
			if (frame.node)
				del_from_cache_write_list(
					(struct cache_list_owner *)frame.cache,
					(struct cache_list_node *)frame.node);
			frame.node = search_from_cache_read_list(
					(struct cache_read_zone *)frame.cache, frame.page);
			if (frame.node)
				del_from_cache_read_list(
					(struct cache_read_delete_owner *)frame.cache,
					(struct cache_read_delete_node *)frame.node);
		}
		/* OEM ignores callback failures and still counts the page. */
		zone->discard_page(zone, frame.page);
		count--;
		frame.page++;
		zone->smart->real_discard_sectors +=
			zone->geometry->sectors_per_page;
	} while (count);
	return frame.result;
}

struct no_use_geometry {
	unsigned char before_sectors[8];
	unsigned char sectors_per_page;
	unsigned char before_pages[11];
	unsigned short pages_per_block;
	unsigned char before_page_size[6];
	unsigned short page_size;
};
struct no_use_block {
	unsigned short number;
	unsigned short state;
	unsigned char remaining[60];
};
struct no_use_zone {
	unsigned char before_geometry[8];
	struct no_use_geometry *geometry;
	unsigned char before_scanned[10];
	unsigned short scanned;
	unsigned char before_sector_count[4];
	unsigned int sector_count;
	unsigned char before_offset[4];
	unsigned int offset;
	struct no_use_block *blocks;
	struct no_use_block *current;
	unsigned char before_data[512 - 64];
	void *data;
	unsigned char before_read[3464 - 520];
	int (*read)(struct no_use_zone *, struct physic_op_param *);
	unsigned char before_check[3488 - 3472];
	int (*check)(struct no_use_zone *, struct physic_op_param *);
};
_Static_assert(sizeof(struct no_use_block) == 64, "no-use block stride");
_Static_assert(__builtin_offsetof(struct no_use_zone, scanned) == 26,
	       "no-use scan count");
_Static_assert(__builtin_offsetof(struct no_use_zone, read) == 3464,
	       "no-use read callback");
_Static_assert(__builtin_offsetof(struct no_use_zone, check) == 3488,
	       "no-use block callback");
__attribute__((optimize("-fno-stack-protector")))
int is_no_use_device(struct no_use_zone *zone, unsigned int expected_size)
{
	struct no_use_block *block = zone->blocks;
	struct physic_op_param op;
	unsigned char oob[16];
	unsigned int limit;
	int read_result;

	/* OEM divides by both geometry fields without checking for zero.
	 * AArch64 UDIV returns zero on a zero divisor in the shipped build. */
	if (zone->sector_count)
		limit = ((zone->sector_count + zone->offset) /
			 zone->geometry->sectors_per_page) /
			zone->geometry->pages_per_block;
	else
		limit = -1U;
	zone->scanned = 0;
	while (zone->scanned < limit) {
		zone->current = block;
		set_physic_op_par(&op, block->number, 0,
				   zone->geometry->page_size, zone->data, oob);
		if (zone->check(zone, &op) == 1) {
			read_result = zone->read(zone, &op);
			if (is_ftl_start_flag_page(oob) == 1 &&
			    (read_result == 0 || read_result == 10)) {
				if (get_spare_data(oob, 3, 4) != expected_size) {
					nand_print(nftl_ro_lc_208);
					return 1;
				}
				zone->blocks = (struct no_use_block *)
					((unsigned char *)block + 64);
				return 0;
			}
			return 1;
		}
		/* OEM writes 0x00ff, not 0xffff, and wraps its 16-bit scan
		 * counter; a never-ending scan is possible when the limit exceeds
		 * 65535 and no block is marked last. */
		block->state = 255;
		zone->scanned++;
		if (is_last_phy_block((struct physical_block *)block,
				      (struct physical_zone *)zone) == 1)
			break;
		block = (struct no_use_block *)((unsigned char *)block + 64);
	}
	return 1;
}

struct garbage_collect_geometry {
	unsigned char before_pages[20];
	unsigned short pages_per_block;
};
struct garbage_collect_node {
	unsigned char before_invalid[4];
	unsigned short invalid_pages;
};
struct garbage_collect_state {
	unsigned char before_limit[16];
	unsigned short free_limit;
};
struct garbage_collect_zone {
	unsigned char before_geometry[8];
	struct garbage_collect_geometry *geometry;
	unsigned char before_free_blocks[14];
	unsigned short free_blocks;
	unsigned char before_candidate[320];
	struct garbage_collect_node *candidate;
	unsigned char before_limits[320];
	unsigned short reset_limit;
	unsigned short start_limit;
	unsigned short active;
	unsigned short frequency;
	unsigned short request;
	unsigned char before_state[3448 - 690];
	struct garbage_collect_state *state;
};
_Static_assert(__builtin_offsetof(struct garbage_collect_zone, candidate) == 352,
	       "garbage candidate offset");
_Static_assert(__builtin_offsetof(struct garbage_collect_zone, request) == 688,
	       "garbage request offset");
_Static_assert(__builtin_offsetof(struct garbage_collect_zone, state) == 3448,
	       "garbage state offset");
int garbage_collect(struct garbage_collect_zone *zone)
{
	unsigned short request;
	unsigned short invalid_pages;
	unsigned short pages_per_block;

	if (zone->free_blocks <= zone->reset_limit)
		zone->active = 0;
	gc_src_type = 3;
	if (zone->active)
		return 0;
	if (zone->free_blocks >= zone->start_limit) {
		zone->active = 1;
		zone->frequency = 0;
		zone->request = 0;
		return 0;
	}
	if (!zone->frequency) {
		if (!zone->candidate) {
			nand_print(nftl_ro_lc_433);
			return 1;
		}
		invalid_pages = zone->candidate->invalid_pages;
		pages_per_block = zone->geometry->pages_per_block;
		if (invalid_pages >= pages_per_block >> 1)
			zone->request = 4;
		else if (invalid_pages >= pages_per_block >> 2)
			zone->request = 8;
		else if (invalid_pages >= pages_per_block >> 3)
			zone->request = 16;
		else {
			nand_print_dbg(nftl_ro_lc_434,
				       zone->free_blocks, invalid_pages);
			zone->request = 0xffff;
		}
		if (zone->free_blocks <= zone->state->free_limit)
			zone->request = 0xffff;
	}
	request = zone->request;
	if (request == 0xffff) {
		/* OEM ignores the result of the fallback GC attempt. */
		gc_win_block((struct gc_win_zone *)zone, 1);
		return 0;
	}
	if (garbage_collect_first((struct gc_one_zone *)zone, 0, request)) {
		nand_print(nftl_ro_lc_435,
			   zone->free_blocks);
		return 1;
	}
	return 0;
}

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
struct page_map_recovery_zone;
extern int recover_block_phy_page_mapping(struct page_map_recovery_zone *,
					   struct new_write_block *,
					   unsigned int *);

__attribute__((optimize("-fno-stack-protector")))
int garbage_collect_first(struct gc_one_zone *raw_zone, void *requested,
			  int requested_limit)
{
	struct gc_first_zone *zone = (struct gc_first_zone *)raw_zone;
	struct gc_first_block *block;
	struct invalid_page_location *entry;
	struct physic_op_param op;
	unsigned char oob[16];
	unsigned short limit = requested_limit;
	unsigned int page, logic, observed;
	int status, bad = 0;

	nand_memset(oob, 0x5a, 16);
	if (!requested)
		block = (struct gc_first_block *)
			out_phy_block_from_invalid_page_list((struct move_zone *)zone);
	else {
		if (is_current_blcok((struct diag_current_zone *)zone,
				      requested) == 1)
			return 0;
		block = (struct gc_first_block *)
			out_phy_block_from_invalid_page_list_by_block(
				(struct invalid_zone *)zone,
				(struct invalid_node *)requested);
	}
	if (!block) {
		nand_print(nftl_ro_lc_436);
		return 8;
	}
	zone->stats->runs++;
	set_physic_op_par(&op, block->number, zone->geometry->pages - 1,
			  zone->geometry->fixed_page, zone->data, oob);
	status = zone->read(zone, &op);
	if (is_phy_mapping_page(oob) != 1) {
		nand_print_dbg(nftl_ro_lc_437);
		nand_print_dbg(nftl_ro_lc_438,
			       block->number, zone->geometry->pages - 1, 0,
			       zone->geometry->fixed_page);
		/* Stock supplies only fifteen values for sixteen %x conversions.
		 * The final diagnostic word comes from its uninitialized frame. */
		nand_print_dbg(nftl_ro_lc_439,
			       oob[1], oob[2], oob[3], oob[4], oob[5],
			       oob[6], oob[7], oob[8], oob[9], oob[10],
			       oob[11], oob[12], oob[13], oob[14], oob[15]);
		nand_print_dbg(nftl_ro_lc_440,
			       zone->mode, block->number);
		recover_block_phy_page_mapping(
			(struct page_map_recovery_zone *)zone,
			(struct new_write_block *)block, zone->map.pages);
		bad = 1;
	} else if (status && status != 10) {
		nand_print_dbg(nftl_ro_lc_441,
			       block->number);
		recover_block_phy_page_mapping(
			(struct page_map_recovery_zone *)zone,
			(struct new_write_block *)block, zone->map.pages);
		bad = 1;
	} else {
		nand_memcpy(zone->map.pages, zone->data, zone->map.bytes);
	}
	status = check_invalid_page_right((struct page_map_write_zone *)zone,
					  &zone->map,
					  (struct page_map_write_block *)block,
					  zone->geometry->pages);
	if (status) {
		nand_print(nftl_ro_lc_442);
		bad = 1;
	}
	if (bad)
		limit = 0xffff;
	for (page = 0; page < (unsigned int)zone->geometry->pages - 1;
	     page++) {
		logic = zone->map.pages[page];
		if (logic == ~0U)
			continue;
		entry = (struct invalid_page_location *)
			get_logic_page_map((struct logic_page_map_zone *)zone, logic);
		if (!entry || entry->block != block->number ||
		    entry->page != page)
			continue;
		zone->stats->copied_pages++;
		set_physic_op_par(&op, block->number, page,
				  zone->geometry->fixed_page,
				  zone->logic_buffer, oob);
		/* OEM's retry counter is initialized to zero and never changed;
		 * only one physical read is made. */
		status = zone->read(zone, &op);
		observed = get_logic_page_from_oob(oob);
		if (status == 0 || status == 10) {
			if (observed != logic) {
				nand_print_dbg(nftl_ro_lc_443,
					       block->number, page, observed, logic);
				nand_print(nftl_ro_lc_444,
					   oob[0], oob[1], oob[2], oob[3],
					   oob[4], oob[5], oob[6], oob[7],
					   oob[8], oob[9], oob[10], oob[11],
					   oob[12], oob[13], oob[14], oob[15]);
				entry->block = 0xffff;
				entry->page = 0xfff;
				bad = 1;
				limit = 0xffff;
				block->invalid_pages++;
			} else if (nand_write_logic_page_no_gc(zone, logic,
						      zone->logic_buffer)) {
				nand_print(nftl_ro_lc_445);
				return 1;
			}
		} else {
			block->invalid_pages++;
			bad = 1;
			limit = 0xffff;
		}
		limit--;
		if (!limit)
			break;
	}
	if (page == (unsigned int)zone->geometry->pages - 1)
		zone->active = 0;
	if (bad && page == (unsigned int)zone->geometry->pages - 1) {
		nand_print(nftl_ro_lc_446);
		erase_block((struct erase_block_zone *)zone,
			    (struct erase_block_info *)block,
			    (unsigned short)(block->erase_count + 1));
	} else {
		put_phy_block_to_invalid_page_list(zone,
					(struct invalid_node *)block);
		adjust_invaild_list((struct move_zone *)zone);
	}
	return 0;
}

struct sandisk_first_request {
	unsigned int channel;
	unsigned int block;
	unsigned int page;
};
struct sandisk_first_command {
	unsigned int first_index;
	unsigned int before_first_wait;
	unsigned int first_wait;
	unsigned int first_code;
	unsigned int first_valid;
	unsigned int first_count;
	unsigned char before_second[80 - 24];
	unsigned int second_wait;
	unsigned int second_code;
	unsigned int second_valid;
	unsigned int second_count;
	unsigned char address[16];
	unsigned int address_cycles;
	unsigned char before_third[152 - 116];
	unsigned int third_wait;
	unsigned int third_code;
	unsigned int third_valid;
	unsigned int third_count;
};
struct sandisk_first_controller {
	unsigned char before_channel[12];
	unsigned int channel;
	unsigned int chips;
	unsigned char before_commands[40 - 20];
	struct sandisk_first_command commands;
};
struct sandisk_first_chip {
	unsigned char before_index[28];
	unsigned int index;
	unsigned int blocks;
	unsigned char before_page_shift[44 - 36];
	unsigned int page_shift;
	unsigned char before_controller[272 - 48];
	struct sandisk_first_controller *controller;
};
struct hynix_ff_request {
	unsigned int channel;
	unsigned int block;
	unsigned int page;
	unsigned int reserved;
	unsigned char *data;
};
struct hynix_ff_command {
	unsigned int index;
	unsigned int before_wait;
	unsigned int wait;
	unsigned int code;
	unsigned int valid;
	unsigned int before_address;
	unsigned char address[16];
	unsigned int address_cycles;
	unsigned int data_enable;
	unsigned int flag0;
	unsigned int flag1;
	unsigned int flag2;
	unsigned int transfer_length;
	unsigned char before_data[8];
	void *data;
	unsigned char before_final[12];
	unsigned int final;
};
struct hynix_ff_controller {
	unsigned char before_channel[12];
	unsigned int channel;
	unsigned int chips;
	unsigned char before_commands[20];
	struct hynix_ff_command commands;
};
struct hynix_ff_chip {
	unsigned char before_index[28];
	unsigned int index;
	unsigned int blocks;
	unsigned char before_page_shift[8];
	unsigned int page_shift;
	unsigned char before_controller[224];
	struct hynix_ff_controller *controller;
};
_Static_assert(__builtin_offsetof(struct hynix_ff_command, address) == 24,
	       "Hynix FF command address offset");
_Static_assert(__builtin_offsetof(struct hynix_ff_command, data) == 72,
	       "Hynix FF command buffer offset");
_Static_assert(__builtin_offsetof(struct hynix_ff_command, final) == 92,
	       "Hynix FF command final offset");
_Static_assert(__builtin_offsetof(struct hynix_ff_chip, controller) == 272,
	       "Hynix FF controller offset");
extern int nand_read_chip_status_ready(void *chip);
extern void ndfc_clean_cmd_seq(struct sandisk_first_command *commands);
extern unsigned int get_row_addr(unsigned int, unsigned int, unsigned int);
extern void fill_cmd_addr(unsigned int, unsigned int, unsigned int,
			  unsigned int, void *);
extern int ndfc_execute_cmd(void *, struct sandisk_first_command *);
extern void ndfc_disable_ecc(void *controller);
int hynix16nm_write_page_FF(struct hynix_ff_request *request,
			    unsigned int mode)
{
	unsigned int index = 0;
	unsigned int chunks = 0;
	unsigned int last_length = 0;
	int result = 0;
	unsigned int row = 0;
	unsigned int column = 0;
	struct hynix_ff_chip *chip = (struct hynix_ff_chip *)
		nci_get_from_nsi(g_nsi, request->channel);
	struct hynix_ff_controller *controller = chip->controller;
	struct hynix_ff_command *command = &chip->controller->commands;

	if (chip->index >= controller->chips || request->block >= chip->blocks) {
		nand_print(nftl_ro_lc_104,
			   controller->channel, chip->index, controller->chips,
			   request->block, chip->blocks);
		return -108;
	}
	nand_read_chip_status_ready(chip);
	nand_enable_chip((struct samsung_set_chip *)chip);
	ndfc_disable_randomize(chip->controller);
	ndfc_disable_ecc(chip->controller);
	ndfc_clean_cmd_seq((struct sandisk_first_command *)command);
	if (mode == 8) {
		chunks = 9;
		last_length = 832;
	} else if (mode == 16) {
		chunks = 18;
		last_length = 640;
	}
	/* Unknown modes send only the final command. Every execute result is
	 * discarded; OEM returns the final ready status without restoring ECC
	 * or randomization on the controller. */
	/* OEM emits separate first/last/middle command bodies and discards
	 * each execute status by overwriting result on the next operation. */
#define HYNIX_FF_PREPARE(opcode, pointer, length) do { \
		command->index = 0; \
		command->code = (opcode); \
		command->wait = 1; \
		command->valid = 1; \
		command->flag2 = 1; \
		command->flag0 = 1; \
		command->flag1 = 1; \
		command->data = (pointer); \
		command->transfer_length = (length); \
		command->data_enable = 1; \
} while (0)
	for (index = 0; index < chunks; index++) {
		if (index == 0) {
			HYNIX_FF_PREPARE(128, request->data, 1024);
			row = get_row_addr(chip->page_shift, request->block,
					   request->page);
			command->address_cycles = 5;
			fill_cmd_addr(column, 2, row, 3, command->address);
			result = ndfc_execute_cmd(chip->controller,
					(struct sandisk_first_command *)command);
		} else if (chunks - 1 == index) {
			HYNIX_FF_PREPARE(133, request->data + index * 1024,
					 last_length);
			command->address_cycles = 2;
			fill_cmd_addr(column, 2, row, 0, command->address);
			result = ndfc_execute_cmd(chip->controller,
					(struct sandisk_first_command *)command);
		} else {
			HYNIX_FF_PREPARE(133, request->data + index * 1024,
					 1024);
			command->address_cycles = 2;
			fill_cmd_addr(column, 2, row, 0, command->address);
			result = ndfc_execute_cmd(chip->controller,
					(struct sandisk_first_command *)command);
		}
		column += 1024;
	}
#undef HYNIX_FF_PREPARE
	command->index = 0;
	command->code = 16;
	command->wait = 1;
	command->valid = 1;
	command->final = 1;
	result = ndfc_execute_cmd(chip->controller,
				  (struct sandisk_first_command *)command);
	result = nand_read_chip_status_ready(chip);
	if (result)
		nand_print(nftl_ro_lc_112);
	nand_disable_chip((struct samsung_set_chip *)chip);
	return result;
}
_Static_assert(__builtin_offsetof(struct sandisk_first_command, address_cycles)
	       == 112, "first-burn address cycles");
_Static_assert(__builtin_offsetof(struct sandisk_first_command, third_count)
	       == 164, "first-burn last command");
_Static_assert(__builtin_offsetof(struct sandisk_first_chip, controller)
	       == 272, "first-burn controller offset");
extern int nand_read_chip_status_ready(void *);
extern void ndfc_clean_cmd_seq(struct sandisk_first_command *);
extern unsigned int get_row_addr(unsigned int, unsigned int, unsigned int);
extern void fill_cmd_addr(unsigned int, unsigned int, unsigned int,
			  unsigned int, void *);
extern int ndfc_execute_cmd(void *, struct sandisk_first_command *);
int sandisk_A19_check_bad_block_first_burn(
	struct sandisk_first_request *request)
{
	/* Keep the OEM's 0x28..0x4f private frame in source order. */
	struct {
		unsigned int before_result; /* Unused OEM FP+0x28 slot. */
		int result;
		struct sandisk_first_command *commands;
		struct sandisk_first_controller *controller;
		struct sandisk_first_chip *chip;
		int column;
		unsigned int row;
	} frame;

	/* OEM initializes the row and column slots, then discards the command
	 * status by replacing it with the final ready result. */
	frame.row = 0;
	frame.column = 0;
	frame.chip = (struct sandisk_first_chip *)
		nci_get_from_nsi(g_nsi, request->channel);
	frame.controller = frame.chip->controller;
	frame.commands = &frame.chip->controller->commands;

	/* OEM trusts g_nsi, chip and controller pointers without NULL checks. */
	if (frame.chip->index >= frame.controller->chips ||
	    request->block >= frame.chip->blocks) {
		nand_print(nftl_ro_lc_104,
			   frame.controller->channel, frame.chip->index,
			   frame.controller->chips,
			   request->block, frame.chip->blocks);
		return -10;
	}
	ndfc_disable_randomize(frame.chip->controller);
	nand_read_chip_status_ready(frame.chip);
	nand_enable_chip((struct samsung_set_chip *)frame.chip);
	ndfc_clean_cmd_seq(frame.commands);
	frame.commands->first_index = 0;
	frame.commands->first_code = 0xa2;
	frame.commands->first_wait = 1;
	frame.commands->first_valid = 1;
	frame.commands->first_count = 0;
	frame.commands->second_code = 0x80;
	frame.commands->second_wait = 1;
	frame.commands->second_valid = 1;
	frame.commands->second_count = 0;
	frame.row = get_row_addr(frame.chip->page_shift, request->block,
			 request->page);
	frame.commands->address_cycles = 5;
	fill_cmd_addr(frame.column, 2, frame.row, 3, frame.commands->address);
	frame.commands->third_code = 0x10;
	frame.commands->third_wait = 1;
	frame.commands->third_valid = 1;
	frame.commands->third_count = 1;
	/* OEM overwrites the command result with the second ready status. */
	frame.result = ndfc_execute_cmd(frame.chip->controller,
					frame.commands);
	frame.result = nand_read_chip_status_ready(frame.chip);
	nand_disable_chip((struct samsung_set_chip *)frame.chip);
	return frame.result;
}

extern int page_read(unsigned short, unsigned short, unsigned short,
		     unsigned int, void *, void *);
__attribute__((optimize("-fno-stack-protector")))
int nand_dbg_phy_read(unsigned short channel, unsigned short block,
		      unsigned short page)
{
	/* Stock reserves twenty stack bytes although it prints only sixteen. */
	unsigned char oob[20];
	unsigned char *data = nftl_malloc(4096);
	int result;

	/* OEM logs failed allocation but continues into page_read and then
	 * dereferences the NULL data pointer while formatting diagnostics. */
	if (!data)
		nand_print(nftl_ro_lc_382, nftl_ro_func_7558);
	result = page_read(channel, block, page, 8, data, oob);
	nand_print_dbg(nftl_ro_lc_383,
		       oob[0], oob[1], oob[2], oob[3],
		       oob[4], oob[5], oob[6], oob[7],
		       oob[8], oob[9], oob[10], oob[11],
		       oob[12], oob[13], oob[14], oob[15]);
	nand_print_dbg(nftl_ro_lc_384,
		       data[0], data[1], data[2], data[3],
		       data[4], data[5], data[6], data[7],
		       data[8], data[9], data[10]);
	nftl_free(data);
	return result;
}

struct nrc_geometry {
	unsigned char before_pages[20];
	unsigned short pages_per_block;
	unsigned char before_oob_bytes[4];
	unsigned short oob_bytes;
};
struct nrc_zone {
	unsigned char before_geometry[8];
	struct nrc_geometry *geometry;
};
int init_nrc(struct nrc_zone *zone, struct nrc_buffers **output)
{
	struct nrc_buffers *buffers;
	unsigned int pages;

	/* Stock zeros both private slots before the first allocation call. */
	pages = 0;
	buffers = 0;
	buffers = nftl_malloc(32);
	if (!buffers) {
		/* OEM leaves *output untouched for this first failure only. */
		nand_print(nftl_ro_lc_279, nftl_ro_func_7312, 1408);
		return -1;
	}
	nand_memset(buffers, 0, 32);
	pages = zone->geometry->pages_per_block;
	/* OEM uses wrapping 32-bit arithmetic for allocation sizes. */
	buffers->second = nftl_malloc((zone->geometry->oob_bytes + 64U) * pages);
	if (!buffers->second) {
		nand_print(nftl_ro_lc_280, nftl_ro_func_7312, 1417);
		goto fail;
	}
	buffers->third = nftl_malloc(pages * 4U);
	if (!buffers->third) {
		nand_print(nftl_ro_lc_281, nftl_ro_func_7312, 1422);
		goto fail;
	}
	nand_memset(buffers->third, -1, pages * 4U);
	buffers->first = nftl_malloc(pages);
	if (!buffers->first) {
		nand_print(nftl_ro_lc_282, nftl_ro_func_7312, 1428);
		goto fail;
	}
	nand_memset(buffers->first, 0, pages);
	*output = buffers;
	nand_print(nftl_ro_lc_283,
		   zone->geometry->pages_per_block, buffers,
		   buffers->second, buffers->third, buffers->first);
	return 0;

fail:
	/* The failure cleanup order differs from free_nrc. */
	if (buffers->second)
		nftl_free(buffers->second);
	if (buffers->third)
		nftl_free(buffers->third);
	if (buffers->first)
		nftl_free(buffers->first);
	nftl_free(buffers);
	*output = 0;
	return -1;
}

struct nand_dbg_zone_read {
	unsigned char before_read[3464];
	int (*read)(struct nand_dbg_zone_read *, struct physic_op_param *);
};
__attribute__((optimize("-fno-stack-protector")))
int nand_dbg_zone_phy_read(struct nand_dbg_zone_read *zone,
			   unsigned short block, unsigned short page)
{
	struct physic_op_param op;
	unsigned char oob[16];
	unsigned char *data = nftl_malloc(4096);
	unsigned char *physical;
	int result;

	/* OEM logs allocation failure but continues with a NULL data pointer;
	 * the later byte diagnostics can fault on that path. */
	if (!data)
		nand_print(nftl_ro_lc_382, nftl_ro_func_7569);
	physical = get_phy_block_addr((struct nftl_phy_outer *)zone, block);
	/* OEM passes the caller's page as the plane argument and fixes the
	 * physical page argument at eight. Preserve the swapped semantics. */
	set_physic_op_par(&op, *(unsigned short *)physical, page, 8, data, oob);
	result = zone->read(zone, &op);
	nand_print_dbg(nftl_ro_lc_383,
		       oob[0], oob[1], oob[2], oob[3],
		       oob[4], oob[5], oob[6], oob[7],
		       oob[8], oob[9], oob[10], oob[11],
		       oob[12], oob[13], oob[14], oob[15]);
	nand_print_dbg(nftl_ro_lc_384,
		       data[0], data[1], data[2], data[3],
		       data[4], data[5], data[6], data[7],
		       data[8], data[9], data[10]);
	nftl_free(data);
	return result;
}

struct hynix20nm_page_chip {
	unsigned char before_retry[184];
	unsigned int retry;
	unsigned char before_controller[272 - 188];
	void *controller;
};
/* GCC's stack-protector pass reorders private retry-state slots without
 * emitting a canary. Keep the OEM frame while retaining its retry quirks. */
__attribute__((optimize("-fno-stack-protector")))
int hynix20nm_read_page_end(struct samsung_read_request *request)
{
	int retry_count = 0;
	int result = 0;
	struct hynix20nm_page_chip *chip;

	chip = (struct hynix20nm_page_chip *)
		nci_get_from_nsi(g_nsi, request->channel);
	result = generic_read_page_end_not_retry(request);
	if (result == -2) {
		nand_print_dbg(nftl_ro_lc_46);
		/* OEM clears the already-zero count here too. */
		retry_count = 0;
		while (hynix20nm_read_retry_cycle > retry_count) {
			result = 0;
			chip->retry++;
			if (chip->retry > hynix20nm_read_retry_cycle)
				chip->retry = 0;
			result = hynix20nm_set_readretry(
					(struct hynix20nm_retry_chip *)chip);
			if (result) {
				/* OEM tries the reset setter here and once again below,
				 * ignoring both reset results. */
				chip->retry = 0;
				hynix20nm_set_readretry(
					(struct hynix20nm_retry_chip *)chip);
				break;
			}
			result = generic_read_page_start(request);
			result |= generic_read_page_end_not_retry(request);
			if (is_nouse_page(request->data) == 1) {
				result = -2;
				nand_print_dbg(
				nftl_ro_lc_47,
					((struct samsung_read_controller *)chip->controller)->channel,
					((struct samsung_read_chip_id *)chip)->chip,
					request->block, request->page, chip->retry);
			} else if (result == 10 || result == 0) {
				result = 10;
				nand_print_dbg(nftl_ro_lc_48,
					       request->data[1], request->data[2],
					       request->data[3], request->data[4]);
				nand_print_dbg(
					nftl_ro_lc_49,
					((struct samsung_read_controller *)chip->controller)->channel,
					((struct samsung_read_chip_id *)chip)->chip,
					request->block, request->page, chip->retry);
				break;
			}
			retry_count++;
		}
		chip->retry = 0;
		hynix20nm_set_readretry((struct hynix20nm_retry_chip *)chip);
	}
	return result;
}

struct nftl_gc_info_geometry {
	unsigned char before_pages[20];
	unsigned short pages_per_block;
};
struct nftl_gc_info_node {
	unsigned short block;
	unsigned short unused;
	unsigned short invalid_pages;
	unsigned char before_next[10];
	struct nftl_gc_info_node *next;
};
struct nftl_gc_info_zone {
	unsigned char before_geometry[8];
	struct nftl_gc_info_geometry *geometry;
	unsigned char before_free[14];
	unsigned short free_blocks;
	unsigned char before_list[320];
	struct nftl_gc_info_node *list;
	unsigned char before_thresholds[320];
	unsigned short low_threshold;
	unsigned short high_threshold;
	unsigned char before_smart[3440 - 684];
	struct smart_print_payload *smart;
};
_Static_assert(__builtin_offsetof(struct nftl_gc_info_zone, list) == 352,
	       "GC info list offset");
_Static_assert(__builtin_offsetof(struct nftl_gc_info_zone, smart) == 3440,
	       "GC info SMART offset");
extern int nand_snprint(char *, unsigned int, const char *, ...);
struct nftl_gc_info_frame {
	unsigned long unused;
	struct nftl_gc_info_geometry *geometry;
	struct nftl_gc_info_node *node;
	unsigned int unused_after_node;
	unsigned int high_invalid;
	unsigned int high_count;
	unsigned int total_invalid;
	unsigned int total_count;
	int written;
};
int nftl_get_gc_info(struct nftl_gc_info_zone *zone, char *buffer,
		     unsigned int size)
{
	struct nftl_gc_info_frame frame;

	frame.written = 0;
	frame.geometry = zone->geometry;
	frame.total_invalid = frame.total_count = frame.high_count =
		frame.high_invalid = 0;
	for (frame.node = zone->list; frame.node; frame.node = frame.node->next) {
		if (frame.node->invalid_pages > frame.geometry->pages_per_block / 2) {
			frame.high_invalid += frame.node->invalid_pages;
			frame.high_count++;
		}
		frame.total_count++;
		frame.total_invalid += frame.node->invalid_pages;
	}
	frame.written = nand_snprint(buffer, size,
			       nftl_ro_lc_468,
			       zone->free_blocks, zone->low_threshold,
			       zone->high_threshold, zone->smart->gc_times,
			       zone->smart->gc_pages, frame.high_count, frame.total_count,
			       frame.high_invalid, frame.total_invalid);
	/* OEM reuses the total-block counter for its output line counter. */
	frame.total_count = 0;
	for (frame.node = zone->list; size - frame.written != 1;
	     frame.node = frame.node->next) {
		if (!frame.node || !frame.node->invalid_pages)
			break;
		frame.written += nand_snprint(buffer + (long)frame.written,
					size - frame.written, nftl_ro_lc_469,
					frame.node->block, frame.node->invalid_pages);
		frame.total_count++;
		if ((frame.total_count & 7) == 0)
			buffer[(long)frame.written - 1] = '\n';
	}
	/* OEM writes before the buffer when the first formatter returns zero
	 * and trusts its return even if it exceeds the remaining size. */
	buffer[(long)frame.written - 1] = '\n';
	return frame.written;
}

struct nand_op_geometry {
	unsigned char before_sectors[8];
	unsigned char sectors_per_page;
};
struct nand_op_zone {
	unsigned char before_geometry[8];
	struct nand_op_geometry *geometry;
	unsigned char before_capacity[16];
	unsigned int capacity;
};
struct nand_cache_write_zone;
extern int nand_cache_write(struct nand_cache_write_zone *, int,
			    unsigned short, void *);
int nand_op(unsigned char kind, struct nand_op_zone *zone,
	    unsigned int sector, unsigned int count, void *data)
{
	unsigned short offset;
	unsigned short take;
	unsigned short previous;
	unsigned short packed;
	unsigned char *cursor;
	unsigned int first_page;
	unsigned int page;
	int result;
	unsigned int (*operation)(void *, unsigned int, unsigned short,
				  unsigned char *);

	result = 0;
	if (!count)
		return 0;
	/* OEM's capacity addition wraps before comparison. */
	if (sector + count > zone->capacity) {
		nand_print(nftl_ro_lc_418);
		return 1;
	}
	/* The operation selector is an eight-bit OEM argument. */
	/* The recovered cache helpers use int parameters; their AArch64 ABI is
	 * the same as the unsigned callback type used by this stock caller. */
	if (kind == 0)
		operation = (typeof(operation))nand_cache_read;
	else
		operation = (typeof(operation))nand_cache_write;
	/* OEM reloads geometry for each quotient, remainder, and split. A
	 * concurrent change can make the chunks inconsistent; retain it. */
	/* OEM performs AArch64 unsigned division even for a zero geometry. */
	page = sector / zone->geometry->sectors_per_page;
	first_page = page;
	offset = sector % zone->geometry->sectors_per_page;
	take = zone->geometry->sectors_per_page - offset;
	if (count <= take) {
		take = count;
		count = 0;
	} else {
		count -= take;
	}
	cursor = data;
	previous = take;
	packed = (offset << 8) | take;
	result = operation(zone, first_page, packed, cursor);
	if (!count)
		return result;
	while (count > zone->geometry->sectors_per_page) {
		take = zone->geometry->sectors_per_page;
		first_page++;
		cursor += previous << 9;
		packed = take;
		count -= zone->geometry->sectors_per_page;
		previous = zone->geometry->sectors_per_page;
		result |= operation(zone, first_page, packed, cursor);
	}
	take = count;
	first_page++;
	cursor += previous << 9;
	packed = take;
	result |= operation(zone, first_page, packed, cursor);
	return result;
}

int hynix16nm_major_check_byte(unsigned char *value, unsigned int mode,
			       unsigned int level, const unsigned char *primary,
			       const unsigned char *secondary, unsigned int length)
{
	unsigned int valid = 0;
	unsigned int bit;
	unsigned char result = 0;

	if ((length >> 1) > level) {
		nand_print(nftl_ro_lc_113,
			   level, length);
		*value = 0xff;
		return -107;
	}
	for (bit = 0; bit < 8; bit++) {
		unsigned int ones = 0;
		unsigned int zeros = 0;
		unsigned int accepted = 0;
		unsigned int index;

		for (index = 0; index < length; index++) {
			if (((unsigned int)primary[index] >> bit) & 1)
				ones++;
			else
				zeros++;
		}
		if (ones > level) {
			result |= (unsigned char)(1U << bit);
			accepted = 1;
		}
		if (zeros > level)
			accepted = 1;
		if (!accepted && mode == 1) {
			/* The second copy is bitwise inverted by the OEM vote.
			 * Do not validate its length independently. */
			ones = 0;
			zeros = 0;
			for (index = 0; index < length; index++) {
				if (((unsigned int)secondary[index] >> bit) & 1)
					zeros++;
				else
					ones++;
			}
			if (zeros > level)
				accepted = 1;
			if (ones > level) {
				result |= (unsigned char)(1U << bit);
				accepted = 1;
			}
		}
		if (accepted)
			valid++;
		else {
			nand_print(nftl_ro_lc_114,
				   bit, ones, zeros);
			nand_print(nftl_ro_lc_115, bit);
		}
	}
	if (valid == 8) {
		*value = result;
		return 0;
	}
	*value = 0xff;
	return -80;
}

/* Aliases preserve the original table locations; their declared bounds are
 * intentionally not used to clamp the OEM retry index. */
extern unsigned char sandisk_19_low_recovered[];
extern unsigned char sandisk_19_high_recovered[];
extern unsigned char sandisk_24_recovered[];
extern unsigned char sandisk_1y_recovered[];
int sandisk_set_readretry(struct samsung_set_chip *chip, unsigned short page)
{
	/* OEM places count at FP+0x24, addresses at +0x28, values at
	 * +0x30 and the private result at +0x3c. */
	struct {
		unsigned int before_count;
		unsigned int count;
		unsigned char *addresses;
		unsigned char *values;
		unsigned int before_result;
		int result;
	} frame;

	frame.result = 0;

	sandisk_read_retry_clock_save((struct clock_chip *)chip);
	nand_enable_chip(chip);
	ndfc_disable_randomize(chip->controller);
	if (ndfc_is_toggle_interface(chip->controller)) {
		ndfc_set_legacy_interface(chip->controller);
		sandisk_toggle_mode_flag = 1;
	}
	if (sandisk_read_retry_mode == 48) {
		/* OEM treats page zero as low and page 255 as high; every
		 * other even nonzero page uses the high table. */
		if (page != 255 && (page == 0 || (page & 1))) {
			frame.values = sandisk_19_low_recovered +
				 (unsigned long)chip->retry * 3;
			sandisk_read_retry_cycle = 16;
		} else {
			frame.values = sandisk_19_high_recovered +
				 (unsigned long)chip->retry * 3;
			sandisk_read_retry_cycle = 20;
		}
		frame.addresses = sandisk_read_retry_reg_adr_19;
		frame.count = 3;
		frame.result = sandisk_vender_set_param(chip, frame.values,
						frame.addresses, frame.count);
		/* OEM ignores the final command result. */
		set_one_cmd((struct command_chip *)chip, -74, 0);
	} else if (sandisk_read_retry_mode == 49) {
		frame.values = sandisk_24_recovered + (unsigned long)chip->retry * 3;
		frame.addresses = sandisk_read_retry_reg_adr_24;
		frame.count = 3;
		frame.result = sandisk_vender_set_param(chip, frame.values,
						frame.addresses, frame.count);
		set_one_cmd((struct command_chip *)chip, -74, 0);
	} else if (sandisk_read_retry_mode == 50) {
		frame.values = sandisk_1y_recovered + (unsigned long)chip->retry * 4;
		frame.addresses = sandisk_read_retry_reg_adr_1y;
		frame.count = 1;
		frame.result = sandisk_vender_set_param(chip, frame.values,
						frame.addresses, frame.count);
		set_one_cmd((struct command_chip *)chip, 93, 0);
	}
	/* OEM's toggle flag is sticky across calls, even on another chip. */
	if (sandisk_toggle_mode_flag == 1)
		ndfc_set_toggle_interface(chip->controller);
	nand_disable_chip(chip);
	sandisk_read_retry_clock_recover((struct clock_chip *)chip);
	return frame.result;
}

/* These readonly aliases identify the OEM's two 7x4 signed delta tables. */
extern const short hynix26nm_para0_recovered[];
extern const short hynix26nm_para1_recovered[];
struct hynix26nm_init_chip {
	unsigned char before_saved[56];
	unsigned char saved[128];
	unsigned int retry;
	unsigned char before_params[280 - 188];
	struct hynix26nm_lsb_params *params;
};
int hynix26nm_readretry_init(struct hynix26nm_init_chip *chip)
{
	const short *table;
	unsigned int row, column;

	/* Stock reloads the nested specification for each field and trusts
	 * the pointer. The three writes precede validation of the mode. */
	hynix26nm_read_retry_mode = chip->params->retry_spec >> 16;
	hynix26nm_read_retry_cycle = chip->params->retry_spec >> 8;
	hynix26nm_read_retry_reg_num = chip->params->retry_spec;
	if (hynix26nm_read_retry_mode == 0) {
		hynix26nm_read_retry_reg_adr[0] = 0xac;
		hynix26nm_read_retry_reg_adr[1] = 0xad;
		hynix26nm_read_retry_reg_adr[2] = 0xae;
		hynix26nm_read_retry_reg_adr[3] = 0xaf;
		table = hynix26nm_para0_recovered;
	} else if (hynix26nm_read_retry_mode == 1) {
		hynix26nm_read_retry_reg_adr[0] = 0xa7;
		hynix26nm_read_retry_reg_adr[1] = 0xad;
		hynix26nm_read_retry_reg_adr[2] = 0xae;
		hynix26nm_read_retry_reg_adr[3] = 0xaf;
		table = hynix26nm_para1_recovered;
	} else {
		nand_print(nftl_ro_lc_64,
			   hynix26nm_read_retry_mode);
		return -54;
	}
	/* OEM loops through cycle+1 rows and register-count columns, but the
	 * source rows have only four entries and the destination has 28.
	 * Preserve its unchecked crossing of row and table boundaries. */
	for (row = 0; row < (unsigned int)hynix26nm_read_retry_cycle + 1;
	     row++)
		for (column = 0; column < hynix26nm_read_retry_reg_num;
		     column++)
			hynix26nm_read_retry_val[row * 4 + column] =
				table[row * 4 + column];
	chip->retry = 0;
	for (row = 0; row < 128; row++)
		chip->saved[row] = 0;
	/* OEM ignores the default-parameter result. */
	hynix26nm_get_default_param((struct hynix_default_chip *)chip);
	return 0;
}

struct nftl_clean_geometry {
	unsigned char before_sectors[8];
	unsigned char sectors_per_page;
};
struct nftl_clean_slot {
	unsigned short index;
	unsigned short state;
	unsigned int other;
	void *first;
	void *second;
	void *third;
};
struct nftl_clean_zone {
	unsigned char before_geometry[8];
	struct nftl_clean_geometry *geometry;
	unsigned char before_zone_number[8];
	unsigned char zone_number;
	unsigned char before_flags[3];
	unsigned short flag0, flag1;
	unsigned int capacity;
	unsigned char before_maps[36];
	void *maps[27];
	void *lists0[6];
	unsigned char before_lists1[16];
	void *lists1[6];
	unsigned char before_lists2[16];
	void *lists2[6];
	unsigned char before_current[64];
	void *current;
	unsigned char before_current_block[48];
	unsigned short current_block;
	unsigned char before_next[6];
	void *next;
	unsigned char before_next_block[48];
	unsigned short next_block;
	unsigned char before_list_count[46];
	unsigned short list_count;
	unsigned short list_state;
	unsigned char before_slots[4];
	struct nftl_clean_slot slots[11];
};
_Static_assert(__builtin_offsetof(struct nftl_clean_zone, maps) == 72,
	       "clean-zone map offset");
_Static_assert(__builtin_offsetof(struct nftl_clean_zone, lists0) == 288,
	       "clean-zone first list offset");
_Static_assert(__builtin_offsetof(struct nftl_clean_zone, current) == 528,
	       "clean-zone current pointer offset");
_Static_assert(__builtin_offsetof(struct nftl_clean_zone, slots) == 704,
	       "clean-zone slot offset");
_Static_assert(sizeof(struct nftl_clean_slot) == 32,
	       "clean-zone slot size");
struct nftl_clean_zone_frame {
	unsigned int tail;
	unsigned int full;
	unsigned int bytes;
	unsigned int pages;
	unsigned int index;
};
int nand_clean_zone_table2(struct nftl_clean_zone *zone)
{
	struct nftl_clean_zone_frame frame;

	/* OEM executes UDIV even for a zero divisor (AArch64 yields zero).
	 * C leaves this case undefined; non-OEM ports should guard it. */
	frame.pages = zone->capacity / zone->geometry->sectors_per_page;
	frame.bytes = frame.pages << 2;
	nand_print_dbg(nftl_ro_lc_386,
		       zone->zone_number, frame.pages);
	frame.full = frame.bytes >> 20;
	/* OEM trusts the map pointer array even if the computed byte count
	 * requires more than its 27 observed pointer slots. */
	for (frame.index = 0; frame.index < frame.full; frame.index++) {
		nand_memset(zone->maps[frame.index], -1, 1048576);
		nand_print_dbg(nftl_ro_lc_387, 1048576);
	}
	frame.tail = frame.bytes & 1048575;
	if (frame.tail) {
		nand_memset(zone->maps[frame.index], -1, frame.tail);
		nand_print_dbg(nftl_ro_lc_387, frame.tail);
	}
	zone->current = 0;
	zone->current_block = 0xffff;
	zone->next = 0;
	zone->next_block = 0xffff;
	for (frame.index = 0; frame.index < 6; frame.index++)
		zone->lists0[frame.index] = 0;
	for (frame.index = 0; frame.index < 6; frame.index++)
		zone->lists1[frame.index] = 0;
	for (frame.index = 0; frame.index < 6; frame.index++)
		zone->lists2[frame.index] = 0;
	zone->flag0 = 0;
	zone->flag1 = 0;
	zone->list_count = 0;
	for (frame.index = 0; frame.index < 10; frame.index++) {
		zone->slots[frame.index].index = frame.index;
		zone->slots[frame.index].state = 0;
		zone->slots[frame.index].first = 0;
		zone->slots[frame.index].second = 0;
		zone->slots[frame.index].third = 0;
	}
	zone->slots[10].index = 255;
	zone->list_state = 0;
	zone->slots[10].state = 0;
	zone->slots[10].first = 0;
	zone->slots[10].second = 0;
	zone->slots[10].third = 0;
	return 0;
}

struct ecc_rebuild_geometry {
	unsigned char before_pages[20];
	unsigned short pages;
	unsigned char before_fixed_page[6];
	unsigned short fixed_page;
};
struct ecc_rebuild_zone {
	unsigned char before_geometry[8];
	struct ecc_rebuild_geometry *geometry;
	unsigned char before_flag[9];
	unsigned char flag;
	unsigned char before_data[486];
	void *data;
	unsigned char before_read[2944];
	int (*read)(struct ecc_rebuild_zone *, struct physic_op_param *);
	int (*write)(struct ecc_rebuild_zone *, struct physic_op_param *);
};
_Static_assert(__builtin_offsetof(struct ecc_rebuild_zone, data) == 512,
	       "ECC rebuild data offset");
_Static_assert(__builtin_offsetof(struct ecc_rebuild_zone, read) == 3464,
	       "ECC rebuild read offset");
_Static_assert(__builtin_offsetof(struct ecc_rebuild_zone, write) == 3472,
	       "ECC rebuild write offset");
__attribute__((optimize("-fno-stack-protector")))
struct new_write_block *current_block_ecc_error(struct ecc_rebuild_zone *zone,
					struct new_write_block *old)
{
	struct new_write_block *replacement;
	struct physic_op_param read_op, write_op;
	/* Stock forwards this stack buffer through the partition to raw NAND.
	 * The raw read copies its geometry-derived spare length without checking
	 * this 16-byte bound; preserve that potential overflow until hardware
	 * geometry is known. The caller assumes no callback retains it. */
	unsigned char oob[16];
	unsigned int page, written;
	int status;

retry:
	replacement = (struct new_write_block *)
		out_phy_block_from_free_list((struct free_block_zone *)zone);
	if (!replacement) {
		nand_print(nftl_ro_lc_313);
		return 0;
	}
	replacement->tag = old->tag;
	if (new_block_init_for_write((struct new_write_zone *)zone,
				     replacement, replacement->tag)) {
		/* OEM leaks the failed replacement and retries forever. */
		nand_print(nftl_ro_lc_259);
		goto retry;
	}
	nand_print(nftl_ro_lc_314, old->number);
	written = 0;
	for (page = 0; page < zone->geometry->pages; page++) {
		set_physic_op_par(&read_op, old->number, page,
				  zone->geometry->fixed_page, zone->data, oob);
		status = zone->read(zone, &read_op);
		if (is_ftl_logic_page_data(oob) == 1) {
			if (status == 0 || status == 10) {
				if (nand_power_off_ecc_error(
						(struct poweroff_ecc_zone *)zone,
						zone->data, oob)) {
					nand_print(nftl_ro_lc_315);
					continue;
				}
				nand_print(nftl_ro_lc_316,
					   page, status);
				set_physic_op_par(&write_op, replacement->number,
						  written,
						  zone->geometry->fixed_page,
						  zone->data, oob);
				/* OEM discards the write result and advances regardless. */
				zone->write(zone, &write_op);
				written++;
			} else {
				nand_print(nftl_ro_lc_317,
					   page);
			}
		} else if (is_ftl_special_data(oob) == 1) {
			nand_print(nftl_ro_lc_318,
				   page);
		} else if (is_nouse_page(oob) != 1) {
			nand_print(nftl_ro_lc_319,
				   page);
		}
	}
	/* Both stock branches erase with the same arguments. The flag test
	 * currently has no effect; preserve it for behavioral provenance. */
	if (zone->flag & 1)
		erase_block((struct erase_block_zone *)zone,
			    (struct erase_block_info *)old,
			    (unsigned short)(old->erase_count + 1));
	else
		erase_block((struct erase_block_zone *)zone,
			    (struct erase_block_info *)old,
			    (unsigned short)(old->erase_count + 1));
	nand_print_dbg(nftl_ro_lc_320,
		       page, written);
	return replacement;
}

struct last_page_geometry {
	unsigned char before_pages[20];
	unsigned short pages;
	unsigned char before_bytes[4];
	unsigned short page_bytes;
	unsigned short fixed_page;
};
struct last_page_block {
	unsigned short number;
};
struct last_page_zone {
	unsigned char before_geometry[8];
	struct last_page_geometry *geometry;
	unsigned char before_data[496];
	void *data;
	unsigned char before_callbacks[2936];
	int (*write_last)(struct last_page_zone *, struct physic_op_param *);
	int (*read)(struct last_page_zone *, struct physic_op_param *);
	int (*write_data)(struct last_page_zone *, struct physic_op_param *);
	unsigned char before_erase[16];
	int (*erase)(struct last_page_zone *, struct physic_op_param *);
};
_Static_assert(__builtin_offsetof(struct last_page_zone, data) == 512,
	       "last-page data offset");
_Static_assert(__builtin_offsetof(struct last_page_zone, write_last) == 3456,
	       "last-page write callback offset");
_Static_assert(__builtin_offsetof(struct last_page_zone, erase) == 3496,
	       "last-page erase callback offset");
/* Stock does not check any allocation, even after logging a NULL result. */
__attribute__((optimize("-fno-stack-protector")))
void *block_last_page_ecc_error(struct last_page_zone *zone,
				struct last_page_block *block)
{
	/* Stock passes the 16-byte spare area at FP+0x48. Raw NAND's
	 * geometry-sized spare copy can overrun it; retain that OEM bound. */
	unsigned char oob[16] __attribute__((aligned(8)));
	struct physic_op_param op;
	void **saved;
	int page;
	int good;
	int status;

	nand_print(nftl_ro_lc_321);
	good = 0;
	saved = nftl_malloc(8192);
	if (!saved)
		nand_print(nftl_ro_lc_322, nftl_ro_func_7526);
	for (page = 0; page < zone->geometry->pages; page++) {
		/* Stock computes the slot before malloc, then reloads it for
		 * the unchecked failure path. Keep the slot across the call. */
		register void **slot = &saved[page];
		*slot = nftl_malloc(zone->geometry->page_bytes);
		if (!saved[page])
			nand_print(nftl_ro_lc_323, nftl_ro_func_7526);
	}
	for (page = 0; page < zone->geometry->pages; page++) {
		set_physic_op_par(&op, block->number, page,
				  zone->geometry->fixed_page, zone->data, oob);
		status = zone->read(zone, &op);
		if (is_nouse_page(oob) == 1)
			break;
		if (is_ftl_logic_page_data(oob) == 1) {
			if (status == 0 || status == 10) {
				nand_memcpy(saved[page], zone->data,
					    zone->geometry->page_bytes);
				good++;
			}
		} else if (is_ftl_special_data(oob) == 1) {
			if (status == 0 || status == 10) {
				nand_memcpy(saved[page], zone->data,
					    zone->geometry->page_bytes);
				good++;
			}
			nand_print(nftl_ro_lc_324);
		} else {
			nand_print(nftl_ro_lc_325);
		}
	}
	set_physic_op_par(&op, block->number, page,
				  zone->geometry->fixed_page, zone->data, oob);
	status = zone->write_last(zone, &op);
	if (status) {
		zone->erase(zone, &op);
		block = (struct last_page_block *)out_phy_block_from_free_list(
			(struct free_block_zone *)zone);
		if (!block) {
			nand_print(nftl_ro_lc_313);
			goto cleanup;
		}
		set_physic_op_par(&op, block->number, page,
					  zone->geometry->fixed_page, zone->data, oob);
		zone->write_last(zone, &op);
	}
	for (page = 0; page < good; page++) {
		/* OEM stores buffers at physical-page indexes, but replays the
		 * first good-count slots. A skipped bad read can replay stale data. */
		set_physic_op_par(&op, block->number, page,
					  zone->geometry->fixed_page, saved[page], oob);
		zone->write_data(zone, &op);
	}
cleanup:
	for (page = 0; page < zone->geometry->pages; page++)
		nftl_free(saved[page]);
	nftl_free(saved);
	nand_print(nftl_ro_lc_326);
	return block;
}

struct page_map_recovery_mode {
	unsigned char before_enabled[28];
	unsigned short enabled;
};
struct page_map_recovery_zone {
	unsigned char before_geometry[8];
	struct ecc_rebuild_geometry *geometry;
	unsigned char before_capacity[20];
	unsigned int capacity;
	unsigned char before_data[472];
	void *data;
	unsigned char before_map_bytes[40];
	unsigned int map_bytes;
	unsigned char before_mode[2884];
	struct page_map_recovery_mode *mode;
	unsigned char before_read[8];
	int (*read)(struct page_map_recovery_zone *, struct physic_op_param *);
};
_Static_assert(__builtin_offsetof(struct page_map_recovery_zone, data) == 512,
	       "page-map data offset");
_Static_assert(__builtin_offsetof(struct page_map_recovery_zone, map_bytes) == 560,
	       "page-map byte count offset");
_Static_assert(__builtin_offsetof(struct page_map_recovery_zone, mode) == 3448,
	       "page-map mode offset");
_Static_assert(__builtin_offsetof(struct page_map_recovery_zone, read) == 3464,
	       "page-map read offset");
__attribute__((optimize("-fno-stack-protector")))
int recover_phy_page_mapping(struct page_map_recovery_zone *zone,
			     struct new_write_block *block,
			     unsigned int *map)
{
	/* Keep OEM's op/OOB frame order: op starts at FP+0x40 and the 16-byte
	 * OOB buffer at FP+0x58. A too-large spare copy can corrupt nearby
	 * locals; stock does not check the callback's write length. */
	unsigned char oob[16];
	struct physic_op_param op;
	int page;
	int status;

	/* OEM trusts map_bytes even if it exceeds the caller's map buffer. */
	nand_memset(map, -1, zone->map_bytes);
	for (page = 0; page < zone->geometry->pages; page++) {
		set_physic_op_par(&op, block->number, page,
				  zone->geometry->fixed_page, zone->data, oob);
		status = zone->read(zone, &op);
		if (is_nouse_page(oob) == 1 &&
		    (status == 0 || status == 10)) {
			nand_print(nftl_ro_lc_262,
				   block->number, page);
			break;
		}
		if (is_ftl_logic_page_data(oob) == 1) {
			if (status == 0 || status == 10) {
				if (nand_power_off_ecc_error(
						(struct poweroff_ecc_zone *)zone,
						zone->data, oob)) {
					nand_print(nftl_ro_lc_263,
						   block->number, page);
					status = -2;
				} else {
					map[page] = get_logic_page_from_oob(oob);
					if (map[page] >= zone->capacity) {
						nand_print(nftl_ro_lc_264,
							   block->number, page, map[page]);
						map[page] = -1;
					}
				}
			}
		} else if (is_ftl_special_data(oob) == 1) {
			nand_print(nftl_ro_lc_265, page);
		} else if (is_power_down_info_page(oob) == 1) {
			if (status == 0 || status == 10) {
				map[page] = get_special_data_from_oob(oob);
				nand_print(nftl_ro_lc_266, page);
			}
		} else {
			/* Stock diagnostic contains the original typo. */
			nand_print(nftl_ro_lc_267,
				   page);
		}
		if (status != 0 && status != 10) {
			nand_print(nftl_ro_lc_268,
				   block->number, page);
			return 65535;
		}
	}
	/* OEM performs one last geometry read and pages-minus-two compare even
	 * though both outcomes return the same page. Keep the redundant branch:
	 * a callback that changed the geometry pointer can make it fault. */
	if ((int)zone->geometry->pages - 2 > page)
		page = page;
	/* Return the first unused page or the geometry page count, instead of
	 * the status of the final read. */
	return page;
}

struct current_used_zone {
	unsigned char before_flag[25];
	unsigned char flags;
	unsigned char before_free_head[304 - 26];
	void *free_head;
	unsigned char before_data[512 - 312];
	void *data;
	unsigned char before_current[528 - 520];
	struct new_write_block *current;
	void *current_map_buffer;
	unsigned int map_copy_bytes;
	unsigned char before_current_map[552 - 548];
	unsigned int *current_map;
	unsigned char before_current_used[584 - 560];
	unsigned short current_used;
	unsigned char before_assist[592 - 586];
	struct new_write_block *assist;
	void *assist_map_buffer;
	unsigned char before_assist_map[616 - 608];
	unsigned int *assist_map;
	unsigned char before_assist_used[648 - 624];
	unsigned short assist_used;
	unsigned char before_copy[3480 - 650];
	int (*copy_page)(struct current_used_zone *, struct new_write_block *,
			 struct new_write_block *, void *, unsigned short);
};
_Static_assert(__builtin_offsetof(struct current_used_zone, current) == 528,
	       "current block offset");
_Static_assert(__builtin_offsetof(struct current_used_zone, assist) == 592,
	       "assist block offset");
_Static_assert(__builtin_offsetof(struct current_used_zone, copy_page) == 3480,
	       "current block copy callback offset");

int get_current_used_block(struct current_used_zone *zone,
			   struct new_write_block *candidate,
			   unsigned int previous_tag)
{
	struct new_write_block *assist;
	unsigned int page, tag;
	unsigned short copy_limit;
	int had_primary_ecc = 0;

	if (zone->current)
		return 0;
	/* Stock emits the existing-block recovery before the no-candidate path. */
	if (!candidate)
		goto allocate_new;
	for (;;) {
		zone->current_used = (unsigned short)
			recover_phy_page_mapping(
				(struct page_map_recovery_zone *)zone,
				candidate, zone->current_map);
		if (zone->current_used != 0xffff)
			break;
		had_primary_ecc = 1;
		nand_print(nftl_ro_lc_250,
			   candidate->number);
		candidate = current_block_ecc_error(
			(struct ecc_rebuild_zone *)zone, candidate);
		if (candidate)
			continue;
		nand_print(nftl_ro_lc_251);
		return 1;
	}
	zone->current = candidate;
	/* OEM reloads the shared current pointer after publishing it. A
	 * concurrent replacement can change the following reads and callback. */
	nand_print_dbg(nftl_ro_lc_252, zone->current->number,
		       zone->current_used);
	recover_logic_page_mapping((struct logic_page_map_zone *)zone,
			   (struct invalid_page_node *)zone->current,
			   zone->current_map, zone->current_used);
	if (zone->current_used < zone->current->page)
		nand_print(nftl_ro_lc_253,
			   zone->current_used, zone->current->page);
	if (!(zone->flags & 1))
		return 0;
	for (;;) {
		if (!zone->assist) {
			zone->assist = (struct new_write_block *)
				out_phy_block_from_free_list(
					(struct free_block_zone *)zone);
			if (!zone->assist)
				return 1;
			zone->assist_used = 0;
			/* OEM reads the shared assist pointer before current->tag. */
			assist = zone->assist;
			tag = zone->current->tag + 1;
			assist->tag = tag;
			/* OEM reloads the published tag for initialization. */
			if (new_block_init_for_write(
				    (struct new_write_zone *)zone,
				    zone->assist, zone->assist->tag)) {
				nand_print(nftl_ro_lc_254);
				zone->assist = 0;
				continue;
			}
			/* OEM snapshots the copy bound before invoking copy_page. */
			copy_limit = zone->current_used;
			for (page = 0; page < copy_limit; page++) {
				/* OEM logs a copy failure but still advances to the next page. */
				if (zone->copy_page(zone, zone->current,
						    zone->assist, zone->data,
						    page))
					nand_print(nftl_ro_lc_255);
			}
		}
		zone->assist_used = (unsigned short)
			recover_phy_page_mapping(
				(struct page_map_recovery_zone *)zone,
				zone->assist, zone->assist_map);
		if (zone->assist_used == 0xffff) {
			nand_print(nftl_ro_lc_256);
			erase_block((struct erase_block_zone *)zone,
				    (struct erase_block_info *)zone->assist,
				    (unsigned short)(zone->assist->erase_count + 1));
			zone->assist = 0;
			continue;
		}
		/* Keep the OEM destination read ahead of the source-tag read. */
		assist = zone->assist;
		assist->tag = zone->current->tag + 1;
		recover_logic_page_mapping((struct logic_page_map_zone *)zone,
				   (struct invalid_page_node *)zone->assist,
				   zone->assist_map, zone->assist_used);
		if (zone->assist_used > zone->current_used) {
			nand_print(nftl_ro_lc_257,
				   zone->assist_used, zone->current_used);
			if (!had_primary_ecc) {
				nand_print(nftl_ro_lc_258,
					   zone->assist_used, zone->current_used);
				return 0;
			}
			erase_block((struct erase_block_zone *)zone,
				    (struct erase_block_info *)zone->current,
				    (unsigned short)(zone->current->erase_count + 1));
			zone->current = zone->assist;
			zone->current_used = zone->assist_used;
			nand_memcpy(zone->current_map_buffer,
				    zone->assist_map_buffer,
				    zone->map_copy_bytes);
			zone->assist = 0;
			continue;
		}
		/* OEM makes a second comparison using >=. If current_used falls
		 * between the two reads, it returns without erasing assist. */
		if (zone->assist_used >= zone->current_used)
			return 0;
		nand_print(nftl_ro_lc_259);
		erase_block((struct erase_block_zone *)zone,
			    (struct erase_block_info *)zone->assist,
			    (unsigned short)(zone->assist->erase_count + 1));
		zone->assist = 0;
	}
allocate_new:
	nand_print_dbg(nftl_ro_lc_260);
	if (!zone->free_head) {
		nand_print(nftl_ro_lc_261);
		print_free_list((struct free_print_zone *)zone);
		print_block_invalid_list((struct invalid_print_zone *)zone);
		return 1;
	}
	zone->current = (struct new_write_block *)
		out_phy_block_from_free_list((struct free_block_zone *)zone);
	if (!zone->current)
		return 1;
	zone->current_used = 0;
	zone->current->tag = previous_tag + 1;
	if (zone->flags & 1) {
		zone->assist = (struct new_write_block *)
			out_phy_block_from_free_list((struct free_block_zone *)zone);
		if (!zone->assist)
			return 1;
		zone->assist_used = 0;
		/* OEM reads assist before the current block's tag. */
		assist = zone->assist;
		assist->tag = zone->current->tag + 1;
	} else {
		zone->assist = 0;
	}
	return 0;
}

__attribute__((optimize("-fno-stack-protector")))
int recover_block_phy_page_mapping(struct page_map_recovery_zone *zone,
				   struct new_write_block *block,
				   unsigned int *map)
{
	/* OEM places op at FP+0x48 and the 20-byte OOB buffer at FP+0x60.
	 * The read callback's spare write is unchecked and can hit later locals. */
	unsigned char oob[20];
	struct physic_op_param op;
	int page;
	int status;
	/* This initial store is dead: the retry loop sets attempt to zero
	 * again. Retain it to match OEM stack writes and fault timing. */
	int attempt = 0;

	/* OEM trusts the byte count independently of page count and map size. */
	nand_memset(map, -1, zone->map_bytes);
	for (page = 0; page < (int)zone->geometry->pages - 1; page++) {
		if (zone->mode->enabled)
			set_physic_op_par(&op, block->number, page,
					  zone->geometry->fixed_page,
					  zone->data, oob);
		else
			/* OEM passes NULL data here, then still gives the ordinary
			 * data buffer to its later ECC check. */
			set_physic_op_par(&op, block->number, page, 0, 0, oob);
		for (attempt = 0; attempt <= 1; attempt++) {
			status = zone->read(zone, &op);
			if (status >= 0)
				break;
			nand_print(nftl_ro_lc_269,
				   status, block->number, page);
			if (attempt == 1)
				nand_print(nftl_ro_lc_270,
					   status, block->number, page, attempt);
		}
		if (is_ftl_logic_page_data(oob) == 1) {
			if (status != 0 && status != 10)
				continue;
			if (nand_power_off_ecc_error(
						(struct poweroff_ecc_zone *)zone,
						zone->data, oob)) {
				nand_print(nftl_ro_lc_271);
				continue;
			}
			map[page] = get_logic_page_from_oob(oob);
			if (map[page] >= zone->capacity) {
				nand_print(nftl_ro_lc_264,
					   block->number, page, map[page]);
				map[page] = -1;
			}
		} else if (is_nouse_page(oob) != 1) {
			nand_print(nftl_ro_lc_272,
			   block->number, page);
		}
	}
	return 0;
}

struct hynix16nm_otp_info_controller {
	unsigned char before_ce[1264];
	unsigned int ce[8];
	unsigned int rb[8];
};
struct hynix16nm_otp_info_chip {
	unsigned char before_index[28];
	unsigned int index;
	unsigned char before_controller[240];
	struct hynix16nm_otp_info_controller *controller;
};
_Static_assert(__builtin_offsetof(struct hynix16nm_otp_info_chip,
				controller) == 272, "Hynix OTP controller offset");
__attribute__((optimize("-fno-stack-protector")))
int hynix16nm_read_otp_info_hynix(struct hynix16nm_otp_info_chip *chip,
				   unsigned char *buffer)
{
	/* OEM reserves separate eight-byte slots for each one-byte bus field. */
	unsigned char address[8] __attribute__((aligned(8)));
	unsigned char command[8] __attribute__((aligned(8)));
	unsigned char data[8] __attribute__((aligned(8)));
	int result;
	unsigned char *destination;

	nand_enable_chip((struct samsung_set_chip *)chip);
	ndfc_disable_randomize(chip->controller);
	/* OEM indexes controller CE/RB arrays by an unchecked chip index and
	 * performs this log before validating the destination buffer. */
	nand_print_dbg(nftl_ro_lc_123,
		       chip->controller->ce[chip->index],
		       chip->controller->rb[chip->index]);
	destination = buffer;
	if (!destination) {
		nand_disable_chip((struct samsung_set_chip *)chip);
		nand_print(nftl_ro_lc_124);
		return -79;
	}
	/* OEM ignores every command and data-transfer status. */
	set_one_cmd((struct command_chip *)chip, -1, 1);
	command[0] = 0x36;
	address[0] = 0x38;
	data[0] = 0x52;
	set_cmd_with_nand_bus(chip, command, 0, address, data, 1, 1);
	set_one_cmd((struct command_chip *)chip, 0x16, 0);
	set_one_cmd((struct command_chip *)chip, 0x17, 0);
	set_one_cmd((struct command_chip *)chip, 0x04, 0);
	set_one_cmd((struct command_chip *)chip, 0x19, 0);
	set_one_cmd((struct command_chip *)chip, 0x00, 0);
	set_one_addr((struct command_chip *)chip, 0);
	set_one_addr((struct command_chip *)chip, 0);
	set_one_addr((struct command_chip *)chip, 0);
	set_one_addr((struct command_chip *)chip, 2);
	set_one_addr((struct command_chip *)chip, 0);
	command[0] = 0x30;
	get_data_with_nand_bus_one_cmd((struct samsung_set_chip *)chip,
					   command, 0, destination, 528);
	set_one_cmd((struct command_chip *)chip, -1, 1);
	command[0] = 0x36;
	address[0] = 0x38;
	data[0] = 0;
	set_cmd_with_nand_bus(chip, command, 0, address, data, 1, 1);
	set_one_cmd((struct command_chip *)chip, 0x16, 0);
	set_one_cmd((struct command_chip *)chip, 0x00, 0);
	set_one_addr((struct command_chip *)chip, 0);
	set_one_addr((struct command_chip *)chip, 0);
	set_one_addr((struct command_chip *)chip, 0);
	set_one_addr((struct command_chip *)chip, 0);
	set_one_addr((struct command_chip *)chip, 0);
	set_one_cmd((struct command_chip *)chip, 0x30, 1);
	if (destination[0] != 8 || destination[1] != 8) {
		nand_print(nftl_ro_lc_51,
			   destination[0], destination[1]);
		nand_disable_chip((struct samsung_set_chip *)chip);
		result = -96;
		return result;
	}
	nand_disable_chip((struct samsung_set_chip *)chip);
	return 0;
}

struct phy_write_block {
	unsigned short number;
	unsigned short active;
	unsigned short used_pages;
	unsigned short erase_count;
	unsigned int tag;
};
struct phy_write_state {
	struct phy_write_block *block;
	unsigned char before_page[48];
	unsigned short page;
};
_Static_assert(__builtin_offsetof(struct phy_write_block, active) == 2,
	       "physical write active offset");
_Static_assert(__builtin_offsetof(struct phy_write_block, tag) == 8,
	       "physical write tag offset");
struct phy_write_geometry {
	unsigned char before_sectors[8];
	unsigned char sectors_per_page;
	unsigned char before_pages[11];
	unsigned short pages;
	unsigned char before_fixed_page[6];
	unsigned short fixed_page;
};
struct phy_write_zone {
	unsigned char before_geometry[8];
	struct phy_write_geometry *geometry;
	unsigned char before_map_bytes[544];
	unsigned int map_bytes;
	unsigned char before_write[2908];
	int (*write)(struct phy_write_zone *, struct physic_op_param *);
};
_Static_assert(__builtin_offsetof(struct phy_write_state, page) == 56,
	       "physical write page offset");
_Static_assert(__builtin_offsetof(struct phy_write_zone, map_bytes) == 560,
	       "physical write map count offset");
_Static_assert(__builtin_offsetof(struct phy_write_zone, write) == 3472,
	       "physical write callback offset");
extern int do_write_error(struct phy_write_zone *, struct phy_write_state *);
__attribute__((optimize("-fno-stack-protector")))
int write_phy_data_to_current_block(struct phy_write_zone *zone,
				    struct phy_write_state *state,
				    const unsigned char *data,
				    unsigned int logic_page,
				    unsigned int kind)
{
	unsigned char oob[16];
	struct physic_op_param op;
	unsigned short crc = 0;
	unsigned short length = 0;
	unsigned int retry;
	int status;

	nand_memset(oob, -91, 16);
	/* Stock keeps only the low seven bits of sectors/page here. */
	length = (zone->geometry->sectors_per_page & 127) << 9;
	if (kind == 0xaaaaffff)
		length = zone->map_bytes;
	crc = nand_crc16(-1, data, length);
	oob[12] = crc >> 8;
	oob[13] = crc;
	retry = 0;
	for (;;) {
		set_physic_op_par(&op, state->block->number, state->page,
				  zone->geometry->fixed_page, (void *)data, oob);
		if (kind == 0xaaaaffff) {
			set_oob_special_page(zone, oob, kind,
					     state->block->tag,
					     state->block->erase_count);
		} else if (kind == 192) {
			if (state->page == (unsigned int)zone->geometry->pages - 1)
				nand_print_dbg(nftl_ro_lc_162,
					       state->block->number);
			set_oob_logic_page(zone, oob, logic_page,
					   state->block->tag,
					   state->block->erase_count);
		} else if (kind == 0xaa113333) {
			if (state->page == (unsigned int)zone->geometry->pages - 1)
				nand_print_dbg(nftl_ro_lc_163,
					       state->block->number);
			set_oob_special_page(zone, oob, kind,
					     state->block->tag,
					     state->block->erase_count);
		}
		state->block->active = 1;
		status = zone->write(zone, &op);
		if (!status || retry > 1)
			return 0;
		nand_print_dbg(nftl_ro_lc_164,
			       op.block);
		if (do_write_error(zone, state)) {
			nand_print(nftl_ro_lc_165);
			return 1;
		}
		/* OEM retries twice, then reports success even if the third
		 * write failed; preserve the misleading return. */
		retry++;
	}
}

struct do_write_geometry {
	unsigned char before_fixed_page[28];
	unsigned short fixed_page;
};
struct do_write_block {
	unsigned short number;
	unsigned short active;
	unsigned short page;
	unsigned short erase_count;
	unsigned int tag;
};
struct do_write_state {
	struct do_write_block *block;
	unsigned char before_map[16];
	unsigned int *map;
	unsigned char before_page[24];
	unsigned short page;
};
struct write_logic_zone {
	unsigned char before_geometry[8];
	struct page_geometry *geometry;
	unsigned char before_mode[648];
	unsigned int mode;
};
_Static_assert(__builtin_offsetof(struct do_write_state, map) == 24,
	       "logic-write map pointer offset");
_Static_assert(__builtin_offsetof(struct do_write_state, page) == 56,
	       "logic-write page offset");
_Static_assert(__builtin_offsetof(struct write_logic_zone, mode) == 664,
	       "logic-write mode offset");
int write_logic_data_to_current_block(void *raw_zone, void *raw_current,
				      unsigned int marker, unsigned int logic_page,
				      void *data)
{
	struct write_logic_zone *zone = raw_zone;
	struct do_write_state *current = raw_current;
	struct invalid_page_location *location;
	struct phy_write_block *old_block = 0;
	int moved = 0;
	int result;

	if (marker == 192) {
		location = (struct invalid_page_location *)
			get_logic_page_map((struct logic_page_map_zone *)zone,
					   logic_page);
		if (!location) {
			nand_print_dbg(nftl_ro_lc_167);
			return 1;
		}
		if (location->block != 0xffff) {
			old_block = (struct phy_write_block *)
				get_phy_block_addr((struct nftl_phy_outer *)zone,
						   location->block);
			if (current->block == (struct do_write_block *)old_block) {
				old_block->used_pages++;
				if (old_block->used_pages >
				    zone->geometry->pages_per_block)
					nand_print_dbg(nftl_ro_lc_168,
						       current->page);
			} else {
				moved = 1;
			}
		}
		result = write_phy_data_to_current_block(
			(struct phy_write_zone *)zone,
			(struct phy_write_state *)current,
			data, logic_page, 192);
		if (result) {
			nand_print(nftl_ro_lc_169);
			return result;
		}
		location->block = current->block->number;
		location->page = current->page & 0xfff;
		location->flags = (location->flags & ~1U) |
			(zone->mode == 0);
		current->map[location->page] = logic_page;
		current->page++;
	} else if (marker == 0xaa111111) {
		location = (struct invalid_page_location *)
			get_logic_page_map((struct logic_page_map_zone *)zone,
					   logic_page);
		if (!location) {
			nand_print_dbg(nftl_ro_lc_167);
			return 1;
		}
		if (location->block != 0xffff) {
			old_block = (struct phy_write_block *)
				get_phy_block_addr((struct nftl_phy_outer *)zone,
						   location->block);
			if (current->block == (struct do_write_block *)old_block) {
				current->map[location->page] = ~0U;
				old_block->used_pages++;
				if (old_block->used_pages >
				    zone->geometry->pages_per_block)
					nand_print(nftl_ro_lc_170,
						   current->page);
			} else {
				moved = 1;
			}
		}
		location->block = 0xffff;
		location->page = 0xfff;
		location->flags |= 1;
	} else if (marker == 0xaa113333) {
		current->block->page++;
		result = write_phy_data_to_current_block(
			(struct phy_write_zone *)zone,
			(struct phy_write_state *)current,
			data, logic_page, marker);
		if (result) {
			nand_print(nftl_ro_lc_171);
			return result;
		}
		current->page++;
	}
	if (moved)
		phy_block_from_invalid_page_incr(
			(struct invalid_page_zone *)zone,
			(struct invalid_page_node *)old_block);
	/* OEM does not undo the metadata mutations on write failure and ignores
	 * both list-helper results. Keep this ordering for stock compatibility. */
	adjust_invaild_list((struct move_zone *)zone);
	return 0;
}
struct do_write_zone {
	unsigned char before_geometry[8];
	struct do_write_geometry *geometry;
	unsigned char before_free_count[14];
	unsigned short free_count;
	unsigned char before_data[480];
	void *data;
	unsigned char before_retry_word[160];
	unsigned short retry_word;
	unsigned char before_erase[2774];
	int (*erase)(struct do_write_zone *, struct physic_op_param *);
	unsigned char before_copy[16];
	int (*copy)(struct do_write_zone *, struct do_write_block *,
		    struct do_write_block *, void *, unsigned short);
	unsigned char before_mark_bad[8];
	int (*mark_bad)(struct do_write_zone *, struct physic_op_param *);
};
_Static_assert(sizeof(struct do_write_block) == 12,
	       "write error block size");
_Static_assert(__builtin_offsetof(struct do_write_state, map) == 24,
	       "write error map offset");
_Static_assert(__builtin_offsetof(struct do_write_state, page) == 56,
	       "write error page offset");
_Static_assert(__builtin_offsetof(struct do_write_zone, data) == 512,
	       "write error data offset");
_Static_assert(__builtin_offsetof(struct do_write_zone, retry_word) == 680,
	       "write error retry limit offset");
_Static_assert(__builtin_offsetof(struct do_write_zone, erase) == 3456,
	       "write error erase callback offset");
_Static_assert(__builtin_offsetof(struct do_write_zone, copy) == 3480,
	       "write error copy callback offset");
_Static_assert(__builtin_offsetof(struct do_write_zone, mark_bad) == 3496,
	       "write error mark-bad callback offset");

__attribute__((optimize("-fno-stack-protector")))
int do_write_error(struct phy_write_zone *owner,
		   struct phy_write_state *current)
{
	struct do_write_zone *zone = (struct do_write_zone *)owner;
	struct do_write_state *state = (struct do_write_state *)current;
	struct do_write_block *replacement;
	struct do_write_block *old;
	struct physic_op_param op;
	unsigned int *entry;
	unsigned int page;
	unsigned int logic_page;
	int status;

retry:
	replacement = (struct do_write_block *)out_phy_block_from_free_list(
		(struct free_block_zone *)zone);
	if (!replacement) {
		nand_print(nftl_ro_lc_149);
		return 1;
	}
	set_physic_op_par(&op, replacement->number, 0,
			  zone->geometry->fixed_page, 0, 0);
	status = zone->erase(zone, &op);
	/* OEM updates the block metadata even when erase fails. */
	replacement->page = 0;
	replacement->erase_count++;
	if (status) {
		zone->mark_bad(zone, &op);
		nand_print(nftl_ro_lc_150,
			   replacement->number);
		if (zone->free_count > (unsigned short)(zone->retry_word >> 2))
			goto retry;
		nand_print(nftl_ro_lc_149);
		return 1;
	}
	old = state->block;
	replacement->tag = old->tag + 1;
	/* The current page itself is excluded from both walks. */
	for (page = 0; page < state->page; page++) {
		logic_page = state->map[page];
		/* OEM stops copying at the first absent map entry. */
		if (logic_page == ~0U)
			break;
		status = zone->copy(zone, old, replacement, zone->data,
				    (unsigned short)page);
		if (status) {
			nand_print(nftl_ro_lc_151,
				   replacement->number);
			set_physic_op_par(&op, replacement->number, 0,
					  zone->geometry->fixed_page, 0, 0);
			zone->mark_bad(zone, &op);
			goto retry;
		}
	}
	replacement->page = old->page;
	/* Stock initializes this unused physical op before erasing the old
	 * block; retain the call and the ignored erase result. */
	set_physic_op_par(&op, old->number, 0,
			  zone->geometry->fixed_page, 0, 0);
	erase_block((struct erase_block_zone *)zone,
		    (struct erase_block_info *)old,
		    (unsigned short)(old->erase_count + 1));
	replacement->active = old->active;
	state->block = replacement;
	for (page = 0; page < state->page; page++) {
		logic_page = state->map[page];
		if (logic_page == ~0U)
			continue;
		entry = get_logic_page_map((struct logic_page_map_zone *)zone,
					   logic_page);
		if (!entry)
			continue;
		((unsigned short *)entry)[1] = state->block->number;
		((unsigned short *)entry)[0] =
			(((unsigned short *)entry)[0] & 0xf000) | (page & 0x0fff);
	}
	return 0;
}

/* Cross-talk recovery uses the same physical block prefix as the block
 * initialization and erase helpers. Private tail fields remain provisional. */
struct cross_rebuild_geometry {
	unsigned char before_pages[20];
	unsigned short pages;
	unsigned char before_bytes[4];
	unsigned short bytes;
	unsigned short fixed_page;
};
struct cross_rebuild_zone {
	unsigned char before_geometry[8];
	struct cross_rebuild_geometry *geometry;
	unsigned char before_write[3440];
	int (*erase)(struct cross_rebuild_zone *, struct physic_op_param *);
	void *read_callback;
	int (*write)(struct cross_rebuild_zone *, struct physic_op_param *);
	unsigned char before_mark_bad[16];
	int (*mark_bad)(struct cross_rebuild_zone *, struct physic_op_param *);
};
struct cross_rebuild_block {
	unsigned short number;
	unsigned short state;
	unsigned short page;
	unsigned short erase_count;
	unsigned int tag;
};
struct cross_rebuild_block *cross_talk_rebuild_current_block(
	struct cross_rebuild_zone *zone, struct cross_rebuild_block *primary,
	struct cross_rebuild_block *assist)
{
	struct nrc_buffers *nrc = 0;
	struct cross_rebuild_block *source, *replacement;
	struct physic_op_param op;
	unsigned int primary_used, assist_used = 0;
	unsigned int primary_flag = 0, assist_flag = 0;
	unsigned int first_erase = 0, second_erase = 0;
	unsigned int page, compact, special;
	unsigned char *data, *oob;

	if (init_nrc((struct nrc_zone *)zone, &nrc)) {
		free_nrc(nrc);
		nand_print(nftl_ro_lc_284);
		return 0;
	}
	primary_used = get_used_page_num((struct used_page_zone *)zone,
		(struct physical_block *)primary, &primary_flag,
		(struct used_page_cache *)nrc);
	if (!assist) {
		nand_print(nftl_ro_lc_285,
			   primary->number, primary_used, primary->tag);
		source = primary;
		first_erase = primary->erase_count;
		if (!*(unsigned int *)nrc) {
			free_nrc(nrc);
			nand_print(nftl_ro_lc_286);
			erase_block((struct erase_block_zone *)zone,
				    (struct erase_block_info *)primary,
				    primary->erase_count + 1);
			return 0;
		}
		if (primary_flag)
			first_erase += 20;
	} else {
		assist_used = get_used_page_num((struct used_page_zone *)zone,
			(struct physical_block *)assist, &assist_flag,
			(struct used_page_cache *)nrc);
		nand_print(nftl_ro_lc_287,
			   primary->number, primary_used, primary->tag,
			   assist->number, assist_used, assist->tag);
		if (!*(unsigned int *)nrc) {
			free_nrc(nrc);
			nand_print(nftl_ro_lc_288);
			erase_block((struct erase_block_zone *)zone,
				    (struct erase_block_info *)primary,
				    primary->erase_count + 1);
			erase_block((struct erase_block_zone *)zone,
				    (struct erase_block_info *)assist,
				    assist->erase_count + 1);
			return 0;
		}
		if (assist_used >= primary_used) {
			first_erase = primary->erase_count;
			second_erase = assist->erase_count;
		} else {
			first_erase = assist->erase_count;
			second_erase = primary->erase_count;
		}
		if (primary_flag || assist_flag) {
			first_erase += 20;
			second_erase += 20;
		}
		if (assist_used > primary_used) {
			nand_print(nftl_ro_lc_289,
				   primary_used, assist_used, *(unsigned int *)nrc);
			erase_block((struct erase_block_zone *)zone,
				    (struct erase_block_info *)primary,
				    (unsigned short)first_erase + 1);
			source = assist;
		} else {
			nand_print(nftl_ro_lc_290,
				   primary_used, assist_used, *(unsigned int *)nrc);
			/* On equal counts OEM takes the primary erase count but
			 * erases the assist block. Preserve this mismatch. */
			erase_block((struct erase_block_zone *)zone,
				    (struct erase_block_info *)assist,
				    (unsigned short)first_erase + 1);
			source = primary;
		}
	}
retry:
	replacement = (struct cross_rebuild_block *)
		out_phy_block_from_free_list((struct free_block_zone *)zone);
	if (!replacement) {
		nand_print(nftl_ro_lc_291);
		/* OEM leaks nrc on this exit. */
		return 0;
	}
	replacement->tag = source->tag + 1;
	if (new_block_init_for_write((struct new_write_zone *)zone,
				     (struct new_write_block *)replacement,
				     replacement->tag)) {
		nand_print(nftl_ro_lc_292);
		/* OEM does not return the failed block to the free list. */
		goto retry;
	}
	compact = 0;
	for (page = 0; page < *(unsigned int *)nrc; page++) {
		if (((unsigned char *)nrc->first)[page]) {
			/* Stock multiplies in 32 bits before extending the result. */
			data = (unsigned char *)nrc->second +
				(unsigned int)(page * (zone->geometry->bytes + 64U));
			oob = data + zone->geometry->bytes;
			set_physic_op_par(&op, replacement->number,
					  (unsigned short)compact,
					  zone->geometry->fixed_page, data, oob);
			special = get_special_data_from_oob(oob);
			set_oob_special_page(zone, oob, special,
					     replacement->tag, replacement->erase_count);
			/* OEM ignores every replay-write result. */
			zone->write(zone, &op);
			compact++;
		} else {
			nand_print(nftl_ro_lc_293, page,
				   *(unsigned int *)nrc);
		}
	}
	/* Without an assist, second_erase remains zero, so OEM erases source
	 * using mode 1 regardless of its old erase count. */
	erase_block((struct erase_block_zone *)zone,
		    (struct erase_block_info *)source,
		    (unsigned short)second_erase + 1);
	free_nrc(nrc);
	return replacement;
}

struct cross_rebuild_block *no_cross_talk_rebuild_current_block(
	struct cross_rebuild_zone *zone, struct cross_rebuild_block *primary,
	struct cross_rebuild_block *assist)
{
	struct nrc_buffers *primary_nrc = 0, *assist_nrc = 0, *selected;
	struct cross_rebuild_block *source, *old, *replacement;
	struct physic_op_param op;
	unsigned int primary_used, assist_used = 0;
	unsigned int first_good = 0, second_good = 0;
	unsigned int page, compact, special;
	unsigned char *data, *oob;
	int status;

	if (init_nrc((struct nrc_zone *)zone, &primary_nrc)) {
		free_nrc(primary_nrc);
		/* OEM uses the cross-talk error string on this path. */
		nand_print(nftl_ro_lc_284);
		return 0;
	}
	primary_used = get_used_page_num_no_crosstalk(
		(struct used_page_zone *)zone, (struct physical_block *)primary,
		&first_good, (struct used_page_cache *)primary_nrc);
	if (!assist) {
		nand_print(nftl_ro_lc_294,
			   primary->number, primary_used, primary->tag);
		source = primary;
		selected = primary_nrc;
		if (!*(unsigned int *)selected) {
			free_nrc(primary_nrc);
			nand_print(nftl_ro_lc_295);
			erase_block((struct erase_block_zone *)zone,
				    (struct erase_block_info *)primary,
				    primary->erase_count + 1);
			return 0;
		}
		goto acquire_single;
	}
	if (init_nrc((struct nrc_zone *)zone, &assist_nrc)) {
		free_nrc(primary_nrc);
		free_nrc(assist_nrc);
		nand_print(nftl_ro_lc_288);
		return 0;
	}
	assist_used = get_used_page_num_no_crosstalk(
		(struct used_page_zone *)zone, (struct physical_block *)assist,
		&second_good, (struct used_page_cache *)assist_nrc);
	nand_print(nftl_ro_lc_296,
		   primary->number, primary_used, primary->tag,
		   assist->number, assist_used, assist->tag);
	if (!*(unsigned int *)assist_nrc) {
		free_nrc(primary_nrc);
		free_nrc(assist_nrc);
		nand_print(nftl_ro_lc_297);
		erase_block((struct erase_block_zone *)zone,
			    (struct erase_block_info *)primary,
			    primary->erase_count + 1);
		erase_block((struct erase_block_zone *)zone,
			    (struct erase_block_info *)assist,
			    assist->erase_count + 1);
		return 0;
	}
	if (assist_used > primary_used) {
		source = assist;
		old = primary;
		selected = assist_nrc;
		nand_print(nftl_ro_lc_289,
			   primary_used, assist_used, *(unsigned int *)selected);
	} else {
		source = primary;
		old = assist;
		selected = primary_nrc;
		nand_print(nftl_ro_lc_290,
			   primary_used, assist_used, *(unsigned int *)selected);
	}
	set_physic_op_par(&op, old->number, 0,
			  zone->geometry->fixed_page, 0, 0);
	status = zone->erase(zone, &op);
	if (status) {
		nand_print(nftl_ro_lc_298,
			   old->number);
		zone->mark_bad(zone, &op);
		/* OEM marks the selected source, not the block whose erase
		 * just failed. This is a persistent metadata bug. */
		source->page = 0;
		source->state = 255;
		(*(unsigned short *)((char *)zone + 28))++;
	} else {
		/* A successful erase reuses the old block as the replacement;
		 * stock only takes a free block after an erase failure. */
		replacement = old;
		replacement->tag = source->tag;
		goto replay;
	}
acquire_dual:
	replacement = (struct cross_rebuild_block *)
		out_phy_block_from_free_list((struct free_block_zone *)zone);
	if (!replacement) {
		nand_print(nftl_ro_lc_291);
		/* OEM leaks both NRC allocations on this exit. */
		return 0;
	}
	if (new_block_init_for_write((struct new_write_zone *)zone,
				     (struct new_write_block *)replacement,
				     replacement->tag)) {
		nand_print(nftl_ro_lc_292);
		goto acquire_dual;
	}
	replacement->tag = source->tag;
	goto replay;
acquire_single:
	replacement = (struct cross_rebuild_block *)
		out_phy_block_from_free_list((struct free_block_zone *)zone);
	if (!replacement) {
		nand_print(nftl_ro_lc_291);
		return 0;
	}
	replacement->tag = source->tag;
	if (new_block_init_for_write((struct new_write_zone *)zone,
				     (struct new_write_block *)replacement,
				     replacement->tag)) {
		nand_print(nftl_ro_lc_292);
		goto acquire_single;
	}
replay:
	compact = 0;
	for (page = 0; page < *(unsigned int *)selected; page++) {
		if (!((unsigned char *)selected->first)[page]) {
			nand_print(nftl_ro_lc_293, page,
				   *(unsigned int *)selected);
			continue;
		}
		data = (unsigned char *)selected->second +
			(unsigned int)(page * (zone->geometry->bytes + 64U));
		oob = data + zone->geometry->bytes;
		set_physic_op_par(&op, replacement->number,
				  (unsigned short)compact,
				  zone->geometry->fixed_page, data, oob);
		special = get_special_data_from_oob(oob);
		set_oob_special_page(zone, oob, special,
				     replacement->tag, replacement->erase_count);
		/* OEM ignores replay-write failures. */
		zone->write(zone, &op);
		compact++;
	}
	erase_block((struct erase_block_zone *)zone,
		    (struct erase_block_info *)source,
		    source->erase_count + 1);
	free_nrc(primary_nrc);
	free_nrc(assist_nrc);
	return replacement;
}

struct nand_cache_write_configuration {
	unsigned short direct;
	unsigned short sorted;
	unsigned char before_limit[20];
	unsigned short write_limit;
	unsigned short cross_talk;
};
struct nand_cache_write_zone {
	unsigned char before_geometry[8];
	struct nand_cache_read_geometry *geometry;
	unsigned char before_cache[1056];
	struct cache_init_data cache;
	unsigned char before_config[568];
	struct nand_cache_write_configuration *configuration;
	unsigned char before_callbacks[48];
	int (*write)(struct nand_cache_write_zone *, int, void *);
	int (*read)(struct nand_cache_write_zone *, int, void *);
};
_Static_assert(__builtin_offsetof(struct nand_cache_write_zone, cache) == 1072,
	       "write cache data offset");
_Static_assert(__builtin_offsetof(struct nand_cache_write_zone, configuration) == 3448,
	       "write cache configuration offset");
_Static_assert(__builtin_offsetof(struct nand_cache_write_zone, read) == 3512,
	       "write cache read callback offset");
int nand_cache_write(struct nand_cache_write_zone *zone, int page,
		     unsigned short bitmap, void *source)
{
	struct cache_init_data *cache;
	struct cache_init_entry *node;
	unsigned char *buffer;
	unsigned short start;
	unsigned short length;
	unsigned short existing;
	int status;

	if (!source)
		return 0;
	start = bitmap >> 8;
	length = bitmap & 0xff;
	start <<= 9;
	length <<= 9;
	status = 0;
	cache = &zone->cache;
	if (zone->configuration->direct) {
		buffer = cache->entries[0].allocation;
		/* OEM ignores the read status and writes the merged buffer. */
		zone->read(zone, page, buffer);
		nand_memcpy(buffer + start, source, length);
		status = zone->write(zone, page, buffer);
		return status;
	}
	if (!zone->configuration->sorted &&
	    bitmap == zone->geometry->sectors_per_page &&
	    !zone->configuration->cross_talk) {
		node = (struct cache_init_entry *)search_from_cache_write_list(
			(struct cache_write_zone *)cache, page);
		if (node)
			del_from_cache_write_list((struct cache_list_owner *)cache,
					  (struct cache_list_node *)node);
		node = (struct cache_init_entry *)search_from_cache_read_list(
			(struct cache_read_zone *)cache, page);
		if (node)
			del_from_cache_read_list(
				(struct cache_read_delete_owner *)cache,
				(struct cache_read_delete_node *)node);
		status = zone->write(zone, page, source);
		if (status) {
			nand_print(nftl_ro_lc_421, bitmap);
			return 1;
		}
		return 0;
	}
	node = (struct cache_init_entry *)search_from_cache_write_list(
		(struct cache_write_zone *)cache, page);
	if (node) {
		del_from_cache_write_list((struct cache_list_owner *)cache,
				  (struct cache_list_node *)node);
		buffer = node->allocation;
		existing = (unsigned short)(node->field1 << 9);
		if (node->field1 != zone->geometry->sectors_per_page) {
			if (start == existing) {
				node->field1 = (unsigned short)
					(node->field1 + (length >> 9));
			} else {
				status = zone->read(zone, page,
						    cache->page_buffer);
				/* OEM ignores read errors, and page_bytes minus
				 * existing can underflow when cache metadata is bad. */
				nand_memcpy(buffer + existing,
					    (unsigned char *)cache->page_buffer + existing,
					    (unsigned int)zone->geometry->page_bytes - existing);
				node->field1 = zone->geometry->sectors_per_page;
				if (status == 10)
					nand_print(nftl_ro_lc_422,
						   page);
			}
		}
		nand_memcpy(buffer + start, source, length);
		add_to_cache_write_list_tail((struct cache_list_owner *)cache,
					     (struct cache_list_node *)node);
		node = (struct cache_init_entry *)search_from_cache_read_list(
			(struct cache_read_zone *)cache, page);
		if (node)
			del_from_cache_read_list(
				(struct cache_read_delete_owner *)cache,
				(struct cache_read_delete_node *)node);
		return 0;
	}
	node = (struct cache_init_entry *)search_from_cache_read_list(
		(struct cache_read_zone *)cache, page);
	if (node) {
		buffer = node->allocation;
		nand_memcpy(buffer + start, source, length);
		node->field0 = 0;
		node->field1 = zone->geometry->sectors_per_page;
		del_from_cache_read_list(
			(struct cache_read_delete_owner *)cache,
			(struct cache_read_delete_node *)node);
		add_to_cache_write_list_tail((struct cache_list_owner *)cache,
					     (struct cache_list_node *)node);
		if (cache->field1 >= zone->configuration->write_limit &&
		    flush_write_cache_to_nand((struct flush_cache_zone *)zone, 0)) {
			nand_print(nftl_ro_lc_423);
			return 1;
		}
		return 0;
	}
	/* OEM retries until the write-cache count falls below the limit.
	 * A successful flush that does not reduce the count loops forever. */
	while (cache->field1 >= zone->configuration->write_limit) {
		if (flush_write_cache_to_nand((struct flush_cache_zone *)zone, 0)) {
			nand_print(nftl_ro_lc_424);
			return 1;
		}
	}
	node = (struct cache_init_entry *)get_empty_cahce_node(
		(struct cache_read_empty_zone *)cache);
	if (!node) {
		nand_print(nftl_ro_lc_420,
			   cache->field1, cache->field0, cache->field2);
		return 1;
	}
	node->page = page;
	buffer = node->allocation;
	if (start == 0) {
		node->field0 = 0;
		node->field1 = length >> 9;
	} else {
		status = zone->read(zone, page, buffer);
		if (status == 10)
			nand_print(nftl_ro_lc_425,
				   page);
		node->field0 = 0;
		node->field1 = zone->geometry->sectors_per_page;
	}
	nand_memcpy(buffer + start, source, length);
	add_to_cache_write_list_tail((struct cache_list_owner *)cache,
					     (struct cache_list_node *)node);
	return 0;
}
