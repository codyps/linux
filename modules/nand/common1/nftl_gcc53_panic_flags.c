/* SPDX-License-Identifier: GPL-2.0 */
/* GCC 5.3.1 preserves the OEM byte truncation instructions in these getters. */
extern unsigned int nand_on_panic;
extern unsigned int panic_enable;

unsigned char is_on_panic(void)
{
	/* OEM reads the shared flag without synchronization or a null guard. */
	return nand_on_panic != 0;
}

int is_panic_enable(void)
{
	/* Preserve the OEM's unchecked shared BSS flag and byte result. */
	return panic_enable != 0;
}
