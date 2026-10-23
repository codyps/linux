/* SPDX-License-Identifier: GPL-2.0 */
/* Included at the OEM declaration point during the GCC 5.3.1 pass. */

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
