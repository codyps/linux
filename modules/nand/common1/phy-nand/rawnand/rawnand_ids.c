/*
 * nand_ids.c
 *
 * Copyright (C) 2019 Allwinner.
 *
 * cuizhikui <cuizhikui@allwinnertech.com>
 *
 * SPDX-License-Identifier: GPL-2.0
 */

#include "rawnand_ids.h"
#include "rawnand_chip.h"
#include "rawnand_boot.h"
#include "rawnand_debug.h"
#include <linux/string.h>

/*cmd stay to handle*/
struct nand_phy_op_par phy_op_para[] = {
	{
		// CMD_SET_0 index
		.instr = {
			.read_instr = {0x00, 0x30},
			.multi_plane_read_instr = {0x00, 0x30, 0x00, 0x30}, //simulate
			.write_instr = {0x80, 0x10},
			.multi_plane_write_instr = {0x80, 0x10, 0x80, 0x10}, //simulate
		},
	},
	{
		// CMD_SET_1 index
		.instr = {
			.read_instr = {0x00, 0x30},
			.multi_plane_read_instr = {0x00, 0x30, 0x00, 0x30}, //simulate
			.write_instr = {0x80, 0x10},
			.multi_plane_write_instr = {0x80, 0x11, 0x80, 0x10},
		},
	},
	{
		// CMD_SET_2 index
		.instr = {
			.read_instr = {0x00, 0x30},
			.multi_plane_read_instr = {0x00, 0x32, 0x00, 0x30},
			.write_instr = {0x80, 0x10},
			.multi_plane_write_instr = {0x80, 0x11, 0x81, 0x10},
		},
	},
	{
		// CMD_SET_3 index
		.instr = {
			.read_instr = {0x00, 0x30},
			.multi_plane_read_instr = {0x00, 0x32, 0x00, 0x30},
			.write_instr = {0x80, 0x10},
			.multi_plane_write_instr = {0x80, 0x11, 0x80, 0x10},
		},
	},
};

/* Recovered TG3040 #860 NAND IDs. See recover_nand_ids.py. */
/* Recovered TG3040 #860 NAND IDs. See recover_nand_ids.py. */
struct sunxi_nand_flash_device raw_sandisk[] = {
	{
		.name = "SDTNSGAMA-016G",
		.id = { 0x45, 0x3a, 0x94, 0x93, 0x76, 0x51, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x1074,
		.operation_opt = 0x30088ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x4,
		.read_retry_type = 0x342009,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x1,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x7,
		.id_number = 0x0,
		.max_blk_erase_times = 0xbb8,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "SDTNRFAMA-004G",
		.id = { 0x45, 0xd7, 0x84, 0x93, 0x72, 0x50, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x424,
		.operation_opt = 0x30180ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x4,
		.read_retry_type = 0x331f04,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x1,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x7,
		.id_number = 0x1,
		.max_blk_erase_times = 0xfa0,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "SDTNRGAMA-008G",
		.id = { 0x45, 0xde, 0x94, 0x93, 0x76, 0x50, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x854,
		.operation_opt = 0x30188ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x4,
		.read_retry_type = 0x331f04,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x1,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x7,
		.id_number = 0x2,
		.max_blk_erase_times = 0xfa0,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "SDTNQGAMA-008G",
		.id = { 0x45, 0xde, 0x94, 0x93, 0x76, 0x57, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x844,
		.operation_opt = 0x30188ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x4,
		.read_retry_type = 0x301409,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x7,
		.id_number = 0x4,
		.max_blk_erase_times = 0xfa0,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "SDTNQFAMA-004G",
		.id = { 0x45, 0xd7, 0x84, 0x93, 0x72, 0x57, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x424,
		.operation_opt = 0x30180ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x4,
		.read_retry_type = 0x301409,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x7,
		.id_number = 0x5,
		.max_blk_erase_times = 0xfa0,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
};
struct sunxi_nand_flash_device raw_toshiba[] = {
	{
		.name = "TC58TEG6DDLTA00",
		.id = { 0x98, 0xde, 0x94, 0x93, 0x76, 0x51, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x864,
		.operation_opt = 0x10088ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x3c,
		.ecc_mode = 0x4,
		.read_retry_type = 0x120a05,
		.ddr_type = 0x3,
		.ddr_opt = 0x10600,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x6,
		.id_number = 0x0,
		.max_blk_erase_times = 0xbb8,
		.access_high_freq = 0x3c,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "TC58NVG2S0HTAI0",
		.id = { 0x98, 0xdc, 0x90, 0x26, 0x76, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x8,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x800,
		.operation_opt = 0x88ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x3,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x0,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x6,
		.id_number = 0x1,
		.max_blk_erase_times = 0xea60,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "TC58NVG1S3HTA00",
		.id = { 0x98, 0xda, 0x90, 0x15, 0x76, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x4,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x800,
		.operation_opt = 0x88ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x3,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x2,
		.max_blk_erase_times = 0xea60,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "TC58TFG7DDLTA0D",
		.id = { 0x98, 0x3a, 0x94, 0x93, 0x76, 0x51, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x1074,
		.operation_opt = 0x10188ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x4,
		.read_retry_type = 0x120a05,
		.ddr_type = 0x3,
		.ddr_opt = 0x10600,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x6,
		.id_number = 0x3,
		.max_blk_erase_times = 0xbb8,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "TC58BVG0S3HTAI0",
		.id = { 0x98, 0xf1, 0x80, 0x15, 0xf2, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x4,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x400,
		.operation_opt = 0x80ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x0,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x0,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x0,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x4,
		.max_blk_erase_times = 0x7530,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "TC58TEG5DCKTA00",
		.id = { 0x98, 0xd7, 0x84, 0x93, 0x72, 0x50, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x424,
		.operation_opt = 0x10180ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x4,
		.read_retry_type = 0x110705,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x6,
		.id_number = 0x5,
		.max_blk_erase_times = 0xbb8,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "TC58TEG6DDJTA00",
		.id = { 0x98, 0xde, 0x94, 0x93, 0x76, 0x57, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x852,
		.operation_opt = 0x10180ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x4,
		.read_retry_type = 0x100604,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x6,
		.id_number = 0x6,
		.max_blk_erase_times = 0xfa0,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "TC58TEG5DCJTA00",
		.id = { 0x98, 0xd7, 0x84, 0x93, 0x72, 0x57, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x424,
		.operation_opt = 0x10180ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x4,
		.read_retry_type = 0x100604,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x6,
		.id_number = 0x7,
		.max_blk_erase_times = 0xfa0,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "TC58NVG6DCJTA00",
		.id = { 0x98, 0xde, 0x84, 0x93, 0x72, 0x57, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x82c,
		.operation_opt = 0x10180ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x4,
		.read_retry_type = 0x100604,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x6,
		.id_number = 0x8,
		.max_blk_erase_times = 0xfa0,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "TH58NVG6D2ETA20",
		.id = { 0x98, 0xd7, 0x95, 0x32, 0xff, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x2,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x80,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x88ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x19,
		.ecc_mode = 0x1,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x3,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x6,
		.id_number = 0x9,
		.max_blk_erase_times = 0x1f40,
		.access_high_freq = 0x19,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "TH58NVG6D2FTA20",
		.id = { 0x98, 0xd7, 0x94, 0x32, 0xff, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x80,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x88ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x19,
		.ecc_mode = 0x2,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x3,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x6,
		.id_number = 0x10,
		.max_blk_erase_times = 0x1f40,
		.access_high_freq = 0x19,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
};
struct sunxi_nand_flash_device raw_micron[] = {
	{
		.name = "MT29F64G08CBABA",
		.id = { 0x2c, 0x64, 0x44, 0x4b, 0xa9, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x41088ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x6,
		.read_retry_type = 0x400a01,
		.ddr_type = 0x0,
		.ddr_opt = 0x8,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0x0,
		.max_blk_erase_times = 0xbb8,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "MT29F32G08CBADBWP",
		.id = { 0x2c, 0x44, 0x44, 0x4b, 0xa9, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x850,
		.operation_opt = 0x441088ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x3c,
		.ecc_mode = 0x6,
		.read_retry_type = 0x400a01,
		.ddr_type = 0x2,
		.ddr_opt = 0xe,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0x1,
		.max_blk_erase_times = 0xbb8,
		.access_high_freq = 0x3c,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "MT29F128G08CFAAA",
		.id = { 0x2c, 0x88, 0x04, 0x4b, 0xa9, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x88ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x2,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x8,
		.bad_block_flag_position = 0x0,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x3,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x2,
		.max_blk_erase_times = 0xbb8,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "FBNL05B128G1KDBABJ4",
		.id = { 0x2c, 0x84, 0x44, 0x32, 0xaa, 0x04, 0x00, 0x00 },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x200,
		.blk_cnt_per_die = 0x890,
		.operation_opt = 0xffffffff90000780ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x4b,
		.ecc_mode = 0xd,
		.read_retry_type = 0x421201,
		.ddr_type = 0x2,
		.ddr_opt = 0x1040b,
		.bad_block_flag_position = 0x0,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x3,
		.ddr_info_no = 0x1,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0x3,
		.max_blk_erase_times = 0x5dc,
		.access_high_freq = 0x4b,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "FBNL06B256G1KDBABJ4",
		.id = { 0x2c, 0xa4, 0x64, 0x32, 0xaa, 0x04, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x400,
		.blk_cnt_per_die = 0x890,
		.operation_opt = 0x44080ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x4b,
		.ecc_mode = 0xd,
		.read_retry_type = 0x421000,
		.ddr_type = 0x2,
		.ddr_opt = 0x1000a,
		.bad_block_flag_position = 0x0,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0x4,
		.max_blk_erase_times = 0x3e8,
		.access_high_freq = 0x4b,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "MT29F4G08ABAF",
		.id = { 0x2c, 0xdc, 0x80, 0xa6, 0x62, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x8,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x800,
		.operation_opt = 0x88ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x3,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x8,
		.bad_block_flag_position = 0x0,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x0,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x5,
		.max_blk_erase_times = 0xea60,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "MT29F32G08CBADAWP",
		.id = { 0x2c, 0x44, 0x44, 0x4b, 0xa9, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x850,
		.operation_opt = 0x441088ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x6,
		.read_retry_type = 0x400a01,
		.ddr_type = 0x0,
		.ddr_opt = 0x8,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0x6,
		.max_blk_erase_times = 0xbb8,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "FxxL85C71KSBAB",
		.id = { 0x2c, 0x84, 0x64, 0x3c, 0xa9, 0x04, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x200,
		.blk_cnt_per_die = 0x830,
		.operation_opt = 0x10441388ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x4b,
		.ecc_mode = 0x4,
		.read_retry_type = 0x410c01,
		.ddr_type = 0x2,
		.ddr_opt = 0x40f,
		.bad_block_flag_position = 0x0,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x1,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0x7,
		.max_blk_erase_times = 0x5dc,
		.access_high_freq = 0x4b,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "FxxL95B71KDBABH6",
		.id = { 0x2c, 0x84, 0x64, 0x54, 0xa9, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x200,
		.blk_cnt_per_die = 0x830,
		.operation_opt = 0x42188ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x4b,
		.ecc_mode = 0xa,
		.read_retry_type = 0x410c01,
		.ddr_type = 0x2,
		.ddr_opt = 0x1000f,
		.bad_block_flag_position = 0x0,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x3,
		.ddr_info_no = 0x1,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0x8,
		.max_blk_erase_times = 0x3e8,
		.access_high_freq = 0x4b,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "L84D",
		.id = { 0x2c, 0x64, 0x64, 0x3c, 0xa5, 0x08, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x200,
		.blk_cnt_per_die = 0x418,
		.operation_opt = 0x41188ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x4,
		.read_retry_type = 0x400a01,
		.ddr_type = 0x2,
		.ddr_opt = 0x1000b,
		.bad_block_flag_position = 0x0,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x3,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0x9,
		.max_blk_erase_times = 0xbb8,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "L04A",
		.id = { 0x2c, 0x64, 0x44, 0x32, 0xa5, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x200,
		.blk_cnt_per_die = 0x440,
		.operation_opt = 0xffffffff80000188ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x4b,
		.ecc_mode = 0xd,
		.read_retry_type = 0x421201,
		.ddr_type = 0x2,
		.ddr_opt = 0x1000b,
		.bad_block_flag_position = 0x0,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x3,
		.ddr_info_no = 0x1,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0xa,
		.max_blk_erase_times = 0x3e8,
		.access_high_freq = 0x4b,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "FxxL85C71KSBAR",
		.id = { 0x2c, 0x84, 0x64, 0x3c, 0xa9, 0x04, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x200,
		.blk_cnt_per_die = 0x830,
		.operation_opt = 0x441188ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x4,
		.read_retry_type = 0x410c01,
		.ddr_type = 0x0,
		.ddr_opt = 0xe,
		.bad_block_flag_position = 0x0,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0xb,
		.max_blk_erase_times = 0x5dc,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "MT29F256G08CJABA",
		.id = { 0x2c, 0x84, 0xc5, 0x4b, 0xa9, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x2,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x41788ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x5,
		.read_retry_type = 0x400a01,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0xd,
		.max_blk_erase_times = 0xdac,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "MT29F64G08CBAAA",
		.id = { 0x2c, 0x88, 0x24, 0x4b, 0xff, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x388ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x2,
		.read_retry_type = 0x400a01,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0xe,
		.max_blk_erase_times = 0x1388,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "MT29F32G08CBACA",
		.id = { 0x2c, 0x68, 0x04, 0x4a, 0xff, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x8,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x288ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x2,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0xf,
		.max_blk_erase_times = 0x1388,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "MT29F32G08CBABA",
		.id = { 0x2c, 0x68, 0x04, 0x46, 0xff, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x8,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x288ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x2,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0x10,
		.max_blk_erase_times = 0x1388,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "MT29F32G08CBAAA",
		.id = { 0x2c, 0xd7, 0x94, 0x3e, 0xff, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x8,
		.page_cnt_per_blk = 0x80,
		.blk_cnt_per_die = 0x2000,
		.operation_opt = 0x288ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x2,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0x11,
		.max_blk_erase_times = 0x1388,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "MT29F8G08MAA",
		.id = { 0x2c, 0xd3, 0x94, 0xa5, 0xff, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x4,
		.page_cnt_per_blk = 0x80,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x88ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x0,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0x10,
		.max_blk_erase_times = 0x2710,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
};
struct sunxi_nand_flash_device raw_spansion[] = {
	{
		.name = "S34ML01G200TFI000",
		.id = { 0x01, 0xf1, 0x80, 0x1d, 0xff, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x4,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x400,
		.operation_opt = 0x80ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x0,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x0,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x0,
		.max_blk_erase_times = 0xea60,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "S34ML02G200TFI000",
		.id = { 0x01, 0xda, 0x90, 0x95, 0x46, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x4,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x800,
		.operation_opt = 0x98ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x3,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x1,
		.max_blk_erase_times = 0xea60,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
};
struct sunxi_nand_flash_device raw_esmt[] = {
	{
		.name = "IS34ML04G084",
		.id = { 0xc8, 0xdc, 0x90, 0x95, 0x54, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x4,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x88ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x0,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x0,
		.max_blk_erase_times = 0xea60,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "F59L2G81KA",
		.id = { 0xc8, 0x6a, 0x90, 0x04, 0x34, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x4,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x800,
		.operation_opt = 0x88ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x3,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x1,
		.max_blk_erase_times = 0xc350,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "F59L1G81MB",
		.id = { 0xc8, 0xd1, 0x80, 0x95, 0x40, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x4,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x400,
		.operation_opt = 0x80ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x0,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x0,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x2,
		.max_blk_erase_times = 0xea60,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
};
struct sunxi_nand_flash_device raw_intel[] = {
	{
		.name = "JS29F64G08ACMF3",
		.id = { 0x89, 0x88, 0x24, 0x4b, 0xa9, 0x84, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x10040388ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x32,
		.ecc_mode = 0x4,
		.read_retry_type = 0x500701,
		.ddr_type = 0x0,
		.ddr_opt = 0x1040b,
		.bad_block_flag_position = 0x0,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x1,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0x0,
		.max_blk_erase_times = 0xbb8,
		.access_high_freq = 0x32,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "AW56B16A4",
		.id = { 0x89, 0x84, 0x64, 0x3c, 0xa5, 0x0c, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x200,
		.blk_cnt_per_die = 0x830,
		.operation_opt = 0x10041388ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x4b,
		.ecc_mode = 0x4,
		.read_retry_type = 0x411201,
		.ddr_type = 0x2,
		.ddr_opt = 0x40f,
		.bad_block_flag_position = 0x0,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x4,
		.id_number = 0x1,
		.max_blk_erase_times = 0x5dc,
		.access_high_freq = 0x4b,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
};
struct sunxi_nand_flash_device raw_mxic[] = {
	{
		.name = "MX30LF2G18AC",
		.id = { 0xc2, 0xda, 0x90, 0x95, 0x06, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x4,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x800,
		.operation_opt = 0x280ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x32,
		.ecc_mode = 0x0,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x0,
		.max_blk_erase_times = 0x2710,
		.access_high_freq = 0x32,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "MX30LF1G18AC",
		.id = { 0xc2, 0xf1, 0x80, 0x95, 0x02, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x4,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x400,
		.operation_opt = 0x280ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x32,
		.ecc_mode = 0x0,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x0,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x1,
		.max_blk_erase_times = 0x2710,
		.access_high_freq = 0x32,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
};
struct sunxi_nand_flash_device raw_giga[] = {
	{
		.name = "GD9FU2G8F2A",
		.id = { 0xc8, 0xda, 0x90, 0x95, 0x46, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x4,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x800,
		.operation_opt = 0x280ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x32,
		.ecc_mode = 0x3,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x0,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x0,
		.max_blk_erase_times = 0x2710,
		.access_high_freq = 0x32,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
};
struct sunxi_nand_flash_device raw_foresee[] = {
	{
		.name = "FS33ND04GS108TF10",
		.id = { 0xec, 0xdc, 0x10, 0x95, 0x56, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x4,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x88ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x0,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x0,
		.max_blk_erase_times = 0xea60,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "FS33ND01GS108TF10",
		.id = { 0xec, 0xf1, 0x00, 0x95, 0x42, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x4,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x400,
		.operation_opt = 0x80ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x0,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x0,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x1,
		.max_blk_erase_times = 0xea60,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
};
struct sunxi_nand_flash_device raw_samsung[] = {
	{
		.name = "FS33ND01GS108TF10",
		.id = { 0xec, 0xf1, 0x00, 0x95, 0x42, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x4,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x400,
		.operation_opt = 0x80ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x0,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x1,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x0,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x0,
		.max_blk_erase_times = 0xea60,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "K9GCGD8U0F",
		.id = { 0xec, 0xde, 0x94, 0xc3, 0xa4, 0xca, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x318,
		.blk_cnt_per_die = 0x2b0,
		.operation_opt = 0x20000180ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x7,
		.read_retry_type = 0x200e04,
		.ddr_type = 0x13,
		.ddr_opt = 0x200,
		.bad_block_flag_position = 0x0,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x5,
		.id_number = 0x1,
		.max_blk_erase_times = 0x5dc,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "K9HDG08U1A",
		.id = { 0xec, 0xde, 0xd5, 0x7a, 0x58, 0x43, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x80,
		.blk_cnt_per_die = 0x1064,
		.operation_opt = 0x888ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x3,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x5,
		.id_number = 0x2,
		.max_blk_erase_times = 0x5dc,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "K9LCG08U0B",
		.id = { 0xec, 0xde, 0xd5, 0x7e, 0x68, 0x44, 0xff, 0xff },
		.die_cnt_per_chip = 0x2,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x80,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x20188ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x4,
		.read_retry_type = 0x200e04,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x5,
		.id_number = 0x3,
		.max_blk_erase_times = 0xdac,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "K9GBG08U0B",
		.id = { 0xec, 0xd7, 0x94, 0x7e, 0x64, 0x44, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x80,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x20188ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x4,
		.read_retry_type = 0x200e04,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x5,
		.id_number = 0x4,
		.max_blk_erase_times = 0xdac,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "K9GBG08U0A",
		.id = { 0xec, 0xd7, 0x94, 0x76, 0x64, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x80,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x88ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x3,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x5,
		.id_number = 0x5,
		.max_blk_erase_times = 0x1f40,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "K9LCG08U0A",
		.id = { 0xec, 0xde, 0xd5, 0x7a, 0x58, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x2,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x80,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x888ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x3,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x5,
		.id_number = 0x6,
		.max_blk_erase_times = 0xfa0,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "K9GAG08U0M",
		.id = { 0xec, 0xd5, 0x14, 0xb6, 0xff, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x8,
		.page_cnt_per_blk = 0x80,
		.blk_cnt_per_die = 0x1000,
		.operation_opt = 0x88ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x0,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x5,
		.id_number = 0x6,
		.max_blk_erase_times = 0x2710,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "K9F2G08U0A",
		.id = { 0xec, 0xda, 0x10, 0x95, 0x44, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x4,
		.page_cnt_per_blk = 0x40,
		.blk_cnt_per_die = 0x800,
		.operation_opt = 0x8cULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x0,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x5,
		.id_number = 0x7,
		.max_blk_erase_times = 0x2710,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
};
struct sunxi_nand_flash_device raw_hynix[] = {
	{
		.name = "H27TDG8T2D8R",
		.id = { 0xad, 0x3a, 0x14, 0x03, 0x08, 0x50, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x184,
		.blk_cnt_per_die = 0xaa4,
		.operation_opt = 0x88ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x32,
		.ecc_mode = 0x8,
		.read_retry_type = 0x0,
		.ddr_type = 0x13,
		.ddr_opt = 0x700,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x1,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x0,
		.id_number = 0x0,
		.max_blk_erase_times = 0x2710,
		.access_high_freq = 0x32,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "H27UCG8T2ETR",
		.id = { 0xad, 0xde, 0x14, 0xa7, 0x42, 0x4a, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x848,
		.operation_opt = 0x188ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x6,
		.read_retry_type = 0x40704,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x1,
		.id_number = 0x1,
		.max_blk_erase_times = 0x7d0,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "H27UBG8T2DTR",
		.id = { 0xad, 0xd7, 0x14, 0x9e, 0x34, 0x4a, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x84a,
		.operation_opt = 0x188ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x6,
		.read_retry_type = 0x40704,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x1,
		.id_number = 0x2,
		.max_blk_erase_times = 0x7d0,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "H27UCG8T2B",
		.id = { 0xad, 0xde, 0x94, 0xeb, 0x74, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x20,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x854,
		.operation_opt = 0x188ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x4,
		.read_retry_type = 0x30708,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x2,
		.id_number = 0x3,
		.max_blk_erase_times = 0x7d0,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "H27UBG8T2C",
		.id = { 0xad, 0xd7, 0x94, 0x91, 0x60, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x82c,
		.operation_opt = 0x188ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x28,
		.ecc_mode = 0x4,
		.read_retry_type = 0x30708,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x2,
		.id_number = 0x4,
		.max_blk_erase_times = 0x7d0,
		.access_high_freq = 0x28,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "H27UBG8T2B",
		.id = { 0xad, 0xd7, 0x94, 0xda, 0xff, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x800,
		.operation_opt = 0x188ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x4,
		.read_retry_type = 0x10604,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x3,
		.id_number = 0x5,
		.max_blk_erase_times = 0x7d0,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "H27UBG8T2A",
		.id = { 0xad, 0xd7, 0x94, 0x9a, 0xff, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x800,
		.operation_opt = 0x88ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x2,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x1,
		.id_number = 0x7,
		.max_blk_erase_times = 0x1388,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
	{
		.name = "H27UAG8T2B",
		.id = { 0xad, 0xd5, 0x94, 0x9a, 0xff, 0xff, 0xff, 0xff },
		.die_cnt_per_chip = 0x1,
		.sect_cnt_per_page = 0x10,
		.page_cnt_per_blk = 0x100,
		.blk_cnt_per_die = 0x400,
		.operation_opt = 0x80ULL,
		.valid_blk_ratio = 0x380,
		.access_freq = 0x1e,
		.ecc_mode = 0x2,
		.read_retry_type = 0x0,
		.ddr_type = 0x0,
		.ddr_opt = 0x0,
		.bad_block_flag_position = 0x2,
		.multi_plane_block_offset = 0x1,
		.cmd_set_no = 0x2,
		.ddr_info_no = 0x0,
		.selected_write_boot0_no = 0x0,
		.selected_readretry_no = 0x3,
		.id_number = 0x7,
		.max_blk_erase_times = 0x1388,
		.access_high_freq = 0x1e,
		.random_cmd2_send_flag = 0x0,
		.random_addr_num = 0x0,
		.nand_real_page_size = 0x0,
	},
};
struct nand_manufacture mfr_tbl[] = {
	{ .id = 0x2c, .name = "micron", .dev = raw_micron, .ndev = ARRAY_SIZE(raw_micron) },
	{ .id = 0x01, .name = "spansion", .dev = raw_spansion, .ndev = ARRAY_SIZE(raw_spansion) },
	{ .id = 0x45, .name = "sandisk", .dev = raw_sandisk, .ndev = ARRAY_SIZE(raw_sandisk) },
	{ .id = 0x98, .name = "toshiba", .dev = raw_toshiba, .ndev = ARRAY_SIZE(raw_toshiba) },
	{ .id = 0xc8, .name = "esmt", .dev = raw_esmt, .ndev = ARRAY_SIZE(raw_esmt) },
	{ .id = 0x89, .name = "intel", .dev = raw_intel, .ndev = ARRAY_SIZE(raw_intel) },
	{ .id = 0xc8, .name = "giga", .dev = raw_giga, .ndev = ARRAY_SIZE(raw_giga) },
	{ .id = 0xc2, .name = "mxic", .dev = raw_mxic, .ndev = ARRAY_SIZE(raw_mxic) },
	{ .id = 0xec, .name = "foresee", .dev = raw_foresee, .ndev = ARRAY_SIZE(raw_foresee) },
	{ .id = 0xec, .name = "samsung", .dev = raw_samsung, .ndev = ARRAY_SIZE(raw_samsung) },
	{ .id = 0xad, .name = "hynix", .dev = raw_hynix, .ndev = ARRAY_SIZE(raw_hynix) },
};

struct sunxi_nand_flash_device *sunxi_search_idtab_by_id(unsigned char *id)
{
	int m = 0;
	int d = 0;
	int i = 0;
	u8 mfr_id = id[0];
	u8 dev_id = id[1];
	int cnt = 0;
	int match_id_cnt = 0;
	struct sunxi_nand_flash_device *match = NULL;

	for (m = 0; m < sizeof(mfr_tbl) / sizeof(struct nand_manufacture); m++) {
		struct nand_manufacture *mfr = &mfr_tbl[m];

		if (mfr_id == mfr->id) {
			for (d = 0; d < mfr->ndev; d++) {
				struct sunxi_nand_flash_device *dev =
					&mfr->dev[d];
				cnt = 0;
				if (dev_id == dev->dev_id) {
					for (i = 0; i < NAND_MAX_ID_LEN; i++) {
						if (dev->id[i] == id[i])
							cnt++;
						else
							break;
					}
					if (cnt > match_id_cnt) {
						if (cnt >= NAND_MIN_ID_LEN) {
							match = dev;
							match_id_cnt = cnt;
						}
					}
				}
			}
		}
	}

	return match;
}

struct sunxi_nand_flash_device *sunxi_search_idtab_by_name(char *mfr_name, char *device_name)
{
	int m = 0;
	int d = 0;
	struct sunxi_nand_flash_device *match = NULL;

	for (m = 0; m < sizeof(mfr_tbl) / sizeof(struct nand_manufacture); m++) {
		struct nand_manufacture *mfr = &mfr_tbl[m];
		if (!memcmp(mfr_name, mfr->name, strlen(mfr->name))) {
			for (d = 0; d < mfr->ndev; d++) {
				struct sunxi_nand_flash_device *dev =
					&mfr->dev[d];
				if (!memcmp(device_name, dev->name, strlen(dev->name))) {
					match = dev;
				}
			}
		}
	}

	return match;
}
/**
 * sunxi_search_id: search id is id table
 * @id : this id is read from device
 * */
struct sunxi_nand_flash_device *sunxi_search_id(struct nand_chip_info *nci,
		unsigned char *id)
{
	/*
	 *int m = 0;
	 *int d = 0;
	 *int i = 0;
	 *u8 mfr_id = id[0];
	 *u8 dev_id = id[1];
	 *int match_id_cnt = 0;
	 */
	struct sunxi_nand_flash_device *match = NULL;
	unsigned char *mem = NULL;
	char *device_name = NULL;
	int ret = 0;
	unsigned int nand_close_model_match = 0;

	mem = kmalloc(REVISION_FEATURES_BLOCK_INFO_PARAMETER_LEN, GFP_KERNEL);
	if (!mem) {
		RAWNAND_ERR("malloc buffer fail in %s %d\n", __func__, 1716 /* stock #860 */);
		goto err0;
	}

	memset(mem, 0, REVISION_FEATURES_BLOCK_INFO_PARAMETER_LEN);

	device_name = (char *)&mem[MCIRON_DEVICE_MODEL_OFFSET];

	match = sunxi_search_idtab_by_id(id);
	if (!match) {
		goto err0;
	}

	/*micron: different flash have the same id, find the id table by name(device model)*/
	if (match->mfr_id == NAND_MFR_MICRON) {
		if (match->operation_opt & NAND_FIND_ID_TAB_BY_NAME) {
			ret = of_property_read_u32(aw_ndfc.dev->of_node,
					"nand0_close_model_match", &nand_close_model_match);
			if (nand_close_model_match != 1) {
				ret = rawnand_read_parameter_page(nci, mem);
				if (ret != NAND_OP_TRUE) {
					RAWNAND_ERR("rawnand read parameter fail ret:%d\n", ret);
					goto err0;
				}

				RAWNAND_INFO("device model from flash:%s\n", device_name);
				match = sunxi_search_idtab_by_name(MICRON_NAME, device_name);
			}
		}
	}

	RAWNAND_INFO("match the device from idtab:%s\n", match ? match->name : "null");
err0:
	return match;
}

int generic_is_lsb_page(__u32 page_num) //0x00
{
	return 1;
}

int hynix20nm_is_lsb_page(__u32 page_num) //0x01
{
	struct nand_chip_info *nci = g_nsi->nci;

	//hynix 20nm
	if ((page_num == 0) || (page_num == 1))
		return 1;
	if ((page_num == nci->npi->page_cnt_per_blk - 2) || (page_num == nci->npi->page_cnt_per_blk - 1))
		return 0;
	if ((page_num % 4 == 2) || (page_num % 4 == 3))
		return 1;
	return 0;
}

int hynix26nm_is_lsb_page(__u32 page_num) //0x01
{
	struct nand_chip_info *nci = g_nsi->nci;

	//hynix 26nm
	if ((page_num == 0) || (page_num == 1))
		return 1;
	if ((page_num == nci->npi->page_cnt_per_blk - 2) || (page_num == nci->npi->page_cnt_per_blk - 1))
		return 0;
	if ((page_num % 4 == 2) || (page_num % 4 == 3))
		return 1;
	return 0;
}

int hynix16nm_is_lsb_page(__u32 page_num) //0x02
{
	__u32 pages_per_block;
	//	__u32 read_retry_type;
	struct nand_chip_info *nci = g_nsi->nci;

	pages_per_block = nci->page_cnt_per_blk;

	if (page_num == 0)
		return 1;
	if (page_num == pages_per_block - 1)
		return 0;
	if (page_num % 2 == 1)
		return 1;
	return 0;
}

int toshiba_is_lsb_page(__u32 page_num) //0x10
{
	struct nand_chip_info *nci = g_nsi->nci;

	//toshiba 2xnm 19nm 1ynm
	if (page_num == 0)
		return 1;
	if (page_num == nci->npi->page_cnt_per_blk - 1)
		return 0;
	if (page_num % 2 == 1)
		return 1;
	return 0;
}

int samsung_is_lsb_page(__u32 page_num) //0x20
{
	struct nand_chip_info *nci = g_nsi->nci;

	//(NAND_LSBPAGE_TYPE == 0x20) //samsung 25nm
	if (page_num == 0)
		return 1;
	if (page_num == nci->npi->page_cnt_per_blk - 1)
		return 0;
	if (page_num % 2 == 1)
		return 1;
	return 0;
}
int sandisk_is_lsb_page(__u32 page_num) //0x30
{
	struct nand_chip_info *nci = g_nsi->nci;

	//sandisk 2xnm 19nm 1ynm
	if (page_num == 0)
		return 1;
	if (page_num == nci->npi->page_cnt_per_blk - 1)
		return 0;
	if (page_num % 2 == 1)
		return 1;
	return 0;
}

int micron_0x40_is_lsb_page(__u32 page_num) // 20nm (29f64g08cbaba) 0x40
{
	struct nand_chip_info *nci = g_nsi->nci;

	if ((page_num == 0) || (page_num == 1))
		return 1;
	if ((page_num == nci->npi->page_cnt_per_blk - 2) || (page_num == nci->npi->page_cnt_per_blk - 1))
		return 0;
	if ((page_num % 4 == 2) || (page_num % 4 == 3))
		return 1;
	return 0;
}

int micron_0x41_is_lsb_page(__u32 page_num) //20nm (29f32g08cbada) 0x41
{
	struct nand_chip_info *nci = g_nsi->nci;

	//micron 20nm L83A L84A L84C L84D L85A L85C
	if ((page_num == 2) || (page_num == 3))
		return 1;
	if ((page_num == nci->npi->page_cnt_per_blk - 2) || (page_num == nci->npi->page_cnt_per_blk - 1))
		return 1;
	if ((page_num % 4 == 0) || (page_num % 4 == 1))
		return 1;
	return 0;
}

int micron_0x42_is_lsb_page(__u32 page_num) // 16nm l95b 0x42
{
	//	struct nand_chip_info *nci = g_nsi->nci;

	//micron 16nm l95b
	if ((page_num == 0) || (page_num == 1) || (page_num == 2) || (page_num == 3) || (page_num == 4) || (page_num == 5) || (page_num == 7) || (page_num == 8) || (page_num == 509))
		return 1;
	if ((page_num == 6) || (page_num == 508) || (page_num == 511))
		return 0;
	if ((page_num % 4 == 2) || (page_num % 4 == 3))
		return 1;
	return 0;
}

int micron_0x43_is_lsb_page(__u32 page_num) // L04A
{
	if ((page_num < 16) || ((page_num > 495) && (page_num < 512)))
		return 1;
	if (page_num % 2 == 0)
		return 1;
	if (page_num % 2 == 1)
		return 0;
	return 0;
}

int micron_0x44_is_lsb_page(__u32 page_num) // L06B
{
	if (page_num < 64)
		return 1;
	if (page_num % 2 == 0)
		return 0;
	if (page_num % 2 == 1)
		return 1;
	return 0;
}
lsb_page_t chose_lsb_func(__u32 no)
{
	no = (no << LSB_PAGE_POS);

	switch (no) {
	case GENERIC_LSB_PAGE:
		return generic_is_lsb_page;
	case HYNIX20_26NM_LSB_PAGE:
		return hynix20nm_is_lsb_page;
	case HYNIX16NM_LSB_PAGE:
		return hynix16nm_is_lsb_page;
	case TOSHIBA_LSB_PAGE:
		return toshiba_is_lsb_page;
	case SAMSUNG_LSB_PAGE:
		return samsung_is_lsb_page;
	case SANDISK_LSB_PAGE:
		return sandisk_is_lsb_page;
	case MICRON_0x40_LSB_PAGE:
		return micron_0x40_is_lsb_page;
	case MICRON_0x41_LSB_PAGE:
		return micron_0x41_is_lsb_page;
	case MICRON_0x42_LSB_PAGE:
		return micron_0x42_is_lsb_page;
	case MICRON_0x43_LSB_PAGE:
		return micron_0x43_is_lsb_page;
	case MICRON_0x44_LSB_PAGE:
		return micron_0x44_is_lsb_page;
	default:
		return generic_is_lsb_page;
	}
	return generic_is_lsb_page;
}

struct sunxi_nand_flash_device selected_id_tbl;

struct nfc_init_ddr_info def_ddr_info[] = {
	////////////////////////////
	{
		0, //en_dqs_c;
		0, //en_re_c;
		0, //odt;
		0, //en_ext_verf;
		0, //dout_re_warmup_cycle;
		0, //din_dqs_warmup_cycle;
		0, //output_driver_strength;
		0, //rb_pull_down_strength;
	},
	////////////////////////////
	{
		0, //en_dqs_c;
		0, //en_re_c;
		0, //odt;
		0, //en_ext_verf;
		0, //dout_re_warmup_cycle;
		0, //din_dqs_warmup_cycle;
		2, //output_driver_strength;
		2, //rb_pull_down_strength;
	},
	////////////////////////////
};


