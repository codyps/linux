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
endif
CFLAGS_nftl_recovered.o += -O0 -fno-builtin -fno-pic -mcmodel=small
# The stock NFTL object identifies Linaro GCC 5.3.1 (2016.05). These
# recovered bodies need its field-address code generation; the rest of NFTL
# remains on the established compiler to retain previously matched bodies.
TRIMUI_NFTL_GCC53 ?= $(srctree)/../../tools/gcc-linaro-5.3.1-2016.05-x86_64_aarch64-linux-gnu/bin/aarch64-linux-gnu-gcc
$(obj)/nftl_cache_init_gcc53.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nftl_cache_init --
$(obj)/nftl_sector_cache_init_gcc53.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) nftl_sector_cache_init --
$(obj)/nftl_invalid_page_incr_gcc53.o: override CC := python3 $(srctree)/scripts/trimui_nftl_select_cc.py $(TRIMUI_NFTL_GCC53) $(CROSS_COMPILE)ld $(OBJCOPY) phy_block_from_invalid_page_incr --
$(obj)/nftl_cache_init_gcc53.o $(obj)/nftl_sector_cache_init_gcc53.o $(obj)/nftl_invalid_page_incr_gcc53.o: override NOSTDINC_FLAGS := -nostdinc -isystem $(shell $(TRIMUI_NFTL_GCC53) -print-file-name=include)
$(obj)/nftl_cache_init_gcc53.o $(obj)/nftl_sector_cache_init_gcc53.o $(obj)/nftl_invalid_page_incr_gcc53.o: $(srctree)/scripts/trimui_nftl_select_cc.py
CFLAGS_nftl_cache_init_gcc53.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_sector_cache_init_gcc53.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
CFLAGS_nftl_invalid_page_incr_gcc53.o += -O0 -fno-builtin -fno-pic -mcmodel=small -mno-omit-leaf-frame-pointer -ffunction-sections -fdata-sections
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
