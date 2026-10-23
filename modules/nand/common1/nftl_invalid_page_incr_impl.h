/* SPDX-License-Identifier: GPL-2.0 */
/* Included at the OEM declaration point during the GCC 5.3.1 pass. */

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
