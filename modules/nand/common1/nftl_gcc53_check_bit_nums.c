/* SPDX-License-Identifier: GPL-2.0 */
/* OEM GCC 5.3.1 emits the original 164-byte per-byte bit counter. */
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
	/* OEM skips the loop for negative length and does not check the
	 * positive length against the actual buffer extent. */
	return total;
}
