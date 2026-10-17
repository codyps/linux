// SPDX-License-Identifier: GPL-2.0
/* Reconstructed from TG3040 #860 and the public Allwinner pmu-ext driver. */
#include <linux/mfd/core.h>
#include <linux/mfd/tcs4838.h>
#include <linux/module.h>
#include <linux/of.h>

static struct mfd_cell tcs4838_cells[] = {
	{ .name = "tcs4838-regulator", },
};

int tcs4838_device_init(struct tcs4838 *tcs)
{
	struct device_node *node = tcs->dev->of_node;
	struct regmap *map = tcs->regmap;
	u32 val;

	if (of_property_read_u32(node, "tcs4838_delay", &val))
		val = 0;
	if (val) {
		val <<= 4;
		regmap_update_bits(map, TCS4838_CTRL, GENMASK(6, 4), val);
	}
	/* The shipped implementation returns zero even if child creation fails. */
	mfd_add_devices(tcs->dev, 0, tcs4838_cells,
			ARRAY_SIZE(tcs4838_cells), NULL, 0, NULL);
	return 0;
}
EXPORT_SYMBOL_GPL(tcs4838_device_init);

int tcs4838_device_exit(struct tcs4838 *tcs)
{
	mfd_remove_devices(tcs->dev);
	return 0;
}
EXPORT_SYMBOL_GPL(tcs4838_device_exit);
MODULE_LICENSE("GPL v2");
