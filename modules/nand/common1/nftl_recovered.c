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
		nand_print("set feature(addr %d) p0: %d readretry Configuration failed!\n",
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
	nand_print_dbg("rr value %x %x %x %x!\n", values[0],
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
	unsigned int count;
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
extern int nand_op(int kind, void *zone, unsigned int sector,
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
		nand_print_dbg("[ND]read sector:0x%x,len:0x%x\n", sector, count);
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
		nand_print_dbg("[ND]write sector:0x%x,len:0x%x\n", sector, count);
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

unsigned int get_spare_data(void *oob, unsigned int offset,
			    unsigned int bytes)
{
	unsigned char offset8 = offset;
	unsigned char bytes8 = bytes;
	unsigned int index = 0;
	unsigned int value = 0;

	while (bytes8 > index) {
		value <<= 8;
		value |= ((unsigned char *)oob)[(unsigned int)offset8 + index];
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
		nand_print("[NE]write logic data to current block error\n");
		return 1;
	}
	/* OEM writes the second plane only when bit zero of byte 25 is set.
	 * The first plane may already have committed before a second failure. */
	if ((((struct write_page_zone *)zone)->flags & 1) &&
	    write_logic_data_to_current_block(zone,
			(unsigned char *)zone + 592, marker, page, data)) {
		nand_print("[NE]write logic data to current block error\n");
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
	volatile unsigned int count;
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
struct invalid_node *get_block_max_count_from_invalid_list(
	struct invalid_zone *zone)
{
	struct invalid_node *best = 0;
	unsigned int maximum = 0;
	struct invalid_node *node = zone->head.next;

	/* OEM selects the later node on ties and reads the chosen count
	 * twice. It never detects a cycle. */
	while (node) {
		if (node->count >= maximum) {
			maximum = node->count;
			best = node;
		}
		node = node->next;
	}
	return best;
}
struct invalid_node *get_block_min_count_from_invalid_list(
	struct invalid_zone *zone)
{
	struct invalid_node *best = 0;
	unsigned int minimum = ~0U;
	struct invalid_node *node = zone->head.next;

	while (node) {
		if (node->count <= minimum) {
			minimum = node->count;
			best = node;
		}
		node = node->next;
	}
	return best;
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
	nand_print_dbg("[ND]block_used_count list: \n");
	node = node->next;
	/* OEM reads count and next after the preceding print calls, so
	 * callbacks can change what the next iteration observes. */
	while (node) {
		nand_print_dbg("[ND]block NO: %d ", node->block);
		nand_print_dbg("[ND]block_used_count: %d \n", node->count);
		node = node->next;
	}
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
struct cache_read_empty_node *get_empty_cahce_node(
	struct cache_read_empty_zone *zone)
{
	struct cache_read_empty_node *node;
	unsigned short index;
	unsigned short desired_state;

	for (index = 0; index < zone->node_count; index++) {
		/* OEM trusts node_count, even beyond the 30 in-place nodes. */
		if (zone->nodes[index].state == 255)
			return &zone->nodes[index];
	}
	/* OEM's count threshold selects only one state; it does not fall back
	 * to the other list state and does not detect a list cycle. */
	desired_state = zone->second_count > 9 ? 2 : 0;
	for (node = zone->head.next; node; node = node->next) {
		if (node->state == desired_state)
			return (struct cache_read_empty_node *)
				del_from_cache_read_list(
					(struct cache_read_delete_owner *)zone,
					(struct cache_read_delete_node *)node);
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
extern int nand_sector_cache_read(void *zone, unsigned int sector, void *data);
extern int nand_sector_cache_write(void *zone, unsigned int sector, void *data);
int __nand_sector_read(void *zone, unsigned int sector, unsigned int count,
		       void *data)
{
	int result = 0;
	unsigned int current = sector;
	unsigned long pointer = (unsigned long)data;
	unsigned int index = 0;

	while (index < count) {
		/* OEM ORs every result and never stops on an error. */
		result |= nand_sector_cache_read(zone, current, (void *)pointer);
		current++;
		pointer += 512;
		index++;
	}
	return result;
}
int __nand_sector_write(void *zone, unsigned int sector, unsigned int count,
			void *data)
{
	int result = 0;
	unsigned int current = sector;
	unsigned long pointer = (unsigned long)data;
	unsigned int index = 0;

	while (index < count) {
		result |= nand_sector_cache_write(zone, current, (void *)pointer);
		current++;
		pointer += 512;
		index++;
	}
	return result;
}
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
struct sector_read_hit {
	unsigned char before_data[8];
	void *data;
};
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
		nand_print("[NE]nand_sector_cache_read 1!\n");
		return 1;
	}
	return result;
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

struct do_after_zone;
extern int do_after_write(struct do_after_zone *zone);
extern int do_before_write(void *zone);
int nand_write_data_page(void *zone, unsigned int marker,
			 unsigned int page, void *data)
{
	if (current_block_at_last_page((struct current_zone *)zone,
				       (struct current_page *)((char *)zone + 528)) == 1) {
		if (do_after_write((struct do_after_zone *)zone)) {
			nand_print("[NE]do after write error1!\n");
			return 1;
		}
	}
	if (do_before_write(zone)) {
		nand_print("[NE]do_before_write error!\n");
		return 1;
	}
	if (write_data_to_page(zone, marker, page, data)) {
		nand_print("[NE]write data to page error!\n");
		return 1;
	}
	if (do_after_write((struct do_after_zone *)zone)) {
		nand_print("[NE]do after write error2!\n");
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
	int result;
	struct physic_op_param op;

	set_physic_op_par(&op, block->number, 0, zone->geometry->page, 0, 0);
	result = zone->erase(zone, &op);
	/* OEM resets page and increments erase count before checking erase
	 * failure; retain the resulting metadata update on a failed erase. */
	block->page = 0;
	block->erase_count++;
	if (block->erase_count > zone->max_erase &&
	    block->erase_count < zone->geometry->erase_limit)
		zone->max_erase = block->erase_count;
	block->tag = tag;
	if (result) {
		zone->mark_bad(zone, &op);
		nand_print("[NE]new_block_init_for_write erase error!\n");
		return 1;
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
	struct physic_op_param op;
	struct physical_block *block = (struct physical_block *)
		((unsigned char *)zone->chip->blocks + ((unsigned long)start << 6));
	unsigned int range = end - start;
	unsigned int remaining = range;
	unsigned int index = start;

	/* OEM compares the absolute index with the range length. Preserve this
	 * mistake: nonzero start can skip work or stop before end. */
	while (index < range) {
		op.block = block->number;
		if (zone->check(zone, &op) == 159) {
			remaining--;
			if (remaining == 0)
				return 0;
		}
		if (is_last_phy_block(block, (struct physical_zone *)zone) == 1)
			break;
		block = (struct physical_block *)((unsigned char *)block + 64);
		index++;
	}
	return remaining;
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
int gc_all_enhance(struct gc_enhance_zone *zone)
{
	struct gc_phyinfo_view *info = (struct gc_phyinfo_view *)phyinfo;
	unsigned int used = zone->used;
	unsigned long quantity;

	if (zone->free_blocks > used)
		return 0;
	/* OEM multiplies in 32 bits and can wrap before this comparison. */
	if ((unsigned int)zone->invalid_blocks * 3 < used)
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
int hynix16nm_set_readretry(struct hynix16nm_retry_zone *zone)
{
	int result = 0;
	unsigned int index;
	unsigned char values[4];

	if (zone->retry > hynix16nm_read_retry_cycle)
		return -74;
	/* OEM trusts the register count although this stack array has four
	 * bytes; a larger count overwrites adjacent stack storage. */
	for (index = 0; index < hynix16nm_read_retry_reg_num; index++)
		values[index] = zone->values[(zone->retry << 2) + index];
	result = hynix16nm_vender_set_param(zone, values,
					 hynix16nm_read_retry_reg_adr,
					 hynix16nm_read_retry_reg_num);
	return result;
}

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
	nand_print_dbg("rr value %x %x %x %x!\n", values[0],
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
	nand_print_dbg("set retry default value: ");
	for (locals.index = 0; locals.index < hynix16nm_read_retry_reg_num;
	     locals.index++)
		nand_print_dbg(" %x", locals.values[locals.index]);
	nand_print_dbg("\n");
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
extern int hynix16nm_read_otp_info_hynix(struct hynix16nm_otp_zone *zone,
					  void *buffer);
extern int hynix16nm_get_read_retry_cfg(unsigned char *mode,
					 unsigned char *cycle,
					 unsigned char *values,
					 void *buffer);
int hynix16nm_get_rr_value_otp_hynix(struct hynix16nm_otp_zone *zone)
{
	int result = 0;
	void *buffer = nand_get_temp_buf(528);
	unsigned char cycle;
	unsigned char mode;

	if (!buffer) {
		nand_print("otp_info_hynix_16nm : allocate memory fail\n");
		return -78;
	}
	result = hynix16nm_read_otp_info_hynix(zone, buffer);
	/* OEM still parses the buffer after a failed OTP read. */
	if (result < 0)
		nand_print("m1 CH %d chip %d get otp info failed!\n",
			   zone->controller->channel, zone->chip);
	result = hynix16nm_get_read_retry_cfg(&mode, &cycle, zone->values,
					       buffer);
	if (result < 0)
		nand_print("m1 CH %d chip %d get read retry cfg from otp info failed!\n",
			   zone->controller->channel, zone->chip);
	nand_free_temp_buf(buffer);
	return result;
}
int hynix16nm_get_otp_param(struct hynix16nm_otp_zone *zone,
			    unsigned char *values)
{
	unsigned int attempt = 0;
	unsigned int complete = 0;
	int result;
	unsigned int group;
	unsigned int index;

	/* OEM retries forever when OTP parsing never returns zero; keep the
	 * unbounded loop for stock behavior. */
	while (complete == 0) {
		nand_print_dbg("_vender_get_param_otp_hynix time %d!\n",
			       attempt);
		result = hynix16nm_get_rr_value_otp_hynix(zone);
		if (result == 0)
			complete = 1;
		attempt++;
	}
	for (group = 0; group <= 7; group++) {
		for (index = 0; index <= 3; index++)
			values[(group << 2) + index] =
				zone->values[(group << 2) + index];
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
static const unsigned char sandisk_activation_commands[9] = {
	0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53, 0x53
};
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
		nand_print("sandisk activation seq fail 0x%x\n", result);
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
/* OEM's page scan has an OOB stack array but no canary prologue. */
__attribute__((optimize("-fno-stack-protector")))
int check_cross_talk(struct cross_talk_zone *zone,
		     struct physical_block *block)
{
	void *data = zone->data;
	unsigned int index = 0;
	struct physic_op_param op;
	unsigned char oob[16];
	unsigned char *oobptr;
	int result;

	for (; index < zone->geometry->pages; index++) {
		data = 0;
		oobptr = oob;
		set_physic_op_par(&op, block->number, index,
				  zone->geometry->fixed_page, data, oobptr);
		result = zone->read(zone, &op);
		if (result != 0)
			continue;
		if (oobptr[14] > 99)
			continue;
		if (zone->stats->max > oobptr[14])
			continue;
		zone->stats->max = oobptr[14];
		break;
	}
	/* OEM reports success even if every physical read failed. */
	return 0;
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
		nand_print("sandisk_A19 retry mode err : %d !\n",
			   sandisk_A19_read_retry_mode);
		return -99;
	}
	nand_print_dbg("sandisk A19 read retry conut: %d !\n",
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
	nand_print_dbg("sandisk read retry conut: %d !\n",
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
		nand_print("[NE]%s:malloc 8192 fail!\n", __func__);
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
	unsigned char oob[16];
	struct physic_op_param op;
	void *buffer = nftl_malloc(8192);
	struct physical_block *physical;
	int result;

	/* OEM logs allocation failure, then passes NULL to nand_memset and
	 * the write callback. Preserve the fault-prone diagnostic path. */
	if (!buffer)
		nand_print("[NE]%s:malloc 8192 fail!\n", __func__);
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

void set_spare_data(unsigned char *oob, int value, int offset, int count)
{
	unsigned char offset8 = offset;
	unsigned char count8 = count;
	unsigned int index = 0;
	unsigned int shift;

	while (index < count8) {
		shift = (count8 - index - 1) * 8;
		/* AArch64 masks variable 32-bit shift counts to five bits.
		 * Preserve that OEM wraparound for counts above four. */
		oob[(unsigned int)offset8 + index] =
			(unsigned char)((unsigned int)value >> (shift & 31));
		index++;
	}
}
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

void nftl_version(void)
{
	nand_print("sunxi nftl version %s.%s.%s  %s-%s\n",
		   "5", "0", "000", "2020.12.24", "09:50");
}

struct discard_zone {
	unsigned char before_debug[0x290];
	int debug_level;
};
extern int nand_discard(struct discard_zone *zone, unsigned int sector,
			unsigned int length);
int __nand_discard(struct discard_zone *zone, unsigned int sector,
		   unsigned int length)
{
	int result;

	if (zone->debug_level == 3)
		/* Preserve OEM's misspelled diagnostic. */
		nand_print_dbg("[ND]dicard sector:0x%x,len:0x%x\n",
			       sector, length);
	result = nand_discard(zone, sector, length);
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
extern void nftl_free(void *ptr);
int free_logic_page_map(struct exit_zone *zone)
{
	unsigned int index;

	for (index = 0; index <= 24; index++) {
		/* OEM probes each pointer and then reads it again for the free.
		 * The zone layout is inferred from slot offsets 72..264.
		 * It leaves freed pointers dangling and never checks zone. */
		void *const volatile *slot = (void *const volatile *)
			((char *)zone + 72 + index * 8);

		if (*slot)
			nftl_free(*slot);
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
	nand_print_dbg("hynix26nm_get_default_param: ch: %d, chip: %d, value: 0x%x 0x%x 0x%x 0x%x \n",
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
	nand_print_dbg("hynix16nm_readretry_exit. \n");
	hynix16nm_set_default_param(chip);
	chip->retry_count = 0;
	return 0;
}

int hynix20nm_readretry_exit(struct nftl_retry_chip *chip)
{
	nand_print_dbg("hynix20nm_readretry_exit. \n");
	hynix20nm_set_default_param((struct otp_chip *)chip);
	chip->retry_count = 0;
	return 0;
}

int hynix26nm_readretry_exit(struct nftl_retry_chip *chip)
{
	nand_print_dbg("hynix26nm_readretry_exit. \n");
	hynix26nm_setdefaultparam(chip);
	chip->retry_count = 0;
	return 0;
}

void *nftl_malloc(unsigned int size)
{
	/* OEM warns above 1.5 MiB but still attempts the allocation. */
	if (size > 0x180000)
		nand_print("[NE]malloc size too large %d!\n", size);
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
	unsigned char before_count[4];
	unsigned short invalid_pages;
	unsigned char before_next[10];
	struct invalid_page_node *next;
};

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

/* Isolated OEM NFTL CRC16 candidates and exact recovered lookup data. */
static const unsigned short nand_crc16_table[256] = {
	0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
	0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
	0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
	0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
	0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
	0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
	0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
	0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
	0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
	0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
	0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
	0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
	0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
	0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
	0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
	0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
	0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
	0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
	0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
	0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
	0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
	0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
	0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
	0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
	0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
	0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
	0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
	0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
	0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
	0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
	0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
	0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040,
};

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
extern void nand_clean_zone_table2(void *zone);

struct nftl_cache_entry_view {
	unsigned char before_allocation[24];
	void *allocation;
	unsigned char after_allocation[24];
};
int nftl_cache_exit(void *zone)
{
	unsigned char *cache = (unsigned char *)zone + 1072;
	int index;

	for (index = 0; index < *(unsigned short *)cache; index++) {
		struct nftl_cache_entry_view *entry =
			(struct nftl_cache_entry_view *)(cache + index * 56);
		/* OEM calls nftl_free even for null entries and re-reads the
		 * count after each call. It never clears an allocation slot. */
		nftl_free(entry->allocation);
	}
	nftl_free(*(void **)(cache + 1800));
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
extern int hynix16nm_init_readretry_param(struct hynix16nm_init_chip *);
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
		nand_print("NFC_ReadRetryInit, unknown read retry mode 0x%x\n",
			   hynix16nm_read_retry_mode);
		return -75;
	}
	for (index = 0; index <= 127; index++)
		chip->otp[index] = 0;
	/* OEM discards OTP setup failure and reports initialization success. */
	hynix16nm_init_readretry_param(chip);
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
int write_data_to_nand(void *zone)
{
	int sector;
	void *buffer = nftl_malloc(512);

	/* OEM logs allocation failure, then still dereferences the null
	 * buffer in nand_memset and continues. Keep this broken path. */
	if (!buffer)
		nand_print("[NE][NE]%s:malloc 512 fail!\n", __func__);
	nand_memset(buffer, -86, 512);
	/* OEM diagnostic overwrites all 38,912 sectors with 0xaa. Its fixed
	 * bound does not consult the zone capacity; never invoke casually. */
	for (sector = 0; sector <= 38911; sector++)
		__nand_write(zone, sector, 1, buffer);
	nftl_free(buffer);
	/* OEM ignores every write result, even after a failed allocation. */
	return 0;
}
void _DumpHexData(const unsigned char *data, int length)
{
	const unsigned char *current = data;
	int index = 0;

	/* OEM inserts a newline after each sixteenth byte and another at the
	 * end, even when length is zero or the final row is exactly full. */
	while (index < length) {
		nand_print("%.2x ", *current++);
		if (index % 16 == 15)
			nand_print("\n");
		index++;
	}
	nand_print("\n");
}
void dump_buf(const unsigned char *data, unsigned int length)
{
	int index = 0;

	nand_print("\n");
	index = 0;
	/* OEM emits both a line terminator at each 16-byte boundary and a
	 * final terminator, so a full final row gets two newlines. */
	while (index < length) {
		if ((index & 15) == 0)
			nand_print("%08x: ", index);
		nand_print("%02x ", data[index]);
		if (((index + 1) & 15) == 0)
			nand_print("\n");
		index++;
	}
	nand_print("\n");
}
int nand_check_table(void *zone)
{
	void *block = zone;

	__nand_flush_write_cache(block, 1000);
	nand_print_dbg("[ND]==========================\n");
	print_nftl_zone(block);
	nand_print_dbg("[ND]==========================\n");
	nand_clean_zone_table(block);
	nand_print_dbg("[ND]==========================\n");
	nand_find_zone_table(block);
	write_data_to_nand(block);
	nand_print_dbg("[ND]==========================\n");
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

extern int hynix20nm_vender_get_param_otp_hynix(struct otp_chip *chip);
int hynix20nm_getotpparam(struct otp_chip *chip, unsigned char *value)
{
	unsigned int attempt = 0;
	unsigned int row;
	unsigned int column;
	int result;

	/* The OEM makes five attempts and reports the last vendor error.
	 * Its debug message prints the zero-based attempt number. */
	do {
		nand_print_dbg("_vender_get_param_otp_hynix time %d!\n",
			       attempt);
		result = hynix20nm_vender_get_param_otp_hynix(chip);
		if (result == 0)
			break;
		attempt++;
	} while (attempt <= 4);
	if (result != 0) {
		nand_print("_vender_get_param_otp_hynix error!\n");
		return result;
	}
	for (row = 0; row <= 7; row++)
		for (column = 0; column <= 7; column++)
			value[(row << 3) + column] =
				chip->saved[(row << 3) + column];
	return 0;
}

extern unsigned char hynix20nm_read_retry_reg_num;
extern unsigned char hynix20nm_read_retry_reg_adr[];
extern unsigned char hynix20nm_read_retry_cycle;
struct hynix20nm_retry_chip {
	unsigned char before_saved[56];
	unsigned char saved[128];
	unsigned int cycle;
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
	nand_print_dbg("retry para:");
	for (locals.index = 0; locals.index < hynix20nm_read_retry_reg_num;
	     locals.index++)
		nand_print_dbg(" %x ", locals.values[locals.index]);
	nand_print_dbg("\n");
	return locals.result;
}
/* Stock omits a canary although its unchecked count can overrun the buffer. */
__attribute__((optimize("-fno-stack-protector")))
int hynix20nm_set_default_param(struct otp_chip *chip)
{
	struct {
		unsigned char values[8];
		int result;
		unsigned int index;
	} locals;

	locals.result = 0;
	/* The OEM trusts the mutable register count. Past eight bytes it
	 * overwrites the result, then the loop index, in its own stack frame. */
	for (locals.index = 0; locals.index < hynix20nm_read_retry_reg_num;
	     locals.index++)
		locals.values[locals.index] = chip->saved[locals.index];
	locals.result = hynix20nm_vender_set_param(chip, locals.values,
					hynix20nm_read_retry_reg_adr,
					hynix20nm_read_retry_reg_num);
	nand_print_dbg("set retry default value: ");
	for (locals.index = 0; locals.index < hynix20nm_read_retry_reg_num;
	     locals.index++)
		nand_print_dbg(" %x", locals.values[locals.index]);
	nand_print_dbg("\n");
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
	nand_print_dbg("set retry default value: ");
	for (locals.index = 0; locals.index < hynix26nm_read_retry_reg_num;
	     locals.index++)
		nand_print_dbg(" %x", locals.values[locals.index]);
	nand_print_dbg("\n");
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
		nand_print_dbg("[ND]cap: 0x%08x.\n", node->next->capacity);
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
			nand_print("[NE]parameter error %d,%d !\n", sector, count);
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
			nand_print("[NE]parameter error %d,%d !\n", sector, count); \
			return 1; \
		} \
	} \
	return node->member(node, sector, count, data); \
}

NFTL_RW(nftl_read, read)
NFTL_RW(nftl_write, write)

/* Inferred NFTL orchestration body; callback signatures remain provisional. */

extern void clean_block_list(void *);
extern void nand_clean_zone_table2(void *);
struct build_zone;
extern int build_zone_list(struct build_zone *);
extern void nftl_cache_init(void *);
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
	nand_print_dbg("[ND]build_zone_list start\n");
	build_zone_list(current);
	nand_print_dbg("[ND]nftl_cache_init start\n");
	nftl_cache_init(current);
	nand_print_dbg("[ND]=================================2=======================================\n");
	print_block_invalid_list(current);
	print_free_list(current);
	nand_print_dbg("[ND]=================================2=======================================\n");
	nand_print_dbg("[ND]nand_find_zone_table end\n");
	return 0;
}

struct build_zone {
	unsigned char before_count[26];
	unsigned short block_count;
};
extern int first_scan_all_blocks(struct build_zone *);
extern int init_zone_after_first_scan(struct build_zone *, unsigned short);
extern int second_scan_all_blocks(struct build_zone *);
extern int power_on_gc(struct build_zone *);
extern int panic_backup_block(struct build_zone *);
int build_zone_list(struct build_zone *zone)
{
	int result;

	nand_print_dbg("[ND]first\n");
	if (first_scan_all_blocks(zone))
		return 1;
	result = init_zone_after_first_scan(zone, zone->block_count);
	if (result) {
		nand_print("[NE]init_zone_after_first_scan error\n");
		return 1;
	}
	result = second_scan_all_blocks(zone);
	if (result) {
		nand_print("[NE]second_scan_all_blocks error\n");
		return 1;
	}
	/* OEM still reports success if GC or panic backup fails. */
	result = power_on_gc(zone);
	print_nftl_zone(zone);
	nand_print("[NI]nftl ok!\n");
	panic_backup_block(zone);
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

int nand_sector_cache_write(void *zone, unsigned int sector, void *data)
{
	struct sector_owner *cache =
		(struct sector_owner *)((unsigned char *)zone + 2880);
	struct sector_node *node = (struct sector_node *)
		search_from_sector_cache_write_list(
			(struct sector_cache_zone *)cache, sector);
	unsigned short bytes = 512;
	int result;

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
			nand_print("[NE]nand_sector_cache_write 1!\n");
			return 1;
		}
	} else {
		node = get_empty_sector_cahce_node(
			(struct empty_sector_pool *)cache);
		if (!node) {
			nand_print("[NE]nand_sector_cache_write 2!\n");
			return 1;
		}
	}
	node->sector = sector;
	nand_memcpy(node->data, data, bytes);
	add_to_sector_cache_write_list_tail(cache, node);
	return 0;
}

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
		nand_print("[NE]nand_flush_write_cache write error\n");
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
extern int flush_write_cache_to_nand(void *zone, int count);
int __nand_flush_write_cache(void *zone, unsigned int limit)
{
	unsigned int flushed = 0;
	struct write_flush_list *cache =
		&((struct write_flush_zone *)zone)->cache;
	int result = 0;

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
		nand_print("[NE]nand_flush_write_cache write error\n");
	if (flushed && ((struct write_flush_zone *)zone)->debug_level)
		nand_print_dbg("[ND]flush cache %d %d!\n",
			       ((struct write_flush_zone *)zone)->id, flushed);
	return result;
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
	nand_print_dbg(" generic special init \n");
	return 0;
}

extern unsigned int nand_on_panic;
extern unsigned int panic_enable;

void panic_mark_panic(void)
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
	nand_print("[ND] panic part %s off %u size %u\n", name, offset, size);
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
			nand_print_dbg("[ND]gc_one ok! from: %d [%d %d %d] @%s %d\n",
				       gc_src_type, zone->first_stat, zone->second_stat,
				       zone->third_stat, nand_get_cur_task_name(),
				       nand_get_cur_task_pid());
			return 0;
		}
		/* OEM collapses every nonzero, non-eight result to one. */
		nand_print("[NE][NE]gc_one error!\n");
		return 1;
	}
	return 8;
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
int gc_win_block(struct gc_win_zone *zone, unsigned short blocks)
{
	unsigned int progress = 0;
	int limit = (int)((unsigned int)blocks * zone->chip->pages_per_block);
	int result;

	/* OEM uses a signed comparison on wrapped 32-bit progress and silently
	 * succeeds if the current node is absent. */
	while ((int)progress < limit) {
		if (!zone->current)
			return 0;
		progress += zone->current->count;
		progress--;
		result = gc_one((struct gc_one_zone *)zone, 1);
		if (result) {
			if (result == 1)
				nand_print_dbg("[ND][NE] gc_win_block gc_one error \n");
			return result;
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
	nand_print_dbg("[ND]%s %p %d %d %d %\n", __func__, current,
		       zone->enabled, zone->first, zone->second, zone->third);
	while (zone->first <= zone->third) {
		current = zone->current;
		if (!current) {
			nand_print("[NI]shutdown_on_gc end! [%d %d %d]\n",
				   zone->first, zone->second, zone->third);
			break;
		}
		/* With enabled==0, stock may loop forever without changing the
		 * counters. When enabled, it ignores gc_one failure and still logs
		 * success. Both behaviors are retained. */
		if (zone->enabled) {
			gc_one((struct gc_one_zone *)zone, 0);
			nand_print_dbg("[ND]shutdown_on_gc one ok! [%d %d %d]\n",
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

struct read_claim_node *get_nftl_need_read_claim(struct read_claim_node *head)
{
	struct read_claim_node *last =
		(struct read_claim_node *)get_last_nftl((struct nftl_list_node *)head);
	int flag = last->zone->settings->flag;
	struct read_claim_node *node = head;

	/* OEM compares the one-byte zone marker to the last node's 32-bit
	 * setting and trusts every pointer in the initial lookup. */
	while (node) {
		if (node->zone->need_claim == flag)
			break;
		node = node->next;
	}
	return node;
}

struct special_exit_chip {
	struct special_exit_chip *next;
};
struct special_exit_info {
	void *before_chips;
	struct special_exit_chip *chips;
};
extern struct special_exit_info *g_nsi;

#define SPECIAL_EXIT(vendor) \
	int vendor##_special_exit(void) \
	{ \
		struct special_exit_chip *chip; \
		/* OEM ignores retry-exit errors and follows unchecked links. */ \
		for (chip = g_nsi->chips; chip != 0; chip = chip->next) \
			vendor##_readretry_exit((void *)chip); \
		nand_print_dbg(" " #vendor "_special_exit \n"); \
		return 0; \
	}

SPECIAL_EXIT(samsung)
SPECIAL_EXIT(hynix20nm)
SPECIAL_EXIT(micron)
SPECIAL_EXIT(toshiba)
SPECIAL_EXIT(hynix26nm)
SPECIAL_EXIT(hynix16nm)
SPECIAL_EXIT(sandisk)

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
				nand_print("%s sandisk toggle vendor specific setting failed!\n",
					   __func__);
				return result;
			}
		}
		chip = chip->next;
	}
	nand_print_dbg(" sandisk_A19_special_exit \n");
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


extern int garbage_collect(void *zone);
int _nftl_nand_write_logic_page(void *zone, unsigned int page, void *data)
{
	/* OEM collapses every nonzero error into one, discarding its code. */
	if (nand_write_logic_page_no_gc(zone, page, data) != 0) {
		nand_print("[NE]error nand_write_logic_page_no_gc page_no:%u\n",
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
extern int nftl_initialize(void *block, unsigned short number);
extern void panic_recover_data(void *block);
struct nftl_build_node *nftl_add(struct nftl_build_node *node)
{
	struct nftl_add_block *block = nftl_malloc(136);

	if (!block) {
		nand_print("[NE]%s:malloc fail for nftl_blk\n", __func__);
		return 0;
	}
	block->partition = build_nand_partition(node);
	if (nftl_initialize(block, node->number)) {
		/* OEM leaks the allocated block on initialization failure. */
		nand_print("[NE]nftl_initialize failed\n");
		return 0;
	}
	add_nftl_blk_list((struct nftl_chain *)&nftl_blk_head,
			  (struct nftl_chain *)block);
	/* OEM ignores panic recovery failure after publishing the new block. */
	panic_recover_data(block);
	nand_print_dbg("[ND]nftl_add ok\n");
	return (struct nftl_build_node *)block;
}
int nftl_build_all(struct nftl_build_owner *owner)
{
	struct nftl_build_node *node = owner->first;

	nftl_blk_head.next = 0;
	while (node) {
		if (!nftl_add(node)) {
			/* OEM discards the underlying add failure. */
			nand_print("[NE]nftl_build_all error!\n");
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
		if (node->number == number && !nftl_add(node)) {
			/* OEM discards the underlying add failure. */
			nand_print("[NE]nftl_build_all error!\n");
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
	unsigned int logic_page;
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
extern void _nftl_nand_read_logic_page(void);
void nftl_ops_init(struct nftl_ops *ops)
{
	/* OEM never clears other slots or checks the pointer. */
	ops->erase = (void (*)(void))_nftl_nand_erase_superblk;
	ops->read = (void (*)(void))_nftl_nand_read_page;
	ops->write = (void (*)(void))_nftl_nand_write_page;
	ops->good = (void (*)(void))_nftl_nand_is_blk_good;
	ops->bad = (void (*)(void))_nftl_nand_mark_bad_blk;
	ops->copy = (void (*)(void))_nftl_nand_copy_page;
	ops->read_logic = _nftl_nand_read_logic_page;
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
		nand_print_dbg("[ND]add invalid_list error !\n");
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
	int index;
	struct physical_block *block;
	struct panic_backup_geometry *geometry;

	if (!is_panic_enable())
		return 0;
	for (index = 0; index <= 2; index++) {
		block = panic_info[index];
		/* OEM returns immediately on the first existing reservation,
		 * even if a later slot is empty. Preserve this early exit. */
		if (block) {
			nand_print("[ND] reserved panic block before %d!\n",
				   block->number);
			return 0;
		}
		block = (struct physical_block *)
			out_phy_block_from_free_list((struct free_block_zone *)zone);
		if (block) {
			nand_print("[NE] reserved panic block %d!\n",
				   block->number);
			panic_info[index] = block;
		} else {
			nand_print("[NE] no reserved panic block!!! quit!\n");
			return -1;
		}
	}
	geometry = ((struct panic_backup_zone *)zone)->geometry;
	/* OEM stores a 32-bit page count over the fourth pointer slot. */
	*(unsigned int *)&panic_info[3] = geometry->pages * 3;
	return 0;
}
int panic_recover_info(struct panic_header *header, struct panic_block *block)
{
	/* OEM follows both pointers unchecked and rejects indices 3 and 4\n	 * despite reserving five pointer slots. */
	unsigned int index = ((unsigned char *)header + 4)[0];

	if (index <= 2) {
		nand_print("[ND] find panic block : %d\n", block->block);
		panic_info[index] = block;
		return 0;
	}
	nand_print("[NE] invalid panic block index %d\n", index);
	return -1;
}

unsigned char toshiba_read_retry_start_cmd[2] = { 0x5c, 0xc5 };
int toshiba_vender_pre_condition(void *chip)
{
	int result;

	result = set_cmd_with_nand_bus(chip, toshiba_read_retry_start_cmd,
				       0, 0, 0, 0, 2);
	if (result != 0) {
		nand_print("toshiba vender_pre_condition error!\n");
		return result;
	}
	return 0;
}

unsigned char sandisk_A19_read_retry_cmd_1y = 0xef;
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
extern int mark_free_block(struct erase_block_zone *, struct erase_block_info *);
int erase_block(struct erase_block_zone *zone, struct erase_block_info *block,
		unsigned short mode)
{
	struct physic_op_param op;
	int result;

	nand_print("[NI]erase_block block %d!\n", block->number);
	set_physic_op_par(&op, block->number, 0, zone->geometry->fixed_page,
			  0, 0);
	result = zone->erase(zone, &op);
	if (result) {
		nand_print("[NE]erase_block block fail %d!\n",
			   block->number);
		/* OEM ignores the bad-block callback result and returns the
		 * original erase status after updating the metadata. */
		zone->mark_bad(zone, &op);
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
		nand_print("[NE]adjust free list fail!!\n");
	erase_block((struct erase_block_zone *)zone,
		    (struct erase_block_info *)block,
		    (unsigned short)(block->erase_time + 1));
	block = (struct adjust_free_block *)out_phy_block_from_free_list_by_block(
		(struct nftl_free_erase_owner *)zone,
		(struct nftl_free_erase_node *)block);
	zone->current = block;
	/* The second removal may also return null, which OEM dereferences. */
	nand_print("[NI]adjust free list : %d %d!!\n",
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
		nand_print_dbg("[ND]free list: \n");
		node = node->next;
		/* OEM prints every node without checking cycles or link validity. */
		while (node) {
			nand_print_dbg("[ND]block NO:%4d; erase:%3d; ",
				       node->number, node->erase_time);
			nand_print_dbg("[ND]count:%4d; ", node->count);
			nand_print_dbg("[ND]info:0x%x\n", node->info);
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
		nand_print_dbg("[ND]invalid_page_count list: \n");
		node = node->next;
		/* OEM trusts every forward link and can loop on a cycle. */
		while (node) {
			nand_print_dbg("[ND]block NO:%4d; erase:%3d; ",
				       node->number, node->erase_time);
			nand_print_dbg("[ND]count:%4d; invalid:%3d;",
				       node->count, node->invalid);
			nand_print_dbg("[ND]info:0x%x\n", node->info);
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
};
struct samsung_read_controller {
	unsigned char before_channel[12];
	unsigned int channel;
};
struct samsung_read_chip_id {
	unsigned char before_id[28];
	unsigned int chip;
};
int samsung_read_page_end(struct samsung_read_request *request)
{
	struct samsung_set_chip *chip;
	int result;
	int retry;

	retry = 0;
	result = 0;
	chip = (struct samsung_set_chip *)nci_get_from_nsi(g_nsi,
						      request->channel);
	result = generic_read_page_end_not_retry(request);
	if (result == -2) {
		nand_print_dbg("samsung retry!\n");
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
						"samsung ReadRetry ok! ch =%d, chip = %d  block = %d, page = %d, RetryCount = %d  \n",
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
extern int micron_set_readretry(struct samsung_set_chip *);
int micron_read_page_end(struct samsung_read_request *request)
{
	struct samsung_set_chip *chip;
	int result;
	int retry;

	retry = 0;
	result = 0;
	chip = (struct samsung_set_chip *)nci_get_from_nsi(g_nsi,
						      request->channel);
	result = generic_read_page_end_not_retry(request);
	if (result == -2) {
		nand_print_dbg("micron retry!\n");
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
						"micron ReadRetry ok! ch =%d, chip = %d  block = %d, page = %d, RetryCount = %d  \n",
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
int toshiba_read_page_end(struct samsung_read_request *request)
{
	struct samsung_set_chip *chip;
	int result;
	int retry;

	retry = 0;
	result = 0;
	chip = (struct samsung_set_chip *)nci_get_from_nsi(g_nsi,
						      request->channel);
	result = generic_read_page_end_not_retry(request);
	if (result == -2) {
		nand_print_dbg("toshiba retry!\n");
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
						"toshiba ReadRetry ok! ch =%d, chip = %d  block = %d, page = %d, RetryCount = %d  \n",
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
extern int set_one_cmd(struct command_chip *chip, unsigned int command,
			unsigned int argument);
extern unsigned char sandisk_A19_read_retry_reg_adr_1y;
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

extern int set_one_addr(struct command_chip *chip, unsigned int address);
#define HYNIX_LSB_EXIT(name) \
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
	nand_print_dbg(#name "\n"); \
	nand_disable_chip(chip); \
	return 0; \
}

HYNIX_LSB_EXIT(hynix20nm_lsb_exit)
HYNIX_LSB_EXIT(hynix26nm_lsb_exit)

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
        nand_print_dbg("[ND]free_list error !\n");
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
	nand_print_dbg("samsung read retry conut: %d !\n",
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
	nand_print_dbg("toshiba read retry conut: %d !\n",
		       toshiba_read_retry_cycle);
	return 0;
}

extern int micron_readretry_init(void *);
extern int hynix20nm_readretry_init(void *);
extern int hynix26nm_readretry_init(void *);
extern void hynix16nm_read_page_end(void);
extern void hynix20nm_read_page_end(void);
extern void hynix26nm_read_page_end(void);
extern void sandisk_read_page_end(void);
extern void sandisk_A19_read_page_end(void);
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
#define SPECIAL_INIT_TWO(name, init, end, mode, cycle, success, failure) \
int name(void) \
{ \
	int result = 0; \
	struct special_exit_chip *chip = g_nsi->chips; \
	while (chip) { \
		result |= init((void *)chip); \
		chip = chip->next; \
	} \
	if (result == 0) { \
		df_read_page_end = (void (*)(void))end; \
		nand_print_dbg(success, mode, cycle); \
	} else { \
		nand_print(failure, mode, cycle); \
	} \
	return result; \
}

#define SPECIAL_INIT_THREE(name, init, end, mode, cycle, reg, success, failure) \
int name(void) \
{ \
	int result = 0; \
	struct special_exit_chip *chip = g_nsi->chips; \
	while (chip) { \
		result |= init((void *)chip); \
		chip = chip->next; \
	} \
	if (result == 0) { \
		df_read_page_end = (void (*)(void))end; \
		nand_print_dbg(success, mode, cycle, reg); \
	} else { \
		nand_print(failure, mode, cycle, reg); \
	} \
	return result; \
}

SPECIAL_INIT_TWO(samsung_special_init, samsung_readretry_init,
	samsung_read_page_end, samsung_read_retry_mode, samsung_read_retry_cycle,
	" samsung_special_init samsung_read_retry_mode:%d samsung_read_retry_cycle :%d \n",
	" samsung_special_init error samsung_read_retry_mode:%d samsung_read_retry_cycle :%d \n")
SPECIAL_INIT_TWO(micron_special_init, micron_readretry_init,
	micron_read_page_end, micron_read_retry_mode, micron_read_retry_cycle,
	" micron_special_init micron_read_retry_mode:%d micron_read_retry_cycle:%d \n",
	" micron_special_init error micron_read_retry_mode:%d micron_read_retry_cycle:%d \n")
SPECIAL_INIT_THREE(toshiba_special_init, toshiba_readretry_init,
	toshiba_read_page_end, toshiba_read_retry_mode, toshiba_read_retry_cycle,
	toshiba_read_retry_reg_cnt,
	" toshiba_special_init toshiba_read_retry_mode :%d toshiba_read_retry_cycle :%d toshiba_read_retry_reg_cnt :%d \n",
	" toshiba_special_init error toshiba_read_retry_mode :%d toshiba_read_retry_cycle :%d toshiba_read_retry_reg_cnt :%d \n")
SPECIAL_INIT_THREE(hynix16nm_special_init, hynix16nm_readretry_init,
	hynix16nm_read_page_end, hynix16nm_read_retry_mode,
	hynix16nm_read_retry_cycle, hynix16nm_read_retry_reg_num,
	" hynix16nm_special_init hynix16nm_read_retry_mode: %d hynix16nm_read_retry_cycle:%d hynix16nm_read_retry_reg_num:%d\n",
	" hynix16nm_special_init error hynix16nm_read_retry_mode: %d hynix16nm_read_retry_cycle:%d hynix16nm_read_retry_reg_num:%d\n")
SPECIAL_INIT_THREE(hynix20nm_special_init, hynix20nm_readretry_init,
	hynix20nm_read_page_end, hynix20nm_read_retry_mode,
	hynix20nm_read_retry_cycle, hynix20nm_read_retry_reg_num,
	" hynix20nm_special_init hynix20nm_read_retry_mode: %d hynix20nm_read_retry_cycle:%d hynix20nm_read_retry_reg_num:%d\n",
	" hynix20nm_special_init error hynix20nm_read_retry_mode: %d hynix20nm_read_retry_cycle:%d hynix20nm_read_retry_reg_num:%d\n")
SPECIAL_INIT_THREE(hynix26nm_special_init, hynix26nm_readretry_init,
	hynix26nm_read_page_end, hynix26nm_read_retry_mode,
	hynix26nm_read_retry_cycle, hynix26nm_read_retry_reg_num,
	" hynix26nm_special_init hynix26nm_read_retry_mode: %d hynix26nm_read_retry_cycle:%d hynix26nm_read_retry_reg_num:%d\n",
	" hynix26nm_special_init error hynix26nm_read_retry_mode: %d hynix26nm_read_retry_cycle:%d hynix26nm_read_retry_reg_num:%d\n")
SPECIAL_INIT_THREE(sandisk_special_init, sandisk_readretry_init,
	sandisk_read_page_end, sandisk_read_retry_mode,
	sandisk_read_retry_cycle, sandisk_read_retry_reg_cnt,
	" sandisk_special_init sandisk_read_retry_mode:%d sandisk_read_retry_cycle :%d sandisk_read_retry_reg_cnt %d \n",
	" sandisk_special_init error sandisk_read_retry_mode:%d sandisk_read_retry_cycle :%d sandisk_read_retry_reg_cnt %d \n")
SPECIAL_INIT_THREE(sandisk_A19_special_init, sandisk_A19_readretry_init,
	sandisk_A19_read_page_end, sandisk_A19_read_retry_mode,
	sandisk_A19_read_retry_cycle, sandisk_A19_read_retry_reg_cnt,
	" sandisk_A19_special_init sandisk_A19_read_retry_mode:%d sandisk_A19_read_retry_cycle :%d sandisk_A19_read_retry_reg_cnt %d \n",
	" sandisk_A19_special_init error sandisk_A19_read_retry_mode:%d sandisk_A19_read_retry_cycle :%d sandisk_A19_read_retry_reg_cnt %d \n")

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

extern unsigned int toshiba_toggle_mode_flag_recovered;
extern unsigned char toshiba_p1_1[], toshiba_p1_2[], toshiba_p1_3[];
extern unsigned char toshiba_read_retry_addr_1[], toshiba_read_retry_addr_2[];
extern int toshiba_vender_set_param(struct samsung_set_chip *,
				   unsigned char *, unsigned char *, unsigned int);
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
		 * changes the node between them. Neither pointer is checked. */
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
int crc_wrong(struct crc_page_zone *zone, const unsigned char *data,
	      unsigned char *oob)
{
	unsigned short actual = 0;
	unsigned short stored = 0;
	unsigned int length = 0;

	length = zone->geometry->page_size_factor << 9;
	if (is_nouse_page(oob))
		return 0;
	if (is_phy_mapping_page(oob))
		length = zone->map_length;
	actual = nand_crc16(-1, data, length);
	stored = (short)((short)oob[13] | (short)(oob[12] << 8));
	if (actual != stored) {
		nand_print("[NE]exception case wb %04x %04x, len: %d\n",
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
		nand_print("[NE]exception case bad block\n");
		error = 1;
	} else if (zone->geometry->crc_enabled) {
		if (crc_wrong((struct crc_page_zone *)zone, data, oob)) {
			nand_print("[NE]exception case wbc\n");
			error = 1;
		}
	} else if (oob[11] != 0xa5 || oob[15] != 0xa5) {
		nand_print("[NE]exception case gd\n");
		error = 1;
	}
	if (error == 1) {
		/* OEM prints its original source line and dumps 16 bytes of OOB;
		 * preserve that diagnostic even if the source line moves. */
		nand_print("[NE]%s %d, flag = %d\n", __func__, 3046, error);
		nand_print("[NE]OOB :");
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
		nand_print("[NE]%s:logic_page:0x%x >= total_pages:0x%x\n",
			   __func__, page, zone->total_pages);
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
int print_logic_page_map(struct logic_page_map_zone *zone, unsigned int ignored)
{
	unsigned int total = zone->summary / zone->geometry->divisor;
	unsigned int index = 0;
	struct logic_page_print_entry *entry;

	/* OEM divides by the unchecked geometry byte; AArch64 UDIV produces
	 * zero when it is zero. It also walks unchecked map pointers. */
	while (index < total) {
		entry = (struct logic_page_print_entry *)
			get_logic_page_map(zone, index);
		if (!entry)
			nand_print("[NI]page_map NULL!!\n");
		else if (entry->marker != 0xffff)
			nand_print_dbg("[ND]%4d:<%4d %3d><%d>\n", index,
				       entry->marker, entry->code & 0xfff,
				       (((unsigned char *)entry)[1] >> 4) & 1);
		index++;
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
int get_cold_logic_page(void *zone, int page, unsigned int pages)
{
	unsigned int current = (unsigned int)page;
	struct cold_map_entry *entry;

	while (current < pages) {
		entry = (struct cold_map_entry *)get_logic_page_map(
			(struct logic_page_map_zone *)zone, current);
		if (!entry) {
			nand_print("[NI]page_map NULL!!\n");
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
extern int nand_cache_read(void *zone, int page, unsigned short mode,
			   void *buffer);
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
		nand_cache_read(selected->zone, cold_page,
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
extern int write_phy_data_to_current_block(struct page_map_write_zone *,
					   struct page_map_write_current *,
					   void *, unsigned int, unsigned int);
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
				nand_print_dbg("[ND]logic_page_map_temp NULL!!\n");
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
		nand_print("[NE]invaild_page num error:[%d] [%d]  used_page:[%d]!!\n",
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
	result = write_phy_data_to_current_block(zone, current, current->data,
					  0, 0xaaaaffff);
	if (result) {
		nand_print("[NE]write_phy_data_to_current_block error1\n");
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
int do_after_write(struct do_after_zone *zone)
{
	if (current_block_at_last_page((struct current_zone *)zone,
				       (struct current_page *)zone->current) == 1) {
		if (write_phy_page_map_to_current_block(
				(struct page_map_write_zone *)zone,
				(struct page_map_write_current *)zone->current)) {
			nand_print("[NE]write_phy_page_map_to_current_block error!\n");
			return 1;
		}
		if (zone->flags & 1) {
			/* OEM logs the second map-write error and continues. */
			if (write_phy_page_map_to_current_block(
					(struct page_map_write_zone *)zone,
					(struct page_map_write_current *)zone->page_map))
				nand_print("[NE]write_phy_page_map_to_current_block error!\n");
		}
		if (get_new_current_block((struct get_new_zone *)zone,
				(struct get_new_current_entry *)zone->current)) {
			nand_print("[NE]get_new_current_block error1!\n");
			return 1;
		}
		if (zone->flags & 1) {
			if (get_new_current_block((struct get_new_zone *)zone,
				    (struct get_new_current_entry *)zone->page_map)) {
				nand_print("[NE]get_new_current_block error2!\n");
				return 1;
			}
		}
		zone->completed++;
	}
	return 0;
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
			nand_print_dbg("[ND]swl: new_cur_blk->alloc ok(max_ec): block_no = %d; ec = %d; \n",
				       current->block->number,
				       current->block->erase_count);
	} else {
		current->block = (struct get_new_block_info *)
			out_phy_block_from_free_list((struct free_block_zone *)zone);
	}
	if (!current->block) {
		nand_print("[NE]no free block!!!!\n");
		return 1;
	}
	current->page = 0;
	current->block->sequence = previous + 1;
	if (zone->flags & 1)
		current->block->sequence++;
	return 0;
}
