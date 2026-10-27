obj-$(CONFIG_SUNXI_NAND) += nand.o

ifdef CONFIG_ARM64
ifeq ($(TRIMUI_NFTL_FALLBACK_ASM),1)
# Retained binary translation for focused stock-object comparisons only.
nftl = nftl_v8.S
obj-$(CONFIG_SUNXI_NAND) += nftl_v8.o
else
# Use the recovered C implementation with the unmodified stock .config.
obj-$(CONFIG_SUNXI_NAND) += nftl_tables_recovered.o
obj-$(CONFIG_SUNXI_NAND) += nftl_bss_recovered.o
obj-$(CONFIG_SUNXI_NAND) += nftl_rodata_recovered.o
obj-$(CONFIG_SUNXI_NAND) += nftl_recovered_merged.o
obj-$(CONFIG_SUNXI_NAND) += nftl_cache_init_gcc53.o
obj-$(CONFIG_SUNXI_NAND) += nftl_sector_cache_init_gcc53.o
obj-$(CONFIG_SUNXI_NAND) += nftl_invalid_page_incr_gcc53.o
obj-$(CONFIG_SUNXI_NAND) += nftl_micron_init_gcc53.o
obj-$(CONFIG_SUNXI_NAND) += nftl_copy_page_gcc53.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_dump_buf.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_nftl_read.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_nftl_write.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_nftl_discard.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_nand_discard.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_samsung_readretry_init.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_toshiba_readretry_init.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_sandisk_A19_readretry_init.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_get_logic_page_map.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_nftl_support_boot_check_crc_from_dts.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_nftl_initialize.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_nftl_start.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_gc_all_enhance.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_out_phy_block_from_free_list.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_out_phy_block_from_free_list_by_block.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_get_cold_logic_page.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_fill_no_use_data_to_current_block.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_set_oob_logic_page.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix26nm_getdefaultparam.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix16nm_set_readretry.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_sandisk_readretry_init.o
nftl_special_names := samsung_special_init micron_special_init toshiba_special_init sandisk_special_init sandisk_A19_special_init
nftl_special_objects := $(addprefix nftl_gcc53_,$(addsuffix .o,$(nftl_special_names)))
obj-$(CONFIG_SUNXI_NAND) += $(nftl_special_objects)
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_nand_dbg_phy_read.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_nand_dbg_zone_phy_read.o
nftl_hynix_special_names := hynix16nm_special_init hynix20nm_special_init hynix26nm_special_init
nftl_hynix_special_objects := $(addprefix nftl_gcc53_,$(addsuffix .o,$(nftl_hynix_special_names)))
obj-$(CONFIG_SUNXI_NAND) += $(nftl_hynix_special_objects)
nftl_small_names := hynix16nm_readretry_init malloc_logic_page_map prio_gc_one put_phy_block_to_free_list
nftl_small_objects := $(addprefix nftl_gcc53_,$(addsuffix .o,$(nftl_small_names)))
obj-$(CONFIG_SUNXI_NAND) += $(nftl_small_objects)
nftl_hynix_end_names := hynix16nm_read_page_end hynix20nm_read_page_end hynix26nm_read_page_end
nftl_hynix_end_objects := $(addprefix nftl_gcc53_,$(addsuffix .o,$(nftl_hynix_end_names)))
obj-$(CONFIG_SUNXI_NAND) += $(nftl_hynix_end_objects)
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_read_reclaim.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_first_scan.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix16nm_major_check_byte.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix16nm_get_special_info.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix20nm_init_readretry_param.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix20nm_default_param.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_get_used_page_num.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_get_used_page_num_no_crosstalk.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_gc_info.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_clean_zone.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_zone_param.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_static_wear_check.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_write_error.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix16nm_init_readretry_param.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix26nm_set_readretry.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_torture_block_and_mark.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_build_zone_list_first.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_toshiba_set_readretry.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53__nftl_nand_read_logic_page.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_recover_logic_page_mapping.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_power_on_gc.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_check_cross_talk.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_init_nrc.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_gc_all_base_on_free_blks.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_done_static_wear_leveling.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_is_no_use_device.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_garbage_collect.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_panic_backup_block.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_sandisk_first_burn.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_get_empty_cahce_node.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_recover_block_phy_page_mapping.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_mark_free_block.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_sandisk_a19_read_page_end.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_sandisk_set_readretry.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_adjust_free_list.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_erase_block.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_flush_write_cache_to_nand.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_write_phy_page_map_to_current_block.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_phy_write.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_logic_write.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_scan_blocks.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_check_invalid_page_right.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_sandisk_read_page_end.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_current_block_ecc_error.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_block_last_page_ecc_error.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix16nm_write_page_FF.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix20nm_vender_get_param_otp_hynix.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix20nm_getotpparam.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix16nm_get_otp_param.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix16nm_cfg.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_recover_phy_page_mapping.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_get_spare_data.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_crc16_pair.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_panic_flags.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_check_bit_nums.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_min_invalid_erase.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_list_nine.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix_lsb_init_pair.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix_lsb_enable_pair.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix_default_pair.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix20_retry.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix20_init.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix26_init.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_panic_write.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_panic_recover.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_cache_exit.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_smart_init.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_eval_order_three.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_eval_cache_three.o
endif
CFLAGS_nftl_recovered.o += -O0 -fno-builtin -fno-pic -mcmodel=small -DTRIMUI_NFTL_GCC7_PASS
# The stock NFTL object identifies Linaro GCC 5.3.1 (2016.05). These
# recovered bodies need its field-address code generation; the rest of NFTL
# remains on the established compiler to retain previously matched bodies.
TRIMUI_NFTL_GCC53 ?= $(srctree)/../../tools/gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
$(obj)/nftl_cache_init_gcc53.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nftl_cache_init --
$(obj)/nftl_sector_cache_init_gcc53.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nftl_sector_cache_init --
$(obj)/nftl_invalid_page_incr_gcc53.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) phy_block_from_invalid_page_incr --
$(obj)/nftl_micron_init_gcc53.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) micron_readretry_init --
$(obj)/nftl_copy_page_gcc53.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) _nftl_nand_copy_page --
$(obj)/nftl_gcc53_dump_buf.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) dump_buf --
$(obj)/nftl_gcc53_nftl_read.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nftl_read --
$(obj)/nftl_gcc53_nftl_write.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nftl_write --
$(obj)/nftl_gcc53_nftl_discard.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nftl_discard --
$(obj)/nftl_gcc53_nand_discard.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nand_discard --
$(obj)/nftl_gcc53_samsung_readretry_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) samsung_readretry_init --
$(obj)/nftl_gcc53_toshiba_readretry_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) toshiba_readretry_init --
$(obj)/nftl_gcc53_sandisk_A19_readretry_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) sandisk_A19_readretry_init --
$(obj)/nftl_gcc53_get_logic_page_map.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) get_logic_page_map --
$(obj)/nftl_gcc53_nftl_support_boot_check_crc_from_dts.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nftl_support_boot_check_crc_from_dts --
$(obj)/nftl_gcc53_nftl_initialize.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nftl_initialize:nftl_version --
$(obj)/nftl_gcc53_nftl_start.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nftl_start --
$(obj)/nftl_gcc53_gc_all_enhance.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) gc_all_enhance --
$(obj)/nftl_gcc53_out_phy_block_from_free_list.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) out_phy_block_from_free_list --
$(obj)/nftl_gcc53_out_phy_block_from_free_list_by_block.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) out_phy_block_from_free_list_by_block --
$(obj)/nftl_gcc53_get_cold_logic_page.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) get_cold_logic_page --
$(obj)/nftl_gcc53_fill_no_use_data_to_current_block.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) fill_no_use_data_to_current_block --
$(obj)/nftl_gcc53_set_oob_logic_page.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) set_oob_logic_page --
$(obj)/nftl_gcc53_hynix26nm_getdefaultparam.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix26nm_getdefaultparam --
$(obj)/nftl_gcc53_hynix16nm_set_readretry.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix16nm_set_readretry --
$(obj)/nftl_gcc53_sandisk_readretry_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) sandisk_readretry_init --
$(obj)/nftl_gcc53_samsung_special_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) samsung_special_init --
$(obj)/nftl_gcc53_micron_special_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) micron_special_init --
$(obj)/nftl_gcc53_toshiba_special_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) toshiba_special_init --
$(obj)/nftl_gcc53_sandisk_special_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) sandisk_special_init --
$(obj)/nftl_gcc53_sandisk_A19_special_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) sandisk_A19_special_init --
$(obj)/nftl_gcc53_nand_dbg_phy_read.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nand_dbg_phy_read --
$(obj)/nftl_gcc53_nand_dbg_zone_phy_read.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nand_dbg_zone_phy_read --
$(obj)/nftl_gcc53_hynix16nm_special_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix16nm_special_init --
$(obj)/nftl_gcc53_hynix20nm_special_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix20nm_special_init --
$(obj)/nftl_gcc53_hynix26nm_special_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix26nm_special_init --
$(obj)/nftl_gcc53_hynix16nm_readretry_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix16nm_readretry_init --
$(obj)/nftl_gcc53_malloc_logic_page_map.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) malloc_logic_page_map --
$(obj)/nftl_gcc53_prio_gc_one.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) prio_gc_one --
$(obj)/nftl_gcc53_put_phy_block_to_free_list.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) put_phy_block_to_free_list --
$(obj)/nftl_gcc53_hynix16nm_read_page_end.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix16nm_read_page_end --
$(obj)/nftl_gcc53_hynix20nm_read_page_end.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix20nm_read_page_end --
$(obj)/nftl_gcc53_hynix26nm_read_page_end.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix26nm_read_page_end --
$(obj)/nftl_gcc53_read_reclaim.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) read_reclaim --
$(obj)/nftl_gcc53_first_scan.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) init_zone_after_first_scan --
$(obj)/nftl_gcc53_hynix16nm_major_check_byte.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix16nm_major_check_byte --
$(obj)/nftl_gcc53_hynix16nm_get_special_info.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix16nm_get_special_info --
$(obj)/nftl_gcc53_hynix20nm_init_readretry_param.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix20nm_init_readretry_param --
$(obj)/nftl_gcc53_hynix20nm_default_param.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix20nm_set_default_param --
$(obj)/nftl_gcc53_get_used_page_num.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) get_used_page_num --
$(obj)/nftl_gcc53_get_used_page_num_no_crosstalk.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) get_used_page_num_no_crosstalk --
$(obj)/nftl_gcc53_gc_info.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nftl_get_gc_info --
$(obj)/nftl_gcc53_clean_zone.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nand_clean_zone_table2 --
$(obj)/nftl_gcc53_zone_param.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) zone_param_init --
$(obj)/nftl_gcc53_static_wear_check.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) check_static_wear_leveling --
$(obj)/nftl_gcc53_write_error.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) do_write_error --
$(obj)/nftl_gcc53_hynix16nm_init_readretry_param.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix16nm_init_readretry_param --
$(obj)/nftl_gcc53_hynix26nm_set_readretry.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix26nm_set_readretry --
$(obj)/nftl_gcc53_torture_block_and_mark.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) torture_block_and_mark --
$(obj)/nftl_gcc53_build_zone_list_first.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) build_zone_list_first --
$(obj)/nftl_gcc53_toshiba_set_readretry.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) toshiba_set_readretry --
$(obj)/nftl_gcc53__nftl_nand_read_logic_page.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) _nftl_nand_read_logic_page --
$(obj)/nftl_gcc53_recover_logic_page_mapping.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) recover_logic_page_mapping --
$(obj)/nftl_gcc53_power_on_gc.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) power_on_gc --
$(obj)/nftl_gcc53_check_cross_talk.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) check_cross_talk --
$(obj)/nftl_gcc53_init_nrc.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) init_nrc --
$(obj)/nftl_gcc53_gc_all_base_on_free_blks.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) gc_all_base_on_free_blks --
$(obj)/nftl_gcc53_done_static_wear_leveling.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) done_static_wear_leveling --
$(obj)/nftl_gcc53_is_no_use_device.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) is_no_use_device --
$(obj)/nftl_gcc53_garbage_collect.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) garbage_collect --
$(obj)/nftl_gcc53_panic_backup_block.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) panic_backup_block --
$(obj)/nftl_gcc53_sandisk_first_burn.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) sandisk_A19_check_bad_block_first_burn --
$(obj)/nftl_gcc53_get_empty_cahce_node.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) get_empty_cahce_node --
$(obj)/nftl_gcc53_recover_block_phy_page_mapping.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) recover_block_phy_page_mapping --
$(obj)/nftl_gcc53_mark_free_block.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) mark_free_block --
$(obj)/nftl_gcc53_sandisk_a19_read_page_end.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) sandisk_A19_read_page_end --
$(obj)/nftl_gcc53_sandisk_set_readretry.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) sandisk_set_readretry --
$(obj)/nftl_gcc53_adjust_free_list.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) adjust_free_list --
$(obj)/nftl_gcc53_erase_block.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) erase_block --
$(obj)/nftl_gcc53_flush_write_cache_to_nand.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) flush_write_cache_to_nand --
$(obj)/nftl_gcc53_write_phy_page_map_to_current_block.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) write_phy_page_map_to_current_block --
$(obj)/nftl_gcc53_phy_write.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) write_phy_data_to_current_block --
$(obj)/nftl_gcc53_logic_write.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) write_logic_data_to_current_block --
$(obj)/nftl_gcc53_scan_blocks.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) first_scan_all_blocks --
$(obj)/nftl_gcc53_check_invalid_page_right.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) check_invalid_page_right --
$(obj)/nftl_gcc53_sandisk_read_page_end.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) sandisk_read_page_end --
$(obj)/nftl_gcc53_current_block_ecc_error.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) current_block_ecc_error --
$(obj)/nftl_gcc53_block_last_page_ecc_error.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) block_last_page_ecc_error --
$(obj)/nftl_gcc53_hynix16nm_write_page_FF.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix16nm_write_page_FF --
$(obj)/nftl_gcc53_hynix20nm_vender_get_param_otp_hynix.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix20nm_vender_get_param_otp_hynix --
$(obj)/nftl_gcc53_hynix20nm_getotpparam.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix20nm_getotpparam --
$(obj)/nftl_gcc53_hynix16nm_get_otp_param.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix16nm_get_otp_param --
$(obj)/nftl_gcc53_hynix16nm_cfg.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix16nm_get_read_retry_cfg --
$(obj)/nftl_gcc53_recover_phy_page_mapping.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) recover_phy_page_mapping --
$(obj)/nftl_gcc53_get_spare_data.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) get_spare_data --
$(obj)/nftl_gcc53_crc16_pair.o: override CC := $(TRIMUI_NFTL_GCC53)
$(obj)/nftl_gcc53_crc_wrong.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) crc_wrong --
$(obj)/nftl_gcc53_panic_flags.o: override CC := $(TRIMUI_NFTL_GCC53)
$(obj)/nftl_gcc53_check_bit_nums.o: override CC := $(TRIMUI_NFTL_GCC53)
$(obj)/nftl_gcc53_min_invalid_erase.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) get_block_min_erase_time_from_invalid_list --
$(obj)/nftl_gcc53_list_nine.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) add_to_cache_read_list_tail,add_to_cache_write_list_tail,add_to_gc_list_tail,add_to_sector_cache_write_list_tail,del_from_cache_read_list,del_from_cache_write_list,del_from_gc_list,del_from_sector_cache_write_list,is_current_blcok,get_empty_gc_node,get_empty_sector_cahce_node,set_spare_data,nftl_cache_clean --
$(obj)/nftl_gcc53_hynix_lsb_init_pair.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix20nm_lsb_init,hynix26nm_lsb_init --
$(obj)/nftl_gcc53_hynix_lsb_enable_pair.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix20nm_lsb_enable,hynix26nm_lsb_enable --
$(obj)/nftl_gcc53_hynix_default_pair.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix16nm_set_default_param,hynix26nm_setdefaultparam --
$(obj)/nftl_gcc53_hynix20_retry.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix20nm_set_readretry --
$(obj)/nftl_gcc53_hynix20_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix20nm_readretry_init --
$(obj)/nftl_gcc53_hynix26_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix26nm_readretry_init --
$(obj)/nftl_gcc53_panic_write.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) panic_write --
$(obj)/nftl_gcc53_panic_recover.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) panic_recover_data --
$(obj)/nftl_gcc53_cache_exit.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nftl_cache_exit --
$(obj)/nftl_gcc53_smart_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) init_smart_info --
$(obj)/nftl_gcc53_eval_order_three.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) new_block_init_for_write,get_vaild_blocks,nand_dbg_single_phy_erase --
$(obj)/nftl_gcc53_eval_cache_three.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nand_cache_read,nand_cache_write,nand_op --
nftl_gcc53_objects := $(addprefix $(obj)/,nftl_cache_init_gcc53.o nftl_sector_cache_init_gcc53.o nftl_invalid_page_incr_gcc53.o nftl_micron_init_gcc53.o nftl_copy_page_gcc53.o nftl_gcc53_dump_buf.o nftl_gcc53_nftl_read.o nftl_gcc53_nftl_write.o nftl_gcc53_nftl_discard.o nftl_gcc53_nand_discard.o nftl_gcc53_samsung_readretry_init.o nftl_gcc53_toshiba_readretry_init.o nftl_gcc53_sandisk_A19_readretry_init.o nftl_gcc53_get_logic_page_map.o nftl_gcc53_nftl_support_boot_check_crc_from_dts.o nftl_gcc53_gc_all_enhance.o nftl_gcc53_out_phy_block_from_free_list.o nftl_gcc53_out_phy_block_from_free_list_by_block.o nftl_gcc53_get_cold_logic_page.o nftl_gcc53_fill_no_use_data_to_current_block.o nftl_gcc53_set_oob_logic_page.o nftl_gcc53_hynix26nm_getdefaultparam.o nftl_gcc53_hynix16nm_set_readretry.o nftl_gcc53_sandisk_readretry_init.o)
nftl_gcc53_objects += $(addprefix $(obj)/,$(nftl_special_objects))
nftl_gcc53_objects += $(addprefix $(obj)/,nftl_gcc53_nand_dbg_phy_read.o nftl_gcc53_nand_dbg_zone_phy_read.o)
nftl_gcc53_objects += $(addprefix $(obj)/,$(nftl_hynix_special_objects))
nftl_gcc53_objects += $(addprefix $(obj)/,$(nftl_small_objects))
nftl_gcc53_objects += $(addprefix $(obj)/,$(nftl_hynix_end_objects))
nftl_gcc53_objects += $(obj)/nftl_gcc53_read_reclaim.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_first_scan.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix16nm_major_check_byte.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix16nm_get_special_info.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix20nm_init_readretry_param.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix20nm_default_param.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_get_used_page_num.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_get_used_page_num_no_crosstalk.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_gc_info.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_clean_zone.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_zone_param.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_static_wear_check.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_write_error.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix16nm_init_readretry_param.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix26nm_set_readretry.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_torture_block_and_mark.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_build_zone_list_first.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_toshiba_set_readretry.o
nftl_gcc53_objects += $(addprefix $(obj)/,nftl_gcc53__nftl_nand_read_logic_page.o nftl_gcc53_recover_logic_page_mapping.o)
nftl_gcc53_objects += $(obj)/nftl_gcc53_power_on_gc.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_check_cross_talk.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_init_nrc.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_gc_all_base_on_free_blks.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_done_static_wear_leveling.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_is_no_use_device.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_garbage_collect.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_panic_backup_block.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_sandisk_first_burn.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_get_empty_cahce_node.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_recover_block_phy_page_mapping.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_mark_free_block.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_sandisk_a19_read_page_end.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_sandisk_set_readretry.o
nftl_gcc53_objects += $(addprefix $(obj)/,nftl_gcc53_adjust_free_list.o nftl_gcc53_erase_block.o)
nftl_gcc53_objects += $(obj)/nftl_gcc53_flush_write_cache_to_nand.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_write_phy_page_map_to_current_block.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_check_invalid_page_right.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_sandisk_read_page_end.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_current_block_ecc_error.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_block_last_page_ecc_error.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix16nm_write_page_FF.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix20nm_vender_get_param_otp_hynix.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix20nm_getotpparam.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix16nm_get_otp_param.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix16nm_cfg.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_recover_phy_page_mapping.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_get_spare_data.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_crc16_pair.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_crc_wrong.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_panic_flags.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_check_bit_nums.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_min_invalid_erase.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_list_nine.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix_lsb_init_pair.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix_lsb_enable_pair.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix_default_pair.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix20_retry.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix20_init.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix26_init.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_panic_write.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_panic_recover.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_cache_exit.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_smart_init.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_eval_order_three.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_eval_cache_three.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_nftl_initialize.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_nftl_start.o
$(nftl_gcc53_objects): override NOSTDINC_FLAGS := -nostdinc -isystem $(shell $(TRIMUI_NFTL_GCC53) -print-file-name=include)
$(nftl_gcc53_objects): $(srctree)/scripts/trimui_nftl_select_cc.py
quiet_cmd_nftl_crc_merge = LD      $@
      cmd_nftl_crc_merge = $(LD) -r -o $@ $(filter %.o,$^) && $(OBJCOPY) --localize-symbol=crc_wrong $@
$(obj)/nftl_recovered_merged.o: $(obj)/nftl_recovered.o $(obj)/nftl_gcc53_crc_wrong.o FORCE
	$(call if_changed,nftl_crc_merge)
CFLAGS_nftl_cache_init_gcc53.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_sector_cache_init_gcc53.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_invalid_page_incr_gcc53.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_micron_init_gcc53.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_copy_page_gcc53.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_dump_buf.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_nftl_read.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_nftl_write.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_nftl_discard.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_samsung_readretry_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_toshiba_readretry_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_sandisk_A19_readretry_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_get_logic_page_map.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_nftl_support_boot_check_crc_from_dts.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_nftl_initialize.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_nftl_start.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -fno-stack-protector -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_gc_all_enhance.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_out_phy_block_from_free_list.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_out_phy_block_from_free_list_by_block.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_get_cold_logic_page.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_fill_no_use_data_to_current_block.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_set_oob_logic_page.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix26nm_getdefaultparam.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix16nm_set_readretry.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_sandisk_readretry_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_samsung_special_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_micron_special_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_toshiba_special_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_sandisk_special_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_sandisk_A19_special_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_nand_dbg_phy_read.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_nand_dbg_zone_phy_read.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix16nm_special_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix20nm_special_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix26nm_special_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix16nm_readretry_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_malloc_logic_page_map.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_prio_gc_one.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_put_phy_block_to_free_list.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix16nm_read_page_end.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix20nm_read_page_end.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix26nm_read_page_end.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_read_reclaim.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_first_scan.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix16nm_major_check_byte.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix16nm_get_special_info.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix20nm_init_readretry_param.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_HYNIX20_PARAM_GCC53_BUILD
CFLAGS_nftl_gcc53_hynix20nm_default_param.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_GCC7_PASS -DTRIMUI_NFTL_HYNIX20_DEFAULT_GCC53_BUILD
CFLAGS_nftl_gcc53_get_used_page_num.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_GCC7_PASS -DTRIMUI_NFTL_USED_PAGE_GCC53_BUILD
CFLAGS_nftl_gcc53_get_used_page_num_no_crosstalk.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_GCC7_PASS -DTRIMUI_NFTL_NO_CROSSTALK_GCC53_BUILD
CFLAGS_nftl_gcc53_gc_info.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_clean_zone.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_zone_param.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_ZONE_PARAM_GCC53_BUILD
CFLAGS_nftl_gcc53_static_wear_check.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_STATIC_CHECK_GCC53_BUILD
CFLAGS_nftl_gcc53_write_error.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_GCC7_PASS -DTRIMUI_NFTL_WRITE_ERROR_GCC53_BUILD
CFLAGS_nftl_gcc53_hynix16nm_init_readretry_param.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_HYNIX16_PARAM_GCC53_BUILD
CFLAGS_nftl_gcc53_hynix26nm_set_readretry.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_HYNIX26_RETRY_GCC53_BUILD
CFLAGS_nftl_gcc53_torture_block_and_mark.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_TORTURE_GCC53_BUILD
CFLAGS_nftl_gcc53_build_zone_list_first.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_FIRST_LIST_GCC53_BUILD
CFLAGS_nftl_gcc53_toshiba_set_readretry.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_TOSHIBA_RETRY_GCC53_BUILD
CFLAGS_nftl_gcc53_nand_discard.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53__nftl_nand_read_logic_page.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_recover_logic_page_mapping.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_power_on_gc.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_check_cross_talk.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_init_nrc.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_gc_all_base_on_free_blks.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_done_static_wear_leveling.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_is_no_use_device.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_garbage_collect.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_panic_backup_block.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_sandisk_first_burn.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_get_empty_cahce_node.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_recover_block_phy_page_mapping.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_mark_free_block.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_sandisk_a19_read_page_end.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_sandisk_set_readretry.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_adjust_free_list.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_erase_block.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_flush_write_cache_to_nand.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_write_phy_page_map_to_current_block.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_phy_write.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -fno-stack-protector -ffunction-sections -fdata-sections -DTRIMUI_NFTL_GCC7_PASS
CFLAGS_nftl_gcc53_logic_write.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_GCC7_PASS
CFLAGS_nftl_gcc53_scan_blocks.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_GCC7_PASS
CFLAGS_nftl_gcc53_check_invalid_page_right.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_sandisk_read_page_end.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_current_block_ecc_error.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_block_last_page_ecc_error.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix16nm_write_page_FF.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix20nm_vender_get_param_otp_hynix.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix20nm_getotpparam.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix16nm_get_otp_param.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix16nm_cfg.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_recover_phy_page_mapping.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_get_spare_data.o += -O0 -fno-builtin -fno-pic -mcmodel=small -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_crc16_pair.o += -O0 -fno-builtin -fno-pic -mcmodel=small
CFLAGS_nftl_gcc53_crc_wrong.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_panic_flags.o += -O0 -fno-builtin -fno-pic -mcmodel=small
CFLAGS_nftl_gcc53_check_bit_nums.o += -O0 -fno-builtin -fno-pic -mcmodel=small
CFLAGS_nftl_gcc53_min_invalid_erase.o += -O0 -fno-builtin -fno-pic -mcmodel=small -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_list_nine.o += -O0 -fno-builtin -fno-pic -mcmodel=small -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix_lsb_init_pair.o += -O0 -fno-builtin -fno-pic -mcmodel=small -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix_lsb_enable_pair.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_HYNIX_LSB_ENABLE_GCC53_BUILD
CFLAGS_nftl_gcc53_hynix_default_pair.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_HYNIX_DEFAULT_GCC53_BUILD
CFLAGS_nftl_gcc53_hynix20_retry.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_HYNIX20_RETRY_GCC53_BUILD
CFLAGS_nftl_gcc53_hynix20_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_HYNIX20_INIT_GCC53_BUILD
CFLAGS_nftl_gcc53_hynix26_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_HYNIX26_INIT_GCC53_BUILD
CFLAGS_nftl_gcc53_panic_write.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_COPY_PAGE_GCC53_BUILD -DTRIMUI_NFTL_OOB_LOGIC_GCC53_BUILD -DTRIMUI_NFTL_PANIC_WRITE_GCC53_BUILD
CFLAGS_nftl_gcc53_panic_recover.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_PANIC_RECOVER_GCC53_BUILD
CFLAGS_nftl_gcc53_cache_exit.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_CACHE_EXIT_GCC53_BUILD
CFLAGS_nftl_gcc53_smart_init.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_SMART_INIT_GCC53_BUILD
CFLAGS_nftl_gcc53_eval_order_three.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_COPY_PAGE_GCC53_BUILD -DTRIMUI_NFTL_EVAL_ORDER_GCC53_BUILD
CFLAGS_nftl_gcc53_eval_cache_three.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_EVAL_CACHE_GCC53_BUILD
CFLAGS_nftl_tables_recovered.o += -O0
CFLAGS_nftl_bss_recovered.o += -O0
CFLAGS_nftl_rodata_recovered.o += -O0
endif

ifdef CONFIG_ARM
nftl = nftl_v7.S
obj-$(CONFIG_SUNXI_NAND) += nftl_v7.o
endif


ifdef CONFIG_ARCH_SUN8IW18
	ndfc-v=phy-nand/rawnand/controller/ndfc_v2px.c
else
	ndfc-v=phy-nand/rawnand/controller/ndfc_v1px.c
endif

ifdef CONFIG_SUNXI_RAWNAND
	BUILD_RAWNAND=y
else
	BUILD_SPINAND=y
endif

ifneq ($(wildcard $(src)/lib),)
$(obj)/$(nftl) : FORCE
	$(MAKE) -C $(obj)/.. \
		LICHEE_KDIR="$(shell pwd)" \
		build
endif

nand-y += nand_interface.o
nand-y += phy2nftl.o
nand-y += nfd/nand_ota_burn.o
nand-y += nfd/nand_base.o
nand-y += nfd/nand_blk.o
nand-y += nfd/nand_dev.o
nand-y += nfd/nand_class.o
nand-y += nfd/nand_test.o
nand-y += nfd/nand_osal_for_linux.o
ifneq ($(CONFIG_PSTORE_BLK),)
nand-y += nfd/nand_panic.o
endif
nand-y += phy-nand/nand_boot.o
nand-y += phy-nand/nand-partition/build_nand_partition.o
nand-y += phy-nand/nand-partition/build_phy_partition.o
nand-y += phy-nand/nand-partition/nand_info_init.o
nand-y += phy-nand/nand-partition2/build_phy_partition_v2.o
nand-y += phy-nand/nand-partition2/nand_info_init_v2.o
nand-y += phy-nand/nand_physic_interface.o
nand-y += phy-nand/nand_secure_storage.o
nand-y += phy-nand/version.o
nand-y += phy-nand/nand_weak.o
nand-y += phy-nand/nand_weak.o
nand-$(BUILD_RAWNAND) += phy-nand/rawnand/controller/ndfc_base.o
nand-$(BUILD_RAWNAND) += phy-nand/rawnand/controller/ndfc_timings.o
nand-$(BUILD_RAWNAND) += phy-nand/rawnand/controller/ndfc_v1px.o
nand-$(BUILD_RAWNAND) += $(basename $(ndfc-v)).o
nand-$(BUILD_RAWNAND) += phy-nand/rawnand/rawnand_base.o
nand-$(BUILD_RAWNAND) += phy-nand/rawnand/rawnand_boot1.o
nand-$(BUILD_RAWNAND) += phy-nand/rawnand/rawnand_boot0.o
nand-$(BUILD_RAWNAND) += phy-nand/rawnand/rawnand_cfg.o
nand-$(BUILD_RAWNAND) += phy-nand/rawnand/rawnand_ops.o
nand-$(BUILD_RAWNAND) += phy-nand/rawnand/rawnand_chip.o
nand-$(BUILD_RAWNAND) += phy-nand/rawnand/rawnand_debug.o
nand-$(BUILD_RAWNAND) += phy-nand/rawnand/rawnand_ids.o
nand-$(BUILD_RAWNAND) += phy-nand/rawnand/rawnand_readretry.o
nand-$(BUILD_RAWNAND) += phy-nand/rawnand/rawnand_timings.o
nand-$(BUILD_SPINAND) += phy-nand/spinand/spinand_boot.o
nand-$(BUILD_SPINAND) += phy-nand/spinand/spinand.o
#nand-y += $(libnand)
