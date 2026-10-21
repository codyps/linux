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
