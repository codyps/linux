/* SPDX-License-Identifier: GPL-2.0 */
/* Recovered OEM NFTL initializer; compile with the OEM GCC 5.3.1. */
#include "nftl_ro_aliases.h"
#include "nftl_initialize_types.h"

extern void nand_print(const char *, ...);
extern void nand_print_dbg(const char *, ...);
extern int nand_memcmp(const void *, const void *, unsigned long);
extern void *nftl_malloc(unsigned int);
extern unsigned int nftl_get_zone_cap(struct nftl_zone_cap_view *);
extern int nftl_support_boot_check_crc_from_dts(void);
extern const char nftl_vendor_allwinner[9];
extern unsigned char phyinfo[20];
extern char com[];
extern char *nand_fdt_get_prop_string_by_path(void *, const char *, const char *);
extern void nftl_get_nand_phyinfo(void *);
extern int nftl_start(struct init_nftl_owner *, struct init_nftl_config *,
                      struct init_nftl_zone **, void *, void *, unsigned int,
                      unsigned int, unsigned int, struct init_partition *,
                      unsigned int, unsigned char);
extern void _nand_read(void), _nand_write(void), _nand_discard(void);
extern void _nand_flush_write_cache(void), _nand_flush_sector_write_cache(void);
extern void _nand_sector_read(void), _nand_sector_write(void);
extern void _shutdown_op(void);

static void nftl_version(void)
{
	nand_print(nftl_ro_lc_15,
		   nftl_ro_lc_14, nftl_ro_lc_13, nftl_ro_lc_12, nftl_ro_lc_11, nftl_ro_lc_10);
}

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
	owner->config->limit_a = part->device->params->limit_a;
	owner->config->limit_b = part->device->params->limit_b;
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
		owner->config->erase_limit = (unsigned short)(
			part->device->params->retry_setting * 50);
		owner->config->interval = (unsigned short)(
			part->device->params->retry_setting * 40);
	}
	owner->config->device_mode = part->device->mode;
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
	    part->device->params->format_revision > 0x1517f)
		owner->config->revision = part->device->params->format_revision;
	owner->config->boot_crc = 1;
	if (!nftl_support_boot_check_crc_from_dts())
		owner->config->boot_crc = 0;
	result = nftl_start(owner, owner->config, &owner->zone, part->read, part->write,
			    part->capacity, part->small, part->pages,
			    part, number, 0);
	if (result)
		return result;
	owner->zone->geometry->max_erase_times =
		part->device->params->max_erase_times;
	owner->zone->geometry->secondary_erase_times =
		part->device->params->erased_block_limit;
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
