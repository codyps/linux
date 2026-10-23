/* SPDX-License-Identifier: GPL-2.0 */
/* Included at the OEM declaration point during the GCC 5.3.1 pass. */

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
