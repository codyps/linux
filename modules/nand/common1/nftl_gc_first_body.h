/* SPDX-License-Identifier: GPL-2.0 */
/* Shared OEM first-GC implementation: included once per selected pass. */
__attribute__((optimize("-fno-stack-protector")))
int garbage_collect_first(struct gc_first_zone *zone, void *requested,
			  unsigned short limit)
{
	struct invalid_page_location *entry;
	unsigned int page, logic, observed = 0;
	unsigned char oob[16];
	struct physic_op_param op;
	unsigned int retry;
	int status, bad = 0;
	struct gc_first_block *block;
	/* OEM keeps a zero-valued diagnostic field in its stack frame. */
	unsigned int diagnostic_zero = 0;

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
			       block->number, zone->geometry->pages - 1,
			       diagnostic_zero,
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
	/* OEM tests for exactly one here and again before erase. Keep the
	 * narrower comparison, even though this function assigns only 0 or 1. */
	if (bad == 1)
		limit = 0xffff;
	for (page = 0; page < zone->geometry->pages - 1;
	     page++) {
		logic = zone->map.pages[page];
		if (logic == ~0U)
			continue;
		entry = (struct invalid_page_location *)
			get_logic_page_map((struct logic_page_map_zone *)zone, logic);
		if (!entry)
			continue;
		if (entry->block != block->number)
			continue;
		if (entry->page != page)
			continue;
		zone->stats->copied_pages++;
		set_physic_op_par(&op, block->number, page,
				  zone->geometry->fixed_page,
				  zone->logic_buffer, oob);
		/* OEM tests an unsigned retry counter against one, but breaks
		 * unconditionally after its first read. Preserve this broken retry
		 * loop: a read error never causes another physical read. */
		for (retry = 0; retry < 2; retry++) {
			status = zone->read(zone, &op);
			break;
		}
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
			} else if ((status = nand_write_logic_page_no_gc(zone, logic,
						      zone->logic_buffer))) {
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
	if (page == zone->geometry->pages - 1)
		zone->active = 0;
	if (bad == 1 && page == zone->geometry->pages - 1) {
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
