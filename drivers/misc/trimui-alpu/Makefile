obj-y += arelink-core-ordered.o arelink-i2c.o trimui-alpu.o
# The recovered C bodies are complete but source order differs from the OEM
# arelink text order. Keep each body separate until this verified partial link.
CFLAGS_arelink-core.o += -ffunction-sections
quiet_cmd_arelink_order = LD      $@
      cmd_arelink_order = python3 $(srctree)/scripts/trimui_arelink_order_text.py $(srctree)/drivers/misc/trimui-alpu/arelink-stock-text-order.txt $(obj)/arelink-core.o $(obj)/.arelink-core-ordered.lds && $(LD) -r -T $(obj)/.arelink-core-ordered.lds -o $@ $(obj)/arelink-core.o
$(obj)/arelink-core-ordered.o: $(obj)/arelink-core.o $(srctree)/drivers/misc/trimui-alpu/arelink-stock-text-order.txt $(srctree)/scripts/trimui_arelink_order_text.py FORCE
	$(call if_changed,arelink_order)
