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
obj-$(CONFIG_SUNXI_NAND) += nftl_recovered.o
obj-$(CONFIG_SUNXI_NAND) += nftl_cache_init_gcc53.o
obj-$(CONFIG_SUNXI_NAND) += nftl_sector_cache_init_gcc53.o
obj-$(CONFIG_SUNXI_NAND) += nftl_invalid_page_incr_gcc53.o
obj-$(CONFIG_SUNXI_NAND) += nftl_micron_init_gcc53.o
obj-$(CONFIG_SUNXI_NAND) += nftl_copy_page_gcc53.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_dump_buf.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_nftl_read.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_nftl_write.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_nftl_discard.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_samsung_readretry_init.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_toshiba_readretry_init.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_sandisk_A19_readretry_init.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_get_logic_page_map.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_nftl_support_boot_check_crc_from_dts.o
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
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53__nftl_nand_read_logic_page.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_recover_logic_page_mapping.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_adjust_free_list.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_erase_block.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_flush_write_cache_to_nand.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_write_phy_page_map_to_current_block.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix16nm_write_page_FF.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix20nm_vender_get_param_otp_hynix.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_recover_phy_page_mapping.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_get_spare_data.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_crc16_pair.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_panic_flags.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_check_bit_nums.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_min_invalid_erase.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_list_nine.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_hynix_lsb_init_pair.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_panic_write.o
obj-$(CONFIG_SUNXI_NAND) += nftl_gcc53_eval_order_three.o
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
$(obj)/nftl_gcc53_samsung_readretry_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) samsung_readretry_init --
$(obj)/nftl_gcc53_toshiba_readretry_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) toshiba_readretry_init --
$(obj)/nftl_gcc53_sandisk_A19_readretry_init.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) sandisk_A19_readretry_init --
$(obj)/nftl_gcc53_get_logic_page_map.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) get_logic_page_map --
$(obj)/nftl_gcc53_nftl_support_boot_check_crc_from_dts.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nftl_support_boot_check_crc_from_dts --
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
$(obj)/nftl_gcc53__nftl_nand_read_logic_page.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) _nftl_nand_read_logic_page --
$(obj)/nftl_gcc53_recover_logic_page_mapping.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) recover_logic_page_mapping --
$(obj)/nftl_gcc53_adjust_free_list.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) adjust_free_list --
$(obj)/nftl_gcc53_erase_block.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) erase_block --
$(obj)/nftl_gcc53_flush_write_cache_to_nand.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) flush_write_cache_to_nand --
$(obj)/nftl_gcc53_write_phy_page_map_to_current_block.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) write_phy_page_map_to_current_block --
$(obj)/nftl_gcc53_hynix16nm_write_page_FF.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix16nm_write_page_FF --
$(obj)/nftl_gcc53_hynix20nm_vender_get_param_otp_hynix.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix20nm_vender_get_param_otp_hynix --
$(obj)/nftl_gcc53_recover_phy_page_mapping.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) recover_phy_page_mapping --
$(obj)/nftl_gcc53_get_spare_data.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) get_spare_data --
$(obj)/nftl_gcc53_crc16_pair.o: override CC := $(TRIMUI_NFTL_GCC53)
$(obj)/nftl_gcc53_panic_flags.o: override CC := $(TRIMUI_NFTL_GCC53)
$(obj)/nftl_gcc53_check_bit_nums.o: override CC := $(TRIMUI_NFTL_GCC53)
$(obj)/nftl_gcc53_min_invalid_erase.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) get_block_min_erase_time_from_invalid_list --
$(obj)/nftl_gcc53_list_nine.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) add_to_cache_read_list_tail,add_to_cache_write_list_tail,add_to_gc_list_tail,add_to_sector_cache_write_list_tail,del_from_cache_read_list,del_from_cache_write_list,del_from_gc_list,del_from_sector_cache_write_list,is_current_blcok --
$(obj)/nftl_gcc53_hynix_lsb_init_pair.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) hynix20nm_lsb_init,hynix26nm_lsb_init --
$(obj)/nftl_gcc53_panic_write.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) panic_write --
$(obj)/nftl_gcc53_eval_order_three.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) new_block_init_for_write,get_vaild_blocks,nand_dbg_single_phy_erase --
nftl_gcc53_objects := $(addprefix $(obj)/,nftl_cache_init_gcc53.o nftl_sector_cache_init_gcc53.o nftl_invalid_page_incr_gcc53.o nftl_micron_init_gcc53.o nftl_copy_page_gcc53.o nftl_gcc53_dump_buf.o nftl_gcc53_nftl_read.o nftl_gcc53_nftl_write.o nftl_gcc53_nftl_discard.o nftl_gcc53_samsung_readretry_init.o nftl_gcc53_toshiba_readretry_init.o nftl_gcc53_sandisk_A19_readretry_init.o nftl_gcc53_get_logic_page_map.o nftl_gcc53_nftl_support_boot_check_crc_from_dts.o nftl_gcc53_gc_all_enhance.o nftl_gcc53_out_phy_block_from_free_list.o nftl_gcc53_out_phy_block_from_free_list_by_block.o nftl_gcc53_get_cold_logic_page.o nftl_gcc53_fill_no_use_data_to_current_block.o nftl_gcc53_set_oob_logic_page.o nftl_gcc53_hynix26nm_getdefaultparam.o nftl_gcc53_hynix16nm_set_readretry.o nftl_gcc53_sandisk_readretry_init.o)
nftl_gcc53_objects += $(addprefix $(obj)/,$(nftl_special_objects))
nftl_gcc53_objects += $(addprefix $(obj)/,nftl_gcc53_nand_dbg_phy_read.o nftl_gcc53_nand_dbg_zone_phy_read.o)
nftl_gcc53_objects += $(addprefix $(obj)/,$(nftl_hynix_special_objects))
nftl_gcc53_objects += $(addprefix $(obj)/,$(nftl_small_objects))
nftl_gcc53_objects += $(addprefix $(obj)/,$(nftl_hynix_end_objects))
nftl_gcc53_objects += $(obj)/nftl_gcc53_read_reclaim.o
nftl_gcc53_objects += $(addprefix $(obj)/,nftl_gcc53__nftl_nand_read_logic_page.o nftl_gcc53_recover_logic_page_mapping.o)
nftl_gcc53_objects += $(addprefix $(obj)/,nftl_gcc53_adjust_free_list.o nftl_gcc53_erase_block.o)
nftl_gcc53_objects += $(obj)/nftl_gcc53_flush_write_cache_to_nand.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_write_phy_page_map_to_current_block.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix16nm_write_page_FF.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix20nm_vender_get_param_otp_hynix.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_recover_phy_page_mapping.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_get_spare_data.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_crc16_pair.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_panic_flags.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_check_bit_nums.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_min_invalid_erase.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_list_nine.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_hynix_lsb_init_pair.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_panic_write.o
nftl_gcc53_objects += $(obj)/nftl_gcc53_eval_order_three.o
$(nftl_gcc53_objects): override NOSTDINC_FLAGS := -nostdinc -isystem $(shell $(TRIMUI_NFTL_GCC53) -print-file-name=include)
$(nftl_gcc53_objects): $(srctree)/scripts/trimui_nftl_select_cc.py
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
CFLAGS_nftl_gcc53__nftl_nand_read_logic_page.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_recover_logic_page_mapping.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_adjust_free_list.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_erase_block.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_flush_write_cache_to_nand.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_write_phy_page_map_to_current_block.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix16nm_write_page_FF.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix20nm_vender_get_param_otp_hynix.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_recover_phy_page_mapping.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_get_spare_data.o += -O0 -fno-builtin -fno-pic -mcmodel=small -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_crc16_pair.o += -O0 -fno-builtin -fno-pic -mcmodel=small
CFLAGS_nftl_gcc53_panic_flags.o += -O0 -fno-builtin -fno-pic -mcmodel=small
CFLAGS_nftl_gcc53_check_bit_nums.o += -O0 -fno-builtin -fno-pic -mcmodel=small
CFLAGS_nftl_gcc53_min_invalid_erase.o += -O0 -fno-builtin -fno-pic -mcmodel=small -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_list_nine.o += -O0 -fno-builtin -fno-pic -mcmodel=small -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_hynix_lsb_init_pair.o += -O0 -fno-builtin -fno-pic -mcmodel=small -ffunction-sections -fdata-sections
CFLAGS_nftl_gcc53_panic_write.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_COPY_PAGE_GCC53_BUILD -DTRIMUI_NFTL_OOB_LOGIC_GCC53_BUILD -DTRIMUI_NFTL_PANIC_WRITE_GCC53_BUILD
CFLAGS_nftl_gcc53_eval_order_three.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections -DTRIMUI_NFTL_COPY_PAGE_GCC53_BUILD -DTRIMUI_NFTL_EVAL_ORDER_GCC53_BUILD
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
