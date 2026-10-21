/* SPDX-License-Identifier: GPL-2.0 */
/* TG3040 #860 initialized NFTL tables recovered from the finished Image.
 * Byte array field types are conservative: original C element types are not
 * known. The offsets and payloads are independently checked against stock.
 *
 * OEM retry indexes are unchecked and may consume neighboring bytes. Keep
 * all fields and gaps contiguous; fix bounds in a separate behavior change.
 */

struct nftl_initialized_layout {
	unsigned char patterns[1];
	unsigned char gap_0[7];
	unsigned char samsung_read_retry_addr[4];
	unsigned char gap_1[4];
	unsigned char samsung_read_retry_cmd[4];
	unsigned char gap_2[4];
	unsigned char samsung_p1[60];
	unsigned char gap_3[4];
	unsigned char sandisk_A19_read_retry_enable_cmd_1y[1];
	unsigned char gap_4[7];
	unsigned char sandisk_A19_read_retry_exit_cmd_1y[1];
	unsigned char gap_5[7];
	unsigned char sandisk_A19_read_retry_cmd_1y[1];
	unsigned char gap_6[7];
	unsigned char sandisk_A19_read_retry_reg_adr_1y[1];
	unsigned char gap_7[7];
	unsigned char sandisk_A19_1y[124];
	unsigned char gap_8[4];
	unsigned char sandisk_A19_1z_16g[128];
	unsigned char sandisk_A19_1z_8g[124];
	unsigned char gap_9[4];
	unsigned char toshiba_read_retry_start_cmd[2];
	unsigned char gap_10[6];
	unsigned char toshiba_read_retry_end_cmd[2];
	unsigned char gap_11[6];
	unsigned char toshiba_read_retry_cmd_1[4];
	unsigned char gap_12[4];
	unsigned char toshiba_read_retry_addr_1[4];
	unsigned char gap_13[4];
	unsigned char toshiba_p1_1[28];
	unsigned char gap_14[4];
	unsigned char toshiba_read_retry_cmd_2[5];
	unsigned char gap_15[3];
	unsigned char toshiba_read_retry_addr_2[5];
	unsigned char gap_16[3];
	unsigned char toshiba_p1_2[40];
	unsigned char toshiba_p1_3[55];
	unsigned char gap_17[1];
	unsigned char toshiba_read_retry_last_cmd[1];
	unsigned char gap_18[7];
	unsigned char toshiba_read_retry_exit_cmd[1];
	unsigned char gap_19[7];
	unsigned char micron_p1_1[10];
	unsigned char gap_20[6];
	unsigned char micron_p1_2[12];
	unsigned char gap_21[4];
	unsigned char micron_p1_3[7];
	unsigned char gap_22[1];
	unsigned char micron_p1_4[18];
	unsigned char gap_23[6];
	unsigned char sandisk_acti_start_cmd[2];
	unsigned char gap_24[6];
	unsigned char sandisk_acti_start_addr[9];
	unsigned char gap_25[7];
	unsigned char sandisk_read_retry_enable_cmd_19[1];
	unsigned char gap_26[7];
	unsigned char sandisk_read_retry_disable_cmd_19[1];
	unsigned char gap_27[7];
	unsigned char sandisk_read_retry_cmd_19[3];
	unsigned char gap_28[5];
	unsigned char sandisk_read_retry_reg_adr_19[9];
	unsigned char gap_29[7];
	unsigned char sandisk_19_low[48];
	unsigned char sandisk_19_high[60];
	unsigned char gap_30[4];
	unsigned char sandisk_read_retry_cmd_24[3];
	unsigned char gap_31[5];
	unsigned char sandisk_read_retry_reg_adr_24[9];
	unsigned char gap_32[7];
	unsigned char sandisk_24[27];
	unsigned char gap_33[5];
	unsigned char sandisk_read_retry_enable_cmd_1y[1];
	unsigned char gap_34[7];
	unsigned char sandisk_read_retry_exit_cmd_1y[1];
	unsigned char gap_35[7];
	unsigned char sandisk_read_retry_cmd_1y[1];
	unsigned char gap_36[7];
	unsigned char sandisk_read_retry_reg_adr_1y[1];
	unsigned char gap_37[7];
	unsigned char sandisk_1y[76];
	unsigned char gap_38[4];
	unsigned char hynix20nm_read_retry_reg_adr_1[8];
	unsigned char hynix20nm_read_retry_reg_adr_2[8];
};

static struct nftl_initialized_layout nftl_initialized_storage
	__asm__(".Lnftl_initialized_storage")
	__attribute__((used, aligned(8))) = {
	.patterns = {
		0xa5,
	},
	.samsung_read_retry_addr = {
		0xa7, 0xa4, 0xa5, 0xa6,
	},
	.samsung_read_retry_cmd = {
		0xa1, 0xa1, 0xa1, 0xa1,
	},
	.samsung_p1 = {
		0x00, 0x00, 0x00, 0x00, 0x05, 0x0a, 0x00, 0x00, 0x28, 0x00, 0xec, 0xd8,
		0xed, 0xf5, 0xed, 0xe6, 0x0a, 0x0f, 0x05, 0x00, 0x0f, 0x0a, 0xfb, 0xec,
		0xe8, 0xef, 0xe8, 0xdc, 0xf1, 0xfb, 0xfe, 0xf0, 0x0a, 0x00, 0xfb, 0xec,
		0xd0, 0xe2, 0xd0, 0xc2, 0x14, 0x0f, 0xfb, 0xec, 0xe8, 0xfb, 0xe8, 0xdc,
		0x1e, 0x14, 0xfb, 0xec, 0xfb, 0xff, 0xfb, 0xf8, 0x07, 0x0c, 0x02, 0x00,
	},
	.sandisk_A19_read_retry_enable_cmd_1y = {
		0x5d,
	},
	.sandisk_A19_read_retry_exit_cmd_1y = {
		0xff,
	},
	.sandisk_A19_read_retry_cmd_1y = {
		0xef,
	},
	.sandisk_A19_read_retry_reg_adr_1y = {
		0x11,
	},
	.sandisk_A19_1y = {
		0x7c, 0x00, 0x00, 0x7c, 0x04, 0x00, 0x7c, 0x78, 0x78, 0x00, 0x78, 0x74,
		0x08, 0x7c, 0x00, 0x7c, 0x00, 0x7c, 0x7c, 0x78, 0x7c, 0x7c, 0x78, 0x74,
		0x00, 0x7c, 0x74, 0x70, 0x00, 0x78, 0x00, 0x7c, 0x00, 0x78, 0x7c, 0x78,
		0x00, 0x78, 0x78, 0x74, 0x00, 0x78, 0x74, 0x70, 0x00, 0x78, 0x70, 0x6c,
		0x00, 0x04, 0x04, 0x00, 0x00, 0x04, 0x00, 0x7c, 0x0c, 0x04, 0x7c, 0x78,
		0x0c, 0x04, 0x78, 0x74, 0x10, 0x08, 0x00, 0x7c, 0x10, 0x08, 0x04, 0x00,
		0x78, 0x74, 0x78, 0x74, 0x78, 0x74, 0x74, 0x70, 0x78, 0x74, 0x70, 0x6c,
		0x78, 0x74, 0x6c, 0x68, 0x78, 0x70, 0x78, 0x74, 0x78, 0x70, 0x74, 0x70,
		0x78, 0x70, 0x6c, 0x68, 0x78, 0x70, 0x70, 0x6c, 0x78, 0x6c, 0x70, 0x6c,
		0x78, 0x6c, 0x6c, 0x68, 0x78, 0x6c, 0x68, 0x64, 0x74, 0x68, 0x6c, 0x68,
		0x74, 0x68, 0x68, 0x64,
	},
	.sandisk_A19_1z_16g = {
		0x7c, 0x00, 0x04, 0x00, 0x04, 0x00, 0x00, 0x7c, 0x78, 0x00, 0x08, 0x04,
		0x08, 0x00, 0x7c, 0x78, 0x7c, 0x7c, 0x04, 0x00, 0x00, 0x7c, 0x00, 0x7c,
		0x78, 0x7c, 0x08, 0x04, 0x04, 0x7c, 0x74, 0x78, 0x04, 0x04, 0x04, 0x00,
		0x08, 0x04, 0x00, 0x7c, 0x00, 0x04, 0x08, 0x04, 0x0c, 0x04, 0x7c, 0x78,
		0x7c, 0x04, 0x0c, 0x08, 0x78, 0x78, 0x04, 0x00, 0x7c, 0x78, 0x00, 0x7c,
		0x74, 0x78, 0x70, 0x78, 0x08, 0x08, 0x04, 0x00, 0x04, 0x08, 0x08, 0x04,
		0x0c, 0x08, 0x0c, 0x08, 0x00, 0x00, 0x0c, 0x08, 0x04, 0x00, 0x78, 0x74,
		0x7c, 0x7c, 0x78, 0x74, 0x78, 0x78, 0x78, 0x74, 0x08, 0x08, 0x00, 0x7c,
		0x74, 0x74, 0x00, 0x7c, 0x78, 0x74, 0x7c, 0x78, 0x70, 0x74, 0x74, 0x74,
		0x7c, 0x74, 0x70, 0x70, 0x0c, 0x0c, 0x08, 0x04, 0x10, 0x0c, 0x0c, 0x08,
		0x70, 0x70, 0x70, 0x74, 0x74, 0x70, 0x70, 0x70,
	},
	.sandisk_A19_1z_8g = {
		0x04, 0x04, 0x78, 0x78, 0x04, 0x04, 0x7c, 0x74, 0x00, 0x00, 0x04, 0x78,
		0x04, 0x04, 0x7c, 0x7c, 0x00, 0x00, 0x00, 0x7c, 0x00, 0x00, 0x00, 0x74,
		0x08, 0x08, 0x04, 0x78, 0x7c, 0x7c, 0x04, 0x78, 0x7c, 0x7c, 0x00, 0x7c,
		0x04, 0x04, 0x7c, 0x70, 0x7c, 0x7c, 0x74, 0x74, 0x00, 0x00, 0x78, 0x70,
		0x0c, 0x0c, 0x08, 0x78, 0x78, 0x78, 0x7c, 0x7c, 0x04, 0x04, 0x08, 0x04,
		0x78, 0x78, 0x08, 0x78, 0x7c, 0x7c, 0x78, 0x70, 0x78, 0x78, 0x70, 0x6c,
		0x00, 0x00, 0x74, 0x6c, 0x08, 0x08, 0x00, 0x74, 0x7c, 0x7c, 0x78, 0x6c,
		0x00, 0x00, 0x04, 0x04, 0x74, 0x74, 0x74, 0x6c, 0x78, 0x78, 0x7c, 0x70,
		0x0c, 0x0c, 0x00, 0x74, 0x04, 0x04, 0x0c, 0x08, 0x78, 0x78, 0x7c, 0x74,
		0x78, 0x78, 0x70, 0x68, 0x08, 0x08, 0x00, 0x70, 0x10, 0x10, 0x0c, 0x78,
		0x00, 0x00, 0x0c, 0x08,
	},
	.toshiba_read_retry_start_cmd = {
		0x5c, 0xc5,
	},
	.toshiba_read_retry_end_cmd = {
		0x26, 0x5d,
	},
	.toshiba_read_retry_cmd_1 = {
		0x55, 0x55, 0x55, 0x55,
	},
	.toshiba_read_retry_addr_1 = {
		0x04, 0x05, 0x06, 0x07,
	},
	.toshiba_p1_1 = {
		0x00, 0x00, 0x00, 0x00, 0x04, 0x04, 0x04, 0x04, 0x7c, 0x7c, 0x7c, 0x7c,
		0x78, 0x78, 0x78, 0x78, 0x74, 0x74, 0x74, 0x74, 0x08, 0x08, 0x08, 0x08,
		0xff, 0xff, 0xff, 0xff,
	},
	.toshiba_read_retry_cmd_2 = {
		0x55, 0x55, 0x55, 0x55, 0x55,
	},
	.toshiba_read_retry_addr_2 = {
		0x04, 0x05, 0x06, 0x07, 0x0d,
	},
	.toshiba_p1_2 = {
		0x04, 0x04, 0x7c, 0x7e, 0x00, 0x00, 0x7c, 0x78, 0x78, 0x00, 0x7c, 0x76,
		0x74, 0x72, 0x00, 0x08, 0x08, 0x00, 0x00, 0x00, 0x0b, 0x7e, 0x76, 0x74,
		0x00, 0x10, 0x76, 0x72, 0x70, 0x00, 0x02, 0x00, 0x7e, 0x7c, 0x00, 0x00,
		0x00, 0x00, 0x00, 0x00,
	},
	.toshiba_p1_3 = {
		0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x04, 0x02, 0x00, 0x00, 0x7c, 0x00,
		0x7c, 0x7c, 0x00, 0x7a, 0x00, 0x7a, 0x7a, 0x00, 0x78, 0x02, 0x78, 0x7a,
		0x00, 0x7e, 0x04, 0x7e, 0x7a, 0x00, 0x76, 0x04, 0x76, 0x78, 0x00, 0x04,
		0x04, 0x04, 0x76, 0x00, 0x06, 0x0a, 0x06, 0x02, 0x00, 0x74, 0x7c, 0x74,
		0x76, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	},
	.toshiba_read_retry_last_cmd = {
		0xb3,
	},
	.toshiba_read_retry_exit_cmd = {
		0xff,
	},
	.micron_p1_1 = {
		0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,
	},
	.micron_p1_2 = {
		0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x0c,
	},
	.micron_p1_3 = {
		0x01, 0x02, 0x03, 0x00, 0x01, 0x02, 0x03,
	},
	.micron_p1_4 = {
		0x00, 0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09,
		0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
	},
	.sandisk_acti_start_cmd = {
		0x3b, 0xb9,
	},
	.sandisk_acti_start_addr = {
		0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c,
	},
	.sandisk_read_retry_enable_cmd_19 = {
		0xb6,
	},
	.sandisk_read_retry_disable_cmd_19 = {
		0xd6,
	},
	.sandisk_read_retry_cmd_19 = {
		0x53, 0x53, 0x53,
	},
	.sandisk_read_retry_reg_adr_19 = {
		0x04, 0x05, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	},
	.sandisk_19_low = {
		0xf0, 0x00, 0xf0, 0xe0, 0x00, 0xe0, 0xd0, 0x00, 0xd0, 0x10, 0x00, 0x10,
		0x20, 0x00, 0x20, 0x30, 0x00, 0x30, 0xc0, 0x00, 0xd0, 0x00, 0x00, 0x10,
		0x00, 0x00, 0x20, 0x10, 0x00, 0x20, 0xb0, 0x00, 0xd0, 0xa0, 0x00, 0xd0,
		0x90, 0x00, 0xd0, 0xb0, 0x00, 0xc0, 0xa0, 0x00, 0xc0, 0x90, 0x00, 0xc0,
	},
	.sandisk_19_high = {
		0x00, 0xf0, 0x00, 0x0f, 0xe0, 0x00, 0x0f, 0xd0, 0x00, 0x0e, 0xe0, 0x00,
		0x0e, 0xd0, 0x00, 0x0d, 0xf0, 0x00, 0x0d, 0xe0, 0x00, 0x0d, 0xd0, 0x00,
		0x01, 0x10, 0x00, 0x02, 0x20, 0x00, 0x02, 0x10, 0x00, 0x03, 0x20, 0x00,
		0x0f, 0x00, 0x00, 0x0e, 0xf0, 0x00, 0x0d, 0xc0, 0x00, 0x0f, 0xf0, 0x00,
		0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x0d, 0xb0, 0x00, 0x0c, 0xa0, 0x00,
	},
	.sandisk_read_retry_cmd_24 = {
		0x53, 0x53, 0x53,
	},
	.sandisk_read_retry_reg_adr_24 = {
		0x04, 0x05, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	},
	.sandisk_24 = {
		0x00, 0xf0, 0x00, 0x00, 0xe0, 0x00, 0xff, 0xf0, 0xf0, 0xee, 0xe0, 0xe0,
		0xde, 0xd0, 0xd0, 0xcd, 0xc0, 0xc0, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00,
		0x03, 0x00, 0x00,
	},
	.sandisk_read_retry_enable_cmd_1y = {
		0x5d,
	},
	.sandisk_read_retry_exit_cmd_1y = {
		0xff,
	},
	.sandisk_read_retry_cmd_1y = {
		0xef,
	},
	.sandisk_read_retry_reg_adr_1y = {
		0x11,
	},
	.sandisk_1y = {
		0x04, 0x00, 0x7c, 0x7c, 0x08, 0x00, 0x04, 0x04, 0x7c, 0x7c, 0x00, 0x7c,
		0x7c, 0x7c, 0x00, 0x00, 0x0c, 0x00, 0x78, 0x78, 0x10, 0x00, 0x08, 0x08,
		0x7c, 0x7c, 0x78, 0x78, 0x7c, 0x7c, 0x04, 0x04, 0x7c, 0x7c, 0x78, 0x74,
		0x14, 0x04, 0x04, 0x00, 0x78, 0x78, 0x00, 0x7c, 0x78, 0x78, 0x78, 0x78,
		0x78, 0x78, 0x78, 0x74, 0x78, 0x78, 0x04, 0x00, 0x78, 0x74, 0x78, 0x74,
		0x78, 0x74, 0x74, 0x70, 0x78, 0x74, 0x78, 0x78, 0x78, 0x70, 0x78, 0x74,
		0x78, 0x70, 0x70, 0x70,
	},
	.hynix20nm_read_retry_reg_adr_1 = {
		0xcc, 0xbf, 0xaa, 0xab, 0xcd, 0xad, 0xae, 0xaf,
	},
	.hynix20nm_read_retry_reg_adr_2 = {
		0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7,
	},
};

_Static_assert(sizeof(struct nftl_initialized_layout) == 1120,
	"NFTL initialized span changed");

_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, patterns) == 0,
	"NFTL patterns offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, samsung_read_retry_addr) == 8,
	"NFTL samsung_read_retry_addr offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, samsung_read_retry_cmd) == 16,
	"NFTL samsung_read_retry_cmd offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, samsung_p1) == 24,
	"NFTL samsung_p1 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_A19_read_retry_enable_cmd_1y) == 88,
	"NFTL sandisk_A19_read_retry_enable_cmd_1y offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_A19_read_retry_exit_cmd_1y) == 96,
	"NFTL sandisk_A19_read_retry_exit_cmd_1y offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_A19_read_retry_cmd_1y) == 104,
	"NFTL sandisk_A19_read_retry_cmd_1y offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_A19_read_retry_reg_adr_1y) == 112,
	"NFTL sandisk_A19_read_retry_reg_adr_1y offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_A19_1y) == 120,
	"NFTL sandisk_A19_1y offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_A19_1z_16g) == 248,
	"NFTL sandisk_A19_1z_16g offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_A19_1z_8g) == 376,
	"NFTL sandisk_A19_1z_8g offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, toshiba_read_retry_start_cmd) == 504,
	"NFTL toshiba_read_retry_start_cmd offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, toshiba_read_retry_end_cmd) == 512,
	"NFTL toshiba_read_retry_end_cmd offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, toshiba_read_retry_cmd_1) == 520,
	"NFTL toshiba_read_retry_cmd_1 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, toshiba_read_retry_addr_1) == 528,
	"NFTL toshiba_read_retry_addr_1 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, toshiba_p1_1) == 536,
	"NFTL toshiba_p1_1 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, toshiba_read_retry_cmd_2) == 568,
	"NFTL toshiba_read_retry_cmd_2 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, toshiba_read_retry_addr_2) == 576,
	"NFTL toshiba_read_retry_addr_2 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, toshiba_p1_2) == 584,
	"NFTL toshiba_p1_2 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, toshiba_p1_3) == 624,
	"NFTL toshiba_p1_3 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, toshiba_read_retry_last_cmd) == 680,
	"NFTL toshiba_read_retry_last_cmd offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, toshiba_read_retry_exit_cmd) == 688,
	"NFTL toshiba_read_retry_exit_cmd offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, micron_p1_1) == 696,
	"NFTL micron_p1_1 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, micron_p1_2) == 712,
	"NFTL micron_p1_2 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, micron_p1_3) == 728,
	"NFTL micron_p1_3 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, micron_p1_4) == 736,
	"NFTL micron_p1_4 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_acti_start_cmd) == 760,
	"NFTL sandisk_acti_start_cmd offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_acti_start_addr) == 768,
	"NFTL sandisk_acti_start_addr offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_read_retry_enable_cmd_19) == 784,
	"NFTL sandisk_read_retry_enable_cmd_19 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_read_retry_disable_cmd_19) == 792,
	"NFTL sandisk_read_retry_disable_cmd_19 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_read_retry_cmd_19) == 800,
	"NFTL sandisk_read_retry_cmd_19 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_read_retry_reg_adr_19) == 808,
	"NFTL sandisk_read_retry_reg_adr_19 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_19_low) == 824,
	"NFTL sandisk_19_low offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_19_high) == 872,
	"NFTL sandisk_19_high offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_read_retry_cmd_24) == 936,
	"NFTL sandisk_read_retry_cmd_24 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_read_retry_reg_adr_24) == 944,
	"NFTL sandisk_read_retry_reg_adr_24 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_24) == 960,
	"NFTL sandisk_24 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_read_retry_enable_cmd_1y) == 992,
	"NFTL sandisk_read_retry_enable_cmd_1y offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_read_retry_exit_cmd_1y) == 1000,
	"NFTL sandisk_read_retry_exit_cmd_1y offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_read_retry_cmd_1y) == 1008,
	"NFTL sandisk_read_retry_cmd_1y offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_read_retry_reg_adr_1y) == 1016,
	"NFTL sandisk_read_retry_reg_adr_1y offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, sandisk_1y) == 1024,
	"NFTL sandisk_1y offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, hynix20nm_read_retry_reg_adr_1) == 1104,
	"NFTL hynix20nm_read_retry_reg_adr_1 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_initialized_layout, hynix20nm_read_retry_reg_adr_2) == 1112,
	"NFTL hynix20nm_read_retry_reg_adr_2 offset changed");

__asm__(".global patterns\n"
        ".set patterns, .Lnftl_initialized_storage+0\n"
        ".type patterns, %object\n"
        ".size patterns, 1");

__asm__(".global samsung_read_retry_addr\n"
        ".set samsung_read_retry_addr, .Lnftl_initialized_storage+8\n"
        ".type samsung_read_retry_addr, %object\n"
        ".size samsung_read_retry_addr, 4");

__asm__(".global samsung_read_retry_cmd\n"
        ".set samsung_read_retry_cmd, .Lnftl_initialized_storage+16\n"
        ".type samsung_read_retry_cmd, %object\n"
        ".size samsung_read_retry_cmd, 4");

__asm__(".global samsung_p1\n"
        ".set samsung_p1, .Lnftl_initialized_storage+24\n"
        ".type samsung_p1, %object\n"
        ".size samsung_p1, 60");

__asm__(".global sandisk_A19_read_retry_enable_cmd_1y\n"
        ".set sandisk_A19_read_retry_enable_cmd_1y, .Lnftl_initialized_storage+88\n"
        ".type sandisk_A19_read_retry_enable_cmd_1y, %object\n"
        ".size sandisk_A19_read_retry_enable_cmd_1y, 1");

__asm__(".global sandisk_A19_read_retry_exit_cmd_1y\n"
        ".set sandisk_A19_read_retry_exit_cmd_1y, .Lnftl_initialized_storage+96\n"
        ".type sandisk_A19_read_retry_exit_cmd_1y, %object\n"
        ".size sandisk_A19_read_retry_exit_cmd_1y, 1");

__asm__(".global sandisk_A19_read_retry_cmd_1y\n"
        ".set sandisk_A19_read_retry_cmd_1y, .Lnftl_initialized_storage+104\n"
        ".type sandisk_A19_read_retry_cmd_1y, %object\n"
        ".size sandisk_A19_read_retry_cmd_1y, 1");

__asm__(".global sandisk_A19_read_retry_reg_adr_1y\n"
        ".set sandisk_A19_read_retry_reg_adr_1y, .Lnftl_initialized_storage+112\n"
        ".type sandisk_A19_read_retry_reg_adr_1y, %object\n"
        ".size sandisk_A19_read_retry_reg_adr_1y, 1");

__asm__(".global sandisk_A19_1y\n"
        ".set sandisk_A19_1y, .Lnftl_initialized_storage+120\n"
        ".type sandisk_A19_1y, %object\n"
        ".size sandisk_A19_1y, 124");

__asm__(".global sandisk_A19_1z_16g\n"
        ".set sandisk_A19_1z_16g, .Lnftl_initialized_storage+248\n"
        ".type sandisk_A19_1z_16g, %object\n"
        ".size sandisk_A19_1z_16g, 128");

__asm__(".global sandisk_A19_1z_8g\n"
        ".set sandisk_A19_1z_8g, .Lnftl_initialized_storage+376\n"
        ".type sandisk_A19_1z_8g, %object\n"
        ".size sandisk_A19_1z_8g, 124");

__asm__(".global toshiba_read_retry_start_cmd\n"
        ".set toshiba_read_retry_start_cmd, .Lnftl_initialized_storage+504\n"
        ".type toshiba_read_retry_start_cmd, %object\n"
        ".size toshiba_read_retry_start_cmd, 2");

__asm__(".global toshiba_read_retry_end_cmd\n"
        ".set toshiba_read_retry_end_cmd, .Lnftl_initialized_storage+512\n"
        ".type toshiba_read_retry_end_cmd, %object\n"
        ".size toshiba_read_retry_end_cmd, 2");

__asm__(".global toshiba_read_retry_cmd_1\n"
        ".set toshiba_read_retry_cmd_1, .Lnftl_initialized_storage+520\n"
        ".type toshiba_read_retry_cmd_1, %object\n"
        ".size toshiba_read_retry_cmd_1, 4");

__asm__(".global toshiba_read_retry_addr_1\n"
        ".set toshiba_read_retry_addr_1, .Lnftl_initialized_storage+528\n"
        ".type toshiba_read_retry_addr_1, %object\n"
        ".size toshiba_read_retry_addr_1, 4");

__asm__(".global toshiba_p1_1\n"
        ".set toshiba_p1_1, .Lnftl_initialized_storage+536\n"
        ".type toshiba_p1_1, %object\n"
        ".size toshiba_p1_1, 28");

__asm__(".global toshiba_read_retry_cmd_2\n"
        ".set toshiba_read_retry_cmd_2, .Lnftl_initialized_storage+568\n"
        ".type toshiba_read_retry_cmd_2, %object\n"
        ".size toshiba_read_retry_cmd_2, 5");

__asm__(".global toshiba_read_retry_addr_2\n"
        ".set toshiba_read_retry_addr_2, .Lnftl_initialized_storage+576\n"
        ".type toshiba_read_retry_addr_2, %object\n"
        ".size toshiba_read_retry_addr_2, 5");

__asm__(".global toshiba_p1_2\n"
        ".set toshiba_p1_2, .Lnftl_initialized_storage+584\n"
        ".type toshiba_p1_2, %object\n"
        ".size toshiba_p1_2, 40");

__asm__(".global toshiba_p1_3\n"
        ".set toshiba_p1_3, .Lnftl_initialized_storage+624\n"
        ".type toshiba_p1_3, %object\n"
        ".size toshiba_p1_3, 55");

__asm__(".global toshiba_read_retry_last_cmd\n"
        ".set toshiba_read_retry_last_cmd, .Lnftl_initialized_storage+680\n"
        ".type toshiba_read_retry_last_cmd, %object\n"
        ".size toshiba_read_retry_last_cmd, 1");

__asm__(".global toshiba_read_retry_exit_cmd\n"
        ".set toshiba_read_retry_exit_cmd, .Lnftl_initialized_storage+688\n"
        ".type toshiba_read_retry_exit_cmd, %object\n"
        ".size toshiba_read_retry_exit_cmd, 1");

__asm__(".global micron_p1_1\n"
        ".set micron_p1_1, .Lnftl_initialized_storage+696\n"
        ".type micron_p1_1, %object\n"
        ".size micron_p1_1, 10");

__asm__(".global micron_p1_2\n"
        ".set micron_p1_2, .Lnftl_initialized_storage+712\n"
        ".type micron_p1_2, %object\n"
        ".size micron_p1_2, 12");

__asm__(".global micron_p1_3\n"
        ".set micron_p1_3, .Lnftl_initialized_storage+728\n"
        ".type micron_p1_3, %object\n"
        ".size micron_p1_3, 7");

__asm__(".global micron_p1_4\n"
        ".set micron_p1_4, .Lnftl_initialized_storage+736\n"
        ".type micron_p1_4, %object\n"
        ".size micron_p1_4, 18");

__asm__(".global sandisk_acti_start_cmd\n"
        ".set sandisk_acti_start_cmd, .Lnftl_initialized_storage+760\n"
        ".type sandisk_acti_start_cmd, %object\n"
        ".size sandisk_acti_start_cmd, 2");

__asm__(".global sandisk_acti_start_addr\n"
        ".set sandisk_acti_start_addr, .Lnftl_initialized_storage+768\n"
        ".type sandisk_acti_start_addr, %object\n"
        ".size sandisk_acti_start_addr, 9");

__asm__(".global sandisk_read_retry_enable_cmd_19\n"
        ".set sandisk_read_retry_enable_cmd_19, .Lnftl_initialized_storage+784\n"
        ".type sandisk_read_retry_enable_cmd_19, %object\n"
        ".size sandisk_read_retry_enable_cmd_19, 1");

__asm__(".global sandisk_read_retry_disable_cmd_19\n"
        ".set sandisk_read_retry_disable_cmd_19, .Lnftl_initialized_storage+792\n"
        ".type sandisk_read_retry_disable_cmd_19, %object\n"
        ".size sandisk_read_retry_disable_cmd_19, 1");

__asm__(".global sandisk_read_retry_cmd_19\n"
        ".set sandisk_read_retry_cmd_19, .Lnftl_initialized_storage+800\n"
        ".type sandisk_read_retry_cmd_19, %object\n"
        ".size sandisk_read_retry_cmd_19, 3");

__asm__(".global sandisk_read_retry_reg_adr_19\n"
        ".set sandisk_read_retry_reg_adr_19, .Lnftl_initialized_storage+808\n"
        ".type sandisk_read_retry_reg_adr_19, %object\n"
        ".size sandisk_read_retry_reg_adr_19, 9");

__asm__(".global sandisk_19_low\n"
        ".set sandisk_19_low, .Lnftl_initialized_storage+824\n"
        ".type sandisk_19_low, %object\n"
        ".size sandisk_19_low, 48");

__asm__(".global sandisk_19_high\n"
        ".set sandisk_19_high, .Lnftl_initialized_storage+872\n"
        ".type sandisk_19_high, %object\n"
        ".size sandisk_19_high, 60");

__asm__(".global sandisk_read_retry_cmd_24\n"
        ".set sandisk_read_retry_cmd_24, .Lnftl_initialized_storage+936\n"
        ".type sandisk_read_retry_cmd_24, %object\n"
        ".size sandisk_read_retry_cmd_24, 3");

__asm__(".global sandisk_read_retry_reg_adr_24\n"
        ".set sandisk_read_retry_reg_adr_24, .Lnftl_initialized_storage+944\n"
        ".type sandisk_read_retry_reg_adr_24, %object\n"
        ".size sandisk_read_retry_reg_adr_24, 9");

__asm__(".global sandisk_24\n"
        ".set sandisk_24, .Lnftl_initialized_storage+960\n"
        ".type sandisk_24, %object\n"
        ".size sandisk_24, 27");

__asm__(".global sandisk_read_retry_enable_cmd_1y\n"
        ".set sandisk_read_retry_enable_cmd_1y, .Lnftl_initialized_storage+992\n"
        ".type sandisk_read_retry_enable_cmd_1y, %object\n"
        ".size sandisk_read_retry_enable_cmd_1y, 1");

__asm__(".global sandisk_read_retry_exit_cmd_1y\n"
        ".set sandisk_read_retry_exit_cmd_1y, .Lnftl_initialized_storage+1000\n"
        ".type sandisk_read_retry_exit_cmd_1y, %object\n"
        ".size sandisk_read_retry_exit_cmd_1y, 1");

__asm__(".global sandisk_read_retry_cmd_1y\n"
        ".set sandisk_read_retry_cmd_1y, .Lnftl_initialized_storage+1008\n"
        ".type sandisk_read_retry_cmd_1y, %object\n"
        ".size sandisk_read_retry_cmd_1y, 1");

__asm__(".global sandisk_read_retry_reg_adr_1y\n"
        ".set sandisk_read_retry_reg_adr_1y, .Lnftl_initialized_storage+1016\n"
        ".type sandisk_read_retry_reg_adr_1y, %object\n"
        ".size sandisk_read_retry_reg_adr_1y, 1");

__asm__(".global sandisk_1y\n"
        ".set sandisk_1y, .Lnftl_initialized_storage+1024\n"
        ".type sandisk_1y, %object\n"
        ".size sandisk_1y, 76");

__asm__(".global hynix20nm_read_retry_reg_adr_1\n"
        ".set hynix20nm_read_retry_reg_adr_1, .Lnftl_initialized_storage+1104\n"
        ".type hynix20nm_read_retry_reg_adr_1, %object\n"
        ".size hynix20nm_read_retry_reg_adr_1, 8");

__asm__(".global hynix20nm_read_retry_reg_adr_2\n"
        ".set hynix20nm_read_retry_reg_adr_2, .Lnftl_initialized_storage+1112\n"
        ".type hynix20nm_read_retry_reg_adr_2, %object\n"
        ".size hynix20nm_read_retry_reg_adr_2, 8");

/* Keep the C helper's historical alternate entry labels at their field
 * addresses. These names were assembly-only views, with no extra storage. */
#define NFTL_TABLE_VIEW(alias, offset) \
	__asm__(".global " #alias "\n" \
		".set " #alias ", .Lnftl_initialized_storage+" #offset "\n" \
		".size " #alias ", 0")
NFTL_TABLE_VIEW(micron_p1_1_recovered, 696);
NFTL_TABLE_VIEW(micron_p1_2_recovered, 712);
NFTL_TABLE_VIEW(micron_p1_3_recovered, 728);
NFTL_TABLE_VIEW(micron_p1_4_recovered, 736);
NFTL_TABLE_VIEW(sandisk_19_low_recovered, 824);
NFTL_TABLE_VIEW(sandisk_19_high_recovered, 872);
NFTL_TABLE_VIEW(sandisk_24_recovered, 960);
NFTL_TABLE_VIEW(sandisk_1y_recovered, 1024);
#undef NFTL_TABLE_VIEW
