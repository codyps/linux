/* SPDX-License-Identifier: GPL-2.0 */
/* TG3040 #860 NFTL BSS: 51 instruction-rooted starts in 436 bytes.
 * Field byte extents are conservative; the vendor C element types are not
 * known. Keep the gaps and order because unchecked retry counts can write
 * into neighboring state. Fix bounds and ignored NAND errors separately.
 */

struct nftl_bss_layout {
	unsigned char phyinfo[20];
	unsigned char gap_0[4];
	unsigned char nftl_blk_head[136];
	unsigned char hynix16nm_read_retry_mode[1];
	unsigned char hynix16nm_read_retry_cycle[1];
	unsigned char hynix16nm_read_retry_reg_num[1];
	unsigned char gap_1[5];
	unsigned char hynix16nm_read_retry_reg_adr[4];
	unsigned char samsung_read_retry_mode[1];
	unsigned char samsung_read_retry_cycle[1];
	unsigned char hynix26nm_read_retry_mode[1];
	unsigned char hynix26nm_read_retry_cycle[1];
	unsigned char hynix26nm_read_retry_reg_num[1];
	unsigned char gap_2[7];
	unsigned char hynix26nm_read_retry_reg_adr[4];
	unsigned char gap_3[4];
	unsigned char hynix26nm_read_retry_val[56];
	unsigned char hynix26nm_lsb_mode_reg_adr[5];
	unsigned char gap_4[3];
	unsigned char hynix26nm_lsb_mode_default_val[5];
	unsigned char gap_5[3];
	unsigned char hynix26nm_lsb_mode_val[5];
	unsigned char hynix26nm_lsb_mode_reg_num[1];
	unsigned char sandisk_A19_read_retry_mode[1];
	unsigned char sandisk_A19_read_retry_cycle[1];
	unsigned char sandisk_A19_read_retry_reg_cnt[1];
	unsigned char toshiba_read_retry_mode[1];
	unsigned char toshiba_read_retry_cycle[1];
	unsigned char toshiba_read_retry_reg_cnt[1];
	unsigned char micron_read_retry_mode[1];
	unsigned char micron_read_retry_cycle[1];
	unsigned char sandisk_read_retry_mode[1];
	unsigned char sandisk_read_retry_cycle[1];
	unsigned char sandisk_read_retry_reg_cnt[1];
	unsigned char hynix20nm_read_retry_mode[1];
	unsigned char hynix20nm_read_retry_cycle[1];
	unsigned char hynix20nm_read_retry_reg_num[1];
	unsigned char gap_6[4];
	unsigned char hynix20nm_read_retry_reg_adr[8];
	unsigned char hynix20nm_lsb_mode_reg_adr[4];
	unsigned char gap_7[4];
	unsigned char hynix20nm_lsb_mode_default_val[4];
	unsigned char gap_8[4];
	unsigned char hynix20nm_lsb_mode_val[4];
	unsigned char hynix20nm_lsb_mode_reg_num[1];
	unsigned char gap_9[3];
	unsigned char gc_src_type[4];
	unsigned char gap_10[4];
	unsigned char panic_info[40];
	unsigned char panic_part[16];
	unsigned char nand_on_panic[4];
	unsigned char panic_enable[4];
	unsigned char sandisk_A19_sclk0_bak[4];
	unsigned char sandisk_A19_sclk1_bak[4];
	unsigned char toshiba_toggle_mode_flag[4];
	unsigned char toshiba_retry_flag[4];
	unsigned char toshiba_sclk0_bak[4];
	unsigned char toshiba_sclk1_bak[4];
	unsigned char micron_p1[8];
	unsigned char sandisk_sclk0_bak[4];
	unsigned char sandisk_sclk1_bak[4];
	unsigned char sandisk_toggle_mode_flag[4];
} __attribute__((packed));

static struct nftl_bss_layout nftl_bss_storage
	__asm__(".Lnftl_bss_storage")
	__attribute__((used, aligned(8)));

_Static_assert(sizeof(struct nftl_bss_layout) == 436,
	"NFTL BSS span changed");

_Static_assert(__builtin_offsetof(struct nftl_bss_layout, phyinfo) == 0,
	"NFTL phyinfo offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, nftl_blk_head) == 24,
	"NFTL nftl_blk_head offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix16nm_read_retry_mode) == 160,
	"NFTL hynix16nm_read_retry_mode offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix16nm_read_retry_cycle) == 161,
	"NFTL hynix16nm_read_retry_cycle offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix16nm_read_retry_reg_num) == 162,
	"NFTL hynix16nm_read_retry_reg_num offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix16nm_read_retry_reg_adr) == 168,
	"NFTL hynix16nm_read_retry_reg_adr offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, samsung_read_retry_mode) == 172,
	"NFTL samsung_read_retry_mode offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, samsung_read_retry_cycle) == 173,
	"NFTL samsung_read_retry_cycle offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix26nm_read_retry_mode) == 174,
	"NFTL hynix26nm_read_retry_mode offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix26nm_read_retry_cycle) == 175,
	"NFTL hynix26nm_read_retry_cycle offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix26nm_read_retry_reg_num) == 176,
	"NFTL hynix26nm_read_retry_reg_num offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix26nm_read_retry_reg_adr) == 184,
	"NFTL hynix26nm_read_retry_reg_adr offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix26nm_read_retry_val) == 192,
	"NFTL hynix26nm_read_retry_val offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix26nm_lsb_mode_reg_adr) == 248,
	"NFTL hynix26nm_lsb_mode_reg_adr offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix26nm_lsb_mode_default_val) == 256,
	"NFTL hynix26nm_lsb_mode_default_val offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix26nm_lsb_mode_val) == 264,
	"NFTL hynix26nm_lsb_mode_val offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix26nm_lsb_mode_reg_num) == 269,
	"NFTL hynix26nm_lsb_mode_reg_num offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, sandisk_A19_read_retry_mode) == 270,
	"NFTL sandisk_A19_read_retry_mode offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, sandisk_A19_read_retry_cycle) == 271,
	"NFTL sandisk_A19_read_retry_cycle offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, sandisk_A19_read_retry_reg_cnt) == 272,
	"NFTL sandisk_A19_read_retry_reg_cnt offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, toshiba_read_retry_mode) == 273,
	"NFTL toshiba_read_retry_mode offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, toshiba_read_retry_cycle) == 274,
	"NFTL toshiba_read_retry_cycle offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, toshiba_read_retry_reg_cnt) == 275,
	"NFTL toshiba_read_retry_reg_cnt offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, micron_read_retry_mode) == 276,
	"NFTL micron_read_retry_mode offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, micron_read_retry_cycle) == 277,
	"NFTL micron_read_retry_cycle offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, sandisk_read_retry_mode) == 278,
	"NFTL sandisk_read_retry_mode offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, sandisk_read_retry_cycle) == 279,
	"NFTL sandisk_read_retry_cycle offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, sandisk_read_retry_reg_cnt) == 280,
	"NFTL sandisk_read_retry_reg_cnt offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix20nm_read_retry_mode) == 281,
	"NFTL hynix20nm_read_retry_mode offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix20nm_read_retry_cycle) == 282,
	"NFTL hynix20nm_read_retry_cycle offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix20nm_read_retry_reg_num) == 283,
	"NFTL hynix20nm_read_retry_reg_num offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix20nm_read_retry_reg_adr) == 288,
	"NFTL hynix20nm_read_retry_reg_adr offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix20nm_lsb_mode_reg_adr) == 296,
	"NFTL hynix20nm_lsb_mode_reg_adr offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix20nm_lsb_mode_default_val) == 304,
	"NFTL hynix20nm_lsb_mode_default_val offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix20nm_lsb_mode_val) == 312,
	"NFTL hynix20nm_lsb_mode_val offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, hynix20nm_lsb_mode_reg_num) == 316,
	"NFTL hynix20nm_lsb_mode_reg_num offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, gc_src_type) == 320,
	"NFTL gc_src_type offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, panic_info) == 328,
	"NFTL panic_info offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, panic_part) == 368,
	"NFTL panic_part offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, nand_on_panic) == 384,
	"NFTL nand_on_panic offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, panic_enable) == 388,
	"NFTL panic_enable offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, sandisk_A19_sclk0_bak) == 392,
	"NFTL sandisk_A19_sclk0_bak offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, sandisk_A19_sclk1_bak) == 396,
	"NFTL sandisk_A19_sclk1_bak offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, toshiba_toggle_mode_flag) == 400,
	"NFTL toshiba_toggle_mode_flag offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, toshiba_retry_flag) == 404,
	"NFTL toshiba_retry_flag offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, toshiba_sclk0_bak) == 408,
	"NFTL toshiba_sclk0_bak offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, toshiba_sclk1_bak) == 412,
	"NFTL toshiba_sclk1_bak offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, micron_p1) == 416,
	"NFTL micron_p1 offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, sandisk_sclk0_bak) == 424,
	"NFTL sandisk_sclk0_bak offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, sandisk_sclk1_bak) == 428,
	"NFTL sandisk_sclk1_bak offset changed");
_Static_assert(__builtin_offsetof(struct nftl_bss_layout, sandisk_toggle_mode_flag) == 432,
	"NFTL sandisk_toggle_mode_flag offset changed");

__asm__(".global phyinfo\n"
        ".set phyinfo, .Lnftl_bss_storage+0\n"
        ".type phyinfo, %object\n"
        ".size phyinfo, 20");

__asm__(".global nftl_blk_head\n"
        ".set nftl_blk_head, .Lnftl_bss_storage+24\n"
        ".type nftl_blk_head, %object\n"
        ".size nftl_blk_head, 136");

__asm__(".global hynix16nm_read_retry_mode\n"
        ".set hynix16nm_read_retry_mode, .Lnftl_bss_storage+160\n"
        ".type hynix16nm_read_retry_mode, %object\n"
        ".size hynix16nm_read_retry_mode, 1");

__asm__(".global hynix16nm_read_retry_cycle\n"
        ".set hynix16nm_read_retry_cycle, .Lnftl_bss_storage+161\n"
        ".type hynix16nm_read_retry_cycle, %object\n"
        ".size hynix16nm_read_retry_cycle, 1");

__asm__(".global hynix16nm_read_retry_reg_num\n"
        ".set hynix16nm_read_retry_reg_num, .Lnftl_bss_storage+162\n"
        ".type hynix16nm_read_retry_reg_num, %object\n"
        ".size hynix16nm_read_retry_reg_num, 1");

__asm__(".global hynix16nm_read_retry_reg_adr\n"
        ".set hynix16nm_read_retry_reg_adr, .Lnftl_bss_storage+168\n"
        ".type hynix16nm_read_retry_reg_adr, %object\n"
        ".size hynix16nm_read_retry_reg_adr, 4");

__asm__(".global samsung_read_retry_mode\n"
        ".set samsung_read_retry_mode, .Lnftl_bss_storage+172\n"
        ".type samsung_read_retry_mode, %object\n"
        ".size samsung_read_retry_mode, 1");

__asm__(".global samsung_read_retry_cycle\n"
        ".set samsung_read_retry_cycle, .Lnftl_bss_storage+173\n"
        ".type samsung_read_retry_cycle, %object\n"
        ".size samsung_read_retry_cycle, 1");

__asm__(".global hynix26nm_read_retry_mode\n"
        ".set hynix26nm_read_retry_mode, .Lnftl_bss_storage+174\n"
        ".type hynix26nm_read_retry_mode, %object\n"
        ".size hynix26nm_read_retry_mode, 1");

__asm__(".global hynix26nm_read_retry_cycle\n"
        ".set hynix26nm_read_retry_cycle, .Lnftl_bss_storage+175\n"
        ".type hynix26nm_read_retry_cycle, %object\n"
        ".size hynix26nm_read_retry_cycle, 1");

__asm__(".global hynix26nm_read_retry_reg_num\n"
        ".set hynix26nm_read_retry_reg_num, .Lnftl_bss_storage+176\n"
        ".type hynix26nm_read_retry_reg_num, %object\n"
        ".size hynix26nm_read_retry_reg_num, 1");

__asm__(".global hynix26nm_read_retry_reg_adr\n"
        ".set hynix26nm_read_retry_reg_adr, .Lnftl_bss_storage+184\n"
        ".type hynix26nm_read_retry_reg_adr, %object\n"
        ".size hynix26nm_read_retry_reg_adr, 4");

__asm__(".global hynix26nm_read_retry_val\n"
        ".set hynix26nm_read_retry_val, .Lnftl_bss_storage+192\n"
        ".type hynix26nm_read_retry_val, %object\n"
        ".size hynix26nm_read_retry_val, 56");

__asm__(".global hynix26nm_lsb_mode_reg_adr\n"
        ".set hynix26nm_lsb_mode_reg_adr, .Lnftl_bss_storage+248\n"
        ".type hynix26nm_lsb_mode_reg_adr, %object\n"
        ".size hynix26nm_lsb_mode_reg_adr, 5");

__asm__(".global hynix26nm_lsb_mode_default_val\n"
        ".set hynix26nm_lsb_mode_default_val, .Lnftl_bss_storage+256\n"
        ".type hynix26nm_lsb_mode_default_val, %object\n"
        ".size hynix26nm_lsb_mode_default_val, 5");

__asm__(".global hynix26nm_lsb_mode_val\n"
        ".set hynix26nm_lsb_mode_val, .Lnftl_bss_storage+264\n"
        ".type hynix26nm_lsb_mode_val, %object\n"
        ".size hynix26nm_lsb_mode_val, 5");

__asm__(".global hynix26nm_lsb_mode_reg_num\n"
        ".set hynix26nm_lsb_mode_reg_num, .Lnftl_bss_storage+269\n"
        ".type hynix26nm_lsb_mode_reg_num, %object\n"
        ".size hynix26nm_lsb_mode_reg_num, 1");

__asm__(".global sandisk_A19_read_retry_mode\n"
        ".set sandisk_A19_read_retry_mode, .Lnftl_bss_storage+270\n"
        ".type sandisk_A19_read_retry_mode, %object\n"
        ".size sandisk_A19_read_retry_mode, 1");

__asm__(".global sandisk_A19_read_retry_cycle\n"
        ".set sandisk_A19_read_retry_cycle, .Lnftl_bss_storage+271\n"
        ".type sandisk_A19_read_retry_cycle, %object\n"
        ".size sandisk_A19_read_retry_cycle, 1");

__asm__(".global sandisk_A19_read_retry_reg_cnt\n"
        ".set sandisk_A19_read_retry_reg_cnt, .Lnftl_bss_storage+272\n"
        ".type sandisk_A19_read_retry_reg_cnt, %object\n"
        ".size sandisk_A19_read_retry_reg_cnt, 1");

__asm__(".global toshiba_read_retry_mode\n"
        ".set toshiba_read_retry_mode, .Lnftl_bss_storage+273\n"
        ".type toshiba_read_retry_mode, %object\n"
        ".size toshiba_read_retry_mode, 1");

__asm__(".global toshiba_read_retry_cycle\n"
        ".set toshiba_read_retry_cycle, .Lnftl_bss_storage+274\n"
        ".type toshiba_read_retry_cycle, %object\n"
        ".size toshiba_read_retry_cycle, 1");

__asm__(".global toshiba_read_retry_reg_cnt\n"
        ".set toshiba_read_retry_reg_cnt, .Lnftl_bss_storage+275\n"
        ".type toshiba_read_retry_reg_cnt, %object\n"
        ".size toshiba_read_retry_reg_cnt, 1");

__asm__(".global micron_read_retry_mode\n"
        ".set micron_read_retry_mode, .Lnftl_bss_storage+276\n"
        ".type micron_read_retry_mode, %object\n"
        ".size micron_read_retry_mode, 1");

__asm__(".global micron_read_retry_cycle\n"
        ".set micron_read_retry_cycle, .Lnftl_bss_storage+277\n"
        ".type micron_read_retry_cycle, %object\n"
        ".size micron_read_retry_cycle, 1");

__asm__(".global sandisk_read_retry_mode\n"
        ".set sandisk_read_retry_mode, .Lnftl_bss_storage+278\n"
        ".type sandisk_read_retry_mode, %object\n"
        ".size sandisk_read_retry_mode, 1");

__asm__(".global sandisk_read_retry_cycle\n"
        ".set sandisk_read_retry_cycle, .Lnftl_bss_storage+279\n"
        ".type sandisk_read_retry_cycle, %object\n"
        ".size sandisk_read_retry_cycle, 1");

__asm__(".global sandisk_read_retry_reg_cnt\n"
        ".set sandisk_read_retry_reg_cnt, .Lnftl_bss_storage+280\n"
        ".type sandisk_read_retry_reg_cnt, %object\n"
        ".size sandisk_read_retry_reg_cnt, 1");

__asm__(".global hynix20nm_read_retry_mode\n"
        ".set hynix20nm_read_retry_mode, .Lnftl_bss_storage+281\n"
        ".type hynix20nm_read_retry_mode, %object\n"
        ".size hynix20nm_read_retry_mode, 1");

__asm__(".global hynix20nm_read_retry_cycle\n"
        ".set hynix20nm_read_retry_cycle, .Lnftl_bss_storage+282\n"
        ".type hynix20nm_read_retry_cycle, %object\n"
        ".size hynix20nm_read_retry_cycle, 1");

__asm__(".global hynix20nm_read_retry_reg_num\n"
        ".set hynix20nm_read_retry_reg_num, .Lnftl_bss_storage+283\n"
        ".type hynix20nm_read_retry_reg_num, %object\n"
        ".size hynix20nm_read_retry_reg_num, 1");

__asm__(".global hynix20nm_read_retry_reg_adr\n"
        ".set hynix20nm_read_retry_reg_adr, .Lnftl_bss_storage+288\n"
        ".type hynix20nm_read_retry_reg_adr, %object\n"
        ".size hynix20nm_read_retry_reg_adr, 8");

__asm__(".global hynix20nm_lsb_mode_reg_adr\n"
        ".set hynix20nm_lsb_mode_reg_adr, .Lnftl_bss_storage+296\n"
        ".type hynix20nm_lsb_mode_reg_adr, %object\n"
        ".size hynix20nm_lsb_mode_reg_adr, 4");

__asm__(".global hynix20nm_lsb_mode_default_val\n"
        ".set hynix20nm_lsb_mode_default_val, .Lnftl_bss_storage+304\n"
        ".type hynix20nm_lsb_mode_default_val, %object\n"
        ".size hynix20nm_lsb_mode_default_val, 4");

__asm__(".global hynix20nm_lsb_mode_val\n"
        ".set hynix20nm_lsb_mode_val, .Lnftl_bss_storage+312\n"
        ".type hynix20nm_lsb_mode_val, %object\n"
        ".size hynix20nm_lsb_mode_val, 4");

__asm__(".global hynix20nm_lsb_mode_reg_num\n"
        ".set hynix20nm_lsb_mode_reg_num, .Lnftl_bss_storage+316\n"
        ".type hynix20nm_lsb_mode_reg_num, %object\n"
        ".size hynix20nm_lsb_mode_reg_num, 1");

__asm__(".global gc_src_type\n"
        ".set gc_src_type, .Lnftl_bss_storage+320\n"
        ".type gc_src_type, %object\n"
        ".size gc_src_type, 4");

__asm__(".global panic_info\n"
        ".set panic_info, .Lnftl_bss_storage+328\n"
        ".type panic_info, %object\n"
        ".size panic_info, 40");

__asm__(".global panic_part\n"
        ".set panic_part, .Lnftl_bss_storage+368\n"
        ".type panic_part, %object\n"
        ".size panic_part, 16");

__asm__(".global nand_on_panic\n"
        ".set nand_on_panic, .Lnftl_bss_storage+384\n"
        ".type nand_on_panic, %object\n"
        ".size nand_on_panic, 4");

__asm__(".global panic_enable\n"
        ".set panic_enable, .Lnftl_bss_storage+388\n"
        ".type panic_enable, %object\n"
        ".size panic_enable, 4");

__asm__(".global sandisk_A19_sclk0_bak\n"
        ".set sandisk_A19_sclk0_bak, .Lnftl_bss_storage+392\n"
        ".type sandisk_A19_sclk0_bak, %object\n"
        ".size sandisk_A19_sclk0_bak, 4");

__asm__(".global sandisk_A19_sclk1_bak\n"
        ".set sandisk_A19_sclk1_bak, .Lnftl_bss_storage+396\n"
        ".type sandisk_A19_sclk1_bak, %object\n"
        ".size sandisk_A19_sclk1_bak, 4");

__asm__(""
        ".set toshiba_toggle_mode_flag, .Lnftl_bss_storage+400\n"
        ".type toshiba_toggle_mode_flag, %object\n"
        ".size toshiba_toggle_mode_flag, 4");

__asm__(""
        ".set toshiba_retry_flag, .Lnftl_bss_storage+404\n"
        ".type toshiba_retry_flag, %object\n"
        ".size toshiba_retry_flag, 4");

__asm__(".global toshiba_sclk0_bak\n"
        ".set toshiba_sclk0_bak, .Lnftl_bss_storage+408\n"
        ".type toshiba_sclk0_bak, %object\n"
        ".size toshiba_sclk0_bak, 4");

__asm__(".global toshiba_sclk1_bak\n"
        ".set toshiba_sclk1_bak, .Lnftl_bss_storage+412\n"
        ".type toshiba_sclk1_bak, %object\n"
        ".size toshiba_sclk1_bak, 4");

__asm__(""
        ".set micron_p1, .Lnftl_bss_storage+416\n"
        ".type micron_p1, %object\n"
        ".size micron_p1, 8");

__asm__(".global sandisk_sclk0_bak\n"
        ".set sandisk_sclk0_bak, .Lnftl_bss_storage+424\n"
        ".type sandisk_sclk0_bak, %object\n"
        ".size sandisk_sclk0_bak, 4");

__asm__(".global sandisk_sclk1_bak\n"
        ".set sandisk_sclk1_bak, .Lnftl_bss_storage+428\n"
        ".type sandisk_sclk1_bak, %object\n"
        ".size sandisk_sclk1_bak, 4");

__asm__(".global sandisk_toggle_mode_flag\n"
        ".set sandisk_toggle_mode_flag, .Lnftl_bss_storage+432\n"
        ".type sandisk_toggle_mode_flag, %object\n"
        ".size sandisk_toggle_mode_flag, 4");

/* Historical C-only views of private BSS fields. */
#define NFTL_BSS_VIEW(alias, offset) \
	__asm__(".global " #alias "\n" \
		".set " #alias ", .Lnftl_bss_storage+" #offset "\n" \
		".size " #alias ", 0")
NFTL_BSS_VIEW(toshiba_toggle_mode_flag_recovered, 400);
NFTL_BSS_VIEW(toshiba_retry_flag_recovered, 404);
NFTL_BSS_VIEW(micron_p1_recovered, 416);
#undef NFTL_BSS_VIEW
