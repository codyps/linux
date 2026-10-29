/*
 * Based on the TPS65218 driver and the previous TPS65912 driver
 */

#include <linux/acpi.h>
#include <linux/err.h>
#include <linux/i2c.h>
#include <linux/module.h>
#include <linux/mfd/tcs4838.h>
#include <linux/of.h>
#include <linux/regmap.h>
#include <linux/slab.h>

/* GCC 7 emits these local tables in reverse declaration order. Keep this
 * order to match the Brick's regmap, volatile, range, ID and OF table layout.
 */
static const struct of_device_id tcs4838_i2c_of_match_table[] = {
	{ .compatible = "ti,tcs4838" },
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(of, tcs4838_i2c_of_match_table);

static const struct i2c_device_id tcs4838_i2c_id_table[] = {
	{ /* sentinel */ }
};
MODULE_DEVICE_TABLE(i2c, tcs4838_i2c_id_table);

static const struct regmap_range tcs4838_yes_ranges[] = {
	regmap_reg_range(TCS4838_VSEL0, TCS4838_PGOOD),
};
static const struct regmap_access_table tcs4838_volatile_table = {
	.yes_ranges = tcs4838_yes_ranges,
	.n_yes_ranges = ARRAY_SIZE(tcs4838_yes_ranges),
};
const struct regmap_config tcs4838_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.volatile_table = &tcs4838_volatile_table,
	.max_register = TCS4838_PGOOD,
	.cache_type = REGCACHE_NONE,
};
EXPORT_SYMBOL_GPL(tcs4838_regmap_config);

static int tcs4838_i2c_probe(struct i2c_client *client,
			      const struct i2c_device_id *ids)
{
	struct tcs4838 *ext;

	ext = devm_kzalloc(&client->dev, sizeof(*ext), GFP_KERNEL);
	if (!ext)
		return -ENOMEM;

	ext->dev = &client->dev;
	dev_set_drvdata(ext->dev, ext);

	ext->regmap = devm_regmap_init_i2c(client, &tcs4838_regmap_config);
	if (IS_ERR(ext->regmap)) {
		dev_err(ext->dev, "Failed to initialize register map\n");
		return PTR_ERR(ext->regmap);
	}

	return tcs4838_device_init(ext);
}

static int tcs4838_i2c_remove(struct i2c_client *client)
{
	struct tcs4838 *ext = i2c_get_clientdata(client);

	return tcs4838_device_exit(ext);
}

static const struct acpi_device_id tcs4838_i2c_acpi_match[] = {
	{ },
};
MODULE_DEVICE_TABLE(acpi, tcs4838_i2c_acpi_match);

static struct i2c_driver tcs4838_i2c_driver = {
	.driver		= {
		.name	= "tcs4838-i2c",
		.of_match_table = of_match_ptr(tcs4838_i2c_of_match_table),
		.acpi_match_table = ACPI_PTR(tcs4838_i2c_acpi_match),
	},
	.probe		= tcs4838_i2c_probe,
	.remove		= tcs4838_i2c_remove,
	.id_table       = tcs4838_i2c_id_table,
};

static int __init tcs4838_i2c_init(void)
{
	int ret;

	ret = i2c_add_driver(&tcs4838_i2c_driver);
	if (ret != 0) {
		pr_err("tcs4838 i2c registration failed %d\n", ret);
		return ret;
	}

	return 0;
}
subsys_initcall(tcs4838_i2c_init);

static void __exit tcs4838_i2c_exit(void)
{
	i2c_del_driver(&tcs4838_i2c_driver);
}
module_exit(tcs4838_i2c_exit);

MODULE_AUTHOR("Andrew F. Davis <afd@ti.com>");
MODULE_DESCRIPTION("tcs4838 I2C Interface Driver");
MODULE_LICENSE("GPL v2");
