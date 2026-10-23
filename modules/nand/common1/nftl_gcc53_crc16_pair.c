/* SPDX-License-Identifier: GPL-2.0 */
/* OEM GCC 5.3.1 generates both stock CRC16 bodies from this C pair.
 * Keep them in one section to preserve the local helper's call and binding. */
extern const unsigned short nand_crc16_table[256];

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
