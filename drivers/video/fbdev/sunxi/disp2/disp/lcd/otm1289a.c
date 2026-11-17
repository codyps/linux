/* SPDX-License-Identifier: GPL-2.0-only */
/* Brick #860 reconstruction. Tables and call sequences recovered from stock Image.
 * Gamma/cmap helper reused from the Allwinner lq101r1sx03 driver.
 */
#include <linux/init.h>
#include <linux/string.h>
#include "panels.h"
#include "trimui-panel-tables.h"

static char lcd_model_name[64] = "otm1289a";
/* Stock keeps the command arrays writable and caches the first selection.
 * Prefix matches (including suffixed model names) and ignored I/O failures
 * are intentional here; corrective validation belongs in a separate change.
 */
static struct trimui_lcm_setting *active_table;

static int __init lcd_model_setup(char *value)
{
	if (value) {
		/* Stock copies the entire argument, including an oversized one.
		 * TODO: bound this copy in a separate fix; the reconstruction must
		 * retain the shipped overflow and NULL-argument logging behavior.
		 */
		strcpy(lcd_model_name, value);
		printk("get lcd model:%s\n", lcd_model_name);
	}
	return 1;
}
__setup("lcd=", lcd_model_setup);

static void lcd_power_on(u32 sel)
{
	sunxi_lcd_power_enable(sel, 0);
	sunxi_lcd_delay_ms(10);
	sunxi_lcd_power_enable(sel, 1);
	sunxi_lcd_delay_ms(10);
	sunxi_lcd_pin_cfg(sel, 1);
	sunxi_lcd_delay_ms(50);
	sunxi_lcd_gpio_set_value(sel, 0, 1);
	sunxi_lcd_delay_ms(20);
	sunxi_lcd_gpio_set_value(sel, 0, 0);
	sunxi_lcd_delay_ms(120);
	sunxi_lcd_gpio_set_value(sel, 0, 1);
	sunxi_lcd_delay_ms(20);
}

static void lcd_power_off(u32 sel)
{
	sunxi_lcd_pin_cfg(sel, 0);
	sunxi_lcd_delay_ms(20);
	sunxi_lcd_gpio_set_value(sel, 0, 0);
	sunxi_lcd_delay_ms(5);
	sunxi_lcd_power_disable(sel, 1);
	sunxi_lcd_delay_ms(5);
	sunxi_lcd_power_disable(sel, 0);
}

static void lcd_bl_open(u32 sel)
{
	sunxi_lcd_pwm_enable(sel);
	sunxi_lcd_backlight_enable(sel);
}

static void lcd_bl_close(u32 sel)
{
	sunxi_lcd_backlight_disable(sel);
	sunxi_lcd_pwm_disable(sel);
}

static void lcd_panel_init(u32 sel)
{
	int i;

	printk("jd9168:lcd panel init\n");
	sunxi_lcd_dsi_clk_enable(sel);
	if (!active_table) {
		printk("check_lcd_module[%s]\n", lcd_model_name);
		if (!strncmp(lcd_model_name, "gc9702c", 7)) {
			active_table = table_gc9702c;
			printk("LCD gc9702c\n");
		} else if (!strncmp(lcd_model_name, "otm1289a", 8)) {
			active_table = table_otm1289a;
			printk("LCD otm1289a\n");
		} else if (!strncmp(lcd_model_name, "er68576", 7)) {
			active_table = table_er68576;
			printk("LCD er68576\n");
		} else if (!strncmp(lcd_model_name, "st7703", 6)) {
			active_table = table_st7703;
			printk("LCD st7703\n");
		} else if (!strncmp(lcd_model_name, "jd9168", 6)) {
			active_table = table_jd9168;
			printk("LCD jd9168\n");
		} else {
			/* Stock U-Boot can pass lcd=nv3052, but this callback has no
			 * nv3052 table. It falls through to JD9168. Keep the shipped
			 * behavior; a panel-specific fix needs separate validation.
			 */
			active_table = table_jd9168;
			printk("LCD default\n");
		}
	}
	for (i = 0; ; i++) {
		if (active_table[i].count == 0xff)
			break;
		if (active_table[i].count == 0xfe)
			sunxi_lcd_delay_ms(active_table[i].data[0]);
		else
			/* Preserve vendor gc9702c counts of 120: these read the
			 * following entry too, within the recovered array storage.
			 */
			sunxi_lcd_dsi_dcs_write(sel, active_table[i].command,
				(u8 *)active_table[i].data, active_table[i].count);
	}
}

static void lcd_panel_exit(u32 sel)
{
	sunxi_lcd_dsi_dcs_write_0para(sel, 0x28);
	sunxi_lcd_delay_ms(25);
	sunxi_lcd_dsi_dcs_write_0para(sel, 0x10);
	sunxi_lcd_delay_ms(150);
}

static s32 lcd_open_flow(u32 sel)
{
	LCD_OPEN_FUNC(sel, lcd_power_on, 10);
	LCD_OPEN_FUNC(sel, lcd_panel_init, 60);
	LCD_OPEN_FUNC(sel, sunxi_lcd_tcon_enable, 50);
	LCD_OPEN_FUNC(sel, lcd_bl_open, 0);
	return 0;
}

static s32 lcd_close_flow(u32 sel)
{
	LCD_CLOSE_FUNC(sel, lcd_bl_close, 0);
	LCD_CLOSE_FUNC(sel, sunxi_lcd_tcon_disable, 0);
	LCD_CLOSE_FUNC(sel, lcd_panel_exit, 200);
	LCD_CLOSE_FUNC(sel, lcd_power_off, 500);
	return 0;
}

static s32 lcd_user_defined_func(u32 sel, u32 para1, u32 para2, u32 para3)
{
	return 0;
}

static void lcd_cfg_panel_info(struct panel_extend_para *info)
{
	u32 i = 0, j = 0;
	u32 items;
	u8 lcd_gamma_tbl[][2] = {
		/* {input value, corrected value} */
		{0, 0},
		{15, 15},
		{30, 30},
		{45, 45},
		{60, 60},
		{75, 75},
		{90, 90},
		{105, 105},
		{120, 120},
		{135, 135},
		{150, 150},
		{165, 165},
		{180, 180},
		{195, 195},
		{210, 210},
		{225, 225},
		{240, 240},
		{255, 255},
	};

	u32 lcd_cmap_tbl[2][3][4] = {
		{
			{LCD_CMAP_G0, LCD_CMAP_B1, LCD_CMAP_G2, LCD_CMAP_B3},
			{LCD_CMAP_B0, LCD_CMAP_R1, LCD_CMAP_B2, LCD_CMAP_R3},
			{LCD_CMAP_R0, LCD_CMAP_G1, LCD_CMAP_R2, LCD_CMAP_G3},
		},
		{
			{LCD_CMAP_B3, LCD_CMAP_G2, LCD_CMAP_B1, LCD_CMAP_G0},
			{LCD_CMAP_R3, LCD_CMAP_B2, LCD_CMAP_R1, LCD_CMAP_B0},
			{LCD_CMAP_G3, LCD_CMAP_R2, LCD_CMAP_G1, LCD_CMAP_R0},
		},
	};

	items = sizeof(lcd_gamma_tbl) / 2;
	for (i = 0; i < items - 1; i++) {
		u32 num = lcd_gamma_tbl[i + 1][0] - lcd_gamma_tbl[i][0];

		for (j = 0; j < num; j++) {
			u32 value = 0;

			value = lcd_gamma_tbl[i][1] +
			    ((lcd_gamma_tbl[i + 1][1] - lcd_gamma_tbl[i][1]) * j) / num;
			info->lcd_gamma_tbl[lcd_gamma_tbl[i][0] + j] =
			    (value << 16) + (value << 8) + value;
		}
	}
	info->lcd_gamma_tbl[255] =
	    (lcd_gamma_tbl[items - 1][1] << 16) +
	    (lcd_gamma_tbl[items - 1][1] << 8) + lcd_gamma_tbl[items - 1][1];

	memcpy(info->lcd_cmap_tbl, lcd_cmap_tbl, sizeof(lcd_cmap_tbl));

}


struct __lcd_panel otm1289a_panel = {
	.name = "otm1289a",
	.func = {
		.cfg_panel_info = lcd_cfg_panel_info,
		.cfg_open_flow = lcd_open_flow,
		.cfg_close_flow = lcd_close_flow,
		.lcd_user_defined_func = lcd_user_defined_func,
	},
};
